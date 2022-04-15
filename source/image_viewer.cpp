#include "system/headers.hpp"

bool imv_already_init = false;
bool imv_main_run = false;
bool imv_thread_run = false;
bool imv_thread_suspend = true;
bool imv_img_dl_request = false;
bool imv_img_load_request = false;
bool imv_type_request = false;
bool imv_enable[64];
int imv_img_pos_x = 0;
int imv_img_pos_y = 0;
int imv_width = 0;
int imv_height = 0;
double imv_img_zoom = 0.5;
std::string imv_img_load_dir_name = "";
std::string imv_img_load_filename = "";
std::string imv_img_url = "";
std::string imv_msg[DEF_IMV_NUM_OF_MSG];
std::string imv_status = "";
Thread imv_init_thread, imv_exit_thread, imv_load_img_thread;
Image_data imv_image[64], imv_reload_button, imv_dl_button, imv_select_file_button, imv_change_url_button, imv_decrease_size_button, imv_increase_size_button;








bool imv_button_selected[2] = { false, false, };
double imv_touch_x_move_left = 0;
double imv_touch_y_move_left = 0;
bool imv_scroll_mode = false;














bool Imv_query_init_flag(void)
{
	return imv_already_init;
}

bool Imv_query_running_flag(void)
{
	return imv_main_run;
}

void Imv_set_url(std::string url)
{
	imv_img_url = url;
	imv_img_dl_request = true;
}

void Imv_set_load_file(std::string filename, std::string dir_name)
{
	imv_img_load_filename = filename;
	imv_img_load_dir_name = dir_name;
	imv_img_load_request = true;
}

void Imv_cancel_select_file(void)
{
}

void Imv_suspend(void)
{
	imv_thread_suspend = true;
	imv_main_run = false;
	Menu_resume();
}

void Imv_resume(void)
{
	imv_thread_suspend = false;
	imv_main_run = true;
	var_need_reflesh = true;
	Menu_suspend();
}

void Imv_hid(Hid_info key)
{
	if(Util_err_query_error_show_flag())
		Util_err_main(key);
	else if(Util_expl_query_show_flag())
		Util_expl_main(key);
	else
	{
		if(Util_hid_is_pressed(key, *Draw_get_bot_ui_button()))
			Draw_get_bot_ui_button()->selected = true;
		else if (key.p_start || (Util_hid_is_released(key, *Draw_get_bot_ui_button()) && Draw_get_bot_ui_button()->selected))
			Imv_suspend();

		if (key.p_touch || key.h_touch)
		{
			imv_touch_x_move_left = 0;
			imv_touch_y_move_left = 0;

			if (imv_scroll_mode)
			{
				imv_touch_x_move_left = key.touch_x_move;
				imv_touch_y_move_left = key.touch_y_move;
			}
			else if (key.p_touch && key.touch_y <= 169)
				imv_scroll_mode = true;
		}
		else
		{
			imv_scroll_mode = false;
			imv_touch_x_move_left -= (imv_touch_x_move_left * 0.025);
			imv_touch_y_move_left -= (imv_touch_y_move_left * 0.025);
			if (imv_touch_x_move_left < 0.5 && imv_touch_x_move_left > -0.5)
				imv_touch_x_move_left = 0;
			if (imv_touch_y_move_left < 0.5 && imv_touch_y_move_left > -0.5)
				imv_touch_y_move_left = 0;
		}

		if (key.h_c_up || key.h_c_down)
		{
			if(key.held_time > 240)
				imv_img_pos_y += ((double)key.cpad_y * var_scroll_speed) * 0.125;
			else
				imv_img_pos_y += ((double)key.cpad_y * var_scroll_speed) * 0.0625;
		}
		if (key.h_c_left || key.h_c_right)
		{
			if(key.held_time > 240)
				imv_img_pos_x -= ((double)key.cpad_x * var_scroll_speed) * 0.125;
			else
				imv_img_pos_x -= ((double)key.cpad_x * var_scroll_speed) * 0.0625;
		}
		else if (Util_hid_is_pressed(key, imv_reload_button))
			imv_reload_button.selected = true;
		else if (key.p_a || (Util_hid_is_released(key, imv_reload_button) && imv_reload_button.selected))
			imv_img_load_request = true;
		else if (Util_hid_is_pressed(key, imv_dl_button))
			imv_dl_button.selected = true;
		else if (key.p_b || (Util_hid_is_released(key, imv_dl_button) && imv_dl_button.selected))
			imv_img_dl_request = true;
		else if (Util_hid_is_pressed(key, imv_select_file_button))
			imv_select_file_button.selected = true;
		else if (key.p_x || (Util_hid_is_released(key, imv_select_file_button) && imv_select_file_button.selected))
		{
			Util_expl_set_callback(Imv_set_load_file);
			Util_expl_set_cancel_callback(Imv_cancel_select_file);
			Util_expl_set_show_flag(true);
		}
		else if (Util_hid_is_pressed(key, imv_change_url_button))
			imv_change_url_button.selected = true;
		else if (key.p_y || (Util_hid_is_released(key, imv_change_url_button) && imv_change_url_button.selected))
		{
			imv_type_request = true;
			while(imv_type_request)
				usleep(20000);
		}
		else if (Util_hid_is_pressed(key, imv_decrease_size_button))
			imv_decrease_size_button.selected = true;
		else if (key.h_l || (Util_hid_is_held(key, imv_decrease_size_button) && imv_decrease_size_button.selected))
		{
			if((imv_img_zoom - 0.0025) * imv_width >= 20)
				imv_img_zoom -= 0.0025;
		}
		else if (Util_hid_is_pressed(key, imv_increase_size_button))
			imv_increase_size_button.selected = true;
		else if (key.h_r || (Util_hid_is_held(key, imv_increase_size_button) && imv_increase_size_button.selected))
		{
			if((imv_img_zoom + 0.0025) * imv_width <= 2000)
				imv_img_zoom += 0.0025;
		}

		imv_img_pos_x -= (imv_touch_x_move_left * var_scroll_speed);
		imv_img_pos_y -= (imv_touch_y_move_left * var_scroll_speed);
	}

	if(!key.p_touch && !key.h_touch)
	{
		Draw_get_bot_ui_button()->selected = false;
		imv_reload_button.selected = false;
		imv_dl_button.selected = false;
		imv_select_file_button.selected = false;
		imv_change_url_button.selected = false;
		imv_decrease_size_button.selected = false;
		imv_increase_size_button.selected = false;
	}

	if(Util_log_query_log_show_flag())
		Util_log_main(key);
}

