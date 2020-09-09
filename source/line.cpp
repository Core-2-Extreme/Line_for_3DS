#include <3ds.h>
#include <algorithm>
#include <string>
#include <unistd.h>

#include "hid.hpp"
#include "stb_image/stb_image.h"
#include "base64/base64.h"
#include "draw.hpp"
#include "file.hpp"
#include "httpc.hpp"
#include "line.hpp"
#include "setting_menu.hpp"
#include "error.hpp"
#include "menu.hpp"
#include "explorer.hpp"
#include "log.hpp"
#include "types.hpp"
#include "swkbd.hpp"

/*For debug*/
bool line_debug_request = false;
int line_debug_found = 0;
std::string line_debug_text = "";
/*---------------------------------------------*/

/*For draw*/
bool line_need_reflesh = false;
bool line_pre_delete_id_check_request = false;
bool line_pre_auto_update = false;
bool line_pre_hide_id = false;
bool line_pre_parse_log_request = false;
bool line_pre_load_log_request = false;
bool line_pre_dl_log_request = false;
bool line_pre_send_check[3] = { false, false, false };
bool line_pre_send_request[3] = { false, false, false };
bool line_pre_select_sticker_request = false;
bool line_pre_select_file_request = false;
bool line_pre_select_chat_room_request = true;
bool line_pre_solve_short_url_request = false;
bool line_pre_dl_log_no_parse_request = false;
bool line_pre_dl_all_log_no_parse_request = false;
bool line_pre_sending_msg = false;
bool line_pre_send_success = false;
bool line_pre_icon_available[128];
int line_pre_num_of_logs = 150;
int line_pre_selected_menu_mode = 0;
int line_pre_selected_sticker_tab_num = 0;
int line_pre_selected_search_highlight_num = 0;
int line_pre_selected_highlight_num = 0;
int line_pre_total_data_size = 0;
int line_pre_sent_data_size = 0;
int line_pre_current_step = 0;
int line_pre_log_dl_progress = 0;
int line_pre_post_and_dl_progress = 0;
double line_pre_text_x = 0.0;
double line_pre_text_y = 0.0;
double line_pre_text_size = 0.66;
double line_pre_text_interval = 35.0;
double line_pre_selected_msg_num = 0.0;
/*---------------------------------------------*/

bool line_thread_suspend = false;
bool line_already_init = false;
bool line_delete_id_check_request = false;
bool line_delete_id_request = false;
bool line_main_run = false;
bool line_worker_thread_run = false;
bool line_log_dl_thread_run = false;
bool line_log_load_thread_run = false;
bool line_log_parse_thread_run = false;
bool line_send_msg_thread_run = false;
bool line_auto_update = false;
bool line_hide_id = false;
bool line_icon_dl_request = false;
bool line_parse_log_request = false;
bool line_load_log_request = false;
bool line_dl_log_request = false;
bool line_send_check[3] = { false, false, false };
bool line_send_request[3] = { false, false, false };
bool line_select_sticker_request = false;
bool line_select_file_request = false;
bool line_select_chat_room_request = true;
bool line_type_id_request = false;
bool line_type_app_ps_request = false;
bool line_type_msg_request = false;
bool line_type_main_url_request = false;
bool line_type_short_url_request = false;
bool line_check_main_url_request = false;
bool line_solve_short_url_request = false;
bool line_type_script_ps_request = false;
bool line_dl_log_no_parse_request = false;
bool line_dl_all_log_no_parse_request = false;
bool line_sending_msg = false;
bool line_send_success = false;
bool line_dl_log_failed[128];
bool line_icon_available[128];
int line_unread_msg_num[128];
int line_msg_offset[128];
int line_num_of_logs = 150;
int line_selected_menu_mode = 0;
int line_selected_room_num = 0;
int line_selected_sticker_tab_num = 0;
int line_selected_sticker_num = 0;
int line_selected_highlight_num = 0;
int line_selected_search_highlight_num = 0;
int line_total_data_size = 0;
int line_sent_data_size = 0;
int line_log_httpc_buffer_size = 0x200000;
int line_log_fs_buffer_size = 0x200000;
int line_send_fs_buffer_size = 0x1000000;
int line_send_fs_cache_buffer_size = 0x300000;
int line_step_max = 1;
int line_current_step = 0;
int line_num_of_ids = 0;
int line_log_dl_progress = 0;
int line_post_and_dl_progress = 0;
int line_num_of_msg = 0;
int line_num_of_lines = 0;
int line_short_msg_pos_start[4000];
int line_short_msg_pos_end[4000];
int line_search_result_pos[4000];
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
double line_msg_pos[4000];
double line_text_x = 0.0;
double line_text_y = 0.0;
double line_text_size = 0.66;
double line_text_interval = 35.0;
double line_max_y = 10.0;
double line_selected_msg_num = 0.0;
std::string line_script_auth = "";
std::string line_short_url = "";
std::string line_ids[128];
std::string line_names[128];
std::string line_icon_url[128];
std::string line_input_text = "";
std::string line_encoded_input_text = "";
std::string line_main_url = "";
std::string line_short_msg_log[60000];
std::string line_log_data = "";
std::string line_msg[LINE_NUM_OF_MSG];
std::string line_msg_log[4000];
std::string line_send_file_dir = "";
std::string line_send_file_name = "";
std::string line_content[60000];
std::string line_main_thread_string = "Line/Main";
std::string line_worker_thread_string = "Line/Worker thread";
std::string line_log_dl_thread_string = "Line/Log dl thread";
std::string line_send_msg_thread_string = "Line/Send msg thread";
std::string line_log_load_thread_string = "Line/Log load thread";
std::string line_parse_thread_string = "Line/Log parse thread";
std::string line_init_string = "Line/Init";
std::string line_exit_string = "Line/Exit";
std::string line_ver = "v1.6.0";
C3D_Tex* line_c3d_cache_tex[128];
Tex3DS_SubTexture* line_c3d_cache_subtex[128];
C2D_Image line_stickers_images[121], line_icon[128];
stbi_uc* line_stb_image[128];
Thread line_dl_log_thread, line_load_log_thread, line_send_msg_thread, line_parse_log_thread, line_worker_thread;

