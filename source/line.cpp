#include <algorithm>

#include "definitions.hpp"
#include "system/types.hpp"

#include "system/menu.hpp"
#include "system/variables.hpp"

#include "system/draw/draw.hpp"

#include "system/util/converter.hpp"
#include "system/util/curl.hpp"
#include "system/util/decoder.hpp"
#include "system/util/error.hpp"
#include "system/util/explorer.hpp"
#include "system/util/file.hpp"
#include "system/util/hid.hpp"
#include "system/util/json.hpp"
#include "system/util/log.hpp"
#include "system/util/queue.hpp"
#include "system/util/swkbd.hpp"
#include "system/util/util.hpp"

extern "C"
{
	#include "system/util/crypt.h"
}

#include "camera_app.hpp"
#include "image_viewer.hpp"
#include "music_player.hpp"
#include "video_player.hpp"

//Include myself.
#include "line.hpp"


#define DEF_LINE_QUEUE_ADD_WITH_LOG(queue, event_id, data, timeout_us, option) \
{ \
	char log_buffer[256]; \
	Result_with_string queue_result = Util_queue_add(queue, event_id, data, timeout_us, option); \
	snprintf(log_buffer, sizeof(log_buffer), "Util_queue_add() %s...%s%s", #event_id, queue_result.string.c_str(), queue_result.error_description.c_str()); \
	Util_log_save(__func__, log_buffer, queue_result.code); \
}

#define DEF_LINE_SECRET_HASH_SIZE			32	//Password hash size.
#define DEF_LINE_MAX_ROOMS					128	//Maximum room that can be registered.
#define DEF_LINE_MSG_TEXT_OFFSET			15	//Fixed offset added after text message.
#define DEF_LINE_MSG_ROOM_ID_OFFSET			15	//Fixed offset added after room ID message.
#define DEF_LINE_MSG_STICKER_OFFSET			70	//Fixed offset added after sticker message.
#define DEF_LINE_MSG_IMAGE_OFFSET			15	//Fixed offset added after image.
#define DEF_LINE_MSG_AUDIO_OFFSET			15	//Fixed offset added after audio.
#define DEF_LINE_MSG_VIDEO_OFFSET			15	//Fixed offset added after video.
#define DEF_LINE_MSG_FILE_OFFSET			15	//Fixed offset added after file.
#define DEF_LINE_MSG_SPEAKER_OFFSET			10	//Fixed offset added after speaker name.
#define DEF_LINE_MSG_TIME_OFFSET			20	//Fixed offset added after time.


enum Line_message_type
{
	LINE_MSG_TYPE_INVALID = -1,

	LINE_MSG_TYPE_TEXT,		//Text message.
	LINE_MSG_TYPE_ROOM_ID,	//Room ID.
	LINE_MSG_TYPE_STICKER,	//Sticker ID.
	LINE_MSG_TYPE_IMAGE,	//Image URL.
	LINE_MSG_TYPE_AUDIO,	//Audio URL.
	LINE_MSG_TYPE_VIDEO,	//Video URL.
	LINE_MSG_TYPE_FILE,		//File URL.
	LINE_MSG_TYPE_SPEAKER,	//Speaker name.
	LINE_MSG_TYPE_TIME,		//Time text.

	LINE_MSG_TYPE_MAX,
};

enum Line_tab_mode
{
	LINE_TAB_MODE_INVALID = -1,

	LINE_TAB_MODE_SEND,		//Send tab.
	LINE_TAB_MODE_RECEIVE,	//Receive tab.
	LINE_TAB_MODE_COPY,		//Copy tab.
	LINE_TAB_MODE_SEARCH,	//Search tab.
	LINE_TAB_MODE_SETTINGS,	//Line settings tab.
	LINE_TAB_MODE_ADVANCED,	//Advanced tab.
	LINE_TAB_MODE_STICKER,	//Sticker tab.

	LINE_TAB_MODE_MAX,

	LINE_TAB_MODE_FORCE_32BIT = INT32_MAX,
};

enum Line_command
{
	LINE_COMMAND_INVALID = -1,

	LINE_COMMAND_MAIN_THREAD_TYPE_GAS_URL_REQUEST,			//Request to edit the Google Apps Scripts URL via swkbd.
	LINE_COMMAND_MAIN_THREAD_SCAN_GAS_URL_REQUEST,			//Request to edit the Google Apps Scripts URL via camera.
	LINE_COMMAND_MAIN_THREAD_EDIT_APP_PASSWORD_REQUEST,		//Request to edit the application password.
	LINE_COMMAND_MAIN_THREAD_ADD_ROOM_ID_REQUEST,			//Request to add a new room ID.
	LINE_COMMAND_MAIN_THREAD_INPUT_TEXT_REQUEST,			//Request to input a text message.
	LINE_COMMAND_MAIN_THREAD_SEARCH_TEXT_REQUEST,			//Request to search text messages.
	LINE_COMMAND_MAIN_THREAD_DISPLAY_IMAGE_REQUEST,			//Request to switch to image viewer and display the image.
	LINE_COMMAND_MAIN_THREAD_PLAY_AUDIO_REQUEST,			//Request to switch to audio player and play the audio.
	LINE_COMMAND_MAIN_THREAD_PLAY_VIDEO_REQUEST,			//Request to switch to video player and play the video.

	LINE_COMMAND_WORKER_THREAD_GET_SCRIPT_URL_REQUEST,		//Request to fetch the Google Apps Script URL.
	LINE_COMMAND_WORKER_THREAD_LOAD_ROOM_REQUEST,			//Request to load room informations.
	LINE_COMMAND_WORKER_THREAD_LOAD_ICON_REQUEST,			//Request to load icons.
	LINE_COMMAND_WORKER_THREAD_DELETE_ROOM_REQUEST,			//Request to delete the room.
	LINE_COMMAND_WORKER_THREAD_DOWNLOAD_CONTENT_REQUEST,	//Request to downalod user content.

	LINE_COMMAND_LOG_THREAD_DOWNLOAD_LOG_REQUEST,			//Request to download logs from Google Apps Script.
	LINE_COMMAND_LOG_THREAD_DOWNLOAD_LOG_NO_PARSE_REQUEST,	//Request to download logs from Google Apps Script but don't parse them.
	LINE_COMMAND_LOG_THREAD_LOAD_LOG_REQUEST,				//Request to load logs from SD card.
	LINE_COMMAND_LOG_THREAD_PARSE_LOG_REQUEST,				//Request to parse logs.
	LINE_COMMAND_LOG_THREAD_SEND_MESSAGE_REQUEST,			//Request to send the message.
	LINE_COMMAND_LOG_THREAD_SEND_STICKER_REQUEST,			//Request to send the sticker.
	LINE_COMMAND_LOG_THREAD_SEND_FILE_REQUEST,				//Request to send the file.

	LINE_COMMAND_MAX,
	LINE_COMMAND_FORCE_32BIT = INT32_MAX,
};

enum Line_main_state
{
	LINE_STATE_MAIN_INVALID = -1,

	LINE_STATE_MAIN_CHAT_ROOM_SELECTION,	//In chat room selection menu.
	LINE_STATE_MAIN_CHAT_ROOM,				//In chat room.

	LINE_STATE_MAIN_MAX,
};

typedef uint32_t Line_sub_state;
#define DEF_LINE_STATE_SUB_NONE									(Line_sub_state)(0 << 0)	//Nothing special.
#define DEF_LINE_STATE_SUB_SCROLL_MODE							(Line_sub_state)(1 << 0)	//Scroll mode is active.
#define DEF_LINE_STATE_SUB_WAIT_SWKBD_EXIT						(Line_sub_state)(1 << 1)	//Waiting for swkbd to exit.
#define DEF_LINE_STATE_SUB_GET_SCRIPT_URL						(Line_sub_state)(1 << 2)	//Waiting for swkbd to exit.
#define DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_MESSAGE		(Line_sub_state)(1 << 3)	//Waiting for user's approvement to send the text message.
#define DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_STICKER		(Line_sub_state)(1 << 4)	//Waiting for user's approvement to send the sticker.
#define DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_FILE			(Line_sub_state)(1 << 5)	//Waiting for user's approvement to send the file.
#define DEF_LINE_STATE_SUB_SEND_TEXT							(Line_sub_state)(1 << 6)	//Sending the text message.
#define DEF_LINE_STATE_SUB_SEND_STICKER							(Line_sub_state)(1 << 7)	//Sending the sticker.
#define DEF_LINE_STATE_SUB_SEND_FILE							(Line_sub_state)(1 << 8)	//Sending the file.
#define DEF_LINE_STATE_SUB_LOAD_LOGS							(Line_sub_state)(1 << 9)	//Downloading or loading the logs.
#define DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_DELETE_ROOM			(Line_sub_state)(1 << 10)	//Waiting for user's approvement to delete the room.
#define DEF_LINE_STATE_SUB_DOWNLOAD_FILE						(Line_sub_state)(1 << 11)	//Downloading the file.
#define DEF_LINE_STATE_SUB_WAIT_SELECTION_GAS_URL				(Line_sub_state)(1 << 12)	//Waiting for user's choice to use swkbd or camera for GAS URL input.
#define DEF_LINE_STATE_SUB_WAIT_APPROVEMENT						(Line_sub_state)(DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_MESSAGE \
| DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_STICKER | DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_FILE | DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_DELETE_ROOM)	//Waiting for user's approvement.
#define DEF_LINE_STATE_SUB_SEND_MESSAGE							(Line_sub_state)(DEF_LINE_STATE_SUB_SEND_TEXT \
| DEF_LINE_STATE_SUB_SEND_STICKER | DEF_LINE_STATE_SUB_SEND_FILE)							//Sending the message.

typedef struct
{
	int log_id = 0;	//Log id that contains target information.
} Line_play_media_request_data;

typedef Line_play_media_request_data Line_download_file_request_data;
typedef Line_play_media_request_data Line_add_room_id_request_data;

typedef struct
{
	std::string url = "";	//Actual Google Apps Script URL.
} Line_parsed_script_url;

typedef struct
{
	char url[512];	//Google Apps Script URL, may be short URL.
} Line_get_script_url_request_data;

typedef struct
{
	uint16_t num_of_logs = 0;	//Available number of logs in this struct.
	uint32_t log_height = 0;	//Current log height.
	struct
	{
		std::string text[4000];		//Actual text data.
		std::string speaker[4000];	//Speaker name for text data.
		std::string time[4000];		//Time for text data.
	} logs;
	std::string room_name = "";		//Chat room name.
	std::string room_icon_url = "";	//Chat room icon URL.
} Line_parsed_logs;

typedef struct
{
	int num_of_lines = 0;				//Available number of lines in this struct.
	int32_t start_pos[4000];			//Start offset of original (Line_parsed_logs) index.
	uint16_t original_index[60000];		//Original (Line_parsed_logs) index of this data.
	uint32_t draw_offset[60000];		//Draw offset for this data.
	std::string data[60000] = { "", };	//Actual data.
	Line_message_type type[60000] = { LINE_MSG_TYPE_INVALID, };	//Data type.
} Line_optimized_logs;

typedef struct
{
	int log_height = 0;				//Current log height.
	int num_of_unread_logs = 0;		//Number of unread logs.
	std::string room_name = "";		//Chat room name.
	std::string room_icon_url = "";	//Chat room icon URL.
} Line_saved_room_data;

typedef struct
{
	bool is_icon_available = false;	//Whether icon is available for this chat room.
	int log_height = 0;				//Current log height.
	int num_of_unread_logs = 0;		//Number of unread logs.
	std::string id = "";			//Chat room ID.
	std::string room_name = "";		//Chat room name.
	std::string room_icon_url = "";	//Chat room icon URL.
} Line_room_data;

typedef struct
{
	int log_index = -1;	//Log index that contains target information.
	Image_data button;	//Button object.
} Line_content_button;

typedef struct
{
	double y = 0.0;		//Current y position.
	double min_y = 0.0;	//Minimum allowed y position.
	double max_y = 0.0;	//Maximum allowed y position.
} Line_y_pos;

typedef struct
{
	bool first = true;				//Whether this is the first request.
	bool end = false;				//Whether operation has been finished.
	int offset = 0;					//File offset.
	std::string filename = "";		//File name.
	std::string dir = "";			//Directory name.
	std::string first_data = "";	//Data that will be sent at the first time.
	std::string end_data = "";		//Data that will be sent at the end.
} Line_upload_file_info;

typedef struct
{
	//todo improve thread safety
	//todo pass parameters to queues instead of reading them from here directly
	//todo use #define instead of magic number
	//todo get rid of old code
	//todo get rid of C++, and many more...

	//Settings.
	int num_of_logs = 150;					//Maximum number of logs that can be saved.
	int selected_room_index = 0;			//Selected room index.
	double text_interval = 5;				//Additional text interval.
	double text_size = 0.66;				//Text message size.
	std::string gas_url = "";				//Google Apps Script URL.

	//Copy.
	int copy_selected_index = 0;			//Selected message index for copy.

	//Search.
	int search_selected_index = 0;			//Selected message index for search.
	int search_found_items = 0;				//Number of occurrence.
	int search_found_item_index[4000];		//Found text message index list.
	std::string search_text = "";			//Query text.

	//Upload.
	int total_data_size = 0;				//Total size to be uploaded.
	int uploaded_size = 0;					//Uploaded size.
	std::string file_dir = "";				//Directory name.
	std::string file_name = "";				//File name.

	//Main data.
	uint8_t secret_hash[DEF_LINE_SECRET_HASH_SIZE];	//Password hash.
	uint16_t num_of_room;					//Number of available (has room ID) room.
	std::string input_text = "";			//User's input text.
	Line_room_data room_data[DEF_LINE_MAX_ROOMS];	//Chat room data.
	Line_parsed_logs logs;					//Chat logs.
	Line_optimized_logs optimized_logs;		//Optimized chat logs.

	//State.
	Line_main_state main_state;				//Main state.
	Line_sub_state sub_state;				//Sub state.
	Line_tab_mode tab_mode;					//Tab mode in chat room.

	//Stickers.
	int selected_sticker_tab_index = 0;		//Selected sticker tab index.
	int selected_sticker_index = 0;			//Selected sticker index.
	int sticker_texture_index = -1;			//Sticker texture handle used for freeing texture.
	uint32_t sticker_list[121] =			//Sticker ID -> texture index translation table.
	{
		0,
		//Package id : 11537.
		52002734, 52002735, 52002736, 52002737, 52002738, 52002739, 52002740, 52002741, 52002742, 52002743,
		52002744, 52002745, 52002746, 52002747, 52002748, 52002749, 52002750, 52002751, 52002752, 52002753,
		52002754, 52002755, 52002756, 52002757, 52002758, 52002759, 52002760, 52002761, 52002762, 52002763,
		52002764, 52002765, 52002766, 52002767, 52002768, 52002769, 52002770, 52002771, 52002772, 52002773,
		//Package id : 11538.
		51626494, 51626495, 51626496, 51626497, 51626498, 51626499, 51626500, 51626501, 51626502, 51626503,
		51626504, 51626505, 51626506, 51626507, 51626508, 51626509, 51626510, 51626511, 51626512, 51626513,
		51626514, 51626515, 51626516, 51626517, 51626518, 51626519, 51626520, 51626521, 51626522, 51626523,
		51626524, 51626525, 51626526, 51626527, 51626528, 51626529, 51626530, 51626531, 51626532, 51626533,
		//Package id : 11539.
		52114110, 52114111, 52114112, 52114113, 52114114, 52114115, 52114116, 52114117, 52114118, 52114119,
		52114120, 52114121, 52114122, 52114123, 52114124, 52114125, 52114126, 52114127, 52114128, 52114129,
		52114130, 52114131, 52114132, 52114133, 52114134, 52114135, 52114136, 52114137, 52114138, 52114139,
		52114140, 52114141, 52114142, 52114143, 52114144, 52114145, 52114146, 52114147, 52114148, 52114149,
	};
	C2D_Image sticker_images[121];			//Sticker texture data.

	//Content.
	int dled_content_size = 0;				//Downloaded size.

	//UI.
	Line_y_pos y_pos[2];					//Y (vertical) position.
	Line_content_button content[32];		//Content (image, audio, video, etc...) buttons.
	double text_y_move_left = 0.0;			//Remaining y (vertical) direction movement for scroll.

	//Update button.
	int update_button_texture_index = -1;	//Update button texture handle used for freeing texture.
	Image_data room_update_button[128];		//Update button texture data.

	//General buttons.
	Image_data icon[128], room_button[128], menu_button[6], add_new_id_button, change_gas_url_button, change_app_password_button,
	back_button, send_msg_button, send_sticker_button, send_file_button, send_success_button, dl_logs_button, max_logs_bar,
	copy_button, copy_select_down_button, copy_select_up_button, increase_interval_button, decrease_interval_button, increase_size_button,
	decrease_size_button, search_button, search_select_down_button, search_select_up_button, delete_room_button, yes_button, no_button, view_image_button,
	edit_msg_button, use_swkbd_button, use_camera_button, scroll_bar;

	//Thread related data.
	Thread log_thread;						//Log thread handle.
	Queue log_thread_queue;					//Log thread command queue.
	Line_parsed_logs parsed_logs_buffer;	//Log thread working buffer.
	Line_parsed_logs old_logs_buffer;		//Log thread working buffer.

	Thread worker_thread;					//Worker thread handle.
	Queue worker_thread_queue;				//Worker thread command queue.

	Queue main_thread_queue;				//Main thread command queue.

	//Mutexs.
	LightLock log_copy_string_lock = 0;		//Mutex for copying log buffers.
	LightLock state_lock = 0;				//Mutex for state.
} Line;


static void Line_add_sub_state(Line* app, Line_sub_state state_to_add);
static void Line_remove_sub_state(Line* app, Line_sub_state state_to_remove);
static bool Line_has_sub_state(Line_sub_state sub_state, Line_sub_state state_to_check);
static bool Line_is_request_success(Json_data* json_data);
static std::string Line_get_error_message(Json_data* json_data);
static Result_with_string Line_parse_script_url(Json_data* json_data, Line_parsed_script_url* url_data);
static Result_with_string Line_parse_logs(Json_data* json_data, Line_parsed_logs* logs);
static Result_with_string Line_parse_room_data(Json_data* json_data, Line_saved_room_data* room_data);
static Result_with_string Line_load_logs(std::string log_file_name, char** log_data);
static Result_with_string Line_merge_logs(Line_parsed_logs* new_logs, std::string old_log_file_name, uint32_t max_logs, std::string* merged_data);
static Result_with_string Line_save_room_info(Line_parsed_logs* logs, std::string file_name, uint32_t unreads);
static Result_with_string Line_save_logs(Line_parsed_logs* logs, std::string file_name, uint32_t unreads, char* save_data);
static uint32_t Line_generate_password_hash(uint8_t* password, uint8_t password_length, uint8_t hash[32]);
static void Line_draw_message(int default_text_color, bool is_top_screen, Line_y_pos* y_pos);
static std::string Line_get_google_drive_file_id(std::string url);
static uint16_t Line_stickers_index_to_textures_index(uint32_t sticker_index);
static void Line_qr_scan_result_callback(char* decoded_data);


bool line_main_run = false;
bool line_thread_run = false;
bool line_thread_suspend = false;
bool line_already_init = false;
std::string line_status = "";
std::string line_msg[DEF_LINE_NUM_OF_MSG];
Thread line_init_thread, line_exit_thread;
Line line;

bool line_set_password_request = false;
bool line_type_password_request = false;


static void Line_add_sub_state(Line* app, Line_sub_state state_to_add)
{
	LightLock_Lock(&app->state_lock);
	app->sub_state = (app->sub_state | state_to_add);
	LightLock_Unlock(&app->state_lock);
}

static void Line_remove_sub_state(Line* app, Line_sub_state state_to_remove)
{
	LightLock_Lock(&app->state_lock);
	app->sub_state = (app->sub_state & ~state_to_remove);
	LightLock_Unlock(&app->state_lock);
}

static bool Line_has_sub_state(Line_sub_state sub_state, Line_sub_state state_to_check)
{
	return (sub_state & state_to_check);
}

static bool Line_is_request_success(Json_data* json_data)
{
	bool is_success = false;
	Json_extracted_data extracted_data;
	Result_with_string result = Util_json_get_data("is_success", json_data, &extracted_data);

	if(result.code == 0 && extracted_data.type == JSON_TYPE_BOOL)
		is_success = (*(bool*)extracted_data.value);

	free(extracted_data.value);
	extracted_data.value = NULL;

	return is_success;
}

static std::string Line_get_error_message(Json_data* json_data)
{
	std::string message = "";
	Json_extracted_data extracted_data;
	Result_with_string result = Util_json_get_data("msg", json_data, &extracted_data);

	if(result.code == 0 && extracted_data.type == JSON_TYPE_STRING)
		message = (char*)extracted_data.value;

	free(extracted_data.value);
	extracted_data.value = NULL;

	return message;
}

static Result_with_string Line_parse_script_url(Json_data* json_data, Line_parsed_script_url* url_data)
{
	char json_key[32] = { 0, };
	std::string temp = "";
	Json_extracted_data extracted_data;
	Result_with_string result;

	if(!Line_is_request_success(json_data))
	{
		result.code = DEF_ERR_GAS_RETURNED_NOT_SUCCESS;
		result.string = DEF_ERR_GAS_RETURNED_NOT_SUCCESS_STR;
		result.error_description = (std::string)"[Error] " + Line_get_error_message(json_data) + " ";
		goto fail;
	}

	//Get the data from json and fill the struct.
	snprintf(json_key, sizeof(json_key), "url");
	result = Util_json_get_data(json_key, json_data, &extracted_data);
	if(result.code != 0 || extracted_data.type != JSON_TYPE_STRING)
		goto fail;

	temp = (char*)extracted_data.value;
	if(temp.find("https://") == std::string::npos)
		goto fail;//URL must start with https://, value is corrupted.

	url_data->url = temp;

	free(extracted_data.value);
	extracted_data.value = NULL;
	return result;

	fail:
	if(result.code == 0)
	{
		//Util_json_get_data() will return success if key couldn't be found.
		//Or value type was an unexpected type.
		result.code = DEF_ERR_OTHER;
		result.string = DEF_ERR_OTHER_STR;
		result.error_description = "[Error] Couldn't extract the value or value is corrupted.\nKey:" + (std::string)json_key + " ";
	}
	free(extracted_data.value);
	extracted_data.value = NULL;
	return result;
}

