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
#include "system/util/httpc.hpp"
#include "system/util/log.hpp"
#include "system/util/swkbd.hpp"
#include "system/util/util.hpp"

#include "image_viewer.hpp"
#include "music_player.hpp"
#include "video_player.hpp"

//Include myself.
#include "line.hpp"

bool line_thread_suspend = false;
bool line_already_init = false;
bool line_delete_id_check_request = false;
bool line_delete_id_request = false;
bool line_main_run = false;
bool line_thread_run = false;
bool line_auto_update = false;
bool line_hide_id = false;
bool line_icon_dl_request = false;
bool line_parse_log_request = false;
bool line_load_log_request = false;
bool line_dl_log_request = false;
bool line_search_request = false;
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
bool line_scroll_mode = false;
bool line_scroll_bar_selected = false;
bool line_dl_content_request = false;
bool line_set_password_request = false;
bool line_type_password_request = false;
bool line_auth_success = false;
bool line_dl_log_failed[128];
bool line_icon_available[128];
bool line_button_selected[4];
u32 line_dled_content_size = 0;
int line_unread_msg_num[128];
int line_msg_offset[128];
int line_cool_time = 0;
int line_found = 0;
int line_num_of_logs = 150;
int line_selected_menu_mode = 0;
int line_selected_room_num = 0;
int line_selected_sticker_tab_num = 0;
int line_selected_sticker_num = 0;
int line_selected_highlight_num = 0;
int line_selected_search_highlight_num = 0;
int line_total_data_size = 0;
int line_sent_data_size = 0;
int line_uploaded_size = 0;
int line_num_of_ids = 0;
int line_post_and_dl_progress = 0;
int line_num_of_msg = 0;
int line_num_of_lines = 0;
int line_sticket_texture_num = -1;
int line_update_texture_num = -1;
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
double line_saved_y[2];
double line_text_x = 50.0;
double line_text_y = 0.0;
double line_text_size = 0.66;
double line_text_interval = 35.0;
double line_max_y = 10.0;
double line_selected_msg_num = 0.0;
double line_touch_x_move_left = 0.0;
double line_touch_y_move_left = 0.0;
std::string line_search_text = "";
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
std::string line_msg[DEF_LINE_NUM_OF_MSG];
std::string line_msg_log[4000];
std::string line_send_file_dir = "";
std::string line_send_file_name = "";
std::string line_content[60000];
std::string line_url = "";
std::string line_status = "";
std::string line_auth_code = "";
C2D_Image line_stickers_images[121];
Image_data line_icon[128], line_room_button[128], line_room_update_button[128], line_menu_button[6], line_change_main_url_short_button, line_add_new_id_button,
line_change_main_url_button, line_hide_id_button, line_change_app_password_button, line_change_gas_password_button, line_back_button, line_send_msg_button,
line_send_sticker_button, line_send_file_button, line_send_success_button, line_dl_logs_button, line_auto_update_button, line_max_logs_bar, line_copy_button,
line_copy_select_down_button, line_copy_select_up_button, line_increase_interval_button, line_decrease_interval_button, line_increase_size_button, line_decrease_size_button,
line_search_button, line_search_select_down_button, line_search_select_up_button, line_delete_room_button, line_yes_button, line_no_button, line_view_image_button,
line_edit_msg_button, line_content_button[16];
Thread line_init_thread, line_exit_thread, line_log_thread, line_worker_thread;