void Imv_img_load_thread(void* arg)
{
	Util_log_save(DEF_IMV_LOAD_THREAD_STR, "Thread started.");
	u8* raw_buffer = NULL;
	u32 dled_size = 0;
	int log_num = 0;
	int width = 0, height = 0;
	size_t cut_pos[2] = { 0, 0, };
	char time[128];
	std::string filename = "";
	Result_with_string result;

	while (imv_thread_run)
	{
		if(imv_img_dl_request)
		{
			filename = imv_img_url;
			cut_pos[0] = filename.find("&id=");
			cut_pos[1] = filename.find("lh3.googleusercontent.com/d/");
			if (cut_pos[0] == std::string::npos && cut_pos[1] == std::string::npos)
			{
				//If URL is not google drive URL, use time as filename
				memset(time, 0, 128);
				sprintf(time, "%04d_%02d_%02d_%02d_%02d_%02d", var_years, var_months, var_days, var_hours, var_minutes, var_seconds);
				filename = time;
			}//If URL is google drive URL, use unique ID as filename
			else if(!(cut_pos[0] == std::string::npos))
				filename = filename.substr(cut_pos[0] + 4);
			else if (!(cut_pos[1] == std::string::npos))
				filename = filename.substr(cut_pos[1] + 28);

			if (filename.length() > 33)
				filename = filename.substr(0, 33);

			log_num = Util_log_save(DEF_IMV_LOAD_THREAD_STR, "Util_httpc_save_data()....");
			result = Util_httpc_save_data(imv_img_url, 1024 * 256, &dled_size, true, 5, DEF_MAIN_DIR + "images/", filename + ".jpg");
			Util_log_add(log_num, result.string, result.code);
			if (result.code == 0)
			{
				//Load it
				imv_img_load_request = true;
				imv_img_load_dir_name = DEF_MAIN_DIR + "images/";
				imv_img_load_filename = filename + ".jpg";
			}

			if(result.code != 0)
			{
				Util_err_set_error_message(result.string, result.error_description, DEF_IMV_LOAD_THREAD_STR, result.code);
				Util_err_set_error_show_flag(true);
			}

			imv_img_dl_request = false;
		}
		else if (imv_img_load_request)
		{
			for(int i = 0; i < 64; i++)
			{
				imv_enable[i] = false;
				Draw_texture_free(&imv_image[i]);
			}
			var_need_reflesh = true;

			log_num = Util_log_save(DEF_IMV_LOAD_THREAD_STR, "Util_image_decoder_decode()...");
			result = Util_image_decoder_decode(imv_img_load_dir_name + imv_img_load_filename, &raw_buffer, &width, &height, false);
			Util_log_add(log_num, std::to_string(width) + "x" + std::to_string(height) + " " + result.string, result.code);

			if (result.code == 0)
			{
				if(width != 0 && height != 0)
				{
					//Fit to screen size
					if((width / 400.0) >= (height / 240.0))
						imv_img_zoom = 1 / (width / 400.0);
					else
						imv_img_zoom = 1 / (height / 240.0);

					imv_img_pos_x = (400 - width * imv_img_zoom) / 2;
					imv_img_pos_y = (240 - height * imv_img_zoom) / 2;
					imv_width = width;
					imv_height = height;
				}
				
				log_num = Util_log_save(DEF_IMV_LOAD_THREAD_STR, "Util_converter_rgb888be_to_rgb888le()...");
				result = Util_converter_rgb888be_to_rgb888le(raw_buffer, width, height);
				Util_log_add(log_num, result.string, result.code);

				//Set texture data
				for (int i = 0; i < 8; i++)
				{
					if(i * 512 > height)
						continue;

					for (int k = 0; k < 8; k++)
					{
						if(k * 512 > width)
							continue;

						log_num = Util_log_save(DEF_IMV_LOAD_THREAD_STR, "Draw_texture_init()...");
						result = Draw_texture_init(&imv_image[k + (i * 8)], 512, 512, DEF_DRAW_FORMAT_RGB888);
						Util_log_add(log_num, result.string, result.code);

						if(result.code == 0)
						{
							Draw_set_texture_filter(&imv_image[k + (i * 8)], true);
							imv_enable[k + (i * 8)] = true;
							log_num = Util_log_save(DEF_IMV_LOAD_THREAD_STR, "Draw_set_texture_data()...");
							result = Draw_set_texture_data(&imv_image[k + (i * 8)], raw_buffer, width, height, (k * 512), (i * 512), 512, 512, DEF_DRAW_FORMAT_RGB888);
							Util_log_add(log_num, result.string, result.code);
							var_need_reflesh = true;
						}
					}
				}
			}
			free(raw_buffer);
			raw_buffer = NULL;

			if(result.code != 0)
			{
				Util_err_set_error_message(result.string, result.error_description, DEF_IMV_LOAD_THREAD_STR, result.code);
				Util_err_set_error_show_flag(true);
			}

			imv_img_load_request = false;
		}
		else
			usleep(DEF_ACTIVE_THREAD_SLEEP_TIME);

		while (imv_thread_suspend)
			usleep(DEF_INACTIVE_THREAD_SLEEP_TIME);
	}

	for(int i = 0; i < 64; i++)
	{
		imv_enable[i] = false;
		Draw_texture_free(&imv_image[i]);
	}
	
	Util_log_save(DEF_IMV_LOAD_THREAD_STR, "Thread exit.");
	threadExit(0);
}