static Result_with_string Line_parse_logs(Json_data* json_data, Line_parsed_logs* logs)
{
	char json_key[32] = { 0, };
	Json_extracted_data extracted_data;
	Result_with_string result;

	if(!Line_is_request_success(json_data))
	{
		result.code = DEF_ERR_GAS_RETURNED_NOT_SUCCESS;
		result.string = DEF_ERR_GAS_RETURNED_NOT_SUCCESS_STR;
		result.error_description = (std::string)"[Error] " + Line_get_error_message(json_data) + " ";
		goto fail;
	}

	//Get the data from json and fill the struct.
	snprintf(json_key, sizeof(json_key), "num_of_logs");
	result = Util_json_get_data(json_key, json_data, &extracted_data);
	if(result.code != 0 || extracted_data.type != JSON_TYPE_NUMBER)
		goto fail;

	logs->num_of_logs = (int)(*(double*)extracted_data.value);
	free(extracted_data.value);
	extracted_data.value = NULL;

	snprintf(json_key, sizeof(json_key), "log_height");
	result = Util_json_get_data(json_key, json_data, &extracted_data);
	if(result.code != 0 || extracted_data.type != JSON_TYPE_NUMBER)
		goto fail;

	logs->log_height = (int)(*(double*)extracted_data.value);
	free(extracted_data.value);
	extracted_data.value = NULL;

	snprintf(json_key, sizeof(json_key), "room_name");
	result = Util_json_get_data(json_key, json_data, &extracted_data);
	if(result.code != 0 || extracted_data.type != JSON_TYPE_STRING)
		goto fail;

	logs->room_name = (char*)extracted_data.value;
	free(extracted_data.value);
	extracted_data.value = NULL;

	snprintf(json_key, sizeof(json_key), "room_icon_url");
	result = Util_json_get_data(json_key, json_data, &extracted_data);
	if(result.code != 0 || extracted_data.type != JSON_TYPE_STRING)
		goto fail;

	logs->room_icon_url = (char*)extracted_data.value;
	free(extracted_data.value);
	extracted_data.value = NULL;

	for(int i = 0; i < logs->num_of_logs; i++)
	{
		snprintf(json_key, sizeof(json_key), "logs.text[%d]", i);
		result = Util_json_get_data(json_key, json_data, &extracted_data);
		if(result.code == 0 && extracted_data.type == JSON_TYPE_STRING)
			logs->logs.text[i] = (char*)extracted_data.value;

		free(extracted_data.value);
		extracted_data.value = NULL;

		snprintf(json_key, sizeof(json_key), "logs.speaker[%d]", i);
		result = Util_json_get_data(json_key, json_data, &extracted_data);
		if(result.code == 0 && extracted_data.type == JSON_TYPE_STRING)
			logs->logs.speaker[i] = (char*)extracted_data.value;

		free(extracted_data.value);
		extracted_data.value = NULL;

		snprintf(json_key, sizeof(json_key), "logs.time[%d]", i);
		result = Util_json_get_data(json_key, json_data, &extracted_data);
		if(result.code == 0 && extracted_data.type == JSON_TYPE_STRING)
			logs->logs.time[i] = (char*)extracted_data.value;

		free(extracted_data.value);
		extracted_data.value = NULL;
	}

	free(extracted_data.value);
	extracted_data.value = NULL;
	return result;

	fail:
	if(result.code == 0)
	{
		//Util_json_get_data() will return success if key couldn't be found.
		//Or value type was an unexpected type.
		result.code = DEF_ERR_OTHER;
		result.string = DEF_ERR_OTHER_STR;
		result.error_description = "[Error] Couldn't extract the value or value is corrupted.\nKey:" + (std::string)json_key + " ";
	}
	free(extracted_data.value);
	extracted_data.value = NULL;
	return result;
}

static Result_with_string Line_parse_room_data(Json_data* json_data, Line_saved_room_data* room_data)
{
	char json_key[32];
	Json_extracted_data extracted_data;
	Result_with_string result;

	//Get the data from json and fill the struct.
	snprintf(json_key, sizeof(json_key), "log_height");
	result = Util_json_get_data(json_key, json_data, &extracted_data);
	if(result.code != 0 || extracted_data.type != JSON_TYPE_NUMBER)
		goto fail;

	room_data->log_height = (int)(*(double*)extracted_data.value);
	free(extracted_data.value);
	extracted_data.value = NULL;

	snprintf(json_key, sizeof(json_key), "num_of_unread_logs");
	result = Util_json_get_data(json_key, json_data, &extracted_data);
	if(result.code != 0 || extracted_data.type != JSON_TYPE_NUMBER)
		goto fail;

	room_data->num_of_unread_logs = (int)(*(double*)extracted_data.value);
	free(extracted_data.value);
	extracted_data.value = NULL;

	snprintf(json_key, sizeof(json_key), "room_name");
	result = Util_json_get_data(json_key, json_data, &extracted_data);
	if(result.code != 0 || extracted_data.type != JSON_TYPE_STRING)
		goto fail;

	room_data->room_name = (char*)extracted_data.value;
	free(extracted_data.value);
	extracted_data.value = NULL;

	snprintf(json_key, sizeof(json_key), "room_icon_url");
	result = Util_json_get_data(json_key, json_data, &extracted_data);
	if(result.code != 0 || extracted_data.type != JSON_TYPE_STRING)
		goto fail;

	room_data->room_icon_url = (char*)extracted_data.value;
	free(extracted_data.value);
	extracted_data.value = NULL;

	return result;

	fail:
	if(result.code == 0)
	{
		//Util_json_get_data() will return success if key couldn't be found.
		//Or value type was an unexpected type.
		result.code = DEF_ERR_OTHER;
		result.string = DEF_ERR_OTHER_STR;
		result.error_description = "[Error] Couldn't extract the value or value is corrupted.\nKey:" + (std::string)json_key + " ";
	}
	free(extracted_data.value);
	extracted_data.value = NULL;
	return result;
}

static Result_with_string Line_load_logs(std::string log_file_name, char** log_data)
{
	uint8_t* buffer = NULL;
	uint32_t read_size = 0;
	Result_with_string result;

	//Load old logs.
	result = Util_file_load_from_file(log_file_name, DEF_MAIN_DIR + "chat_logs/", &buffer, 1024 * 1024 * 5, &read_size);
	if(result.code == DEF_SUCCESS)
	{
		uint8_t key[DEF_LINE_SECRET_HASH_SIZE] = { 0, };
		uint8_t iv[DEF_LINE_SECRET_HASH_SIZE] = { 0, };
		Util_crypt_decrypt_parameters parameters = { 0, };

		for(uint8_t i = 0; i < DEF_LINE_SECRET_HASH_SIZE; i++)
			key[i] = line.secret_hash[i];

		parameters.input = buffer;
		parameters.input_size = read_size;
		parameters.private_key = key;
		parameters.key_size = DEF_CRYPT_AES_256_KEY_SIZE;
		//We can throw first block away because it's a dummy data, so iv can be anything
		//(because incorrect iv only affects to the first block in CBC mode).
		parameters.initialization_vector = iv;
		parameters.iv_size = DEF_CRYPT_AES_IV_SIZE;
		parameters.decrypted = NULL;
		parameters.decrypted_size = 0;
		parameters.type = CRYPT_DECRYPTION_TYPE_AES_256_CBC;

		result.code = Util_crypt_decrypt(&parameters);
		if(result.code == DEF_SUCCESS)
		{
			uint32_t original_length = (parameters.decrypted_size - DEF_CRYPT_AES_BLOCK_SIZE);

			*log_data = (char*)malloc((original_length + 1));
			if(*log_data)
			{
				//Throw first block away.
				memcpy(*log_data, (parameters.decrypted + DEF_CRYPT_AES_BLOCK_SIZE), original_length);

				//Add NULL terminator.
				(*log_data)[original_length] = 0x00;
			}
			else
				result.code = DEF_ERR_OUT_OF_MEMORY;
		}
		else
			Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_crypt_decrypt()...", result.code);

		free(parameters.decrypted);
		parameters.decrypted = NULL;
	}
	else
		Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_file_load_from_file()..." + result.string + result.error_description, result.code);

	free(buffer);
	buffer = NULL;

	return result;
}

static Result_with_string Line_merge_logs(Line_parsed_logs* new_logs, std::string old_log_file_name, uint32_t max_logs, std::string* merged_data)
{
	char* buffer = NULL;
	Result_with_string result;

	//Load old logs.
	result = Line_load_logs(old_log_file_name, &buffer);
	if(result.code == DEF_SUCCESS)
	{
		Json_data json_data;

		result = Util_json_parse(buffer, &json_data);
		if(result.code == DEF_SUCCESS)
		{
			result = Line_parse_logs(&json_data, &line.old_logs_buffer);
			if(result.code == DEF_SUCCESS)
			{
				uint16_t old_log_offset = 0;
				uint16_t num_of_old_logs_to_merge = 0;
				uint16_t num_of_old_logs = line.old_logs_buffer.num_of_logs;
				uint16_t num_of_new_logs = new_logs->num_of_logs;
				uint16_t num_of_merged_logs = 0;
				uint16_t num_of_max_logs = line.num_of_logs;

				//We've successfully loaded old logs, so merge with new logs.
				num_of_merged_logs = Util_min(num_of_max_logs, (num_of_new_logs + num_of_old_logs));

				//Calculate old log offset.
				if(num_of_max_logs <= num_of_new_logs)
					old_log_offset = num_of_old_logs;//No spaces for old logs.
				else if(num_of_old_logs >= (num_of_max_logs - num_of_new_logs))
					old_log_offset = (num_of_old_logs - (num_of_max_logs - num_of_new_logs));
				else
					old_log_offset = 0;//All of old logs and new logs can be saved.

				num_of_old_logs_to_merge = (num_of_old_logs - old_log_offset);

				//Update parsed logs first.
				//Move new logs to backward so that old logs can be merged.
				for(int32_t old_index = (num_of_new_logs - 1); old_index >= 0; old_index--)
				{
					uint32_t new_index = (old_index + num_of_old_logs_to_merge);
					if(new_index >= 4000)
						continue;//We can't hold this log, throw it away.

					new_logs->logs.text[new_index] = new_logs->logs.text[old_index];
					new_logs->logs.speaker[new_index] = new_logs->logs.speaker[old_index];
					new_logs->logs.time[new_index] = new_logs->logs.time[old_index];
				}

				//Merge old logs.
				for(uint16_t i = 0; i < num_of_old_logs_to_merge; i++)
				{
					new_logs->logs.text[i] = line.old_logs_buffer.logs.text[old_log_offset + i];
					new_logs->logs.speaker[i] = line.old_logs_buffer.logs.speaker[old_log_offset + i];
					new_logs->logs.time[i] = line.old_logs_buffer.logs.time[old_log_offset + i];
				}
				new_logs->num_of_logs = num_of_merged_logs;

				//Generate JSON file for merged logs.
				*merged_data = DEF_JSON_START_OBJECT;
				*merged_data += DEF_JSON_NON_STRING_DATA_WITH_KEY("is_success", "true");
				*merged_data += DEF_JSON_STRING_DATA_WITH_KEY("msg", "");
				*merged_data += DEF_JSON_STRING_DATA_WITH_KEY("room_name", new_logs->room_name);
				*merged_data += DEF_JSON_STRING_DATA_WITH_KEY("room_icon_url", new_logs->room_icon_url);
				*merged_data += DEF_JSON_NON_STRING_DATA_WITH_KEY("log_height", std::to_string(new_logs->log_height));
				*merged_data += DEF_JSON_NON_STRING_DATA_WITH_KEY("num_of_logs", std::to_string(new_logs->num_of_logs));

				//Set up log text.
				*merged_data += DEF_JSON_KEY("logs") + DEF_JSON_START_OBJECT;
				*merged_data += DEF_JSON_KEY("text") + DEF_JSON_START_ARRAY;
				for(uint16_t i = 0; i < num_of_merged_logs; i++)
				{
					bool is_last = ((i + 1) == num_of_merged_logs);
					//Old one first, new one last.
					*merged_data += DEF_JSON_STRING_DATA(new_logs->logs.text[i]) + (is_last ? "" : DEF_JSON_COMMA);
				}
				*merged_data += DEF_JSON_END_ARRAY + DEF_JSON_COMMA;

				//Set up speaker text.
				*merged_data += DEF_JSON_KEY("speaker") + DEF_JSON_START_ARRAY;
				for(uint16_t i = 0; i < num_of_merged_logs; i++)
				{
					bool is_last = ((i + 1) == num_of_merged_logs);
					//Old one first, new one last.
					*merged_data += DEF_JSON_STRING_DATA(new_logs->logs.speaker[i]) + (is_last ? "" : DEF_JSON_COMMA);
				}
				*merged_data += DEF_JSON_END_ARRAY + DEF_JSON_COMMA;

				//Set up time text.
				*merged_data += DEF_JSON_KEY("time") + DEF_JSON_START_ARRAY;
				for(uint16_t i = 0; i < num_of_merged_logs; i++)
				{
					bool is_last = ((i + 1) == num_of_merged_logs);
					//Old one first, new one last.
					*merged_data += DEF_JSON_STRING_DATA(new_logs->logs.time[i]) + (is_last ? "" : DEF_JSON_COMMA);
				}
				*merged_data += DEF_JSON_END_ARRAY + DEF_JSON_END_OBJECT + DEF_JSON_END_OBJECT;
			}
			else
				Util_log_save(DEF_LINE_LOG_THREAD_STR, "Line_parse_logs()..." + result.string + result.error_description, result.code);
		}
		else
			Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_json_parse()..." + result.string + result.error_description, result.code);

		Util_json_free(&json_data);
	}
	else
		Util_log_save(DEF_LINE_LOG_THREAD_STR, "Line_load_logs()..." + result.string + result.error_description, result.code);

	free(buffer);
	buffer = NULL;
	return result;
}

static Result_with_string Line_save_room_info(Line_parsed_logs* logs, std::string file_name, uint32_t unreads)
{
	std::string room_info = "";
	Result_with_string result;

	room_info = DEF_JSON_START_OBJECT;
	room_info += DEF_JSON_NON_STRING_DATA_WITH_KEY("log_height", std::to_string(logs->log_height));
	room_info += DEF_JSON_NON_STRING_DATA_WITH_KEY("num_of_unread_logs", std::to_string(unreads));
	room_info += DEF_JSON_STRING_DATA_WITH_KEY("room_name", logs->room_name);
	room_info += DEF_JSON_STRING_DATA_WITH_KEY_WITHOUT_COMMA("room_icon_url", logs->room_icon_url);
	room_info += DEF_JSON_END_OBJECT;

	//Also save lite version that doesn't contain actual logs (this will be used in Line_read_id()).
	//Since there is no logs (just room name and icon URL), this file will be saved without encryption.
	result = Util_file_save_to_file(file_name, DEF_MAIN_DIR + "to/", (u8*)room_info.c_str(), room_info.length(), true);
	if(result.code != DEF_SUCCESS)
		Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_file_save_to_file()..." + result.string + result.error_description, result.code);

	return result;
}

static Result_with_string Line_save_logs(Line_parsed_logs* logs, std::string file_name, uint32_t unreads, char* save_data)
{
	uint8_t* padded_plain_data = NULL;
	uint32_t plain_data_size = 0;
	uint32_t padded_plain_data_size = 0;
	Result_with_string result;

	plain_data_size = strlen(save_data);
	//Add 16 bytes to make first block dummy, so that
	//we don't need to save initialization vector.
	padded_plain_data_size = plain_data_size + DEF_CRYPT_AES_BLOCK_SIZE;

	//Check if we need to add padding.
	if((plain_data_size % DEF_CRYPT_AES_BLOCK_SIZE) != 0)
		padded_plain_data_size += (DEF_CRYPT_AES_BLOCK_SIZE - (plain_data_size % DEF_CRYPT_AES_BLOCK_SIZE));

	padded_plain_data = (uint8_t*)malloc(padded_plain_data_size);
	if(padded_plain_data)
	{
		uint8_t key[DEF_LINE_SECRET_HASH_SIZE] = { 0, };
		uint8_t iv_source[DEF_LINE_SECRET_HASH_SIZE] = { 0, };
		uint64_t timestamp = osGetTime();
		Util_crypt_hash_parameters hash_parameters = { 0, };

		for(uint8_t i = 0; i < DEF_LINE_SECRET_HASH_SIZE; i++)
		{
			key[i] = line.secret_hash[i];
			iv_source[i] = (uint8_t)(line.secret_hash[i] * timestamp);
		}

		//Use SHA256ed (password hash * timestamp) as a initialization vector.
		hash_parameters.input = iv_source;
		hash_parameters.input_size = DEF_LINE_SECRET_HASH_SIZE;
		hash_parameters.hash = NULL;
		hash_parameters.hash_size = 0;
		hash_parameters.type = CRYPT_HASH_TYPE_SHA_256;

		result.code = Util_crypt_hash(&hash_parameters);
		if(result.code == DEF_SUCCESS)
		{
			Util_crypt_encrypt_parameters enc_parameters = { 0, };

			//Don't care about first block.
			//Copy plain data and zero padded data if needed.
			memcpy((padded_plain_data + DEF_CRYPT_AES_BLOCK_SIZE), save_data, plain_data_size);
			memset((padded_plain_data + DEF_CRYPT_AES_BLOCK_SIZE + plain_data_size), 0x00, ((padded_plain_data_size - plain_data_size) - DEF_CRYPT_AES_BLOCK_SIZE));

			enc_parameters.input = padded_plain_data;
			enc_parameters.input_size = padded_plain_data_size;
			enc_parameters.private_key = key;
			enc_parameters.key_size = DEF_CRYPT_AES_256_KEY_SIZE;
			//Only use first 16 bytes.
			enc_parameters.initialization_vector = hash_parameters.hash;
			enc_parameters.iv_size = DEF_CRYPT_AES_IV_SIZE;
			enc_parameters.encrypted = NULL;
			enc_parameters.encrypted_size = 0;
			enc_parameters.type = CRYPT_ENCRYPTION_TYPE_AES_256_CBC;

			result.code = Util_crypt_encrypt(&enc_parameters);
			if(result.code == DEF_SUCCESS)
			{
				//Write log data to the SD card.
				result = Util_file_save_to_file(file_name, DEF_MAIN_DIR + "chat_logs/", enc_parameters.encrypted, enc_parameters.encrypted_size, true);
				if(result.code == DEF_SUCCESS)
				{
					result = Line_save_room_info(logs, file_name, unreads);
					if(result.code != DEF_SUCCESS)
						Util_log_save(DEF_LINE_LOG_THREAD_STR, "Line_save_room_info()..." + result.string + result.error_description, result.code);
				}
				else
					Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_file_save_to_file()..." + result.string + result.error_description, result.code);
			}
			else
				Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_crypt_encrypt()...", result.code);

			free(enc_parameters.encrypted);
			enc_parameters.encrypted = NULL;
		}
		else
			Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_crypt_hash()...", result.code);

		free(hash_parameters.hash);
		hash_parameters.hash = NULL;
	}

	free(padded_plain_data);
	padded_plain_data = NULL;

	return result;
}

static uint32_t Line_generate_password_hash(uint8_t* password, uint8_t password_length, uint8_t hash[32])
{
	uint32_t result = DEF_ERR_OTHER;
	Util_crypt_hash_parameters parameters = { 0, };

	parameters.input = password;
	parameters.input_size = password_length;
	parameters.type = CRYPT_HASH_TYPE_SHA_512;
	parameters.hash = NULL;
	parameters.hash_size = 0;

	result = Util_crypt_hash(&parameters);
	if(result == DEF_SUCCESS)
	{
		//We use even index as a password hash.
		//e.g. If SHA 512 hash is 0x00 0x11 0x22 0x33 0x44 ...
		//Password hash will be 0x00 0x22 0x44 ...
		for(uint8_t i = 0; i < parameters.hash_size; i += 2)
			hash[(i / 2)] = parameters.hash[i];
	}
	else
		Util_log_save(DEF_LINE_INIT_STR, "Util_crypt_hash()...", result);

	free(parameters.hash);
	parameters.hash = NULL;

	return result;
}

