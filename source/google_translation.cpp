#include <3ds.h>
#include <string>
#include <unistd.h>

#include "hid.hpp"
#include "draw.hpp"
#include "httpc.hpp"
#include "google_translation.hpp"
#include "setting_menu.hpp"
#include "error.hpp"
#include "menu.hpp"
#include "log.hpp"
#include "types.hpp"
#include "swkbd.hpp"

/*For draw*/
bool gtr_need_reflesh = false;
bool gtr_pre_select_sorce_lang_request = false;
bool gtr_pre_select_target_lang_request = false;
int gtr_pre_current_history_num = 9;
int gtr_pre_selected_history_num = 0;
double gtr_pre_selected_sorce_lang_num = 0;
double gtr_pre_selected_target_lang_num = 0;
double gtr_pre_sorce_lang_offset = 0;
double gtr_pre_target_lang_offset = 0;
double gtr_pre_text_pos_x = 0.0;
std::string gtr_pre_sorce_lang = "en";
std::string gtr_pre_target_lang = "ja";
std::string gtr_pre_input_text = "n/a";
/*---------------------------------------------*/

bool gtr_already_init = false;
bool gtr_main_run = false;
bool gtr_tr_thread_run = false;
bool gtr_thread_suspend = false;
bool gtr_tr_request = false;
bool gtr_select_sorce_lang_request = false;
bool gtr_select_target_lang_request = false;
int gtr_current_history_num = 9;
int gtr_selected_history_num = 0;
double gtr_selected_sorce_lang_num = 0;
double gtr_selected_target_lang_num = 0;
double gtr_sorce_lang_offset = 0;
double gtr_target_lang_offset = 0;
double gtr_text_pos_x = 0.0;
std::string gtr_sorce_lang = "en";
std::string gtr_target_lang = "ja";
std::string gtr_input_text = "n/a";
std::string gtr_history[10] = { "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", };
std::string gtr_lang_short_list[GTR_NUM_OF_LANG_SHORT_LIST_MSG];
std::string gtr_lang_list[GTR_NUM_OF_LANG_SHORT_LIST_MSG];
std::string gtr_msg[GTR_NUM_OF_MSG];
std::string gtr_tr_thread_string = "Gtr/Tr thread";
std::string gtr_init_string = "Gtr/Init";
std::string gtr_exit_string = "Gtr/Exit";
std::string gtr_ver = "v1.1.2";

Thread gtr_tr_thread;

bool Gtr_query_init_flag(void)
{
	return gtr_already_init;
}

bool Gtr_query_running_flag(void)
{
	return gtr_main_run;
}

void Gtr_set_msg(int msg_num, int msg_type, std::string msg)
{
	if (msg_type == GTR_SHORT_LANG_LIST && msg_num >= 0 && msg_num < GTR_NUM_OF_LANG_SHORT_LIST_MSG)
	    gtr_lang_short_list[msg_num] = msg;
	else if (msg_type == GTR_LANG_LIST && msg_num >= 0 && msg_num < GTR_NUM_OF_LANG_LIST_MSG)
		gtr_lang_list[msg_num] = msg;
	else if (msg_type == GTR_MSG && msg_num >= 0 && msg_num < GTR_NUM_OF_MSG)
		gtr_msg[msg_num] = msg;
}

void Gtr_suspend(void)
{
	gtr_thread_suspend = true;
	gtr_main_run = false;
	Menu_resume();
}

void Gtr_resume(void)
{
	Menu_suspend();
	gtr_thread_suspend = false;
	gtr_main_run = true;
	gtr_need_reflesh = true;
}

std::string Gtr_get_lang_name(std::string short_name)
{
	int num_of_lang = 105;

	for (int i = 0; i < num_of_lang; i++)
	{
		if (gtr_lang_short_list[i] == short_name)
			return gtr_lang_list[i];
	}
	return "Unknown";
}