void Imv_init_thread(void* arg)
{
	Util_log_save(DEF_IMV_INIT_STR, "Thread started.");
	Result_with_string result;

	imv_status = "Starting threads...";
	imv_thread_run = true;
	if((var_model == CFG_MODEL_N2DSXL || var_model == CFG_MODEL_N3DSXL || var_model == CFG_MODEL_N3DS) && var_core_2_available)
		imv_load_img_thread = threadCreate(Imv_img_load_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 2, false);
	else
		imv_load_img_thread = threadCreate(Imv_img_load_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 0, false);

	imv_status += "\nInitializing variables...";
	for (int i = 0; i < 64; i++)
		imv_enable[i] = false;

	imv_reload_button.c2d = var_square_image[0];
	imv_dl_button.c2d = var_square_image[0];
	imv_select_file_button.c2d = var_square_image[0];
	imv_change_url_button.c2d = var_square_image[0];
	imv_decrease_size_button.c2d = var_square_image[0];
	imv_increase_size_button.c2d = var_square_image[0];

	Util_add_watch(&imv_img_pos_x);
	Util_add_watch(&imv_img_pos_y);
	Util_add_watch(&imv_img_zoom);
	Util_add_watch(&imv_reload_button.selected);
	Util_add_watch(&imv_dl_button.selected);
	Util_add_watch(&imv_select_file_button.selected);
	Util_add_watch(&imv_change_url_button.selected);
	Util_add_watch(&imv_decrease_size_button.selected);
	Util_add_watch(&imv_increase_size_button.selected);

	imv_already_init = true;

	Util_log_save(DEF_IMV_INIT_STR, "Thread exit.");
	threadExit(0);
}

