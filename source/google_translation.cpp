#include "definitions.hpp"
#include "system/types.hpp"

#include "system/menu.hpp"
#include "system/variables.hpp"

#include "system/draw/draw.hpp"

#include "system/util/error.hpp"
#include "system/util/hid.hpp"
#include "system/util/httpc.hpp"
#include "system/util/log.hpp"
#include "system/util/swkbd.hpp"
#include "system/util/util.hpp"

//Include myself.
#include "google_translation.hpp"

bool gtr_already_init = false;
bool gtr_main_run = false;
bool gtr_tr_thread_run = false;
bool gtr_thread_suspend = false;
bool gtr_tr_request = false;
bool gtr_select_sorce_lang_request = false;
bool gtr_select_target_lang_request = false;
bool gtr_type_request = false;
int gtr_current_history_num = 9;
int gtr_selected_history_num = 0;
int gtr_source_lang_index = 21;
int gtr_target_lang_index = 0;
double gtr_selected_sorce_lang_num = 0;
double gtr_selected_target_lang_num = 0;
double gtr_sorce_lang_offset = 0;
double gtr_target_lang_offset = 0;
double gtr_text_pos_x = 0.0;
std::string gtr_input_text = "n/a";
std::string gtr_history[10] = { "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", };
std::string gtr_lang_short_list[DEF_GTR_NUM_OF_LANG_LIST_MSG];
std::string gtr_lang_list[DEF_GTR_NUM_OF_LANG_LIST_MSG];
std::string gtr_msg[DEF_GTR_NUM_OF_MSG];
std::string gtr_status = "";
Thread gtr_init_thread, gtr_exit_thread, gtr_tr_thread;
Image_data gtr_translate_button, gtr_copy_button, gtr_up_button, gtr_down_button, gtr_change_source_lang_button, gtr_change_target_lang_button;

bool Gtr_query_init_flag(void)
{
	return gtr_already_init;
}

bool Gtr_query_running_flag(void)
{
	return gtr_main_run;
}

void Gtr_suspend(void)
{
	gtr_thread_suspend = true;
	gtr_main_run = false;
	var_need_reflesh = true;
	Menu_resume();
}

void Gtr_resume(void)
{
	gtr_thread_suspend = false;
	gtr_main_run = true;
	var_need_reflesh = true;
	Menu_suspend();
}

void Gtr_tr_thread(void* arg)
{
	Util_log_save(DEF_GTR_TRANSLATION_THREAD_STR, "Thread started.");

	u8* httpc_buffer = NULL;
	u32 dl_size = 0;
	int log_num = 0;
	std::string send_data = "";
	std::string url = "https://script.google.com/macros/s/AKfycbwbL6swXIeycS-WpVBrfKLh40bXg2CAv1PdAzuIhalqq2SGrJM/exec";
	Result_with_string result;

	while (gtr_tr_thread_run)
	{
		if (gtr_tr_request)
		{
			if (gtr_current_history_num + 1 > 9)
				gtr_current_history_num = 0;
			else
				gtr_current_history_num++;

			gtr_history[gtr_current_history_num] = gtr_msg[3];
			gtr_input_text = Util_encode_to_escape(gtr_input_text);
			send_data = "{ \"text\": \"" + gtr_input_text + "\",\"sorce\" : \"" + gtr_lang_short_list[gtr_source_lang_index] + "\",\"target\" : \"" + gtr_lang_short_list[gtr_target_lang_index] + "\" }";

			log_num = Util_log_save(DEF_GTR_TRANSLATION_THREAD_STR, "Util_httpc_post_and_dl_data()...");
			result = Util_httpc_post_and_dl_data(url, (u8*)send_data.c_str(), send_data.length(), &httpc_buffer, 0x10000, &dl_size, true, 5);
			Util_log_add(log_num, result.string, result.code);

			if (result.code == 0)
				gtr_history[gtr_current_history_num] = (char*)httpc_buffer;
			else
			{
				gtr_history[gtr_current_history_num] = "***Translation failed.***";
				Util_err_set_error_message(result.string, result.error_description, DEF_GTR_TRANSLATION_THREAD_STR, result.code);
				Util_err_set_error_show_flag(true);
			}

			free(httpc_buffer);
			httpc_buffer = NULL;
			gtr_tr_request = false;
		}
		else
			Util_sleep(DEF_ACTIVE_THREAD_SLEEP_TIME);

		while (gtr_thread_suspend)
			Util_sleep(DEF_INACTIVE_THREAD_SLEEP_TIME);
	}
	Util_log_save(DEF_GTR_TRANSLATION_THREAD_STR, "Thread exit.");
	threadExit(0);
}