static void Line_draw_message(int default_text_color, bool is_top_screen, Line_y_pos* y_pos_data)
{
	if(!y_pos_data)
		return;

	if(LightLock_TryLock(&line.log_copy_string_lock) == 0)
	{
		uint16_t step = 30000;
		uint16_t content_index = 0;
		uint32_t start_index = step;
		uint32_t end_index = 0;
		int16_t screen_offset = (is_top_screen ? 0 : -240);

		//Search for start index to draw.
		while(true)
		{
			int32_t draw_offset = (line.optimized_logs.draw_offset[start_index] + y_pos_data->y + screen_offset);

			if(draw_offset < 0)
			{
				//+----------+
				//|??????????| <- Now,
				//|??????????| <- We are
				//|??????????| <- in somewhere
				//|??????????| <- at "?" mark.
				//|##########| <- Offset that we are looking for (first occurrence of (draw_offset >= 0)).
				//|          |
				//|          |
				//|          |
				//|          |
				//+----------+

				//End of search.
				if(step == 0 || start_index == 59999)
					break;

				step /= 2;
				start_index += step;

				if(start_index > 59999)
					start_index = 59999;
			}
			else
			{
				//+----------+
				//|          |
				//|          |
				//|          |
				//|          |
				//|??????????| <- Offset that we are looking for (first occurrence of (draw_offset >= 0)).
				//|??????????| <- Now,
				//|??????????| <- We are
				//|??????????| <- in somewhere
				//|??????????| <- at "?" mark.
				//+----------+

				//End of search.
				if(step == 0 || start_index == 0)
					break;

				step /= 2;
				start_index -= step;

				//Safety guard.
				if(start_index < 0)
					start_index = 0;
			}
		}

		//Above calculation has low accuracy due to array size.
		//So double check and fix it here.
		while(true)
		{
			int32_t draw_offset = 0;

			if(start_index == 0)
				break;

			draw_offset = (line.optimized_logs.draw_offset[(start_index - 1)] + y_pos_data->y + screen_offset);
			if(draw_offset < 0)
				break;
			else
				start_index--;
		}

		if(line.optimized_logs.draw_offset[start_index] != UINT32_MAX)
		{
			//Search for end index.
			end_index = start_index;
			for (uint32_t i = start_index; i < 60000; i++)
			{
				int32_t draw_offset = 0;

				if(i != 0)
					draw_offset = (line.optimized_logs.draw_offset[i - 1] + y_pos_data->y + screen_offset);
				else
					draw_offset = (y_pos_data->y + screen_offset);

				//End of search.
				if(line.optimized_logs.draw_offset[i] == UINT32_MAX
				|| (is_top_screen && draw_offset > 240) || (!is_top_screen && draw_offset > 130))
					break;

				end_index = i;
			}

			end_index++;
		}

		for (uint32_t i = start_index; i < end_index; i++)
		{
			int color_cache = default_text_color;
			double x_pos = (is_top_screen ? 50 : 10);
			double y_pos = 0;

			if(i != 0)
				y_pos = (line.optimized_logs.draw_offset[i - 1] + y_pos_data->y + screen_offset);
			else
				y_pos = (y_pos_data->y + screen_offset);

			if((line.tab_mode == LINE_TAB_MODE_SEARCH && line.search_found_item_index[line.search_selected_index] == line.optimized_logs.original_index[i])
			|| (line.tab_mode == LINE_TAB_MODE_COPY && line.copy_selected_index == line.optimized_logs.original_index[i]))
			{
				//If message is selected, change the text color.
				color_cache = 0xFFB000B0;//Purple.
			}

			switch(line.optimized_logs.type[i])
			{
				case LINE_MSG_TYPE_TEXT:
				{
					Draw(line.optimized_logs.data[i], x_pos, y_pos, line.text_size, line.text_size, color_cache);

					break;
				}

				case LINE_MSG_TYPE_SPEAKER:
				{
					Draw(line.optimized_logs.data[i] + " :", x_pos, y_pos, (line.text_size * 0.75), (line.text_size * 0.75), 0xFF404040);

					break;
				}

				case LINE_MSG_TYPE_TIME:
				{
					Draw(line.optimized_logs.data[i], x_pos, y_pos, (line.text_size * 0.65), (line.text_size * 0.65), 0xFF808080);

					break;
				}

				case LINE_MSG_TYPE_ROOM_ID:
				{
					if(!is_top_screen && content_index < 32)
					{
						int color = (line.content[content_index].button.selected ? DEF_DRAW_RED : DEF_DRAW_WEAK_RED);

						Draw(line_msg[28], x_pos, y_pos, line.text_size, line.text_size, color_cache, X_ALIGN_LEFT,
						Y_ALIGN_TOP, 0, 0, BACKGROUND_UNDER_TEXT, &line.content[content_index].button, color);

						//Set the log index reference.
						line.content[content_index++].log_index = i;
					}
					else
					{
						Draw(line_msg[28], x_pos, y_pos, line.text_size, line.text_size, color_cache, X_ALIGN_LEFT,
						Y_ALIGN_TOP, 0, 0, BACKGROUND_UNDER_TEXT, var_square_image[0], DEF_DRAW_WEAK_RED);
					}

					break;
				}

				case LINE_MSG_TYPE_STICKER:
				{
					uint16_t texture_index = Line_stickers_index_to_textures_index(atoi(line.optimized_logs.data[i].c_str()));

					Draw_texture(line.sticker_images[texture_index], x_pos, y_pos, (line.text_size * 120.0), (line.text_size * 120.0));

					break;
				}

				case LINE_MSG_TYPE_IMAGE:
				{
					if(!is_top_screen && content_index < 32)
					{
						int color = (line.content[content_index].button.selected ? DEF_DRAW_RED : DEF_DRAW_WEAK_RED);

						Draw(line_msg[27], x_pos, y_pos, line.text_size, line.text_size, color_cache, X_ALIGN_LEFT,
						Y_ALIGN_TOP, 0, 0, BACKGROUND_UNDER_TEXT, &line.content[content_index].button, color);

						//Set the log index reference.
						line.content[content_index++].log_index = i;
					}
					else
					{
						Draw(line_msg[27], x_pos, y_pos, line.text_size, line.text_size, color_cache, X_ALIGN_LEFT,
						Y_ALIGN_TOP, 0, 0, BACKGROUND_UNDER_TEXT, var_square_image[0], DEF_DRAW_WEAK_RED);
					}

					break;
				}

				case LINE_MSG_TYPE_AUDIO:
				{
					if(!is_top_screen && content_index < 32)
					{
						int color = (line.content[content_index].button.selected ? DEF_DRAW_RED : DEF_DRAW_WEAK_RED);

						Draw(line_msg[53], x_pos, y_pos, line.text_size, line.text_size, color_cache, X_ALIGN_LEFT,
						Y_ALIGN_TOP, 0, 0, BACKGROUND_UNDER_TEXT, &line.content[content_index].button, color);

						//Set the log index reference.
						line.content[content_index++].log_index = i;
					}
					else
					{
						Draw(line_msg[53], x_pos, y_pos, line.text_size, line.text_size, color_cache, X_ALIGN_LEFT,
						Y_ALIGN_TOP, 0, 0, BACKGROUND_UNDER_TEXT, var_square_image[0], DEF_DRAW_WEAK_RED);
					}

					break;
				}

				case LINE_MSG_TYPE_VIDEO:
				{
					if(!is_top_screen && content_index < 32)
					{
						int color = (line.content[content_index].button.selected ? DEF_DRAW_RED : DEF_DRAW_WEAK_RED);

						Draw(line_msg[54], x_pos, y_pos, line.text_size, line.text_size, color_cache, X_ALIGN_LEFT,
						Y_ALIGN_TOP, 0, 0, BACKGROUND_UNDER_TEXT, &line.content[content_index].button, color);

						//Set the log index reference.
						line.content[content_index++].log_index = i;
					}
					else
					{
						Draw(line_msg[54], x_pos, y_pos, line.text_size, line.text_size, color_cache, X_ALIGN_LEFT,
						Y_ALIGN_TOP, 0, 0, BACKGROUND_UNDER_TEXT, var_square_image[0], DEF_DRAW_WEAK_RED);
					}

					break;
				}

				case LINE_MSG_TYPE_FILE:
				{
					if(!is_top_screen && content_index < 32)
					{
						int color = (line.content[content_index].button.selected ? DEF_DRAW_RED : DEF_DRAW_WEAK_RED);

						Draw(line_msg[55], x_pos, y_pos, line.text_size, line.text_size, color_cache, X_ALIGN_LEFT,
						Y_ALIGN_TOP, 0, 0, BACKGROUND_UNDER_TEXT, &line.content[content_index].button, color);

						//Set the log index reference.
						line.content[content_index++].log_index = i;
					}
					else
					{
						Draw(line_msg[55], x_pos, y_pos, line.text_size, line.text_size, color_cache, X_ALIGN_LEFT,
						Y_ALIGN_TOP, 0, 0, BACKGROUND_UNDER_TEXT, var_square_image[0], DEF_DRAW_WEAK_RED);
					}

					break;
				}

				default:
					break;
			}
		}

		if(!is_top_screen)
		{
			//Reset struct data that doesn't have content information.
			for(int i = content_index; i < 32; i++)
			{
				line.content[i].log_index = -1;
				line.content[i].button.x = -1;
				line.content[i].button.y = -1;
				line.content[i].button.x_size = -1;
				line.content[i].button.y_size = -1;
			}
		}

		LightLock_Unlock(&line.log_copy_string_lock);
	}
}

static std::string Line_get_google_drive_file_id(std::string url)
{
	std::size_t start_offset = std::string::npos;
	std::size_t end_offset = std::string::npos;

	//Check if the URL is google drive's URL.
	//Check for "https://lh3.googleusercontent.com/d/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx=w16383-h16383".
	if(url.find("lh3.googleusercontent.com") != std::string::npos)
	{
		start_offset = url.find("/d/");
		if(start_offset != std::string::npos)
		{
			//We've found the file ID, set offsets.
			start_offset += 3;//Skip "/d/" itself.
			//Remove "=w16383-h16383" if exists.
			end_offset = url.find("=");
		}
	}

	if(start_offset == std::string::npos)
	{
		//Check for "https://drive.google.com/uc?export=download&id=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx".
		if(url.find("drive.google.com") != std::string::npos)
		{
			start_offset = url.find("id=");
			if(start_offset != std::string::npos)
			{
				//We've found the file ID, set offsets.
				start_offset += 3;//Skip "id=" itself.
				end_offset = std::string::npos;
			}
		}
	}

	if(start_offset != std::string::npos)
	{
		//Extract file ID.
		if(end_offset != std::string::npos)
			return url.substr(start_offset, (end_offset - start_offset));
		else
			return url.substr(start_offset);
	}

	//Not found.
	return "";
}

static uint16_t Line_stickers_index_to_textures_index(uint32_t sticker_index)
{
	int texture_index = 0;
	for (int i = 1; i < 121; i++)
	{
		if (line.sticker_list[i] == sticker_index)
		{
			texture_index = i;
			break;
		}
	}
	return texture_index;
}

static void Line_qr_scan_result_callback(char* decoded_data)
{
	Line_main_state main_state = LINE_STATE_MAIN_INVALID;
	Line_sub_state sub_state = DEF_LINE_STATE_SUB_NONE;
	Line_get_script_url_request_data* data = NULL;

	if(!line_already_init)
		return;

	LightLock_Lock(&line.state_lock);
	main_state = line.main_state;
	sub_state = line.sub_state;
	LightLock_Unlock(&line.state_lock);

	//If we are not in chat room selection or we are processing GAS URL, do nothing.
	if(main_state != LINE_STATE_MAIN_CHAT_ROOM_SELECTION
	|| Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_GET_SCRIPT_URL))
		return;

	//Suspend camera app and resume line app.
	Cam_suspend();
	Line_resume();

	data = (Line_get_script_url_request_data*)malloc(sizeof(Line_get_script_url_request_data));
	if(data)
	{
		Line_add_sub_state(&line, DEF_LINE_STATE_SUB_GET_SCRIPT_URL);
		snprintf(data->url, sizeof(data->url), "%s", decoded_data);

		//Get script URL (in case of short URL) in worker thread.
		DEF_LINE_QUEUE_ADD_WITH_LOG(&line.worker_thread_queue, LINE_COMMAND_WORKER_THREAD_GET_SCRIPT_URL_REQUEST, data, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
	}
}

bool Line_query_init_flag(void)
{
	return line_already_init;
}

bool Line_query_running_flag(void)
{
	return line_main_run;
}

void Line_reset_id(void)
{
	for (int i = 0; i < 128; i++)
	{
		line.room_data[i].is_icon_available = false;
		line.room_data[i].num_of_unread_logs = 0;
		line.room_data[i].log_height = 0;
		line.room_data[i].id = "";
		line.room_data[i].room_name = "";
		line.room_data[i].room_icon_url = "";

		Draw_texture_free(&line.icon[i]);
	}
}

void Line_reset_msg(void)
{
	line.search_found_items = 0;
	line.optimized_logs.num_of_lines = 0;
	line.logs.num_of_logs = 0;
	line.logs.log_height = 0;
	line.logs.room_name = "";
	line.logs.room_icon_url = "";

	for (int i = 0; i < 4000; i++)
	{
		line.logs.logs.text[i] = "";
		line.logs.logs.speaker[i] = "";
		line.logs.logs.time[i] = "";
		line.search_found_item_index[i] = 0;
		line.optimized_logs.start_pos[i] = 0;
	}
	for (int i = 0; i < 60000; i++)
	{
		line.optimized_logs.original_index[i] = UINT16_MAX;
		line.optimized_logs.draw_offset[i] = UINT32_MAX;
		line.optimized_logs.data[i] = "";
		line.optimized_logs.type[i] = LINE_MSG_TYPE_INVALID;
	}
}

Result_with_string Line_load_msg(std::string lang)
{
	return Util_load_msg("line_" + lang + ".txt", line_msg, DEF_LINE_NUM_OF_MSG);
}

void Line_set_send_file(std::string filename, std::string dir_name)
{
	line.file_name = filename;
	line.file_dir = dir_name;
	Line_add_sub_state(&line, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_FILE);
}

void Line_cancel_select_file(void)
{
}

void Line_resume(void)
{
	Menu_suspend();
	line_thread_suspend = false;
	line_main_run = true;
	var_need_reflesh = true;
}

void Line_suspend(void)
{
	line_thread_suspend = true;
	line_main_run = false;
	Menu_resume();
}