struct Line_upload_file_info
{
	bool first = true;
	bool end = false;
	int offset = 0;
	std::string filename = "";
	std::string dir = "";
	std::string first_data = "";
	std::string end_data = "";
};

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
		Draw_texture_free(&line_icon[i]);
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
	line_found = 0;
	line_text_y = 0.0;
	for (int i = 0; i < 4000; i++)
	{
		line_msg_log[i] = "";
		line_msg_log[i].reserve(1);
		line_msg_pos[i] = 0;
		line_search_result_pos[i] = 0;
		line_short_msg_pos_start[i] = 0;
		line_short_msg_pos_end[i] = 0;
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

Result_with_string Line_load_msg(std::string lang)
{
	return Util_load_msg("line_" + lang + ".txt", line_msg, DEF_LINE_NUM_OF_MSG);
}

void Line_set_send_file(std::string filename, std::string dir_name)
{
	line_send_file_name = filename;
	line_send_file_dir = dir_name;
	line_select_file_request = false;
	line_send_check[2] = true;
}

void Line_cancel_select_file(void)
{
	line_select_file_request = false;
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
	bool video = false;
	bool audio = false;
	bool hit = false;
	double pos_x;
	size_t cut_pos[5];
	std::string cache_string = "";
	std::string dir = "";
	Result_with_string result;

	if(Util_err_query_error_show_flag())
		Util_err_main(key);
	else if(Util_expl_query_show_flag())
		Util_expl_main(key);
	else
	{
		if (key.p_touch || key.h_touch)
		{
			line_touch_x_move_left = 0;
			line_touch_y_move_left = 0;
		}
		else
		{
			line_scroll_mode = false;
			line_scroll_bar_selected = false;
			line_touch_x_move_left -= (line_touch_x_move_left * 0.025);
			line_touch_y_move_left -= (line_touch_y_move_left * 0.025);
			if (line_touch_x_move_left < 0.5 && line_touch_x_move_left > -0.5)
				line_touch_x_move_left = 0;
			if (line_touch_y_move_left < 0.5 && line_touch_y_move_left > -0.5)
				line_touch_y_move_left = 0;
		}

		if(Util_hid_is_pressed(key, *Draw_get_bot_ui_button()))
			Draw_get_bot_ui_button()->selected = true;
		else if (key.p_start || (Util_hid_is_released(key, *Draw_get_bot_ui_button()) && Draw_get_bot_ui_button()->selected))
			Line_suspend();
		else if (line_select_chat_room_request)
		{
			if (!line_solve_short_url_request)
			{
				if(!line_dl_log_no_parse_request && !line_dl_all_log_no_parse_request)
				{
					for (int i = 0; i < 128; i++)
					{
						if(Util_hid_is_pressed(key, line_room_button[i]))
						{
							line_scroll_mode = false;
							line_room_button[i].selected = true;
							hit = true;
							break;
						}
						else if (Util_hid_is_released(key, line_room_button[i]) && line_room_button[i].selected)
						{
							line_selected_room_num = i;
							line_select_chat_room_request = false;
							line_saved_y[0] = line_text_y;
							line_text_y = line_saved_y[1];
							line_load_log_request = true;
							line_scroll_mode = false;
							break;
						}
						else if (!Util_hid_is_held(key, line_room_button[i]) && line_room_button[i].selected)
						{
							line_room_button[i].selected = false;
							line_scroll_mode = true;
							break;
						}
						else if(Util_hid_is_pressed(key, line_room_update_button[i]))
						{
							line_scroll_mode = false;
							line_room_update_button[i].selected = true;
							hit = true;
							break;
						}
						else if (Util_hid_is_released(key, line_room_update_button[i]) && line_room_update_button[i].selected)
						{
							line_selected_room_num = i;
							line_dl_log_no_parse_request = true;
							line_scroll_mode = false;
							break;
						}
						else if (!Util_hid_is_held(key, line_room_update_button[i]) && line_room_update_button[i].selected)
						{
							line_room_update_button[i].selected = false;
							line_scroll_mode = true;
							break;
						}
					}

					//if (hid_key_ZL_press)
					//	Line_set_operation_flag(LINE_DL_ALL_LOG_NO_PARSE_REQUEST, true);
					if(Util_hid_is_pressed(key, line_add_new_id_button))
						line_add_new_id_button.selected = true;
					else if (key.p_y || (Util_hid_is_released(key, line_add_new_id_button) && line_add_new_id_button.selected))
					{
						line_type_id_request = true;
						while(line_type_id_request)
							Util_sleep(20000);
					}
					else if(Util_hid_is_pressed(key, line_change_main_url_short_button))
						line_change_main_url_short_button.selected = true;
					else if ((key.p_l && key.h_r) || (key.h_l && key.p_r) || (Util_hid_is_released(key, line_change_main_url_short_button) && line_change_main_url_short_button.selected))
					{
						line_type_short_url_request = true;
						while(line_type_short_url_request)
							Util_sleep(20000);
					}
					else if(Util_hid_is_pressed(key, line_change_main_url_button))
						line_change_main_url_button.selected = true;
					else if (key.p_x || (Util_hid_is_released(key, line_change_main_url_button) && line_change_main_url_button.selected))
					{
						line_type_main_url_request = true;
						while(line_type_main_url_request)
							Util_sleep(20000);
					}
					else if(Util_hid_is_pressed(key, line_hide_id_button))
						line_hide_id_button.selected = true;
					else if (Util_hid_is_released(key, line_hide_id_button) && line_hide_id_button.selected)
						line_hide_id = !line_hide_id;
					else if(Util_hid_is_pressed(key, line_change_app_password_button))
						line_change_app_password_button.selected = true;
					else if (key.p_a || (Util_hid_is_released(key, line_change_app_password_button) && line_change_app_password_button.selected))
					{
						line_type_app_ps_request = true;
						while(line_type_app_ps_request)
							Util_sleep(20000);
					}
					else if(Util_hid_is_pressed(key, line_change_gas_password_button))
						line_change_gas_password_button.selected = true;
					else if (key.p_b || (Util_hid_is_released(key, line_change_gas_password_button) && line_change_gas_password_button.selected))
					{
						line_type_script_ps_request = true;
						while(line_type_script_ps_request)
							Util_sleep(20000);
					}
				}

				if (!hit && key.p_touch && key.touch_x >= 305 && key.touch_x <= 320 && key.touch_y >= 15 && key.touch_y < 220)
					line_scroll_bar_selected = true;
				else if (!hit && key.p_touch && key.touch_y <= 169)
					line_scroll_mode = true;
			}
		}
		else
		{
			if (line_send_check[0] || line_send_check[1] || line_send_check[2] || line_delete_id_check_request)
			{
				if(Util_hid_is_pressed(key, line_yes_button))
					line_yes_button.selected = true;
				else if (key.p_a || (Util_hid_is_released(key, line_yes_button) && line_yes_button.selected))
				{
					if (line_send_check[0])
					{
						line_send_request[0] = true;
						line_send_check[0] = false;
					}
					else if (line_send_check[1])
					{
						line_send_request[1] = true;
						line_send_check[1] = false;
					}
					else if (line_send_check[2])
					{
						line_send_request[2] = true;
						line_send_check[2] = false;
					}
					else if (line_delete_id_check_request && !line_dl_log_request && !line_parse_log_request && !line_auto_update && !line_load_log_request)
					{
						line_saved_y[1] = line_text_y;
						line_text_y = line_saved_y[0];
						line_delete_id_request = true;
						line_delete_id_check_request = false;
						Line_reset_msg();
					}
				}
				else if(Util_hid_is_pressed(key, line_no_button))
					line_no_button.selected = true;
				else if (key.p_b || (Util_hid_is_released(key, line_no_button) && line_no_button.selected))
				{
					if (line_send_check[0])
						line_send_check[0] = false;
					else if (line_send_check[1])
						line_send_check[1] = false;
					else if (line_send_check[2])
						line_send_check[2] = false;
					else if(line_delete_id_check_request)
						line_delete_id_check_request = false;
				}
				else if(Util_hid_is_pressed(key, line_edit_msg_button) && line_send_check[0])
					line_edit_msg_button.selected = true;
				else if ((key.p_x || (Util_hid_is_released(key, line_edit_msg_button) && line_edit_msg_button.selected)) && line_send_check[0])
				{
					line_type_msg_request = true;
					line_send_check[0] = false;
					while(line_type_msg_request)
						Util_sleep(20000);
				}
				else if(Util_hid_is_pressed(key, line_view_image_button) && line_send_check[2])
					line_view_image_button.selected = true;
				else if ((key.p_x || (Util_hid_is_released(key, line_view_image_button) && line_view_image_button.selected)) && line_send_check[2])
				{
					Imv_set_load_file(line_send_file_name, line_send_file_dir);
					Line_suspend();
					if(!Imv_query_init_flag())
						Imv_init(false);
					else
						Imv_resume();
				}
			}
			else if (line_select_sticker_request)
			{
				if (key.p_b || (key.p_touch && key.touch_x >= 290 && key.touch_x <= 309 && key.touch_y > 120 && key.touch_y < 139))
				{
					hit = true;
					line_select_sticker_request = false;
				}

				for (int i = 0; i < 10; i++)
				{
					if (key.p_touch && key.touch_x >= 10 + (i * 30) && key.touch_x <= 39 + (i * 30) && key.touch_y > 140 && key.touch_y < 149)
						line_selected_sticker_tab_num = i;
				}

				for (int i = 0; i < 7; i++)
				{
					if (key.p_touch && key.touch_x >= 20 + (i * 50) && key.touch_x <= 49 + (i * 50) && key.touch_y > 150 && key.touch_y < 179)
					{
						line_selected_sticker_num = (line_selected_sticker_tab_num * 12) + (i + 1);
						line_send_check[1] = true;
						line_select_sticker_request = false;
					}
					else if (key.p_touch && key.touch_x >= 20 + (i * 50) && key.touch_x <= 49 + (i * 50) && key.touch_y > 190 && key.touch_y < 219)
					{
						line_selected_sticker_num = (line_selected_sticker_tab_num * 12) + (i + 7);
						line_send_check[1] = true;
						line_select_sticker_request = false;
					}
				}
			}
			else
			{
				if(Util_hid_is_pressed(key, line_menu_button[0]))
					line_menu_button[0].selected = true;
				else if (Util_hid_is_released(key, line_menu_button[0]) && line_menu_button[0].selected)
					line_selected_menu_mode = DEF_LINE_MENU_SEND;
				else if(Util_hid_is_pressed(key, line_menu_button[1]))
					line_menu_button[1].selected = true;
				else if (Util_hid_is_released(key, line_menu_button[1]) && line_menu_button[1].selected)
					line_selected_menu_mode = DEF_LINE_MENU_RECEIVE;
				else if(Util_hid_is_pressed(key, line_menu_button[2]))
					line_menu_button[2].selected = true;
				else if (Util_hid_is_released(key, line_menu_button[2]) && line_menu_button[2].selected)
					line_selected_menu_mode = DEF_LINE_MENU_COPY;
				else if(Util_hid_is_pressed(key, line_menu_button[3]))
					line_menu_button[3].selected = true;
				else if (Util_hid_is_released(key, line_menu_button[3]) && line_menu_button[3].selected)
					line_selected_menu_mode = DEF_LINE_MENU_SETTINGS;
				else if(Util_hid_is_pressed(key, line_menu_button[4]))
					line_menu_button[4].selected = true;
				else if (Util_hid_is_released(key, line_menu_button[4]) && line_menu_button[4].selected)
					line_selected_menu_mode = DEF_LINE_MENU_SEARCH;
				else if(Util_hid_is_pressed(key, line_menu_button[5]))
					line_menu_button[5].selected = true;
				else if (Util_hid_is_released(key, line_menu_button[5]) && line_menu_button[5].selected)
					line_selected_menu_mode = DEF_LINE_MENU_ADVANCED;
				else if(Util_hid_is_pressed(key, line_back_button) && !line_dl_log_request && !line_parse_log_request && !line_auto_update &&
				!line_load_log_request && !line_send_request[0] && !line_send_request[1] && !line_send_request[2])
					line_back_button.selected = true;
				else if (Util_hid_is_released(key, line_back_button) && line_back_button.selected && !line_dl_log_request && !line_parse_log_request && !line_auto_update &&
				!line_load_log_request && !line_send_request[0] && !line_send_request[1] && !line_send_request[2])
				{
					line_saved_y[1] = line_text_y;
					Line_reset_msg();
					line_text_y = line_saved_y[0];
					line_select_chat_room_request = true;
				}
				else if (line_selected_menu_mode == DEF_LINE_MENU_SEND && line_send_success)
				{
					if(Util_hid_is_pressed(key, line_send_success_button))
						line_send_success_button.selected = true;
					else if (key.p_a || (Util_hid_is_released(key, line_send_success_button) && line_send_success_button.selected))
						line_send_success = false;
				}
				else if (line_selected_menu_mode == DEF_LINE_MENU_SEND && !line_sending_msg)
				{
					if(Util_hid_is_pressed(key, line_send_msg_button))
						line_send_msg_button.selected = true;
					else if (key.p_a || (Util_hid_is_released(key, line_send_msg_button) && line_send_msg_button.selected))
					{
						line_type_msg_request = true;
						while(line_type_msg_request)
							Util_sleep(20000);
					}
					else if(Util_hid_is_pressed(key, line_send_sticker_button))
						line_send_sticker_button.selected = true;
					else if (key.p_y || (Util_hid_is_released(key, line_send_sticker_button) && line_send_sticker_button.selected))
						line_select_sticker_request = true;
					else if(Util_hid_is_pressed(key, line_send_file_button))
						line_send_file_button.selected = true;
					else if (key.p_x || (Util_hid_is_released(key, line_send_file_button) && line_send_file_button.selected))
					{
						Util_expl_set_callback(Line_set_send_file);
						Util_expl_set_cancel_callback(Line_cancel_select_file);
						Util_expl_set_show_flag(true);
					}
				}
				else if (line_selected_menu_mode == DEF_LINE_MENU_RECEIVE)
				{
					if(Util_hid_is_pressed(key, line_dl_logs_button))
						line_dl_logs_button.selected = true;
					else if (key.p_b || (Util_hid_is_released(key, line_dl_logs_button) && line_dl_logs_button.selected))
						line_dl_log_request = true;
					else if(Util_hid_is_pressed(key, line_auto_update_button))
						line_auto_update_button.selected = true;
					else if (key.p_x || (Util_hid_is_released(key, line_auto_update_button) && line_auto_update_button.selected))
						line_auto_update = !line_auto_update;
					else if(Util_hid_is_pressed(key, line_max_logs_bar) && !line_dl_log_request && !line_auto_update && !line_sending_msg)
						line_max_logs_bar.selected = true;
					else if (key.h_touch && line_max_logs_bar.selected)
					{
						if(key.touch_x <= 99)
							pos_x = 100;
						else if(key.touch_x >= 300)
							pos_x = 299;
						else
							pos_x = key.touch_x;

						line_num_of_logs = (pos_x - 99) * 20;
					}
				}
				else if (line_selected_menu_mode == DEF_LINE_MENU_COPY)
				{
					if(Util_hid_is_pressed(key, line_copy_button))
						line_copy_button.selected = true;
					else if (key.p_x || (Util_hid_is_released(key, line_copy_button) && line_copy_button.selected))
						var_clipboard = line_msg_log[line_selected_highlight_num];
					else if(Util_hid_is_pressed(key, line_copy_select_down_button))
						line_copy_select_down_button.selected = true;
					else if(key.h_d_down || (Util_hid_is_held(key, line_copy_select_down_button) && line_copy_select_down_button.selected))
					{
						if(line_cool_time > 0)
							line_cool_time--;
						else if(line_num_of_msg - 1 >= line_selected_highlight_num + 1)
						{
							line_selected_highlight_num++;
							if(key.held_time > 150)
								line_cool_time = 1;
							else
								line_cool_time = 20;
						}
						line_text_y = line_msg_pos[line_selected_highlight_num];
					}
					else if(Util_hid_is_pressed(key, line_copy_select_up_button))
						line_copy_select_up_button.selected = true;
					else if(key.h_d_up || (Util_hid_is_held(key, line_copy_select_up_button) && line_copy_select_up_button.selected))
					{
						if(line_cool_time > 0)
							line_cool_time--;
						else if(line_selected_highlight_num - 1 >= 0)
						{
							line_selected_highlight_num--;
							if(key.held_time > 150)
								line_cool_time = 1;
							else
								line_cool_time = 20;
						}
						line_text_y = line_msg_pos[line_selected_highlight_num];
					}
				}
				else if (line_selected_menu_mode == DEF_LINE_MENU_SETTINGS)
				{
					if (Util_hid_is_pressed(key, line_increase_interval_button))
						line_increase_interval_button.selected = true;
					else if (Util_hid_is_pressed(key, line_decrease_interval_button))
						line_decrease_interval_button.selected = true;
					else if (Util_hid_is_pressed(key, line_decrease_size_button))
						line_decrease_size_button.selected = true;
					else if (Util_hid_is_pressed(key, line_increase_size_button))
						line_increase_size_button.selected = true;
					else if ((key.h_d_up || (Util_hid_is_held(key, line_increase_interval_button) && line_increase_interval_button.selected)) && (line_text_interval + 0.5) < 250.0)
						line_text_interval += 0.5;
					else if ((key.h_d_down || (Util_hid_is_held(key, line_decrease_interval_button) && line_decrease_interval_button.selected)) && (line_text_interval - 0.5) > 10.0)
						line_text_interval -= 0.5;
					else if ((key.h_l || (Util_hid_is_held(key, line_decrease_size_button) && line_decrease_size_button.selected)) && (line_text_size - 0.003) > 0.25)
						line_text_size -= 0.003;
					else if ((key.h_r || (Util_hid_is_held(key, line_increase_size_button) && line_increase_size_button.selected)) && (line_text_size + 0.003) < 3.0)
						line_text_size += 0.003;
				}
				else if (line_selected_menu_mode == DEF_LINE_MENU_SEARCH)
				{
					if (Util_hid_is_pressed(key, line_search_button))
						line_search_button.selected = true;
					else if (key.p_a || (Util_hid_is_released(key, line_search_button) && line_search_button.selected))
					{
						line_search_request = true;
						while(line_search_request)
							Util_sleep(20000);
					}
					else if (Util_hid_is_pressed(key, line_search_select_down_button))
						line_search_select_down_button.selected = true;
					else if((key.h_d_down || (Util_hid_is_held(key, line_search_select_down_button) && line_search_select_down_button.selected)) && line_found >= line_selected_search_highlight_num + 1)
					{
						if(line_cool_time > 0)
							line_cool_time--;
						else
						{
							line_selected_search_highlight_num++;

							if(key.held_time > 150)
								line_cool_time = 1;
							else
								line_cool_time = 20;

							line_selected_highlight_num = line_search_result_pos[line_selected_search_highlight_num];
							line_text_y = line_msg_pos[line_selected_highlight_num];
						}
					}
					else if (Util_hid_is_pressed(key, line_search_select_up_button))
						line_search_select_up_button.selected = true;
					else if((key.h_d_up || (Util_hid_is_held(key, line_search_select_up_button) && line_search_select_up_button.selected)) && line_selected_search_highlight_num - 1 >= 0)
					{
						if(line_cool_time > 0)
							line_cool_time--;
						else
						{
							line_selected_search_highlight_num--;

							if(key.held_time > 150)
								line_cool_time = 1;
							else
								line_cool_time = 20;

							line_selected_highlight_num = line_search_result_pos[line_selected_search_highlight_num];
							line_text_y = line_msg_pos[line_selected_highlight_num];
						}
					}
				}
				else if (line_selected_menu_mode == DEF_LINE_MENU_ADVANCED)
				{
					if(Util_hid_is_pressed(key, line_delete_room_button))
						line_delete_room_button.selected = true;
					else if (((key.p_l && key.h_r) || (key.h_l && key.p_r)) || (Util_hid_is_released(key, line_delete_room_button) && line_delete_room_button.selected))
						line_delete_id_check_request = true;
				}
			}

			if (!hit && key.p_touch && key.touch_x >= 305 && key.touch_x <= 320 && key.touch_y >= 15 && key.touch_y < 220)
				line_scroll_bar_selected = true;
			else if (!hit && (key.p_touch || key.r_touch || key.h_touch) && key.touch_y >= 0 && key.touch_y < 140)
			{
				for (int i = 1, content_index = 0; i <= 59999; i++)
				{
					if (i > line_num_of_lines || (line_text_y + line_text_interval * i) - 240 >= 130)
						break;
					else if (line_text_y + line_text_interval * i <= -1000)
					{
						if ((line_text_y + line_text_interval * (i + 100)) <= 10)
							i += 100;
					}
					else if ((line_text_y + line_text_interval * i) - 240 <= -10)
					{
					}
					else
					{
						cut_pos[0] = line_content[i].find("<type>image</type>");
						cut_pos[1] = line_content[i].find("<type>id</type>");
						cut_pos[2] = line_content[i].find("<type>video</type>");
						cut_pos[3] = line_content[i].find("<type>audio</type>");
						cut_pos[4] = line_content[i].find("<type>file</type>");

						if(cut_pos[0] != std::string::npos || cut_pos[1] != std::string::npos || cut_pos[2] != std::string::npos || cut_pos[3] != std::string::npos || cut_pos[4] != std::string::npos)
						{
							if(Util_hid_is_pressed(key, line_content_button[content_index]))
							{
								line_scroll_mode = false;
								hit = true;
								line_content_button[content_index].selected = true;
							}
							//jump to imv
							else if (cut_pos[0] != std::string::npos && Util_hid_is_released(key, line_content_button[content_index]) && line_content_button[content_index].selected)
							{
								hit = true;
								result.code = -1;
								cut_pos[0] = line_content[i].find("&id=");
								cut_pos[1] = line_content[i].find("om/d/");
								if (!(cut_pos[0] == std::string::npos && cut_pos[1] == std::string::npos))
								{
									cache_string = "";
									if (!(cut_pos[0] == std::string::npos))
										cache_string = line_content[i].substr(cut_pos[0] + 4);
									else if (!(cut_pos[1] == std::string::npos))
										cache_string = line_content[i].substr(cut_pos[1] + 5);

									if (cache_string.length() > 33)
										cache_string = cache_string.substr(0, 33);

									cache_string += ".jpg";
									result = Util_file_check_file_exist(cache_string, DEF_MAIN_DIR + "images/");
								}

								if (result.code == 0)
									Imv_set_load_file(cache_string, DEF_MAIN_DIR + "images/");
								else
								{
									cut_pos[0] = line_content[i].find("<url>");
									cut_pos[1] = line_content[i].find("</url>");
									if (cut_pos[0] == std::string::npos || cut_pos[1] == std::string::npos)
										Imv_set_url("");
									else
										Imv_set_url(line_content[i].substr((cut_pos[0] + 5), cut_pos[1] - (cut_pos[0] + 5)));
								}
								Line_suspend();
								if(!Imv_query_init_flag())
									Imv_init(false);
								else
									Imv_resume();

								break;
							}
							//jump to mup or vid
							else if ((cut_pos[2] != std::string::npos || cut_pos[3] != std::string::npos)
							&& Util_hid_is_released(key, line_content_button[content_index]) && line_content_button[content_index].selected)
							{
								if(!(cut_pos[2] == std::string::npos))
									video = true;
								else if(!(cut_pos[3] == std::string::npos))
									audio = true;

								hit = true;
								result.code = -1;
								cut_pos[0] = line_content[i].find("&id=");
								if (!(cut_pos[0] == std::string::npos))
								{
									cache_string = "";
									if (!(cut_pos[0] == std::string::npos))
										cache_string = line_content[i].substr(cut_pos[0] + 4);

									if (cache_string.length() > 33)
										cache_string = cache_string.substr(0, 33);

									if(video)
									{
										dir = DEF_MAIN_DIR + "videos/";
										cache_string += ".mp4";
									}
									else if(audio)
									{
										dir = DEF_MAIN_DIR + "audio/";
										cache_string += ".m4a";
									}

									result = Util_file_check_file_exist(cache_string, dir);
								}

								if (result.code == 0)
								{
									if(video)
									{
										Vid_set_load_file(cache_string, dir);
									}
									else if(audio)
										Mup_set_load_file(cache_string, dir);
								}
								else
								{
									cut_pos[0] = line_content[i].find("<url>");
									cut_pos[1] = line_content[i].find("</url>");
									if (cut_pos[0] == std::string::npos || cut_pos[1] == std::string::npos)
									{
										if(video)
										{
											Vid_set_url("");
										}
										else if(audio)
											Mup_set_url("");
									}
									else
									{
										if(video)
											Vid_set_url(line_content[i].substr((cut_pos[0] + 5), cut_pos[1] - (cut_pos[0] + 5)));
										else if(audio)
											Mup_set_url(line_content[i].substr((cut_pos[0] + 5), cut_pos[1] - (cut_pos[0] + 5)));
									}
								}
								Line_suspend();
								if(video)
								{
									if(!Vid_query_init_flag())
										Vid_init(false);
									else
										Vid_resume();
								}
								else if(audio)
								{
									if(!Mup_query_init_flag())
										Mup_init(false);
									else
										Mup_resume();
								}
								break;
							}
							else if (cut_pos[4] != std::string::npos && Util_hid_is_released(key, line_content_button[content_index]) && line_content_button[content_index].selected)
							{
								hit = true;
								result.code = -1;
								cut_pos[0] = line_content[i].find("&id=");
								cut_pos[1] = line_content[i].find("om/d/");
								if (!(cut_pos[0] == std::string::npos && cut_pos[1] == std::string::npos))
								{
									cache_string = "";
									if (!(cut_pos[0] == std::string::npos))
										cache_string = line_content[i].substr(cut_pos[0] + 4);
									else if (!(cut_pos[1] == std::string::npos))
										cache_string = line_content[i].substr(cut_pos[1] + 5);

									if (cache_string.length() > 33)
										cache_string = cache_string.substr(0, 33);

									result = Util_file_check_file_exist(cache_string, DEF_MAIN_DIR + "contents/");
								}

								if (result.code != 0)
								{
									cut_pos[0] = line_content[i].find("<url>");
									cut_pos[1] = line_content[i].find("</url>");
									if (cut_pos[0] == std::string::npos || cut_pos[1] == std::string::npos)
										line_url = "";
									else
										line_url = line_content[i].substr((cut_pos[0] + 5), cut_pos[1] - (cut_pos[0] + 5));

									line_dl_content_request = true;
								}
								break;
							}
							//add id
							else if (cut_pos[1] != std::string::npos && Util_hid_is_released(key, line_content_button[content_index]) && line_content_button[content_index].selected
							&& !line_dl_log_request && !line_parse_log_request && !line_auto_update && !line_load_log_request)
							{
								hit = true;
								cut_pos[0] = line_content[i].find("<id>");
								cut_pos[1] = line_content[i].find("</id>");
								if (cut_pos[0] == std::string::npos || cut_pos[1] == std::string::npos)
									var_clipboard = "";
								else
									var_clipboard = line_content[i].substr((cut_pos[0] + 4), cut_pos[1] - (cut_pos[0] + 4));

								line_saved_y[1] = line_text_y;
								Line_reset_msg();
								line_text_y = line_saved_y[0];
								line_select_chat_room_request = true;
								line_type_id_request = true;

								while(line_type_id_request)
									Util_sleep(20000);

								break;
							}
							else if(!Util_hid_is_held(key, line_content_button[content_index]) && line_content_button[content_index].selected)
							{
								line_scroll_mode = true;
								line_content_button[content_index].selected = false;
								break;
							}

							content_index++;
						}
					}
				}
			}

			if (!hit && key.p_touch && key.touch_y < 110)
				line_scroll_mode = true;
			else if (!hit && key.p_touch && key.touch_y < 140 && !(line_send_check[0] || line_send_check[1] || line_send_check[2] || line_delete_id_check_request))
				line_scroll_mode = true;
		}

		if (key.h_c_down || key.h_c_up)
		{
			if (key.held_time > 600)
				line_text_y += (double)key.cpad_y * var_scroll_speed * 0.5;
			else if (key.held_time > 240)
				line_text_y += (double)key.cpad_y * var_scroll_speed * 0.125;
			else
				line_text_y += (double)key.cpad_y * var_scroll_speed * 0.0625;
		}
		if (key.h_c_left || key.h_c_right)
		{
			/*if (key.held_time > 240)
				line_text_x -= (double)key.cpad_x * var_scroll_speed * 0.125;
			else
				line_text_x -= (double)key.cpad_x * var_scroll_speed * 0.0625;*/
		}

		if (line_scroll_bar_selected && key.h_touch)
		{
			line_touch_x_move_left = 0;
			line_touch_y_move_left = 0;
			line_text_y = line_max_y * ((key.touch_y - 15.0) / 195.0);
		}
		else if (line_scroll_mode && key.h_touch)
		{
			line_touch_x_move_left += key.touch_x_move;
			line_touch_y_move_left += key.touch_y_move;
		}

		//line_text_x -= line_touch_x_move_left * var_scroll_speed;
		line_text_y -= line_touch_y_move_left * var_scroll_speed;

		if (line_text_y > 0.0)
			line_text_y = 0.0;
		if (line_text_y < line_max_y)
			line_text_y = line_max_y;
		/*if (line_text_x > 40.0)
			line_text_x = 40.0;
		if (line_text_x < -500.0)
			line_text_x = -500.0;*/
	}

	if(!key.p_touch && !key.h_touch)
	{
		Draw_get_bot_ui_button()->selected = false;
		line_change_main_url_short_button.selected = false;
		line_add_new_id_button.selected = false;
		line_change_main_url_button.selected = false;
		line_hide_id_button.selected = false;
		line_change_app_password_button.selected = false;
		line_change_gas_password_button.selected = false;
		line_back_button.selected = false;
		line_send_msg_button.selected = false;
		line_send_sticker_button.selected = false;
		line_send_file_button.selected = false;
		line_send_success_button.selected = false;
		line_dl_logs_button.selected = false;
		line_auto_update_button.selected = false;
		line_max_logs_bar.selected = false;
		line_copy_button.selected = false;
		line_copy_select_down_button.selected = false;
		line_copy_select_up_button.selected = false;
		line_increase_interval_button.selected = false;
		line_decrease_interval_button.selected = false;
		line_increase_size_button.selected = false;
		line_decrease_size_button.selected = false;
		line_search_button.selected = false;
		line_search_select_down_button.selected = false;
		line_search_select_up_button.selected = false;
		line_delete_room_button.selected = false;
		line_yes_button.selected = false;
		line_no_button.selected = false;
		line_view_image_button.selected = false;
		line_edit_msg_button.selected = false;
		for(int i = 0; i < 16; i++)
			line_content_button[i].selected = false;
		for(int i = 0; i < 6; i++)
			line_menu_button[i].selected = false;
		for(int i = 0; i < 128; i++)
		{
			line_room_button[i].selected = false;
			line_room_update_button[i].selected = false;
		}
	}

	if(Util_log_query_log_show_flag())
		Util_log_main(key);
}

Result_with_string Line_read_id(std::string dir_path)
{
	u8* buffer = NULL;
	File_type type[128];
	std::string data[4];
	Result_with_string result;

	for (int i = 0; i < 128; i++)
	{
		line_ids[i] = "";
		line_names[i] = "";
		line_icon_url[i] = "";
	}
	for (int i = 0; i < 4; i++)
		data[i] = "";

	result = Util_file_read_dir(dir_path, &line_num_of_ids, line_ids, type, 128);
	if (result.code == 0)
	{
		for (int i = 0; i < line_num_of_ids; i++)
		{
			result = Util_file_load_from_file(line_ids[i], dir_path, &buffer, 0x10000);
			if(result.code == 0)
			{
				result = Util_parse_file((char*)buffer, 4, data);
				if(result.code == 0)
				{
					line_names[i] = data[0];
					line_icon_url[i] = data[1];
					line_msg_offset[i] = atoi(data[2].c_str());
					line_unread_msg_num[i] = atoi(data[3].c_str());
				}
			}

			free(buffer);
			buffer = NULL;
		}
	}

	return result;
}

Result_with_string Line_load_icon(int room_num)
{
	int image_width = 0;
	int image_height = 0;
	size_t cut_pos = 0;
	u8* raw_buffer = NULL;
	u32 dled_size = 0;
	std::string filename = "";
	Result_with_string result;

	Draw_texture_free(&line_icon[room_num]);

	if (line_icon_url[room_num] != "")
	{
		cut_pos = line_icon_url[room_num].find("lh3.googleusercontent.com/d/");
		if (!(cut_pos == std::string::npos))
		{
			filename = line_icon_url[room_num].substr(cut_pos + 28);
			if (filename.length() > 33)
				filename = filename.substr(0, 33);

			result = Util_file_check_file_exist(filename, DEF_MAIN_DIR + "images/");

			//If file is not exist, download it
			if (result.code != 0)
				result = Util_httpc_save_data(line_icon_url[room_num], 1024 * 128, &dled_size, true, 5, DEF_MAIN_DIR + "images/", filename);

			if(result.code == 0)
			{
				Pixel_format pixel_format = PIXEL_FORMAT_INVALID;

				result = Util_image_decoder_decode(DEF_MAIN_DIR + "images/" + filename, &raw_buffer, &image_width, &image_height, &pixel_format);
				if(result.code == 0)
				{
					result = Draw_texture_init(&line_icon[room_num], 32, 32, PIXEL_FORMAT_RGB565LE);
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
							Draw_set_texture_filter(&line_icon[room_num], true);
							Draw_set_texture_data(&line_icon[room_num], converter_parameters.converted, converter_parameters.out_width, converter_parameters.out_height);
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

Result_with_string Line_load_log_from_sd(std::string filename)
{
	u8* buffer = NULL;
	Result_with_string result;

	line_log_data = "";
	result = Util_file_load_from_file(filename, DEF_MAIN_DIR, &buffer, 1024 * 1024 * 5);

	if (result.code == 0)
	{
		line_log_data = (char*)buffer;
		line_parse_log_request = true;
	}

	free(buffer);
	buffer = NULL;
	return result;
}

void Line_worker_thread(void* arg)
{
	Util_log_save(DEF_LINE_WORKER_THREAD_STR, "Thread started.");
	int log_num = 0;
	size_t cut_pos = 0;
	std::string filename = "";
	Result_with_string result;

	while (line_thread_run)
	{
		if (line_icon_dl_request)
		{
			line_icon_dl_request = false;
			Line_reset_id();
			log_num = Util_log_save(DEF_LINE_WORKER_THREAD_STR, "Line_read_id()...");
			result = Line_read_id(DEF_MAIN_DIR + "to/");
			Util_log_add(log_num, result.string, result.code);

			for (int i = 0; i < line_num_of_ids; i++)
			{
				log_num = Util_log_save(DEF_LINE_WORKER_THREAD_STR, "Line_load_icon()...");
				result = Line_load_icon(i);
				Util_log_add(log_num, result.string, result.code);
				if(result.code == 0)
					line_icon_available[i] = true;
			}
		}
		else if (line_delete_id_request)
		{
			line_delete_id_request = false;
			log_num = Util_log_save(DEF_LINE_WORKER_THREAD_STR, "Util_file_delete_file()...");
			result = Util_file_delete_file(line_ids[line_selected_room_num], DEF_MAIN_DIR + "to/");
			Util_log_add(log_num, result.string, result.code);

			if (result.code == 0)
			{
				line_select_chat_room_request = true;

				log_num = Util_log_save(DEF_LINE_WORKER_THREAD_STR, "Util_file_delete_file()...");
				result = Util_file_delete_file(line_ids[line_selected_room_num].substr(0, 16), DEF_MAIN_DIR);
				Util_log_add(log_num, result.string, result.code);

				Line_reset_id();
				line_icon_dl_request = true;
			}
		}
		else if(line_dl_content_request)
		{
			cut_pos = line_url.find("&id=");
			if (!(cut_pos == std::string::npos))
			{
				filename = line_url.substr(cut_pos + 4);

				if (filename.length() > 33)
					filename = filename.substr(0, 33);
			}

			line_dled_content_size = 0;
			log_num = Util_log_save(DEF_LINE_WORKER_THREAD_STR, "Util_httpc_save_data()...");
			result = Util_httpc_save_data(line_url, 1024 * 256, &line_dled_content_size, true, 5, DEF_MAIN_DIR + "contents/", filename);
			Util_log_add(log_num, result.string, result.code);

			line_dl_content_request = false;
		}
		else
			Util_sleep(DEF_ACTIVE_THREAD_SLEEP_TIME);

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

	//IO error
	if(result.code != 0)
		return -1;

	//EOF
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
	u8* buffer = NULL;
	u32 dl_size = 0;
	u32 status_code = 0;
	u64 file_size = 0;
	size_t image_url_end_pos;
	size_t image_url_start_pos;
	size_t video_url_end_pos;
	size_t video_url_start_pos;
	size_t audio_url_end_pos;
	size_t audio_url_start_pos;
	size_t file_url_end_pos;
	size_t file_url_start_pos;
	size_t sticker_end_pos;
	size_t sticker_start_pos;
	size_t id_end_pos;
	size_t id_start_pos;
	size_t message_start_pos;
	size_t message_next_pos;
	int cut_length = 0;
	int log_num = 0;
	int index = 0;
	int room_num = 0;
	int audio_tracks = 0;
	int video_tracks = 0;
	int subtitle_tracks = 0;
	int check_length = 0;
	int text_start_pos = 0;
	bool failed = false;
	bool sticker_msg = false;
	bool send_file = false;
	double x_size = 0;
	double x_cache = 0;
	double y_cache = 0;
	std::string filename = "";
	std::string last_url = "";
	std::string content_cache = "";
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
	std::string id_start = "<id>";
	std::string id_end = "</id>";
	std::string message_start = "";
	std::string image_message = "image";
	std::string video_message = "video";
	std::string audio_message = "audio";
	std::string file_message = "file";
	std::string id_message = "";
	std::string send_data = "";
	std::string file_type = "";
	std::string encoded_data = "";
	std::string response_string = "";
	std::string one_string = "";
	std::string out_data[6];
	Result_with_string result;
	Line_upload_file_info upload_file_info;

	while (line_thread_run)
	{
		if (line_parse_log_request)
		{
			message_start_pos = std::string::npos;
			message_next_pos = std::string::npos;
			image_url_start_pos = std::string::npos;
			image_url_end_pos = std::string::npos;
			video_url_start_pos = std::string::npos;
			video_url_end_pos = std::string::npos;
			audio_url_start_pos = std::string::npos;
			audio_url_end_pos = std::string::npos;
			file_url_start_pos = std::string::npos;
			file_url_end_pos = std::string::npos;
			sticker_end_pos = std::string::npos;
			sticker_start_pos = std::string::npos;
			id_end_pos = std::string::npos;
			id_start_pos = std::string::npos;
			line_num_of_msg = 0;
			line_num_of_lines = 10;
			cut_length = 0;
			failed = false;

			image_message = line_msg[27];
			id_message = line_msg[28];
			audio_message = line_msg[53];
			video_message = line_msg[54];
			file_message = line_msg[55];

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
				video_url_start_pos = line_msg_log[i].find(video_url_start);
				video_url_end_pos = line_msg_log[i].find(video_url_end);
				audio_url_start_pos = line_msg_log[i].find(audio_url_start);
				audio_url_end_pos = line_msg_log[i].find(audio_url_end);
				file_url_start_pos = line_msg_log[i].find(file_url_start);
				file_url_end_pos = line_msg_log[i].find(file_url_end);
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
				else if (!(video_url_start_pos == std::string::npos || video_url_end_pos == std::string::npos))
				{
					line_content[line_num_of_lines + 1] = "<url>" + line_msg_log[i].substr((video_url_start_pos + video_url_start.length()), (video_url_end_pos - (video_url_start_pos + video_url_start.length()))) + "</url>";
					line_content[line_num_of_lines + 1] += "<type>video</type>";

					content_cache = line_msg_log[i].substr(0, video_url_start_pos);
					content_cache += video_message;
					content_cache += line_msg_log[i].substr(video_url_end_pos + video_url_end.length(), line_msg_log[i].length() - (video_url_end_pos + video_url_end.length()));
					line_msg_log[i] = content_cache;
				}
				else if (!(audio_url_start_pos == std::string::npos || audio_url_end_pos == std::string::npos))
				{
					line_content[line_num_of_lines + 1] = "<url>" + line_msg_log[i].substr((audio_url_start_pos + audio_url_start.length()), (audio_url_end_pos - (audio_url_start_pos + audio_url_start.length()))) + "</url>";
					line_content[line_num_of_lines + 1] += "<type>audio</type>";

					content_cache = line_msg_log[i].substr(0, audio_url_start_pos);
					content_cache += audio_message;
					content_cache += line_msg_log[i].substr(audio_url_end_pos + audio_url_end.length(), line_msg_log[i].length() - (audio_url_end_pos + audio_url_end.length()));
					line_msg_log[i] = content_cache;
				}
				else if (!(file_url_start_pos == std::string::npos || file_url_end_pos == std::string::npos))
				{
					line_content[line_num_of_lines + 1] = "<url>" + line_msg_log[i].substr((file_url_start_pos + file_url_start.length()), (file_url_end_pos - (file_url_start_pos + file_url_start.length()))) + "</url>";
					line_content[line_num_of_lines + 1] += "<type>file</type>";

					content_cache = line_msg_log[i].substr(0, file_url_start_pos);
					content_cache += file_message;
					content_cache += line_msg_log[i].substr(file_url_end_pos + file_url_end.length(), line_msg_log[i].length() - (file_url_end_pos + file_url_end.length()));
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

				cut_length = 0;
				text_start_pos = 0;

				//Wrap text
				while (true)
				{
					if (line_num_of_lines >= 59990)
					{
						failed = true;
						break;
					}

					//If we reach end of text, just copy it
					if(line_msg_log[i].length() < (uint)(text_start_pos + cut_length))
					{
						x_size = 0;
						line_num_of_lines++;
						line_short_msg_log[line_num_of_lines] = line_msg_log[i].substr(text_start_pos, cut_length);
						break;
					}
					else
					{
						check_length = mblen(&line_msg_log[i].c_str()[text_start_pos + cut_length], 4);
						if (check_length >= 1)
						{
							one_string = line_msg_log[i].substr(text_start_pos + cut_length, check_length);
							//If string is new line, go to next line
							if(one_string == "\n")
							{
								x_size = 0;
								cut_length += check_length;

								line_num_of_lines++;
								line_short_msg_log[line_num_of_lines] = line_msg_log[i].substr(text_start_pos, cut_length);
								if(line_short_msg_log[line_num_of_lines].find("\n") != std::string::npos)
									line_short_msg_log[line_num_of_lines].erase(line_short_msg_log[line_num_of_lines].find("\n"), 1);

								text_start_pos = text_start_pos + cut_length;
								cut_length = 0;
							}
							else
							{
								Draw_get_text_size(one_string, line_text_size, line_text_size, &x_cache, &y_cache);
								//If total text width is more than 300px, go to next line
								if(x_size + x_cache > 300)
								{
									x_size = 0;
									line_num_of_lines++;
									line_short_msg_log[line_num_of_lines] = line_msg_log[i].substr(text_start_pos, cut_length);

									text_start_pos = text_start_pos + cut_length;
									cut_length = check_length;
									x_size += x_cache;
								}
								else
								{
									x_size += x_cache;
									cut_length += check_length;
								}
							}
						}
						else
							cut_length++;
					}
				}

				if(sticker_msg)
					line_num_of_lines += 3;
				else
					line_num_of_lines++;

				line_short_msg_pos_end[i] = line_num_of_lines;
				line_max_y = (-line_text_interval * line_num_of_lines) + 100;
				line_text_y = line_max_y;
			}

			line_max_y = (-line_text_interval * line_num_of_lines) + 100;
			line_text_y = line_max_y;

			if (failed)
			{
				Util_err_set_error_message(DEF_ERR_OTHER_STR, "[Error] Too many messages ", DEF_LINE_LOG_THREAD_STR, DEF_ERR_OTHER);
				Util_err_set_error_show_flag(true);
			}
			else
			{
				log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_file_load_from_file()...");
				result = Util_file_load_from_file(line_ids[line_selected_room_num], DEF_MAIN_DIR + "to/", &buffer, 0x1000);
				Util_log_add(log_num , result.string, result.code);
				result = Util_parse_file((char*)buffer, 4, out_data);
				free(buffer);
				buffer = NULL;

				line_msg_offset[line_selected_room_num] += line_unread_msg_num[line_selected_room_num];
				line_unread_msg_num[line_selected_room_num] = 0;
				out_data[0] = "<0>" + out_data[0] + "</0>"; //user/group name
				out_data[1] = "<1>" + out_data[1] + "</1>"; //img url
				out_data[2] = "<2>" + std::to_string(line_msg_offset[line_selected_room_num]) + "</2>"; //msg offset
				out_data[3] = "<3>" + std::to_string(line_unread_msg_num[line_selected_room_num]) + "</3>"; //num of new msg

				log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_file_save_to_file()...");
				result = Util_file_save_to_file(line_ids[line_selected_room_num], DEF_MAIN_DIR + "to/", (u8*)(out_data[0] + out_data[1] + out_data[2] + out_data[3]).c_str(), (out_data[0] + out_data[1] + out_data[2] + out_data[3]).length(), true);
				Util_log_add(log_num, result.string, result.code);
			}

		if(line_num_of_msg <= line_selected_highlight_num)
			line_selected_highlight_num = 0;

		line_selected_search_highlight_num = 0;
		line_parse_log_request = false;
		}
		else if (line_send_request[0] || line_send_request[1] || line_send_request[2])
		{
			failed = false;
			line_send_success = false;
			line_sending_msg = true;
			dl_size = 0;
			file_size = 0;
			line_total_data_size = 0;
			line_sent_data_size = 0;
			line_uploaded_size = 0;
			file_type = "";
			room_num = line_selected_room_num;

			if (!failed)
			{
				if (line_send_request[0])
					send_data = "{ \"type\": \"send_text\",\"id\" : \"" + line_ids[line_selected_room_num] + "\",\"message\" : \"" + line_encoded_input_text + "\",\"auth\" : \"" + (line_script_auth == " " ? "" : line_script_auth) + "\",\"gas_ver\" : \"" + std::to_string(DEF_LINE_GAS_VER) + "\",\"logs\" : \"" + std::to_string(line_num_of_logs) + "\" }";
				else if (line_send_request[1])
				{
					if (line_selected_sticker_num >= 1 && line_selected_sticker_num <= 40)
						send_data = "{ \"type\": \"send_sticker\",\"id\" : \"" + line_ids[line_selected_room_num] + "\",\"package_id\" : \"11537\" ,\"sticker_id\" : \"" + std::to_string(line_sticker_num_list[line_selected_sticker_num]) + "\",\"auth\" : \"" + (line_script_auth == " " ? "" : line_script_auth) + "\",\"gas_ver\" : \"" + std::to_string(DEF_LINE_GAS_VER) + "\",\"logs\" : \"" + std::to_string(line_num_of_logs) + "\" }";
					else if (line_selected_sticker_num >= 41 && line_selected_sticker_num <= 80)
						send_data = "{ \"type\": \"send_sticker\",\"id\" : \"" + line_ids[line_selected_room_num] + "\",\"package_id\" : \"11538\" ,\"sticker_id\" : \"" + std::to_string(line_sticker_num_list[line_selected_sticker_num]) + "\",\"auth\" : \"" + (line_script_auth == " " ? "" : line_script_auth) + "\",\"gas_ver\" : \"" + std::to_string(DEF_LINE_GAS_VER) + "\",\"logs\" : \"" + std::to_string(line_num_of_logs) + "\" }";
					else if (line_selected_sticker_num >= 81 && line_selected_sticker_num <= 120)
						send_data = "{ \"type\": \"send_sticker\",\"id\" : \"" + line_ids[line_selected_room_num] + "\",\"package_id\" : \"11539\" ,\"sticker_id\" : \"" + std::to_string(line_sticker_num_list[line_selected_sticker_num]) + "\",\"auth\" : \"" + (line_script_auth == " " ? "" : line_script_auth) + "\",\"gas_ver\" : \"" + std::to_string(DEF_LINE_GAS_VER) + "\",\"logs\" : \"" + std::to_string(line_num_of_logs) + "\" }";
				}
				else if (line_send_request[2])
				{
					log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_file_check_file_size()...");
					result = Util_file_check_file_size(line_send_file_name, line_send_file_dir, &file_size);
					Util_log_add(log_num, result.string, result.code);

					result = Util_decoder_open_file(line_send_file_dir + line_send_file_name, &audio_tracks, &video_tracks, &subtitle_tracks, 2);

					if(result.code == 0)
					{
						if(video_tracks > 0)
							file_type = "video";
						else if(audio_tracks > 0)
							file_type = "audio";
					}

					Util_decoder_close_file(2);

					line_total_data_size = file_size * 1.34;
					send_file = true;

					upload_file_info.first = true;
					upload_file_info.end = false;
					upload_file_info.filename = line_send_file_name;
					upload_file_info.dir = line_send_file_dir;
					upload_file_info.first_data = "{ \"type\": \"upload_content\",\"id\" : \"" + line_ids[line_selected_room_num] + "\",\"count\" : \"0\",\"name\" : \"" + line_send_file_name + "\",\"content_data\" : \"";
					upload_file_info.end_data = "\",\"last\" : \"true\",\"auth\" : \"" + (line_script_auth == " " ? "" : line_script_auth) + "\",\"gas_ver\" : \"" + std::to_string(DEF_LINE_GAS_VER) + "\",\"logs\" : \"" + std::to_string(line_num_of_logs) + "\",\"data_type\" : \"" + file_type + "\" }";
					upload_file_info.offset = 0;
				}
			}

			if (!failed)
			{
				log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_curl_post_and_dl_data()...");
				if(send_file)
				{
					result = Util_curl_post_and_dl_data(line_main_url, Line_upload_file_callback, (void*)&upload_file_info, &buffer, 1024 * 1024 * 5, (int*)&dl_size, &line_uploaded_size, true, 5);
					send_file = false;
				}
				else
					result = Util_curl_post_and_dl_data(line_main_url, (u8*)send_data.c_str(), send_data.length(), &buffer, 1024 * 1024 * 5, (int*)&dl_size, &line_uploaded_size, true, 5);

				Util_log_add(log_num, result.string, result.code);
				if (result.code == 0)
				{
					line_sent_data_size = line_total_data_size;
					line_uploaded_size = 0;
					response_string = (char*)buffer;
					result = Util_parse_file(response_string, 5, out_data);
					if (result.code == 0 && out_data[4] == "Success")
					{
						line_send_success = true;
						line_input_text = "";
						line_log_data = out_data[0];
						line_names[room_num] = out_data[1];
						line_icon_url[room_num] = out_data[2];
						line_unread_msg_num[room_num] = std::stoi(out_data[3]) - line_msg_offset[room_num];
						out_data[1] = "<0>" + out_data[1] + "</0>"; //user/group name
						out_data[2] = "<1>" + out_data[2] + "</1>"; //img url
						out_data[3] = "<2>" + std::to_string(line_msg_offset[room_num]) + "</2>"; //msg offset
						out_data[5] = "<3>" + std::to_string(line_unread_msg_num[room_num]) + "</3>"; //num of new msg

						filename = line_ids[room_num].substr(0, 16);
						log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_file_save_to_file()...");
						result = Util_file_save_to_file(filename, DEF_MAIN_DIR, (u8*)out_data[0].c_str(), out_data[0].length(), true);
						Util_log_add(log_num, result.string, result.code);

						if (result.code != 0)
							failed = true;

						if (!failed)
						{
							log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_file_save_to_file()...");
							result = Util_file_save_to_file(line_ids[room_num], DEF_MAIN_DIR + "to/", (u8*)(out_data[1] + out_data[2] + out_data[3] + out_data[5]).c_str(), (out_data[1] + out_data[2] + out_data[3] + out_data[5]).length(), true);
							Util_log_add(log_num, result.string, result.code);
							if (result.code != 0)
								failed = true;
						}

						line_parse_log_request = true;

						if (result.code != 0)
						{
							Util_err_set_error_message(result.string, result.error_description, DEF_LINE_LOG_THREAD_STR, result.code);
							Util_err_set_error_show_flag(true);
						}

						if (!failed)
						{
							log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Line_load_icon()...");
							result = Line_load_icon(room_num);
							Util_log_add(log_num, result.string, result.code);
							if(result.code == 0)
								line_icon_available[room_num] = true;
						}
					}
					else
					{
						Util_err_set_error_message(DEF_ERR_GAS_RETURNED_NOT_SUCCESS_STR, response_string, DEF_LINE_LOG_THREAD_STR, DEF_ERR_GAS_RETURNED_NOT_SUCCESS);
						Util_err_set_error_show_flag(true);
					}
				}
				else
				{
					Util_err_set_error_message(result.string, result.error_description, DEF_LINE_LOG_THREAD_STR, result.code);
					Util_err_set_error_show_flag(true);
				}
				free(buffer);
				buffer = NULL;
			}

			response_string = "";
			response_string.reserve(1);
			send_data = "";
			send_data.reserve(1);
			encoded_data = "";
			encoded_data.reserve(1);
			for (int i = 0; i < 6; i++)
			{
				out_data[i] = "";
				out_data[i].reserve(10);
			}

			line_sending_msg = false;
			if (line_send_request[0])
				line_send_request[0] = false;
			else if (line_send_request[1])
				line_send_request[1] = false;
			else if (line_send_request[2])
				line_send_request[2] = false;
		}
		else if(line_dl_all_log_no_parse_request && line_ids[index] == "")
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
			if(line_dl_all_log_no_parse_request)
				line_selected_room_num = index;

			line_dl_log_failed[line_selected_room_num] = false;
			room_num = line_selected_room_num;

			log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_httpc_dl_data()...");
			std::string cache = line_main_url + "?id=" + line_ids[room_num] + "&logs=" + std::to_string(line_num_of_logs) + "&script_auth=" + (line_script_auth = " " ? "" : line_script_auth) + "&gas_ver=" + std::to_string(DEF_LINE_GAS_VER);
			result = Util_httpc_dl_data(cache, &buffer, 1024 * 1024 * 5, &dl_size, &status_code, true, 5);
			Util_log_add(log_num, result.string + std::to_string(dl_size / 1024) + "KB (" + std::to_string(dl_size) + "B) ", result.code);

			if (result.code == 0)
			{
				result = Util_parse_file((char*)buffer, 5, out_data);
				if(result.code == 0 && out_data[4] == "Success")
				{
					line_log_data = out_data[0];
					line_names[room_num] = out_data[1];
					line_icon_url[room_num] = out_data[2];
					line_unread_msg_num[room_num] = atoi(out_data[3].c_str()) - line_msg_offset[room_num];
					out_data[1] = "<0>" + out_data[1] + "</0>"; //user/group name
					out_data[2] = "<1>" + out_data[2] + "</1>"; //img url
					out_data[3] = "<2>" + std::to_string(line_msg_offset[room_num]) + "</2>"; //msg offset
					out_data[5] = "<3>" + std::to_string(line_unread_msg_num[room_num]) + "</3>"; //num of new msg

					filename = line_ids[room_num].substr(0, 16);
					log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_file_save_to_file()...");
					result = Util_file_save_to_file(filename, DEF_MAIN_DIR, (u8*)out_data[0].c_str(), out_data[0].length(), true);
					Util_log_add(log_num, result.string, result.code);

					if (result.code != 0)
						failed = true;

					if (!failed)
					{
						log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_file_save_to_file()...");
						result = Util_file_save_to_file(line_ids[room_num], DEF_MAIN_DIR + "to/", (u8*)(out_data[1] + out_data[2] + out_data[3] + out_data[5]).c_str(), (out_data[1] + out_data[2] + out_data[3] + out_data[5]).length(), true);
						Util_log_add(log_num, result.string, result.code);
						if (result.code != 0)
							failed = true;
					}

					if(!line_dl_log_no_parse_request && !line_dl_all_log_no_parse_request && line_unread_msg_num[room_num] > 0)
						line_parse_log_request = true;

					if (result.code != 0)
					{
						Util_err_set_error_message(result.string, result.error_description, DEF_LINE_LOG_THREAD_STR, result.code);
						Util_err_set_error_show_flag(true);
					}

					if (!failed)
					{
						log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Line_load_icon()...");
						result = Line_load_icon(room_num);
						Util_log_add(log_num, result.string, result.code);
						if(result.code == 0)
							line_icon_available[room_num] = true;
					}
				}
				else
				{
					out_data[0] = (char*)buffer;
					Util_err_set_error_message(DEF_ERR_GAS_RETURNED_NOT_SUCCESS_STR, out_data[0], DEF_LINE_LOG_THREAD_STR, DEF_ERR_GAS_RETURNED_NOT_SUCCESS);
					Util_err_set_error_show_flag(true);
					failed = true;
					line_auto_update = false;
				}
			}
			else
			{
				Util_err_set_error_message(result.string, result.error_description, DEF_LINE_LOG_THREAD_STR, result.code);
				Util_err_set_error_show_flag(true);
				failed = true;
				line_auto_update = false;
			}

			for (int i = 0; i < 6; i++)
			{
				out_data[i] = "";
				out_data[i].reserve(10);
			}

			if(failed)
				line_dl_log_failed[line_selected_room_num] = true;

			free(buffer);
			buffer = NULL;
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

			log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Util_httpc_dl_data()...");
			result = Util_httpc_dl_data(line_short_url, &buffer, 0x10000, &dl_size, true, 1, &last_url);
			Util_log_add(log_num, result.string, result.code);
			var_clipboard = last_url;
			line_check_main_url_request = true;

			if (result.code != 0)
			{
				Util_err_set_error_message(result.string, result.error_description, DEF_LINE_LOG_THREAD_STR, result.code);
				Util_err_set_error_show_flag(true);
			}

			free(buffer);
			buffer = NULL;
			line_solve_short_url_request = false;
		}
		else if (line_load_log_request)
		{
			log_num = Util_log_save(DEF_LINE_LOG_THREAD_STR, "Line_load_from_log_sd()...");
			result = Line_load_log_from_sd(line_ids[line_selected_room_num].substr(0, 16));
			Util_log_add(log_num, result.string, result.code);
			if (result.code != 0 && result.code != 0xC8804478 && result.code != 0xC92044FA)
			{
				Util_err_set_error_message(result.string, result.error_description, DEF_LINE_LOG_THREAD_STR, result.code);
				Util_err_set_error_show_flag(true);
			}
			line_load_log_request = false;
		}
		else
			Util_sleep(DEF_ACTIVE_THREAD_SLEEP_TIME);

		while (line_thread_suspend)
			Util_sleep(DEF_INACTIVE_THREAD_SLEEP_TIME);
	}
	Util_curl_exit();

	Util_log_save(DEF_LINE_LOG_THREAD_STR, "Thread exit.");
	threadExit(0);
}

void Line_init_thread(void* arg)
{
	Util_log_save(DEF_LINE_INIT_STR, "Thread started.");
	u8* fs_buffer = NULL;
	int log_num = 0;
	std::string data[4] = { "", "", "", "", };
	Result_with_string result;

	line_status = "Authing...";
	log_num = Util_log_save(DEF_LINE_INIT_STR, "Util_file_load_from_file()...");
	result = Util_file_load_from_file("auth", DEF_MAIN_DIR, &fs_buffer, 0x2000);
	Util_log_add(log_num, result.string, result.code);

	if (result.code == 0)
		line_auth_code = (char*)fs_buffer;
	else
	{
		line_set_password_request = true;
		while(line_set_password_request)
			Util_sleep(100000);

		line_auth_code == " ";
	}
	free(fs_buffer);
	fs_buffer = NULL;

	if (line_auth_code == " " || line_auth_code == "")
	{
		Util_log_save(DEF_LINE_INIT_STR, "Password is not set");
		line_auth_success = true;
	}
	else
	{
		line_type_password_request = true;
		while(line_type_password_request)
			Util_sleep(100000);
	}

	if (!line_auth_success)
	{
		Util_log_save(DEF_LINE_INIT_STR, "Auth failed, rebooting...");
		line_status += "\nAuth failed";
		APT_HardwareResetAsync();
		for(;;)
			Util_sleep(10000);
	}

	line_status += "\nLoading settings...";
	log_num = Util_log_save(DEF_LINE_INIT_STR, "Util_file_load_from_file()...");
	result = Util_file_load_from_file("script_auth", DEF_MAIN_DIR, &fs_buffer, 0x2000);
	Util_log_add(log_num, result.string, result.code);
	if (result.code == 0)
		line_script_auth = (char*)fs_buffer;
	else
		line_script_auth = "";

	free(fs_buffer);
	fs_buffer = NULL;

	log_num = Util_log_save(DEF_LINE_INIT_STR, "Util_file_load_from_file()...");
	result = Util_file_load_from_file("main_url.txt", DEF_MAIN_DIR, &fs_buffer, 0x2000);
	Util_log_add(log_num, result.string, result.code);
	if (result.code == 0)
		line_main_url = (char*)fs_buffer;
	else
		line_main_url = "";

	free(fs_buffer);
	fs_buffer = NULL;

	line_status += "\nInitializing variables...";
	Line_reset_id();
	Line_reset_msg();
	line_text_size = 0.66;
	line_text_interval = 35.0;
	line_num_of_logs = 150;
	line_hide_id = false;

	line_change_main_url_short_button.c2d = var_square_image[0];
	line_add_new_id_button.c2d = var_square_image[0];
	line_change_main_url_button.c2d = var_square_image[0];
	line_hide_id_button.c2d = var_square_image[0];
	line_change_app_password_button.c2d = var_square_image[0];
	line_change_gas_password_button.c2d = var_square_image[0];
	line_back_button.c2d = var_square_image[0];
	line_send_msg_button.c2d = var_square_image[0];
	line_send_sticker_button.c2d = var_square_image[0];
	line_send_file_button.c2d = var_square_image[0];
	line_send_success_button.c2d = var_square_image[0];
	line_dl_logs_button.c2d = var_square_image[0];
	line_auto_update_button.c2d = var_square_image[0];
	line_max_logs_bar.c2d = var_square_image[0];
	line_copy_button.c2d = var_square_image[0];
	line_copy_select_down_button.c2d = var_square_image[0];
	line_copy_select_up_button.c2d = var_square_image[0];
	line_increase_interval_button.c2d = var_square_image[0];
	line_decrease_interval_button.c2d = var_square_image[0];
	line_increase_size_button.c2d = var_square_image[0];
	line_decrease_size_button.c2d = var_square_image[0];
	line_search_button.c2d = var_square_image[0];
	line_search_select_down_button.c2d = var_square_image[0];
	line_search_select_up_button.c2d = var_square_image[0];
	line_delete_room_button.c2d = var_square_image[0];
	line_yes_button.c2d = var_square_image[0];
	line_no_button.c2d = var_square_image[0];
	line_view_image_button.c2d = var_square_image[0];
	line_edit_msg_button.c2d = var_square_image[0];
	for(int i = 0; i < 128; i++)
		line_room_button[i].c2d = var_square_image[0];
	for(int i = 0; i < 6; i++)
		line_menu_button[i].c2d = var_square_image[0];
	for(int i = 0; i < 16; i++)
		line_content_button[i].c2d = var_square_image[0];

	Util_add_watch(&line_change_main_url_short_button.selected);
	Util_add_watch(&line_add_new_id_button.selected);
	Util_add_watch(&line_change_main_url_button.selected);
	Util_add_watch(&line_hide_id_button.selected);
	Util_add_watch(&line_change_app_password_button.selected);
	Util_add_watch(&line_change_gas_password_button.selected);
	Util_add_watch(&line_back_button.selected);
	Util_add_watch(&line_send_msg_button.selected);
	Util_add_watch(&line_send_sticker_button.selected);
	Util_add_watch(&line_send_file_button.selected);
	Util_add_watch(&line_send_success_button.selected);
	Util_add_watch(&line_dl_logs_button.selected);
	Util_add_watch(&line_auto_update_button.selected);
	Util_add_watch(&line_max_logs_bar.selected);
	Util_add_watch(&line_copy_button.selected);
	Util_add_watch(&line_copy_select_down_button.selected);
	Util_add_watch(&line_copy_select_up_button.selected);
	Util_add_watch(&line_increase_interval_button.selected);
	Util_add_watch(&line_decrease_interval_button.selected);
	Util_add_watch(&line_increase_size_button.selected);
	Util_add_watch(&line_decrease_size_button.selected);
	Util_add_watch(&line_search_button.selected);
	Util_add_watch(&line_search_select_down_button.selected);
	Util_add_watch(&line_search_select_up_button.selected);
	Util_add_watch(&line_delete_room_button.selected);
	Util_add_watch(&line_yes_button.selected);
	Util_add_watch(&line_no_button.selected);
	Util_add_watch(&line_view_image_button.selected);
	Util_add_watch(&line_edit_msg_button.selected);
	for(int i = 0; i < 128; i++)
	{
		Util_add_watch(&line_room_button[i].selected);
		Util_add_watch(&line_room_update_button[i].selected);
	}
	for(int i = 0; i < 6; i++)
		Util_add_watch(&line_menu_button[i].selected);
	for(int i = 0; i < 16; i++)
		Util_add_watch(&line_content_button[i].selected);

	Util_add_watch(&line_num_of_logs);
	Util_add_watch(&line_selected_menu_mode);
	Util_add_watch(&line_text_x);
	Util_add_watch(&line_text_y);
	Util_add_watch(&line_uploaded_size);

	line_status += "\nLoading settings...";
	log_num = Util_log_save(DEF_LINE_INIT_STR, "Util_file_load_from_file()...");
	result = Util_file_load_from_file("Line_setting.txt", DEF_MAIN_DIR, &fs_buffer, 0x2000);
	Util_log_add(log_num, result.string, result.code);
	if (result.code == 0)
	{
		result = Util_parse_file((char*)fs_buffer, 4, data);
		if(result.code == 0)
		{
			line_text_size = atof(data[0].c_str());
			line_text_interval = atof(data[1].c_str());
			line_num_of_logs = atoi(data[2].c_str());
			line_hide_id = (data[3] == "1");
		}
	}
	free(fs_buffer);
	fs_buffer = NULL;

	if(line_text_size >= 3.0 || line_text_size <= 0.25)
		line_text_size = 0.66;
	if(line_text_interval >= 250 || line_text_interval <= 10)
		line_text_interval = 35;
	if(line_num_of_logs > 4000 || line_num_of_logs < 20)
		line_num_of_logs = 150;

	line_status += "\nStarting threads...";
	line_thread_run = true;

	line_worker_thread = threadCreate(Line_worker_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, -1, false);
	line_log_thread = threadCreate(Line_log_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, -1, false);

	line_icon_dl_request = true;

	line_status += "\nLoading textures...";
	line_sticket_texture_num = Draw_get_free_sheet_num();
	log_num = Util_log_save(DEF_LINE_INIT_STR, "Draw_load_texture()...");
	result = Draw_load_texture("romfs:/gfx/draw/line/stickers.t3x", line_sticket_texture_num, line_stickers_images, 0, 121);
	Util_log_add(log_num, result.string, result.code);

	line_update_texture_num = Draw_get_free_sheet_num();
	log_num = Util_log_save(DEF_LINE_INIT_STR, "Draw_load_texture()...");
	result = Draw_load_texture("romfs:/gfx/draw/line/update.t3x", line_update_texture_num, &line_room_update_button[0].c2d, 0, 1);
	Util_log_add(log_num, result.string, result.code);

	for(int i = 1; i < 128; i++)
		line_room_update_button[i].c2d = line_room_update_button[0].c2d;

	line_select_chat_room_request = true;

	line_already_init = true;

	Util_log_save(DEF_LINE_INIT_STR, "Thread exit.");
	threadExit(0);
}

void Line_exit_thread(void* arg)
{
	Util_log_save(DEF_LINE_EXIT_STR, "Thread started.");
	int log_num;
	std::string data = "<0>" + std::to_string(line_text_size) + "</0><1>" + std::to_string(line_text_interval) + "</1><2>" + std::to_string(line_num_of_logs) + "</2><3>" + std::to_string(line_hide_id) + "</3>";
	Result_with_string result;

	line_thread_suspend = false;
	line_thread_run = false;

	line_status = "Saving settings...";
	log_num = Util_log_save(DEF_LINE_EXIT_STR, "Util_file_save_to_file()...");
	result = Util_file_save_to_file("Line_setting.txt", DEF_MAIN_DIR, (u8*)data.c_str(), data.length(), true);
	Util_log_add(log_num, result.string, result.code);

	line_status += "\nExiting threads...";
	Util_log_save(DEF_LINE_EXIT_STR, "threadJoin()...", threadJoin(line_worker_thread, DEF_THREAD_WAIT_TIME));

	line_status += ".";
	Util_log_save(DEF_LINE_EXIT_STR, "threadJoin()...", threadJoin(line_worker_thread, DEF_THREAD_WAIT_TIME));

	line_status += "\nCleaning up...";
	threadFree(line_log_thread);
	threadFree(line_worker_thread);

	for (int i = 0; i < 128; i++)
	{
		Draw_texture_free(&line_icon[i]);
		line_room_update_button[i].c2d.tex = NULL;
	}
	for (int i = 0; i < 121; i++)
		line_stickers_images[i].tex = NULL;

	Draw_free_texture(line_sticket_texture_num);
	Draw_free_texture(line_update_texture_num);

	Util_remove_watch(&line_change_main_url_short_button.selected);
	Util_remove_watch(&line_add_new_id_button.selected);
	Util_remove_watch(&line_change_main_url_button.selected);
	Util_remove_watch(&line_hide_id_button.selected);
	Util_remove_watch(&line_change_app_password_button.selected);
	Util_remove_watch(&line_change_gas_password_button.selected);
	Util_remove_watch(&line_back_button.selected);
	Util_remove_watch(&line_send_msg_button.selected);
	Util_remove_watch(&line_send_sticker_button.selected);
	Util_remove_watch(&line_send_file_button.selected);
	Util_remove_watch(&line_send_success_button.selected);
	Util_remove_watch(&line_dl_logs_button.selected);
	Util_remove_watch(&line_auto_update_button.selected);
	Util_remove_watch(&line_max_logs_bar.selected);
	Util_remove_watch(&line_copy_button.selected);
	Util_remove_watch(&line_copy_select_down_button.selected);
	Util_remove_watch(&line_copy_select_up_button.selected);
	Util_remove_watch(&line_increase_interval_button.selected);
	Util_remove_watch(&line_decrease_interval_button.selected);
	Util_remove_watch(&line_increase_size_button.selected);
	Util_remove_watch(&line_decrease_size_button.selected);
	Util_remove_watch(&line_search_button.selected);
	Util_remove_watch(&line_search_select_down_button.selected);
	Util_remove_watch(&line_search_select_up_button.selected);
	Util_remove_watch(&line_delete_room_button.selected);
	Util_remove_watch(&line_yes_button.selected);
	Util_remove_watch(&line_no_button.selected);
	Util_remove_watch(&line_view_image_button.selected);
	Util_remove_watch(&line_edit_msg_button.selected);
	for(int i = 0; i < 128; i++)
	{
		Util_remove_watch(&line_room_button[i].selected);
		Util_remove_watch(&line_room_update_button[i].selected);
	}
	for(int i = 0; i < 6; i++)
		Util_remove_watch(&line_menu_button[i].selected);
	for(int i = 0; i < 16; i++)
		Util_remove_watch(&line_content_button[i].selected);

	Util_remove_watch(&line_num_of_logs);
	Util_remove_watch(&line_selected_menu_mode);
	Util_remove_watch(&line_text_x);
	Util_remove_watch(&line_text_y);
	Util_remove_watch(&line_uploaded_size);

	line_already_init = false;

	Util_log_save(DEF_LINE_EXIT_STR, "Thread exit.");
	threadExit(0);
}

void Line_init(bool draw)
{
	Util_log_save(DEF_LINE_INIT_STR, "Initializing...");
	int color = DEF_DRAW_BLACK;
	int back_color = DEF_DRAW_WHITE;
	int log_num = 0;
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
				Util_swkbd_init(SWKBD_TYPE_QWERTY, SWKBD_ANYTHING, 1, 512, "アプリパスワードを入力 / Type app password here.", "");
				Util_swkbd_launch(&input_string[0]);
				Util_swkbd_exit();

				Util_swkbd_init(SWKBD_TYPE_QWERTY, SWKBD_ANYTHING, 1, 512, "アプリパスワードを入力(再度) / Type app password here.(again)", "");
				Util_swkbd_launch(&input_string[1]);
				Util_swkbd_exit();

				if (input_string[0] == input_string[1])
				{
					if(input_string[0] == "")
						input_string[0] = " ";

					log_num = Util_log_save(DEF_LINE_INIT_STR, "Util_file_save_to_file()...");
					result = Util_file_save_to_file("auth", DEF_MAIN_DIR, (u8*)input_string[0].c_str(), input_string[0].length(), true);
					Util_log_add(log_num, result.string, result.code);
					line_set_password_request = false;
					break;
				}
			}
		}

		if(line_type_password_request)
		{
			for (int i = 0; i < 3; i++)
			{
				Util_swkbd_init(SWKBD_TYPE_QWERTY, SWKBD_ANYTHING, 1, 512, "アプリパスワードを入力 / Type app password here.", "");
				Util_swkbd_launch(&input_string[0]);
				Util_swkbd_exit();
				if (line_auth_code == input_string[0])
				{
					line_auth_success = true;
					Util_log_save(DEF_LINE_INIT_STR, "Password is correct");
					break;
				}
				else
					Util_log_save(DEF_LINE_INIT_STR, "Password is incorrect");
			}
			line_type_password_request = false;
		}
	}

	if(!(var_model == CFG_MODEL_N2DSXL || var_model == CFG_MODEL_N3DSXL || var_model == CFG_MODEL_N3DS) || !var_core_2_available)
		APT_SetAppCpuTimeLimit(10);

	Util_log_save(DEF_LINE_EXIT_STR, "threadJoin()...", threadJoin(line_init_thread, DEF_THREAD_WAIT_TIME));
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
	int max_length = 512;
	int num_of_words = 0;
	int log_num = 0;
	u8 dummy = 0;
	u32 feature = 0;
	double pos_x = 0;
	double pos_y = 0;
	double texture_size = 0;
	double scroll_bar_y_pos = (195.0 / (-line_max_y / -line_text_y));
	size_t sticker_num_start_pos = 0;
	size_t sticker_num_end_pos = 0;
	std::string hidden_id = "";
	std::string status = "";
	std::string init_text = "";
	std::string hint_text = "";
	std::string filename = "";
	std::string line_dic_first_spell[7];
	std::string line_dic_full_spell[7];
	std::string swkbd_data = "";
	SwkbdType type;
	SwkbdValidInput valid_input;
	Result_with_string result;
	Keyboard_button button = KEYBOARD_BUTTON_NONE;

	texture_size = line_text_size;

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

			if(line_dl_content_request)
				Draw(line_msg[56] + std::to_string(line_dled_content_size / 1024.0 / 1024.0).substr(0, 4) + "MB", 0, 20, 0.5, 0.5, color);

			if (!line_select_chat_room_request)
			{
				for (int i = 1; i <= 59999; i++)
				{
					color_cache = color;

					if (i > line_num_of_lines || line_text_y + line_text_interval * i >= 240)
						break;
					else if (line_text_y + line_text_interval * i <= -1000)
					{
						if ((line_text_y + line_text_interval * (i + 100)) <= -20)
							i += 100;
					}
					else if (line_text_y + line_text_interval * i <= 5)
					{
					}
					else
					{
						if (line_content[i].find("<type>sticker</type>") != std::string::npos)
						{
							sticker_num_start_pos = line_content[i].find("<num>");
							sticker_num_end_pos = line_content[i].find("</num>");;
							Draw_texture(line_stickers_images[atoi(line_content[i].substr((sticker_num_start_pos + 5), sticker_num_end_pos - (sticker_num_start_pos + 5)).c_str())], line_text_x, (line_text_y + line_text_interval * i), (texture_size * 120.0), (texture_size * 120.0));
						}
						else if (line_content[i].find("<type>image</type>") != std::string::npos || line_content[i].find("<type>video</type>") != std::string::npos
							|| line_content[i].find("<type>audio</type>") != std::string::npos || line_content[i].find("<type>id</type>") != std::string::npos || line_content[i].find("<type>file</type>") != std::string::npos)
						{
							Draw_texture(var_square_image[0], DEF_DRAW_WEAK_RED, line_text_x, (line_text_y + line_text_interval * i), 300, 20);
							if (line_content[i].find("<type>id</type>") != std::string::npos && (line_dl_log_request || line_parse_log_request || line_auto_update || line_load_log_request))
								color_cache = weak_color;
						}

						if((line_selected_menu_mode == DEF_LINE_MENU_SEARCH && i >= line_short_msg_pos_start[line_search_result_pos[line_selected_search_highlight_num]] && i <= line_short_msg_pos_end[line_search_result_pos[line_selected_search_highlight_num]])
							|| (line_selected_menu_mode == DEF_LINE_MENU_COPY && i >= line_short_msg_pos_start[line_selected_highlight_num] && i <= line_short_msg_pos_end[line_selected_highlight_num]))//Use purple, If msg is highlighted
							color_cache = 0xFFB000B0;

						Draw(line_short_msg_log[i], line_text_x, line_text_y + line_text_interval * i, line_text_size, line_text_size, color_cache);
					}
				}
			}
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

			if (line_select_chat_room_request)
			{
				if(line_dl_log_no_parse_request || line_dl_all_log_no_parse_request)
					color_cache = weak_color;
				else
					color_cache = color;

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

						Draw_texture(&line_room_button[i], (line_unread_msg_num[i] > 0 || line_room_button[i].selected) ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA,
						0, 0 + line_text_y + (i * 35), 230, 30);
						if(line_icon_available[i])
							Draw_texture(&line_icon[i], 0, 0 + line_text_y + (i * 35), 30, 30);

						Draw_texture(&line_room_update_button[i], line_room_update_button[i].selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_BLACK,
						230, 0 + line_text_y + (i * 35.0), 30, 30);
						Draw(line_names[i], 35, 0 + line_text_y + (i * 35), 0.5, 0.5, color_cache);
						Draw(hidden_id, 35, 20 + line_text_y + (i * 35), 0.325, 0.325, color_cache);
						if(line_unread_msg_num[i] > 0)
							Draw(std::to_string(line_unread_msg_num[i]), 200, 0 + line_text_y + (i * 35), 0.5, 0.5, DEF_DRAW_RED);
					}
				}

				Draw_texture(var_square_image[0], back_color, 0, 165, 320, 60);
				Draw_texture(var_square_image[0], DEF_DRAW_WEAK_YELLOW, 10, 165, 300, 60);

				if (line_solve_short_url_request || line_dl_log_no_parse_request || line_dl_all_log_no_parse_request)
					Draw(line_msg[42], 40, 170, 0.5, 0.5, color);
				else
				{
					//Change main URL (short) button
					Draw(line_msg[40], 20, 170, 0.375, 0.375, color, X_ALIGN_LEFT, Y_ALIGN_CENTER, 130, 15, BACKGROUND_ENTIRE_BOX,
					&line_change_main_url_short_button, line_change_main_url_short_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Add new ID button
					Draw(line_msg[18], 20, 190, 0.375, 0.375, color, X_ALIGN_LEFT, Y_ALIGN_CENTER, 130, 15, BACKGROUND_ENTIRE_BOX,
					&line_add_new_id_button, line_add_new_id_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Change main URL button
					Draw(line_msg[19], 20, 210, 0.375, 0.375, color, X_ALIGN_LEFT, Y_ALIGN_CENTER, 130, 15, BACKGROUND_ENTIRE_BOX,
					&line_change_main_url_button, line_change_main_url_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Hide ID button
					Draw(line_msg[41], 170, 170, 0.375, 0.375, color, X_ALIGN_LEFT, Y_ALIGN_CENTER, 130, 15, BACKGROUND_ENTIRE_BOX,
					&line_hide_id_button, line_hide_id_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Change app password button
					Draw(line_msg[22], 170, 190, 0.375, 0.375, color, X_ALIGN_LEFT, Y_ALIGN_CENTER, 130, 15, BACKGROUND_ENTIRE_BOX,
					&line_change_app_password_button, line_change_app_password_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Change script password button
					Draw(line_msg[23], 170, 210, 0.375, 0.375, color, X_ALIGN_LEFT, Y_ALIGN_CENTER, 130, 15, BACKGROUND_ENTIRE_BOX,
					&line_change_gas_password_button, line_change_gas_password_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
				}
			}
			else
			{
				for (int i = 1, content_index = 0; i <= 59999; i++)
				{
					color_cache = color;

					if (i > line_num_of_lines || (line_text_y + line_text_interval * i) - 240 >= 130)
						break;
					else if (line_text_y + line_text_interval * i <= -1000)
					{
						if ((line_text_y + line_text_interval * (i + 100)) <= 10)
							i += 100;
					}
					else if ((line_text_y + line_text_interval * i) - 240 <= -10)
					{
					}
					else
					{
						if (line_content[i].find("<type>sticker</type>") != std::string::npos)
						{
							sticker_num_start_pos = line_content[i].find("<num>");
							sticker_num_end_pos = line_content[i].find("</num>");;
							Draw_texture(line_stickers_images[atoi(line_content[i].substr((sticker_num_start_pos + 5), sticker_num_end_pos - (sticker_num_start_pos + 5)).c_str())], line_text_x - 40, (line_text_y + line_text_interval * i) - 240, (texture_size * 120), (texture_size * 120));
						}
						else if (line_content[i].find("<type>image</type>") != std::string::npos || line_content[i].find("<type>video</type>") != std::string::npos
							|| line_content[i].find("<type>audio</type>") != std::string::npos || line_content[i].find("<type>id</type>") != std::string::npos || line_content[i].find("<type>file</type>") != std::string::npos)
						{
							Draw_texture(&line_content_button[content_index], line_content_button[content_index].selected ? DEF_DRAW_RED : DEF_DRAW_WEAK_RED,
							line_text_x - 40, (line_text_y + line_text_interval * i) - 240, 300, 20);
							content_index++;
							if (line_content[i].find("<type>id</type>") != std::string::npos && (line_dl_log_request || line_parse_log_request || line_auto_update || line_load_log_request))
								color_cache = weak_color;
						}

						if((line_selected_menu_mode == DEF_LINE_MENU_SEARCH && i >= line_short_msg_pos_start[line_search_result_pos[line_selected_search_highlight_num]] && i <= line_short_msg_pos_end[line_search_result_pos[line_selected_search_highlight_num]])
							|| (line_selected_menu_mode == DEF_LINE_MENU_COPY && i >= line_short_msg_pos_start[line_selected_highlight_num] && i <= line_short_msg_pos_end[line_selected_highlight_num]))//Use purple, If msg is highlighted
							color_cache = 0xFFB000B0;

						Draw(line_short_msg_log[i], line_text_x - 40, (line_text_y + line_text_interval * i) - 240, line_text_size, line_text_size, color_cache);
					}
				}
				Draw_texture(var_square_image[0], back_color, 0, 135, 320, 60);

				if (line_hide_id && line_ids[line_selected_room_num].length() >= 5)
				{
					hidden_id = line_ids[line_selected_room_num].substr(0, 5);

					for (size_t i = 6; i <= line_ids[line_selected_room_num].length(); i++)
						hidden_id += "*";
				}
				else
					hidden_id = line_ids[line_selected_room_num];

				status = "ID = " + hidden_id + "\n" + std::to_string(line_num_of_msg) + line_msg[0] + std::to_string(line_num_of_lines) + line_msg[1];

				Draw_texture(&line_icon[line_selected_room_num], 10, 135, 32, 32);
				Draw(status, 45, 135, 0.35, 0.35, color);
				Draw(line_names[line_selected_room_num], 45, 155, 0.475, 0.475, color);

				if((line_dl_log_request || line_parse_log_request || line_auto_update || line_load_log_request
					|| line_send_request[0] || line_send_request[1] || line_send_request[2]))
					color_cache = weak_color;

				//Back button
				Draw(line_msg[43], 260, 135, 0.45, 0.45, color_cache, X_ALIGN_CENTER, Y_ALIGN_CENTER, 40, 23,
				BACKGROUND_ENTIRE_BOX, &line_back_button, line_back_button.selected ? DEF_DRAW_YELLOW : DEF_DRAW_WEAK_YELLOW);

				Draw(DEF_LINE_VER, 260, 155, 0.45, 0.45, DEF_DRAW_GREEN);
				Draw_texture(var_square_image[0], DEF_DRAW_WEAK_AQUA, 10, 170, 300, 60);

				//Menu buttons
				Draw(line_msg[2], 10, 170, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 50, 10, BACKGROUND_ENTIRE_BOX,
				&line_menu_button[0], (line_menu_button[0].selected || line_selected_menu_mode == DEF_LINE_MENU_SEND) ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_YELLOW);

				Draw(line_msg[35], 60, 170, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 50, 10, BACKGROUND_ENTIRE_BOX,
				&line_menu_button[1], (line_menu_button[1].selected || line_selected_menu_mode == DEF_LINE_MENU_RECEIVE) ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_RED);

				Draw(line_msg[3], 110, 170, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 50, 10, BACKGROUND_ENTIRE_BOX,
				&line_menu_button[2], (line_menu_button[2].selected || line_selected_menu_mode == DEF_LINE_MENU_COPY) ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_YELLOW);

				Draw(line_msg[4], 160, 170, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 50, 10, BACKGROUND_ENTIRE_BOX,
				&line_menu_button[3], (line_menu_button[3].selected || line_selected_menu_mode == DEF_LINE_MENU_SETTINGS) ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_RED);

				Draw(line_msg[51], 210, 170, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 50, 10, BACKGROUND_ENTIRE_BOX,
				&line_menu_button[4], (line_menu_button[4].selected || line_selected_menu_mode == DEF_LINE_MENU_SEARCH) ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_YELLOW);

				Draw(line_msg[5], 260, 170, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 50, 10, BACKGROUND_ENTIRE_BOX,
				&line_menu_button[5], (line_menu_button[5].selected || line_selected_menu_mode == DEF_LINE_MENU_ADVANCED) ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_RED);

				if (line_selected_menu_mode == DEF_LINE_MENU_SEND && line_sending_msg)
				{
					if (line_send_request[2])
					{
						if(line_total_data_size != 0)
							Draw_texture(var_square_image[0], DEF_DRAW_YELLOW, 20, 205, (280.0 / line_total_data_size) * (line_sent_data_size + line_uploaded_size), 13);

						Draw(std::to_string((line_sent_data_size + line_uploaded_size) / 1024.0 / 1024.0).substr(0, 4) + "/" + std::to_string(line_total_data_size / 1024.0 / 1024.0).substr(0, 4) + "MB", 120, 205, 0.45, 0.45, color);
					}

					if (line_send_request[0])
						Draw(line_msg[36], 22.5, 185, 0.45, 0.45, color);
					else if (line_send_request[1])
						Draw(line_msg[37], 22.5, 185, 0.45, 0.45, color);
					else if (line_send_request[2])
						Draw(line_msg[38], 22.5, 185, 0.45, 0.45, color);
				}
				else if (line_selected_menu_mode == DEF_LINE_MENU_SEND && line_send_success)
				{
					//Send success button
					Draw(line_msg[39], 20, 185, 0.45, 0.45, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 280, 13,
					BACKGROUND_ENTIRE_BOX, &line_send_success_button, line_send_success_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
				}
				else if (line_selected_menu_mode == DEF_LINE_MENU_SEND)
				{
					//Send a message button
					Draw(line_msg[6], 20, 185, 0.45, 0.45, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 280, 13,
					BACKGROUND_ENTIRE_BOX, &line_send_msg_button, line_send_msg_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Send a sticker button
					Draw(line_msg[24], 20, 205, 0.45, 0.45, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 130, 13,
					BACKGROUND_ENTIRE_BOX, &line_send_sticker_button, line_send_sticker_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Send a file button
					Draw(line_msg[34], 170, 205, 0.45, 0.45, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 130, 13,
					BACKGROUND_ENTIRE_BOX, &line_send_file_button, line_send_file_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
				}
				else if (line_selected_menu_mode == DEF_LINE_MENU_RECEIVE)
				{
					if(line_dl_log_request || line_auto_update || line_sending_msg)
						color_cache = weak_color;
					else
						color_cache = color;

					//Dl logs button
					Draw(line_msg[7], 20, 185, 0.45, 0.45, color_cache, X_ALIGN_CENTER, Y_ALIGN_CENTER, 130, 13,
					BACKGROUND_ENTIRE_BOX, &line_dl_logs_button, line_dl_logs_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Auto update button
					Draw(line_msg[8 + line_auto_update], 170, 185, 0.45, 0.45, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 130, 13,
					BACKGROUND_ENTIRE_BOX, &line_auto_update_button, line_auto_update_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Max logs bar
					Draw_texture(var_square_image[0], DEF_DRAW_WEAK_AQUA, 100, 210, 200, 5);
					Draw_texture(&line_max_logs_bar, line_max_logs_bar.selected ? DEF_DRAW_RED : DEF_DRAW_WEAK_RED, (line_num_of_logs / 20) + 99, 205, 7, 13);
					Draw(line_msg[50] + std::to_string(line_num_of_logs), 20, 205, 0.35, 0.35, color_cache);
				}
				else if (line_selected_menu_mode == DEF_LINE_MENU_COPY)
				{
					Draw(std::to_string(line_selected_highlight_num) + "/" + std::to_string(line_num_of_msg - 1), 20, 190, 0.5, 0.5, color);

					//Copy button
					Draw(line_msg[11], 90, 185, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 30,
					BACKGROUND_ENTIRE_BOX, &line_copy_button, line_copy_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Select down button
					Draw(line_msg[12], 160, 185, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 30,
					BACKGROUND_ENTIRE_BOX, &line_copy_select_down_button, line_copy_select_down_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Select up button
					Draw(line_msg[13], 230, 185, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 30,
					BACKGROUND_ENTIRE_BOX, &line_copy_select_up_button, line_copy_select_up_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
				}
				else if (line_selected_menu_mode == DEF_LINE_MENU_SETTINGS)
				{
					//Increase font interval button
					Draw(line_msg[14], 20, 185, 0.375, 0.375, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 30,
					BACKGROUND_ENTIRE_BOX, &line_increase_interval_button, line_increase_interval_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Decrease font interval button
					Draw(line_msg[15], 90, 185, 0.375, 0.375, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 30,
					BACKGROUND_ENTIRE_BOX, &line_decrease_interval_button, line_decrease_interval_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Decrease font size button
					Draw(line_msg[16], 170, 185, 0.375, 0.375, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 30,
					BACKGROUND_ENTIRE_BOX, &line_decrease_size_button, line_decrease_size_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Increase font size button
					Draw(line_msg[17], 240, 185, 0.375, 0.375, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 30,
					BACKGROUND_ENTIRE_BOX, &line_increase_size_button, line_increase_size_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
				}
				else if (line_selected_menu_mode == DEF_LINE_MENU_SEARCH)
				{
					Draw(std::to_string(line_selected_search_highlight_num) + "/" + std::to_string(line_found), 20, 190, 0.5, 0.5, color);

					//Search button
					Draw(line_msg[52], 90, 185, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 30,
					BACKGROUND_ENTIRE_BOX, &line_search_button, line_search_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Select down button
					Draw(line_msg[12], 160, 185, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 30,
					BACKGROUND_ENTIRE_BOX, &line_search_select_down_button, line_search_select_down_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//Select up button
					Draw(line_msg[13], 230, 185, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 30,
					BACKGROUND_ENTIRE_BOX, &line_search_select_up_button, line_search_select_up_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
				}
				else if (line_selected_menu_mode == DEF_LINE_MENU_ADVANCED)
				{
					//Delete room button
					Draw(line_msg[44], 20, 185, 0.45, 0.45, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 280, 30,
					BACKGROUND_ENTIRE_BOX, &line_delete_room_button, line_delete_room_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
				}

				if (line_send_check[0] || line_send_check[1] || line_send_check[2] || line_delete_id_check_request)
				{
					if ((line_dl_log_request || line_parse_log_request || line_auto_update || line_load_log_request) && line_delete_id_check_request)
						color_cache = weak_color;
					else
						color_cache = color;

					Draw_texture(var_square_image[0], DEF_DRAW_BLUE, 10, 110, 300, 110);

					//Yes button
					Draw(line_msg[20], 30, 200, 0.45, 0.45, color_cache, X_ALIGN_CENTER, Y_ALIGN_CENTER, 70, 15,
					BACKGROUND_ENTIRE_BOX, &line_yes_button, line_yes_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					//No button
					Draw(line_msg[21], 120, 200, 0.45, 0.45, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 70, 15,
					BACKGROUND_ENTIRE_BOX, &line_no_button, line_no_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

					if (line_send_check[0])
					{
						//Edit msg button
						Draw(line_msg[47], 210, 200, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 70, 15,
						BACKGROUND_ENTIRE_BOX, &line_edit_msg_button, line_edit_msg_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
					}
					else if (line_send_check[2])
					{
						//View image button
						Draw(line_msg[32], 210, 200, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 70, 15,
						BACKGROUND_ENTIRE_BOX, &line_view_image_button, line_view_image_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
					}

					Draw_texture(&line_icon[line_selected_room_num], 10, 110, 30, 30);
					Draw(line_names[line_selected_room_num], 45, 110, 0.475, 0.475, DEF_DRAW_RED);
					if (line_send_check[0])
					{
						Draw(line_msg[25], 45, 120, 0.55, 0.55, color);
						Draw(line_input_text, 45, 135, 0.45, 0.45, color);
					}
					else if (line_send_check[1])
					{
						Draw(line_msg[26], 45, 120, 0.55, 0.55, color);
						Draw_texture(line_stickers_images[line_selected_sticker_num], 45, 135, 60, 60);
					}
					else if (line_send_check[2])
					{
						Draw(line_msg[29], 45, 120, 0.55, 0.55, color);
						Draw(line_msg[30] + line_send_file_dir, 45, 140, 0.4, 0.4, color);
						//Draw(line_msg[48] + std::to_string(Expl_query_size((int)Expl_query_selected_num(EXPL_SELECTED_FILE_NUM) + (int)Expl_query_view_offset_y()) / 1024.0 / 1024.0).substr(0, 5) + line_msg[49], 0, 45, 150, 0.45, 0.45, 1, 1, 1, 0.75);
						Draw(line_msg[31] + line_send_file_name, 45, 160, 0.45, 0.45, color);
					}
					else if (line_delete_id_check_request)
					{
						Draw(line_msg[45], 45, 120, 0.55, 0.55, color);
						Draw(line_msg[46], 45, 130, 0.4, 0.4, color);
					}
				}
				else if (line_select_sticker_request)
				{
					Draw_texture(var_square_image[0], DEF_DRAW_AQUA, 10, 140, 300, 90);
					Draw_texture(var_square_image[0], DEF_DRAW_WEAK_YELLOW, 290, 120, 20, 20);
					Draw("X", 292.5, 120, 0.7, 0.7, DEF_DRAW_RED);

					for (int i = 0; i < 5; i++)
					{
						Draw_texture(var_square_image[0], DEF_DRAW_WEAK_YELLOW, 10 + (i * 60), 140, 30, 10);
						Draw_texture(var_square_image[0], DEF_DRAW_WEAK_RED, 40 + (i * 60), 140, 30, 10);
					}
					Draw_texture(var_square_image[0], DEF_DRAW_BLUE, 10 + (line_selected_sticker_tab_num * 30), 140, 30, 10);

					pos_x = 20;
					pos_y = 150;
					for (int i = 1 + (line_selected_sticker_tab_num * 12); i < 13 + (line_selected_sticker_tab_num * 12); i++)
					{
						Draw_texture(var_square_image[0], DEF_DRAW_WEAK_BLUE, pos_x, pos_y, 30, 30);
						Draw_texture(line_stickers_images[i], pos_x, pos_y, 30, 30);
						pos_x += 50;
						if (pos_x > 271)
						{
							pos_x = 20;
							pos_y += 40;
						}
					}
				}
			}

			Draw_texture(var_square_image[0], back_color, 312.5, 0, 7.5, 15);
			Draw_texture(var_square_image[0], back_color, 312.5, 215, 7.5, 10);

			if (scroll_bar_y_pos <= 0.0)
				Draw_texture(var_square_image[0], DEF_DRAW_RED, 312.5, 15, 7.5, 5);
			else if (scroll_bar_y_pos >= 195.0)
				Draw_texture(var_square_image[0], DEF_DRAW_RED, 312.5, 210, 7.5, 5);
			else
				Draw_texture(var_square_image[0], DEF_DRAW_BLUE, 312.5, 15 + scroll_bar_y_pos, 7.5, 5);

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

	if (line_type_msg_request || line_type_id_request || line_type_short_url_request || line_type_main_url_request
		|| line_check_main_url_request || line_type_app_ps_request || line_type_script_ps_request || line_search_request)
	{
		init_text = var_clipboard;
		max_length = 512;
		type = SWKBD_TYPE_NORMAL;
		valid_input = SWKBD_NOTEMPTY_NOTBLANK;
		feature = 0;

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
			feature = SWKBD_MULTILINE | SWKBD_PREDICTIVE_INPUT;
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
			feature = SWKBD_PREDICTIVE_INPUT;
		}
		else if(line_type_main_url_request || line_check_main_url_request)
		{
			line_dic_full_spell[0] = "https://script.google.com/macros/s/";
			line_dic_first_spell[0] = "h";

			num_of_words = 1;
			hint_text = "URLを入力 / Type your url here.";
			if(line_type_main_url_request)
				init_text = line_main_url;

			feature = SWKBD_PREDICTIVE_INPUT;
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
		else if(line_search_request)
		{
			num_of_words = 0;
			hint_text = "検索 単語/文 を入力 / Type search word/message(s) here.";
			init_text = line_search_text;
			feature = SWKBD_PREDICTIVE_INPUT;
		}

		Util_swkbd_init(type, valid_input, 2, max_length, hint_text, init_text, feature);
		if(num_of_words != 0)
			Util_swkbd_set_dic_word(line_dic_first_spell, line_dic_full_spell, num_of_words);

		Util_swkbd_launch(&swkbd_data, &button);
		Util_swkbd_exit();
		if (button == KEYBOARD_BUTTON_NONE)
		{
			log_num = 0;
			if (line_type_msg_request)
			{
				line_input_text = swkbd_data;
				line_encoded_input_text = Util_encode_to_escape(swkbd_data);
				if (line_encoded_input_text.length() > 4000)
					line_encoded_input_text = line_encoded_input_text.substr(0, 3990);

				line_send_check[0] = true;
			}
			else if(line_type_id_request)
			{
				log_num = Util_log_save(DEF_LINE_MAIN_STR, "Util_file_save_to_file()...");
				result = Util_file_save_to_file(swkbd_data, DEF_MAIN_DIR + "to/", &dummy, 1, true);
				Util_log_add(log_num, result.string, result.code);
			}
			else if(line_type_short_url_request)
			{
				line_short_url = swkbd_data;
				line_solve_short_url_request = true;
			}
			else if(line_type_main_url_request || line_check_main_url_request || line_type_app_ps_request || line_type_script_ps_request)
			{
				if(line_type_main_url_request || line_check_main_url_request)
				{
					filename = "main_url.txt";
					line_main_url = swkbd_data;
				}
				else if(line_type_app_ps_request)
				{
					if(swkbd_data == "")
						swkbd_data = " ";

					filename = "auth";
				}
				else if(line_type_script_ps_request)
				{
					if(swkbd_data == "")
						swkbd_data = " ";

					filename = "script_auth";
					line_script_auth = swkbd_data;
				}

				log_num = Util_log_save(DEF_LINE_MAIN_STR, "Util_file_save_to_file()...");
				result = Util_file_save_to_file(filename, DEF_MAIN_DIR, (u8*)swkbd_data.c_str(), swkbd_data.length(), true);
				Util_log_add(log_num, result.string, result.code);
			}
			else if(line_search_request)
			{
				line_search_text = swkbd_data;
				line_selected_search_highlight_num = 0;
				line_found = 0;
				for(int i = 0; i < 4000; i++)
					line_search_result_pos[i] = 0;

				for(int i = 0; i < 4000; i++)
				{
					if(line_msg_log[i].find(line_search_text) != std::string::npos)
					{
						line_search_result_pos[line_found] = i;
						line_found++;
					}
				}
				if(line_found > 0)
				{
					line_found--;
					line_selected_search_highlight_num = line_found;
					line_text_y = line_msg_pos[line_search_result_pos[line_found]];
				}
			}
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
		else if(line_search_request)
			line_search_request = false;
	}
}