void Gtr_hid(Hid_info key)
{
	if(Util_err_query_error_show_flag())
		Util_err_main(key);
	else
	{
		if(Util_hid_is_pressed(key, *Draw_get_bot_ui_button()))
			Draw_get_bot_ui_button()->selected = true;
		else if (key.p_start || (Util_hid_is_released(key, *Draw_get_bot_ui_button()) && Draw_get_bot_ui_button()->selected))
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
							gtr_source_lang_index = (int)gtr_selected_sorce_lang_num + (int)gtr_sorce_lang_offset;
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
				gtr_source_lang_index = (int)gtr_selected_sorce_lang_num + (int)gtr_sorce_lang_offset;
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
							gtr_target_lang_index = (int)gtr_selected_target_lang_num + (int)gtr_target_lang_offset;
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
				gtr_target_lang_index = (int)gtr_selected_target_lang_num + (int)gtr_target_lang_offset;
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
			if(Util_hid_is_pressed(key, gtr_translate_button))
				gtr_translate_button.selected = true;
			else if (key.p_a || (Util_hid_is_released(key, gtr_translate_button) && gtr_translate_button.selected))
			{
				gtr_type_request = true;
				while(gtr_type_request)
					Util_sleep(20000);
			}
			else if(Util_hid_is_pressed(key, gtr_copy_button))
				gtr_copy_button.selected = true;
			else if (key.p_x || (Util_hid_is_released(key, gtr_copy_button) && gtr_copy_button.selected))
				var_clipboard = gtr_history[gtr_selected_history_num];
			else if (key.h_d_right)
				gtr_text_pos_x -= 5.0;
			else if (key.h_d_left)
			{
				if (gtr_text_pos_x + 5.0 <= 0.0)
					gtr_text_pos_x += 5.0;
				else
					gtr_text_pos_x = 0.0;
			}
			else if(Util_hid_is_pressed(key, gtr_down_button))
				gtr_down_button.selected = true;
			else if ((key.p_d_down || (Util_hid_is_released(key, gtr_down_button) && gtr_down_button.selected)) && (gtr_selected_history_num + 1) <= 9)
				gtr_selected_history_num++;
			else if(Util_hid_is_pressed(key, gtr_up_button))
				gtr_up_button.selected = true;
			else if ((key.p_d_up || (Util_hid_is_released(key, gtr_up_button) && gtr_up_button.selected)) && (gtr_selected_history_num - 1) >= 0)
				gtr_selected_history_num--;
			else if(Util_hid_is_pressed(key, gtr_change_source_lang_button))
				gtr_change_source_lang_button.selected = true;
			else if (key.p_l || (Util_hid_is_released(key, gtr_change_source_lang_button) && gtr_change_source_lang_button.selected))
				gtr_select_sorce_lang_request = true;
			else if(Util_hid_is_pressed(key, gtr_change_target_lang_button))
				gtr_change_target_lang_button.selected = true;
			else if (key.p_r || (Util_hid_is_released(key, gtr_change_target_lang_button) && gtr_change_target_lang_button.selected))
				gtr_select_target_lang_request = true;
		}
	}

	if(!key.p_touch && !key.h_touch)
	{
		Draw_get_bot_ui_button()->selected = false;
		gtr_translate_button.selected = false;
		gtr_copy_button.selected = false;
		gtr_up_button.selected = false;
		gtr_down_button.selected = false;
		gtr_change_source_lang_button.selected = false;
		gtr_change_target_lang_button.selected = false;
	}

	if(Util_log_query_log_show_flag())
		Util_log_main(key);
}