void Line_hid(Hid_info key)
{
	bool hit = false;
	double pos_x;
	std::string cache_string = "";
	std::string dir = "";
	Result_with_string result;
	Line_main_state main_state = LINE_STATE_MAIN_INVALID;
	Line_sub_state sub_state = DEF_LINE_STATE_SUB_NONE;

	LightLock_Lock(&line.state_lock);
	main_state = line.main_state;
	sub_state = line.sub_state;
	LightLock_Unlock(&line.state_lock);

	//We shouldn't handle any key input when software keyboard is active.
	if(Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_SWKBD_EXIT))
		return;

	if(Util_err_query_error_show_flag())
		Util_err_main(key);
	else if(Util_expl_query_show_flag())
		Util_expl_main(key);
	else
	{
		if (key.p_touch || key.h_touch)
		{
			line.text_y_move_left = 0;
		}
		else
		{
			Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_SCROLL_MODE);

			line.text_y_move_left -= (line.text_y_move_left * 0.025);
			if (line.text_y_move_left < 0.5 && line.text_y_move_left > -0.5)
				line.text_y_move_left = 0;
		}

		if(Util_hid_is_pressed(key, *Draw_get_bot_ui_button()))
			Draw_get_bot_ui_button()->selected = true;
		else if (key.p_start || (Util_hid_is_released(key, *Draw_get_bot_ui_button()) && Draw_get_bot_ui_button()->selected))
			Line_suspend();
		else if (main_state == LINE_STATE_MAIN_CHAT_ROOM_SELECTION)
		{
			if(!Line_has_sub_state(sub_state, (DEF_LINE_STATE_SUB_GET_SCRIPT_URL | DEF_LINE_STATE_SUB_LOAD_LOGS | DEF_LINE_STATE_SUB_WAIT_SELECTION_GAS_URL)))
			{
				if(key.touch_y < 165)
				{
					for (int i = 0; i < 128; i++)
					{
						if(Util_hid_is_pressed(key, line.room_button[i]))
						{
							Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_SCROLL_MODE);

							line.room_button[i].selected = true;
							hit = true;
							break;
						}
						else if (Util_hid_is_released(key, line.room_button[i]) && line.room_button[i].selected)
						{
							LightLock_Lock(&line.state_lock);
							line.main_state = LINE_STATE_MAIN_CHAT_ROOM;
							LightLock_Unlock(&line.state_lock);
							Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_SCROLL_MODE);

							line.selected_room_index = i;
							line.y_pos[line.main_state].min_y = 0;

							//Set sub state and send a load log request.
							Line_add_sub_state(&line, DEF_LINE_STATE_SUB_LOAD_LOGS);
							DEF_LINE_QUEUE_ADD_WITH_LOG(&line.log_thread_queue, LINE_COMMAND_LOG_THREAD_LOAD_LOG_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
							break;
						}
						else if (!Util_hid_is_held(key, line.room_button[i]) && line.room_button[i].selected)
						{
							Line_add_sub_state(&line, DEF_LINE_STATE_SUB_SCROLL_MODE);
							line.room_button[i].selected = false;
							break;
						}
						else if(Util_hid_is_pressed(key, line.room_update_button[i]))
						{
							Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_SCROLL_MODE);
							line.room_update_button[i].selected = true;
							hit = true;
							break;
						}
						else if (Util_hid_is_released(key, line.room_update_button[i]) && line.room_update_button[i].selected)
						{
							Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_SCROLL_MODE);
							line.selected_room_index = i;

							//Set sub state and send a download logs request.
							Line_add_sub_state(&line, DEF_LINE_STATE_SUB_LOAD_LOGS);
							DEF_LINE_QUEUE_ADD_WITH_LOG(&line.log_thread_queue, LINE_COMMAND_LOG_THREAD_DOWNLOAD_LOG_NO_PARSE_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);

							break;
						}
						else if (!Util_hid_is_held(key, line.room_update_button[i]) && line.room_update_button[i].selected)
						{
							Line_add_sub_state(&line, DEF_LINE_STATE_SUB_SCROLL_MODE);
							line.room_update_button[i].selected = false;
							break;
						}
					}
				}

				if(Util_hid_is_pressed(key, line.add_new_id_button))
					line.add_new_id_button.selected = true;
				else if (key.p_y || (Util_hid_is_released(key, line.add_new_id_button) && line.add_new_id_button.selected))
				{
					//Send a add new room ID request.
					DEF_LINE_QUEUE_ADD_WITH_LOG(&line.main_thread_queue, LINE_COMMAND_MAIN_THREAD_ADD_ROOM_ID_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
				}
				else if(Util_hid_is_pressed(key, line.change_gas_url_button))
					line.change_gas_url_button.selected = true;
				else if (key.p_x || (Util_hid_is_released(key, line.change_gas_url_button) && line.change_gas_url_button.selected))
				{
					//Wait for user.
					Line_add_sub_state(&line, DEF_LINE_STATE_SUB_WAIT_SELECTION_GAS_URL);
				}
				else if(Util_hid_is_pressed(key, line.change_app_password_button))
					line.change_app_password_button.selected = true;
				else if (key.p_a || (Util_hid_is_released(key, line.change_app_password_button) && line.change_app_password_button.selected))
				{
					//Send a edit application password request.
					DEF_LINE_QUEUE_ADD_WITH_LOG(&line.main_thread_queue, LINE_COMMAND_MAIN_THREAD_EDIT_APP_PASSWORD_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
				}
			}
			else if(Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_SELECTION_GAS_URL))
			{
				if(Util_hid_is_pressed(key, line.use_camera_button))
				{
					line.use_camera_button.selected = true;
					hit = true;
				}
				else if (key.p_a || (Util_hid_is_released(key, line.use_camera_button) && line.use_camera_button.selected))
				{
					//Remove waiting flag.
					Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_WAIT_SELECTION_GAS_URL);

					//Send a edit Google Apps Script URL request.
					DEF_LINE_QUEUE_ADD_WITH_LOG(&line.main_thread_queue, LINE_COMMAND_MAIN_THREAD_SCAN_GAS_URL_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
				}
				else if(Util_hid_is_pressed(key, line.use_swkbd_button))
				{
					line.use_swkbd_button.selected = true;
					hit = true;
				}
				else if (key.p_x || (Util_hid_is_released(key, line.use_swkbd_button) && line.use_swkbd_button.selected))
				{
					//Remove waiting flag.
					Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_WAIT_SELECTION_GAS_URL);

					//Send a edit Google Apps Script URL request.
					DEF_LINE_QUEUE_ADD_WITH_LOG(&line.main_thread_queue, LINE_COMMAND_MAIN_THREAD_TYPE_GAS_URL_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
				}
				else if(Util_hid_is_pressed(key, line.back_button))
				{
					line.back_button.selected = true;
					hit = true;
				}
				else if (key.p_b || (Util_hid_is_released(key, line.back_button) && line.back_button.selected))
					Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_WAIT_SELECTION_GAS_URL);//Canceled by user.
			}

			if(!hit && Util_hid_is_pressed(key, line.scroll_bar))
				line.scroll_bar.selected = true;
			else if (!hit && key.p_touch && key.touch_y <= 169)
				Line_add_sub_state(&line, DEF_LINE_STATE_SUB_SCROLL_MODE);
		}
		else
		{
			if (Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT))
			{
				if(Util_hid_is_pressed(key, line.yes_button))
					line.yes_button.selected = true;
				else if (key.p_a || (Util_hid_is_released(key, line.yes_button) && line.yes_button.selected))
				{
					if (Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_MESSAGE))
					{
						Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_MESSAGE);
						//User has approved, send it.
						Line_add_sub_state(&line, DEF_LINE_STATE_SUB_SEND_TEXT);
						DEF_LINE_QUEUE_ADD_WITH_LOG(&line.log_thread_queue, LINE_COMMAND_LOG_THREAD_SEND_MESSAGE_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
					}
					else if (Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_STICKER))
					{
						Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_STICKER);
						//User has approved, send it.
						Line_add_sub_state(&line, DEF_LINE_STATE_SUB_SEND_STICKER);
						DEF_LINE_QUEUE_ADD_WITH_LOG(&line.log_thread_queue, LINE_COMMAND_LOG_THREAD_SEND_STICKER_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
					}
					else if (Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_FILE))
					{
						Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_FILE);
						//User has approved, send it.
						Line_add_sub_state(&line, DEF_LINE_STATE_SUB_SEND_FILE);
						DEF_LINE_QUEUE_ADD_WITH_LOG(&line.log_thread_queue, LINE_COMMAND_LOG_THREAD_SEND_FILE_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
					}
					else if (Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_DELETE_ROOM)
					&& !Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_LOAD_LOGS))
					{
						Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_DELETE_ROOM);
						//User has approved, delete the room.
						DEF_LINE_QUEUE_ADD_WITH_LOG(&line.worker_thread_queue, LINE_COMMAND_WORKER_THREAD_DELETE_ROOM_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);

						Line_reset_msg();
					}
				}
				else if(Util_hid_is_pressed(key, line.no_button))
					line.no_button.selected = true;
				else if (key.p_b || (Util_hid_is_released(key, line.no_button) && line.no_button.selected))
				{
					//Cancel sending a message or deleting a room.
					if (Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_MESSAGE))
						Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_MESSAGE);
					else if (Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_STICKER))
						Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_STICKER);
					else if (Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_FILE))
						Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_FILE);
					else if (Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_DELETE_ROOM))
						Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_DELETE_ROOM);
				}
				else if(Util_hid_is_pressed(key, line.edit_msg_button) && Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_MESSAGE))
					line.edit_msg_button.selected = true;
				else if ((key.p_x || (Util_hid_is_released(key, line.edit_msg_button) && line.edit_msg_button.selected)) && Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_MESSAGE))
				{
					//Send a input text message request.
					DEF_LINE_QUEUE_ADD_WITH_LOG(&line.main_thread_queue, LINE_COMMAND_MAIN_THREAD_INPUT_TEXT_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
				}
				else if(Util_hid_is_pressed(key, line.view_image_button) && Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_FILE))
					line.view_image_button.selected = true;
				else if ((key.p_x || (Util_hid_is_released(key, line.view_image_button) && line.view_image_button.selected)) && Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_FILE))
				{
					Line_suspend();
					if(!Imv_query_init_flag())
						Imv_init(false);
					else
						Imv_resume();

					Imv_set_load_file(line.file_name, line.file_dir);
				}
			}
			else if (line.tab_mode == LINE_TAB_MODE_STICKER)
			{
				if (key.p_b || (key.p_touch && key.touch_x >= 290 && key.touch_x <= 309 && key.touch_y > 120 && key.touch_y < 139))
				{
					hit = true;
					line.tab_mode = LINE_TAB_MODE_SEND;
				}

				for (int i = 0; i < 10; i++)
				{
					if (key.p_touch && key.touch_x >= 10 + (i * 30) && key.touch_x <= 39 + (i * 30) && key.touch_y > 140 && key.touch_y < 149)
						line.selected_sticker_tab_index = i;
				}

				for (int i = 0; i < 7; i++)
				{
					if (key.p_touch && key.touch_x >= 20 + (i * 50) && key.touch_x <= 49 + (i * 50) && key.touch_y > 150 && key.touch_y < 179)
					{
						Line_add_sub_state(&line, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_STICKER);
						line.selected_sticker_index = (line.selected_sticker_tab_index * 12) + (i + 1);
						line.tab_mode = LINE_TAB_MODE_SEND;
					}
					else if (key.p_touch && key.touch_x >= 20 + (i * 50) && key.touch_x <= 49 + (i * 50) && key.touch_y > 190 && key.touch_y < 219)
					{
						Line_add_sub_state(&line, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_STICKER);
						line.selected_sticker_index = (line.selected_sticker_tab_index * 12) + (i + 7);
						line.tab_mode = LINE_TAB_MODE_SEND;
					}
				}
			}
			else if(main_state == LINE_STATE_MAIN_CHAT_ROOM)
			{
				if(Util_hid_is_pressed(key, line.menu_button[0]))
					line.menu_button[0].selected = true;
				else if (Util_hid_is_released(key, line.menu_button[0]) && line.menu_button[0].selected)
					line.tab_mode = LINE_TAB_MODE_SEND;
				else if(Util_hid_is_pressed(key, line.menu_button[1]))
					line.menu_button[1].selected = true;
				else if (Util_hid_is_released(key, line.menu_button[1]) && line.menu_button[1].selected)
					line.tab_mode = LINE_TAB_MODE_RECEIVE;
				else if(Util_hid_is_pressed(key, line.menu_button[2]))
					line.menu_button[2].selected = true;
				else if (Util_hid_is_released(key, line.menu_button[2]) && line.menu_button[2].selected)
					line.tab_mode = LINE_TAB_MODE_COPY;
				else if(Util_hid_is_pressed(key, line.menu_button[3]))
					line.menu_button[3].selected = true;
				else if (Util_hid_is_released(key, line.menu_button[3]) && line.menu_button[3].selected)
					line.tab_mode = LINE_TAB_MODE_SETTINGS;
				else if(Util_hid_is_pressed(key, line.menu_button[4]))
					line.menu_button[4].selected = true;
				else if (Util_hid_is_released(key, line.menu_button[4]) && line.menu_button[4].selected)
					line.tab_mode = LINE_TAB_MODE_SEARCH;
				else if(Util_hid_is_pressed(key, line.menu_button[5]))
					line.menu_button[5].selected = true;
				else if (Util_hid_is_released(key, line.menu_button[5]) && line.menu_button[5].selected)
					line.tab_mode = LINE_TAB_MODE_ADVANCED;
				else if(Util_hid_is_pressed(key, line.back_button) && !Line_has_sub_state(sub_state, (DEF_LINE_STATE_SUB_LOAD_LOGS | DEF_LINE_STATE_SUB_SEND_MESSAGE)))
					line.back_button.selected = true;
				else if (Util_hid_is_released(key, line.back_button) && line.back_button.selected
				&& !Line_has_sub_state(sub_state, (DEF_LINE_STATE_SUB_LOAD_LOGS | DEF_LINE_STATE_SUB_SEND_MESSAGE)))
				{
					LightLock_Lock(&line.state_lock);
					line.main_state = LINE_STATE_MAIN_CHAT_ROOM_SELECTION;
					LightLock_Unlock(&line.state_lock);

					Line_reset_msg();
				}
				else if (line.tab_mode == LINE_TAB_MODE_SEND && !Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_SEND_MESSAGE))
				{
					if(Util_hid_is_pressed(key, line.send_msg_button))
						line.send_msg_button.selected = true;
					else if (key.p_a || (Util_hid_is_released(key, line.send_msg_button) && line.send_msg_button.selected))
					{
						//Send a input text message request.
						DEF_LINE_QUEUE_ADD_WITH_LOG(&line.main_thread_queue, LINE_COMMAND_MAIN_THREAD_INPUT_TEXT_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
					}
					else if(Util_hid_is_pressed(key, line.send_sticker_button))
						line.send_sticker_button.selected = true;
					else if (key.p_y || (Util_hid_is_released(key, line.send_sticker_button) && line.send_sticker_button.selected))
						line.tab_mode = LINE_TAB_MODE_STICKER;
					else if(Util_hid_is_pressed(key, line.send_file_button))
						line.send_file_button.selected = true;
					else if (key.p_x || (Util_hid_is_released(key, line.send_file_button) && line.send_file_button.selected))
					{
						Util_expl_set_callback(Line_set_send_file);
						Util_expl_set_cancel_callback(Line_cancel_select_file);
						Util_expl_set_show_flag(true);
					}
				}
				else if (line.tab_mode == LINE_TAB_MODE_RECEIVE)
				{
					if(Util_hid_is_pressed(key, line.dl_logs_button))
						line.dl_logs_button.selected = true;
					else if (key.p_b || (Util_hid_is_released(key, line.dl_logs_button) && line.dl_logs_button.selected))
					{
						//Set sub state and send a download logs request.
						Line_add_sub_state(&line, DEF_LINE_STATE_SUB_LOAD_LOGS);
						DEF_LINE_QUEUE_ADD_WITH_LOG(&line.log_thread_queue, LINE_COMMAND_LOG_THREAD_DOWNLOAD_LOG_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
					}
					else if(Util_hid_is_pressed(key, line.max_logs_bar) && !Line_has_sub_state(sub_state, (DEF_LINE_STATE_SUB_LOAD_LOGS | DEF_LINE_STATE_SUB_SEND_MESSAGE)))
						line.max_logs_bar.selected = true;
					else if (key.h_touch && line.max_logs_bar.selected)
					{
						if(key.touch_x <= 99)
							pos_x = 100;
						else if(key.touch_x >= 300)
							pos_x = 299;
						else
							pos_x = key.touch_x;

						line.num_of_logs = (pos_x - 99) * 20;
					}
				}
				else if (line.tab_mode == LINE_TAB_MODE_COPY)
				{
					if(Util_hid_is_pressed(key, line.copy_button))
						line.copy_button.selected = true;
					else if (key.p_x || (Util_hid_is_released(key, line.copy_button) && line.copy_button.selected))
					{
						if(LightLock_TryLock(&line.log_copy_string_lock) == 0)
						{
							var_clipboard = line.logs.logs.text[line.copy_selected_index];
							LightLock_Unlock(&line.log_copy_string_lock);
						}
					}
					else if(Util_hid_is_pressed(key, line.copy_select_down_button))
						line.copy_select_down_button.selected = true;
					else if(key.h_d_down || (Util_hid_is_held(key, line.copy_select_down_button) && line.copy_select_down_button.selected))
					{
						if((key.held_time > 150 && key.held_time % 2 == 0)
						|| (key.held_time <= 150 && key.held_time % 20 == 0))
						{
							if(line.logs.num_of_logs - 1 >= line.copy_selected_index + 1)
							{
								line.copy_selected_index++;
								line.y_pos[main_state].y = line.optimized_logs.start_pos[line.copy_selected_index];
							}
						}
					}
					else if(Util_hid_is_pressed(key, line.copy_select_up_button))
						line.copy_select_up_button.selected = true;
					else if(key.h_d_up || (Util_hid_is_held(key, line.copy_select_up_button) && line.copy_select_up_button.selected))
					{
						if((key.held_time > 150 && key.held_time % 2 == 0)
						|| (key.held_time <= 150 && key.held_time % 20 == 0))
						{
							if(line.copy_selected_index - 1 >= 0)
							{
								line.copy_selected_index--;
								line.y_pos[main_state].y = line.optimized_logs.start_pos[line.copy_selected_index];
							}
						}
					}
				}
				else if (line.tab_mode == LINE_TAB_MODE_SETTINGS)
				{
					if (Util_hid_is_pressed(key, line.increase_interval_button))
						line.increase_interval_button.selected = true;
					else if (Util_hid_is_pressed(key, line.decrease_interval_button))
						line.decrease_interval_button.selected = true;
					else if (Util_hid_is_pressed(key, line.decrease_size_button))
						line.decrease_size_button.selected = true;
					else if (Util_hid_is_pressed(key, line.increase_size_button))
						line.increase_size_button.selected = true;
					else if ((key.h_d_up || (Util_hid_is_held(key, line.increase_interval_button) && line.increase_interval_button.selected)))
					{
						if((line.text_interval + 0.5) < 30)
							line.text_interval += 0.5;
						else
							line.text_interval = 30;
					}
					else if ((key.h_d_down || (Util_hid_is_held(key, line.decrease_interval_button) && line.decrease_interval_button.selected)))
					{
						if((line.text_interval - 0.5) > 0)
							line.text_interval -= 0.5;
						else
							line.text_interval = 0;
					}
					else if ((key.h_l || (Util_hid_is_held(key, line.decrease_size_button) && line.decrease_size_button.selected)) && (line.text_size - 0.003) > 0.25)
						line.text_size -= 0.003;
					else if ((key.h_r || (Util_hid_is_held(key, line.increase_size_button) && line.increase_size_button.selected)) && (line.text_size + 0.003) < 3.0)
						line.text_size += 0.003;
				}
				else if (line.tab_mode == LINE_TAB_MODE_SEARCH)
				{
					if (Util_hid_is_pressed(key, line.search_button))
						line.search_button.selected = true;
					else if (key.p_a || (Util_hid_is_released(key, line.search_button) && line.search_button.selected))
					{
						//Send a search text message request.
						DEF_LINE_QUEUE_ADD_WITH_LOG(&line.main_thread_queue, LINE_COMMAND_MAIN_THREAD_SEARCH_TEXT_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
					}
					else if (Util_hid_is_pressed(key, line.search_select_down_button))
						line.search_select_down_button.selected = true;
					else if((key.h_d_down || (Util_hid_is_held(key, line.search_select_down_button) && line.search_select_down_button.selected)))
					{
						if((key.held_time > 150 && key.held_time % 2 == 0)
						|| (key.held_time <= 150 && key.held_time % 20 == 0))
						{
							if(line.search_found_items >= line.search_selected_index + 1)
							{
								line.search_selected_index++;
								line.copy_selected_index = line.search_found_item_index[line.search_selected_index];
								line.y_pos[main_state].y = line.optimized_logs.start_pos[line.copy_selected_index];
							}
						}
					}
					else if (Util_hid_is_pressed(key, line.search_select_up_button))
						line.search_select_up_button.selected = true;
					else if((key.h_d_up || (Util_hid_is_held(key, line.search_select_up_button) && line.search_select_up_button.selected)))
					{
						if((key.held_time > 150 && key.held_time % 2 == 0)
						|| (key.held_time <= 150 && key.held_time % 20 == 0))
						{
							if(line.search_selected_index - 1 >= 0)
							{
								line.search_selected_index--;
								line.copy_selected_index = line.search_found_item_index[line.search_selected_index];
								line.y_pos[main_state].y = line.optimized_logs.start_pos[line.copy_selected_index];
							}
						}
					}
				}
				else if (line.tab_mode == LINE_TAB_MODE_ADVANCED)
				{
					if(Util_hid_is_pressed(key, line.delete_room_button))
						line.delete_room_button.selected = true;
					else if (((key.p_l && key.h_r) || (key.h_l && key.p_r)) || (Util_hid_is_released(key, line.delete_room_button) && line.delete_room_button.selected))
						Line_add_sub_state(&line, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_DELETE_ROOM);
				}
			}

			if(!hit && Util_hid_is_pressed(key, line.scroll_bar))
				line.scroll_bar.selected = true;
			else if (!hit && (key.p_touch || key.r_touch || key.h_touch) && key.touch_y >= 0 && key.touch_y < 140)
			{
				LightLock_Lock(&line.log_copy_string_lock);
				for (int i = 0; i < 32; i++)
				{
					if(Util_hid_is_pressed(key, line.content[i].button))
					{
						hit = true;
						Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_SCROLL_MODE);
						line.content[i].button.selected = true;
						break;
					}
					else if(Util_hid_is_released(key, line.content[i].button) && line.content[i].button.selected)
					{
						Line_message_type type = LINE_MSG_TYPE_INVALID;

						if(line.content[i].log_index < 0 || line.content[i].log_index >= 60000)
							break;//Invalid log index.

						type = line.optimized_logs.type[line.content[i].log_index];
						//Setup the command based on the content type.
						switch (type)
						{
							case LINE_MSG_TYPE_IMAGE:
							case LINE_MSG_TYPE_AUDIO:
							case LINE_MSG_TYPE_VIDEO:
							{
								Line_play_media_request_data* command_data = (Line_play_media_request_data*)malloc(sizeof(Line_play_media_request_data));

								if(command_data)
								{
									command_data->log_id = line.content[i].log_index;

									if(type == LINE_MSG_TYPE_IMAGE)
									{
										//Send a display image request.
										DEF_LINE_QUEUE_ADD_WITH_LOG(&line.main_thread_queue, LINE_COMMAND_MAIN_THREAD_DISPLAY_IMAGE_REQUEST, command_data, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
									}
									else if(type == LINE_MSG_TYPE_AUDIO)
									{
										//Send a play audio request.
										DEF_LINE_QUEUE_ADD_WITH_LOG(&line.main_thread_queue, LINE_COMMAND_MAIN_THREAD_PLAY_AUDIO_REQUEST, command_data, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
									}
									else if(type == LINE_MSG_TYPE_VIDEO)
									{
										//Send a play video request.
										DEF_LINE_QUEUE_ADD_WITH_LOG(&line.main_thread_queue, LINE_COMMAND_MAIN_THREAD_PLAY_VIDEO_REQUEST, command_data, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
									}
								}
								else
									Util_log_save(DEF_LINE_HID_CALLBACK_STR, DEF_ERR_OUT_OF_MEMORY_STR, DEF_ERR_OUT_OF_MEMORY);

								break;
							}

							case LINE_MSG_TYPE_FILE:
							{
								Line_download_file_request_data* command_data = NULL;

								if(Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_DOWNLOAD_FILE))
									break;

								command_data = (Line_download_file_request_data*)malloc(sizeof(Line_download_file_request_data));

								if(command_data)
								{
									command_data->log_id = line.content[i].log_index;

									if(type == LINE_MSG_TYPE_FILE)
									{
										//Set sub state and send a download file request.
										Line_add_sub_state(&line, DEF_LINE_STATE_SUB_DOWNLOAD_FILE);
										DEF_LINE_QUEUE_ADD_WITH_LOG(&line.worker_thread_queue, LINE_COMMAND_WORKER_THREAD_DOWNLOAD_CONTENT_REQUEST, command_data, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
									}
								}
								else
									Util_log_save(DEF_LINE_HID_CALLBACK_STR, DEF_ERR_OUT_OF_MEMORY_STR, DEF_ERR_OUT_OF_MEMORY);

								break;
							}

							case LINE_MSG_TYPE_ROOM_ID:
							{
								Line_add_room_id_request_data* command_data = (Line_add_room_id_request_data*)malloc(sizeof(Line_add_room_id_request_data));

								if(command_data)
								{
									command_data->log_id = line.content[i].log_index;

									//Send a add new room request.
									DEF_LINE_QUEUE_ADD_WITH_LOG(&line.main_thread_queue, LINE_COMMAND_MAIN_THREAD_ADD_ROOM_ID_REQUEST, command_data, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
								}
								else
									Util_log_save(DEF_LINE_HID_CALLBACK_STR, DEF_ERR_OUT_OF_MEMORY_STR, DEF_ERR_OUT_OF_MEMORY);

								break;
							}

							default:
							{
								//Invalid type.
								char msg[48];
								snprintf(msg, sizeof(msg), "Invalid type (%X) !!!!!", type);
								Util_log_save(DEF_LINE_HID_CALLBACK_STR, msg);

								break;
							}
						}
					}
					else if(!Util_hid_is_held(key, line.content[i].button) && line.content[i].button.selected)
					{
						Line_add_sub_state(&line, DEF_LINE_STATE_SUB_SCROLL_MODE);
						line.content[i].button.selected = false;
						break;
					}
				}
				LightLock_Unlock(&line.log_copy_string_lock);
			}

			if (!hit && key.p_touch && key.touch_y < 110)
				Line_add_sub_state(&line, DEF_LINE_STATE_SUB_SCROLL_MODE);
			else if (!hit && key.p_touch && key.touch_y < 140 && !Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_MESSAGE))
				Line_add_sub_state(&line, DEF_LINE_STATE_SUB_SCROLL_MODE);
		}

		if (key.h_c_down || key.h_c_up)
		{
			if (key.held_time > 600)
				line.y_pos[main_state].y += (double)key.cpad_y * var_scroll_speed * 0.5;
			else if (key.held_time > 240)
				line.y_pos[main_state].y += (double)key.cpad_y * var_scroll_speed * 0.125;
			else
				line.y_pos[main_state].y += (double)key.cpad_y * var_scroll_speed * 0.0625;
		}

		if (line.scroll_bar.selected && key.h_touch)
		{
			line.text_y_move_left = 0;
			line.y_pos[main_state].y = line.y_pos[main_state].min_y * ((key.touch_y - 15.0) / 190.0);
		}
		else if (Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_SCROLL_MODE) && key.h_touch)
		{
			line.text_y_move_left += key.touch_y_move;
		}

		line.y_pos[main_state].y -= line.text_y_move_left * var_scroll_speed;

		if (line.y_pos[main_state].y < line.y_pos[main_state].min_y)
			line.y_pos[main_state].y = line.y_pos[main_state].min_y;
		if (line.y_pos[main_state].y > line.y_pos[main_state].max_y)
			line.y_pos[main_state].y = line.y_pos[main_state].max_y;
	}

	if(!key.p_touch && !key.h_touch)
	{
		Draw_get_bot_ui_button()->selected = false;
		line.add_new_id_button.selected = false;
		line.change_gas_url_button.selected = false;
		line.change_app_password_button.selected = false;
		line.back_button.selected = false;
		line.send_msg_button.selected = false;
		line.send_sticker_button.selected = false;
		line.send_file_button.selected = false;
		line.send_success_button.selected = false;
		line.dl_logs_button.selected = false;
		line.max_logs_bar.selected = false;
		line.copy_button.selected = false;
		line.copy_select_down_button.selected = false;
		line.copy_select_up_button.selected = false;
		line.increase_interval_button.selected = false;
		line.decrease_interval_button.selected = false;
		line.increase_size_button.selected = false;
		line.decrease_size_button.selected = false;
		line.search_button.selected = false;
		line.search_select_down_button.selected = false;
		line.search_select_up_button.selected = false;
		line.delete_room_button.selected = false;
		line.yes_button.selected = false;
		line.no_button.selected = false;
		line.view_image_button.selected = false;
		line.edit_msg_button.selected = false;
		line.use_swkbd_button.selected = false;
		line.use_camera_button.selected = false;
		line.scroll_bar.selected = false;
		for(int i = 0; i < 16; i++)
			line.content[i].button.selected = false;
		for(int i = 0; i < 6; i++)
			line.menu_button[i].selected = false;
		for(int i = 0; i < 128; i++)
		{
			line.room_button[i].selected = false;
			line.room_update_button[i].selected = false;
		}
	}

	if(Util_log_query_log_show_flag())
		Util_log_main(key);
}

Result_with_string Line_read_id(std::string dir_path, uint16_t* num_of_ids)
{
	File_type type[128];
	int temp = 0;
	std::string ids[128];
	Result_with_string result;

	*num_of_ids = 0;

	result = Util_file_read_dir(dir_path, &temp, ids, type, 128);
	if (result.code == DEF_SUCCESS)
	{
		uint16_t room_data_index = 0;
		*num_of_ids = temp;

		for (uint16_t i = 0; i < *num_of_ids; i++)
		{
			u8* buffer = NULL;

			result = Util_file_load_from_file(ids[i], dir_path, &buffer, 1024 * 1024);
			if(result.code == DEF_SUCCESS)
			{
				Json_data json_data;

				//This file is NOT encrypted.
				result = Util_json_parse((char*)buffer, &json_data);
				if(result.code == DEF_SUCCESS)
				{
					Line_saved_room_data room_data;

					result = Line_parse_room_data(&json_data, &room_data);
					if(result.code == DEF_SUCCESS)
					{
						//Set up room information.
						line.room_data[room_data_index].room_name = room_data.room_name;
						line.room_data[room_data_index].room_icon_url = room_data.room_icon_url;
						line.room_data[room_data_index].log_height = room_data.log_height;
						line.room_data[room_data_index].num_of_unread_logs = room_data.num_of_unread_logs;
					}
					else
						Util_log_save(DEF_LINE_LOG_THREAD_STR, "Line_parse_room_data()..." + result.string + result.error_description, result.code);
				}
				else
					Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_json_parse()..." + result.string + result.error_description, result.code);

				Util_json_free(&json_data);
			}
			else
				Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_file_load_from_file()..." + result.string + result.error_description, result.code);

			line.room_data[room_data_index].id = ids[i];
			room_data_index++;

			free(buffer);
			buffer = NULL;
		}
	}
	else
		Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_file_read_dir()..." + result.string + result.error_description, result.code);

	return result;
}

Result_with_string Line_load_icon(int room_num)
{
	int image_width = 0;
	int image_height = 0;
	int dled_size = 0;
	size_t cut_pos = 0;
	u8* raw_buffer = NULL;
	std::string filename = "";
	Result_with_string result;

	Draw_texture_free(&line.icon[room_num]);

	if (line.room_data[room_num].room_icon_url != "")
	{
		cut_pos = line.room_data[room_num].room_icon_url.find("lh3.googleusercontent.com/d/");
		if (!(cut_pos == std::string::npos))
		{
			filename = line.room_data[room_num].room_icon_url.substr(cut_pos + 28);
			if (filename.length() > 33)
				filename = filename.substr(0, 33);

			result = Util_file_check_file_exist(filename, DEF_MAIN_DIR + "images/");

			//If file doesn't exist, download it.
			if (result.code != 0)
				result = Util_curl_save_data(line.room_data[room_num].room_icon_url, 1024 * 128, &dled_size, true, 5, DEF_MAIN_DIR + "images/", filename);

			if(result.code == 0)
			{
				Pixel_format pixel_format = PIXEL_FORMAT_INVALID;

				result = Util_image_decoder_decode(DEF_MAIN_DIR + "images/" + filename, &raw_buffer, &image_width, &image_height, &pixel_format);
				if(result.code == 0)
				{
					result = Draw_texture_init(&line.icon[room_num], 32, 32, PIXEL_FORMAT_RGB565LE);
					if(result.code == 0)
					{
						Color_converter_parameters converter_parameters;

						converter_parameters.source = raw_buffer;
						converter_parameters.in_color_format = pixel_format;
						converter_parameters.in_width = image_width;
						converter_parameters.in_height = image_height;
						converter_parameters.converted = NULL;
						converter_parameters.out_color_format = PIXEL_FORMAT_RGB565LE;
						converter_parameters.out_width = 32;
						converter_parameters.out_height = 32;

						result = Util_converter_convert_color(&converter_parameters);
						if(result.code == 0)
						{
							Draw_set_texture_filter(&line.icon[room_num], true);
							Draw_set_texture_data(&line.icon[room_num], converter_parameters.converted, converter_parameters.out_width, converter_parameters.out_height);
						}

						free(converter_parameters.converted);
						converter_parameters.converted = NULL;
					}
				}
				free(raw_buffer);
				raw_buffer = NULL;
			}
		}
		else
		{
			result.code = DEF_ERR_OTHER;
			result.string = "[Error] Bad icon info ";
		}
	}
	else
	{
		result.code = DEF_ERR_OTHER;
		result.string = "[Error] No icon info ";
	}

	return result;
}

