#include <3ds.h>
#include <algorithm>
#include <string>
#include <unistd.h>

#include "hid.hpp"
#include "base64.h"
#include "draw.hpp"
#include "file.hpp"
#include "httpc.hpp"
#include "line.hpp"
#include "share_function.hpp"
#include "setting_menu.hpp"
#include "error.hpp"
#include "menu.hpp"
#include "explorer.hpp"
#include "log.hpp"
#include "types.hpp"

bool line_thread_suspend = false;
bool line_already_init = false;
bool line_main_run = false;
bool line_log_dl_thread_run = false;
bool line_log_load_thread_run = false;
bool line_log_parse_thread_run = false;
bool line_send_msg_thread_run = false;
bool line_auto_update = false;
bool line_hide_id = false;
bool line_parse_log_request = false;
bool line_load_log_request = false;
bool line_dl_log_request = false;
bool line_send_check[3] = { false, false, false };
bool line_send_request[3] = { false, false, false };
bool line_select_sticker_request = false;
bool line_select_file_request = false;
bool line_type_id_request = false;
bool line_type_app_ps_request = false;
bool line_type_msg_request = false;
bool line_type_main_url_request = false;
bool line_type_script_ps_request = false;
bool line_sending_msg = false;
bool line_send_success = false;
int line_selected_menu_mode = 0;
int line_selected_room_num = 0;
int line_selected_sticker_tab_num = 0;
int line_selected_sticker_num = 0;
int line_log_httpc_buffer_size = 0x200000;
int line_log_fs_buffer_size = 0x200000;
int line_send_fs_buffer_size = 0x1000000;
int line_send_fs_cache_buffer_size = 0x300000;
int line_step_max = 1;
int line_current_step = 0;

int line_log_dl_progress = 0;
int line_num_of_msg = 0;
int line_num_of_lines = 0;
int line_sticker_num_list[121] = { 0, 
// 11537
52002734, 52002735, 52002736, 52002737, 52002738, 52002739, 52002740, 52002741, 52002742, 52002743, 
52002744, 52002745, 52002746, 52002747, 52002748, 52002749, 52002750, 52002751, 52002752, 52002753,
52002754, 52002755, 52002756, 52002757, 52002758, 52002759, 52002760, 52002761, 52002762, 52002763, 
52002764, 52002765, 52002766, 52002767, 52002768, 52002769, 52002770, 52002771, 52002772, 52002773, 
// 11538
51626494, 51626495, 51626496, 51626497, 51626498, 51626499, 51626500, 51626501, 51626502, 51626503, 
51626504, 51626505, 51626506, 51626507, 51626508, 51626509, 51626510, 51626511, 51626512, 51626513, 
51626514, 51626515, 51626516, 51626517, 51626518, 51626519, 51626520, 51626521, 51626522, 51626523, 
51626524, 51626525, 51626526, 51626527, 51626528, 51626529, 51626530, 51626531, 51626532, 51626533,
// 11539
52114110, 52114111, 52114112, 52114113, 52114114, 52114115, 52114116, 52114117, 52114118, 52114119,
52114120, 52114121, 52114122, 52114123, 52114124, 52114125, 52114126, 52114127, 52114128, 52114129,
52114130, 52114131, 52114132, 52114133, 52114134, 52114135, 52114136, 52114137, 52114138, 52114139,
52114140, 52114141, 52114142, 52114143, 52114144, 52114145, 52114146, 52114147, 52114148, 52114149,
};
double line_text_x = 0.0;
double line_text_x_cache = 0.0;
double line_text_y = 0.0;
double line_text_y_cache = 0.0;
double line_text_size = 0.66;
double line_text_size_cache = 0.66;
double line_text_interval = 35.0;
double line_text_interval_cache = 35.0;
double line_max_y = 10.0;
double line_selected_msg_num = 0.0;

std::string line_script_auth = "";
std::string line_ids[100];
std::string line_input_text = "";
std::string line_main_url = "";
std::string line_short_msg_log[60000];
std::string line_log_data = "";
std::string line_msg_en[42] = {
	" Message(s) found. (",
	" Line(s))",
	"Send",
	"Copy" ,
	"settings" ,
	"Advanced settings" ,
	"Send a message(A)" ,
	"Log update(B)" ,
	"Auto update off",
	"Auto update on",
	"Selected message num : " ,
	"",
	"Message copy\n(ZR)",
	"Up\n(D ↑)",
	"Down\n(D ↓)",
	"Increase interval\n(D ↑)",
	"Decrease interval\n(D ↓)",
	"Decrease size\n(L)",
	"Increase size\n(R)",
	"Add new ID(Y)",
	"Change main url(X)",
	"Yes(A)",
	"No(B)",
	"Change app password(A)",
	"Change script password(B)",
	"Send a sticker(Y)",
	"Do you want to send a message?",
	"Do you want to send a sticker?",
	"Touch to download and display image ",
	"Touch to display image ",
	"Do you want to send a file?",
	"(If the file's invalid it'll fail.)",
	"Path : ",
	"File name : ",
	"View image(X)",
	"Select(A) Go back(B) Move(DPAD ↑,↓,→,←) Close(Y)",
	"Send a file(X)",
	"Receive",
	"Sending a message...",
	"Sending a sticker...",
	"Sending a file...",
	"Success (A to close)",
};
std::string line_msg_jp[42] = {
	"件のメッセージが検出されました。(",
	" 行)",
	"送信",
	"コピー",
	"設定",
	"上級設定",
	"メッセージ送信(A)",
	"ログ更新(B)",
	"自動更新 オフ",
	"自動更新 オン",
	"選択されたメッセージ番号 : ",
	"",
	"メッセージコピー\n(ZR)",
	"上\n(十字↑)",
	"下\n(十字↓)",
	"間隔を広く\n(十字↑)",
	"間隔を狭く\n(十字↓)",
	"サイズを小さく\n(L)",
	"サイズを大きく\n(R)",
	"新規ID追加(Y)",
	"メインURL変更(X)",
	"はい(A)",
	"いいえ(B)",
	"アプリパスワード変更(A)",
	"スクリプトパスワード変更(B)",
	"スタンプ送信(Y)",
	"メッセージを送信しますか?",
	"スタンプを送信しますか?",
	"タッチすると画像をダウンロードして表示",
	"タッチすると画像を表示",
	"ファイルを送信しますか?",
	"(ファイルが無効な場合失敗します)",
	"パッチ : ",
	"ファイル名 : ",
	"画像を見る(X)",
	"選択(A) 戻る(B) 移動(十字 ↑,↓,→,←) 閉じる(Y)",
	"ファイル送信(X)",
	"受信",
	"メッセージを送信中...",
	"スタンプを送信中...",
	"ファイルを送信中...",
	"成功 (Aで閉じる)",
};
std::string line_msg_log[300];
std::string line_send_file_dir = "";
std::string line_send_file_name = "";
std::string line_content[60000];

C2D_Image line_stickers_images[121];

Thread line_dl_log_thread, line_load_log_thread, line_send_msg_thread, line_parse_log_thread;

bool Line_query_init_flag(void)
{
	return line_already_init;
}

bool Line_query_running_flag(void)
{
	return line_main_run;
}

int Line_query_buffer_size(int buffer_num)
{
	if (buffer_num == LINE_HTTPC_BUFFER)
		return line_log_httpc_buffer_size;
	else if (buffer_num == LINE_FS_BUFFER)
		return line_log_fs_buffer_size;
	else if (buffer_num == LINE_SEND_FS_BUFFER)
		return line_send_fs_buffer_size;
	else if (buffer_num == LINE_SEND_FS_CACHE_BUFFER)
		return line_send_fs_cache_buffer_size;
	else
		return -1;
}

std::string Line_query_content_info(int log_num)
{
	if (log_num >= 0 && log_num <= 59999)
		return line_content[log_num];
	else
		return "";
}

double Line_query_max_y(void)
{
	return line_max_y;
}

std::string Line_query_msg_log(int log_num)
{
	if (log_num >= 0 && log_num <= 299)
		return line_msg_log[log_num];
	else
		return "";
}

bool Line_query_operation_flag(int operation_num)
{
	if (operation_num == LINE_SEND_MSG_CHECK_REQUEST)
		return line_send_check[0];
	else if (operation_num == LINE_SEND_STICKER_CHECK_REQUEST)
		return line_send_check[1];
	else if (operation_num == LINE_SEND_CONTENT_CHECK_REQUEST)
		return line_send_check[2];
	else if (operation_num == LINE_SEND_MSG_REQUEST)
		return line_send_request[0];
	else if (operation_num == LINE_SEND_STICKER_REQUEST)
		return line_send_request[1];
	else if (operation_num == LINE_SEND_CONTENT_REQUEST)
		return line_send_request[2];
	else if (operation_num == LINE_DL_LOG_REQUEST)
		return line_dl_log_request;
	else if (operation_num == LINE_LOAD_LOG_REQUEST)
		return line_load_log_request;
	else if (operation_num == LINE_TYPE_MSG_REQUEST)
		return line_type_msg_request;
	else if (operation_num == LINE_TYPE_ID_REQUEST)
		return line_type_id_request;
	else if (operation_num == LINE_TYPE_MAIN_URL_REQUEST)
		return line_type_main_url_request;
	else if (operation_num == LINE_TYPE_APP_PS_REQUEST)
		return line_type_app_ps_request;
	else if (operation_num == LINE_TYPE_SCRIPT_PS_REQUEST)
		return line_type_script_ps_request;
	else if (operation_num == LINE_SELECT_STICKER_REQUEST)
		return line_select_sticker_request;
	else if (operation_num == LINE_SELECT_FILE_REQUEST)
		return line_select_file_request;
	else if (operation_num == LINE_SENDING_MSG)
		return line_sending_msg;
	else if (operation_num == LINE_SEND_SUCCESS)
		return line_send_success;
	else
		return false;
}

double Line_query_selected_num_d(int item_num)
{
	if (item_num == LINE_SELECTED_MSG_NUM_D)
		return line_selected_msg_num;
	else
		return -1.0;
}

int Line_query_selected_num(int item_num)
{
	if (item_num == LINE_SELECTED_MENU_MODE_NUM)
		return line_selected_menu_mode;
	else if (item_num == LINE_SELECTED_STICKER_NUM)
		return line_selected_sticker_num;
	else if (item_num == LINE_SELECTED_STICKER_TAB_NUM)
		return line_selected_sticker_tab_num;
	else if (item_num == LINE_SELECTED_ROOM_NUM)
		return line_selected_room_num;
	else
		return -1;
}

double Line_query_x_y_size_interval(int item_num)
{
	if (item_num == LINE_TEXT_X)
		return line_text_x_cache;
	else if (item_num == LINE_TEXT_Y)
		return line_text_y_cache;
	else if (item_num == LINE_TEXT_SIZE)
		return line_text_size_cache;
	else if (item_num == LINE_TEXT_INTERVAL)
		return line_text_interval_cache;
	else
		return -1.0;
}