void Imv_exit_thread(void* arg)
{
	Util_log_save(DEF_IMV_EXIT_STR, "Thread started.");
	Result_with_string result;

	imv_already_init = false;
	imv_thread_run = false;
	imv_thread_suspend = false;

	imv_status = "Exiting threads...";
	Util_log_save(DEF_IMV_EXIT_STR, "threadJoin()...", threadJoin(imv_init_thread, DEF_THREAD_WAIT_TIME));	

	imv_status += ".";
	Util_log_save(DEF_IMV_EXIT_STR, "threadJoin()...", threadJoin(imv_load_img_thread, DEF_THREAD_WAIT_TIME));

	imv_status += "\nCleaning up...";
	threadFree(imv_init_thread);
	threadFree(imv_load_img_thread);

	Util_remove_watch(&imv_img_pos_x);
	Util_remove_watch(&imv_img_pos_y);
	Util_remove_watch(&imv_img_zoom);
	Util_remove_watch(&imv_reload_button.selected);
	Util_remove_watch(&imv_dl_button.selected);
	Util_remove_watch(&imv_select_file_button.selected);
	Util_remove_watch(&imv_change_url_button.selected);
	Util_remove_watch(&imv_decrease_size_button.selected);
	Util_remove_watch(&imv_increase_size_button.selected);

	imv_already_init = false;

	Util_log_save(DEF_IMV_EXIT_STR, "Thread exit.");
	threadExit(0);
}

Result_with_string Imv_load_msg(std::string lang)
{
	return  Util_load_msg("imv_" + lang + ".txt", imv_msg, DEF_IMV_NUM_OF_MSG);
}

void Imv_init(bool draw)
{
	Util_log_save(DEF_IMV_INIT_STR, "Initializing...");
	int color = DEF_DRAW_BLACK;
	int back_color = DEF_DRAW_WHITE;

	Util_add_watch(&imv_status);
	imv_status = "";

	if((var_model == CFG_MODEL_N2DSXL || var_model == CFG_MODEL_N3DSXL || var_model == CFG_MODEL_N3DS) && var_core_2_available)
		imv_init_thread = threadCreate(Imv_init_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 2, false);
	else
	{
		APT_SetAppCpuTimeLimit(80);
		imv_init_thread = threadCreate(Imv_init_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 1, false);
	}

	while(!imv_already_init)
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
				Draw_screen_ready(0, back_color);
				Draw_top_ui();
				Draw(imv_status, 0, 20, 0.65, 0.65, color);

				Draw_apply_draw();
			}
			else
				gspWaitForVBlank();
		}
		else
			usleep(20000);
	}

	if(!(var_model == CFG_MODEL_N2DSXL || var_model == CFG_MODEL_N3DSXL || var_model == CFG_MODEL_N3DS) || !var_core_2_available)
		APT_SetAppCpuTimeLimit(10);

	Imv_resume();

	Util_log_save(DEF_IMV_INIT_STR, "Initialized.");
}

void Imv_exit(bool draw)
{
	Util_log_save(DEF_IMV_EXIT_STR, "Exiting...");

	int color = DEF_DRAW_BLACK;
	int back_color = DEF_DRAW_WHITE;

	imv_status = "";
	imv_exit_thread = threadCreate(Imv_exit_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 1, false);

	while(imv_already_init)
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
				Draw_screen_ready(0, back_color);
				Draw_top_ui();
				Draw(imv_status, 0, 20, 0.65, 0.65, color);

				Draw_apply_draw();
			}
			else
				gspWaitForVBlank();
		}
		else
			usleep(20000);
	}

	Util_log_save(DEF_IMV_EXIT_STR, "threadJoin()...", threadJoin(imv_exit_thread, DEF_THREAD_WAIT_TIME));	
	threadFree(imv_exit_thread);
	Util_remove_watch(&imv_status);
	var_need_reflesh = true;

	Util_log_save(DEF_IMV_EXIT_STR, "Exited.");
}