void Line_worker_thread(void* arg)
{
	Util_log_save(DEF_LINE_WORKER_THREAD_STR, "Thread started.");
	int log_num = 0;
	Result_with_string result;

	while (line_thread_run)
	{
		void* command_data = NULL;
		Line_command command_id = LINE_COMMAND_INVALID;

		result = Util_queue_get(&line.worker_thread_queue, (u32*)&command_id, &command_data, DEF_ACTIVE_THREAD_SLEEP_TIME);
		if(result.code == DEF_SUCCESS)
		{
			switch (command_id)
			{
				case LINE_COMMAND_WORKER_THREAD_GET_SCRIPT_URL_REQUEST:
				{
					Line_get_script_url_request_data* data = (Line_get_script_url_request_data*)command_data;
					uint8_t* buffer = NULL;
					int dled_size = 0;

					//Command data is mandatory.
					if(!command_data)
					{
						Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_GET_SCRIPT_URL);
						break;
					}

					log_num = Util_log_save(DEF_LINE_WORKER_THREAD_STR, "Util_curl_dl_data()...");
					result = Util_curl_dl_data(data->url, &buffer, (1024 * 1024), &dled_size, true, 20);
					Util_log_add(log_num, result.string, result.code);

					if(result.code == DEF_SUCCESS)
					{
						Json_data json_data;

						//Parse the query result.
						log_num = Util_log_save(DEF_LINE_WORKER_THREAD_STR, "Util_json_parse()...");
						result = Util_json_parse((char*)buffer, &json_data);
						Util_log_add(log_num, result.string + result.error_description, result.code);

						if(result.code == DEF_SUCCESS)
						{
							Line_parsed_script_url url_data;

							log_num = Util_log_save(DEF_LINE_WORKER_THREAD_STR, "Line_parse_script_url()...");
							result = Line_parse_script_url(&json_data, &url_data);
							Util_log_add(log_num, result.string + result.error_description, result.code);

							if(result.code == DEF_SUCCESS)
							{
								line.gas_url = url_data.url;

								log_num = Util_log_save(DEF_LINE_WORKER_THREAD_STR, "Util_file_save_to_file()...");
								result = Util_file_save_to_file("gas_url.txt", DEF_MAIN_DIR, (u8*)url_data.url.c_str(), url_data.url.length(), true);
								Util_log_add(log_num, result.string, result.code);
							}
						}

						Util_json_free(&json_data);
					}
					free(buffer);
					buffer = NULL;

					if(result.code != DEF_SUCCESS)
					{
						Util_err_set_error_message(result.string, result.error_description, DEF_LINE_WORKER_THREAD_STR, result.code);
						Util_err_set_error_show_flag(true);
					}

					Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_GET_SCRIPT_URL);
					break;
				}

				case LINE_COMMAND_WORKER_THREAD_LOAD_ROOM_REQUEST:
				{
					Line_reset_id();
					log_num = Util_log_save(DEF_LINE_WORKER_THREAD_STR, "Line_read_id()...");
					result = Line_read_id(DEF_MAIN_DIR + "to/", &line.num_of_room);
					Util_log_add(log_num, result.string, result.code);

					break;
				}

				case LINE_COMMAND_WORKER_THREAD_LOAD_ICON_REQUEST:
				{
					for (uint16_t i = 0; i < line.num_of_room; i++)
					{
						log_num = Util_log_save(DEF_LINE_WORKER_THREAD_STR, "Line_load_icon()...");
						result = Line_load_icon(i);
						Util_log_add(log_num, result.string, result.code);

						line.room_data[i].is_icon_available = (result.code == DEF_SUCCESS);
					}

					break;
				}

				case LINE_COMMAND_WORKER_THREAD_DELETE_ROOM_REQUEST:
				{
					log_num = Util_log_save(DEF_LINE_WORKER_THREAD_STR, "Util_file_delete_file()...");
					result = Util_file_delete_file(line.room_data[line.selected_room_index].id, DEF_MAIN_DIR + "chat_logs/");
					Util_log_add(log_num, result.string, result.code);

					log_num = Util_log_save(DEF_LINE_WORKER_THREAD_STR, "Util_file_delete_file()...");
					result = Util_file_delete_file(line.room_data[line.selected_room_index].id, DEF_MAIN_DIR + "to/");
					Util_log_add(log_num, result.string, result.code);

					//If "IDs" room was deleted, regenerate it.
					if(line.room_data[line.selected_room_index].id == "IDs")
					{
						std::string ids_room_info = DEF_JSON_START_OBJECT;
						ids_room_info += DEF_JSON_NON_STRING_DATA_WITH_KEY("log_height", "0");
						ids_room_info += DEF_JSON_NON_STRING_DATA_WITH_KEY("num_of_unread_logs", "0");
						ids_room_info += DEF_JSON_STRING_DATA_WITH_KEY("room_name", "IDs");
						ids_room_info += DEF_JSON_STRING_DATA_WITH_KEY("room_icon_url", "");
						ids_room_info += DEF_JSON_END_OBJECT;

						result = Util_file_save_to_file("IDs", DEF_MAIN_DIR + "to/", (u8*)ids_room_info.c_str(), ids_room_info.length(), true);
						Util_log_save(DEF_LINE_INIT_STR, "Util_file_save_to_file()..." + result.string + result.error_description, result.code);
					}

					LightLock_Lock(&line.state_lock);
					line.main_state = LINE_STATE_MAIN_CHAT_ROOM_SELECTION;
					LightLock_Unlock(&line.state_lock);

					Line_reset_id();

					//We need to reload room information and icons.
					DEF_LINE_QUEUE_ADD_WITH_LOG(&line.worker_thread_queue, LINE_COMMAND_WORKER_THREAD_LOAD_ROOM_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
					DEF_LINE_QUEUE_ADD_WITH_LOG(&line.worker_thread_queue, LINE_COMMAND_WORKER_THREAD_LOAD_ICON_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);

					break;
				}

				case LINE_COMMAND_WORKER_THREAD_DOWNLOAD_CONTENT_REQUEST:
				{
					Line_download_file_request_data* data = (Line_download_file_request_data*)command_data;
					std::string url = "";
					std::string file_name = "";

					//Command data is mandatory.
					if(!data)
					{
						Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_DOWNLOAD_FILE);
						break;
					}

					LightLock_Lock(&line.log_copy_string_lock);
					url = line.optimized_logs.data[data->log_id];
					LightLock_Unlock(&line.log_copy_string_lock);

					file_name = Line_get_google_drive_file_id(url);
					if(file_name == "")
					{
						char date[64];
						snprintf(date, sizeof(date), "%04d_%02d_%02d_%02d_%02d_%02d", var_years, var_months, var_days, var_hours, var_minutes, var_seconds);
						file_name = date;
					}

					line.dled_content_size = 0;

					log_num = Util_log_save(DEF_LINE_WORKER_THREAD_STR, "Util_curl_save_data()...");
					result = Util_curl_save_data(url, 1024 * 256, &line.dled_content_size, true, 5, DEF_MAIN_DIR + "contents/", file_name);
					Util_log_add(log_num, result.string, result.code);

					Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_DOWNLOAD_FILE);

					break;
				}

				default:
				{
					//Invalid command was received.
					char msg[64];
					snprintf(msg, sizeof(msg), "Invalid command (%08X) was received!!!!!", command_id);
					Util_log_save(DEF_LINE_WORKER_THREAD_STR, msg);
					break;
				}
			}
		}

		free(command_data);
		command_data = NULL;

		while (line_thread_suspend)
			Util_sleep(DEF_INACTIVE_THREAD_SLEEP_TIME);
	}

	Util_log_save(DEF_LINE_WORKER_THREAD_STR, "Thread exit.");
	threadExit(0);
}

int Line_upload_file_callback(void* out_buffer, int buffer_size, void* user_data)
{
	u8* read_buffer = NULL;
	u32 actual_read_size = 0;
	int read_size = 0;
	std::string encoded_data = "";
	Result_with_string result;
	Line_upload_file_info* upload_info = (Line_upload_file_info*)user_data;

	if(!user_data)
		return -1;

	if(upload_info->end)
		return 0;

	if(upload_info->first)
	{
		upload_info->first = false;
		memcpy(out_buffer, upload_info->first_data.c_str(), upload_info->first_data.length());
		return upload_info->first_data.length();
	}

	read_size = (int)(buffer_size * 0.73) - ((int)(buffer_size * 0.73) % 3);
	result = Util_file_load_from_file_with_range(upload_info->filename, upload_info->dir, &read_buffer, read_size, upload_info->offset, &actual_read_size);

	//IO error.
	if(result.code != 0)
		return -1;

	//EOF.
	if(actual_read_size == 0 && result.code == 0)
	{
		upload_info->end = true;
		memcpy(out_buffer, upload_info->end_data.c_str(), upload_info->end_data.length());
		return upload_info->end_data.length();
	}

	upload_info->offset += actual_read_size;

	encoded_data = Util_encode_to_base64((char*)read_buffer, actual_read_size);
	free(read_buffer);
	read_buffer= NULL;

	memcpy(out_buffer, encoded_data.c_str(), encoded_data.length());
	return encoded_data.length();
}

void Line_log_thread(void* arg)
{
	Util_log_save(DEF_LINE_LOG_THREAD_STR, "Thread started.");
	int log_num = 0;
	Result_with_string result;

	while (line_thread_run)
	{
		void* command_data = NULL;
		Line_command command_id = LINE_COMMAND_INVALID;

		result = Util_queue_get(&line.log_thread_queue, (u32*)&command_id, &command_data, DEF_ACTIVE_THREAD_SLEEP_TIME);
		if(result.code == DEF_SUCCESS)
		{
			switch (command_id)
			{
				case LINE_COMMAND_LOG_THREAD_DOWNLOAD_LOG_REQUEST:
				case LINE_COMMAND_LOG_THREAD_DOWNLOAD_LOG_NO_PARSE_REQUEST:
				case LINE_COMMAND_LOG_THREAD_LOAD_LOG_REQUEST:
				{
					u8* buffer = NULL;
					uint32_t log_height = line.room_data[line.selected_room_index].log_height;
					uint16_t max_logs = line.num_of_logs;
					int dled_size = 0;
					std::string room_id = line.room_data[line.selected_room_index].id;

					if(command_id == LINE_COMMAND_LOG_THREAD_DOWNLOAD_LOG_REQUEST || command_id == LINE_COMMAND_LOG_THREAD_DOWNLOAD_LOG_NO_PARSE_REQUEST)
					{
						uint16_t secret_offset = 0;
						char secret[(DEF_LINE_SECRET_HASH_SIZE * 2) + 1] = { 0, };
						std::string url = line.gas_url + "?id=" + room_id + "&log_height=" + std::to_string(log_height) + "&logs=" + std::to_string(max_logs) + "&gas_ver=" + std::to_string(DEF_LINE_GAS_VER);

						for(uint16_t i = 0; i < DEF_LINE_SECRET_HASH_SIZE; i++)
							secret_offset += snprintf((secret + secret_offset), (sizeof(secret) - secret_offset), "%02X", line.secret_hash[i]);

						url += (std::string)"&secret=" + secret;

						log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_curl_dl_data()...");
						result = Util_curl_dl_data(url, &buffer, 1024 * 1024 * 5, &dled_size, true, 5);
						Util_log_add(log_num, result.string + result.error_description, result.code);
					}
					else if(command_id == LINE_COMMAND_LOG_THREAD_LOAD_LOG_REQUEST)
					{
						log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Line_load_logs()...");
						result = Line_load_logs(room_id, (char**)&buffer);
						Util_log_add(log_num, result.string, result.code);
					}

					if (result.code == DEF_SUCCESS)
					{
						Json_data json_data;

						//Parse the query result.
						log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_json_parse()...");
						result = Util_json_parse((char*)buffer, &json_data);
						Util_log_add(log_num, result.string + result.error_description, result.code);

						if(result.code == DEF_SUCCESS)
						{
							log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Line_parse_logs()...");
							result = Line_parse_logs(&json_data, &line.parsed_logs_buffer);
							Util_log_add(log_num, result.string + result.error_description, result.code);

							if(result.code == DEF_SUCCESS)
							{
								if(command_id == LINE_COMMAND_LOG_THREAD_DOWNLOAD_LOG_REQUEST || command_id == LINE_COMMAND_LOG_THREAD_DOWNLOAD_LOG_NO_PARSE_REQUEST)
								{
									if(log_height != line.parsed_logs_buffer.log_height)
									{
										std::string merged_data = "";
										char* save_data = NULL;

										log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Line_merge_logs()...");
										result = Line_merge_logs(&line.parsed_logs_buffer, room_id, max_logs, &merged_data);
										Util_log_add(log_num, result.string + result.error_description, result.code);
										if(result.code == DEF_SUCCESS)
											save_data = (char*)merged_data.c_str();
										else
											save_data = (char*)buffer;

										if(command_id == LINE_COMMAND_LOG_THREAD_DOWNLOAD_LOG_REQUEST)
										{
											line.room_data[line.selected_room_index].num_of_unread_logs = 0;
											//Request to parse logs.
											DEF_LINE_QUEUE_ADD_WITH_LOG(&line.log_thread_queue, LINE_COMMAND_LOG_THREAD_PARSE_LOG_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
										}
										else if(command_id == LINE_COMMAND_LOG_THREAD_DOWNLOAD_LOG_NO_PARSE_REQUEST)
										{
											//Don't parse logs, set room info only.
											line.room_data[line.selected_room_index].num_of_unread_logs = (line.parsed_logs_buffer.log_height - line.room_data[line.selected_room_index].log_height);
											line.room_data[line.selected_room_index].log_height = line.parsed_logs_buffer.log_height;
											line.room_data[line.selected_room_index].room_name = line.parsed_logs_buffer.room_name;
											line.room_data[line.selected_room_index].room_icon_url = line.parsed_logs_buffer.room_icon_url;

											Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_LOAD_LOGS);
										}

										//Room icon may be changed, reload it.
										DEF_LINE_QUEUE_ADD_WITH_LOG(&line.worker_thread_queue, LINE_COMMAND_WORKER_THREAD_LOAD_ICON_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);

										log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Line_save_logs()...");
										result = Line_save_logs(&line.parsed_logs_buffer, room_id, line.room_data[line.selected_room_index].num_of_unread_logs, save_data);
										Util_log_add(log_num, result.string + result.error_description, result.code);
									}
									else
										Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_LOAD_LOGS);
								}
								else if(command_id == LINE_COMMAND_LOG_THREAD_LOAD_LOG_REQUEST)
								{
									//Mark as read.
									result = Line_save_room_info(&line.parsed_logs_buffer, room_id, 0);
									Util_log_save(DEF_LINE_LOG_THREAD_STR, "Line_save_room_info()..." + result.string + result.error_description, result.code);

									//Request to parse logs.
									DEF_LINE_QUEUE_ADD_WITH_LOG(&line.log_thread_queue, LINE_COMMAND_LOG_THREAD_PARSE_LOG_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
								}
								else
									Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_LOAD_LOGS);
							}
							else
								Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_LOAD_LOGS);
						}
						else
							Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_LOAD_LOGS);

						Util_json_free(&json_data);
					}
					else
						Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_LOAD_LOGS);

					free(buffer);
					buffer = NULL;

					if(result.code != DEF_SUCCESS)
					{
						//Only display error message if we were downloading logs.
						//Because it is possible to try to load the logs
						//for the room that we haven't downloaded the logs.
						if((command_id == LINE_COMMAND_LOG_THREAD_DOWNLOAD_LOG_REQUEST
						|| command_id == LINE_COMMAND_LOG_THREAD_DOWNLOAD_LOG_NO_PARSE_REQUEST))
						{
							Util_err_set_error_message(result.string, result.error_description, DEF_LINE_LOG_THREAD_STR, result.code);
							Util_err_set_error_show_flag(true);
						}
						if(command_id == LINE_COMMAND_LOG_THREAD_LOAD_LOG_REQUEST)
							line.room_data[line.selected_room_index].log_height = 0;//Reset log height as we couldn't load logs.
					}

					break;
				}

				case LINE_COMMAND_LOG_THREAD_PARSE_LOG_REQUEST:
				{
					int room_index = line.selected_room_index;
					int32_t previous_offset = 0;
					std::string image_url_start = "<image_url>";
					std::string image_url_end = "</image_url>";
					std::string video_url_start = "<video_url>";
					std::string video_url_end = "</video_url>";
					std::string audio_url_start = "<audio_url>";
					std::string audio_url_end = "</audio_url>";
					std::string file_url_start = "<file_url>";
					std::string file_url_end = "</file_url>";
					std::string sticker_start = "<sticker>";
					std::string sticker_end = "</sticker>";
					std::string room_id_start = "<id>";
					std::string room_id_end = "</id>";

					result.code = DEF_SUCCESS;
					result.string = "[Success] ";

					LightLock_Lock(&line.log_copy_string_lock);

					//Update room info.
					line.room_data[room_index].log_height = line.parsed_logs_buffer.log_height;
					line.room_data[room_index].num_of_unread_logs = 0;
					line.room_data[room_index].room_name = line.parsed_logs_buffer.room_name;
					line.room_data[room_index].room_icon_url = line.parsed_logs_buffer.room_icon_url;

					line.logs.num_of_logs = line.parsed_logs_buffer.num_of_logs;
					line.logs.log_height = line.parsed_logs_buffer.log_height;
					line.logs.room_name = line.parsed_logs_buffer.room_name;
					line.logs.room_icon_url = line.parsed_logs_buffer.room_icon_url;
					line.optimized_logs.num_of_lines = 0;

					for(int i = 0; i < 4000; i++)
					{
						line.logs.logs.text[i] = line.parsed_logs_buffer.logs.text[i];
						line.logs.logs.speaker[i] = line.parsed_logs_buffer.logs.speaker[i];
						line.logs.logs.time[i] = line.parsed_logs_buffer.logs.time[i];
						line.optimized_logs.start_pos[i] = 0;
					}

					for(int i = 0; i < 60000; i++)
					{
						line.optimized_logs.original_index[i] = UINT16_MAX;
						line.optimized_logs.draw_offset[i] = UINT32_MAX;
						line.optimized_logs.data[i] = "";
						line.optimized_logs.type[i] = LINE_MSG_TYPE_INVALID;
					}

					for (uint16_t i = 0; i < line.logs.num_of_logs; i++)
					{
						size_t start_pos = std::string::npos;
						size_t end_pos = std::string::npos;
						int text_index = 0;
						int text_length = 0;
						double total_width = 0;

						if(line.optimized_logs.num_of_lines >= 60000)
							goto too_many;

						if(line.optimized_logs.num_of_lines != 0)
						{
							previous_offset = line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines - 1];
							//Set message start position for copy and search.
							line.optimized_logs.start_pos[i] = -previous_offset;
						}

						//Set speaker data.
						line.optimized_logs.original_index[line.optimized_logs.num_of_lines] = i;
						line.optimized_logs.data[line.optimized_logs.num_of_lines] = line.logs.logs.speaker[i];
						line.optimized_logs.type[line.optimized_logs.num_of_lines] = LINE_MSG_TYPE_SPEAKER;
						line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines] = previous_offset + DEF_LINE_MSG_SPEAKER_OFFSET + line.text_interval;
						previous_offset = line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines];
						line.optimized_logs.num_of_lines++;

						if(line.optimized_logs.num_of_lines >= 60000)
							goto too_many;

						//Check if message type is image.
						start_pos = line.logs.logs.text[i].find(image_url_start);
						end_pos = line.logs.logs.text[i].find(image_url_end);
						if (start_pos != std::string::npos && end_pos != std::string::npos)
						{
							//Extract image URL and store it.
							start_pos += image_url_start.length();
							line.optimized_logs.original_index[line.optimized_logs.num_of_lines] = i;
							line.optimized_logs.data[line.optimized_logs.num_of_lines] = line.logs.logs.text[i].substr(start_pos, (end_pos - start_pos));
							line.optimized_logs.type[line.optimized_logs.num_of_lines] = LINE_MSG_TYPE_IMAGE;
							line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines] = previous_offset + DEF_LINE_MSG_IMAGE_OFFSET + line.text_interval;
							previous_offset = line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines];
							line.optimized_logs.num_of_lines++;

							goto set_time;
						}

						//Check if message type is sticker.
						start_pos = line.logs.logs.text[i].find(sticker_start);
						end_pos = line.logs.logs.text[i].find(sticker_end);
						if (start_pos != std::string::npos && end_pos != std::string::npos)
						{
							//Extract sticker ID and store it.
							start_pos += sticker_start.length();
							line.optimized_logs.original_index[line.optimized_logs.num_of_lines] = i;
							line.optimized_logs.data[line.optimized_logs.num_of_lines] = line.logs.logs.text[i].substr(start_pos, (end_pos - start_pos));
							line.optimized_logs.type[line.optimized_logs.num_of_lines] = LINE_MSG_TYPE_STICKER;
							line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines] = previous_offset + DEF_LINE_MSG_STICKER_OFFSET + line.text_interval;
							previous_offset = line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines];
							line.optimized_logs.num_of_lines++;

							goto set_time;
						}

						//Check if message type is audio.
						start_pos = line.logs.logs.text[i].find(audio_url_start);
						end_pos = line.logs.logs.text[i].find(audio_url_end);
						if (start_pos != std::string::npos && end_pos != std::string::npos)
						{
							//Extract audio URL and store it.
							start_pos += audio_url_start.length();
							line.optimized_logs.original_index[line.optimized_logs.num_of_lines] = i;
							line.optimized_logs.data[line.optimized_logs.num_of_lines] = line.logs.logs.text[i].substr(start_pos, (end_pos - start_pos));
							line.optimized_logs.type[line.optimized_logs.num_of_lines] = LINE_MSG_TYPE_AUDIO;
							line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines] = previous_offset + DEF_LINE_MSG_AUDIO_OFFSET + line.text_interval;
							previous_offset = line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines];
							line.optimized_logs.num_of_lines++;

							goto set_time;
						}

						//Check if message type is video.
						start_pos = line.logs.logs.text[i].find(video_url_start);
						end_pos = line.logs.logs.text[i].find(video_url_end);
						if (start_pos != std::string::npos && end_pos != std::string::npos)
						{
							//Extract video URL and store it.
							start_pos += video_url_start.length();
							line.optimized_logs.original_index[line.optimized_logs.num_of_lines] = i;
							line.optimized_logs.data[line.optimized_logs.num_of_lines] = line.logs.logs.text[i].substr(start_pos, (end_pos - start_pos));
							line.optimized_logs.type[line.optimized_logs.num_of_lines] = LINE_MSG_TYPE_VIDEO;
							line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines] = previous_offset + DEF_LINE_MSG_VIDEO_OFFSET + line.text_interval;
							previous_offset = line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines];
							line.optimized_logs.num_of_lines++;

							goto set_time;
						}

						//Check if message type is file.
						start_pos = line.logs.logs.text[i].find(file_url_start);
						end_pos = line.logs.logs.text[i].find(file_url_end);
						if (start_pos != std::string::npos && end_pos != std::string::npos)
						{
							//Extract file URL and store it.
							start_pos += file_url_start.length();
							line.optimized_logs.original_index[line.optimized_logs.num_of_lines] = i;
							line.optimized_logs.data[line.optimized_logs.num_of_lines] = line.logs.logs.text[i].substr(start_pos, (end_pos - start_pos));
							line.optimized_logs.type[line.optimized_logs.num_of_lines] = LINE_MSG_TYPE_FILE;
							line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines] = previous_offset + DEF_LINE_MSG_FILE_OFFSET + line.text_interval;
							previous_offset = line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines];
							line.optimized_logs.num_of_lines++;

							goto set_time;
						}

						//Check if message type is room ID.
						start_pos = line.logs.logs.text[i].find(room_id_start);
						end_pos = line.logs.logs.text[i].find(room_id_end);
						if (start_pos != std::string::npos && end_pos != std::string::npos)
						{
							//Extract room ID and store it.
							start_pos += room_id_start.length();
							line.optimized_logs.original_index[line.optimized_logs.num_of_lines] = i;
							line.optimized_logs.data[line.optimized_logs.num_of_lines] = line.logs.logs.text[i].substr(start_pos, (end_pos - start_pos));
							line.optimized_logs.type[line.optimized_logs.num_of_lines] = LINE_MSG_TYPE_ROOM_ID;
							line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines] = previous_offset + DEF_LINE_MSG_ROOM_ID_OFFSET + line.text_interval;
							previous_offset = line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines];
							line.optimized_logs.num_of_lines++;

							goto set_time;
						}

						//Message type must be text, wrap the text.
						while (true)
						{
							int remaining_length = Util_min((line.logs.logs.text[i].length() - (text_index + text_length)), 4);

							if(line.optimized_logs.num_of_lines >= 60000)
								goto too_many;

							//If we reach end of text, just copy it.
							if(remaining_length <= 0)
							{
								total_width = 0;
								line.optimized_logs.original_index[line.optimized_logs.num_of_lines] = i;
								line.optimized_logs.data[line.optimized_logs.num_of_lines] = line.logs.logs.text[i].substr(text_index, text_length);
								line.optimized_logs.type[line.optimized_logs.num_of_lines] = LINE_MSG_TYPE_TEXT;
								line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines] = previous_offset + DEF_LINE_MSG_TEXT_OFFSET + line.text_interval;
								previous_offset = line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines];
								line.optimized_logs.num_of_lines++;
								break;
							}
							else
							{
								int char_length = mblen(&line.logs.logs.text[i].c_str()[text_index + text_length], remaining_length);
								if (char_length > 0)
								{
									std::string one_char = line.logs.logs.text[i].substr(text_index + text_length, char_length);

									//If string is new line, immedietaly go to the new line.
									if(one_char == "\n")
									{
										line.optimized_logs.original_index[line.optimized_logs.num_of_lines] = i;
										line.optimized_logs.data[line.optimized_logs.num_of_lines] = line.logs.logs.text[i].substr(text_index, text_length);
										line.optimized_logs.type[line.optimized_logs.num_of_lines] = LINE_MSG_TYPE_TEXT;
										line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines] = previous_offset + DEF_LINE_MSG_TEXT_OFFSET + line.text_interval;
										previous_offset = line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines];
										line.optimized_logs.num_of_lines++;

										total_width = 0;
										text_index += text_length;
										text_index++;//Skip "\n".
										text_length = 0;
									}
									else
									{
										double width = 0;
										double height = 0;

										Draw_get_text_size(one_char, line.text_size, line.text_size, &width, &height);
										//If total text width is more than 300px, go to next line.
										if(total_width + width > 300)
										{
											line.optimized_logs.original_index[line.optimized_logs.num_of_lines] = i;
											line.optimized_logs.data[line.optimized_logs.num_of_lines] = line.logs.logs.text[i].substr(text_index, text_length);
											line.optimized_logs.type[line.optimized_logs.num_of_lines] = LINE_MSG_TYPE_TEXT;
											line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines] = previous_offset + DEF_LINE_MSG_TEXT_OFFSET + line.text_interval;
											previous_offset = line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines];
											line.optimized_logs.num_of_lines++;

											total_width = width;
											text_index += text_length;
											text_length = char_length;
										}
										else
										{
											total_width += width;
											text_length += char_length;
										}
									}
								}
								else
									text_length++;
							}
						}
						//Fall through.

						set_time:
						if(line.optimized_logs.num_of_lines >= 60000)
							goto too_many;

						//Set time data.
						line.optimized_logs.original_index[line.optimized_logs.num_of_lines] = i;
						line.optimized_logs.data[line.optimized_logs.num_of_lines] = line.logs.logs.time[i];
						line.optimized_logs.type[line.optimized_logs.num_of_lines] = LINE_MSG_TYPE_TIME;
						line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines] = previous_offset + DEF_LINE_MSG_TIME_OFFSET + line.text_interval;
						previous_offset = line.optimized_logs.draw_offset[line.optimized_logs.num_of_lines];
						line.optimized_logs.num_of_lines++;

						continue;

						too_many:
						result.code = DEF_ERR_OTHER;
						result.string = DEF_ERR_OTHER_STR;
						result.error_description = "[Error] Too many messages. ";
						break;
					}

					line.y_pos[LINE_STATE_MAIN_CHAT_ROOM].min_y = -(previous_offset - 240);
					line.y_pos[LINE_STATE_MAIN_CHAT_ROOM].y = line.y_pos[LINE_STATE_MAIN_CHAT_ROOM].min_y;

					LightLock_Unlock(&line.log_copy_string_lock);

					if(result.code != DEF_SUCCESS)
					{
						Util_err_set_error_message(result.string, result.error_description, DEF_LINE_LOG_THREAD_STR, result.code);
						Util_err_set_error_show_flag(true);
					}

					Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_LOAD_LOGS);

					break;
				}

				case LINE_COMMAND_LOG_THREAD_SEND_MESSAGE_REQUEST:
				case LINE_COMMAND_LOG_THREAD_SEND_STICKER_REQUEST:
				case LINE_COMMAND_LOG_THREAD_SEND_FILE_REQUEST:
				{
					bool send_file = false;
					u8* buffer = NULL;
					uint32_t log_height = line.room_data[line.selected_room_index].log_height;
					uint16_t max_logs = line.num_of_logs;
					uint16_t secret_offset = 0;
					int dled_size = 0;
					char secret[(DEF_LINE_SECRET_HASH_SIZE * 2) + 1] = { 0, };
					std::string send_data = "";
					std::string room_id = line.room_data[line.selected_room_index].id;
					Line_upload_file_info upload_file_info;

					for(uint16_t i = 0; i < DEF_LINE_SECRET_HASH_SIZE; i++)
						secret_offset += snprintf((secret + secret_offset), (sizeof(secret) - secret_offset), "%02X", line.secret_hash[i]);

					line.total_data_size = 0;
					line.uploaded_size = 0;

					if (command_id == LINE_COMMAND_LOG_THREAD_SEND_MESSAGE_REQUEST)
					{
						std::string encoded_text = "";

						encoded_text = Util_encode_to_escape(line.input_text);
						if (encoded_text.length() > 4000)
							encoded_text = encoded_text.substr(0, 3990);

						send_data = DEF_JSON_START_OBJECT;
						send_data += DEF_JSON_STRING_DATA_WITH_KEY("type", "send_text");
						send_data += DEF_JSON_STRING_DATA_WITH_KEY("id", room_id);
						send_data += DEF_JSON_STRING_DATA_WITH_KEY("message", encoded_text);
						send_data += DEF_JSON_NON_STRING_DATA_WITH_KEY("log_height", std::to_string(log_height));
						send_data += DEF_JSON_NON_STRING_DATA_WITH_KEY("logs", std::to_string(max_logs));
						send_data += DEF_JSON_NON_STRING_DATA_WITH_KEY("gas_ver", std::to_string(DEF_LINE_GAS_VER));
						send_data += DEF_JSON_STRING_DATA_WITH_KEY_WITHOUT_COMMA("secret", secret);
						send_data += DEF_JSON_END_OBJECT;
					}
					else if (command_id == LINE_COMMAND_LOG_THREAD_SEND_STICKER_REQUEST)
					{
						std::string package_id = "";

						if (line.selected_sticker_index >= 1 && line.selected_sticker_index <= 40)
							package_id = "11537";
						else if (line.selected_sticker_index >= 41 && line.selected_sticker_index <= 80)
							package_id = "11538";
						else if (line.selected_sticker_index >= 81 && line.selected_sticker_index <= 120)
							package_id = "11539";

						send_data = DEF_JSON_START_OBJECT;
						send_data += DEF_JSON_STRING_DATA_WITH_KEY("type", "send_sticker");
						send_data += DEF_JSON_STRING_DATA_WITH_KEY("id", room_id);
						send_data += DEF_JSON_NON_STRING_DATA_WITH_KEY("package_id", package_id);
						send_data += DEF_JSON_NON_STRING_DATA_WITH_KEY("sticker_id", std::to_string(line.sticker_list[line.selected_sticker_index]));
						send_data += DEF_JSON_NON_STRING_DATA_WITH_KEY("log_height", std::to_string(log_height));
						send_data += DEF_JSON_NON_STRING_DATA_WITH_KEY("logs", std::to_string(max_logs));
						send_data += DEF_JSON_NON_STRING_DATA_WITH_KEY("gas_ver", std::to_string(DEF_LINE_GAS_VER));
						send_data += DEF_JSON_STRING_DATA_WITH_KEY_WITHOUT_COMMA("secret", secret);
						send_data += DEF_JSON_END_OBJECT;
					}
					else if (command_id == LINE_COMMAND_LOG_THREAD_SEND_FILE_REQUEST)
					{
						u64 file_size = 0;
						int audio_tracks = 0;
						int video_tracks = 0;
						int subtitle_tracks = 0;
						double duration_ms = 0;
						std::string file_type = "file";

						result = Util_file_check_file_size(line.file_name, line.file_dir, &file_size);
						if(result.code == DEF_SUCCESS)
						{
							//todo abort if size is more than 50MB
							line.total_data_size = ((((4 * file_size) / 3) + 3) & ~3);

							//Check if file type is image.
							result = Util_image_decoder_check_file(line.file_dir + line.file_name);
							if(result.code == DEF_SUCCESS)
								file_type = "image";
							else
							{
								//Check if file type is video or audio.
								result = Util_decoder_open_file(line.file_dir + line.file_name, &audio_tracks, &video_tracks, &subtitle_tracks, 2);
								if(result.code == DEF_SUCCESS)
								{
									if(video_tracks > 0)
										file_type = "video";
									else if(audio_tracks > 0)
									{
										Audio_info audio_info;

										file_type = "audio";
										Util_audio_decoder_init(audio_tracks, 2);
										Util_audio_decoder_get_info(&audio_info, 0, 2);

										//In case of an error, set duration as 1ms because
										//setting 0ms will result in an error in line API.
										duration_ms = Util_max((audio_info.duration * 1000), 1);
									}
								}
								Util_decoder_close_file(2);
							}
						}

						send_file = true;

						upload_file_info.first = true;
						upload_file_info.end = false;
						upload_file_info.filename = line.file_name;
						upload_file_info.first_data = DEF_JSON_START_OBJECT;
						upload_file_info.first_data += DEF_JSON_STRING_DATA_WITH_KEY("type", "send_file");
						upload_file_info.first_data += DEF_JSON_STRING_DATA_WITH_KEY("id", room_id);
						upload_file_info.first_data += DEF_JSON_STRING_DATA_WITH_KEY("file_name", line.file_name);
						upload_file_info.first_data += DEF_JSON_STRING_DATA_WITH_KEY("file_type", file_type);
						//media_duration_ms is only used for audio, otherwise this field is ignored.
						upload_file_info.first_data += DEF_JSON_NON_STRING_DATA_WITH_KEY("media_duration_ms", std::to_string(duration_ms));
						upload_file_info.first_data += DEF_JSON_NON_STRING_DATA_WITH_KEY("log_height", std::to_string(log_height));
						upload_file_info.first_data += DEF_JSON_NON_STRING_DATA_WITH_KEY("logs", std::to_string(max_logs));
						upload_file_info.first_data += DEF_JSON_NON_STRING_DATA_WITH_KEY("gas_ver", std::to_string(DEF_LINE_GAS_VER));
						upload_file_info.first_data += DEF_JSON_STRING_DATA_WITH_KEY("secret", secret);
						upload_file_info.first_data += DEF_JSON_KEY("file_data") + "\"";
						upload_file_info.end_data = "\"";
						upload_file_info.end_data += DEF_JSON_END_OBJECT;
						upload_file_info.dir = line.file_dir;
						upload_file_info.offset = 0;
					}

					log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_curl_post_and_dl_data()...");
					if(send_file)
					{
						result = Util_curl_post_and_dl_data(line.gas_url, Line_upload_file_callback, (void*)&upload_file_info, &buffer, 1024 * 1024 * 5, &dled_size, &line.uploaded_size, true, 5);
						send_file = false;
					}
					else
						result = Util_curl_post_and_dl_data(line.gas_url, (u8*)send_data.c_str(), send_data.length(), &buffer, 1024 * 128, &dled_size, &line.uploaded_size, true, 5);

					Util_log_add(log_num, result.string, result.code);
					if (result.code == DEF_SUCCESS)
					{
						line.uploaded_size = line.total_data_size;

						Json_data json_data;

						//Parse the query result.
						log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_json_parse()...");
						result = Util_json_parse((char*)buffer, &json_data);
						Util_log_add(log_num, result.string + result.error_description, result.code);

						if(result.code == DEF_SUCCESS)
						{
							log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Line_parse_logs()...");
							result = Line_parse_logs(&json_data, &line.parsed_logs_buffer);
							Util_log_add(log_num, result.string + result.error_description, result.code);

							if(result.code == DEF_SUCCESS && log_height != line.parsed_logs_buffer.log_height)
							{
								std::string merged_data = "";
								char* save_data = NULL;

								log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Line_merge_logs()...");
								result = Line_merge_logs(&line.parsed_logs_buffer, room_id, max_logs, &merged_data);
								Util_log_add(log_num, result.string + result.error_description, result.code);
								if(result.code == DEF_SUCCESS)
									save_data = (char*)merged_data.c_str();
								else
									save_data = (char*)buffer;

								//Request to parse logs.
								DEF_LINE_QUEUE_ADD_WITH_LOG(&line.log_thread_queue, LINE_COMMAND_LOG_THREAD_PARSE_LOG_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);

								//Room icon may be changed, reload it.
								DEF_LINE_QUEUE_ADD_WITH_LOG(&line.worker_thread_queue, LINE_COMMAND_WORKER_THREAD_LOAD_ICON_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);

								log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Line_save_logs()...");
								result = Line_save_logs(&line.parsed_logs_buffer, room_id, 0, save_data);
								Util_log_add(log_num, result.string + result.error_description, result.code);
							}
						}

						Util_json_free(&json_data);
					}

					if(result.code != DEF_SUCCESS)
					{
						Util_err_set_error_message(result.string, result.error_description, DEF_LINE_LOG_THREAD_STR, result.code);
						Util_err_set_error_show_flag(true);
					}

					free(buffer);
					buffer = NULL;

					if (command_id == LINE_COMMAND_LOG_THREAD_SEND_MESSAGE_REQUEST)
					{
						line.input_text = "";
						Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_SEND_TEXT);
					}
					else if (command_id == LINE_COMMAND_LOG_THREAD_SEND_STICKER_REQUEST)
						Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_SEND_STICKER);
					else if (command_id == LINE_COMMAND_LOG_THREAD_SEND_FILE_REQUEST)
						Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_SEND_FILE);

					break;
				}

				default:
				{
					//Invalid command was received.
					char msg[64];
					snprintf(msg, sizeof(msg), "Invalid command (%08X) was received!!!!!", command_id);
					Util_log_save(DEF_LINE_LOG_THREAD_STR, msg);
					break;
				}
			}
		}

		free(command_data);
		command_data = NULL;

		while (line_thread_suspend)
			Util_sleep(DEF_INACTIVE_THREAD_SLEEP_TIME);

		while (line_thread_suspend)
			Util_sleep(DEF_INACTIVE_THREAD_SLEEP_TIME);
	}

	Util_log_save(DEF_LINE_LOG_THREAD_STR, "Thread exit.");
	threadExit(0);
}