bool Line_query_setting(int setting_num)
{
	if (setting_num == LINE_HIDE_ID)
		return line_hide_id;
	else if (setting_num == LINE_AUTO_UPDATE)
		return line_auto_update;
	else
		return false;
}

void Line_set_buffer_size(int buffer_num, int size)
{
	if (buffer_num == LINE_HTTPC_BUFFER)
		line_log_httpc_buffer_size = size;
	else if (buffer_num == LINE_FS_BUFFER)
		line_log_fs_buffer_size = size;
	else if (buffer_num == LINE_SEND_FS_BUFFER)
		line_send_fs_buffer_size = size;
	else if (buffer_num == LINE_SEND_FS_CACHE_BUFFER)
		line_send_fs_cache_buffer_size = size;
}

void Line_set_send_file_name(std::string file_name)
{
	line_send_file_name = file_name;
}

void Line_set_send_dir_name(std::string dir_name)
{
	line_send_file_dir = dir_name;
}

void Line_set_operation_flag(int operation_num, bool flag)
{
	if (operation_num == LINE_SEND_MSG_CHECK_REQUEST)
		line_send_check[0] = flag;
	else if (operation_num == LINE_SEND_STICKER_CHECK_REQUEST)
		line_send_check[1] = flag;
	else if (operation_num == LINE_SEND_CONTENT_CHECK_REQUEST)
		line_send_check[2] = flag;
	else if (operation_num == LINE_SEND_MSG_REQUEST)
		line_send_request[0] = flag;
	else if (operation_num == LINE_SEND_STICKER_REQUEST)
		line_send_request[1] = flag;
	else if (operation_num == LINE_SEND_CONTENT_REQUEST)
		line_send_request[2] = flag;
	else if (operation_num == LINE_DL_LOG_REQUEST)
		line_dl_log_request = flag;
	else if (operation_num == LINE_LOAD_LOG_REQUEST)
		line_load_log_request = flag;
	else if (operation_num == LINE_TYPE_MSG_REQUEST)
		line_type_msg_request = flag;
	else if (operation_num == LINE_TYPE_ID_REQUEST)
		line_type_id_request = flag;
	else if (operation_num == LINE_TYPE_MAIN_URL_REQUEST)
		line_type_main_url_request = flag;
	else if (operation_num == LINE_TYPE_APP_PS_REQUEST)
		line_type_app_ps_request = flag;
	else if (operation_num == LINE_TYPE_SCRIPT_PS_REQUEST)
		line_type_script_ps_request = flag;
	else if (operation_num == LINE_SELECT_STICKER_REQUEST)
		line_select_sticker_request = flag;
	else if (operation_num == LINE_SELECT_FILE_REQUEST)
		line_select_file_request = flag;
	else if (operation_num == LINE_SENDING_MSG)
		line_sending_msg = flag;
	else if (operation_num == LINE_SEND_SUCCESS)
		line_send_success = flag;
}

void Line_set_selected_num_d(int item_num, double value)
{
	if (item_num == LINE_SELECTED_MSG_NUM_D)
		line_selected_msg_num = value;
}

void Line_set_selected_num(int item_num, int value)
{
	if (item_num == LINE_SELECTED_MENU_MODE_NUM)
		line_selected_menu_mode = value;
	else if (item_num == LINE_SELECTED_STICKER_NUM)
		line_selected_sticker_num = value;
	else if (item_num == LINE_SELECTED_STICKER_TAB_NUM)
		line_selected_sticker_tab_num = value;
	else if (item_num == LINE_SELECTED_ROOM_NUM)
		line_selected_room_num = value;
}

void Line_set_x_y_size_interval(int item_num, double value)
{
	if (item_num == LINE_TEXT_X)
		line_text_x_cache = value;
	else if (item_num == LINE_TEXT_Y)
		line_text_y_cache = value;
	else if (item_num == LINE_TEXT_SIZE)
		line_text_size_cache = value;
	else if (item_num == LINE_TEXT_INTERVAL)
		line_text_interval_cache = value;
}

void Line_set_setting(int setting_num, bool flag)
{
	if (setting_num == LINE_HIDE_ID)
		line_hide_id = flag;
	else if (setting_num == LINE_AUTO_UPDATE)
		line_auto_update = flag;
}

void Line_resume(void)
{
	Menu_suspend();
	line_thread_suspend = false;
	line_main_run = true;
}

void Line_suspend(void)
{
	line_thread_suspend = true;
	line_main_run = false;
	Menu_resume();
}

void Line_init(void)
{
	Log_log_save("Line/Init", "Initializing...", 1234567890, s_debug_slow);
	bool auth_success = false;
	u8* fs_buffer;
	u32 read_size = 0;
	int log_num;
	std::string auth_code = "";
	std::string input_string;
	FS_Archive fs_archive = 0;
	Handle fs_handle = 0;
	SwkbdState swkbd_state;
	Result_with_string result;
	fs_buffer = (u8*)malloc(0x2000);


	Draw_progress("0/3 [Line] Authing...");
	memset(fs_buffer, 0x0, 0x2000);
	log_num = Log_log_save("Line/Init/fs", "Share_load_from_file(auth)...", 1234567890, s_debug_slow);
	result = Share_load_from_file("auth", fs_buffer, 0x2000, &read_size, "/Line/",  fs_handle, fs_archive);
	Log_log_add(log_num, result.string, result.code, s_debug_slow);

	if (result.code == 0)
		auth_code = (char*)fs_buffer;
	else
	{
		while (true)
		{
			memset(s_swkb_input_text, 0x0, 8192);
			swkbdInit(&swkbd_state, SWKBD_TYPE_QWERTY, 1, 256);
			swkbdSetHintText(&swkbd_state, "パスワードを入力 / Type password here.");
			swkbdSetValidation(&swkbd_state, SWKBD_ANYTHING, 0, 0);
			swkbdSetPasswordMode(&swkbd_state, SWKBD_PASSWORD_HIDE);
			swkbdInputText(&swkbd_state, s_swkb_input_text, 256);
			input_string = s_swkb_input_text;

			memset(s_swkb_input_text, 0x0, 8192);
			swkbdInit(&swkbd_state, SWKBD_TYPE_QWERTY, 1, 256);
			swkbdSetHintText(&swkbd_state, "パスワードを入力(再度) / Type password here.(again)");
			swkbdSetValidation(&swkbd_state, SWKBD_ANYTHING, 0, 0);
			swkbdSetPasswordMode(&swkbd_state, SWKBD_PASSWORD_HIDE);
			swkbdInputText(&swkbd_state, s_swkb_input_text, 256);

			if (input_string == s_swkb_input_text)
			{
				log_num = Log_log_save("Line/Init/fs", "Save_to_file(auth)...", 1234567890, s_debug_slow);
				result = Share_save_to_file("auth", (u8*)s_swkb_input_text, sizeof(s_swkb_input_text), "/Line/", true, fs_handle, fs_archive);
				Log_log_add(log_num, result.string, result.code, s_debug_slow);
				auth_code = s_swkb_input_text;

				break;
			}
		}
	}

	Log_log_save("Line/Init/auth", "Please enter password", 1234567890, s_debug_slow);
	if (auth_code == "")
	{
		Log_log_save("Line/Init/auth", "Password is not set", 1234567890, s_debug_slow);
		auth_success = true;
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			swkbdInit(&swkbd_state, SWKBD_TYPE_QWERTY, 1, 256);
			swkbdSetHintText(&swkbd_state, "パスワードを入力 / Type password here.");
			swkbdSetValidation(&swkbd_state, SWKBD_ANYTHING, 0, 0);
			swkbdSetPasswordMode(&swkbd_state, SWKBD_PASSWORD_HIDE);

			swkbdInputText(&swkbd_state, s_swkb_input_text, 256);
			if (auth_code == s_swkb_input_text)
			{
				auth_success = true;
				Log_log_save("Line/Init/auth", "Password is correct", 1234567890, s_debug_slow);
				break;
			}
			else
				Log_log_save("Line/Init/auth", "Password is incorrect", 1234567890, s_debug_slow);
		}
	}

	if (!auth_success)
	{
		Log_log_save("Line/Init/auth", "Auth failed, rebooting...", 1234567890, s_debug_slow);
		Draw_progress("0/3 [Line] Auth failed.");
		usleep(500000);
		APT_HardwareResetAsync();
	}


	Draw_progress("1/3 [Line] Loading settings...");
	memset(fs_buffer, 0x0, 0x2000);
	log_num = Log_log_save("Line/Init/fs", "Share_load_from_file(script_auth)...", 1234567890, s_debug_slow);
	result = Share_load_from_file("script_auth", fs_buffer, 0x2000, &read_size, "/Line/", fs_handle, fs_archive);
	Log_log_add(log_num, result.string, result.code, s_debug_slow);
	if (result.code == 0)
		line_script_auth = (char*)fs_buffer;
	else
		line_script_auth = "";
	
	memset(fs_buffer, 0x0, 0x2000);
	log_num = Log_log_save("Line/Init/fs", "Share_load_from_file(main_url.txt)...", 1234567890, s_debug_slow);
	result = Share_load_from_file("main_url.txt", fs_buffer, 0x2000, &read_size, "/Line/", fs_handle, fs_archive);
	Log_log_add(log_num, result.string, result.code, s_debug_slow);
	if (result.code == 0)
		line_main_url = (char*)fs_buffer;
	else
		line_main_url = "";

	log_num = Log_log_save("Line/Init/fs", "Read_id...", 1234567890, false);
	result = Line_read_id("/Line/to/");
	Log_log_add(log_num, result.string, result.code, s_debug_slow);

	Draw_progress("2/3 [Line] Starting threads...");
	line_log_dl_thread_run = true;
	line_log_load_thread_run = true;
	line_log_parse_thread_run = true;
	line_send_msg_thread_run = true;
	
	line_dl_log_thread = threadCreate(Line_log_download_thread, (void*)(""), STACKSIZE, 0x30, -1, s_debug_slow);
	line_load_log_thread = threadCreate(Line_log_load_thread, (void*)(""), STACKSIZE, 0x30, -1, s_debug_slow);
	line_parse_log_thread = threadCreate(Line_log_parse_thread, (void*)(""), STACKSIZE, 0x32, -1, s_debug_slow);
	line_send_msg_thread = threadCreate(Line_send_message_thread, (void*)(""), STACKSIZE, 0x30, -1, s_debug_slow);


	Draw_progress("3/3 [Line] Loading textures...");
	log_num = Log_log_save("Line/Init/c2d", "Loading texture (stickers.t3x)...", 1234567890, s_debug_slow);
	result = Draw_load_texture("romfs:/gfx/stickers.t3x", 51, line_stickers_images, 0, 121);
	Log_log_add(log_num, result.string, result.code, s_debug_slow);

	Line_resume();
	line_already_init = true;
	free(fs_buffer);
	Log_log_save("Line/Init", "Initialized", 1234567890, s_debug_slow);
}