void Imv_main(void)
{
	int img_pos_x_offset = 0;
	int img_pos_y_offset = 0;
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
			Draw_screen_ready(0, back_color);

			Draw_top_ui();

			//Draw image
			for (int i = 0; i < 8; i++)
			{
				for (int k = 0; k < 8; k++)
				{
					if (imv_enable[k + (i * 8)])
					{
						Draw_texture(&imv_image[k + (i * 8)], (imv_img_pos_x + img_pos_x_offset), (imv_img_pos_y + img_pos_y_offset),
						imv_image[k + (i * 8)].subtex->width * imv_img_zoom, imv_image[k + (i * 8)].subtex->height * imv_img_zoom);
						img_pos_x_offset += imv_image[k + (i * 8)].subtex->width * imv_img_zoom;
					}
				}

				img_pos_x_offset = 0;
				if (imv_enable[i * 8])
					img_pos_y_offset += imv_image[i * 8].subtex->height * imv_img_zoom;
			}

			if(Util_log_query_log_show_flag())
				Util_log_draw();

			if(var_3d_mode)
			{
				Draw_screen_ready(2, back_color);

				if(Util_log_query_log_show_flag())
					Util_log_draw();

				Draw_top_ui();
			}
		}
		
		if(var_turn_on_bottom_lcd)
		{
			Draw_screen_ready(1, back_color);

			Draw(DEF_IMV_VER, 0, 0, 0.4, 0.4, DEF_DRAW_GREEN);

			//Draw image
			img_pos_x_offset = 0;
			img_pos_y_offset = 0;
			for (int i = 0; i < 8; i++)
			{
				for (int k = 0; k < 8; k++)
				{
					if (imv_enable[k + (i * 8)])
					{
						Draw_texture(&imv_image[k + (i * 8)], (imv_img_pos_x + img_pos_x_offset - 40), (imv_img_pos_y + img_pos_y_offset - 240),
						imv_image[k + (i * 8)].subtex->width * imv_img_zoom, imv_image[k + (i * 8)].subtex->height * imv_img_zoom);
						img_pos_x_offset += imv_image[k + (i * 8)].subtex->width * imv_img_zoom;
					}
				}

				img_pos_x_offset = 0;
				if (imv_enable[i * 8])
					img_pos_y_offset += imv_image[i * 8].subtex->height * imv_img_zoom;
			}

			Draw_texture(var_square_image[0], DEF_DRAW_WEAK_RED, 5, 170, 310, 55);

			//Reload button
			Draw(imv_msg[0], 10, 185, 0.4, 0.4, color, DEF_DRAW_X_ALIGN_CENTER, DEF_DRAW_Y_ALIGN_CENTER, 65, 13,
			DEF_DRAW_BACKGROUND_ENTIRE_BOX, &imv_reload_button, imv_reload_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
			
			//Download button
			Draw(imv_msg[1], 10, 205, 0.4, 0.4, color, DEF_DRAW_X_ALIGN_CENTER, DEF_DRAW_Y_ALIGN_CENTER, 65, 13,
			DEF_DRAW_BACKGROUND_ENTIRE_BOX, &imv_dl_button, imv_dl_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

			//Select file button
			Draw(imv_msg[2], 80, 185, 0.4, 0.4, color, DEF_DRAW_X_ALIGN_CENTER, DEF_DRAW_Y_ALIGN_CENTER, 65, 13,
			DEF_DRAW_BACKGROUND_ENTIRE_BOX, &imv_select_file_button, imv_select_file_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

			//Change URL button
			Draw(imv_msg[3], 80, 205, 0.4, 0.4, color, DEF_DRAW_X_ALIGN_CENTER, DEF_DRAW_Y_ALIGN_CENTER, 65, 13,
			DEF_DRAW_BACKGROUND_ENTIRE_BOX, &imv_change_url_button, imv_change_url_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

			//Decrease size button
			Draw(imv_msg[4], 170, 185, 0.4, 0.4, color, DEF_DRAW_X_ALIGN_CENTER, DEF_DRAW_Y_ALIGN_CENTER, 140, 13,
			DEF_DRAW_BACKGROUND_ENTIRE_BOX, &imv_decrease_size_button, imv_decrease_size_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
			
			//Increase size button
			Draw(imv_msg[5], 170, 205, 0.4, 0.4, color, DEF_DRAW_X_ALIGN_CENTER, DEF_DRAW_Y_ALIGN_CENTER, 140, 13,
			DEF_DRAW_BACKGROUND_ENTIRE_BOX, &imv_increase_size_button, imv_increase_size_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

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

	if(imv_type_request)
	{
		Util_swkbd_init(SWKBD_TYPE_NORMAL, SWKBD_NOTEMPTY_NOTBLANK, 1, 8192, "画像URLを入力 / Type image url here.", imv_img_url, SWKBD_PREDICTIVE_INPUT);
		Util_swkbd_launch(&imv_img_url);
		Util_swkbd_exit();
		imv_type_request = false;
	}
}