void Gtr_init_thread(void* arg)
{
	Util_log_save(DEF_GTR_INIT_STR, "Thread started.");
	Result_with_string result;
	
	gtr_status = "Starting threads...";
	gtr_tr_thread_run = true;
	gtr_tr_thread = threadCreate(Gtr_tr_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 0, false);

	gtr_status += "\nInitializing variables";
	Util_load_msg("gtr_lang_list.txt", gtr_lang_list, DEF_GTR_NUM_OF_LANG_LIST_MSG);
	Util_load_msg("gtr_short_lang_list.txt", gtr_lang_short_list, DEF_GTR_NUM_OF_LANG_LIST_MSG);
	gtr_current_history_num = 9;
	gtr_text_pos_x = 0.0;
	
	gtr_translate_button.c2d = var_square_image[0];
	gtr_copy_button.c2d = var_square_image[0];
	gtr_up_button.c2d = var_square_image[0];
	gtr_down_button.c2d = var_square_image[0];
	gtr_change_source_lang_button.c2d = var_square_image[0];
	gtr_change_target_lang_button.c2d = var_square_image[0];

	Util_add_watch(&gtr_translate_button.selected);
	Util_add_watch(&gtr_copy_button.selected);
	Util_add_watch(&gtr_up_button.selected);
	Util_add_watch(&gtr_down_button.selected);
	Util_add_watch(&gtr_change_source_lang_button.selected);
	Util_add_watch(&gtr_change_target_lang_button.selected);
	Util_add_watch(&gtr_select_sorce_lang_request);
	Util_add_watch(&gtr_select_target_lang_request);
	Util_add_watch(&gtr_selected_history_num);
	Util_add_watch(&gtr_current_history_num);
	Util_add_watch(&gtr_text_pos_x);
	Util_add_watch(&gtr_sorce_lang_offset);
	Util_add_watch(&gtr_selected_sorce_lang_num);
	Util_add_watch(&gtr_target_lang_offset);
	Util_add_watch(&gtr_selected_target_lang_num);
	
	gtr_already_init = true;

	Util_log_save(DEF_GTR_INIT_STR, "Thread exit.");
	threadExit(0);
}

void Gtr_exit_thread(void* arg)
{
	Util_log_save(DEF_GTR_EXIT_STR, "Thread started.");

	gtr_already_init = false;
	gtr_tr_thread_run = false;
	gtr_thread_suspend = false;

	gtr_status = "Exiting threads...";
	Util_log_save(DEF_GTR_EXIT_STR, "threadJoin()...", threadJoin(gtr_tr_thread, DEF_THREAD_WAIT_TIME));
	
	gtr_status += "\nCleaning up...";
	threadFree(gtr_tr_thread);

	gtr_input_text = "n/a";
	for (int i = 0; i < 10; i++)
		gtr_history[i] = "n/a";

	Util_remove_watch(&gtr_translate_button.selected);
	Util_remove_watch(&gtr_copy_button.selected);
	Util_remove_watch(&gtr_up_button.selected);
	Util_remove_watch(&gtr_down_button.selected);
	Util_remove_watch(&gtr_change_source_lang_button.selected);
	Util_remove_watch(&gtr_change_target_lang_button.selected);
	Util_remove_watch(&gtr_select_sorce_lang_request);
	Util_remove_watch(&gtr_select_target_lang_request);
	Util_remove_watch(&gtr_selected_history_num);
	Util_remove_watch(&gtr_current_history_num);
	Util_remove_watch(&gtr_text_pos_x);
	Util_remove_watch(&gtr_sorce_lang_offset);
	Util_remove_watch(&gtr_selected_sorce_lang_num);
	Util_remove_watch(&gtr_target_lang_offset);
	Util_remove_watch(&gtr_selected_target_lang_num);

	Util_log_save(DEF_GTR_EXIT_STR, "Thread exit.");
	threadExit(0);
}