void Line_main(void)
{
	int log_y = Log_query_y();
	int font_num = 0;
	float text_red;
	float text_green;
	float text_blue;
	float text_alpha;
	float texture_size;
	float scroll_bar_y_size = 5.0;
	float scroll_bar_y_pos = (195.0 / (-line_max_y / -line_text_y));
	double log_x = Log_query_x();
	double draw_x;
	double draw_y;
	int main_log_num_return;
	size_t type_pos[3];
	size_t sticker_num_start_pos;
	size_t sticker_num_end_pos;
	std::string status;
	std::string hidden_id;
	FS_Archive main_fs_archive = 0;
	Handle main_fs_handle = 0;
	SwkbdState main_swkbd;
	SwkbdStatusData main_swkbd_status;
	Result_with_string main_result;

	osTickCounterUpdate(&s_tcount_frame_time);

	if (Sem_query_font_flag(SEM_USE_DEFAULT_FONT))
		font_num = 0;
	else if (Sem_query_font_flag(SEM_USE_SYSTEM_SPEIFIC_FONT))
		font_num = Sem_query_selected_num(SEM_SELECTED_LANG_NUM) + 1;
	else if (Sem_query_font_flag(SEM_USE_EXTERNAL_FONT))
		font_num = 5;

	line_text_size = line_text_size_cache;
	line_text_interval = line_text_interval_cache;
	line_text_x = line_text_x_cache;
	line_text_y = line_text_y_cache;
	texture_size = line_text_size_cache;

	if (line_auto_update || line_dl_log_request)
		line_log_dl_progress = Httpc_query_dl_progress();

	if (Sem_query_settings(SEM_NIGHT_MODE))
	{
		text_red = 1.0;
		text_green = 1.0;
		text_blue = 1.0;
		text_alpha = 0.75;
	}
	else
	{
		text_red = 0.0;
		text_green = 0.0;
		text_blue = 0.0;
		text_alpha = 1.0;
	}

	Draw_set_draw_mode(Sem_query_settings(SEM_VSYNC_MODE));
	if (Sem_query_settings(SEM_NIGHT_MODE))
		Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);

	Draw_texture(Square_image, black_tint, 0, 0.0, 0.0, 400.0, 15.0);
	Draw_texture(Wifi_icon_image, dammy_tint, s_wifi_signal, 360.0, 0.0, 15.0, 15.0);
	Draw_texture(Battery_level_icon_image, dammy_tint, s_battery_level / 5, 330.0, 0.0, 30.0, 15.0);
	if (s_battery_charge)
		Draw_texture(Battery_charge_icon_image, dammy_tint, 0, 310.0, 0.0, 20.0, 15.0);
	Draw(s_status, 0, 0.0f, 0.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 1.0f);
	Draw(s_battery_level_string, 0, 337.5f, 1.25f, 0.4f, 0.4f, 0.0f, 0.0f, 0.0f, 0.5f);

	for (int i = 1; i <= 59999; i++)
	{
		if (i > line_num_of_lines || line_text_y + line_text_interval * i >= 240)
			break;
		else if (line_text_y + line_text_interval * i <= -1000)
		{
			if ((line_text_y + line_text_interval * (i + 100)) <= -20)
				i += 100;
		}
		else if (line_text_y + line_text_interval * i <= 10)
		{
		}
		else
		{
			type_pos[0] = line_content[i].find("<type>sticker</type>");
			type_pos[1] = line_content[i].find("<type>exist_image</type>");
			type_pos[2] = line_content[i].find("<type>image</type>");

			if (!(type_pos[0] == std::string::npos))
			{
				sticker_num_start_pos = line_content[i].find("<num>");
				sticker_num_end_pos = line_content[i].find("</num>");;
				Draw_texture(line_stickers_images, dammy_tint, std::stoi(line_content[i].substr((sticker_num_start_pos + 5), sticker_num_end_pos - (sticker_num_start_pos + 5))), line_text_x, (line_text_y + line_text_interval * i), (texture_size * 120.0), (texture_size * 120.0));
			}
			else if(!(type_pos[1] == std::string::npos) || !(type_pos[2] == std::string::npos))
				Draw_texture(Square_image, weak_red_tint, 0, line_text_x, (line_text_y + line_text_interval * i), 500.0, 20.0);

			Draw(line_short_msg_log[i], font_num, line_text_x, line_text_y + line_text_interval * i, line_text_size, line_text_size, text_red, text_green, text_blue, text_alpha);
		}
	}

	if (Sem_query_settings(SEM_DEBUG_MODE))
		Draw_debug_info();
	if (Log_query_log_show_flag())
	{
		for (int i = 0; i < 23; i++)
			Draw(Log_query_log(log_y + i), 0, log_x, 10.0f + (i * 10), 0.4, 0.4, 0.0, 0.5, 1.0, 1.0);
	}

	if (line_hide_id && line_ids[line_selected_room_num].length() >= 5)
	{
		hidden_id = line_ids[line_selected_room_num].substr(0, 5);

		for (size_t i = 6; i <= line_ids[line_selected_room_num].length(); i++)
			hidden_id += "*";
	}
	else
		hidden_id = line_ids[line_selected_room_num];

	Draw_texture(Square_image, weak_aqua_tint, 0, 0.0, 15.0, 50.0 * line_log_dl_progress, 3.0);

	if (Sem_query_settings(SEM_NIGHT_MODE))
		Draw_screen_ready_to_draw(1, true, 2, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(1, true, 2, 1.0, 1.0, 1.0);

	for (int i = 1; i <= 59999; i++)
	{
		if (i > line_num_of_lines || (line_text_y + line_text_interval * i) - 240 >= 125)
			break;
		else if (line_text_y + line_text_interval * i <= -1000)
		{
			if ((line_text_y + line_text_interval * (i + 100)) <= 10)
				i += 100;
		}
		else if ((line_text_y + line_text_interval * i) - 240 <= -60)
		{
		}
		else
		{
			type_pos[0] = line_content[i].find("<type>sticker</type>");
			type_pos[1] = line_content[i].find("<type>exist_image</type>");
			type_pos[2] = line_content[i].find("<type>image</type>");

			if (!(type_pos[0] == std::string::npos))
			{
				sticker_num_start_pos = line_content[i].find("<num>");
				sticker_num_end_pos = line_content[i].find("</num>");;
				Draw_texture(line_stickers_images, dammy_tint, std::stoi(line_content[i].substr((sticker_num_start_pos + 5), sticker_num_end_pos - (sticker_num_start_pos + 5))), line_text_x - 40.0, (line_text_y + line_text_interval * i) - 240.0, (texture_size * 120.0), (texture_size * 120.0));
			}
			else if (!(type_pos[1] == std::string::npos) || !(type_pos[2] == std::string::npos))
				Draw_texture(Square_image, weak_red_tint, 0, line_text_x - 40.0, (line_text_y + line_text_interval * i) - 240.0, 500.0, 20.0);

			Draw(line_short_msg_log[i], font_num, line_text_x - 40, (line_text_y + line_text_interval * i) - 240, line_text_size, line_text_size, text_red, text_green, text_blue, text_alpha);
		}
	}

	if (s_setting[1] == "jp")
		status = "id = " + hidden_id + "\n" + std::to_string(line_num_of_msg) + line_msg_jp[0] + std::to_string(line_num_of_lines) + line_msg_jp[1];
	else
		status = "id = " + hidden_id + "\n" + std::to_string(line_num_of_msg) + line_msg_en[0] + std::to_string(line_num_of_lines) + line_msg_en[1];

	Draw_texture(Square_image, white_or_black_tint, 0, 312.5, 0.0, 7.5, 15.0);
	Draw_texture(Square_image, white_or_black_tint, 0, 312.5, 215.0, 7.5, 10.0);

	if (scroll_bar_y_pos <= 0.0)
		Draw_texture(Square_image, red_tint, 0, 312.5, 15.0, 7.5, scroll_bar_y_size);
	else if (scroll_bar_y_pos >= 195.0)
		Draw_texture(Square_image, red_tint, 0, 312.5, 210.0, 7.5, scroll_bar_y_size);
	else
		Draw_texture(Square_image, blue_tint, 0, 312.5, 15.0 + scroll_bar_y_pos, 7.5, scroll_bar_y_size);

	if (line_selected_menu_mode != LINE_MENU_COPY)
	{
		Draw(status, 0, 12.5, 140.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("hide id", 0, 260.0, 140.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		Draw(s_line_ver, 0, 260.0, 150.0, 0.45, 0.45, 0.0, 1.0, 0.0, 1.0f);
	}

	Draw_texture(Square_image, weak_aqua_tint, 0, 10.0, 170.0, 300.0, 60.0);
	Draw_texture(Square_image, weak_blue_tint, 0, 10.0, 170.0, 50.0, 10.0);
	Draw_texture(Square_image, weak_white_tint, 0, 60.0, 170.0, 50.0, 10.0);
	Draw_texture(Square_image, weak_yellow_tint, 0, 110.0, 170.0, 50.0, 10.0);
	Draw_texture(Square_image, weak_aqua_tint, 0, 160.0, 170.0, 50.0, 10.0);
	Draw_texture(Square_image, weak_red_tint, 0, 210.0, 170.0, 100.0, 10.0);

	if (line_selected_menu_mode == LINE_MENU_SEND && line_sending_msg)
	{
		Draw_texture(Square_image, blue_tint, 0, 10.0, 170.0, 50.0, 10.0);
		Draw_texture(Square_image, weak_red_tint, 0, 20.0, 185.0, 35.0 * Httpc_query_post_and_dl_progress(), 13.0);

		if (line_send_request[2])
			Draw_texture(Square_image, yellow_tint, 0, 20.0, 205.0, (280.0 / line_step_max) * line_current_step, 13.0);

		if (s_setting[1] == "en")
		{
			if (line_send_request[0])
				Draw(line_msg_en[38], 0, 22.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
			else if (line_send_request[1])
				Draw(line_msg_en[39], 0, 22.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
			else if (line_send_request[2])
				Draw(line_msg_en[40], 0, 22.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		}
		else if (s_setting[1] == "jp")
		{
			if (line_send_request[0])
				Draw(line_msg_jp[38], 0, 22.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
			else if (line_send_request[1])
				Draw(line_msg_jp[39], 0, 22.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
			else if (line_send_request[2])
				Draw(line_msg_jp[40], 0, 22.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		}
	}
	else if (line_selected_menu_mode == LINE_MENU_SEND && line_send_success)
	{
		Draw_texture(Square_image, blue_tint, 0, 10.0, 170.0, 50.0, 10.0);
		Draw_texture(Square_image, weak_red_tint, 0, 20.0, 185.0, 280.0, 13.0);

		if (s_setting[1] == "en")
			Draw(line_msg_en[41], 0, 22.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		else if (s_setting[1] == "jp")
			Draw(line_msg_jp[41], 0, 22.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
	}
	else if (line_selected_menu_mode == LINE_MENU_SEND)
	{
		Draw_texture(Square_image, blue_tint, 0, 10.0, 170.0, 50.0, 10.0);

		Draw_texture(Square_image, weak_aqua_tint, 0, 20.0, 185.0, 280.0, 13.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 20.0, 205.0, 130.0, 13.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 170.0, 205.0, 130.0, 13.0);

		if (s_setting[1] == "en")
		{
			Draw(line_msg_en[6], 0, 22.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_en[25], 0, 22.5, 205.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_en[36], 0, 172.5, 205.0f, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		}
		else if (s_setting[1] == "jp")
		{
			Draw(line_msg_jp[6], 0, 22.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_jp[25], 0, 22.5, 205.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_jp[36], 0, 172.5, 205.0f, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		}
	}
	else if (line_selected_menu_mode == LINE_MENU_RECEIVE)
	{
		Draw_texture(Square_image, white_tint, 0, 60.0, 170.0, 50.0, 10.0);

		Draw_texture(Square_image, weak_aqua_tint, 0, 20.0, 185.0, 130.0, 13.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 170.0, 185.0, 130.0, 13.0);

		if (s_setting[1] == "en")
		{
			Draw(line_msg_en[7], 0, 22.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_en[8 + line_auto_update], 0, 172.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		}
		else if (s_setting[1] == "jp")
		{
			Draw(line_msg_jp[7], 0, 22.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_jp[8 + line_auto_update], 0, 172.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		}
	}
	else if (line_selected_menu_mode == LINE_MENU_COPY)
	{
		Draw_texture(Square_image, yellow_tint, 0, 110.0, 170.0, 50.0, 10.0);

		Draw_texture(Square_image, weak_blue_tint, 0, 10.0, 140.0, 300.0, 25.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 20.0, 185.0, 130.0, 30.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 170.0, 185.0, 60.0, 30.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 240.0, 185.0, 60.0, 30.0);

		if (s_setting[1] == "en")
		{
			Draw(line_msg_en[10] + std::to_string((int)line_selected_msg_num), 0, 12.5, 140.0, 0.4, 0.4, text_red, text_green, text_blue, 1.0f);
			Draw(line_msg_log[(int)line_selected_msg_num].substr(0, 100), font_num, 12.5, 150.0, 0.4, 0.4, text_red, text_green, text_blue, 1.0f);

			Draw(line_msg_en[12], 0, 22.5, 185.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_en[13], 0, 172.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_en[14], 0, 242.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		}
		else if (s_setting[1] == "jp")
		{
			Draw(line_msg_jp[10] + std::to_string((int)line_selected_msg_num), 0, 12.5, 140.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_log[(int)line_selected_msg_num].substr(0, 100), font_num, 12.5, 150.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);

			Draw(line_msg_jp[12], 0, 22.5, 185.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_jp[13], 0, 172.5, 185.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_jp[14], 0, 242.5, 185.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		}
	}
	else if (line_selected_menu_mode == LINE_MENU_SETTINGS)
	{
		Draw_texture(Square_image, aqua_tint, 0, 160.0, 170.0, 50.0, 10.0);

		Draw_texture(Square_image, weak_aqua_tint, 0, 20.0, 185.0, 60.0, 30.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 90.0, 185.0, 60.0, 30.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 170.0, 185.0, 60.0, 30.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 240.0, 185.0, 60.0, 30.0);
		if (s_setting[1] == "en")
		{
			Draw(line_msg_en[15], 0, 22.5, 185.0, 0.35, 0.35, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_en[16], 0, 92.5, 185.0, 0.35, 0.35, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_en[17], 0, 172.5, 185.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_en[18], 0, 242.5, 185.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		}
		else if (s_setting[1] == "jp")
		{
			Draw(line_msg_jp[15], 0, 22.5, 185.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_jp[16], 0, 92.5, 185.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_jp[17], 0, 172.5, 185.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_jp[18], 0, 242.5, 185.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		}
	}
	else if (line_selected_menu_mode == LINE_MENU_ADVANCED_SETTINGS)
	{
		Draw_texture(Square_image, red_tint, 0, 210.0, 170.0, 100.0, 10.0);

		Draw_texture(Square_image, weak_aqua_tint, 0, 20.0, 185.0, 130.0, 13.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 20.0, 205.0, 130.0, 13.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 170.0, 185.0, 130.0, 13.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 170.0, 205.0, 130.0, 13.0);

		if (s_setting[1] == "en")
		{
			Draw(line_msg_en[19], 0, 22.5, 185.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_en[20], 0, 22.5, 205.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_en[23], 0, 172.5, 185.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_en[24], 0, 172.5, 205.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		}
		else if (s_setting[1] == "jp")
		{
			Draw(line_msg_jp[19], 0, 22.5, 185.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_jp[20], 0, 22.5, 205.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_jp[23], 0, 172.5, 185.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw(line_msg_jp[24], 0, 172.5, 205.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		}
	}

	if (s_setting[1] == "en")
	{
		Draw(line_msg_en[2], 0, 12.5, 170.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(line_msg_en[37], 0, 62.5, 170.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
		Draw(line_msg_en[3], 0, 112.5, 170.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(line_msg_en[4], 0, 162.5, 170.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(line_msg_en[5], 0, 212.5, 170.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	}
	else if (s_setting[1] == "jp")
	{
		Draw(line_msg_jp[2], 0, 12.5, 170.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(line_msg_jp[37], 0, 62.5, 170.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
		Draw(line_msg_jp[3], 0, 112.5, 170.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(line_msg_jp[4], 0, 162.5, 170.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(line_msg_jp[5], 0, 212.5, 170.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	}

	if (line_send_check[0] || line_send_check[1] || line_send_check[2])
	{
		Draw_texture(Square_image, blue_tint, 0, 10.0, 110.0, 300.0, 60.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 30.0, 150.0, 70.0, 15.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 120.0, 150.0, 70.0, 15.0);
		if (line_send_check[2])
			Draw_texture(Square_image, weak_aqua_tint, 0, 210.0, 150.0, 70.0, 15.0);

		if (s_setting[1] == "en")
		{
			Draw(line_msg_en[21], 0, 32.5, 150.0, 0.45, 0.45, 1.0, 1.0, 1.0, 1.0);
			Draw(line_msg_en[22], 0, 122.5, 150.0, 0.45, 0.45, 1.0, 1.0, 1.0, 1.0);
			if (line_send_check[2])
				Draw(line_msg_en[34], 0, 212.5, 150.0, 0.4, 0.4, 1.0, 1.0, 1.0, 1.0);
		}
		else if (s_setting[1] == "jp")
		{
			Draw(line_msg_jp[21], 0, 32.5, 150.0, 0.45, 0.45, 1.0, 1.0, 1.0, 1.0);
			Draw(line_msg_jp[22], 0, 122.5, 150.0, 0.45, 0.45, 1.0, 1.0, 1.0, 1.0);
			if (line_send_check[2])
				Draw(line_msg_jp[34], 0, 212.5, 150.0, 0.4, 0.4, 1.0, 1.0, 1.0, 1.0);
		}

		if (line_send_check[0])
		{
			if (s_setting[1] == "en")
			{
				Draw(line_msg_en[26], 0, 10.0, 110.0, 0.45, 0.45, 1.0, 1.0, 1.0, 1.0);
				Draw(line_input_text, font_num, 10.0, 120.0, 0.45, 0.45, 1.0, 1.0, 1.0, 1.0);
			}
			else if (s_setting[1] == "jp")
			{
				Draw(line_msg_jp[26], 0, 10.0, 110.0, 0.45, 0.45, 1.0, 1.0, 1.0, 1.0);
				Draw(line_input_text, font_num, 10.0, 120.0, 0.45, 0.45, 1.0, 1.0, 1.0, 1.0);
			}
		}
		else if (line_send_check[1])
		{
			if (s_setting[1] == "en")
				Draw(line_msg_en[27], 0, 10.0, 110.0, 0.45, 0.45, 1.0, 1.0, 1.0, 1.0);
			else if (s_setting[1] == "jp")
				Draw(line_msg_jp[27], 0, 10.0, 110.0, 0.45, 0.45, 1.0, 1.0, 1.0, 1.0);

			Draw_texture(line_stickers_images, dammy_tint, line_selected_sticker_num, 230.0, 115.0, 45.0, 45.0);
		}
		else if (line_send_check[2])
		{
			if (s_setting[1] == "en")
			{
				Draw(line_msg_en[30], 0, 10.0, 110.0, 0.45, 0.45, 1.0, 1.0, 1.0, 1.0);
				Draw(line_msg_en[31], 0, 10.0, 120.0, 0.45, 0.45, 1.0, 1.0, 1.0, 1.0);
				Draw(line_msg_en[32] + Expl_query_current_patch(), 0, 10.0, 130.0, 0.45, 0.45, 1.0, 1.0, 1.0, 1.0);
				Draw(line_msg_en[33] + Expl_query_file_name((int)Expl_query_selected_num(EXPL_SELECTED_FILE_NUM) + (int)Expl_query_view_offset_y()), 0, 10.0, 140.0, 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
			}
			else if (s_setting[1] == "jp")
			{
				Draw(line_msg_jp[30], 0, 10.0, 110.0, 0.45, 0.45, 1.0, 1.0, 1.0, 1.0);
				Draw(line_msg_jp[31], 0, 10.0, 120.0, 0.45, 0.45, 1.0, 1.0, 1.0, 1.0);
				Draw(line_msg_jp[32] + Expl_query_current_patch(), 0, 10.0, 130.0, 0.45, 0.45, 1.0, 1.0, 1.0, 1.0);
				Draw(line_msg_jp[33] + Expl_query_file_name((int)Expl_query_selected_num(EXPL_SELECTED_FILE_NUM) + (int)Expl_query_view_offset_y()), 0, 10.0, 140.0, 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
			}
		}
	}
	else if (line_select_sticker_request)
	{
		Draw_texture(Square_image, aqua_tint, 0, 10.0, 140.0, 300.0, 90.0);
		Draw_texture(Square_image, weak_yellow_tint, 0, 290.0, 120.0, 20.0, 20.0);
		Draw("X", 0, 292.5, 120.0, 0.7, 0.7, 1.0, 0.0, 0.0, 1.0);

		for (int i = 0; i < 5; i++)
		{
			Draw_texture(Square_image, weak_yellow_tint, 0, 10.0 + (i * 60.0), 140.0, 30.0, 10.0);
			Draw_texture(Square_image, weak_red_tint, 0, 40.0 + (i * 60.0), 140.0, 30.0, 10.0);
		}
		Draw_texture(Square_image, blue_tint, 0, 10.0 + (line_selected_sticker_tab_num * 30.0), 140.0, 30.0, 10.0);

		draw_x = 20.0;
		draw_y = 150.0;
		for (int i = 1 + (line_selected_sticker_tab_num * 12); i < 13 + (line_selected_sticker_tab_num * 12); i++)
		{
			Draw_texture(Square_image, weak_blue_tint, 0, draw_x, draw_y, 30.0, 30.0);
			Draw_texture(line_stickers_images, dammy_tint, i, draw_x, draw_y, 30.0, 30.0);
			draw_x += 50.0;
			if (draw_x > 271.0)
			{
				draw_x = 20.0;
				draw_y += 40.0;
			}
		}
	}
	else if (line_select_file_request)
	{
		Draw_texture(Square_image, aqua_tint, 0, 10.0, 20.0, 300.0, 190.0);
		
		if (s_setting[1] == "en")
			Draw(line_msg_en[35], 0, 12.5, 185.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
		else if (s_setting[1] == "jp")
			Draw(line_msg_jp[35], 0, 12.5, 185.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);

		Draw(Expl_query_current_patch(), 0, 12.5, 195.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);
		for (int i = 0; i < 16; i++)
		{
			if(i == (int)Expl_query_selected_num(EXPL_SELECTED_FILE_NUM))
				Draw(Expl_query_file_name(i + (int)Expl_query_view_offset_y()) + "(" + Expl_query_type(i + (int)Expl_query_view_offset_y()) + ")", 0, 12.5, 20.0 + (i * 10.0), 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
			else
				Draw(Expl_query_file_name(i + (int)Expl_query_view_offset_y()) + "(" + Expl_query_type(i + (int)Expl_query_view_offset_y()) + ")", 0, 12.5, 20.0 + (i * 10.0), 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
		}
	}

	if (Err_query_error_show_flag())
		Draw_error();

	Draw_texture(Square_image, black_tint, 0, 0.0, 225.0, 320.0, 15.0);
	Draw(s_bot_button_string[0], 0, 30.0f, 220.0f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 1.0f);
	if (Hid_query_key_held_state(KEY_H_TOUCH))
		Draw(s_circle_string, 0, Hid_query_touch_pos(true), Hid_query_touch_pos(false), 0.20f, 0.20f, 1.0f, 0.0f, 0.0f, 1.0f);

	Draw_apply_draw();
	s_frame_time = osTickCounterRead(&s_tcount_frame_time);
	s_fps += 1;

	Hid_set_disable_flag(true);
	if (line_type_msg_request)
	{
		memset(s_swkb_input_text, 0x0, 8192);
		swkbdInit(&main_swkbd, SWKBD_TYPE_NORMAL, 2, 8192);
		swkbdSetHintText(&main_swkbd, "メッセージを入力 / Type message here.");
		swkbdSetValidation(&main_swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
		swkbdSetFeatures(&main_swkbd, SWKBD_PREDICTIVE_INPUT);
		swkbdSetInitialText(&main_swkbd, s_clipboards[0].c_str());
		swkbdSetDictWord(&s_swkb_words[0], "ぬべ", "壁|՞ةڼ)イーヒヒヒヒヒヒｗｗｗｗｗｗｗｗｗｗｗ");
		swkbdSetDictWord(&s_swkb_words[1], "ぬべ", "┌(☝┌՞ ՞)☝キエェェェエェェwwwww");
		swkbdSetDictWord(&s_swkb_words[2], "ぬべ", "┌(┌ ՞ةڼ)┐<ｷｴｪｪｪｴｴｪｪｪ");
		swkbdSetDictWord(&s_swkb_words[3], "ぬべ", "└(՞ةڼ◔)」");
		swkbdSetDictWord(&s_swkb_words[4], "ぬべ", "(  ՞ةڼ  )");
		swkbdSetDictWord(&s_swkb_words[5], "ぬべ", "└(՞ةڼ◔)」");
		swkbdSetDictWord(&s_swkb_words[6], "びぇ", "。゜( ;⊃՞ةڼ⊂; )゜。びぇぇえええんｗｗｗｗ");
		swkbdSetDictWord(&s_swkb_words[7], "うえ", "(✌ ՞ةڼ ✌ )");
		swkbdSetDictionary(&main_swkbd, s_swkb_words, 8);

		swkbdSetStatusData(&main_swkbd, &main_swkbd_status, true, true);
		swkbdSetLearningData(&main_swkbd, &s_swkb_learn_data, true, true);
		s_swkb_press_button = swkbdInputText(&main_swkbd, s_swkb_input_text, 8192);
		if (s_swkb_press_button == SWKBD_BUTTON_RIGHT)
		{
			line_input_text = s_swkb_input_text;
			if (line_input_text.length() > 4000)
				line_input_text = line_input_text.substr(0, 3990);

			line_send_check[0] = true;
		}
		line_type_msg_request = false;
	}
	if (line_type_id_request)
	{
		memset(s_swkb_input_text, 0x0, 8192);
		swkbdInit(&main_swkbd, SWKBD_TYPE_QWERTY, 2, 39);
		swkbdSetHintText(&main_swkbd, "idを入力 / Type id here.");
		swkbdSetInitialText(&main_swkbd, s_clipboards[0].c_str());
		swkbdSetValidation(&main_swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
		s_swkb_press_button = swkbdInputText(&main_swkbd, s_swkb_input_text, 39);
		if (!s_swkb_press_button == SWKBD_BUTTON_LEFT)
		{
			main_log_num_return = Log_log_save("Line/Main/fs", "Save_new_id...", 1234567890, false);
			main_result = Line_save_new_id(s_swkb_input_text, "/Line/to/");
			Log_log_add(main_log_num_return, main_result.string, main_result.code, false);
		}
		main_log_num_return = Log_log_save("Line/Main/fs", "Read_id...", 1234567890, false);
		main_result = Line_read_id("/Line/to/");
		Log_log_add(main_log_num_return, main_result.string, main_result.code, false);
		line_type_id_request = false;
	}
	if (line_type_main_url_request)
	{
		memset(s_swkb_input_text, 0x0, 8192);
		swkbdInit(&main_swkbd, SWKBD_TYPE_NORMAL, 2, 8192);
		swkbdSetHintText(&main_swkbd, "URLを入力 / Type your url here.");
		swkbdSetInitialText(&main_swkbd, line_main_url.c_str());
		swkbdSetValidation(&main_swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
		swkbdSetFeatures(&main_swkbd, SWKBD_PREDICTIVE_INPUT);
		swkbdSetDictWord(&s_swkb_words[0], "h", "https://script.google.com/macros/s/");
		swkbdSetDictionary(&main_swkbd, s_swkb_words, 1);
		s_swkb_press_button = swkbdInputText(&main_swkbd, s_swkb_input_text, 512);
		if (!s_swkb_press_button == SWKBD_BUTTON_LEFT)
		{
			main_log_num_return = Log_log_save("Line/Main/fs", "Save_to_file(main_url.txt)...", 1234567890, false);
			main_result = Share_save_to_file("main_url.txt", (u8*)s_swkb_input_text, sizeof(s_swkb_input_text), "/Line/", true, main_fs_handle, main_fs_archive);
			Log_log_add(main_log_num_return, main_result.string, main_result.code, false);
			line_main_url = s_swkb_input_text;
		}

		line_type_main_url_request = false;
	}
	if (line_type_app_ps_request)
	{
		memset(s_swkb_input_text, 0x0, 8192);
		swkbdInit(&main_swkbd, SWKBD_TYPE_QWERTY, 2, 512);
		swkbdSetHintText(&main_swkbd, "アプリパスワードを入力 / Type app password here.");
		swkbdSetValidation(&main_swkbd, SWKBD_ANYTHING, 0, 0);
		s_swkb_press_button = swkbdInputText(&main_swkbd, s_swkb_input_text, 512);
		swkbdSetPasswordMode(&main_swkbd, SWKBD_PASSWORD_HIDE);
		if (!s_swkb_press_button == SWKBD_BUTTON_LEFT)
		{
			main_log_num_return = Log_log_save("Line/Main/fs", "Save_to_file(auth)...", 1234567890, false);
			main_result = Share_save_to_file("auth", (u8*)s_swkb_input_text, sizeof(s_swkb_input_text), "/Line/", true, main_fs_handle, main_fs_archive);
			Log_log_add(main_log_num_return, main_result.string, main_result.code, false);
		}

		line_type_app_ps_request = false;
	}
	if (line_type_script_ps_request)
	{
		memset(s_swkb_input_text, 0x0, 8192);
		swkbdInit(&main_swkbd, SWKBD_TYPE_QWERTY, 2, 512);
		swkbdSetHintText(&main_swkbd, "スクリプトパスワードを入力 / Type script password here.");
		swkbdSetValidation(&main_swkbd, SWKBD_ANYTHING, 0, 0);
		swkbdSetPasswordMode(&main_swkbd, SWKBD_PASSWORD_HIDE);
		s_swkb_press_button = swkbdInputText(&main_swkbd, s_swkb_input_text, 512);
		if (!s_swkb_press_button == SWKBD_BUTTON_LEFT)
		{
			main_log_num_return = Log_log_save("Line/Main/fs", "Save_to_file(script_auth)...", 1234567890, false);
			main_result = Share_save_to_file("script_auth", (u8*)s_swkb_input_text, sizeof(s_swkb_input_text), "/Line/", true, main_fs_handle, main_fs_archive);
			Log_log_add(main_log_num_return, main_result.string, main_result.code, false);
			line_script_auth = s_swkb_input_text;
		}

		line_type_script_ps_request = false;
	}
}

void Line_log_download_thread(void* arg)
{
	Log_log_save("Line/Log dl thread", "Thread started.", 1234567890, false);
	u8* httpc_buffer ;
	u32 downloaded_log_size;
	u32 status_code;
	int log_download_log_num_return;
	size_t check;
	std::string file_name;
	std::string downloaded_data_string;
	std::string check_string = "<success>";
	FS_Archive log_dl_fs_archive = 0;
	Handle log_dl_fs_handle = 0;
	Result_with_string log_download_result;

	while (line_log_dl_thread_run)
	{
		if (line_auto_update || line_dl_log_request)
		{
			downloaded_log_size = 0;
			status_code = 0;
			httpc_buffer = (u8*)malloc(line_log_httpc_buffer_size);

			if (httpc_buffer == NULL)
			{
				Err_set_error_message("[Error] Out of memory.", "Couldn't allocate 'httpc buffer'(" + std::to_string(line_log_httpc_buffer_size / 1024) + "KB). ", "Line/Log dl thread", OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				Log_log_save("Line/Log dl thread", "[Error] Out of memory. ", OUT_OF_MEMORY, false);
				line_auto_update = false;
			}
			else
			{
				memset(httpc_buffer, 0x0, line_log_httpc_buffer_size);
				log_download_log_num_return = Log_log_save("Line/Log dl thread/httpc", "Downloading logs...", 1234567890, false);
				log_download_result = Httpc_dl_data(line_main_url + "?id=" + line_ids[line_selected_room_num] + "&script_auth=" + line_script_auth + "&gas_ver=" + std::to_string(s_current_gas_ver), httpc_buffer, line_log_httpc_buffer_size, &downloaded_log_size, &status_code, true);
				line_log_dl_progress = Httpc_query_dl_progress();
				if (log_download_result.code == 0)
				{
					line_log_data = (char*)httpc_buffer;
					check = std::string::npos;
					check = line_log_data.find("<success>");
					if (check != std::string::npos)
					{
						Log_log_add(log_download_log_num_return, log_download_result.string + std::to_string(downloaded_log_size / 1024) + "KB (" + std::to_string(downloaded_log_size) + "B) ", log_download_result.code, false);
						line_log_data = line_log_data.substr(0, line_log_data.length() - check_string.length());
						line_parse_log_request = true;
						file_name = line_ids[line_selected_room_num].substr(0, 16);
						log_download_log_num_return = Log_log_save("Line/Log dl thread/fs", "Save_to_file...", 1234567890, false);
						log_download_result = Share_save_to_file(file_name, (u8*)httpc_buffer, downloaded_log_size - (u32)check_string.length(), "/Line/", true, log_dl_fs_handle, log_dl_fs_archive);
						Log_log_add(log_download_log_num_return, log_download_result.string, log_download_result.code, false);
						if (log_download_result.code != 0)
						{
							Err_set_error_message(log_download_result.string, log_download_result.error_description, "Line/Log dl thread", log_download_result.code);
							Err_set_error_show_flag(true);
						}
					}
					else
					{
						Err_set_error_message("Log download failed.", line_log_data, "Line/Log dl thread/httpc", GAS_RETURNED_NOT_SUCCESS);
						Err_set_error_show_flag(true);
						Log_log_add(log_download_log_num_return, "[Error] Log download failed. " + std::to_string(downloaded_log_size / 1024) + "KB (" + std::to_string(downloaded_log_size) + "B) ", -4, false);
						line_auto_update = false;
					}
				}
				else
				{
					Err_set_error_message(log_download_result.string, log_download_result.error_description, "Line/Log dl thread/httpc", log_download_result.code);
					Err_set_error_show_flag(true);
					Log_log_add(log_download_log_num_return, log_download_result.string + std::to_string(downloaded_log_size / 1024) + "KB (" + std::to_string(downloaded_log_size) + "B) ", log_download_result.code, false);
					line_auto_update = false;
				}
			}
			free(httpc_buffer);
			httpc_buffer = NULL;
			line_dl_log_request = false;
		}
		usleep(100000);

		while (line_thread_suspend)
			usleep(250000);
	}
	Log_log_save("Line/Log dl thread", "Thread exit.", 1234567890, false);
}

void Line_send_message_thread(void* arg)
{
	Log_log_save("Line/Send msg thread", "Thread started.", 1234567890, false);
	bool failed = false;
	u8* httpc_buffer;
	u8* content_fs_buffer[2];
	u8* check;
	u32 status_code = 0;
	u32 dl_size = 0;
	u32 total_read_size;
	u32 read_size;
	u64 file_size;
	int log_num[2];
	int num_of_loop = 0;
	std::string send_data;
	std::string encoded_data;
	std::string response_string;
	FS_Archive fs_archive = 0;
	Handle fs_handle = 0;
	Result_with_string result;

	while (line_send_msg_thread_run)
	{
		if (line_send_request[0] || line_send_request[1] || line_send_request[2])
		{
			if (line_send_request[0])
				log_num[0] = Log_log_save("Line/Send msg thread/httpc", "Sending a message...", 1234567890, false);
			else if (line_send_request[1])
				log_num[0] = Log_log_save("Line/Send msg thread/httpc", "Sending a sticker...", 1234567890, false);
			else if (line_send_request[2])
				log_num[0] = Log_log_save("Line/Send msg thread/httpc", "Sending a content...", 1234567890, false);
			else
				log_num[0] = Log_log_save("Line/Send msg thread/httpc", "...", 1234567890, false);

			failed = false;
			line_send_success = false;
			line_sending_msg = true;
			dl_size = 0;
			file_size = 0;
			read_size = 0;
			total_read_size = 0;
			line_step_max = 1;
			line_current_step = 0;
			httpc_buffer = (u8*)malloc(0x5000);

			if (httpc_buffer == NULL)
			{
				Err_set_error_message("[Error] Out of memory.", "Couldn't allocate 'httpc buffer'(" + std::to_string(0x5000 / 1024) + "KB). ", "Line/Send msg thread", OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				Log_log_save("Line/Send msg thread", "[Error] Out of memory. ", OUT_OF_MEMORY, false);
				failed = true;
			}
			else
				memset(httpc_buffer, 0x0, 0x5000);

			if (line_send_request[2] && !failed)
			{
				content_fs_buffer[0] = (u8*)malloc(line_send_fs_buffer_size);
				content_fs_buffer[1] = (u8*)malloc(line_send_fs_cache_buffer_size);
				if (content_fs_buffer[0] == NULL || content_fs_buffer[1] == NULL)
				{
					if (content_fs_buffer[0] == NULL && content_fs_buffer[1] == NULL)
						Err_set_error_message("[Error] Out of memory.", "Couldn't allocate 'content fs buffer'(" + std::to_string(line_send_fs_buffer_size / 1024) + "KB) and\n'content fs cache buffer'(" + std::to_string(line_send_fs_cache_buffer_size / 1024) + "KB). ", "Line/Send msg thread", OUT_OF_MEMORY);
					else if (content_fs_buffer[0])
						Err_set_error_message("[Error] Out of memory.", "Couldn't allocate 'content fs buffer'(" + std::to_string(line_send_fs_buffer_size / 1024) + "KB). ", "Line/Send msg thread", OUT_OF_MEMORY);
					else if (content_fs_buffer[1] == NULL)
						Err_set_error_message("[Error] Out of memory.", "Couldn't allocate 'content fs cache buffer'(" + std::to_string(line_send_fs_cache_buffer_size / 1024) + "KB). ", "Line/Send msg thread", OUT_OF_MEMORY);

					Err_set_error_show_flag(true);
					Log_log_save("Line/Send msg thread", "[Error] Out of memory. ", OUT_OF_MEMORY, false);
					failed = true;
				}
				else
				{
					memset(content_fs_buffer[0], 0x0, line_send_fs_buffer_size);
					memset(content_fs_buffer[1], 0x0, line_send_fs_cache_buffer_size);
				}
			}

			if (!failed)
			{
				if (line_send_request[0])
					send_data = "{ \"type\": \"send_text\",\"id\" : \"" + line_ids[line_selected_room_num] + "\",\"message\" : \"" + line_input_text + "\",\"auth\" : \"" + line_script_auth + "\",\"gas_ver\" : \"" + std::to_string(s_current_gas_ver) + "\" }";
				else if (line_send_request[1])
				{
					if (line_selected_sticker_num >= 1 && line_selected_sticker_num <= 40)
						send_data = "{ \"type\": \"send_sticker\",\"id\" : \"" + line_ids[line_selected_room_num] + "\",\"package_id\" : \"11537\" ,\"sticker_id\" : \"" + std::to_string(line_sticker_num_list[line_selected_sticker_num]) + "\",\"auth\" : \"" + line_script_auth + "\",\"gas_ver\" : \"" + std::to_string(s_current_gas_ver) + "\" }";
					else if (line_selected_sticker_num >= 41 && line_selected_sticker_num <= 80)
						send_data = "{ \"type\": \"send_sticker\",\"id\" : \"" + line_ids[line_selected_room_num] + "\",\"package_id\" : \"11538\" ,\"sticker_id\" : \"" + std::to_string(line_sticker_num_list[line_selected_sticker_num]) + "\",\"auth\" : \"" + line_script_auth + "\",\"gas_ver\" : \"" + std::to_string(s_current_gas_ver) + "\" }";
					else if (line_selected_sticker_num >= 81 && line_selected_sticker_num <= 120)
						send_data = "{ \"type\": \"send_sticker\",\"id\" : \"" + line_ids[line_selected_room_num] + "\",\"package_id\" : \"11539\" ,\"sticker_id\" : \"" + std::to_string(line_sticker_num_list[line_selected_sticker_num]) + "\",\"auth\" : \"" + line_script_auth + "\",\"gas_ver\" : \"" + std::to_string(s_current_gas_ver) + "\" }";
				}
				else if (line_send_request[2])
				{
					log_num[1] = Log_log_save("Line/Send msg thread/fs", "Share_check_file_size(" + line_send_file_name + ")...", 1234567890, false);
					result = Share_check_file_size(line_send_file_name, line_send_file_dir, &file_size, fs_handle, fs_archive);
					Log_log_add(log_num[1], result.string, result.code, false);
					if ((int)file_size > line_send_fs_buffer_size)
					{
						Err_set_error_message("File size is too big. ", "File size is too big. \nFile size is " + std::to_string((double)file_size / 1024.0 / 1024.0) + "MB. ", "Line/Send msg thread", FILE_SIZE_IS_TOO_BIG);
						Err_set_error_show_flag(true);
						Log_log_add(log_num[0], "File size is too big. ", FILE_SIZE_IS_TOO_BIG, false);
						failed = true;
					}
					else
					{
						num_of_loop = ((int)file_size / line_send_fs_cache_buffer_size);
						for (int i = 0; i <= num_of_loop; i++)
						{
							log_num[1] = Log_log_save("Line/Send msg thread/fs", "Share_load_from_file(" + line_send_file_name + ")...", 1234567890, false);
							result = Share_load_from_file_with_range(line_send_file_name, content_fs_buffer[1], line_send_fs_cache_buffer_size, u64(i * line_send_fs_cache_buffer_size), &read_size, line_send_file_dir, fs_handle, fs_archive);
							Log_log_add(log_num[1], result.string, result.code, false);
							memcpy(content_fs_buffer[0] + (i * line_send_fs_cache_buffer_size), content_fs_buffer[1], read_size);
							total_read_size += read_size;
							if (result.code != 0)
							{
								Err_set_error_message(result.string, result.error_description, "Line/Send msg thread", result.code);
								Err_set_error_show_flag(true);
								Log_log_add(log_num[0], result.string, result.code, false);
								failed = true;
								break;
							}
						}

						free(content_fs_buffer[1]);
						content_fs_buffer[1] = NULL;

						if (!failed)
						{
							check = (u8*)malloc((total_read_size * 2) + 0x1000);
							if (check == NULL)
							{
								Err_set_error_message("Out of memory", "Not enough memory", "Line/Send msg thread", OUT_OF_MEMORY);
								Err_set_error_show_flag(true);
								Log_log_add(log_num[0], "Out of memory", OUT_OF_MEMORY, false);
								failed = true;
							}
							free(check);
							check = NULL;

							if (!failed)
							{
								encoded_data = base64_encode((unsigned char const*)content_fs_buffer[0], (int)total_read_size);
								free(content_fs_buffer[0]);
								content_fs_buffer[0] = NULL;

								num_of_loop = ((int)encoded_data.length() / line_send_fs_cache_buffer_size);
								line_step_max = num_of_loop + 2;
								for (int i = 0; i <= num_of_loop; i++)
								{
									send_data.reserve(100);
									check = (u8*)malloc((encoded_data.length() * 1.25)+ line_send_fs_cache_buffer_size);
									if (check == NULL)
									{
										Err_set_error_message("Out of memory", "Not enough memory", "Line/Send msg thread", OUT_OF_MEMORY);
										Err_set_error_show_flag(true);
										Log_log_add(log_num[0], "Out of memory", OUT_OF_MEMORY, false);
										failed = true;
										break;
									}
									free(check);
									check = NULL;

									if (num_of_loop <= i)
										send_data = "{ \"type\": \"upload_content\",\"id\" : \"" + line_ids[line_selected_room_num] + "\",\"count\" : \"" + std::to_string(i) + "\",\"name\" : \"" + line_send_file_name + "\",\"content_data\" : \"" + encoded_data.substr((i * line_send_fs_cache_buffer_size), (encoded_data.length() - (i * line_send_fs_cache_buffer_size))) + "\",\"auth\" : \"" + line_script_auth + "\",\"gas_ver\" : \"" + std::to_string(s_current_gas_ver) + "\" }";
									else
										send_data = "{ \"type\": \"upload_content\",\"id\" : \"" + line_ids[line_selected_room_num] + "\",\"count\" : \"" + std::to_string(i) + "\",\"name\" : \"" + line_send_file_name + "\",\"content_data\" : \"" + encoded_data.substr((i * line_send_fs_cache_buffer_size), line_send_fs_cache_buffer_size) + "\",\"auth\" : \"" + line_script_auth + "\",\"gas_ver\" : \"" + std::to_string(s_current_gas_ver) + "\" }";

									log_num[1] = Log_log_save("Line/Send msg thread/httpc", "Uploading a content(" + std::to_string(i) + "/" + std::to_string(num_of_loop) + ")...", 1234567890, false);
									Log_log_save("", std::to_string(send_data.length()) + " cap : " + std::to_string(send_data.capacity()), 1234567890, false);

									result = Httpc_post_and_dl_data(line_main_url, (char*)send_data.c_str(), send_data.length(), httpc_buffer, 0x5000, &dl_size, &status_code, true);
									line_current_step++;
									if (result.code == 0)
									{
										response_string = (char*)httpc_buffer;
										if (response_string == "Success")
											Log_log_add(log_num[1], result.string, result.code, false);
										else
										{
											Err_set_error_message("Upload content failed.", (char*)httpc_buffer, "Line/Send msg thread", GAS_RETURNED_NOT_SUCCESS);
											Err_set_error_show_flag(true);
											Log_log_add(log_num[1], "[Error] Send failed. ", GAS_RETURNED_NOT_SUCCESS, false);
											failed = true;
											break;
										}
									}
									else
									{
										Err_set_error_message(result.string, result.error_description, "Line/Send msg thread", result.code);
										Err_set_error_show_flag(true);
										Log_log_add(log_num[1], result.string, result.code, false);
										failed = true;
										break;
									}
								}
							}

							if(!failed)
								send_data = "{ \"type\": \"send_content\",\"id\" : \"" + line_ids[line_selected_room_num] + "\",\"name\" : \"" + line_send_file_name + "\",\"auth\" : \"" + line_script_auth + "\",\"gas_ver\" : \"" + std::to_string(s_current_gas_ver) + "\" }";
						}
						else
						{
							Err_set_error_message(result.string, result.error_description, "Line/Send msg thread", result.code);
							Err_set_error_show_flag(true);
							Log_log_add(log_num[0], result.string, result.code, false);
							failed = true;
						}
					}
				}
			}

			if (!failed)
			{
				result = Httpc_post_and_dl_data(line_main_url, (char*)send_data.c_str(), send_data.length(), httpc_buffer, 0x5000, &dl_size, &status_code, true);
				line_current_step++;
				if (result.code == 0)
				{
					response_string = (char*)httpc_buffer;
					if (response_string == "Success")
					{
						line_send_success = true;
						Log_log_add(log_num[0], result.string, result.code, false);
					}
					else
					{
						Err_set_error_message("Send message/sticker failed.", (char*)httpc_buffer, "Line/Send msg thread", GAS_RETURNED_NOT_SUCCESS);
						Err_set_error_show_flag(true);
						Log_log_add(log_num[0], "[Error] Send failed. ", GAS_RETURNED_NOT_SUCCESS, false);
					}
				}
				else
				{
					Err_set_error_message(result.string, result.error_description, "Line/Send msg thread", result.code);
					Err_set_error_show_flag(true);
					Log_log_add(log_num[0], result.string, result.code, false);
				}
			}

			response_string = "";
			response_string.reserve(1);
			send_data = "";
			send_data.reserve(1);
			encoded_data = "";
			encoded_data.reserve(1);
			free(httpc_buffer);
			httpc_buffer = NULL;
			free(content_fs_buffer[0]);
			content_fs_buffer[0] = NULL;
			free(content_fs_buffer[1]);
			content_fs_buffer[1] = NULL;
			free(check);
			check = NULL;

			line_sending_msg = false;
			if (line_send_request[0])
				line_send_request[0] = false;
			else if (line_send_request[1])
				line_send_request[1] = false;
			else if (line_send_request[2])
				line_send_request[2] = false;
		}
		usleep(100000);

		while (line_thread_suspend)
			usleep(250000);
	}
	Log_log_save("Line/Send msg thread", "Thread exit.", 1234567890, false);
}

Result_with_string Line_save_new_id(std::string id, std::string dir_path)
{
	bool failed = false;
	FS_Archive fs_save_id_archive = 0;
	Result_with_string save_id_result;
	save_id_result.code = 0;
	save_id_result.string = "[Success] ";

	save_id_result.code = FSUSER_OpenArchive(&fs_save_id_archive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
	if (save_id_result.code != 0)
	{
		save_id_result.string = "[Error] FSUSER_OpenArchive failed. ";
		failed = true;
	}

	if (!failed)
	{
		save_id_result.code = FSUSER_CreateDirectory(fs_save_id_archive, fsMakePath(PATH_ASCII, dir_path.c_str()), FS_ATTRIBUTE_DIRECTORY);
		if (save_id_result.code != 0 && save_id_result.code != (s32)0xC82044BE)//#0xC82044BE directory already exist
		{
			save_id_result.string = "[Error] FSUSER_CreateDirectory failed. ";
			failed = true;
		}
	}

	if (!failed)
	{
		dir_path += id;
		save_id_result.code = FSUSER_CreateFile(fs_save_id_archive, fsMakePath(PATH_ASCII, dir_path.c_str()), FS_ATTRIBUTE_ARCHIVE, 0);
		if (save_id_result.code != 0)
		{
			save_id_result.string = "[Error] FSUSER_CreateFile failed. ";
			failed = true;
		}
	}

	FSUSER_CloseArchive(fs_save_id_archive);

	return save_id_result;
}

Result_with_string Line_read_id(std::string dir_path)
{
	int num_of_detected = 0;
	std::string type[100];
	Result_with_string read_id_result;

	for(int i = 0; i < 100; i ++)
		line_ids[i] = "";
	
	read_id_result = Share_read_dir(&num_of_detected, line_ids, 100, type, 100, "/Line/to/");

	return read_id_result;
}

Result_with_string Line_load_log_from_sd(std::string file_name)
{
	u8* fs_buffer;
	u32 read_size = 0;
	FS_Archive fs_archive = 0;
	Handle fs_handle = 0;
	Result_with_string result;
	result.code = 0;
	result.string = "[Success] ";
	result.error_description = "N/A";

	fs_buffer = (u8*)malloc(line_log_fs_buffer_size);
	if (fs_buffer == NULL)
	{
		result.code = OUT_OF_MEMORY;
		result.string = "[Error] Out of memory.";
		result.error_description = "Couldn't allocate 'fs buffer'(" + std::to_string(line_log_fs_buffer_size / 1024) + "KB).";
	}
	else
	{
		memset(fs_buffer, 0x0, line_log_fs_buffer_size);
		line_log_data = "";
		result = Share_load_from_file(file_name, fs_buffer, line_log_fs_buffer_size, &read_size, "/Line/", fs_handle, fs_archive);

		if (result.code == 0)
		{
			line_log_data = (char*)fs_buffer;
			line_parse_log_request = true;
		}
	}

	free(fs_buffer);
	return result;
}

int Line_stickers_num_to_textures_num(int sticker_num)
{
	int texture_num = 0;
	for (int i = 1; i < 121; i++)
	{
		if (line_sticker_num_list[i] == sticker_num)
		{
			texture_num = i;
			break;
		}
	}
	return texture_num;
}

void Line_log_parse_thread(void* arg)
{
	Log_log_save("Line/Log parse thread", "Thread started.", 1234567890, false);
	int length_count = 0;
	int log_num[2];
	int cut_length = 60;
	int text_length = 0;
	size_t image_url_end_pos;
	size_t image_url_start_pos;
	size_t sticker_end_pos;
	size_t sticker_start_pos;
	size_t message_start_pos;
	size_t message_next_pos;
	size_t new_line_pos;
	size_t check_file_name_start_pos[2];
	bool failed = false;
	bool sticker_msg = false;
	char* parse_cache;
	std::string content_cache = "";
	std::string image_url_end = "</image_url>";
	std::string image_url_start = "<image_url>";
	std::string sticker_end = "</sticker>";
	std::string sticker_start = "<sticker>";
	std::string message_start = "";
	std::string image_message[3] = { "", "", "", };
	FS_Archive log_parse_fs_archive = 0;
	Handle log_parse_fs_handle = 0;
	Result_with_string log_parse_result;

	while (line_log_parse_thread_run)
	{
		if (line_thread_suspend)
			usleep(250000);
		else
		{
			if (line_parse_log_request)
			{
				log_num[0] = Log_log_save("Line/Log parse thread", "Parsing logs...", 1234567890, false);
				usleep(50000);

				message_start_pos = std::string::npos;
				message_next_pos = std::string::npos;
				image_url_start_pos = std::string::npos;
				image_url_end_pos = std::string::npos;
				sticker_end_pos = std::string::npos;
				sticker_start_pos = std::string::npos;
				new_line_pos = std::string::npos;
				check_file_name_start_pos[0] = std::string::npos;
				check_file_name_start_pos[1] = std::string::npos;
				line_num_of_msg = 0;
				line_num_of_lines = 10;
				length_count = 0;
				cut_length = 60;
				text_length = 0;
				failed = false;

				if (s_setting[1] == "en")
				{
					image_message[0] = line_msg_en[28];
					image_message[1] = line_msg_en[29];
				}
				else if (s_setting[1] == "jp")
				{
					image_message[0] = line_msg_jp[28];
					image_message[1] = line_msg_jp[29];
				}

				parse_cache = (char*)malloc(0x10000);
				if (parse_cache == NULL)
				{
					Err_set_error_message("[Error] Out of memory.", "Couldn't allocate 'parse cache'(" + std::to_string(0x10000 / 1024) + "KB). ", "Line/Log parse thread", OUT_OF_MEMORY);
					Err_set_error_show_flag(true);
					Log_log_save("Line/Log parse thread", "[Error] Out of memory. ", OUT_OF_MEMORY, false);
				}
				else
				{
					for (int i = 1; i <= 14; i++)
						s_clipboards[i] = "";
					for (int i = 0; i <= 299; i++)
						line_msg_log[i] = "";
					for (int i = 0; i < 59999; i++)
					{
						line_short_msg_log[i] = "";
						line_content[i] = "";
					}

					for (int i = 0; i <= 299; i++)
					{
						message_start_pos = line_log_data.find(message_start, (message_start_pos + message_start.length()));
						if (message_start_pos == std::string::npos)
							break;

						message_next_pos = line_log_data.find(message_start, (message_start_pos + message_start.length()));
						if (message_next_pos == std::string::npos)
							line_msg_log[i] = line_log_data.substr((message_start_pos + message_start.length()), line_log_data.length() - (message_start_pos + message_start.length()));
						else
							line_msg_log[i] = line_log_data.substr((message_start_pos + message_start.length()), message_next_pos - (message_start_pos + message_start.length()));

						line_num_of_msg++;
					}

					for (int i = 0; i < line_num_of_msg; i++)
					{
						sticker_msg = false;

						image_url_start_pos = line_msg_log[i].find(image_url_start);
						image_url_end_pos = line_msg_log[i].find(image_url_end);
						sticker_start_pos = line_msg_log[i].find(sticker_start);
						sticker_end_pos = line_msg_log[i].find(sticker_end);
						if (!(image_url_start_pos == std::string::npos || image_url_end_pos == std::string::npos))
						{
							line_content[line_num_of_lines + 1] = "<url>" + line_msg_log[i].substr((image_url_start_pos + image_url_start.length()), (image_url_end_pos - (image_url_start_pos + image_url_start.length()))) + "</url>";

							content_cache = line_msg_log[i].substr(0, image_url_start_pos);

							check_file_name_start_pos[0] = line_content[line_num_of_lines + 1].find("&id=");
							check_file_name_start_pos[1] = line_content[line_num_of_lines + 1].find("om/d/");
							if (check_file_name_start_pos[0] == std::string::npos && check_file_name_start_pos[1] == std::string::npos)
							{
								line_content[line_num_of_lines + 1] += "<type>image</type>";
								content_cache += image_message[0];
							}
							else
							{
								if(!(check_file_name_start_pos[0] == std::string::npos))
									image_message[2] = line_content[line_num_of_lines + 1].substr(check_file_name_start_pos[0] + 4);
								else if (!(check_file_name_start_pos[1] == std::string::npos))
									image_message[2] = line_content[line_num_of_lines + 1].substr(check_file_name_start_pos[1] + 5);

								if (image_message[2].length() > 33)
									image_message[2] = image_message[2].substr(0, 33);

								image_message[2] += ".jpg";
								log_num[1] = Log_log_save("Line/Log parse thread", "Share_check_file_exist(" + image_message[2] + ")...", 1234567890, false);
								log_parse_result = Share_check_file_exist(image_message[2], "/Line/images/", log_parse_fs_handle, log_parse_fs_archive);
								Log_log_add(log_num[1], log_parse_result.string, log_parse_result.code, false);
								if (log_parse_result.code == 0)
								{
									line_content[line_num_of_lines + 1] += "<type>exist_image</type>";
									content_cache += image_message[1];
								}
								else
								{
									line_content[line_num_of_lines + 1] += "<type>image</type>";
									content_cache += image_message[0];
								}
							}
							content_cache += line_msg_log[i].substr(image_url_end_pos + image_url_end.length(), line_msg_log[i].length() - (image_url_end_pos + image_url_end.length()));
							line_msg_log[i] = content_cache;
						}
						else if (!(sticker_start_pos == std::string::npos || sticker_end_pos == std::string::npos))
						{
							sticker_msg = true;
							line_content[line_num_of_lines + 2] = "<type>sticker</type>";
							content_cache = line_msg_log[i].substr(sticker_start_pos + sticker_start.length(), sticker_end_pos - (sticker_start_pos + sticker_start.length()));
							if (std::all_of(content_cache.cbegin(), content_cache.cend(), isdigit))
								line_content[line_num_of_lines + 2] += "<num>" + std::to_string(Line_stickers_num_to_textures_num(std::stoi(content_cache))) + "</num>";
							else
								line_content[line_num_of_lines + 2] += "<num>0</num>";

							content_cache = line_msg_log[i].substr(0, sticker_start_pos);
							content_cache += line_msg_log[i].substr(sticker_end_pos + sticker_end.length(), line_msg_log[i].length() - (sticker_end_pos + sticker_end.length()));				
							line_msg_log[i] = content_cache;
						}

						memset(parse_cache, 0x0, 0x10000);
						strcpy(parse_cache, line_msg_log[i].c_str());
						text_length = line_msg_log[i].length();

						while (true)
						{
							if (line_num_of_lines >= 59990)
							{
								failed = true;
								break;
							}

							if (length_count + cut_length >= text_length)
							{
								line_short_msg_log[59999] = line_msg_log[i].substr(length_count, cut_length);
								new_line_pos = line_short_msg_log[59999].find_first_of("\u000a");
								if (!(new_line_pos == std::string::npos))
								{
									cut_length = new_line_pos + 1;
									line_num_of_lines++;
									line_short_msg_log[line_num_of_lines] = line_msg_log[i].substr(length_count, cut_length);
									length_count += cut_length;
									cut_length = 60;
								}
								else
								{
									cut_length = text_length - length_count;
									line_num_of_lines++;
									line_short_msg_log[line_num_of_lines] = line_msg_log[i].substr(length_count, cut_length);
									break;
								}
							}
							else
							{
								int check_length = mblen(&parse_cache[length_count + cut_length], 4);
								if (check_length >= 1)
								{
									line_short_msg_log[59999] = line_msg_log[i].substr(length_count, cut_length);
									new_line_pos = line_short_msg_log[59999].find_first_of("\u000a");
									if (!(new_line_pos == std::string::npos))
										cut_length = new_line_pos + 1;

									line_num_of_lines++;
									line_short_msg_log[line_num_of_lines] = line_msg_log[i].substr(length_count, cut_length);
									length_count += cut_length;
									cut_length = 60;
								}
								else
									cut_length++;
							}
						}
						
						if(sticker_msg)
							line_num_of_lines += 3;
						else
							line_num_of_lines++;
	
						length_count = 0;
						cut_length = 60;

						line_max_y = (-line_text_interval * line_num_of_lines) + 100;
						line_text_y_cache = line_max_y;
					}

					line_max_y = (-line_text_interval * line_num_of_lines) + 100;
					line_text_y_cache = line_max_y;

					if (failed)
					{
						Err_set_error_message("[Error] Parsing aborted due to too many messages. ", "Parsing aborted due to too many messages.", "Line/Log parse thread", TOO_MANY_MESSAGES);
						Err_set_error_show_flag(true);
						Log_log_add(log_num[0], "[Error] Parsing aborted due to too many messages. ", TOO_MANY_MESSAGES, false);
					}
					else
						Log_log_add(log_num[0], "[Success] ", 1234567890, false);						
				}

				free(parse_cache);
				parse_cache = NULL;
				line_parse_log_request = false;
			}
			usleep(100000);
		}
	}
	Log_log_save("Line/Log parse thread", "Thread exit.", 1234567890, false);
}

void Line_log_load_thread(void* arg)
{
	Log_log_save("Line/Log load thread", "Thread started.", 1234567890, false);
	int log_load_log_return_num = 0;
	Result_with_string log_load_result;
	while (line_log_load_thread_run)
	{
		if (line_thread_suspend)
			usleep(250000);
		else
		{
			if (line_load_log_request)
			{
				log_load_log_return_num = Log_log_save("Line/Log load thread/fs", "Load_from_log_sd(" + line_ids[line_selected_room_num].substr(0, 16) + ")...", 1234567890, false);
				log_load_result = Line_load_log_from_sd(line_ids[line_selected_room_num].substr(0, 16));
				Log_log_add(log_load_log_return_num, log_load_result.string, log_load_result.code, false);
				if (log_load_result.code != 0)
				{
					Err_set_error_message(log_load_result.string, log_load_result.error_description, "Line/Log load thread", log_load_result.code);
					Err_set_error_show_flag(true);
				}
				line_load_log_request = false;
			}
			usleep(100000);
		}
	}
	Log_log_save("Line/Log load thread", "Thread exit.", 1234567890, false);
}

void Line_exit(void)
{
	Log_log_save("Line/Exit", "Exiting...", 1234567890, s_debug_slow);
	u64 time_out = 10000000000;
	int log_num;
	bool failed = false;
	Result_with_string result;

	Draw_progress("0/1 [Line] Exiting threads...");
	line_already_init = false;
	line_thread_suspend = false;
	line_log_dl_thread_run = false;
	line_log_load_thread_run = false;
	line_log_parse_thread_run = false;
	line_send_msg_thread_run = false;

	log_num = Log_log_save("Line/Exit", "Exiting thread(0/3)...", 1234567890, s_debug_slow);
	result.code = threadJoin(line_dl_log_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, "[Success] ", result.code, s_debug_slow);
	else
	{
		failed = true;
		Log_log_add(log_num, "[Error] ", result.code, s_debug_slow);
	}

	log_num = Log_log_save("Line/Exit", "Exiting thread(1/3)...", 1234567890, s_debug_slow);
	result.code = threadJoin(line_load_log_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, "[Success] ", result.code, s_debug_slow);
	else
	{
		failed = true;
		Log_log_add(log_num, "[Error] ", result.code, s_debug_slow);
	}

	log_num = Log_log_save("Line/Exit", "Exiting thread(2/3)...", 1234567890, s_debug_slow);
	result.code = threadJoin(line_parse_log_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, "[Success] ", result.code, s_debug_slow);
	else
	{
		failed = true;
		Log_log_add(log_num, "[Error] ", result.code, s_debug_slow);
	}

	log_num = Log_log_save("Line/Exit", "Exiting thread(3/3)...", 1234567890, s_debug_slow);
	result.code = threadJoin(line_send_msg_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, "[Success] ", result.code, s_debug_slow);
	else
	{
		failed = true;
		Log_log_add(log_num, "[Error] ", result.code, s_debug_slow);
	}


	Draw_progress("1/1 [Line] Cleaning up...");
	for (int i = 0; i <= 299; i++)
	{
		line_msg_log[i] = "";
		line_msg_log[i].reserve(1);
	}
	for (int i = 0; i <= 59999; i++)
	{
		line_short_msg_log[i] = "";
		line_short_msg_log[i].reserve(1);
		line_content[i] = "";
		line_content[i].reserve(1);
	}

	line_log_data = "";
	line_log_data.reserve(1);

	Draw_free_texture(51);

	if(failed)
		Log_log_save("Line/Exit", "[Warn] Some function returned error.", 1234567890, s_debug_slow);

	Log_log_save("Line/Exit", "Exited.", 1234567890, s_debug_slow);
}