void Line_main(void)
{
	int font_num = 0;
	int msg_num_list[10];
	int num_of_words = 0;
	int max_length = 512;
	u32 feature[2];
	float text_red;
	float text_green;
	float text_blue;
	float text_alpha;
	float red[1], green[1], blue[1], alpha[1];
	float texture_size;
	float scroll_bar_y_size = 5.0;
	float scroll_bar_y_pos = (195.0 / (-line_max_y / -line_text_y));
	double draw_x;
	double draw_y;
	int msg_num[6];
	int log_num;
	size_t type_pos[3];
	size_t sticker_num_start_pos;
	size_t sticker_num_end_pos;
	std::string status;
	std::string hidden_id;
	std::string swkbd_data;
	std::string line_dic_first_spell[7];
	std::string line_dic_full_spell[7];
	std::string init_text;
	std::string hint_text;
	std::string file_name;
	FS_Archive main_fs_archive = 0;
	Handle main_fs_handle = 0;
	SwkbdType type;
	SwkbdValidInput valid_input;
	C2D_ImageTint weak_aqua_or_aqua_tint, dammy_or_aqua_tint;
	Result_with_string main_result;

	texture_size = line_text_size;

	if (Sem_query_font_flag(SEM_USE_DEFAULT_FONT))
		font_num = 0;
	else if (Sem_query_font_flag(SEM_USE_SYSTEM_SPEIFIC_FONT))
		font_num = Sem_query_selected_num(SEM_SELECTED_LANG_NUM) + 1;
	else if (Sem_query_font_flag(SEM_USE_EXTERNAL_FONT))
		font_num = 5;

	if (line_auto_update || line_dl_log_request)
		line_log_dl_progress = Httpc_query_dl_progress(LINE_HTTP_PORT0);
	if (line_solve_short_url_request)
		line_log_dl_progress = Httpc_query_dl_progress(LINE_HTTP_PORT1);

	if(line_sending_msg)
		line_post_and_dl_progress = Httpc_query_post_and_dl_progress(LINE_HTTP_POST_PORT0);

	if (Sem_query_settings(SEM_NIGHT_MODE))
	{
		text_red = 1.0;
		text_green = 1.0;
		text_blue = 1.0;
		text_alpha = 0.75;
		black_or_white_tint = black_tint;
		white_or_black_tint = white_tint;
	}
	else
	{
		text_red = 0.0;
		text_green = 0.0;
		text_blue = 0.0;
		text_alpha = 1.0;
		black_or_white_tint = white_tint;
		white_or_black_tint = black_tint;
	}

	red[0] = text_red;
	green[0] = text_green;
	blue[0] = text_blue;
	alpha[0] = text_alpha;

	for(int i = 0; i < 3; i++)
	{
		if(line_pre_send_request[i] != line_send_request[i] || line_pre_send_check[i] != line_send_check[i])
		{
			line_need_reflesh = true;
			break;
		}
	}

	for(int i = 0; i < 128; i++)
	{
		if(line_pre_icon_available[i] != line_icon_available[i])
		{
			line_need_reflesh = true;
			break;
		}
	}

	if(line_need_reflesh || line_pre_select_chat_room_request != line_select_chat_room_request || line_pre_dl_log_no_parse_request != line_dl_log_no_parse_request
		|| line_pre_dl_all_log_no_parse_request != line_dl_all_log_no_parse_request || line_pre_solve_short_url_request != line_solve_short_url_request
		|| line_pre_log_dl_progress != line_log_dl_progress || line_pre_text_x != line_text_x || line_pre_text_y != line_text_y
		|| line_pre_text_interval != line_text_interval || line_pre_text_size != line_text_size || line_pre_hide_id != line_hide_id
		|| line_pre_selected_menu_mode != line_selected_menu_mode || line_pre_dl_log_request != line_dl_log_request
		|| line_pre_parse_log_request != line_parse_log_request || line_pre_auto_update != line_auto_update || line_pre_load_log_request != line_load_log_request
		|| line_pre_sending_msg != line_sending_msg || line_pre_send_success != line_send_success || line_pre_current_step != line_current_step
		|| line_pre_sent_data_size != line_sent_data_size || line_pre_total_data_size != line_total_data_size || line_pre_post_and_dl_progress != line_post_and_dl_progress
		|| line_pre_num_of_logs != line_num_of_logs || line_pre_selected_msg_num != line_selected_msg_num || line_pre_delete_id_check_request != line_delete_id_check_request
		|| line_pre_select_sticker_request != line_select_sticker_request || line_pre_select_file_request != line_select_file_request
		|| line_pre_selected_search_highlight_num != line_selected_search_highlight_num || line_pre_selected_highlight_num != line_selected_highlight_num)
	{
		line_pre_select_chat_room_request = line_select_chat_room_request;
		line_pre_dl_log_no_parse_request = line_dl_log_no_parse_request;
		line_pre_dl_all_log_no_parse_request = line_dl_all_log_no_parse_request;
		line_pre_solve_short_url_request = line_solve_short_url_request;
		line_pre_log_dl_progress = line_log_dl_progress;
		line_pre_text_x = line_text_x;
		line_pre_text_y = line_text_y;
		line_pre_text_interval = line_text_interval;
		line_pre_text_size = line_text_size;
		line_pre_hide_id = line_hide_id;
		line_pre_selected_menu_mode = line_selected_menu_mode;
		line_pre_dl_log_request = line_dl_log_request;
		line_pre_parse_log_request = line_parse_log_request;
		line_pre_auto_update = line_auto_update;
		line_pre_load_log_request = line_load_log_request;
		line_pre_sending_msg = line_sending_msg;
		line_pre_send_success = line_send_success;
		line_pre_current_step = line_current_step;
		line_pre_sent_data_size = line_sent_data_size;
		line_pre_total_data_size = line_total_data_size;
		line_pre_post_and_dl_progress = line_post_and_dl_progress;
		line_pre_num_of_logs = line_num_of_logs;
		line_pre_selected_msg_num = line_selected_msg_num;
		line_pre_delete_id_check_request = line_delete_id_check_request;
		line_pre_select_sticker_request = line_select_sticker_request;
		line_pre_selected_sticker_tab_num = line_selected_sticker_tab_num;
		line_pre_select_file_request = line_select_file_request;
		line_pre_selected_search_highlight_num = line_selected_search_highlight_num;
		line_pre_selected_highlight_num = line_selected_highlight_num;

		for(int i = 0; i < 3; i++)
		{
			line_pre_send_request[i] = line_send_request[i];
			line_send_check[i] = line_send_check[i];
		}
		for(int i = 0; i < 128; i++)
			line_pre_icon_available[i] = line_icon_available[i];

		line_need_reflesh = true;
	}

	if(Draw_query_need_reflesh() || !Sem_query_settings(SEM_ECO_MODE) || Expl_query_need_reflesh())
		line_need_reflesh = true;

	if(line_need_reflesh)
	{
		Draw_set_draw_mode(Sem_query_settings(SEM_VSYNC_MODE));
		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);

		if (!line_select_chat_room_request)
		{
			Draw_texture(Square_image, weak_aqua_tint, 0, 0.0, 15.0, 50.0 * line_log_dl_progress, 3.0);

			for (int i = 1; i <= 59999; i++)
			{
				red[0] = text_red;
				green[0] = text_green;
				blue[0] = text_blue;
				alpha[0] = text_alpha;

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
					type_pos[1] = line_content[i].find("<type>image</type>");
					type_pos[2] = line_content[i].find("<type>id</type>");

					if (!(type_pos[0] == std::string::npos))
					{
						sticker_num_start_pos = line_content[i].find("<num>");
						sticker_num_end_pos = line_content[i].find("</num>");;
						Draw_texture(line_stickers_images, dammy_tint, std::stoi(line_content[i].substr((sticker_num_start_pos + 5), sticker_num_end_pos - (sticker_num_start_pos + 5))), line_text_x, (line_text_y + line_text_interval * i), (texture_size * 120.0), (texture_size * 120.0));
					}
					else if (!(type_pos[1] == std::string::npos) || !(type_pos[2] == std::string::npos))
						Draw_texture(Square_image, weak_red_tint, 0, line_text_x, (line_text_y + line_text_interval * i), 500.0, 20.0);

					if (!(type_pos[2] == std::string::npos) && (line_dl_log_request || line_parse_log_request || line_auto_update || line_load_log_request))
						alpha[0] = 0.25;

					if((line_selected_menu_mode == LINE_MENU_SEARCH && i >= line_short_msg_pos_start[line_search_result_pos[line_selected_search_highlight_num]] && i <= line_short_msg_pos_end[line_search_result_pos[line_selected_search_highlight_num]])
						|| (line_selected_menu_mode == LINE_MENU_COPY && i >= line_short_msg_pos_start[line_selected_highlight_num] && i <= line_short_msg_pos_end[line_selected_highlight_num]))//Use purple, If msg is highlighted
					{
						red[0] = 0.5;
						green[0] = 0.0;
						blue[0] = 0.5;
					}

					Draw(line_short_msg_log[i], font_num, line_text_x, line_text_y + line_text_interval * i, line_text_size, line_text_size, red[0], green[0], blue[0], alpha[0]);
				}
			}
		}
		Draw_top_ui();

		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(1, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(1, true, 2, 1.0, 1.0, 1.0);

		if (line_select_chat_room_request)
		{
			if(line_dl_log_no_parse_request || line_dl_all_log_no_parse_request)
				alpha[0] = 0.25;
			else
				alpha[0] = text_alpha;

			line_max_y = -124.0 * 35.0;
			for (int i = 0; i < 128; i++)
			{
				if ((line_text_y + (i * 35.0)) >= 165)
					break;
				else if ((line_text_y + (i * 35.0)) <= -60)
				{
				}
				else
				{
					if (line_hide_id && line_ids[i].length() >= 5)
					{
						hidden_id = line_ids[i].substr(0, 5);

						for (size_t k = 6; k <= line_ids[i].length(); k++)
							hidden_id += "*";
					}
					else
						hidden_id = line_ids[i];

					if(line_unread_msg_num[i] > 0)
						weak_aqua_or_aqua_tint = aqua_tint;
					else
						weak_aqua_or_aqua_tint = weak_aqua_tint;

					if((line_dl_log_no_parse_request || line_dl_all_log_no_parse_request) && line_selected_room_num == i)
						dammy_or_aqua_tint = aqua_tint;
					else
						dammy_or_aqua_tint = dammy_tint;

					Draw_texture(Square_image, weak_aqua_or_aqua_tint, 0, 0.0, 0.0 + line_text_y + (i * 35.0), 230.0, 30.0);
					if(line_icon_available[i])
						Draw_texture(line_icon, dammy_tint, i, 0.0, 0.0 + line_text_y + (i * 35.0), 30.0, 30.0);

					Draw_texture(ui_image, dammy_or_aqua_tint, line_dl_log_failed[i], 230.0, 0.0 + line_text_y + (i * 35.0), 30.0, 30.0);
					Draw(line_names[i], font_num, 35.0, 0.0 + line_text_y + (i * 35.0), 0.5, 0.5, text_red, text_green, text_blue, alpha[0]);
					Draw(hidden_id, 0, 35.0, 20.0 + line_text_y + (i * 35.0), 0.325, 0.325, text_red, text_green, text_blue, alpha[0]);
					if(line_unread_msg_num[i] > 0)
						Draw(std::to_string(line_unread_msg_num[i]), 0, 200.0, 0.0 + line_text_y + (i * 35.0), 0.5, 0.5, 1.0, 0.0, 0.0, alpha[0]);
				}
			}

			Draw_texture(Square_image, black_or_white_tint, 0, 0.0, 165.0, 320.0, 60.0);
			Draw_texture(Square_image, weak_yellow_tint, 0, 10.0, 165.0, 300.0, 60.0);

		  if (line_solve_short_url_request || line_dl_log_no_parse_request || line_dl_all_log_no_parse_request)
			{
				Draw_texture(Square_image, weak_red_tint, 0, 20.0, 185.0, 30.0 * line_log_dl_progress, 20.0);
				Draw(line_msg[42], 0, 40.0, 170.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
			}
			else
			{
				msg_num_list[0] = 40;
				msg_num_list[1] = 18;
				msg_num_list[2] = 19;
				msg_num_list[3] = 41;
				msg_num_list[4] = 22;
				msg_num_list[5] = 23;

				draw_x = 20.0;
				draw_y = 170.0;
				for (int i = 0; i < 6; i++)
				{
					Draw_texture(Square_image, weak_aqua_tint, 0, draw_x, draw_y, 130.0, 13.0);
					Draw(line_msg[msg_num_list[i]], 0, draw_x, draw_y, 0.375, 0.375, text_red, text_green, text_blue, text_alpha);

					draw_y += 20.0;
					if (i == 2)
					{
						draw_x += 150.0;
						draw_y = 170.0;
					}
				}
			}
		}
		else
		{
			for (int i = 1; i <= 59999; i++)
			{
				red[0] = text_red;
				green[0] = text_green;
				blue[0] = text_blue;
				alpha[0] = text_alpha;
				if (i > line_num_of_lines || (line_text_y + line_text_interval * i) - 240 >= 120)
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
					type_pos[1] = line_content[i].find("<type>image</type>");
					type_pos[2] = line_content[i].find("<type>id</type>");

					if (!(type_pos[0] == std::string::npos))
					{
						sticker_num_start_pos = line_content[i].find("<num>");
						sticker_num_end_pos = line_content[i].find("</num>");;
						Draw_texture(line_stickers_images, dammy_tint, std::stoi(line_content[i].substr((sticker_num_start_pos + 5), sticker_num_end_pos - (sticker_num_start_pos + 5))), line_text_x - 40.0, (line_text_y + line_text_interval * i) - 240.0, (texture_size * 120.0), (texture_size * 120.0));
					}
					else if (!(type_pos[1] == std::string::npos) || !(type_pos[2] == std::string::npos))
						Draw_texture(Square_image, weak_red_tint, 0, line_text_x - 40.0, (line_text_y + line_text_interval * i) - 240.0, 500.0, 20.0);

					if (!(type_pos[2] == std::string::npos) && (line_dl_log_request || line_parse_log_request || line_auto_update || line_load_log_request))
						alpha[0] = 0.25;

					if((line_selected_menu_mode == LINE_MENU_SEARCH && i >= line_short_msg_pos_start[line_search_result_pos[line_selected_search_highlight_num]] && i <= line_short_msg_pos_end[line_search_result_pos[line_selected_search_highlight_num]])
						|| (line_selected_menu_mode == LINE_MENU_COPY && i >= line_short_msg_pos_start[line_selected_highlight_num] && i <= line_short_msg_pos_end[line_selected_highlight_num]))//Use purple, If msg is highlighted
					{
						red[0] = 0.5;
						green[0] = 0.0;
						blue[0] = 0.5;
					}

					Draw(line_short_msg_log[i], font_num, line_text_x - 40, (line_text_y + line_text_interval * i) - 240, line_text_size, line_text_size, red[0], green[0], blue[0], alpha[0]);
				}
			}

			if (line_hide_id && line_ids[line_selected_room_num].length() >= 5)
			{
				hidden_id = line_ids[line_selected_room_num].substr(0, 5);

				for (size_t i = 6; i <= line_ids[line_selected_room_num].length(); i++)
					hidden_id += "*";
			}
			else
				hidden_id = line_ids[line_selected_room_num];

			status = "ID = " + hidden_id + "\n" + std::to_string(line_num_of_msg) + line_msg[0] + std::to_string(line_num_of_lines) + line_msg[1];

			Draw_texture(line_icon, dammy_tint, line_selected_room_num, 10.0, 135.0, 32.0, 32.0);
			Draw(status, 0, 45.0, 135.0, 0.35, 0.35, text_red, text_green, text_blue, text_alpha);
			Draw(line_names[line_selected_room_num], font_num, 45.0, 155.0, 0.475, 0.475, text_red, text_green, text_blue, text_alpha);
			Draw_texture(Square_image, yellow_tint, 0, 260.0, 135.0, 40.0, 23.0);
			red[0] = text_red;
			green[0] = text_green;
			blue[0] = text_blue;
			alpha[0] = text_alpha;
			if((line_dl_log_request || line_parse_log_request || line_auto_update || line_load_log_request
				|| line_send_request[0] || line_send_request[1] || line_send_request[2]))
				alpha[0] = 0.25;

			Draw(line_msg[43], 0, 260.0, 140.0, 0.45, 0.45, red[0], green[0], blue[0], alpha[0]);
			Draw(line_ver, 0, 260.0, 155.0, 0.45, 0.45, 0.0, 1.0, 0.0, 1.0f);
			Draw_texture(Square_image, weak_aqua_tint, 0, 10.0, 170.0, 300.0, 60.0);
			for(int i = 0; i < 3; i++)
			{
				Draw_texture(Square_image, weak_yellow_tint, 0, 10.0 + (100.0 * i), 170.0, 50.0, 10.0);
				Draw_texture(Square_image, weak_red_tint, 0, 60.0 + (100.0 * i), 170.0, 50.0, 10.0);
			}

			if (line_selected_menu_mode == LINE_MENU_SEND)
				Draw_texture(Square_image, aqua_tint, 0, 10.0, 170.0, 50.0, 10.0);

			if (line_selected_menu_mode == LINE_MENU_SEND && line_sending_msg)
			{
				Draw_texture(Square_image, weak_red_tint, 0, 20.0, 185.0, 35.0 * Httpc_query_post_and_dl_progress(LINE_HTTP_POST_PORT0), 13.0);

				if (line_send_request[2])
				{
					Draw_texture(Square_image, yellow_tint, 0, 20.0, 205.0, (280.0 / line_step_max) * line_current_step, 13.0);
					Draw(std::to_string(line_sent_data_size / 1024.0 / 1024.0).substr(0, 4) + "/" + std::to_string(line_total_data_size / 1024.0 / 1024.0).substr(0, 4) + "MB", 0, 120.0, 205.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
				}

				if (line_send_request[0])
					Draw(line_msg[36], 0, 22.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
				else if (line_send_request[1])
					Draw(line_msg[37], 0, 22.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
				else if (line_send_request[2])
					Draw(line_msg[38], 0, 22.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
			}
			else if (line_selected_menu_mode == LINE_MENU_SEND && line_send_success)
			{
				Draw_texture(Square_image, weak_red_tint, 0, 20.0, 185.0, 280.0, 13.0);
				Draw(line_msg[39], 0, 22.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
			}
			else if (line_selected_menu_mode == LINE_MENU_SEND)
			{
				Draw_texture(Square_image, weak_aqua_tint, 0, 20.0, 185.0, 280.0, 13.0);
				Draw_texture(Square_image, weak_aqua_tint, 0, 20.0, 205.0, 130.0, 13.0);
				Draw_texture(Square_image, weak_aqua_tint, 0, 170.0, 205.0, 130.0, 13.0);

				Draw(line_msg[6], 0, 22.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
				Draw(line_msg[24], 0, 22.5, 205.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
				Draw(line_msg[34], 0, 172.5, 205.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
			}
			else if (line_selected_menu_mode == LINE_MENU_RECEIVE)
			{
				if(line_dl_log_request || line_auto_update)
					alpha[0] = 0.25;
				else
					alpha[0] = text_alpha;

				Draw_texture(Square_image, aqua_tint, 0, 60.0, 170.0, 50.0, 10.0);

				Draw_texture(Square_image, weak_aqua_tint, 0, 20.0, 185.0, 130.0, 13.0);
				Draw_texture(Square_image, weak_aqua_tint, 0, 170.0, 185.0, 130.0, 13.0);
				Draw_texture(Square_image, weak_aqua_tint, 0, 100.0, 210.0, 200.0, 5.0);
				Draw_texture(Square_image, red_tint, 0, (line_num_of_logs / 20) + 99, 205.0, 5.0, 13.0);

				Draw(line_msg[7], 0, 22.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, alpha[0]);
				Draw(line_msg[8 + line_auto_update], 0, 172.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
				Draw(line_msg[50] + std::to_string(line_num_of_logs), 0, 22.5, 205.0, 0.35, 0.35, text_red, text_green, text_blue, alpha[0]);
			}
			else if (line_selected_menu_mode == LINE_MENU_COPY)
			{
				Draw_texture(Square_image, aqua_tint, 0, 110.0, 170.0, 50.0, 10.0);
				Draw(std::to_string(line_selected_highlight_num) + "/" + std::to_string(line_num_of_msg - 1), 0, 20.0, 190.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
				for (int i = 0; i < 3; i++)
				{
					Draw_texture(Square_image, weak_aqua_tint, 0, 90.0 + (i * 70.0), 185.0, 60.0, 30.0);
					Draw(line_msg[11 + i], 0, 92.5 + (i * 70.0), 185.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
				}
			}
			else if (line_selected_menu_mode == LINE_MENU_SETTINGS)
			{
				Draw_texture(Square_image, aqua_tint, 0, 160.0, 170.0, 50.0, 10.0);

				draw_x = 0.0;
				for (int i = 0; i < 4; i++)
				{
					if (i == 2)
						draw_x += 10.0;

					Draw_texture(Square_image, weak_aqua_tint, 0, 20.0 + (i * 70.0), 185.0, 60.0, 30.0);
					Draw(line_msg[14 + i], 0, 22.5 + (i * 70.0), 185.0, 0.375, 0.375, text_red, text_green, text_blue, text_alpha);
				}
			}
			else if (line_selected_menu_mode == LINE_MENU_SEARCH)
			{
				msg_num[0] = 52;
				msg_num[1] = 12;
				msg_num[2] = 13;
				Draw_texture(Square_image, aqua_tint, 0, 210.0, 170.0, 50.0, 10.0);
				Draw(std::to_string(line_selected_search_highlight_num) + "/" + std::to_string(line_debug_found), 0, 20.0, 190.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
				for(int i = 0; i < 3; i++)
				{
					Draw_texture(Square_image, weak_aqua_tint, 0, 90.0 + (i * 70.0), 185.0, 60.0, 30.0);
					Draw(line_msg[msg_num[i]], 0, 92.5 + (i * 70.0), 185.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
				}
			}
			else if (line_selected_menu_mode == LINE_MENU_ADVANCED)
			{
				Draw_texture(Square_image, aqua_tint, 0, 260.0, 170.0, 50.0, 10.0);
				Draw_texture(Square_image, weak_aqua_tint, 0, 20.0, 185.0, 280.0, 30.0);
				Draw(line_msg[44], 0, 22.5, 185.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
			}

			msg_num[0] = 2;
			msg_num[1] = 35;
			msg_num[2] = 3;
			msg_num[3] = 4;
			msg_num[4] = 51;
			msg_num[5] = 5;
			for (int i = 0; i < 6; i++)
			{
				Draw(line_msg[msg_num[i]], 0, 12.5 + (i * 50.0), 170.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			}

			if (line_send_check[0] || line_send_check[1] || line_send_check[2] || line_delete_id_check_request)
			{
				if ((line_dl_log_request || line_parse_log_request || line_auto_update || line_load_log_request) && line_delete_id_check_request)
					alpha[0] = 0.25;
				else
					alpha[0] = 0.75;

				Draw_texture(Square_image, blue_tint, 0, 10.0, 110.0, 300.0, 110.0);
				Draw_texture(Square_image, weak_aqua_tint, 0, 30.0, 200.0, 70.0, 15.0);
				Draw_texture(Square_image, weak_aqua_tint, 0, 120.0, 200.0, 70.0, 15.0);
				if (line_send_check[0] || line_send_check[2])
					Draw_texture(Square_image, weak_aqua_tint, 0, 210.0, 200.0, 70.0, 15.0);

				Draw(line_msg[20], 0, 32.5, 200.0, 0.45, 0.45, 1.0, 1.0, 1.0, alpha[0]);
				Draw(line_msg[21], 0, 122.5, 200.0, 0.45, 0.45, 1.0, 1.0, 1.0, 0.75);
				if (line_send_check[0])
					Draw(line_msg[47], 0, 212.5, 200.0, 0.4, 0.4, 1.0, 1.0, 1.0, 0.75);
				else if (line_send_check[2])
					Draw(line_msg[32], 0, 212.5, 200.0, 0.4, 0.4, 1.0, 1.0, 1.0, 0.75);

				Draw_texture(line_icon, dammy_tint, line_selected_room_num, 10.0, 110.0, 30.0, 30.0);
				Draw(line_names[line_selected_room_num], font_num, 45.0, 110.0, 0.475, 0.475, 1.0, 0.0, 0.0, 1.0);
				if (line_send_check[0])
				{
					Draw(line_msg[25], 0, 45.0, 120.0, 0.55, 0.55, 1.0, 1.0, 1.0, 0.75);
					Draw(line_input_text, font_num, 45.0, 135.0, 0.45, 0.45, 1.0, 1.0, 1.0, 0.75);
				}
				else if (line_send_check[1])
				{
					Draw(line_msg[26], 0, 45.0, 120.0, 0.55, 0.55, 1.0, 1.0, 1.0, 0.75);
					Draw_texture(line_stickers_images, dammy_tint, line_selected_sticker_num, 45.0, 135.0, 60.0, 60.0);
				}
				else if (line_send_check[2])
				{
					Draw(line_msg[29], 0, 45.0, 120.0, 0.55, 0.55, 1.0, 1.0, 1.0, 0.75);
					Draw(line_msg[30] + Expl_query_current_patch(), 0, 45.0, 140.0, 0.4, 0.4, 1.0, 1.0, 1.0, 0.75);
					Draw(line_msg[48] + std::to_string(Expl_query_size((int)Expl_query_selected_num(EXPL_SELECTED_FILE_NUM) + (int)Expl_query_view_offset_y()) / 1024.0 / 1024.0).substr(0, 5) + line_msg[49], 0, 45.0, 150.0, 0.45, 0.45, 1.0, 1.0, 1.0, 0.75);
					Draw(line_msg[31] + Expl_query_file_name((int)Expl_query_selected_num(EXPL_SELECTED_FILE_NUM) + (int)Expl_query_view_offset_y()), 0, 45.0, 160.0, 0.45, 0.45, 1.0, 0.0, 0.0, 1.0);
				}
				else if (line_delete_id_check_request)
				{
					Draw(line_msg[45], 0, 45.0, 120.0, 0.55, 0.55, 1.0, 1.0, 1.0, 0.75);
					Draw(line_msg[46], 0, 45.0, 130.0, 0.4, 0.4, 1.0, 1.0, 1.0, 0.75);
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
				Draw_expl(line_msg[33]);
		}

		Draw_texture(Square_image, white_or_black_tint, 0, 312.5, 0.0, 7.5, 15.0);
		Draw_texture(Square_image, white_or_black_tint, 0, 312.5, 215.0, 7.5, 10.0);

		if (scroll_bar_y_pos <= 0.0)
			Draw_texture(Square_image, red_tint, 0, 312.5, 15.0, 7.5, scroll_bar_y_size);
		else if (scroll_bar_y_pos >= 195.0)
			Draw_texture(Square_image, red_tint, 0, 312.5, 210.0, 7.5, scroll_bar_y_size);
		else
			Draw_texture(Square_image, blue_tint, 0, 312.5, 15.0 + scroll_bar_y_pos, 7.5, scroll_bar_y_size);

		Draw_bot_ui();
		Draw_touch_pos();

		Draw_apply_draw();
		line_need_reflesh = false;
	}
	else
		gspWaitForVBlank();

	Hid_set_disable_flag(true);
	if (line_type_msg_request || line_type_id_request || line_type_short_url_request || line_type_main_url_request
		|| line_check_main_url_request || line_type_app_ps_request || line_type_script_ps_request || line_debug_request)
	{
		init_text = Menu_query_clipboard();
		max_length = 512;
		type = SWKBD_TYPE_NORMAL;
		valid_input = SWKBD_NOTEMPTY_NOTBLANK;
		feature[0] = -1;
		feature[1] = -1;

		if(line_type_msg_request)
		{
			line_dic_full_spell[0] = "└(՞ةڼ◔)」";
			line_dic_full_spell[1] = "┌(☝┌՞ ՞)☝キエェェェエェェwwwww";
			line_dic_full_spell[2] = "┌(┌ ՞ةڼ)┐<ｷｴｪｪｪｴｴｪｪｪ";
			line_dic_full_spell[3] = "(  ՞ةڼ  )";
			line_dic_full_spell[4] = "壁|՞ةڼ)イーヒヒヒヒヒヒｗｗｗｗｗｗｗｗｗｗｗ";
			line_dic_full_spell[5] = "。゜( ;⊃՞ةڼ⊂; )゜。びぇぇえええんｗｗｗｗ";
			line_dic_full_spell[6] = "(✌ ՞ةڼ ✌ )";
			for(int i = 0; i < 5; i++)
				line_dic_first_spell[i] = "ぬべ";

			line_dic_first_spell[5] = "びぇ";
			line_dic_first_spell[6] = "うえ";

			num_of_words = 7;
			if(line_input_text != "")
				init_text = line_input_text;

			hint_text = "メッセージを入力 / Type message here.";
			max_length = 8192;
			feature[0] = SWKBD_MULTILINE;
			feature[1] = SWKBD_PREDICTIVE_INPUT;
		}
		else if (line_type_id_request)
		{
			num_of_words = 0;
			hint_text = "idを入力 / Type id here.";
			max_length = 39;
			type = SWKBD_TYPE_QWERTY;
		}
		else if(line_type_short_url_request)
		{
			line_dic_full_spell[0] = "http://rb.gy/";
			line_dic_full_spell[1] = "https://tiny.cc/";
			line_dic_full_spell[2] = "http://tinyurl.com/";
			for(int i = 0; i < 3; i++)
				line_dic_first_spell[i] = "h";

			num_of_words = 3;
			hint_text = "短縮URLを入力 / Type your short url here.";
			feature[0] = SWKBD_PREDICTIVE_INPUT;
		}
		else if(line_type_main_url_request || line_check_main_url_request)
		{
			line_dic_full_spell[0] = "https://script.google.com/macros/s/";
			line_dic_first_spell[0] = "h";

			num_of_words = 1;
			hint_text = "URLを入力 / Type your url here.";
			if(line_type_main_url_request)
				init_text = line_main_url;

			feature[0] = SWKBD_PREDICTIVE_INPUT;
		}
		else if(line_type_app_ps_request)
		{
			num_of_words = 0;
			hint_text = "アプリパスワードを入力 / Type app password here.";
			init_text = "";
			type = SWKBD_TYPE_QWERTY;
			valid_input = SWKBD_ANYTHING;
		}
		else if(line_type_script_ps_request)
		{
			num_of_words = 0;
			hint_text = "スクリプトパスワードを入力 / Type script password here.";
			init_text = "";
			type = SWKBD_TYPE_QWERTY;
			valid_input = SWKBD_ANYTHING;
		}
		else if(line_debug_request)
		{
			num_of_words = 0;
			hint_text = "検索 単語/文 を入力 / Type search word/message(s) here.";
			init_text = line_debug_text;
			feature[0] = SWKBD_PREDICTIVE_INPUT;
		}

		Swkbd_set_parameter(type, valid_input, feature[0], feature[1], 2, max_length, hint_text, init_text);
		if(num_of_words != 0)
			Swkbd_set_dic_word(line_dic_first_spell, line_dic_full_spell, num_of_words);

		if (Swkbd_launch(max_length, &swkbd_data, SWKBD_BUTTON_RIGHT))
		{
			log_num = 0;
			if (line_type_msg_request)
			{
				line_input_text = swkbd_data;
				line_encoded_input_text = Line_encode_to_escape(swkbd_data);
				if (line_encoded_input_text.length() > 4000)
					line_encoded_input_text = line_encoded_input_text.substr(0, 3990);

				line_send_check[0] = true;
			}
			else if(line_type_id_request)
			{
				log_num = Log_log_save(line_main_thread_string, "Line_save_new_id()...", 1234567890, false);
				main_result = Line_save_new_id(swkbd_data, "/Line/to/");
			}
			else if(line_type_short_url_request)
			{
				line_short_url = swkbd_data;
				line_solve_short_url_request = true;
			}
			else if(line_type_main_url_request || line_check_main_url_request || line_type_app_ps_request || line_type_script_ps_request)
			{
				log_num = Log_log_save(line_main_thread_string, "File_save_to_file()...", 1234567890, false);

				if(line_type_main_url_request || line_check_main_url_request)
				{
					file_name = "main_url.txt";
					line_main_url = swkbd_data;
				}
				else if(line_type_app_ps_request)
					file_name = "auth";
				else if(line_type_script_ps_request)
				{
					file_name = "script_auth";
					line_script_auth = swkbd_data;
				}
				main_result = File_save_to_file(file_name, (u8*)swkbd_data.c_str(), swkbd_data.length(), "/Line/", true, main_fs_handle, main_fs_archive);
			}
			else if(line_debug_request)
			{
				line_debug_text = swkbd_data;
				line_selected_search_highlight_num = 0;
				line_debug_found = 0;
				for(int i = 0; i < 4000; i++)
					line_search_result_pos[i] = 0;

				for(int i = 0; i < 4000; i++)
				{
					if(line_msg_log[i].find(line_debug_text) != std::string::npos)
					{
						line_search_result_pos[line_debug_found] = i;
						line_debug_found++;
					}
				}
				if(line_debug_found > 0)
				{
					line_debug_found--;
					line_selected_search_highlight_num = line_debug_found;
					line_text_y = line_msg_pos[line_search_result_pos[line_debug_found]];
				}
			}

			if(line_type_id_request || line_type_main_url_request || line_check_main_url_request
				|| line_type_app_ps_request || line_type_script_ps_request)
				Log_log_add(log_num, main_result.string, main_result.code, false);
		}

		if (line_type_msg_request)
			line_type_msg_request = false;
		else if(line_type_id_request)
		{
			line_icon_dl_request = true;
			line_type_id_request = false;
		}
		else if(line_type_short_url_request)
			line_type_short_url_request = false;
		else if(line_type_main_url_request || line_check_main_url_request)
		{
			line_type_main_url_request = false;
			line_check_main_url_request = false;
		}
		else if(line_type_app_ps_request)
			line_type_app_ps_request = false;
		else if(line_type_script_ps_request)
			line_type_script_ps_request = false;
		else if(line_debug_request)
			line_debug_request = false;
	}
}

void Line_init(void)
{
	Log_log_save(line_init_string, "Initializing...", 1234567890, DEBUG);
	bool auth_success = false;
	u8* fs_buffer;
	u32 read_size = 0;
	int log_num;
	std::string auth_code = "";
	std::string input_string = "";
	std::string swkbd_data = ".";
	std::string data[4];
	FS_Archive fs_archive = 0;
	Handle fs_handle = 0;
	Result_with_string result;
	fs_buffer = (u8*)malloc(0x2000);


	Draw_progress("0/3 [Line] Authing...");
	memset(fs_buffer, 0x0, 0x2000);
	log_num = Log_log_save(line_init_string, "File_load_from_file()...", 1234567890, DEBUG);
	result = File_load_from_file("auth", fs_buffer, 0x2000, &read_size, "/Line/",  fs_handle, fs_archive);
	Log_log_add(log_num, result.string, result.code, DEBUG);

	if (result.code == 0)
		auth_code = (char*)fs_buffer;
	else
	{
		while (true)
		{

			Swkbd_set_parameter(SWKBD_TYPE_QWERTY, SWKBD_ANYTHING, -1, -1, 2, 512, "アプリパスワードを入力 / Type app password here.", "");
			Swkbd_launch(512, &swkbd_data, SWKBD_BUTTON_RIGHT);
			input_string = swkbd_data;

			Swkbd_set_parameter(SWKBD_TYPE_QWERTY, SWKBD_ANYTHING, -1, -1, 2, 512, "アプリパスワードを入力(再度) / Type app password here.(again)", "");
			Swkbd_launch(512, &swkbd_data, SWKBD_BUTTON_RIGHT);

			if (input_string == swkbd_data)
			{
				log_num = Log_log_save(line_init_string, "File_save_to_file()...", 1234567890, DEBUG);
				result = File_save_to_file("auth", (u8*)swkbd_data.c_str(), swkbd_data.length(), "/Line/", true, fs_handle, fs_archive);
				Log_log_add(log_num, result.string, result.code, DEBUG);
				auth_code = swkbd_data;
				break;
			}
		}
	}

	Log_log_save(line_init_string, "Please enter password", 1234567890, DEBUG);
	if (auth_code == "")
	{
		Log_log_save(line_init_string, "Password is not set", 1234567890, DEBUG);
		auth_success = true;
	}
	else if (input_string == swkbd_data)
		auth_success = true;
	else
	{
		for (int i = 0; i < 3; i++)
		{
			Swkbd_set_parameter(SWKBD_TYPE_QWERTY, SWKBD_ANYTHING, -1, -1, 2, 512, "アプリパスワードを入力 / Type app password here.", "");
			Swkbd_launch(512, &swkbd_data, SWKBD_BUTTON_RIGHT);
			if (auth_code == swkbd_data)
			{
				auth_success = true;
				Log_log_save(line_init_string, "Password is correct", 1234567890, DEBUG);
				break;
			}
			else
				Log_log_save(line_init_string, "Password is incorrect", 1234567890, DEBUG);
		}
	}

	if (!auth_success)
	{
		Log_log_save(line_init_string, "Auth failed, rebooting...", 1234567890, DEBUG);
		Draw_progress("0/3 [Line] Auth failed.");
		APT_HardwareResetAsync();
	}


	Draw_progress("1/3 [Line] Loading settings...");
	memset(fs_buffer, 0x0, 0x2000);
	log_num = Log_log_save(line_init_string, "File_load_from_file()...", 1234567890, DEBUG);
	result = File_load_from_file("script_auth", fs_buffer, 0x2000, &read_size, "/Line/", fs_handle, fs_archive);
	Log_log_add(log_num, result.string, result.code, DEBUG);
	if (result.code == 0)
		line_script_auth = (char*)fs_buffer;
	else
		line_script_auth = "";

	memset(fs_buffer, 0x0, 0x2000);
	log_num = Log_log_save(line_init_string, "File_load_from_file()...", 1234567890, DEBUG);
	result = File_load_from_file("main_url.txt", fs_buffer, 0x2000, &read_size, "/Line/", fs_handle, fs_archive);
	Log_log_add(log_num, result.string, result.code, DEBUG);
	if (result.code == 0)
		line_main_url = (char*)fs_buffer;
	else
		line_main_url = "";

	line_text_size = 0.66;
	line_text_interval = 35.0;
	line_num_of_logs = 150;
	line_hide_id = false;
	memset(fs_buffer, 0x0, 0x2000);
	log_num = Log_log_save(line_init_string, "File_load_from_file()...", 1234567890, DEBUG);
	result = File_load_from_file("Line_setting.txt", fs_buffer, 0x2000, &read_size, "/Line/", fs_handle, fs_archive);
	Log_log_add(log_num, result.string, result.code, DEBUG);
	if (result.code == 0)
	{
		result = Sem_parse_file((char*)fs_buffer, 4, data);
		if(result.code == 0)
		{
			if (!(data[0] == ""))
				line_text_size = strtod(data[0].c_str(), NULL);
			if (!(data[1] == ""))
				line_text_interval = strtod(data[1].c_str(), NULL);
			if (std::all_of(data[2].cbegin(), data[2].cend(), isdigit) && !(data[2] == ""))
				line_num_of_logs = stoi(data[2]);

			line_hide_id = (data[3] == "1");
		}
	}

	if(line_text_size >= 3.0 || line_text_size <= 0.25)
		line_text_size = 0.66;
	if(line_text_interval >= 250.0 || line_text_interval <= 10.0)
		line_text_interval = 35.0;

	for (int i = 0; i < 128; i++)
	{
		line_c3d_cache_tex[i] = (C3D_Tex*)linearAlloc(sizeof(C3D_Tex));
		line_c3d_cache_subtex[i] = (Tex3DS_SubTexture*)linearAlloc(sizeof(Tex3DS_SubTexture));
	}

	Draw_progress("2/3 [Line] Starting threads...");
	line_worker_thread_run = true;
	line_log_dl_thread_run = true;
	line_log_load_thread_run = true;
	line_log_parse_thread_run = true;
	line_send_msg_thread_run = true;

	line_worker_thread = threadCreate(Line_worker_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, -1, false);
	line_dl_log_thread = threadCreate(Line_log_download_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, -1, false);
	line_load_log_thread = threadCreate(Line_log_load_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, -1, false);
	line_parse_log_thread = threadCreate(Line_log_parse_thread, (void*)(""), STACKSIZE, PRIORITY_LOW, -1, false);
	line_send_msg_thread = threadCreate(Line_send_message_thread, (void*)(""), STACKSIZE, PRIORITY_HIGHT, -1, false);

	line_icon_dl_request = true;

	Draw_progress("3/3 [Line] Loading textures...");
	log_num = Log_log_save(line_init_string, "Draw_load_texture()...", 1234567890, DEBUG);
	result = Draw_load_texture("romfs:/gfx/stickers.t3x", 51, line_stickers_images, 0, 121);
	Log_log_add(log_num, result.string, result.code, DEBUG);

	line_select_chat_room_request = true;
	line_text_y = 0.0;

	Line_resume();
	line_already_init = true;
	free(fs_buffer);
	fs_buffer = NULL;


	/*std::string web_url = "https://192.168.1.1/";

	log_num = Log_log_save(line_init_string, "APT_PrepareToJumpToHomeMenu()...", 1234567890, DEBUG);
	result.code = APT_PrepareToJumpToHomeMenu();
	Log_log_add(log_num, "", result.code, DEBUG);

	log_num = Log_log_save(line_init_string, "APT_JumpToHomeMenu()...", 1234567890, DEBUG);
	result.code = APT_JumpToHomeMenu(0, 0, 0);
	Log_log_add(log_num, "", result.code, DEBUG);

	log_num = Log_log_save(line_init_string, "APT_PrepareToStartSystemApplet()...", 1234567890, DEBUG);
	result.code = APT_PrepareToStartSystemApplet(APPID_WEB);
	Log_log_add(log_num, "", result.code, DEBUG);
	log_num = Log_log_save(line_init_string, "	APT_StartSystemApplet()...", 1234567890, DEBUG);
	result.code = APT_StartSystemApplet(APPID_WEB, (const void*)"https://192.168.1.1/", strlen("https://192.168.1.1/"), CUR_PROCESS_HANDLE);
	Log_log_add(log_num, "", result.code, DEBUG);*/

	Log_log_save(line_init_string, "Initialized", 1234567890, DEBUG);
}

void Line_exit(void)
{
	Log_log_save(line_exit_string , "Exiting...", 1234567890, DEBUG);
	u64 time_out = 10000000000;
	int log_num;
	bool failed = false;
	std::string data =  "<0>" + std::to_string(line_text_size) + "</0><1>" + std::to_string(line_text_interval) + "</1><2>" + std::to_string(line_num_of_logs) + "</2><3>" + std::to_string(line_hide_id) + "</3>";
	FS_Archive fs_archive = 0;
	Handle fs_handle = 0;
	Result_with_string result;

	Draw_progress("[Line] Exiting...");
	line_already_init = false;
	line_thread_suspend = false;
	line_worker_thread_run = false;
	line_log_dl_thread_run = false;
	line_log_load_thread_run = false;
	line_log_parse_thread_run = false;
	line_send_msg_thread_run = false;

	log_num = Log_log_save(line_exit_string, "File_save_to_file()...", 1234567890, DEBUG);
	result = File_save_to_file("Line_setting.txt", (u8*)data.c_str(), data.length(), "/Line/", true, fs_handle, fs_archive);
	Log_log_add(log_num, result.string, result.code, DEBUG);

	for(int i = 0; i < 5; i++)
	{
		log_num = Log_log_save(line_exit_string, "threadJoin()" + std::to_string(i) + "/4...", 1234567890, DEBUG);

		if (i == 0)
			result.code = threadJoin(line_dl_log_thread, time_out);
		else if (i == 1)
			result.code = threadJoin(line_load_log_thread, time_out);
		else if (i == 2)
			result.code = threadJoin(line_parse_log_thread, time_out);
		else if (i == 3)
			result.code = threadJoin(line_send_msg_thread, time_out);
		else if (i == 4)
			result.code = threadJoin(line_worker_thread, time_out);

		if (result.code == 0)
			Log_log_add(log_num, Err_query_template_summary(0), result.code, DEBUG);
		else
		{
			failed = true;
			Log_log_add(log_num, Err_query_template_summary(-1024), result.code, DEBUG);
		}
	}

	threadFree(line_dl_log_thread);
	threadFree(line_load_log_thread);
	threadFree(line_parse_log_thread);
	threadFree(line_send_msg_thread);
	threadFree(line_worker_thread);

	Line_reset_id();
	Line_reset_msg();
	for (int i = 0; i < 128; i++)
	{
		linearFree(line_c3d_cache_tex[i]->data);
		linearFree(line_c3d_cache_tex[i]);
		linearFree(line_c3d_cache_subtex[i]);
		line_c3d_cache_tex[i]->data = NULL;
		line_c3d_cache_tex[i] = NULL;
		line_c3d_cache_subtex[i] = NULL;
	}

	Draw_free_texture(51);
	for (int i = 0; i < 121; i++)
		line_stickers_images[i].tex = NULL;

	if(failed)
		Log_log_save(line_exit_string , "[Warn] Some function returned error.", 1234567890, DEBUG);

	Log_log_save(line_exit_string , "Exited.", 1234567890, DEBUG);
}

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

int Line_query_max_msg(void)
{
	return line_num_of_msg - 1;
}

int Line_query_max_search_result(void)
{
	return line_debug_found;
}

std::string Line_query_msg_log(int log_num)
{
	if (log_num >= 0 && log_num <= 3999)
		return line_msg_log[log_num];
	else
		return "";
}

double Line_query_msg_pos(int log_num)
{
	if (log_num >= 0 && log_num <= 3999)
		return line_msg_pos[log_num];
	else
		return -1.0;
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
	else if (operation_num == LINE_SELECT_CHAT_ROOM_REQUEST)
		return line_select_chat_room_request;
	else if (operation_num == LINE_DELETE_ID_CHECK_REQUEST)
		return line_delete_id_check_request;
	else if (operation_num == LINE_DELETE_ID_REQUEST)
		return line_delete_id_request;
	else if (operation_num == LINE_SOLVE_SHORT_URL_REQUEST)
		return line_solve_short_url_request;
	else if (operation_num == LINE_CHECK_MAIN_URL_REQUEST)
		return line_check_main_url_request;
	else if (operation_num == LINE_TYPE_SHORT_URL_REQUEST)
		return line_type_short_url_request;
	else if (operation_num == LINE_PARSE_LOG_REQUEST)
		return line_parse_log_request;
	else if(operation_num == LINE_DL_LOG_NO_PARSE_REQUEST)
		return line_dl_log_no_parse_request;
	else if(operation_num == LINE_DL_ALL_LOG_NO_PARSE_REQUEST)
		return line_dl_all_log_no_parse_request;
	else if(operation_num == LINE_DEBUG_REQUEST)
		return line_debug_request;
	else
		return false;
}

int Line_query_search_num(int search_num)
{
	if (search_num >= 0 && search_num <= 3999)
		return line_search_result_pos[search_num];
	else
		return 0;
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
	else if(item_num == LINE_SELECTED_NUM_OF_LOGS)
		return line_num_of_logs;
	else if(item_num == LINE_SELECTED_HIGHLIGHT_NUM)
		return line_selected_highlight_num;
	else if(item_num == LINE_SELECTED_SEARCH_HIGHLIGHT_NUM)
		return line_selected_search_highlight_num;
	else
		return -1;
}

double Line_query_x_y_size_interval(int item_num)
{
	if (item_num == LINE_TEXT_X)
		return line_text_x;
	else if (item_num == LINE_TEXT_Y)
		return line_text_y;
	else if (item_num == LINE_TEXT_SIZE)
		return line_text_size;
	else if (item_num == LINE_TEXT_INTERVAL)
		return line_text_interval;
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

void Line_reset_id(void)
{
	for (int i = 0; i < 128; i++)
	{
		free(line_stb_image[i]);
		linearFree(line_c3d_cache_tex[i]->data);
		linearFree(line_c3d_cache_tex[i]);
		linearFree(line_c3d_cache_subtex[i]);
		line_c3d_cache_tex[i]->data = NULL;
		line_stb_image[i] = NULL;
		line_c3d_cache_tex[i] = (C3D_Tex*)linearAlloc(sizeof(C3D_Tex));
		line_c3d_cache_subtex[i] = (Tex3DS_SubTexture*)linearAlloc(sizeof(Tex3DS_SubTexture));
		line_ids[i] = "";
		line_ids[i].reserve(10);
		line_names[i] = "";
		line_names[i].reserve(10);
		line_icon_url[i] = "";
		line_icon_url[i].reserve(10);
		line_msg_offset[i] = 0;
		line_unread_msg_num[i] = 0;
		line_dl_log_failed[i] = false;
		line_icon_available[i] = false;
	}
}

void Line_reset_msg(void)
{
	line_num_of_msg = 0;
	line_num_of_lines = 0;
	for (int i = 0; i < 4000; i++)
	{
		line_msg_log[i] = "";
		line_msg_log[i].reserve(1);
	}
	for (int i = 0; i < 60000; i++)
	{
		line_short_msg_log[i] = "";
		line_short_msg_log[i].reserve(1);
		line_content[i] = "";
		line_content[i].reserve(1);
	}

	line_log_data = "";
	line_log_data.reserve(1);
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

void Line_set_msg(int msg_num, std::string msg)
{
	if (msg_num >= 0 && msg_num < LINE_NUM_OF_MSG)
		line_msg[msg_num] = msg;
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
	else if (operation_num == LINE_SELECT_CHAT_ROOM_REQUEST)
		line_select_chat_room_request = flag;
	else if (operation_num == LINE_DELETE_ID_CHECK_REQUEST)
		line_delete_id_check_request = flag;
	else if (operation_num == LINE_DELETE_ID_REQUEST)
		line_delete_id_request = flag;
	else if (operation_num == LINE_SOLVE_SHORT_URL_REQUEST)
		line_solve_short_url_request = flag;
	else if (operation_num == LINE_CHECK_MAIN_URL_REQUEST)
		line_check_main_url_request = flag;
	else if (operation_num == LINE_TYPE_SHORT_URL_REQUEST)
		line_type_short_url_request = flag;
	else if (operation_num == LINE_PARSE_LOG_REQUEST)
		line_parse_log_request = flag;
	else if(operation_num == LINE_DL_LOG_NO_PARSE_REQUEST)
		line_dl_log_no_parse_request = flag;
	else if(operation_num == LINE_DL_ALL_LOG_NO_PARSE_REQUEST)
		line_dl_all_log_no_parse_request = flag;
	else if(operation_num == LINE_DEBUG_REQUEST)
		line_debug_request = flag;
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
	else if(item_num == LINE_SELECTED_NUM_OF_LOGS)
		line_num_of_logs = value;
	else if(item_num == LINE_SELECTED_HIGHLIGHT_NUM)
		line_selected_highlight_num = value;
	else if(item_num == LINE_SELECTED_SEARCH_HIGHLIGHT_NUM)
		line_selected_search_highlight_num = value;
}

void Line_set_x_y_size_interval(int item_num, double value)
{
	if (item_num == LINE_TEXT_X)
		line_text_x = value;
	else if (item_num == LINE_TEXT_Y)
		line_text_y = value;
	else if (item_num == LINE_TEXT_SIZE)
		line_text_size = value;
	else if (item_num == LINE_TEXT_INTERVAL)
		line_text_interval = value;
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
	line_need_reflesh = true;
}

void Line_suspend(void)
{
	line_thread_suspend = true;
	line_main_run = false;
	Menu_resume();
}

std::string Line_encode_to_escape(std::string in_data)
{
	int string_length = in_data.length();
	std::string check;
	std::string return_data = "";

	for(int i = 0; i < string_length; i++)
	{
		check = in_data.substr(i, 1);
		if(check == "\n")
			return_data += "\\n";
		else if(check == "\u0022")
			return_data += "\\\u0022";
		else if(check == "\u005c")
			return_data += "\\\u005c";
		else
			return_data += in_data.substr(i, 1);
	}

	return return_data;
}

void Line_worker_thread(void* arg)
{
	Log_log_save(line_worker_thread_string, "Thread started.", 1234567890, false);
	int log_num;
	FS_Archive fs_archive = 0;
	Result_with_string result;

	while (line_worker_thread_run)
	{
		if (line_icon_dl_request)
		{
			line_icon_dl_request = false;
			Line_reset_id();
			log_num = Log_log_save(line_worker_thread_string, "Line_read_id()...", 1234567890, false);
			result = Line_read_id("/Line/to/");
			Log_log_add(log_num, result.string, result.code, false);

			for (int i = 0; i < line_num_of_ids; i++)
			{
				log_num = Log_log_save(line_worker_thread_string, "Line_load_icon()...", 1234567890, false);
				result = Line_load_icon(i);
				Log_log_add(log_num, result.string, result.code, false);
				if(result.code == 0)
					line_icon_available[i] = true;
			}
		}
		else if (line_delete_id_request)
		{
			line_delete_id_request = false;
			log_num = Log_log_save(line_send_msg_thread_string, "File_delete_file()...", 1234567890, false);
			result = File_delete_file(line_ids[line_selected_room_num], "/Line/to/", fs_archive);
			Log_log_add(log_num, result.string, result.code, false);

			if (result.code == 0)
			{
				line_select_chat_room_request = true;

				log_num = Log_log_save(line_send_msg_thread_string, "File_delete_file()...", 1234567890, false);
				result = File_delete_file(line_ids[line_selected_room_num].substr(0, 16), "/Line/", fs_archive);
				Log_log_add(log_num, result.string, result.code, false);

				Line_reset_id();
				line_icon_dl_request = true;
			}
		}
		else
			usleep(ACTIW_THREAD_SLEEP_TIME);

		while (line_thread_suspend)
			usleep(INACTIW_THREAD_SLEEP_TIME);
	}

	Log_log_save(line_worker_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Line_log_download_thread(void* arg)
{
	Log_log_save(line_log_dl_thread_string, "Thread started.", 1234567890, false);
	u8* httpc_buffer;
	u32 dl_size;
	u32 status_code;
	int log_num;
	int index = 0;
	int room_num = 0;
	bool failed = false;
	std::string file_name;
	std::string out_data[6];
	std::string last_url;
	FS_Archive fs_archive = 0;
	Handle fs_handle = 0;
	Result_with_string result;

	while (line_log_dl_thread_run)
	{
		if(line_dl_all_log_no_parse_request && line_ids[index] == "")
		{
			line_dl_all_log_no_parse_request = false;
			index = 0;
		}
		else if (line_auto_update || line_dl_log_request || line_dl_log_no_parse_request || line_dl_all_log_no_parse_request)
		{
			line_dl_log_request = true;
			failed = false;
			dl_size = 0;
			status_code = 0;
			line_dl_log_failed[line_selected_room_num] = false;

			if(line_dl_all_log_no_parse_request)
				line_selected_room_num = index;

			room_num = line_selected_room_num;
			httpc_buffer = (u8*)malloc(line_log_httpc_buffer_size);

			if (httpc_buffer == NULL)
			{
				Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), line_log_dl_thread_string, OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				Log_log_save(line_log_dl_thread_string, Err_query_template_summary(OUT_OF_MEMORY), OUT_OF_MEMORY, false);
				failed = true;
				line_auto_update = false;
			}
			else
			{
				memset(httpc_buffer, 0x0, line_log_httpc_buffer_size);
				log_num = Log_log_save(line_log_dl_thread_string, "Httpc_dl_data()...", 1234567890, false);
				result = Httpc_dl_data(line_main_url + "?id=" + line_ids[room_num] + "&logs=" + std::to_string(line_num_of_logs) + "&script_auth=" + line_script_auth + "&gas_ver=" + std::to_string(Sem_query_gas_ver()), httpc_buffer, line_log_httpc_buffer_size, &dl_size, &status_code, true, &last_url, false, 100, LINE_HTTP_PORT0);
				Log_log_add(log_num, result.string + std::to_string(dl_size / 1024) + "KB (" + std::to_string(dl_size) + "B) ", result.code, false);
				line_log_dl_progress = Httpc_query_dl_progress(LINE_HTTP_PORT0);

				if (result.code == 0)
				{
					result = Sem_parse_file((char*)httpc_buffer, 5, out_data);
					if(result.code == 0 && out_data[4] == "Success")
					{
						line_log_data = out_data[0];
						line_names[room_num] = out_data[1];
						line_icon_url[room_num] = out_data[2];
						line_unread_msg_num[room_num] = std::stoi(out_data[3]) - line_msg_offset[room_num];
						out_data[1] = "<0>" + out_data[1] + "</0>"; //user/group name
						out_data[2] = "<1>" + out_data[2] + "</1>"; //img url
						out_data[3] = "<2>" + std::to_string(line_msg_offset[room_num]) + "</2>"; //msg offset
						out_data[5] = "<3>" + std::to_string(line_unread_msg_num[room_num]) + "</3>"; //num of new msg

						file_name = line_ids[room_num].substr(0, 16);
						log_num = Log_log_save(line_log_dl_thread_string, "File_save_to_file()...", 1234567890, false);
						result = File_save_to_file(file_name, (u8*)out_data[0].c_str(), out_data[0].length(), "/Line/", true, fs_handle, fs_archive);
						Log_log_add(log_num, result.string, result.code, false);

						if (result.code != 0)
							failed = true;

						if (!failed)
						{
							log_num = Log_log_save(line_log_dl_thread_string, "File_save_to_file()...", 1234567890, false);
							result = File_save_to_file(line_ids[room_num], (u8*)(out_data[1] + out_data[2] + out_data[3] + out_data[5]).c_str(), (out_data[1] + out_data[2] + out_data[3] + out_data[5]).length(), "/Line/to/", true, fs_handle, fs_archive);
							Log_log_add(log_num, result.string, result.code, false);
							if (result.code != 0)
								failed = true;
						}

						if(!line_dl_log_no_parse_request && !line_dl_all_log_no_parse_request)
							line_parse_log_request = true;

						if (result.code != 0)
						{
							Err_set_error_message(result.string, result.error_description, line_log_dl_thread_string, result.code);
							Err_set_error_show_flag(true);
						}

						if (!failed)
						{
							log_num = Log_log_save(line_log_dl_thread_string, "Line_load_icon()...", 1234567890, false);
							result = Line_load_icon(room_num);
							Log_log_add(log_num, result.string, result.code, false);
							if(result.code == 0)
								line_icon_available[room_num] = true;
						}
					}
					else
					{
						Err_set_error_message(Err_query_template_summary(GAS_RETURNED_NOT_SUCCESS), (char*)httpc_buffer, line_log_dl_thread_string, GAS_RETURNED_NOT_SUCCESS);
						Err_set_error_show_flag(true);
						failed = true;
						line_auto_update = false;
					}
				}
				else
				{
					Err_set_error_message(result.string, result.error_description, line_log_dl_thread_string, result.code);
					Err_set_error_show_flag(true);
					failed = true;
					line_auto_update = false;
				}

				for (int i = 0; i < 6; i++)
				{
					out_data[i] = "";
					out_data[i].reserve(10);
				}
			}

			if(failed)
				line_dl_log_failed[line_selected_room_num] = true;

			free(httpc_buffer);
			httpc_buffer = NULL;
			line_dl_log_request = false;
			if(line_dl_log_no_parse_request)
				line_dl_log_no_parse_request = false;
			else if(line_dl_all_log_no_parse_request)
				index++;
		}
		else if (line_solve_short_url_request)
		{
			failed = false;
			dl_size = 0;
			status_code = 0;
			httpc_buffer = (u8*)malloc(0x10000);

			if (httpc_buffer == NULL)
			{
				Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), line_log_dl_thread_string, OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				Log_log_save(line_log_dl_thread_string, Err_query_template_summary(OUT_OF_MEMORY), OUT_OF_MEMORY, false);
				line_auto_update = false;
			}
			else
			{
				memset(httpc_buffer, 0x0, 0x10000);
				log_num = Log_log_save(line_log_dl_thread_string, "Httpc_dl_data()...", 1234567890, false);
				result = Httpc_dl_data(line_short_url, httpc_buffer, 0x10000, &dl_size, &status_code, true, &last_url, true, 1, LINE_HTTP_PORT1);
				Log_log_add(log_num, result.string + " " + last_url + " status code" + std::to_string(status_code), result.code, false);
				line_log_dl_progress = Httpc_query_dl_progress(LINE_HTTP_PORT1);
				Menu_set_clipboard(last_url);
				line_check_main_url_request = true;

				if (result.code != 0)
				{
					Err_set_error_message(result.string, result.error_description, line_log_dl_thread_string, result.code);
					Err_set_error_show_flag(true);
				}
			}
			free(httpc_buffer);
			httpc_buffer = NULL;
			line_solve_short_url_request = false;
		}
		else
			usleep(ACTIW_THREAD_SLEEP_TIME);

		while (line_thread_suspend)
			usleep(INACTIW_THREAD_SLEEP_TIME);
	}
	Log_log_save(line_log_dl_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Line_send_message_thread(void* arg)
{
	Log_log_save(line_send_msg_thread_string, "Thread started.", 1234567890, false);
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
			log_num[0] = Log_log_save(line_send_msg_thread_string, "Httpc_post_and_dl_data()...", 1234567890, false);

			failed = false;
			line_send_success = false;
			line_sending_msg = true;
			dl_size = 0;
			file_size = 0;
			read_size = 0;
			total_read_size = 0;
			line_step_max = 1;
			line_current_step = 0;
			line_total_data_size = 0;
			line_sent_data_size = 0;
			content_fs_buffer[0] = NULL;
			content_fs_buffer[1] = NULL;
			check = NULL;
			httpc_buffer = (u8*)malloc(0x5000);
			Httpc_reset_post_and_dl_progress(LINE_HTTP_POST_PORT0);

			if (httpc_buffer == NULL)
			{
				Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), line_send_msg_thread_string, OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				Log_log_save(line_send_msg_thread_string, Err_query_template_summary(OUT_OF_MEMORY), OUT_OF_MEMORY, false);
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
					Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), line_send_msg_thread_string, OUT_OF_MEMORY);
					Err_set_error_show_flag(true);
					Log_log_save(line_send_msg_thread_string, Err_query_template_summary(OUT_OF_MEMORY), OUT_OF_MEMORY, false);
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
					send_data = "{ \"type\": \"send_text\",\"id\" : \"" + line_ids[line_selected_room_num] + "\",\"message\" : \"" + line_encoded_input_text + "\",\"auth\" : \"" + line_script_auth + "\",\"gas_ver\" : \"" + std::to_string(Sem_query_gas_ver()) + "\" }";
				else if (line_send_request[1])
				{
					if (line_selected_sticker_num >= 1 && line_selected_sticker_num <= 40)
						send_data = "{ \"type\": \"send_sticker\",\"id\" : \"" + line_ids[line_selected_room_num] + "\",\"package_id\" : \"11537\" ,\"sticker_id\" : \"" + std::to_string(line_sticker_num_list[line_selected_sticker_num]) + "\",\"auth\" : \"" + line_script_auth + "\",\"gas_ver\" : \"" + std::to_string(Sem_query_gas_ver()) + "\" }";
					else if (line_selected_sticker_num >= 41 && line_selected_sticker_num <= 80)
						send_data = "{ \"type\": \"send_sticker\",\"id\" : \"" + line_ids[line_selected_room_num] + "\",\"package_id\" : \"11538\" ,\"sticker_id\" : \"" + std::to_string(line_sticker_num_list[line_selected_sticker_num]) + "\",\"auth\" : \"" + line_script_auth + "\",\"gas_ver\" : \"" + std::to_string(Sem_query_gas_ver()) + "\" }";
					else if (line_selected_sticker_num >= 81 && line_selected_sticker_num <= 120)
						send_data = "{ \"type\": \"send_sticker\",\"id\" : \"" + line_ids[line_selected_room_num] + "\",\"package_id\" : \"11539\" ,\"sticker_id\" : \"" + std::to_string(line_sticker_num_list[line_selected_sticker_num]) + "\",\"auth\" : \"" + line_script_auth + "\",\"gas_ver\" : \"" + std::to_string(Sem_query_gas_ver()) + "\" }";
				}
				else if (line_send_request[2])
				{
					log_num[1] = Log_log_save(line_send_msg_thread_string, "File_check_file_size()...", 1234567890, false);
					result = File_check_file_size(line_send_file_name, line_send_file_dir, &file_size, fs_handle, fs_archive);
					Log_log_add(log_num[1], result.string, result.code, false);
					if ((int)file_size > line_send_fs_buffer_size)
					{
						Err_set_error_message(Err_query_template_summary(BUFFER_SIZE_IS_TOO_SMALL), Err_query_template_detail(BUFFER_SIZE_IS_TOO_SMALL), line_send_msg_thread_string, BUFFER_SIZE_IS_TOO_SMALL);
						Err_set_error_show_flag(true);
						Log_log_add(log_num[0], Err_query_template_summary(BUFFER_SIZE_IS_TOO_SMALL), BUFFER_SIZE_IS_TOO_SMALL, false);
						failed = true;
					}
					else
					{
						num_of_loop = ((int)file_size / line_send_fs_cache_buffer_size);
						for (int i = 0; i <= num_of_loop; i++)
						{
							log_num[1] = Log_log_save(line_send_msg_thread_string, "File_load_from_file()...", 1234567890, false);
							result = File_load_from_file_with_range(line_send_file_name, content_fs_buffer[1], line_send_fs_cache_buffer_size, u64(i * line_send_fs_cache_buffer_size), &read_size, line_send_file_dir, fs_handle, fs_archive);
							Log_log_add(log_num[1], result.string, result.code, false);
							memcpy(content_fs_buffer[0] + (i * line_send_fs_cache_buffer_size), content_fs_buffer[1], read_size);
							total_read_size += read_size;
							if (result.code != 0)
							{
								Err_set_error_message(result.string, result.error_description, line_send_msg_thread_string, result.code);
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
							if(check == NULL)
							{
								Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), line_send_msg_thread_string, OUT_OF_MEMORY);
								Err_set_error_show_flag(true);
								Log_log_add(log_num[0], Err_query_template_summary(OUT_OF_MEMORY), OUT_OF_MEMORY, false);
								failed = true;
							}
							free(check);
							check = NULL;

							if (!failed)
							{
								encoded_data = base64_encode((unsigned char const*)content_fs_buffer[0], (int)total_read_size);
								free(content_fs_buffer[0]);
								content_fs_buffer[0] = NULL;

								line_total_data_size = (int)encoded_data.length();
								num_of_loop = ((int)encoded_data.length() / line_send_fs_cache_buffer_size);
								line_step_max = num_of_loop + 1;
								for (int i = 0; i <= num_of_loop; i++)
								{
									check = (u8*)malloc((encoded_data.length() * 1.25)+ line_send_fs_cache_buffer_size);
									if(check == NULL)
									{
										Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), line_send_msg_thread_string, OUT_OF_MEMORY);
										Err_set_error_show_flag(true);
										Log_log_add(log_num[0], Err_query_template_summary(OUT_OF_MEMORY), OUT_OF_MEMORY, false);
										failed = true;
										break;
									}
									free(check);
									check = NULL;

									if (num_of_loop <= i)
										send_data = "{ \"type\": \"upload_content\",\"id\" : \"" + line_ids[line_selected_room_num] + "\",\"count\" : \"" + std::to_string(i) + "\",\"name\" : \"" + line_send_file_name + "\",\"content_data\" : \"" + encoded_data.substr((i * line_send_fs_cache_buffer_size), (encoded_data.length() - (i * line_send_fs_cache_buffer_size))) + "\",\"last\" : \"true\",\"auth\" : \"" + line_script_auth + "\",\"gas_ver\" : \"" + std::to_string(Sem_query_gas_ver()) + "\" }";
									else
										send_data = "{ \"type\": \"upload_content\",\"id\" : \"" + line_ids[line_selected_room_num] + "\",\"count\" : \"" + std::to_string(i) + "\",\"name\" : \"" + line_send_file_name + "\",\"content_data\" : \"" + encoded_data.substr((i * line_send_fs_cache_buffer_size), line_send_fs_cache_buffer_size) + "\",\"last\" : \"false\",\"auth\" : \"" + line_script_auth + "\",\"gas_ver\" : \"" + std::to_string(Sem_query_gas_ver()) + "\" }";

									log_num[1] = Log_log_save(line_send_msg_thread_string, "Httpc_post_and_dl_data()" + std::to_string(i) + "/" + std::to_string(num_of_loop) + "...", 1234567890, false);

									result = Httpc_post_and_dl_data(line_main_url, (char*)send_data.c_str(), send_data.length(), httpc_buffer, 0x5000, &dl_size, &status_code, true, LINE_HTTP_POST_PORT0);
									line_current_step++;
									if (result.code == 0)
									{
										response_string = (char*)httpc_buffer;
										if (response_string == "Success")
										{
											if (num_of_loop <= i)
												line_sent_data_size = line_total_data_size;
											else
												line_sent_data_size += line_send_fs_cache_buffer_size;

											Log_log_add(log_num[1], result.string, result.code, false);
										}
										else
										{
											Err_set_error_message(Err_query_template_summary(GAS_RETURNED_NOT_SUCCESS), (char*)httpc_buffer, line_send_msg_thread_string, GAS_RETURNED_NOT_SUCCESS);
											Err_set_error_show_flag(true);
											Log_log_add(log_num[1], Err_query_template_summary(GAS_RETURNED_NOT_SUCCESS), GAS_RETURNED_NOT_SUCCESS, false);
											failed = true;
											break;
										}
									}
									else
									{
										Err_set_error_message(result.string, result.error_description, line_send_msg_thread_string, result.code);
										Err_set_error_show_flag(true);
										Log_log_add(log_num[1], result.string, result.code, false);
										failed = true;
										break;
									}
								}
							}

							if(!failed)
							{
								line_send_success = true;
								Log_log_add(log_num[0], result.string, result.code, false);
							}
							send_data = "done";
						}
						else
						{
							Err_set_error_message(result.string, result.error_description, line_send_msg_thread_string, result.code);
							Err_set_error_show_flag(true);
							Log_log_add(log_num[0], result.string, result.code, false);
							failed = true;
						}
					}
				}
			}

			if (!failed && send_data != "done")
			{
				result = Httpc_post_and_dl_data(line_main_url, (char*)send_data.c_str(), send_data.length(), httpc_buffer, 0x5000, &dl_size, &status_code, true, LINE_HTTP_POST_PORT0);
				line_current_step++;
				if (result.code == 0)
				{
					response_string = (char*)httpc_buffer;
					if (response_string == "Success")
					{
						line_send_success = true;
						line_input_text = "";
						Log_log_add(log_num[0], result.string, result.code, false);
					}
					else
					{
						Err_set_error_message(Err_query_template_summary(GAS_RETURNED_NOT_SUCCESS), (char*)httpc_buffer, line_send_msg_thread_string, GAS_RETURNED_NOT_SUCCESS);
						Err_set_error_show_flag(true);
						Log_log_add(log_num[0], Err_query_template_summary(GAS_RETURNED_NOT_SUCCESS), GAS_RETURNED_NOT_SUCCESS, false);
					}
				}
				else
				{
					Err_set_error_message(result.string, result.error_description, line_send_msg_thread_string, result.code);
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
			free(content_fs_buffer[0]);
			free(content_fs_buffer[1]);
			free(check);
			httpc_buffer = NULL;
			content_fs_buffer[0] = NULL;
			content_fs_buffer[1] = NULL;
			check = NULL;

			line_sending_msg = false;
			if (line_send_request[0])
				line_send_request[0] = false;
			else if (line_send_request[1])
				line_send_request[1] = false;
			else if (line_send_request[2])
				line_send_request[2] = false;
		}
		else
			usleep(ACTIW_THREAD_SLEEP_TIME);

		while (line_thread_suspend)
			usleep(INACTIW_THREAD_SLEEP_TIME);
	}
	Log_log_save(line_send_msg_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

Result_with_string Line_save_new_id(std::string id, std::string dir_path)
{
	bool failed = false;
	FS_Archive fs_save_id_archive = 0;
	Result_with_string save_id_result;

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

Result_with_string Line_load_icon(int room_num)
{
	int image_width = 0;
	int image_height = 0;
	size_t cut_pos;
	u8* httpc_fs_buffer;
	u32 pic_size;
	u32 status_code;
	std::string file_name = "";
	std::string last_url;
	FS_Archive fs_archive = 0;
	Handle fs_handle = 0;
	Result_with_string result;

	httpc_fs_buffer = (u8*)malloc(0x4000);
	free(line_stb_image[room_num]);
	line_stb_image[room_num] = NULL;

	if (line_icon_url[room_num] != "")
	{
		memset(httpc_fs_buffer, 0x0, 0x4000);
		cut_pos = line_icon_url[room_num].find("lh3.googleusercontent.com/d/");
		if (!(cut_pos == std::string::npos))
		{
			file_name = line_icon_url[room_num].substr(cut_pos + 28);
			if (file_name.length() > 33)
				file_name = file_name.substr(0, 33);

			result = File_check_file_exist(file_name, "/Line/images/", fs_handle, fs_archive);
			if(result.code == 0)
				result = File_load_from_file(file_name, httpc_fs_buffer, 0x4000, &pic_size, "/Line/images/", fs_handle, fs_archive);

			if (result.code != 0)
			{
				memset(httpc_fs_buffer, 0x0, 0x4000);
				result = Httpc_dl_data(line_icon_url[room_num], httpc_fs_buffer, 0x4000, &pic_size, &status_code, true, &last_url, false, 100, LINE_HTTP_PORT2);
				File_save_to_file(file_name, (u8*)httpc_fs_buffer, pic_size, "/Line/images/", true, fs_handle, fs_archive);
			}
		}
		else
		{
			result.code = BAD_ICON_INFO;
			result.string = Err_query_template_summary(BAD_ICON_INFO);
			result.error_description = Err_query_template_detail(BAD_ICON_INFO);
		}

		if (result.code == 0)
		{
			free(line_stb_image[room_num]);
			line_stb_image[room_num] = NULL;
			line_stb_image[room_num] = stbi_load_from_memory((stbi_uc const*)httpc_fs_buffer, (int)pic_size, &image_width, &image_height, NULL, STBI_rgb_alpha);
			if (line_stb_image[room_num] != NULL)
			{
				linearFree(line_c3d_cache_tex[room_num]->data);
				line_icon[room_num].tex = line_c3d_cache_tex[room_num];
				line_icon[room_num].subtex = line_c3d_cache_subtex[room_num];
				Draw_rgba_to_abgr(line_stb_image[room_num], (u32)image_width, (u32)image_height);
				result = Draw_c3dtex_to_c2dimage(line_c3d_cache_tex[room_num], line_c3d_cache_subtex[room_num], line_stb_image[room_num], (u32)(image_width * image_height * 4), image_width, image_height, 0, 0, 32, 32, GPU_RGBA8);
			}
		}
	}
	else
	{
		result.code = NO_ICON_INFO;
		result.string = Err_query_template_summary(NO_ICON_INFO);
		result.error_description = Err_query_template_detail(NO_ICON_INFO);
	}

	free(httpc_fs_buffer);
	httpc_fs_buffer = NULL;
	return result;
}

Result_with_string Line_read_id(std::string dir_path)
{
	bool failed = false;
	std::string type[128];
	std::string data[4];
	Result_with_string result, cache_result;

	for (int i = 0; i < 128; i++)
	{
		line_ids[i] = "";
		line_names[i] = "";
		line_icon_url[i] = "";
	}

	result = File_read_dir(&line_num_of_ids, line_ids, 128, type, 128, "/Line/to/");
	if (result.code != 0)
		failed = true;

	if (!failed)
	{
		for (int i = 0; i < line_num_of_ids; i++)
		{
			for (int i = 0; i < 4; i++)
				data[i] = "";

			cache_result = Sem_load_setting(line_ids[i], "/Line/to/", 4, data);
			if(cache_result.code == 0)
			{
				line_names[i] = data[0];
				line_icon_url[i] = data[1];

				if(data[2] != "" && std::all_of(data[2].cbegin(), data[2].cend(), isdigit))
					line_msg_offset[i] = std::stoi(data[2]);
				if(data[3] != "" && std::all_of(data[3].cbegin(), data[3].cend(), isdigit))
					line_unread_msg_num[i] = std::stoi(data[3]);
			}
		}
	}

	return result;
}

Result_with_string Line_load_log_from_sd(std::string file_name)
{
	u8* fs_buffer;
	u32 read_size = 0;
	FS_Archive fs_archive = 0;
	Handle fs_handle = 0;
	Result_with_string result;

	fs_buffer = (u8*)malloc(line_log_fs_buffer_size);
	if (fs_buffer == NULL)
	{
		result.code = OUT_OF_MEMORY;
		result.string = Err_query_template_summary(OUT_OF_MEMORY);
		result.error_description = Err_query_template_detail(OUT_OF_MEMORY);
	}
	else
	{
		memset(fs_buffer, 0x0, line_log_fs_buffer_size);
		line_log_data = "";
		result = File_load_from_file(file_name, fs_buffer, line_log_fs_buffer_size, &read_size, "/Line/", fs_handle, fs_archive);

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
	Log_log_save(line_parse_thread_string, "Thread started.", 1234567890, false);
	int length_count = 0;
	int log_num;
	int cut_length = 60;
	int text_length = 0;
	u8* fs_buffer;
	u32 read_size;
	size_t image_url_end_pos;
	size_t image_url_start_pos;
	size_t sticker_end_pos;
	size_t sticker_start_pos;
	size_t id_end_pos;
	size_t id_start_pos;
	size_t message_start_pos;
	size_t message_next_pos;
	size_t new_line_pos;
	bool failed = false;
	bool sticker_msg = false;
	char* parse_cache;
	std::string content_cache = "";
	std::string image_url_end = "</image_url>";
	std::string image_url_start = "<image_url>";
	std::string sticker_end = "</sticker>";
	std::string sticker_start = "<sticker>";
	std::string id_end = "</id>";
	std::string id_start = "<id>";
	std::string message_start = "";
	std::string image_message = "";
	std::string id_message = "";
	std::string data[4];
	Result_with_string result;
	FS_Archive fs_archive = 0;
	Handle fs_handle = 0;

	while (line_log_parse_thread_run)
	{
		if (line_parse_log_request)
		{
			log_num = Log_log_save(line_parse_thread_string, "Parsing logs...", 1234567890, false);
			message_start_pos = std::string::npos;
			message_next_pos = std::string::npos;
			image_url_start_pos = std::string::npos;
			image_url_end_pos = std::string::npos;
			sticker_end_pos = std::string::npos;
			sticker_start_pos = std::string::npos;
			id_end_pos = std::string::npos;
			id_start_pos = std::string::npos;
			new_line_pos = std::string::npos;
			line_num_of_msg = 0;
			line_num_of_lines = 10;
			length_count = 0;
			cut_length = 60;
			text_length = 0;
			failed = false;
			for(int i = 0; i < 4; i++)
				data[i] = "";

			image_message = line_msg[27];
			id_message = line_msg[28];

			fs_buffer = (u8*)malloc(0x1000);
			parse_cache = (char*)malloc(0x10000);
			if (parse_cache == NULL)
			{
				Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), line_parse_thread_string, OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				Log_log_add(log_num, Err_query_template_summary(OUT_OF_MEMORY), OUT_OF_MEMORY, false);
			}
			else
			{
				for (int i = 0; i <= 3999; i++)
				{
					line_msg_pos[i] = -1;
					line_search_result_pos[i] = 0;
					line_short_msg_pos_start[i] = 0;
					line_short_msg_pos_end[i] = 0;
					line_msg_log[i] = "";
				}
				for (int i = 0; i < 59999; i++)
				{
					line_short_msg_log[i] = "";
					line_content[i] = "";
				}

				for (int i = 0; i <= 3999; i++)
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
					line_msg_pos[i] = -line_text_interval * line_num_of_lines;
					line_short_msg_pos_start[i] = line_num_of_lines;

					image_url_start_pos = line_msg_log[i].find(image_url_start);
					image_url_end_pos = line_msg_log[i].find(image_url_end);
					sticker_start_pos = line_msg_log[i].find(sticker_start);
					sticker_end_pos = line_msg_log[i].find(sticker_end);
					id_start_pos = line_msg_log[i].find(id_start);
					id_end_pos = line_msg_log[i].find(id_end);
					if (!(image_url_start_pos == std::string::npos || image_url_end_pos == std::string::npos))
					{
						line_content[line_num_of_lines + 1] = "<url>" + line_msg_log[i].substr((image_url_start_pos + image_url_start.length()), (image_url_end_pos - (image_url_start_pos + image_url_start.length()))) + "</url>";
						line_content[line_num_of_lines + 1] += "<type>image</type>";

						content_cache = line_msg_log[i].substr(0, image_url_start_pos);
						content_cache += image_message;
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
					else if (!(id_start_pos == std::string::npos || id_end_pos == std::string::npos))
					{
						line_content[line_num_of_lines + 1] = "<id>" + line_msg_log[i].substr((id_start_pos + id_start.length()), (id_end_pos - (id_start_pos + id_start.length()))) + "</id>";
						line_content[line_num_of_lines + 1] += "<type>id</type>";

						content_cache = line_msg_log[i].substr(0, id_start_pos);
						content_cache += id_message + "\n" + line_msg_log[i].substr((id_start_pos + id_start.length()), (id_end_pos - (id_start_pos + id_start.length())));
						content_cache += line_msg_log[i].substr(id_end_pos + id_end.length(), line_msg_log[i].length() - (id_end_pos + id_end.length()));
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

					line_short_msg_pos_end[i] = line_num_of_lines;
					line_max_y = (-line_text_interval * line_num_of_lines) + 100;
					line_text_y = line_max_y;
				}

				line_max_y = (-line_text_interval * line_num_of_lines) + 100;
				line_text_y = line_max_y;

				if (failed)
				{
					Err_set_error_message(Err_query_template_summary(TOO_MANY_MESSAGES), Err_query_template_detail(TOO_MANY_MESSAGES), line_parse_thread_string, TOO_MANY_MESSAGES);
					Err_set_error_show_flag(true);
					Log_log_add(log_num, Err_query_template_summary(TOO_MANY_MESSAGES), TOO_MANY_MESSAGES, false);
				}
				else
				{
					Log_log_add(log_num, Err_query_template_summary(0), 1234567890, false);

					log_num = Log_log_save(line_parse_thread_string, "File_load_from_file()...", 1234567890, false);
					result = File_load_from_file(line_ids[line_selected_room_num], fs_buffer, 0x1000, &read_size, "/Line/to/", fs_handle, fs_archive);
					Log_log_add(log_num , result.string, result.code, false);
					result = Sem_parse_file((char*)fs_buffer, 4, data);

					line_msg_offset[line_selected_room_num] += line_unread_msg_num[line_selected_room_num];
					line_unread_msg_num[line_selected_room_num] = 0;
					data[0] = "<0>" + data[0] + "</0>"; //user/group name
					data[1] = "<1>" + data[1] + "</1>"; //img url
					data[2] = "<2>" + std::to_string(line_msg_offset[line_selected_room_num]) + "</2>"; //msg offset
					data[3] = "<3>" + std::to_string(line_unread_msg_num[line_selected_room_num]) + "</3>"; //num of new msg

					log_num = Log_log_save(line_parse_thread_string, "File_save_to_file()...", 1234567890, false);
					result = File_save_to_file(line_ids[line_selected_room_num], (u8*)(data[0] + data[1] + data[2] + data[3]).c_str(), (data[0] + data[1] + data[2] + data[3]).length(), "/Line/to/", true, fs_handle, fs_archive);
					Log_log_add(log_num, result.string, result.code, false);
				}
			}

			if(line_num_of_msg <= line_selected_highlight_num)
				line_selected_highlight_num = 0;

			line_selected_search_highlight_num = 0;
			free(fs_buffer);
			free(parse_cache);
			fs_buffer = NULL;
			parse_cache = NULL;
			line_parse_log_request = false;
		}
		else
			usleep(ACTIW_THREAD_SLEEP_TIME);

		while (line_thread_suspend)
			usleep(INACTIW_THREAD_SLEEP_TIME);
	}
	Log_log_save(line_parse_thread_string, "Thread exit.", 1234567890, false);
}

void Line_log_load_thread(void* arg)
{
	Log_log_save(line_log_load_thread_string , "Thread started.", 1234567890, false);
	int log_load_log_return_num = 0;
	Result_with_string log_load_result;
	while (line_log_load_thread_run)
	{
		if (line_load_log_request)
		{
			log_load_log_return_num = Log_log_save(line_log_load_thread_string, "Line_load_from_log_sd()...", 1234567890, false);
			log_load_result = Line_load_log_from_sd(line_ids[line_selected_room_num].substr(0, 16));
			Log_log_add(log_load_log_return_num, log_load_result.string, log_load_result.code, false);
			if (log_load_result.code != 0 && log_load_result.code != (int)0xC8804478 && log_load_result.code != (int)0xC92044FA)
			{
				Err_set_error_message(log_load_result.string, log_load_result.error_description, line_log_load_thread_string , log_load_result.code);
				Err_set_error_show_flag(true);
			}
			line_load_log_request = false;
		}
		else
			usleep(ACTIW_THREAD_SLEEP_TIME);

		while (line_thread_suspend)
			usleep(INACTIW_THREAD_SLEEP_TIME);
	}
	Log_log_save(line_log_load_thread_string , "Thread exit.", 1234567890, false);
	threadExit(0);
}