Result_with_string Gtr_load_msg(std::string lang)
{
	return Util_load_msg("gtr_" + lang + ".txt", gtr_msg, DEF_GTR_NUM_OF_MSG);
}

void Gtr_init(bool draw)
{
	Util_log_save(DEF_MIC_INIT_STR, "Initializing...");
	int color = DEF_DRAW_BLACK;
	int back_color = DEF_DRAW_WHITE;

	Util_add_watch(&gtr_status);
	gtr_status = "";

	if((var_model == CFG_MODEL_N2DSXL || var_model == CFG_MODEL_N3DSXL || var_model == CFG_MODEL_N3DS) && var_core_2_available)
		gtr_init_thread = threadCreate(Gtr_init_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 2, false);
	else
	{
		APT_SetAppCpuTimeLimit(80);
		gtr_init_thread = threadCreate(Gtr_init_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 1, false);
	}

	while(!gtr_already_init)
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

				Draw(gtr_status, 0, 20, 0.65, 0.65, color);

				Draw_apply_draw();
			}
			else
				gspWaitForVBlank();
		}
		else
			Util_sleep(20000);
	}

	if(!(var_model == CFG_MODEL_N2DSXL || var_model == CFG_MODEL_N3DSXL || var_model == CFG_MODEL_N3DS) || !var_core_2_available)
		APT_SetAppCpuTimeLimit(10);

	Util_log_save(DEF_GTR_EXIT_STR, "threadJoin()...", threadJoin(gtr_init_thread, DEF_THREAD_WAIT_TIME));	
	threadFree(gtr_init_thread);
	Gtr_resume();

	Util_log_save(DEF_MIC_INIT_STR, "Initialized.");
}

void Gtr_exit(bool draw)
{
	Util_log_save(DEF_MIC_EXIT_STR, "Exiting...");

	int color = DEF_DRAW_BLACK;
	int back_color = DEF_DRAW_WHITE;

	gtr_status = "";
	gtr_exit_thread = threadCreate(Gtr_exit_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 1, false);

	while(gtr_already_init)
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

				Draw(gtr_status, 0, 20, 0.65, 0.65, color);

				Draw_apply_draw();
			}
			else
				gspWaitForVBlank();
		}
		else
			Util_sleep(20000);
	}

	Util_log_save(DEF_MIC_EXIT_STR, "threadJoin()...", threadJoin(gtr_exit_thread, DEF_THREAD_WAIT_TIME));	
	threadFree(gtr_exit_thread);
	Util_remove_watch(&gtr_status);
	var_need_reflesh = true;

	Util_log_save(DEF_MIC_EXIT_STR, "Exited.");
}