void Line_init_thread(void* arg)
{
	Util_log_save(DEF_LINE_INIT_STR, "Thread started.");
	uint8_t* fs_buffer = NULL;
	uint32_t read_size = 0;
	int log_num = 0;
	std::string data[4] = { "", "", "", "", };
	std::string ids_room_info = "";
	Result_with_string result;

	line.main_state = LINE_STATE_MAIN_CHAT_ROOM_SELECTION;
	line.sub_state = DEF_LINE_STATE_SUB_NONE;
	line.tab_mode = LINE_TAB_MODE_SEND;
	line.num_of_room = 0;
	memset(line.secret_hash, 0x00, DEF_LINE_SECRET_HASH_SIZE);

	line_status = "Authing...";

	//Remove old version files if exist.
	Util_file_delete_file("auth", DEF_MAIN_DIR);
	Util_file_delete_file("script_auth", DEF_MAIN_DIR);

	log_num = Util_log_save(DEF_LINE_INIT_STR, "Util_file_load_from_file()...");
	result = Util_file_load_from_file("secret", DEF_MAIN_DIR, &fs_buffer, 0x2000, &read_size);
	Util_log_add(log_num, result.string, result.code);

	if (result.code == DEF_SUCCESS && sizeof(line.secret_hash) == read_size)
	{
		uint8_t empty_password = 0x00;
		uint8_t empty_password_hash[DEF_LINE_SECRET_HASH_SIZE] = { 0, };

		memcpy(line.secret_hash, fs_buffer, sizeof(line.secret_hash));

		//Check if password is not set.
		Line_generate_password_hash(&empty_password, sizeof(empty_password), empty_password_hash);
		if(memcmp(empty_password_hash, line.secret_hash, sizeof(line.secret_hash)) == 0)
		{
			//If so, skip password validation and continue.
		}
		else
		{
			//Password is set.
			line_type_password_request = true;
			while(line_type_password_request)
				Util_sleep(100000);
		}
	}
	else
	{
		line_set_password_request = true;
		while(line_set_password_request)
			Util_sleep(100000);
	}

	free(fs_buffer);
	fs_buffer = NULL;

	line_status += "\nInitializing variables...";
	Line_reset_id();
	Line_reset_msg();
	line.text_size = 0.66;
	line.text_interval = 5;
	line.num_of_logs = 150;
	line.y_pos[LINE_STATE_MAIN_CHAT_ROOM].min_y = 0;
	line.y_pos[LINE_STATE_MAIN_CHAT_ROOM].max_y = 240;
	line.y_pos[LINE_STATE_MAIN_CHAT_ROOM_SELECTION].min_y = -(35 * (DEF_LINE_MAX_ROOMS - 1));
	line.y_pos[LINE_STATE_MAIN_CHAT_ROOM_SELECTION].max_y = 0;

	line.add_new_id_button.c2d = var_square_image[0];
	line.change_gas_url_button.c2d = var_square_image[0];
	line.change_app_password_button.c2d = var_square_image[0];
	line.back_button.c2d = var_square_image[0];
	line.send_msg_button.c2d = var_square_image[0];
	line.send_sticker_button.c2d = var_square_image[0];
	line.send_file_button.c2d = var_square_image[0];
	line.send_success_button.c2d = var_square_image[0];
	line.dl_logs_button.c2d = var_square_image[0];
	line.max_logs_bar.c2d = var_square_image[0];
	line.copy_button.c2d = var_square_image[0];
	line.copy_select_down_button.c2d = var_square_image[0];
	line.copy_select_up_button.c2d = var_square_image[0];
	line.increase_interval_button.c2d = var_square_image[0];
	line.decrease_interval_button.c2d = var_square_image[0];
	line.increase_size_button.c2d = var_square_image[0];
	line.decrease_size_button.c2d = var_square_image[0];
	line.search_button.c2d = var_square_image[0];
	line.search_select_down_button.c2d = var_square_image[0];
	line.search_select_up_button.c2d = var_square_image[0];
	line.delete_room_button.c2d = var_square_image[0];
	line.yes_button.c2d = var_square_image[0];
	line.no_button.c2d = var_square_image[0];
	line.view_image_button.c2d = var_square_image[0];
	line.edit_msg_button.c2d = var_square_image[0];
	line.use_swkbd_button.c2d = var_square_image[0];
	line.use_camera_button.c2d = var_square_image[0];
	line.scroll_bar.c2d = var_square_image[0];
	for(int i = 0; i < 128; i++)
		line.room_button[i].c2d = var_square_image[0];
	for(int i = 0; i < 6; i++)
		line.menu_button[i].c2d = var_square_image[0];
	for(int i = 0; i < 32; i++)
		line.content[i].button.c2d = var_square_image[0];

	Util_add_watch(&line.add_new_id_button.selected);
	Util_add_watch(&line.change_gas_url_button.selected);
	Util_add_watch(&line.change_app_password_button.selected);
	Util_add_watch(&line.back_button.selected);
	Util_add_watch(&line.send_msg_button.selected);
	Util_add_watch(&line.send_sticker_button.selected);
	Util_add_watch(&line.send_file_button.selected);
	Util_add_watch(&line.send_success_button.selected);
	Util_add_watch(&line.dl_logs_button.selected);
	Util_add_watch(&line.max_logs_bar.selected);
	Util_add_watch(&line.copy_button.selected);
	Util_add_watch(&line.copy_select_down_button.selected);
	Util_add_watch(&line.copy_select_up_button.selected);
	Util_add_watch(&line.increase_interval_button.selected);
	Util_add_watch(&line.decrease_interval_button.selected);
	Util_add_watch(&line.increase_size_button.selected);
	Util_add_watch(&line.decrease_size_button.selected);
	Util_add_watch(&line.search_button.selected);
	Util_add_watch(&line.search_select_down_button.selected);
	Util_add_watch(&line.search_select_up_button.selected);
	Util_add_watch(&line.delete_room_button.selected);
	Util_add_watch(&line.yes_button.selected);
	Util_add_watch(&line.no_button.selected);
	Util_add_watch(&line.view_image_button.selected);
	Util_add_watch(&line.edit_msg_button.selected);
	Util_add_watch(&line.use_swkbd_button.selected);
	Util_add_watch(&line.use_camera_button.selected);
	Util_add_watch(&line.scroll_bar.selected);
	for(int i = 0; i < 128; i++)
	{
		Util_add_watch(&line.room_button[i].selected);
		Util_add_watch(&line.room_update_button[i].selected);
	}
	for(int i = 0; i < 6; i++)
		Util_add_watch(&line.menu_button[i].selected);
	for(int i = 0; i < 16; i++)
		Util_add_watch(&line.content[i].button.selected);

	Util_add_watch(&line.num_of_logs);
	Util_add_watch((int*)&line.tab_mode);
	Util_add_watch(&line.uploaded_size);
	for(uint8_t i = 0; i < (sizeof(line.y_pos) / sizeof(Line_y_pos)); i++)
		Util_add_watch(&line.y_pos[i].y);

	Util_add_watch((int*)&line.sub_state);

	result = Util_queue_create(&line.log_thread_queue, 50);
	Util_log_save(DEF_LINE_INIT_STR, "Util_queue_create()..." + result.string + result.error_description, result.code);

	result = Util_queue_create(&line.worker_thread_queue, 50);
	Util_log_save(DEF_LINE_INIT_STR, "Util_queue_create()..." + result.string + result.error_description, result.code);

	result = Util_queue_create(&line.main_thread_queue, 50);
	Util_log_save(DEF_LINE_INIT_STR, "Util_queue_create()..." + result.string + result.error_description, result.code);

	LightLock_Init(&line.log_copy_string_lock);
	LightLock_Init(&line.state_lock);

	//Make "IDs" room if it doesn't exist.
	if(Util_file_check_file_exist("IDs", DEF_MAIN_DIR + "to/").code != DEF_SUCCESS)
	{
		ids_room_info = DEF_JSON_START_OBJECT;
		ids_room_info += DEF_JSON_NON_STRING_DATA_WITH_KEY("log_height", "0");
		ids_room_info += DEF_JSON_NON_STRING_DATA_WITH_KEY("num_of_unread_logs", "0");
		ids_room_info += DEF_JSON_STRING_DATA_WITH_KEY("room_name", "IDs");
		ids_room_info += DEF_JSON_STRING_DATA_WITH_KEY("room_icon_url", "");
		ids_room_info += DEF_JSON_END_OBJECT;

		result = Util_file_save_to_file("IDs", DEF_MAIN_DIR + "to/", (u8*)ids_room_info.c_str(), ids_room_info.length(), true);
		Util_log_save(DEF_LINE_INIT_STR, "Util_file_save_to_file()..." + result.string + result.error_description, result.code);
	}

	line_status += "\nLoading settings...";
	log_num = Util_log_save(DEF_LINE_INIT_STR, "Util_file_load_from_file()...");
	result = Util_file_load_from_file("gas_url.txt", DEF_MAIN_DIR, &fs_buffer, 0x2000);
	Util_log_add(log_num, result.string, result.code);
	if (result.code == 0)
		line.gas_url = (char*)fs_buffer;
	else
		line.gas_url = "";

	free(fs_buffer);
	fs_buffer = NULL;

	log_num = Util_log_save(DEF_LINE_INIT_STR, "Util_file_load_from_file()...");
	result = Util_file_load_from_file("Line_setting.txt", DEF_MAIN_DIR, &fs_buffer, 0x2000);
	Util_log_add(log_num, result.string, result.code);
	if (result.code == 0)
	{
		result = Util_parse_file((char*)fs_buffer, 4, data);
		if(result.code == 0)
		{
			line.text_size = atof(data[0].c_str());
			line.text_interval = atof(data[1].c_str());
			line.num_of_logs = atoi(data[2].c_str());
			//data[3] is no longer used.
		}
	}
	free(fs_buffer);
	fs_buffer = NULL;

	if(line.text_size >= 3.0 || line.text_size <= 0.25)
		line.text_size = 0.66;
	if(line.text_interval > 30 || line.text_interval < 0)
		line.text_interval = 5;
	if(line.num_of_logs > 4000 || line.num_of_logs < 20)
		line.num_of_logs = 150;

	line_status += "\nStarting threads...";
	line_thread_run = true;

	line.worker_thread = threadCreate(Line_worker_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, -1, false);
	line.log_thread = threadCreate(Line_log_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, -1, false);

	//Send a load room informations and icons request.
	DEF_LINE_QUEUE_ADD_WITH_LOG(&line.worker_thread_queue, LINE_COMMAND_WORKER_THREAD_LOAD_ROOM_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
	DEF_LINE_QUEUE_ADD_WITH_LOG(&line.worker_thread_queue, LINE_COMMAND_WORKER_THREAD_LOAD_ICON_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);

	line_status += "\nLoading textures...";
	line.sticker_texture_index = Draw_get_free_sheet_num();
	log_num = Util_log_save(DEF_LINE_INIT_STR, "Draw_load_texture()...");
	result = Draw_load_texture("romfs:/gfx/draw/line/stickers.t3x", line.sticker_texture_index, line.sticker_images, 0, 121);
	Util_log_add(log_num, result.string, result.code);

	line.update_button_texture_index = Draw_get_free_sheet_num();
	log_num = Util_log_save(DEF_LINE_INIT_STR, "Draw_load_texture()...");
	result = Draw_load_texture("romfs:/gfx/draw/line/update.t3x", line.update_button_texture_index, &line.room_update_button[0].c2d, 0, 1);
	Util_log_add(log_num, result.string, result.code);

	for(int i = 1; i < 128; i++)
		line.room_update_button[i].c2d = line.room_update_button[0].c2d;

	line_already_init = true;

	Util_log_save(DEF_LINE_INIT_STR, "Thread exit.");
	threadExit(0);
}

void Line_exit_thread(void* arg)
{
	Util_log_save(DEF_LINE_EXIT_STR, "Thread started.");
	int log_num;
	std::string data = "<0>" + std::to_string(line.text_size) + "</0><1>" + std::to_string(line.text_interval) + "</1><2>" + std::to_string(line.num_of_logs) + "</2><3>0</3>";
	Result_with_string result;

	line_thread_suspend = false;
	line_thread_run = false;

	line_status = "Saving settings...";
	log_num = Util_log_save(DEF_LINE_EXIT_STR, "Util_file_save_to_file()...");
	result = Util_file_save_to_file("Line_setting.txt", DEF_MAIN_DIR, (u8*)data.c_str(), data.length(), true);
	Util_log_add(log_num, result.string, result.code);

	line_status += "\nExiting threads...";
	Util_log_save(DEF_LINE_EXIT_STR, "threadJoin()...", threadJoin(line.log_thread, DEF_THREAD_WAIT_TIME));

	line_status += ".";
	Util_log_save(DEF_LINE_EXIT_STR, "threadJoin()...", threadJoin(line.worker_thread, DEF_THREAD_WAIT_TIME));

	line_status += "\nCleaning up...";
	threadFree(line.log_thread);
	threadFree(line.worker_thread);

	for (int i = 0; i < 128; i++)
	{
		Draw_texture_free(&line.icon[i]);
		line.room_update_button[i].c2d.tex = NULL;
	}
	for (int i = 0; i < 121; i++)
		line.sticker_images[i].tex = NULL;

	Draw_free_texture(line.sticker_texture_index);
	Draw_free_texture(line.update_button_texture_index);

	Util_remove_watch(&line.add_new_id_button.selected);
	Util_remove_watch(&line.change_gas_url_button.selected);
	Util_remove_watch(&line.change_app_password_button.selected);
	Util_remove_watch(&line.back_button.selected);
	Util_remove_watch(&line.send_msg_button.selected);
	Util_remove_watch(&line.send_sticker_button.selected);
	Util_remove_watch(&line.send_file_button.selected);
	Util_remove_watch(&line.send_success_button.selected);
	Util_remove_watch(&line.dl_logs_button.selected);
	Util_remove_watch(&line.max_logs_bar.selected);
	Util_remove_watch(&line.copy_button.selected);
	Util_remove_watch(&line.copy_select_down_button.selected);
	Util_remove_watch(&line.copy_select_up_button.selected);
	Util_remove_watch(&line.increase_interval_button.selected);
	Util_remove_watch(&line.decrease_interval_button.selected);
	Util_remove_watch(&line.increase_size_button.selected);
	Util_remove_watch(&line.decrease_size_button.selected);
	Util_remove_watch(&line.search_button.selected);
	Util_remove_watch(&line.search_select_down_button.selected);
	Util_remove_watch(&line.search_select_up_button.selected);
	Util_remove_watch(&line.delete_room_button.selected);
	Util_remove_watch(&line.yes_button.selected);
	Util_remove_watch(&line.no_button.selected);
	Util_remove_watch(&line.view_image_button.selected);
	Util_remove_watch(&line.edit_msg_button.selected);
	Util_remove_watch(&line.use_swkbd_button.selected);
	Util_remove_watch(&line.use_camera_button.selected);
	Util_remove_watch(&line.scroll_bar.selected);
	for(int i = 0; i < 128; i++)
	{
		Util_remove_watch(&line.room_button[i].selected);
		Util_remove_watch(&line.room_update_button[i].selected);
	}
	for(int i = 0; i < 6; i++)
		Util_remove_watch(&line.menu_button[i].selected);
	for(int i = 0; i < 16; i++)
		Util_remove_watch(&line.content[i].button.selected);

	Util_remove_watch(&line.num_of_logs);
	Util_remove_watch((int*)&line.tab_mode);
	Util_remove_watch(&line.uploaded_size);
	for(uint8_t i = 0; i < (sizeof(line.y_pos) / sizeof(Line_y_pos)); i++)
		Util_remove_watch(&line.y_pos[i].y);

	Util_remove_watch((int*)&line.sub_state);

	Util_queue_delete(&line.log_thread_queue);
	Util_queue_delete(&line.worker_thread_queue);
	Util_queue_delete(&line.main_thread_queue);

	line_already_init = false;

	Util_log_save(DEF_LINE_EXIT_STR, "Thread exit.");
	threadExit(0);
}

void Line_init(bool draw)
{
	Util_log_save(DEF_LINE_INIT_STR, "Initializing...");
	int color = DEF_DRAW_BLACK;
	int back_color = DEF_DRAW_WHITE;
	std::string input_string[2] = { " ", "", };
	Result_with_string result;

	Util_add_watch(&line_status);
	line_status = "";

	if((var_model == CFG_MODEL_N2DSXL || var_model == CFG_MODEL_N3DSXL || var_model == CFG_MODEL_N3DS) && var_core_2_available)
		line_init_thread = threadCreate(Line_init_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 2, false);
	else
	{
		APT_SetAppCpuTimeLimit(80);
		line_init_thread = threadCreate(Line_init_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 1, false);
	}

	while(!line_already_init)
	{
		if(draw)
		{
			if (var_night_mode)
			{
				color = DEF_DRAW_WHITE;
				back_color = DEF_DRAW_BLACK;
			}

			if(Util_is_watch_changed() || var_need_reflesh || !var_eco_mode)
			{
				var_need_reflesh = false;
				Draw_frame_ready();
				Draw_screen_ready(SCREEN_TOP_LEFT, back_color);
				Draw_top_ui();
				if(var_monitor_cpu_usage)
					Draw_cpu_usage_info();

				Draw(line_status, 0, 20, 0.65, 0.65, color);

				Draw_apply_draw();
			}
			else
				gspWaitForVBlank();
		}
		else
			Util_sleep(20000);

		if(line_set_password_request)
		{
			while(true)
			{
				Util_swkbd_init(SWKBD_TYPE_QWERTY, SWKBD_ANYTHING, 1, 512, "パスワードを入力 / Type password here.", "");
				Util_swkbd_launch(&input_string[0]);
				Util_swkbd_exit();

				Util_swkbd_init(SWKBD_TYPE_QWERTY, SWKBD_ANYTHING, 1, 512, "パスワードを入力(再度) / Type password here.(again)", "");
				Util_swkbd_launch(&input_string[1]);
				Util_swkbd_exit();

				if (input_string[0] == input_string[1])
				{
					uint8_t hash[DEF_LINE_SECRET_HASH_SIZE] = { 0, };

					//Also include NULL terminator.
					result.code = Line_generate_password_hash((uint8_t*)input_string[0].c_str(), (input_string[0].length() + 1), hash);
					if(result.code == DEF_SUCCESS)
					{
						result = Util_file_save_to_file("secret", DEF_MAIN_DIR, hash, sizeof(hash), true);
						if(result.code == DEF_SUCCESS)
							memcpy(line.secret_hash, hash, sizeof(line.secret_hash));
						else
							Util_log_save(DEF_LINE_INIT_STR, "Util_file_save_to_file()...", result.code);
					}
					else
						Util_log_save(DEF_LINE_INIT_STR, "Line_generate_password_hash()...", result.code);

					line_set_password_request = false;
					break;
				}
			}
		}

		if(line_type_password_request)
		{
			bool is_auth_success = false;

			for (int i = 0; i < 3; i++)
			{
				uint8_t hash[DEF_LINE_SECRET_HASH_SIZE] = { 0, };

				Util_swkbd_init(SWKBD_TYPE_QWERTY, SWKBD_ANYTHING, 1, 512, "パスワードを入力 / Type password here.", "");
				Util_swkbd_launch(&input_string[0]);
				Util_swkbd_exit();

				result.code = Line_generate_password_hash((uint8_t*)input_string[0].c_str(), (input_string[0].length() + 1), hash);

				if(result.code == DEF_SUCCESS && memcmp(line.secret_hash, hash, sizeof(line.secret_hash)) == 0)
				{
					is_auth_success = true;
					Util_log_save(DEF_LINE_INIT_STR, "Password is correct");
					break;
				}
				else
					Util_log_save(DEF_LINE_INIT_STR, "Password is incorrect");
			}

			if (!is_auth_success)
			{
				Util_log_save(DEF_LINE_INIT_STR, "Auth failed, rebooting...");
				APT_HardwareResetAsync();
				for(;;)
					Util_sleep(10000);
			}

			line_type_password_request = false;
		}
	}

	if(!(var_model == CFG_MODEL_N2DSXL || var_model == CFG_MODEL_N3DSXL || var_model == CFG_MODEL_N3DS) || !var_core_2_available)
		APT_SetAppCpuTimeLimit(10);

	Util_log_save(DEF_LINE_INIT_STR, "threadJoin()...", threadJoin(line_init_thread, DEF_THREAD_WAIT_TIME));
	threadFree(line_init_thread);
	Line_resume();

	Util_log_save(DEF_LINE_INIT_STR, "Initialized.");
}

void Line_exit(bool draw)
{
	Util_log_save(DEF_LINE_EXIT_STR, "Exiting...");

	int color = DEF_DRAW_BLACK;
	int back_color = DEF_DRAW_WHITE;

	line_status = "";
	line_exit_thread = threadCreate(Line_exit_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 1, false);

	while(line_already_init)
	{
		if(draw)
		{
			if (var_night_mode)
			{
				color = DEF_DRAW_WHITE;
				back_color = DEF_DRAW_BLACK;
			}

			if(Util_is_watch_changed() || var_need_reflesh || !var_eco_mode)
			{
				var_need_reflesh = false;
				Draw_frame_ready();
				Draw_screen_ready(SCREEN_TOP_LEFT, back_color);
				Draw_top_ui();
				if(var_monitor_cpu_usage)
					Draw_cpu_usage_info();

				Draw(line_status, 0, 20, 0.65, 0.65, color);

				Draw_apply_draw();
			}
			else
				gspWaitForVBlank();
		}
		else
			Util_sleep(20000);
	}

	Util_log_save(DEF_LINE_EXIT_STR, "threadJoin()...", threadJoin(line_exit_thread, DEF_THREAD_WAIT_TIME));
	threadFree(line_exit_thread);
	Util_remove_watch(&line_status);
	var_need_reflesh = true;

	Util_log_save(DEF_LINE_EXIT_STR, "Exited.");
}

void Line_main(void)
{
	int color = DEF_DRAW_BLACK;
	int back_color = DEF_DRAW_WHITE;
	int weak_color = DEF_DRAW_WEAK_BLACK;
	int color_cache = color;
	double pos_x = 0;
	double pos_y = 0;
	double scroll_bar_y_pos = 0;
	void* command_data = NULL;
	std::string status = "";
	Result_with_string result;
	Line_command command_id = LINE_COMMAND_INVALID;
	Line_main_state main_state = LINE_STATE_MAIN_INVALID;
	Line_sub_state sub_state = DEF_LINE_STATE_SUB_NONE;
	Line_y_pos* y_pos = NULL;

	LightLock_Lock(&line.state_lock);
	main_state = line.main_state;
	sub_state = line.sub_state;
	LightLock_Unlock(&line.state_lock);

	y_pos = &line.y_pos[main_state];
	if((y_pos->max_y - y_pos->y) == 0)
		scroll_bar_y_pos = 0;
	else
		scroll_bar_y_pos = (190.0 / ((abs(y_pos->min_y) + abs(y_pos->max_y)) / (y_pos->max_y - y_pos->y)));

	if (var_night_mode)
	{
		color = DEF_DRAW_WHITE;
		back_color = DEF_DRAW_BLACK;
		weak_color = DEF_DRAW_WEAK_WHITE;
	}

	if(Util_is_watch_changed() || var_need_reflesh || !var_eco_mode)
	{
		var_need_reflesh = false;
		Draw_frame_ready();

		if(var_turn_on_top_lcd)
		{
			Draw_screen_ready(SCREEN_TOP_LEFT, back_color);

			if(Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_DOWNLOAD_FILE))
				Draw(line_msg[56] + std::to_string(line.dled_content_size / 1024.0 / 1024.0).substr(0, 4) + "MB", 0, 20, 0.5, 0.5, color);

			if (main_state == LINE_STATE_MAIN_CHAT_ROOM)
				Line_draw_message(color, true, y_pos);
			if(Util_log_query_log_show_flag())
				Util_log_draw();

			Draw_top_ui();

			if(var_monitor_cpu_usage)
				Draw_cpu_usage_info();

			if(Draw_is_3d_mode())
			{
				Draw_screen_ready(SCREEN_TOP_RIGHT, back_color);

				if(Util_log_query_log_show_flag())
					Util_log_draw();

				Draw_top_ui();

				if(var_monitor_cpu_usage)
					Draw_cpu_usage_info();
			}
		}

		if(var_turn_on_bottom_lcd)
		{
			Draw_screen_ready(SCREEN_BOTTOM, back_color);

			if (main_state == LINE_STATE_MAIN_CHAT_ROOM_SELECTION)
			{
				if(Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_LOAD_LOGS))
					color_cache = weak_color;
				else
					color_cache = color;

				for (int i = 0; i < 128; i++)
				{
					if ((y_pos->y + (i * 35.0)) >= 165)
						break;
					else if ((y_pos->y + (i * 35.0)) <= -60)
					{
					}
					else
					{
						Draw_texture(&line.room_button[i], (line.room_data[i].num_of_unread_logs > 0 || line.room_button[i].selected) ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA,
						0, y_pos->y + (i * 35), 230, 30);
						if(line.room_data[i].is_icon_available)
							Draw_texture(&line.icon[i], 0, 0 + y_pos->y + (i * 35), 30, 30);

						Draw_texture(&line.room_update_button[i], line.room_update_button[i].selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_BLACK,
						230, y_pos->y + (i * 35.0), 30, 30);
						Draw(line.room_data[i].room_name, 35, 0 + y_pos->y + (i * 35), 0.5, 0.5, color_cache);
						Draw(line.room_data[i].id, 35, 20 + y_pos->y + (i * 35), 0.325, 0.325, color_cache);
						if(line.room_data[i].num_of_unread_logs > 0)
							Draw(std::to_string(line.room_data[i].num_of_unread_logs), 200, y_pos->y + (i * 35), 0.5, 0.5, DEF_DRAW_RED);
					}
				}

				Draw_texture(var_square_image[0], back_color, 0, 165, 320, 60);
				Draw_texture(var_square_image[0], DEF_DRAW_WEAK_YELLOW, 10, 165, 300, 60);

				if (Line_has_sub_state(sub_state, (DEF_LINE_STATE_SUB_LOAD_LOGS | DEF_LINE_STATE_SUB_GET_SCRIPT_URL)))
					Draw(line_msg[42], 40, 180, 0.5, 0.5, color);
				else
				{
					//Change GAS URL button.
					Draw(line_msg[19], 20, 170, 0.42, 0.42, color, X_ALIGN_LEFT, Y_ALIGN_CENTER, 280, 20, BACKGROUND_ENTIRE_BOX,
					&line.change_gas_url_button, line.change_gas_url_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Add new ID button.
					Draw(line_msg[18], 20, 200, 0.42, 0.42, color, X_ALIGN_LEFT, Y_ALIGN_CENTER, 135, 20, BACKGROUND_ENTIRE_BOX,
					&line.add_new_id_button, line.add_new_id_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Change app password button.
					Draw(line_msg[22], 165, 200, 0.42, 0.42, color, X_ALIGN_LEFT, Y_ALIGN_CENTER, 135, 20, BACKGROUND_ENTIRE_BOX,
					&line.change_app_password_button, line.change_app_password_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
				}

				if(Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_SELECTION_GAS_URL))
				{
					Draw_texture(var_square_image[0], DEF_DRAW_GREEN, 40, 75, 240, 90);

					//Use camera button.
					Draw("camera (A)", 50, 80, 0.42, 0.42, color, X_ALIGN_LEFT, Y_ALIGN_CENTER, 220, 20, BACKGROUND_ENTIRE_BOX,
					&line.use_camera_button, line.use_camera_button.selected ? DEF_DRAW_YELLOW : DEF_DRAW_WEAK_YELLOW);

					//Use swkbd button.
					Draw("keyboard (X)", 50, 110, 0.42, 0.42, color, X_ALIGN_LEFT, Y_ALIGN_CENTER, 220, 20, BACKGROUND_ENTIRE_BOX,
					&line.use_swkbd_button, line.use_swkbd_button.selected ? DEF_DRAW_YELLOW : DEF_DRAW_WEAK_YELLOW);

					//Back (cancel) button.
					Draw(line_msg[43], 50, 140, 0.42, 0.42, color, X_ALIGN_LEFT, Y_ALIGN_CENTER, 220, 20, BACKGROUND_ENTIRE_BOX,
					&line.back_button, line.back_button.selected ? DEF_DRAW_YELLOW : DEF_DRAW_WEAK_YELLOW);
				}

			}
			else if(main_state == LINE_STATE_MAIN_CHAT_ROOM)
			{
				Line_draw_message(color, false, y_pos);

				Draw_texture(var_square_image[0], back_color, 0, 135, 320, 85);

				status = "ID = " + line.room_data[line.selected_room_index].id + "\n" + std::to_string(line.logs.num_of_logs) + line_msg[0] + std::to_string(line.optimized_logs.num_of_lines) + line_msg[1];

				Draw_texture(&line.icon[line.selected_room_index], 10, 135, 32, 32);
				Draw(status, 45, 135, 0.35, 0.35, color);
				Draw(line.room_data[line.selected_room_index].room_name, 45, 155, 0.475, 0.475, color);

				if(Line_has_sub_state(sub_state, (DEF_LINE_STATE_SUB_LOAD_LOGS | DEF_LINE_STATE_SUB_SEND_MESSAGE)))
					color_cache = weak_color;

				//Back button.
				Draw(line_msg[43], 260, 135, 0.45, 0.45, color_cache, X_ALIGN_CENTER, Y_ALIGN_CENTER, 40, 23,
				BACKGROUND_ENTIRE_BOX, &line.back_button, line.back_button.selected ? DEF_DRAW_YELLOW : DEF_DRAW_WEAK_YELLOW);

				Draw(DEF_LINE_VER, 260, 155, 0.45, 0.45, DEF_DRAW_GREEN);
				Draw_texture(var_square_image[0], DEF_DRAW_WEAK_AQUA, 10, 170, 300, 60);

				//Menu buttons.
				Draw(line_msg[2], 10, 170, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 50, 10, BACKGROUND_ENTIRE_BOX,
				&line.menu_button[0], (line.menu_button[0].selected || line.tab_mode == LINE_TAB_MODE_SEND) ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_YELLOW);

				Draw(line_msg[35], 60, 170, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 50, 10, BACKGROUND_ENTIRE_BOX,
				&line.menu_button[1], (line.menu_button[1].selected || line.tab_mode == LINE_TAB_MODE_RECEIVE) ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_RED);

				Draw(line_msg[3], 110, 170, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 50, 10, BACKGROUND_ENTIRE_BOX,
				&line.menu_button[2], (line.menu_button[2].selected || line.tab_mode == LINE_TAB_MODE_COPY) ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_YELLOW);

				Draw(line_msg[4], 160, 170, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 50, 10, BACKGROUND_ENTIRE_BOX,
				&line.menu_button[3], (line.menu_button[3].selected || line.tab_mode == LINE_TAB_MODE_SETTINGS) ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_RED);

				Draw(line_msg[51], 210, 170, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 50, 10, BACKGROUND_ENTIRE_BOX,
				&line.menu_button[4], (line.menu_button[4].selected || line.tab_mode == LINE_TAB_MODE_SEARCH) ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_YELLOW);

				Draw(line_msg[5], 260, 170, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 50, 10, BACKGROUND_ENTIRE_BOX,
				&line.menu_button[5], (line.menu_button[5].selected || line.tab_mode == LINE_TAB_MODE_ADVANCED) ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_RED);

				if (line.tab_mode == LINE_TAB_MODE_SEND && Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_SEND_MESSAGE))
				{
					if (Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_SEND_FILE))
					{
						if(line.total_data_size != 0)
							Draw_texture(var_square_image[0], DEF_DRAW_YELLOW, 20, 205, (280.0 / line.total_data_size) * line.uploaded_size, 13);

						Draw(std::to_string(line.uploaded_size / 1024.0 / 1024.0).substr(0, 4) + "/" + std::to_string(line.total_data_size / 1024.0 / 1024.0).substr(0, 4) + "MB", 120, 205, 0.45, 0.45, color);
					}

					if (Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_SEND_TEXT))
						Draw(line_msg[36], 22.5, 185, 0.45, 0.45, color);
					else if(Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_SEND_STICKER))
						Draw(line_msg[37], 22.5, 185, 0.45, 0.45, color);
					else if(Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_SEND_FILE))
						Draw(line_msg[38], 22.5, 185, 0.45, 0.45, color);
				}
				else if (line.tab_mode == LINE_TAB_MODE_SEND)
				{
					//Send a message button.
					Draw(line_msg[6], 20, 185, 0.45, 0.45, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 280, 13,
					BACKGROUND_ENTIRE_BOX, &line.send_msg_button, line.send_msg_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Send a sticker button.
					Draw(line_msg[24], 20, 205, 0.45, 0.45, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 130, 13,
					BACKGROUND_ENTIRE_BOX, &line.send_sticker_button, line.send_sticker_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Send a file button.
					Draw(line_msg[34], 170, 205, 0.45, 0.45, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 130, 13,
					BACKGROUND_ENTIRE_BOX, &line.send_file_button, line.send_file_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
				}
				else if (line.tab_mode == LINE_TAB_MODE_RECEIVE)
				{
					if(Line_has_sub_state(sub_state, (DEF_LINE_STATE_SUB_LOAD_LOGS | DEF_LINE_STATE_SUB_SEND_MESSAGE)))
						color_cache = weak_color;
					else
						color_cache = color;

					//DL logs button.
					Draw(line_msg[7], 20, 185, 0.45, 0.45, color_cache, X_ALIGN_CENTER, Y_ALIGN_CENTER, 280, 13,
					BACKGROUND_ENTIRE_BOX, &line.dl_logs_button, line.dl_logs_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Max logs bar.
					Draw_texture(var_square_image[0], DEF_DRAW_WEAK_AQUA, 100, 210, 200, 5);
					Draw_texture(&line.max_logs_bar, line.max_logs_bar.selected ? DEF_DRAW_RED : DEF_DRAW_WEAK_RED, (line.num_of_logs / 20) + 99, 205, 7, 13);
					Draw(line_msg[50] + std::to_string(line.num_of_logs), 20, 205, 0.35, 0.35, color_cache);
				}
				else if (line.tab_mode == LINE_TAB_MODE_COPY)
				{
					Draw(std::to_string(line.copy_selected_index) + "/" + std::to_string(line.logs.num_of_logs - 1), 20, 190, 0.5, 0.5, color);

					//Copy button.
					Draw(line_msg[11], 90, 185, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 30,
					BACKGROUND_ENTIRE_BOX, &line.copy_button, line.copy_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Select down button.
					Draw(line_msg[12], 160, 185, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 30,
					BACKGROUND_ENTIRE_BOX, &line.copy_select_down_button, line.copy_select_down_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Select up button.
					Draw(line_msg[13], 230, 185, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 30,
					BACKGROUND_ENTIRE_BOX, &line.copy_select_up_button, line.copy_select_up_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
				}
				else if (line.tab_mode == LINE_TAB_MODE_SETTINGS)
				{
					//Increase font interval button.
					Draw(line_msg[14], 20, 185, 0.375, 0.375, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 30,
					BACKGROUND_ENTIRE_BOX, &line.increase_interval_button, line.increase_interval_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Decrease font interval button.
					Draw(line_msg[15], 90, 185, 0.375, 0.375, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 30,
					BACKGROUND_ENTIRE_BOX, &line.decrease_interval_button, line.decrease_interval_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Decrease font size button.
					Draw(line_msg[16], 170, 185, 0.375, 0.375, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 30,
					BACKGROUND_ENTIRE_BOX, &line.decrease_size_button, line.decrease_size_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Increase font size button.
					Draw(line_msg[17], 240, 185, 0.375, 0.375, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 30,
					BACKGROUND_ENTIRE_BOX, &line.increase_size_button, line.increase_size_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
				}
				else if (line.tab_mode == LINE_TAB_MODE_SEARCH)
				{
					Draw(std::to_string(line.search_selected_index) + "/" + std::to_string(line.search_found_items), 20, 190, 0.5, 0.5, color);

					//Search button.
					Draw(line_msg[52], 90, 185, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 30,
					BACKGROUND_ENTIRE_BOX, &line.search_button, line.search_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Select down button.
					Draw(line_msg[12], 160, 185, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 30,
					BACKGROUND_ENTIRE_BOX, &line.search_select_down_button, line.search_select_down_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Select up button.
					Draw(line_msg[13], 230, 185, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 30,
					BACKGROUND_ENTIRE_BOX, &line.search_select_up_button, line.search_select_up_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
				}
				else if (line.tab_mode == LINE_TAB_MODE_ADVANCED)
				{
					//Delete room button.
					Draw(line_msg[44], 20, 185, 0.45, 0.45, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 280, 30,
					BACKGROUND_ENTIRE_BOX, &line.delete_room_button, line.delete_room_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
				}
				else if (line.tab_mode == LINE_TAB_MODE_STICKER)
				{
					Draw_texture(var_square_image[0], DEF_DRAW_AQUA, 10, 140, 300, 90);
					Draw_texture(var_square_image[0], DEF_DRAW_WEAK_YELLOW, 290, 120, 20, 20);
					Draw("X", 292.5, 120, 0.7, 0.7, DEF_DRAW_RED);

					for (int i = 0; i < 5; i++)
					{
						Draw_texture(var_square_image[0], DEF_DRAW_WEAK_YELLOW, 10 + (i * 60), 140, 30, 10);
						Draw_texture(var_square_image[0], DEF_DRAW_WEAK_RED, 40 + (i * 60), 140, 30, 10);
					}
					Draw_texture(var_square_image[0], DEF_DRAW_BLUE, 10 + (line.selected_sticker_tab_index * 30), 140, 30, 10);

					pos_x = 20;
					pos_y = 150;
					for (int i = 1 + (line.selected_sticker_tab_index * 12); i < 13 + (line.selected_sticker_tab_index * 12); i++)
					{
						Draw_texture(var_square_image[0], DEF_DRAW_WEAK_BLUE, pos_x, pos_y, 30, 30);
						Draw_texture(line.sticker_images[i], pos_x, pos_y, 30, 30);
						pos_x += 50;
						if (pos_x > 271)
						{
							pos_x = 20;
							pos_y += 40;
						}
					}
				}

				if (Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT))
				{
					if ((Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_LOAD_LOGS))
					&& Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_DELETE_ROOM))
						color_cache = weak_color;
					else
						color_cache = color;

					Draw_texture(var_square_image[0], DEF_DRAW_BLUE, 10, 110, 300, 110);

					//Yes button.
					Draw(line_msg[20], 30, 200, 0.45, 0.45, color_cache, X_ALIGN_CENTER, Y_ALIGN_CENTER, 70, 15,
					BACKGROUND_ENTIRE_BOX, &line.yes_button, line.yes_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//No button.
					Draw(line_msg[21], 120, 200, 0.45, 0.45, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 70, 15,
					BACKGROUND_ENTIRE_BOX, &line.no_button, line.no_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					if (Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_MESSAGE))
					{
						//Edit message button.
						Draw(line_msg[47], 210, 200, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 70, 15,
						BACKGROUND_ENTIRE_BOX, &line.edit_msg_button, line.edit_msg_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
					}
					else if (Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_FILE))
					{
						//View image button.
						Draw(line_msg[32], 210, 200, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 70, 15,
						BACKGROUND_ENTIRE_BOX, &line.view_image_button, line.view_image_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
					}

					Draw_texture(&line.icon[line.selected_room_index], 10, 110, 30, 30);
					Draw(line.logs.room_name, 45, 110, 0.475, 0.475, DEF_DRAW_RED);
					if (Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_MESSAGE))
					{
						Draw(line_msg[25], 45, 120, 0.55, 0.55, DEF_DRAW_WHITE);
						Draw(line.input_text, 45, 135, 0.45, 0.45, DEF_DRAW_WHITE);
					}
					else if (Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_STICKER))
					{
						Draw(line_msg[26], 45, 120, 0.55, 0.55, DEF_DRAW_WHITE);
						Draw_texture(line.sticker_images[line.selected_sticker_index], 45, 135, 60, 60);
					}
					else if (Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_FILE))
					{
						Draw(line_msg[29], 45, 120, 0.55, 0.55, DEF_DRAW_WHITE);
						Draw(line_msg[30] + line.file_dir, 45, 140, 0.4, 0.4, DEF_DRAW_WHITE);
						Draw(line_msg[31] + line.file_name, 45, 160, 0.45, 0.45, DEF_DRAW_WHITE);
					}
					else if (Line_has_sub_state(sub_state, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_DELETE_ROOM))
					{
						Draw(line_msg[45], 45, 120, 0.55, 0.55, DEF_DRAW_WHITE);
						Draw(line_msg[46], 45, 130, 0.4, 0.4, DEF_DRAW_WHITE);
					}
				}
			}

			Draw_texture(var_square_image[0], color, 312.5, 0, 10, 15);
			Draw_texture(var_square_image[0], color, 312.5, 215, 10, 10);

			if (scroll_bar_y_pos < 0)
				scroll_bar_y_pos = 0;
			else if (scroll_bar_y_pos > 190)
				scroll_bar_y_pos = 190;

			Draw_texture(&line.scroll_bar, (line.scroll_bar.selected ? DEF_DRAW_BLUE : DEF_DRAW_WEAK_BLUE), 312.5, 15 + scroll_bar_y_pos, 10, 10);

			if(Util_expl_query_show_flag())
				Util_expl_draw();

			if(Util_err_query_error_show_flag())
				Util_err_draw();

			Draw_bot_ui();
		}

		Draw_apply_draw();
	}
	else
		gspWaitForVBlank();

	result = Util_queue_get(&line.main_thread_queue, (u32*)&command_id, &command_data, 0);
	if(result.code == DEF_SUCCESS)
	{
		switch (command_id)
		{
			case LINE_COMMAND_MAIN_THREAD_DISPLAY_IMAGE_REQUEST:
			case LINE_COMMAND_MAIN_THREAD_PLAY_AUDIO_REQUEST:
			case LINE_COMMAND_MAIN_THREAD_PLAY_VIDEO_REQUEST:
			{
				Line_play_media_request_data* data = (Line_play_media_request_data*)command_data;
				std::string file_name = "";

				//Valid command data is mandatory.
				if(!data)
					break;

				//Suspend line and jump to different application based on file type.
				Line_suspend();
				if(command_id == LINE_COMMAND_MAIN_THREAD_DISPLAY_IMAGE_REQUEST)
				{
					if(!Imv_query_init_flag())
						Imv_init(true);
					else
						Imv_resume();
				}
				else if(command_id == LINE_COMMAND_MAIN_THREAD_PLAY_AUDIO_REQUEST)
				{
					if(!Mup_query_init_flag())
						Mup_init(true);
					else
						Mup_resume();
				}
				else if(command_id == LINE_COMMAND_MAIN_THREAD_PLAY_VIDEO_REQUEST)
				{
					if(!Vid_query_init_flag())
						Vid_init(true);
					else
						Vid_resume();
				}

				LightLock_Lock(&line.log_copy_string_lock);

				//Search for file ID.
				file_name = Line_get_google_drive_file_id(line.optimized_logs.data[data->log_id]);

				if(command_id == LINE_COMMAND_MAIN_THREAD_DISPLAY_IMAGE_REQUEST)
				{
					if(file_name != "")
					{
						file_name += ".jpg";

						//Check if file exists.
						result = Util_file_check_file_exist(file_name, DEF_MAIN_DIR + "images/");

						//Specify the file name if it exists, otherwise specify the URL.
						if(result.code == DEF_SUCCESS)
							Imv_set_load_file(file_name, DEF_MAIN_DIR + "images/");
						else
							Imv_set_url(line.optimized_logs.data[data->log_id]);
					}
					else//If URL is not google drive's URL, just specify it.
						Imv_set_url(line.optimized_logs.data[data->log_id]);
				}
				else if(command_id == LINE_COMMAND_MAIN_THREAD_PLAY_AUDIO_REQUEST)
				{
					if(file_name != "")
					{
						file_name += ".m4a";

						//Check if file exists.
						result = Util_file_check_file_exist(file_name, DEF_MAIN_DIR + "audio/");

						//Specify the file name if it exists, otherwise specify the URL.
						if(result.code == DEF_SUCCESS)
							Mup_set_load_file(file_name, DEF_MAIN_DIR + "audio/");
						else
							Mup_set_url(line.optimized_logs.data[data->log_id]);
					}
					else//If URL is not google drive's URL, just specify it.
						Mup_set_url(line.optimized_logs.data[data->log_id]);
				}
				else if(command_id == LINE_COMMAND_MAIN_THREAD_PLAY_VIDEO_REQUEST)
				{
					if(file_name != "")
					{
						file_name += ".mp4";

						//Check if file exists.
						result = Util_file_check_file_exist(file_name, DEF_MAIN_DIR + "videos/");

						//Specify the file name if it exists, otherwise specify the URL.
						if(result.code == DEF_SUCCESS)
							Vid_set_load_file(file_name, DEF_MAIN_DIR + "videos/");
						else
							Vid_set_url(line.optimized_logs.data[data->log_id]);
					}
					else//If URL is not google drive's URL, just specify it.
						Vid_set_url(line.optimized_logs.data[data->log_id]);
				}

				LightLock_Unlock(&line.log_copy_string_lock);

				break;
			}

			case LINE_COMMAND_MAIN_THREAD_SCAN_GAS_URL_REQUEST:
			{
				//Suspend line and jump to camera to scan QR code.
				Line_suspend();
				if(!Cam_query_init_flag())
					Cam_init(true);
				else
					Cam_resume();

				//Set callback and enable QR code scan.
				Cam_set_qr_scan_callback(Line_qr_scan_result_callback);
				Cam_set_qr_scan_mode(true);

				break;
			}

			case LINE_COMMAND_MAIN_THREAD_TYPE_GAS_URL_REQUEST:
			case LINE_COMMAND_MAIN_THREAD_EDIT_APP_PASSWORD_REQUEST:
			case LINE_COMMAND_MAIN_THREAD_ADD_ROOM_ID_REQUEST:
			case LINE_COMMAND_MAIN_THREAD_INPUT_TEXT_REQUEST:
			case LINE_COMMAND_MAIN_THREAD_SEARCH_TEXT_REQUEST:
			{
				int max_length = 512;
				int num_of_words = 0;
				u32 feature = 0;
				std::string user_input = "";
				std::string init_text = "";//todo move hint text to language file
				std::string hint_text = "";
				std::string line_dic_first_spell[3];
				std::string line_dic_full_spell[3];
				SwkbdType type = SWKBD_TYPE_NORMAL;
				SwkbdValidInput valid_input = SWKBD_NOTEMPTY_NOTBLANK;
				Keyboard_button button = KEYBOARD_BUTTON_NONE;

				Line_add_sub_state(&line, DEF_LINE_STATE_SUB_WAIT_SWKBD_EXIT);

				if(command_id == LINE_COMMAND_MAIN_THREAD_TYPE_GAS_URL_REQUEST)
				{
					line_dic_full_spell[0] = "https://t.ly/";
					line_dic_first_spell[0] = "h";
					line_dic_full_spell[1] = "https://reduced.to/";
					line_dic_first_spell[1] = "h";
					line_dic_full_spell[2] = "https://script.google.com/macros/s/";
					line_dic_first_spell[2] = "h";

					num_of_words = 1;
					hint_text = "URLを入力 / Type your url here.";
					if(line.gas_url == "")
						init_text = line_dic_full_spell[0];
					else
						init_text = line.gas_url;

					feature = SWKBD_PREDICTIVE_INPUT;
				}
				else if(command_id == LINE_COMMAND_MAIN_THREAD_EDIT_APP_PASSWORD_REQUEST)
				{
					hint_text = "パスワードを入力 / Type password here.";
					type = SWKBD_TYPE_QWERTY;
					valid_input = SWKBD_ANYTHING;
				}
				else if(command_id == LINE_COMMAND_MAIN_THREAD_ADD_ROOM_ID_REQUEST)
				{
					Line_add_room_id_request_data* data = (Line_add_room_id_request_data*)command_data;

					//Valid command data is NOT mandatory.
					if(data)
					{
						LightLock_Lock(&line.log_copy_string_lock);
						init_text = line.optimized_logs.data[data->log_id];
						LightLock_Unlock(&line.log_copy_string_lock);
					}

					hint_text = "idを入力 / Type id here";
					max_length = 64;

					//Back to chat room selection.
					Line_reset_msg();

					LightLock_Lock(&line.state_lock);
					line.main_state = LINE_STATE_MAIN_CHAT_ROOM_SELECTION;
					LightLock_Unlock(&line.state_lock);
				}
				else if(command_id == LINE_COMMAND_MAIN_THREAD_INPUT_TEXT_REQUEST)
				{
					if(line.input_text != "")
						init_text = line.input_text;
					else
						init_text = var_clipboard;

					hint_text = "メッセージを入力 / Type message here.";
					max_length = 8192;
					feature = SWKBD_MULTILINE | SWKBD_PREDICTIVE_INPUT;
					valid_input = SWKBD_NOTBLANK;
				}
				else if(command_id == LINE_COMMAND_MAIN_THREAD_SEARCH_TEXT_REQUEST)
				{
					hint_text = "検索 単語/文 を入力 / Type search word/message(s) here.";
					init_text = line.search_text;
					feature = SWKBD_PREDICTIVE_INPUT;
				}

				result = Util_swkbd_init(type, valid_input, 2, max_length, hint_text, init_text, feature);
				Util_log_save(DEF_LINE_MAIN_STR, "Util_swkbd_init()..." + result.string + result.error_description, result.code);

				if(num_of_words != 0)
				{
					result = Util_swkbd_set_dic_word(line_dic_first_spell, line_dic_full_spell, num_of_words);
					Util_log_save(DEF_LINE_MAIN_STR, "Util_swkbd_set_dic_word()..." + result.string + result.error_description, result.code);
				}

				result = Util_swkbd_launch(&user_input, &button);
				Util_log_save(DEF_LINE_MAIN_STR, "Util_swkbd_launch()..." + result.string + result.error_description, result.code);

				Util_swkbd_exit();

				if (button == KEYBOARD_BUTTON_RIGHT)
				{
					if(command_id == LINE_COMMAND_MAIN_THREAD_TYPE_GAS_URL_REQUEST)
					{
						Line_get_script_url_request_data* data = (Line_get_script_url_request_data*)malloc(sizeof(Line_get_script_url_request_data));

						if(data)
						{
							Line_add_sub_state(&line, DEF_LINE_STATE_SUB_GET_SCRIPT_URL);
							snprintf(data->url, sizeof(data->url), "%s", user_input.c_str());

							//Get script URL (in case of short URL) in worker thread.
							DEF_LINE_QUEUE_ADD_WITH_LOG(&line.worker_thread_queue, LINE_COMMAND_WORKER_THREAD_GET_SCRIPT_URL_REQUEST, data, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
						}
					}
					else if(command_id == LINE_COMMAND_MAIN_THREAD_EDIT_APP_PASSWORD_REQUEST)
					{
						uint8_t hash[DEF_LINE_SECRET_HASH_SIZE] = { 0, };

						//Also include NULL terminator.
						result.code = Line_generate_password_hash((uint8_t*)user_input.c_str(), (user_input.length() + 1), hash);
						if(result.code == DEF_SUCCESS)
						{
							result = Util_file_save_to_file("secret", DEF_MAIN_DIR, hash, sizeof(hash), true);
							if(result.code == DEF_SUCCESS)
								memcpy(line.secret_hash, hash, sizeof(hash));
							else
								Util_log_save(DEF_LINE_MAIN_STR, "Util_file_save_to_file()...", result.code);
						}
						else
							Util_log_save(DEF_LINE_MAIN_STR, "Line_generate_password_hash()...", result.code);

					}
					else if(command_id == LINE_COMMAND_MAIN_THREAD_ADD_ROOM_ID_REQUEST)
					{
						result = Util_file_check_file_exist(user_input, DEF_MAIN_DIR + "to/");
						Util_log_save(DEF_LINE_MAIN_STR, "Util_file_check_file_exist()..." + result.string + result.error_description, result.code);

						//Only add it if no such room ID exists.
						if(result.code != DEF_SUCCESS)
						{
							std::string default_room_info = "";

							default_room_info = DEF_JSON_START_OBJECT;
							default_room_info += DEF_JSON_NON_STRING_DATA_WITH_KEY("log_height", "0");
							default_room_info += DEF_JSON_NON_STRING_DATA_WITH_KEY("num_of_unread_logs", "0");
							default_room_info += DEF_JSON_STRING_DATA_WITH_KEY("room_name", "");
							default_room_info += DEF_JSON_STRING_DATA_WITH_KEY("room_icon_url", "");
							default_room_info += DEF_JSON_END_OBJECT;

							result = Util_file_save_to_file(user_input, DEF_MAIN_DIR + "to/", (u8*)default_room_info.c_str(), default_room_info.length(), true);
							Util_log_save(DEF_LINE_MAIN_STR, "Util_file_save_to_file()..." + result.string + result.error_description, result.code);

							//Reload room informations so that user can see the newly added room.
							DEF_LINE_QUEUE_ADD_WITH_LOG(&line.worker_thread_queue, LINE_COMMAND_WORKER_THREAD_LOAD_ROOM_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
							DEF_LINE_QUEUE_ADD_WITH_LOG(&line.worker_thread_queue, LINE_COMMAND_WORKER_THREAD_LOAD_ICON_REQUEST, NULL, 0, QUEUE_OPTION_DO_NOT_ADD_IF_EXIST);
						}
					}
					else if(command_id == LINE_COMMAND_MAIN_THREAD_INPUT_TEXT_REQUEST)
					{
						line.input_text = user_input;
						if(line.input_text != "")
							Line_add_sub_state(&line, DEF_LINE_STATE_SUB_WAIT_APPROVEMENT_SEND_MESSAGE);
					}
					else if(command_id == LINE_COMMAND_MAIN_THREAD_SEARCH_TEXT_REQUEST)
					{
						line.search_text = user_input;
						line.search_selected_index = 0;
						line.search_found_items = 0;
						for(int i = 0; i < 4000; i++)
							line.search_found_item_index[i] = 0;

						LightLock_Lock(&line.log_copy_string_lock);
						for(int i = 0; i < 4000; i++)
						{
							if(line.logs.logs.text[i].find(line.search_text) != std::string::npos)
							{
								line.search_found_item_index[line.search_found_items] = i;
								line.search_found_items++;
							}
						}
						LightLock_Unlock(&line.log_copy_string_lock);

						if(line.search_found_items > 0)
						{
							line.search_found_items--;
							line.search_selected_index = line.search_found_items;
							y_pos->y = line.optimized_logs.start_pos[line.search_found_item_index[line.search_found_items]];
						}
					}
				}

				Line_remove_sub_state(&line, DEF_LINE_STATE_SUB_WAIT_SWKBD_EXIT);

				break;
			}

			default:
				break;
		}

		free(command_data);
		command_data = NULL;
	}
}