void Gtr_tr_thread(void* arg)
{
	Log_log_save(gtr_tr_thread_string, "Thread started.", 1234567890, false);

	u8* httpc_buffer;
	u32 dl_size;
	u32 status_code;
	int log_num = 0;
	std::string send_data;
	std::string url = "https://script.google.com/macros/s/AKfycbwbL6swXIeycS-WpVBrfKLh40bXg2CAv1PdAzuIhalqq2SGrJM/exec";
	Result_with_string result;

	httpc_buffer = (u8*)malloc(0x10000);

	while (gtr_tr_thread_run)
	{
		if (gtr_tr_request)
		{
			if (gtr_current_history_num + 1 > 9)
				gtr_current_history_num = 0;
			else
				gtr_current_history_num++;

			gtr_history[gtr_current_history_num] = gtr_msg[3];
			gtr_input_text = Sem_encode_to_escape(gtr_input_text);
			send_data = "{ \"text\": \"" + gtr_input_text + "\",\"sorce\" : \"" + gtr_sorce_lang + "\",\"target\" : \"" + gtr_target_lang + "\" }";

			memset(httpc_buffer, 0x0, 0x10000);
			log_num = Log_log_save(gtr_tr_thread_string, "Httpc_post_and_dl_data()...", 1234567890, false);
			result = Httpc_post_and_dl_data(url, (char*)send_data.c_str(), send_data.length(), httpc_buffer, 0x10000, &dl_size, &status_code, true, GTR_HTTP_POST_PORT0);
			Log_log_add(log_num, result.string, result.code, false);

			if (result.code == 0)
				gtr_history[gtr_current_history_num] = (char*)httpc_buffer;
			else
			{
				gtr_history[gtr_current_history_num] = "***Translation failed.***";
				Err_set_error_message(result.string, result.error_description, "Gtr/Tr thread/httpc", result.code);
				Err_set_error_show_flag(true);
			}

			gtr_tr_request = false;
		}
		else
			usleep(ACTIW_THREAD_SLEEP_TIME);

		while (gtr_thread_suspend)
			usleep(INACTIW_THREAD_SLEEP_TIME);
	}
	Log_log_save(gtr_tr_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Gtr_init(void)
{
	Log_log_save(gtr_init_string, "Initializing...", 1234567890, FORCE_DEBUG);

	gtr_current_history_num = 9;
	gtr_text_pos_x = 0.0;

	Draw_progress("0/0 [Gtr] Starting threads...");
	gtr_tr_thread_run = true;
	gtr_tr_thread = threadCreate(Gtr_tr_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, 0, false);

	Gtr_resume();
	gtr_already_init = true;
	Log_log_save(gtr_init_string, "Initialized", 1234567890, FORCE_DEBUG);
}

void Gtr_exit(void)
{
	Log_log_save(gtr_exit_string, "Exiting...", 1234567890, FORCE_DEBUG);
	u64 time_out = 10000000000;
	int log_num;
	bool failed = false;
	Result_with_string result;

	Draw_progress("[Gtr] Exiting...");
	gtr_already_init = false;
	gtr_tr_thread_run = false;
	gtr_thread_suspend = false;

	log_num = Log_log_save(gtr_exit_string, "Exiting thread...", 1234567890, FORCE_DEBUG);
	result.code = threadJoin(gtr_tr_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, Err_query_template_summary(0), result.code, FORCE_DEBUG);
	else
	{
		failed = true;
		Log_log_add(log_num, Err_query_template_summary(-1024), result.code, FORCE_DEBUG);
	}

	threadFree(gtr_tr_thread);

	gtr_input_text = "n/a";
	gtr_input_text.reserve(10);

	for (int i = 0; i < 10; i++)
	{
		gtr_history[i] = "n/a";
		gtr_history[i].reserve(10);
	}

	if (failed)
		Log_log_save(gtr_exit_string, "[Warn] Some function returned error.", 1234567890, FORCE_DEBUG);

	Log_log_save(gtr_exit_string, "Exited.", 1234567890, FORCE_DEBUG);
}

void Gtr_main(void)
{
	float red;
	float green;
	float blue;
	float alpha;
	std::string swkbd_data;
	Hid_info key;

	if(gtr_pre_sorce_lang != gtr_sorce_lang || gtr_pre_target_lang != gtr_target_lang || gtr_pre_current_history_num != gtr_current_history_num
	|| gtr_pre_text_pos_x != gtr_text_pos_x || gtr_pre_input_text != gtr_input_text || gtr_pre_selected_history_num != gtr_selected_history_num
	|| gtr_pre_select_target_lang_request != gtr_select_target_lang_request || gtr_pre_select_sorce_lang_request != gtr_select_sorce_lang_request
	|| gtr_pre_selected_sorce_lang_num != gtr_selected_sorce_lang_num || gtr_pre_selected_target_lang_num != gtr_selected_target_lang_num
	|| gtr_pre_sorce_lang_offset != gtr_sorce_lang_offset || gtr_pre_target_lang_offset != gtr_target_lang_offset)
	{
		gtr_pre_sorce_lang = gtr_sorce_lang;
		gtr_pre_target_lang = gtr_target_lang;
		gtr_pre_current_history_num = gtr_current_history_num;
		gtr_pre_text_pos_x = gtr_text_pos_x;
		gtr_pre_input_text = gtr_input_text;
		gtr_pre_selected_history_num = gtr_selected_history_num;
		gtr_pre_select_target_lang_request = gtr_select_target_lang_request;
		gtr_pre_select_sorce_lang_request = gtr_select_sorce_lang_request;
		gtr_pre_selected_sorce_lang_num = gtr_selected_sorce_lang_num;
		gtr_pre_selected_target_lang_num = gtr_selected_target_lang_num;
		gtr_pre_sorce_lang_offset = gtr_sorce_lang_offset;
		gtr_pre_target_lang_offset = gtr_target_lang_offset;
		gtr_need_reflesh = true;
	}

	Hid_query_key_state(&key);
	Log_main();
	if(Draw_query_need_reflesh() || !Sem_query_settings(SEM_ECO_MODE))
		gtr_need_reflesh = true;

	Hid_key_flag_reset();

	if(gtr_need_reflesh)
	{
		Draw_frame_ready();
		if (Sem_query_settings(SEM_NIGHT_MODE))
		{
			red = 1.0;
			green = 1.0;
			blue = 1.0;
			alpha = 0.75;
			Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
		}
		else
		{
			red = 0.0;
			green = 0.0;
			blue = 0.0;
			alpha = 1.0;
			Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);
		}

		Draw("Sorce : " + Gtr_get_lang_name(gtr_sorce_lang), 0, 0.0, 20.0, 0.6, 0.6, red, green, blue, alpha);
		Draw(gtr_input_text, 0, gtr_text_pos_x, 50.0, 0.6, 0.6, 0.25, 0.0, 0.5, 1.0);
		Draw("Target : " + Gtr_get_lang_name(gtr_target_lang), 0, 0.0, 100.0, 0.6, 0.6, red, green, blue, alpha);
		Draw(gtr_history[gtr_current_history_num], 0, gtr_text_pos_x, 130.0, 0.6, 0.6, 0.25, 0.0, 0.5, 1.0);
		Draw_top_ui();

		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(1, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(1, true, 2, 1.0, 1.0, 1.0);

		Draw(gtr_ver, 0, 0.0, 0.0, 0.45, 0.45, 0.0, 1.0, 0.0, 1.0);
		for (int i = 0; i < 10; i++)
		{
			if(i == gtr_selected_history_num)
				Draw(gtr_history[i], 0, gtr_text_pos_x, 10.0 + (i * 17.5), 0.6, 0.6, 0.25, 0.0, 0.5, 1.0);
			else
				Draw(gtr_history[i], 0, gtr_text_pos_x, 10.0 + (i * 17.5), 0.6, 0.6, 0.75, 0.5, 0.0, 1.0);
		}

		Draw_texture(Square_image, weak_aqua_tint, 0, 10.0, 190.0, 75.0, 15.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 10.0, 210.0, 75.0, 15.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 100.0, 190.0, 75.0, 15.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 100.0, 210.0, 75.0, 15.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 190.0, 190.0, 120.0, 15.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 190.0, 210.0, 120.0, 15.0);
		Draw(gtr_msg[4], 0, 12.5, 190.0, 0.4, 0.4, red, green, blue, alpha);
		Draw(gtr_msg[5], 0, 12.5, 210.0, 0.4, 0.4, red, green, blue, alpha);
		Draw(gtr_msg[6], 0, 102.5, 190.0, 0.4, 0.4, red, green, blue, alpha);
		Draw(gtr_msg[7], 0, 102.5, 210.0, 0.4, 0.4, red, green, blue, alpha);
		Draw(gtr_msg[8], 0, 192.5, 190.0, 0.375, 0.375, red, green, blue, alpha);
		Draw(gtr_msg[9], 0, 192.5, 210.0, 0.375, 0.375, red, green, blue, alpha);

		if (gtr_select_sorce_lang_request)
		{
			Draw_texture(Square_image, aqua_tint, 0, 25.0, 10.0, 270.0, 205.0);
			Draw(gtr_msg[0], 0, 27.5, 10.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);
			Draw(gtr_msg[2], 0, 27.5, 200.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);

			for (int i = 0; i < 10; i++)
			{
				if(i == (int)gtr_selected_sorce_lang_num)
					Draw(gtr_lang_list[(int)gtr_sorce_lang_offset + i], 0, 27.5, 20.0 + (i * 18.0), 0.6, 0.6, 1.0, 0.0, 0.0, 1.0);
				else
					Draw(gtr_lang_list[(int)gtr_sorce_lang_offset + i], 0, 27.5, 20.0 + (i * 18.0), 0.6, 0.6, 0.0, 0.0, 0.0, 1.0);
			}
		}
		else if (gtr_select_target_lang_request)
		{
			Draw_texture(Square_image, aqua_tint, 0, 25.0, 10.0, 270.0, 205.0);
			Draw(gtr_msg[1], 0, 27.5, 10.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);
			Draw(gtr_msg[2], 0, 27.5, 200.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);

			for (int i = 0; i < 10; i++)
			{
				if (i == (int)gtr_selected_target_lang_num)
					Draw(gtr_lang_list[(int)gtr_target_lang_offset + i], 0, 27.5, 20.0 + (i * 18.0), 0.6, 0.6, 1.0, 0.0, 0.0, 1.0);
				else
					Draw(gtr_lang_list[(int)gtr_target_lang_offset + i], 0, 27.5, 20.0 + (i * 18.0), 0.6, 0.6, 0.0, 0.0, 0.0, 1.0);
			}
		}
		Draw_bot_ui();
		Draw_touch_pos();

		Draw_apply_draw();
		gtr_need_reflesh = false;
	}
	else
		gspWaitForVBlank();

	if (Err_query_error_show_flag())
	{
		if (key.p_touch && key.touch_x >= 150 && key.touch_x <= 170 && key.touch_y >= 150 && key.touch_y < 170)
			Err_set_error_show_flag(false);
	}
	else
	{
		if (key.p_start || (key.p_touch && key.touch_x >= 110 && key.touch_x <= 230 && key.touch_y >= 220 && key.touch_y <= 240))
			Gtr_suspend();
		else if (gtr_select_sorce_lang_request)
		{
			if (key.p_touch)
			{
				for (int i = 0; i < 10; i++)
				{
					if (key.touch_x >= 27 && key.touch_x <= 279 && key.touch_y >= 20 + (i * 18) && key.touch_y <= 37 + (i * 18))
					{
						if (i == (int)gtr_selected_sorce_lang_num)
						{
							gtr_sorce_lang = gtr_lang_short_list[(int)gtr_selected_sorce_lang_num + (int)gtr_sorce_lang_offset];
							gtr_select_sorce_lang_request = false;
						}
						else
							gtr_selected_sorce_lang_num = i;

						break;
					}
				}
			}
			else if (key.p_a)
			{
				gtr_sorce_lang = gtr_lang_short_list[(int)gtr_selected_sorce_lang_num + (int)gtr_sorce_lang_offset];
				gtr_select_sorce_lang_request = false;
			}
			else if (key.p_y)
				gtr_select_sorce_lang_request = false;
			else if (key.h_d_left)
			{
				if((gtr_selected_sorce_lang_num - 0.5) >= 0.0)
					gtr_selected_sorce_lang_num -= 0.5;
				else if((gtr_sorce_lang_offset - 0.5) >= 0.0)
					gtr_sorce_lang_offset -= 0.5;
			}
			else if (key.h_d_right)
			{
				if((gtr_selected_sorce_lang_num + 0.5) <= 9.75)
					gtr_selected_sorce_lang_num += 0.5;
				else if ((gtr_sorce_lang_offset + 0.5) <= 95.75)
					gtr_sorce_lang_offset += 0.5;
			}
			else if (key.h_d_up)
			{
				if((gtr_selected_sorce_lang_num - 0.125) >= 0.0)
					gtr_selected_sorce_lang_num -= 0.125;
				else if((gtr_sorce_lang_offset - 0.125) >= 0.0)
					gtr_sorce_lang_offset -= 0.125;
			}
			else if (key.h_d_down)
			{
				if((gtr_selected_sorce_lang_num + 0.125) <= 9.75)
					gtr_selected_sorce_lang_num += 0.125;
				else if((gtr_sorce_lang_offset + 0.125) <= 95.75)
					gtr_sorce_lang_offset += 0.125;
			}
		}
		else if (gtr_select_target_lang_request)
		{
			if (key.p_touch)
			{
				for (int i = 0; i < 10; i++)
				{
					if (key.touch_x >= 27 && key.touch_x <= 279 && key.touch_y >= 20 + (i * 18) && key.touch_y <= 37 + (i * 18))
					{
						if (i == (int)gtr_selected_target_lang_num)
						{
							gtr_target_lang = gtr_lang_short_list[(int)gtr_selected_target_lang_num + (int)gtr_target_lang_offset];
							gtr_select_target_lang_request = false;
						}
						else
							gtr_selected_target_lang_num = i;

						break;
					}
				}
			}
			else if (key.p_a)
			{
				gtr_target_lang = gtr_lang_short_list[(int)gtr_selected_target_lang_num + (int)gtr_target_lang_offset];
				gtr_select_target_lang_request = false;
			}
			else if (key.p_y)
				gtr_select_target_lang_request = false;
			else if (key.h_d_left)
			{
				if((gtr_selected_target_lang_num - 0.5) >= 0.0)
					gtr_selected_target_lang_num -= 0.5;
				else if((gtr_target_lang_offset - 0.5) >= 0.0)
					gtr_target_lang_offset -= 0.5;
			}
			else if (key.h_d_right)
			{
				if((gtr_selected_target_lang_num + 0.5) <= 9.75)
					gtr_selected_target_lang_num += 0.5;
				else if ((gtr_target_lang_offset + 0.5) <= 95.75)
					gtr_target_lang_offset += 0.5;
			}
			else if (key.h_d_up)
			{
				if((gtr_selected_target_lang_num - 0.125) >= 0.0)
					gtr_selected_target_lang_num -= 0.125;
				else if((gtr_target_lang_offset - 0.125) >= 0.0)
					gtr_target_lang_offset -= 0.125;
			}
			else if (key.h_d_down)
			{
				if((gtr_selected_target_lang_num + 0.125) <= 9.75)
					gtr_selected_target_lang_num += 0.125;
				else if((gtr_target_lang_offset + 0.125) <= 95.75)
					gtr_target_lang_offset += 0.125;
			}
		}
		else
		{
			if (key.p_a || (key.p_touch && key.touch_x >= 10 && key.touch_x <= 84 && key.touch_y >= 190 && key.touch_y <= 204))
			{
				Hid_set_disable_flag(true);
				Swkbd_set_parameter(SWKBD_TYPE_NORMAL, SWKBD_NOTEMPTY_NOTBLANK, SWKBD_PREDICTIVE_INPUT, SWKBD_MULTILINE, 2, 8192, "メッセージを入力 / Type message here.", Menu_query_clipboard());
				if (Swkbd_launch(8192, &swkbd_data, SWKBD_BUTTON_RIGHT))
				{
					gtr_input_text = swkbd_data;
					gtr_tr_request = true;
				}
			}
			else if (key.p_x || (key.p_touch && key.touch_x >= 10 && key.touch_x <= 84 && key.touch_y >= 210 && key.touch_y <= 224))
				Menu_set_clipboard(gtr_history[gtr_selected_history_num]);
			else if (key.h_d_right)
				gtr_text_pos_x -= 5.0;
			else if (key.h_d_left)
			{
				if (gtr_text_pos_x + 5.0 <= 0.0)
					gtr_text_pos_x += 5.0;
				else
					gtr_text_pos_x = 0.0;
			}
			else if ((key.p_d_down || (key.p_touch && key.touch_x >= 100 && key.touch_x <= 174 && key.touch_y >= 210 && key.touch_y <= 224)) && (gtr_selected_history_num + 1) <= 9)
				gtr_selected_history_num++;
			else if ((key.p_d_up|| (key.p_touch && key.touch_x >= 100 && key.touch_x <= 174 && key.touch_y >= 190 && key.touch_y <= 204)) && (gtr_selected_history_num - 1) >= 0)
				gtr_selected_history_num--;
			else if (key.p_l || (key.p_touch && key.touch_x >= 190 && key.touch_x <= 309 && key.touch_y >= 190 && key.touch_y <= 204))
				gtr_select_sorce_lang_request = true;
			else if (key.p_r || (key.p_touch && key.touch_x >= 190 && key.touch_x <= 309 && key.touch_y >= 210 && key.touch_y <= 224))
				gtr_select_target_lang_request = true;
		}
	}
}