void Gtr_main(void)
{
	int color = DEF_DRAW_BLACK;
	int back_color = DEF_DRAW_WHITE;

	if (var_night_mode)
	{
		color = DEF_DRAW_WHITE;
		back_color = DEF_DRAW_BLACK;
	}

	if(Util_is_watch_changed() || var_need_reflesh || !var_eco_mode)
	{
		var_need_reflesh = false;
		Draw_frame_ready();

		if(var_turn_on_top_lcd)
		{
			Draw_screen_ready(SCREEN_TOP_LEFT, back_color);

			Draw("Sorce : " + gtr_lang_list[gtr_source_lang_index], 0.0, 20.0, 0.6, 0.6, color);
			Draw(gtr_input_text, gtr_text_pos_x, 50, 0.6, 0.6, color);
			Draw("Target : " + gtr_lang_list[gtr_target_lang_index], 0.0, 100.0, 0.6, 0.6, color);
			Draw(gtr_history[gtr_current_history_num], gtr_text_pos_x, 130, 0.6, 0.6, color);

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

			Draw(DEF_GTR_VER, 0, 0, 0.4, 0.4, DEF_DRAW_GREEN);

			for (int i = 0; i < 10; i++)
				Draw(gtr_history[i], gtr_text_pos_x, 10.0 + (i * 17.5), 0.6, 0.6, i == gtr_selected_history_num ? 0xFF8000FF : 0xFF808000);

			Draw(gtr_msg[4], 10, 190, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 75, 15,
			BACKGROUND_ENTIRE_BOX, &gtr_translate_button, gtr_translate_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
			Draw(gtr_msg[5], 10, 210, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 75, 15,
			BACKGROUND_ENTIRE_BOX, &gtr_copy_button, gtr_copy_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
			Draw(gtr_msg[6], 100, 190, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 75, 15,
			BACKGROUND_ENTIRE_BOX, &gtr_up_button, gtr_up_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
			Draw(gtr_msg[7], 100, 210, 0.4, 0.4, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 75, 15,
			BACKGROUND_ENTIRE_BOX, &gtr_down_button, gtr_down_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
			Draw(gtr_msg[8], 190, 190, 0.375, 0.375, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 120, 15,
			BACKGROUND_ENTIRE_BOX, &gtr_change_source_lang_button, gtr_change_source_lang_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
			Draw(gtr_msg[9], 190, 210, 0.375, 0.375, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 120, 15,
			BACKGROUND_ENTIRE_BOX, &gtr_change_target_lang_button, gtr_change_target_lang_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

			if (gtr_select_sorce_lang_request)
			{
				Draw_texture(var_square_image[0], DEF_DRAW_AQUA, 25, 10, 270, 205);
				Draw(gtr_msg[0], 27.5, 10.0, 0.45, 0.45, DEF_DRAW_BLACK);
				Draw(gtr_msg[2], 27.5, 200.0, 0.45, 0.45, DEF_DRAW_BLACK);

				for (int i = 0; i < 10; i++)
					Draw(gtr_lang_list[(int)gtr_sorce_lang_offset + i], 27.5, 20 + (i * 18), 0.6, 0.6, i == (int)gtr_selected_sorce_lang_num ? DEF_DRAW_RED : DEF_DRAW_BLACK);
			}
			else if (gtr_select_target_lang_request)
			{
				Draw_texture(var_square_image[0], DEF_DRAW_AQUA, 25, 10, 270, 205);
				Draw(gtr_msg[1], 27.5, 10.0, 0.45, 0.45, DEF_DRAW_BLACK);
				Draw(gtr_msg[2], 27.5, 200.0, 0.45, 0.45, DEF_DRAW_BLACK);

				for (int i = 0; i < 10; i++)
					Draw(gtr_lang_list[(int)gtr_target_lang_offset + i], 27.5, 20 + (i * 18), 0.6, 0.6, i == (int)gtr_selected_target_lang_num ? DEF_DRAW_RED : DEF_DRAW_BLACK);
			}

			if(Util_err_query_error_show_flag())
				Util_err_draw();

			Draw_bot_ui();
		}

		Draw_apply_draw();
	}
	else
		gspWaitForVBlank();

	if(gtr_type_request)
	{
		Util_swkbd_init(SWKBD_TYPE_NORMAL, SWKBD_NOTEMPTY_NOTBLANK, 1, 8192, "メッセージを入力 / Type message here.", var_clipboard, SWKBD_PREDICTIVE_INPUT);
		Util_swkbd_launch(&gtr_input_text);
		Util_swkbd_exit();
		gtr_tr_request = true;
		gtr_type_request = false;
	}
}
