#include <3ds.h>
#include <unistd.h>
#include <string>
#include <algorithm>
#include <malloc.h>
#include "citro2d.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"
#include "hid.hpp"
#include "draw.hpp"
#include "file.hpp"
#include "log.hpp"
#include "error.hpp"
#include "menu.hpp"
#include "types.hpp"
#include "setting_menu.hpp"
#include "camera.hpp"
#include "util.hpp"

extern "C" void memcpy_asm(u8*, u8*, int);

/*For draw*/
bool cam_need_reflesh = false;
bool cam_pre_encode_request = false;
int cam_pre_selected_menu_mode = 0;
int cam_pre_display_img_num = -1;
int cam_pre_resolution_mode = 2;
int cam_pre_exposure_mode = 0;
int cam_pre_noise_filter_mode = 1;
int cam_pre_fps_mode = 8;
int cam_pre_contrast_mode = 5;
int cam_pre_white_balance_mode = 0;
int cam_pre_lens_correction_mode = 0;
int cam_pre_camera_mode = 0;
int cam_pre_shutter_sound_mode = 0;
int cam_pre_encode_format_mode = 1;
/*---------------------------------------------*/

bool cam_main_run = false;
bool cam_already_init = false;
bool cam_capture_thread_run = false;
bool cam_encode_thread_run = false;
bool cam_parse_thread_run = false;
bool cam_thread_suspend = false;
bool cam_take_request = false;
bool cam_take_video_request = false;
bool cam_parse_request = false;
bool cam_encode_request = false;
bool cam_change_resolution_request = false;
bool cam_change_fps_request = false;
bool cam_change_contrast_request = false;
bool cam_change_white_balance_request = false;
bool cam_change_lens_correction_request = false;
bool cam_change_camera_request = false;
bool cam_change_exposure_request = false;
bool cam_change_noise_filter_request = false;
bool cam_encode_video_request = false;
bool cam_button_selected[4];
u8* cam_capture_buffer = NULL;
u8* cam_capture_frame_buffer[4] = {  NULL, NULL, NULL, NULL, };
u8* cam_mic_buffer[2] = { NULL, NULL, };
u32 cam_buffer_size = 0;
int cam_mic_buffer_size = 0x40000;
int cam_selected_menu_mode = 0;
int cam_selected_jpg_quality = 95;
int cam_current_img_num = 0;
int cam_display_img_num = -1;
int cam_resolution_mode = 2;
int cam_request_resolution_mode = 2;
int cam_exposure_mode = 0;
int cam_request_exposure_mode = 0;
int cam_noise_filter_mode = 1;
int cam_request_noise_filter_mode = 1;
int cam_fps_mode = 8;
int cam_request_fps_mode = 8;
int cam_contrast_mode = 5;
int cam_request_contrast_mode = 5;
int cam_white_balance_mode = 0;
int cam_request_white_balance_mode = 0;
int cam_lens_correction_mode = 0;
int cam_request_lens_correction_mode = 0;
int cam_camera_mode = 0;
int cam_request_camera_mode = 0;
int cam_shutter_sound_mode = 0;
int cam_fps = 0;
int cam_encode_format_mode = 1;
std::string cam_msg[CAM_NUM_OF_MSG];
std::string cam_resolution_list[9];
std::string cam_exposure_list[6];
std::string cam_lens_correction_list[3];
std::string cam_white_balance_list[6];
std::string cam_contrast_list[11];
std::string cam_framelate_list[15];
std::string cam_format_name_list[2];
std::string cam_capture_thread_string = "Cam/Capture thread";
std::string cam_encode_thread_string = "Cam/Encode thread";
std::string cam_parse_thread_string = "Cam/Parse thread";
std::string cam_init_string = "Cam/Init";
std::string cam_exit_string = "Cam/Exit";
std::string cam_ver = "v1.0.4";
Thread cam_capture_thread, cam_encode_thread, cam_parse_thread;
C2D_Image cam_capture_image[4];

void Cam_main(void)
{
	int image_num = 0;
	int random_num;
	float text_red;
	float text_green;
	float text_blue;
	float text_alpha;
	double red[15], green[15], blue[15], alpha[15];
	double pos_x = 0.0;
	double pos_y = 0.0;
	double bar_x[4];
	Hid_info key;

	if (Sem_query_settings(SEM_NIGHT_MODE))
	{
		text_red = 1.0;
		text_green = 1.0;
		text_blue = 1.0;
		text_alpha = 0.75;
		white_or_black_tint = white_tint;
	}
	else
	{
		text_red = 0.0;
		text_green = 0.0;
		text_blue = 0.0;
		text_alpha = 1.0;
		white_or_black_tint = black_tint;
	}
	Sem_set_color(text_red, text_green, text_blue, text_alpha, red, green, blue, alpha, 15);

	if(cam_pre_display_img_num != cam_display_img_num || cam_pre_resolution_mode != cam_resolution_mode
	|| cam_pre_fps_mode != cam_fps_mode || cam_pre_contrast_mode != cam_contrast_mode
	|| cam_pre_white_balance_mode != cam_white_balance_mode || cam_pre_lens_correction_mode != cam_lens_correction_mode
	|| cam_pre_exposure_mode != cam_exposure_mode || cam_pre_encode_format_mode != cam_encode_format_mode
	|| cam_pre_selected_menu_mode != cam_selected_menu_mode || cam_pre_encode_request != cam_encode_request || cam_pre_shutter_sound_mode != cam_shutter_sound_mode
	|| cam_pre_noise_filter_mode != cam_noise_filter_mode || cam_pre_camera_mode != cam_camera_mode)
	{
		cam_pre_display_img_num = cam_display_img_num;
		cam_pre_resolution_mode = cam_resolution_mode;
		cam_pre_fps_mode = cam_fps_mode;
		cam_pre_contrast_mode = cam_contrast_mode;
		cam_pre_white_balance_mode = cam_white_balance_mode;
		cam_pre_lens_correction_mode = cam_lens_correction_mode;
		cam_pre_exposure_mode = cam_exposure_mode;
		cam_pre_encode_format_mode = cam_encode_format_mode;
		cam_pre_selected_menu_mode = cam_selected_menu_mode;
		cam_pre_encode_request = cam_encode_request;
		cam_pre_shutter_sound_mode = cam_shutter_sound_mode;
		cam_pre_noise_filter_mode = cam_noise_filter_mode;
		cam_pre_camera_mode = cam_camera_mode;
		cam_need_reflesh = true;
	}

	Hid_query_key_state(&key);
	Log_main();
	if(Draw_query_need_reflesh() || !Sem_query_settings(SEM_ECO_MODE))
		cam_need_reflesh = true;

	Hid_key_flag_reset();

	if(cam_need_reflesh)
	{
		Draw_frame_ready();
		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);

		if (cam_display_img_num != -1)
		{
			if (cam_display_img_num == 1)
				image_num = 0;
			else
				image_num = 2;

			if (Cam_convert_to_resolution(cam_resolution_mode, true) == 400 && Cam_convert_to_resolution(cam_resolution_mode, false) == 240)
				Draw_texture(cam_capture_image, dammy_tint, image_num, 0.0, 0.0, 400.0, 240.0);
			else if (cam_resolution_mode == 0)
			{
				Draw_texture(cam_capture_image, dammy_tint, image_num, 40.0, 0.0, 256.0, 240.0);
				Draw_texture(cam_capture_image, dammy_tint, image_num + 1, 296.0, 0.0, 64.0, 240.0);
			}
			else
				Draw_texture(cam_capture_image, dammy_tint, image_num, 40.0, 0.0, 320.0, 240.0);
		}
		Draw_top_ui();

		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(1, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(1, true, 2, 1.0, 1.0, 1.0);

		Draw(cam_ver, 0, 0.0, 0.0, 0.4, 0.4, 0.0, 1.0, 0.0, 1.0);
		Draw(std::to_string(Cam_convert_to_resolution(cam_resolution_mode, true)) + cam_msg[0] + std::to_string(Cam_convert_to_resolution(cam_resolution_mode, false)) + cam_msg[1] + cam_framelate_list[cam_fps_mode] + cam_msg[2], 0, 20.0, 15.0, 0.6, 0.6, text_red, text_green, text_blue, text_alpha);
		Draw(cam_msg[3] + cam_contrast_list[cam_contrast_mode] + cam_msg[4] + cam_white_balance_list[cam_white_balance_mode], 0, 20.0, 30.0, 0.55, 0.55, text_red, text_green, text_blue, text_alpha);
		Draw(cam_msg[5] + cam_lens_correction_list[cam_lens_correction_mode] + cam_msg[6] + cam_exposure_list[cam_exposure_mode], 0, 20.0, 45.0, 0.55, 0.55, text_red, text_green, text_blue, text_alpha);
		Draw(cam_msg[28] + cam_format_name_list[cam_encode_format_mode] + "/" + Menu_query_time(1) + "/", 0, 20.0, 60.0, 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);

		Draw_texture(Square_image, weak_yellow_tint, 0, 40.0, 110.0, 60.0, 10.0);
		Draw_texture(Square_image, weak_blue_tint, 0, 100.0, 110.0, 60.0, 10.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 160.0, 110.0, 60.0, 10.0);
		Draw_texture(Square_image, weak_red_tint, 0, 220.0, 110.0, 60.0, 10.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 40.0, 120.0, 240.0, 80.0);

		for (int i = 0; i < 3; i++)
		{
			if(i == cam_camera_mode)
			{
				red[i] = 1.0;
			}

			Draw_texture(Square_image, weak_aqua_tint, 0, 40.0 + (i * 85.0), 85.0, 70.0, 20.0);
			Draw(cam_msg[25 + i], 0, 40.0 + (i * 85.0), 85.0, 0.4, 0.4, red[i], green[i], blue[i], alpha[i]);
		}
		Sem_set_color(text_red, text_green, text_blue, text_alpha, red, green, blue, alpha, 15);

		if (cam_selected_menu_mode == CAM_MENU_RESOLUTION)
		{
			Draw_texture(Square_image, yellow_tint, 0, 40.0, 110.0, 60.0, 10.0);
			pos_x = 40.0;
			pos_y = 120.0;
			for (int i = 0; i < 9; i++)
			{
				if (i == cam_resolution_mode)
				{
					red[i] = 1.0;
					green[i] = 0.0;
					blue[i] = 0.0;
					alpha[i] = 1.0;
				}

				Draw_texture(Square_image, weak_aqua_tint, 0, pos_x, pos_y, 60.0, 20.0);
				Draw(cam_resolution_list[i], 0, (pos_x + 2.5), pos_y, 0.4, 0.4, red[i], green[i], blue[i], alpha[i]);
				if (pos_y + 30.0 > 180.0)
				{
					pos_x += 90.0;
					pos_y = 120.0;
				}
				else
					pos_y += 30.0;
			}
		}
		else if (cam_selected_menu_mode == CAM_MENU_FPS)
		{
			Draw_texture(Square_image, weak_blue_tint, 0, 100.0, 110.0, 60.0, 10.0);
			pos_x = 40.0;
			pos_y = 120.0;
			for (int i = 0; i < 15; i++)
			{
				if (i == cam_fps_mode)
				{
					red[i] = 1.0;
					green[i] = 0.0;
					blue[i] = 0.0;
					alpha[i] = 1.0;
				}

				Draw_texture(Square_image, weak_aqua_tint, 0, pos_x, pos_y, 30.0, 20.0);
				Draw(cam_framelate_list[i], 0, (pos_x + 2.5), pos_y, 0.4, 0.4, red[i], green[i], blue[i], alpha[i]);
				if (pos_y + 30.0 > 180.0)
				{
					pos_x += 50.0;
					pos_y = 120.0;
				}
				else
					pos_y += 30.0;
			}
		}
		else if (cam_selected_menu_mode == CAM_MENU_ADVANCED_0)
		{
			Draw_texture(Square_image, weak_aqua_tint, 0, 160.0, 110.0, 60.0, 10.0);
			pos_x = 40.0;
			pos_y = 120.0;
			bar_x[0] = 40.0 + 9.5 * cam_contrast_mode;
			bar_x[1] = 40.0 + 19.0 * cam_white_balance_mode;
			bar_x[2] = 180.0 + 47.5 * cam_lens_correction_mode;
			bar_x[3] = 180.0 + 19.0 * cam_exposure_mode;
			for (int i = 0; i < 4; i++)
			{
				Draw(cam_msg[7 + i], 0, (pos_x + 2.5), pos_y, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
				Draw_texture(Square_image, weak_red_tint, 0, pos_x, (pos_y + 17.5), 100.0, 5.0);
				Draw_texture(Square_image, white_or_black_tint, 0, bar_x[i], (pos_y + 10.0), 5.0, 20.0);

				pos_y += 30.0;
				if (150.0 < pos_y)
				{
					pos_x = 180.0;
					pos_y = 120.0;
				}
			}
		}
		else if (cam_selected_menu_mode == CAM_MENU_ADVANCED_1)
		{
			Draw_texture(Square_image, red_tint, 0, 220.0, 110.0, 60.0, 10.0);

			Draw(cam_msg[11], 0, 42.5, 120.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw_texture(Square_image, weak_aqua_tint, 0, 40.0, 130.0, 40.0, 20.0);
			Draw_texture(Square_image, weak_aqua_tint, 0, 100.0, 130.0, 40.0, 20.0);
			if (cam_encode_format_mode == 0)
			{
				Draw(cam_msg[12], 0, 42.5, 130.0, 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
				Draw(cam_msg[13], 0, 102.5, 130.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			}
			else if(cam_encode_format_mode == 1)
			{
				Draw(cam_msg[12], 0, 42.5, 130.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
				Draw(cam_msg[13], 0, 102.5, 130.0, 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
				Draw(cam_msg[14] + std::to_string(cam_selected_jpg_quality), 0, 42.5, 150.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
				Draw_texture(Square_image, weak_red_tint, 0, 41.0, 167.5, 100.0, 5.0);
				Draw_texture(Square_image, white_or_black_tint, 0, (40.0 + 1.0 * cam_selected_jpg_quality), 160.0, 5.0, 20.0);
			}

			Draw(cam_msg[15], 0, 182.5, 120.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			pos_x = 180.0;
			pos_y = 130.0;
			if (cam_noise_filter_mode == 1)//Noise filter
			{
				red[0] = 1.0;
				green[0] = 0.0;
				blue[0] = 0.0;
				alpha[0] = 1.0;
			}
			else
			{
				red[1] = 1.0;
				green[1] = 0.0;
				blue[1] = 0.0;
				alpha[1] = 1.0;
			}

			for (int i = 0; i < 2; i++)
			{
				Draw_texture(Square_image, weak_aqua_tint, 0, pos_x, pos_y, 40.0, 20.0);
				Draw(cam_msg[16 + i], 0, (pos_x + 2.5), pos_y, 0.4, 0.4, red[i], green[i], blue[i], alpha[i]);
				pos_x += 60.0;
			}

			Draw(cam_msg[18], 0, 182.5, 150.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);//Shutter sound
			Sem_set_color(text_red, text_green, text_blue, text_alpha, red, green, blue, alpha, 2);
			pos_x = 180.0;
			pos_y = 160.0;
			if (cam_shutter_sound_mode == 1)
			{
				red[0] = 1.0;
				green[0] = 0.0;
				blue[0] = 0.0;
				alpha[0] = 1.0;
			}
			else
			{
				red[1] = 1.0;
				green[1] = 0.0;
				blue[1] = 0.0;
				alpha[1] = 1.0;
			}

			for (int i = 0; i < 2; i++)
			{
				Draw_texture(Square_image, weak_aqua_tint, 0, pos_x, pos_y, 40.0, 20.0);
				Draw(cam_msg[16 + i], 0, (pos_x + 2.5), pos_y, 0.4, 0.4, red[i], green[i], blue[i], alpha[i]);
				pos_x += 60.0;
			}
		}
		for (int i = 0; i < 4; i++)
			Draw(cam_msg[i + 21], 0, 40.0 + (i * 60.0), 110.0, 0.35, 0.35, text_red, text_green, text_blue, text_alpha);

		if (cam_encode_request)
		{
			Draw_texture(Square_image, aqua_tint, 0, 40.0, 80.0, 240.0, 50.0);

			if (Sem_query_lang() == "jp")
				Draw(cam_format_name_list[cam_encode_format_mode] + cam_msg[19] + cam_msg[20], 0, 52.5, 100.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
			else
				Draw(cam_msg[19] + cam_format_name_list[cam_encode_format_mode] + cam_msg[20], 0, 52.5, 100.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		}
		Draw_bot_ui();
		Draw_touch_pos();

		Draw_apply_draw();
		cam_need_reflesh = false;
	}
	else
		gspWaitForVBlank();

	pos_x = 0.0;
	pos_y = 0.0;
	if(!key.h_touch)
	{
		for(int i = 0; i < 4; i++)
			cam_button_selected[i] = false;
	}

	if (Err_query_error_show_flag())
	{
		if (key.p_a || (key.p_touch && key.touch_x >= 150 && key.touch_x <= 169 && key.touch_y >= 150 && key.touch_y <= 169))
			Err_set_error_show_flag(false);
		else if(key.p_x || (key.p_touch && key.touch_x >= 200 && key.touch_x <= 239 && key.touch_y >= 150 && key.touch_y <= 169))
			Err_save_error();
	}
	else
	{
		if (key.p_start || (key.p_touch && key.touch_x >= 110 && key.touch_x <= 230 && key.touch_y >= 220 && key.touch_y <= 240))
			Cam_suspend();
		else if (!cam_encode_request)
		{
			if (key.p_a)
				cam_take_request = true;
			else if(key.p_b)
				cam_take_video_request = !cam_take_video_request;
			else if (key.p_touch && key.touch_x >= 40 && key.touch_x <= 279 && key.touch_y >= 85 && key.touch_y <= 104)
			{
				for (int i = 0; i < 3; i++)
				{
					if (key.p_touch && key.touch_x >= 40 + (i * 85) && key.touch_x <= 109 + (i * 85) && key.touch_y >= 85 && key.touch_y <= 104)
					{
						if(cam_request_camera_mode != i)
						{
							cam_request_camera_mode = i;
							cam_change_camera_request = true;
						}
						break;
					}
				}
			}
			else
			{
				for (int i = 0; i < 4; i++)
				{
					if (key.p_touch && key.touch_x >= 40.0 + (60.0 * i) && key.touch_x <= 99.0 + (60.0 * i) && key.touch_y >= 110.0 && key.touch_y <= 119.0)
					{
						cam_selected_menu_mode = i;
						break;
					}
				}
			}

			if (cam_selected_menu_mode == CAM_MENU_RESOLUTION)
			{
				pos_x = 40.0;
				pos_y = 120.0;
				for (int i = 0; i < 9; i++)
				{
					if (i != cam_request_resolution_mode && key.p_touch && key.touch_x >= pos_x && key.touch_x <= pos_x + 59 && key.touch_y >= pos_y && key.touch_y <= pos_y + 19)
					{
						if (i == 4)
						{
							random_num = 4;
							srand(time(NULL));
							while (random_num == 4)
							{
								random_num = rand() % 9;
								usleep(10000);
							}
							i = random_num;
						}
						cam_request_resolution_mode = i;
						cam_change_resolution_request = true;
						break;
					}

					if (pos_y + 30.0 > 180.0)
					{
						pos_x += 90.0;
						pos_y = 120.0;
					}
					else
						pos_y += 30.0;
				}
			}
			else if (cam_selected_menu_mode == CAM_MENU_FPS)
			{
				pos_x = 40.0;
				pos_y = 120.0;
				for (int i = 0; i < 15; i++)
				{
					if (i != cam_request_fps_mode && key.p_touch && key.touch_x >= pos_x && key.touch_x <= pos_x + 29 && key.touch_y >= pos_y && key.touch_y <= pos_y + 19)
					{
						if (i == 13 || i == 14)
						{
							srand(time(NULL));
							if (i == 13)
							{
								random_num = 13;
								while (random_num == 13 || random_num == 0 || random_num == 3 || random_num == 4 || random_num == 5
									|| random_num == 6 || random_num == 8)
								{
									random_num = rand() % 13;
									usleep(10000);
								}
							}
							else if(i == 14)
							{
								random_num = 14;
								while (random_num == 14 || random_num == 1 || random_num == 2 || random_num == 7 || random_num == 9
									|| random_num == 10 || random_num == 11 || random_num == 12)
								{
									random_num = rand() % 13;
									usleep(10000);
								}
							}
							i = random_num;
						}
						cam_request_fps_mode = i;
						cam_change_fps_request = true;
						break;
					}

					if (pos_y + 30.0 > 180.0)
					{
						pos_x += 50.0;
						pos_y = 120.0;
					}
					else
						pos_y += 30.0;
				}
			}
			else if (cam_selected_menu_mode == CAM_MENU_ADVANCED_0)
			{
				if (key.p_touch && key.touch_x >= 40 && key.touch_x <= 149 && key.touch_y >= 130 && key.touch_y <= 149)
					cam_button_selected[0] = true;
				else if (key.p_touch && key.touch_x >= 40 && key.touch_x <= 149 && key.touch_y >= 160 && key.touch_y <= 179)
					cam_button_selected[1] = true;
				else if (key.p_touch && key.touch_x >= 180 && key.touch_x <= 279 && key.touch_y >= 130 && key.touch_y <= 149)
					cam_button_selected[2] = true;
				else if (key.p_touch && key.touch_x >= 180 && key.touch_x <= 279 && key.touch_y >= 160 && key.touch_y <= 179)
					cam_button_selected[3] = true;

				pos_x = 35.0;
				for (int i = 0; i < 11; i++)
				{
					if (i != cam_request_contrast_mode && key.h_touch && key.touch_x >= pos_x && key.touch_x <= pos_x + 9 && cam_button_selected[0])
					{
						cam_request_contrast_mode = i;
						cam_change_contrast_request = true;
						break;
					}
					pos_x += 10.0;
				}
				pos_x = 30.0;
				for (int i = 0; i < 6; i++)
				{
					if (i != cam_request_white_balance_mode && key.h_touch && key.touch_x >= pos_x && key.touch_x <= pos_x + 19 && cam_button_selected[1])
					{
						cam_request_white_balance_mode = i;
						cam_change_white_balance_request = true;
						break;
					}
					pos_x += 20.0;
				}
				pos_x = 155.0;
				for (int i = 0; i < 3; i++)
				{
					if (i != cam_request_lens_correction_mode && key.h_touch && key.touch_x >= pos_x && key.touch_x <= pos_x + 49 && cam_button_selected[2])
					{
						cam_request_lens_correction_mode = i;
						cam_change_lens_correction_request = true;
						break;
					}
					pos_x += 50.0;
				}
				pos_x = 170.0;
				for (int i = 0; i < 6; i++)
				{
					if (i != cam_request_exposure_mode && key.h_touch && key.touch_x >= pos_x && key.touch_x <= pos_x + 19 && cam_button_selected[3])
					{
						cam_request_exposure_mode = i;
						cam_change_exposure_request = true;
						break;
					}
					pos_x += 20.0;
				}
			}
			else if (cam_selected_menu_mode == CAM_MENU_ADVANCED_1)
			{
				if (key.p_touch && key.touch_x >= 40 && key.touch_x <= 79 && key.touch_y >= 130 && key.touch_y <= 149)
					cam_encode_format_mode = 0;
				else if (key.p_touch && key.touch_x >= 100 && key.touch_x <= 139 && key.touch_y >= 130 && key.touch_y <= 149)
					cam_encode_format_mode = 1;
				else if (key.p_touch && key.touch_x >= 180 && key.touch_x <= 219 && key.touch_y >= 130 && key.touch_y <= 149)
				{
					cam_request_noise_filter_mode = 1;
					cam_change_noise_filter_request = true;
				}
				else if (key.p_touch && key.touch_x >= 240 && key.touch_x <= 279 && key.touch_y >= 130 && key.touch_y <= 149)
				{
					cam_request_noise_filter_mode = 0;
					cam_change_noise_filter_request = true;
				}
				else if (key.p_touch && key.touch_x >= 180 && key.touch_x <= 219 && key.touch_y >= 160 && key.touch_y <= 179)
					cam_shutter_sound_mode = 1;
				else if (key.p_touch && key.touch_x >= 240 && key.touch_x <= 279 && key.touch_y >= 160 && key.touch_y <= 179)
					cam_shutter_sound_mode = 0;

				if (cam_encode_format_mode == 1)
				{
					if (key.p_touch && key.touch_x >= 40 && key.touch_x <= 139 && key.touch_y >= 160 && key.touch_y <= 179)
						cam_button_selected[0] = true;

					for (int i = 0; i < 100; i++)
					{
						if (key.h_touch && key.touch_x == 40 + i && cam_button_selected[0])
						{
							cam_selected_jpg_quality = i + 1;
							break;
						}
					}
				}
			}
		}
	}
}

Result_with_string Cam_load_msg(std::string lang)
{
	int num_of_msg[2] = { CAM_NUM_OF_OPTION_MSG, CAM_NUM_OF_MSG, };
	int msg_list[2] = { CAM_OPTION_MSG, CAM_MSG, };
	u8* fs_buffer = NULL;
	u32 read_size;
	std::string setting_data[128];
	std::string file_name[2] = { "cam_options.txt", "cam_" + lang + ".txt",};
	Result_with_string result;
	fs_buffer = (u8*)malloc(0x2000);

	for(int i = 0; i < 2; i++)
	{
		result = File_load_from_rom(file_name[i], fs_buffer, 0x2000, &read_size, "romfs:/gfx/msg/");
		if (result.code != 0)
		{
			free(fs_buffer);
			return result;
		}

		result = Sem_parse_file((char*)fs_buffer, num_of_msg[i], setting_data);
		if (result.code != 0)
		{
			free(fs_buffer);
			return result;
		}

		for (int k = 0; k < num_of_msg[i]; k++)
			Cam_set_msg(k, msg_list[i], setting_data[k]);
	}

	free(fs_buffer);
	return result;
}

void Cam_init(void)
{
	Log_log_save(cam_init_string , "Initializing...", 1234567890, FORCE_DEBUG);
	bool failed = false;
	bool new_3ds = false;
	int log_num;
	u8* fs_buffer;
	u32 read_size;
	std::string data[11];
	Result_with_string result;
	fs_buffer = (u8*)malloc(0x1000);

	cam_mic_buffer[0] = (u8*)memalign(0x1000, cam_mic_buffer_size);
	if (cam_mic_buffer[0] == NULL)
	{
		Err_set_error_message("Out of memory.", "Couldn't allocate memory.", cam_init_string, OUT_OF_MEMORY);
		Err_set_error_show_flag(true);
		failed = true;
	}

	Draw_progress("[Cam] Initializing camera...");
	log_num = Log_log_save(cam_init_string , "camInit()...", 1234567890, FORCE_DEBUG);
	result.code = camInit();
	if (result.code == 0)
		Log_log_add(log_num, Err_query_template_summary(0), result.code, FORCE_DEBUG);
	else
	{
		failed = true;
		Err_set_error_message("camInit() failed. ", "", cam_init_string , result.code);
		Err_set_error_show_flag(true);
		Log_log_add(log_num, Err_query_template_summary(-1024), result.code, FORCE_DEBUG);
	}

	Draw_progress("[Cam] Initializing mic...");
	if (!failed)
	{
		log_num = Log_log_save(cam_init_string, "micInit()...", 1234567890, FORCE_DEBUG);
		result.code = micInit(cam_mic_buffer[0], cam_mic_buffer_size);
		if (result.code == 0)
		{
			MICU_SetAllowShellClosed(true);
			Log_log_add(log_num, Err_query_template_summary(0), result.code, FORCE_DEBUG);
		}
		else
		{
			failed = true;
			Err_set_error_message("micInit() failed.", "", cam_init_string, result.code);
			Err_set_error_show_flag(true);
			Log_log_add(log_num, Err_query_template_summary(-1024), result.code, FORCE_DEBUG);
		}
	}


	Draw_progress("[Cam] Loading settings...");
	memset(fs_buffer, 0x0, 0x1000);
	log_num = Log_log_save(cam_init_string, "File_load_from_file()...", 1234567890, FORCE_DEBUG);
	result = File_load_from_file("Cam_setting.txt", fs_buffer, 0x2000, &read_size, "/Line/");
	Log_log_add(log_num, result.string, result.code, FORCE_DEBUG);
	if (result.code == 0)
	{
		result = Sem_parse_file((char*)fs_buffer, 11, data);
		if(result.code == 0)
		{
			for(int i = 0; i < 11; i++)
			{
				if (!(data[i] == "") && std::all_of(data[i].cbegin(), data[i].cend(), isdigit))
				{
					if(i == 0)
						cam_camera_mode = std::stoi(data[i]);
					else if(i == 1)
						cam_resolution_mode = std::stoi(data[i]);
					else if(i == 2)
						cam_fps_mode = std::stoi(data[i]);
					else if(i == 3)
						cam_contrast_mode = std::stoi(data[i]);
					else if(i == 4)
						cam_white_balance_mode = std::stoi(data[i]);
					else if(i == 5)
						cam_lens_correction_mode = std::stoi(data[i]);
					else if(i == 6)
						cam_exposure_mode = std::stoi(data[i]);
					else if(i == 7)
						cam_encode_format_mode= std::stoi(data[i]);
					else if(i == 8)
						cam_noise_filter_mode = std::stoi(data[i]);
					else if(i == 9)
						cam_selected_jpg_quality = std::stoi(data[i]);
					else if(i == 10)
						cam_shutter_sound_mode = std::stoi(data[i]);
				}
			}
		}
	}
	if(!(cam_camera_mode >= 0 && cam_camera_mode <= 2))
		cam_camera_mode = 0;
	if(!(cam_resolution_mode >= 0 && cam_resolution_mode <= 8) || cam_resolution_mode == 4)
		cam_resolution_mode = 2;
	if(!(cam_fps_mode >= 0 && cam_fps_mode <= 12))
		cam_fps_mode = 8;
	if(!(cam_contrast_mode >= 0 && cam_contrast_mode <= 10))
		cam_contrast_mode = 5;
	if(!(cam_white_balance_mode >= 0 && cam_white_balance_mode <= 5))
		cam_white_balance_mode = 0;
	if(!(cam_lens_correction_mode >= 0 && cam_lens_correction_mode <= 2))
		cam_lens_correction_mode = 0;
	if(!(cam_exposure_mode >= 0 && cam_exposure_mode <= 5))
		cam_exposure_mode = 0;
	if(!(cam_encode_format_mode >= 0 && cam_encode_format_mode <= 1))
		cam_encode_format_mode = 1;
	if(!(cam_noise_filter_mode >= 0 && cam_noise_filter_mode <= 1))
		cam_noise_filter_mode = 1;
	if(!(cam_selected_jpg_quality >= 1 && cam_selected_jpg_quality <= 100))
		cam_selected_jpg_quality = 95;
	if(!(cam_shutter_sound_mode >= 0 && cam_shutter_sound_mode <= 1))
		cam_shutter_sound_mode = 0;

	cam_request_camera_mode = cam_camera_mode;
	cam_request_resolution_mode = cam_resolution_mode;
	cam_request_fps_mode = cam_fps_mode;
	cam_request_contrast_mode = cam_contrast_mode;
	cam_request_white_balance_mode = cam_white_balance_mode;
	cam_request_lens_correction_mode = cam_lens_correction_mode;
	cam_request_exposure_mode = cam_exposure_mode;

	Draw_progress("[Cam] Initializing camera...");
	if (!failed)
	{
		log_num = Log_log_save(cam_init_string, "Initializing camera...", 1234567890, FORCE_DEBUG);
		result = Cam_cam_init(cam_camera_mode, cam_resolution_mode, cam_noise_filter_mode, cam_exposure_mode, cam_white_balance_mode,
			cam_fps_mode, cam_contrast_mode, cam_lens_correction_mode, &cam_buffer_size);
		Log_log_add(log_num, result.string, result.code, FORCE_DEBUG);
		if (result.code != 0)
		{
			Err_set_error_message(result.string, result.error_description, cam_init_string , result.code);
			Err_set_error_show_flag(true);
		}
	}

	if (!failed)
	{
		for (int i = 0; i < 4; i++)
		{
			cam_capture_frame_buffer[i] = (u8*)linearAlloc(640 * 480 * 2);
			cam_capture_image[i].tex = (C3D_Tex*)linearAlloc(sizeof(C3D_Tex));
			cam_capture_image[i].subtex = (Tex3DS_SubTexture*)linearAlloc(sizeof(Tex3DS_SubTexture));

			if (cam_capture_frame_buffer[i] == NULL)
			{
				Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), cam_init_string, OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				Log_log_save(cam_init_string, Err_query_template_summary(OUT_OF_MEMORY), OUT_OF_MEMORY, FORCE_DEBUG);
				failed = true;
				break;
			}
		}
	}

	Draw_progress("[Cam] Starting threads...");
	if (!failed)
	{
		APT_CheckNew3DS(&new_3ds);
		cam_capture_thread_run = true;
		cam_encode_thread_run = true;
		cam_parse_thread_run = true;
		cam_capture_thread = threadCreate(Cam_capture_thread, (void*)(""), STACKSIZE, PRIORITY_REALTIME, 0, false);
		if(new_3ds)
		{
			cam_encode_thread = threadCreate(Cam_encode_thread, (void*)(""), STACKSIZE, PRIORITY_HIGH, 2, false);
			cam_parse_thread = threadCreate(Cam_parse_thread, (void*)(""), STACKSIZE, PRIORITY_HIGH, 0, false);
		}
		else
		{
			cam_encode_thread = threadCreate(Cam_encode_thread, (void*)(""), STACKSIZE, PRIORITY_HIGH, 0, false);
			cam_parse_thread = threadCreate(Cam_parse_thread, (void*)(""), STACKSIZE, PRIORITY_HIGH, 1, false);
		}
	}
	cam_display_img_num = -1;

	Cam_resume();
	cam_already_init = true;
	Log_log_save(cam_init_string , "Initialized", 1234567890, FORCE_DEBUG);
}

void Cam_exit(void)
{
	Log_log_save(cam_exit_string , "Exiting...", 1234567890, FORCE_DEBUG);
	u64 time_out = 10000000000;
	int log_num;
	std::string data =  "<0>" + std::to_string(cam_camera_mode) + "</0><1>" + std::to_string(cam_resolution_mode) + "</1><2>" + std::to_string(cam_fps_mode) + "</2>"
	+ "<3>" + std::to_string(cam_contrast_mode) + "</3><4>" + std::to_string(cam_white_balance_mode) + "</4><5>" + std::to_string(cam_lens_correction_mode) + "</5>"
	+ "<6>" + std::to_string(cam_exposure_mode) + "</6><7>" + std::to_string(cam_encode_format_mode) + "</7><8>" + std::to_string(cam_noise_filter_mode) + "</8>"
	+ "<9>" + std::to_string(cam_selected_jpg_quality) + "</9><10>" + std::to_string(cam_shutter_sound_mode) + "</10>";
	Result_with_string result;

	Draw_progress("[Cam] Exiting...");
	cam_already_init = false;
	cam_thread_suspend = false;
	cam_capture_thread_run = false;
	cam_encode_thread_run = false;
	cam_parse_thread_run = false;

	log_num = Log_log_save(cam_exit_string, "File_save_to_file()...", 1234567890, FORCE_DEBUG);
	result = File_save_to_file("Cam_setting.txt", (u8*)data.c_str(), data.length(), "/Line/", true);
	Log_log_add(log_num, result.string, result.code, FORCE_DEBUG);

	for (int i = 0; i < 3; i++)
	{
		log_num = Log_log_save(cam_exit_string, "threadJoin()" + std::to_string(i) + "/2...", 1234567890, FORCE_DEBUG);

		if (i == 0)
			result.code = threadJoin(cam_encode_thread, time_out);
		else if (i == 1)
			result.code = threadJoin(cam_parse_thread, time_out);
		else if (i == 2)
			result.code = threadJoin(cam_capture_thread, time_out);

		if (result.code == 0)
			Log_log_add(log_num, Err_query_template_summary(0), result.code, FORCE_DEBUG);
		else
			Log_log_add(log_num, Err_query_template_summary(-1024), result.code, FORCE_DEBUG);
	}

	threadFree(cam_encode_thread);
	threadFree(cam_parse_thread);
	threadFree(cam_capture_thread);

	camExit();

	for (int i = 0; i < 4; i++)
	{
		linearFree(cam_capture_frame_buffer[i]);
		linearFree(cam_capture_image[i].tex->data);
		linearFree(cam_capture_image[i].tex);
		linearFree((void*)cam_capture_image[i].subtex);
		cam_capture_frame_buffer[i] = NULL;
		cam_capture_image[i].tex->data = NULL;
		cam_capture_image[i].tex = NULL;
		cam_capture_image[i].subtex = NULL;
	}
}

void Cam_encode_thread(void* arg)
{
	Log_log_save(cam_encode_thread_string, "Thread started.", 1234567890, false);
	int log_num;
	int file_size;
	int stbi_result;
	int fps_value = 0;
	u8* cam_rgb888_buffer = NULL;
	u8* rgb565_buffer = NULL;
	u32 buffer_pos = 0;
	u32 buffer_offset = 0;
	u32 sample_size = 0;
	uint8_t* cam_png_buffer = NULL;
	std::string file_name = "";
	std::string extension = "";
	std::string dir_patch = "";
	Result_with_string result;

	//create directory
	File_save_to_file(".", NULL, 0, "/Line/dcim/", true);
	File_save_to_file(".", NULL, 0, "/Line/dcim/JPG/", true);
	File_save_to_file(".", NULL, 0, "/Line/dcim/PNG/", true);
	File_save_to_file(".", NULL, 0, "/Line/dcim/MP4/", true);

	while (cam_encode_thread_run)
	{
		if (cam_encode_request)
		{
			log_num = Log_log_save(cam_encode_thread_string, "APT_SetAppCpuTimeLimit()...", 1234567890, FORCE_DEBUG);
			result.code = APT_SetAppCpuTimeLimit(80);
			if (result.code == 0)
				Log_log_add(log_num, Err_query_template_summary(0), result.code, FORCE_DEBUG);
			else
				Log_log_add(log_num, Err_query_template_summary(-1024), result.code, FORCE_DEBUG);

			file_name = Menu_query_time(2) + "_";
			if(cam_encode_format_mode == 0)
			{
				extension = ".png";
				dir_patch = "/Line/dcim/PNG/" + Menu_query_time(1) + "/";
			}
			else if(cam_encode_format_mode == 1)
			{
				extension = ".jpg";
				dir_patch = "/Line/dcim/JPG/" + Menu_query_time(1) + "/";
			}

			for(int i = 0; i < 100; i++)
			{
				result = File_check_file_exist(file_name + std::to_string(i) + extension, dir_patch);
				if(result.code != 0)
				{
					file_name = file_name + std::to_string(i) + extension;
					break;
				}
			}

			cam_rgb888_buffer = (uint8_t*)malloc((Cam_convert_to_resolution(cam_resolution_mode, true) * Cam_convert_to_resolution(cam_resolution_mode, false) * 3));

			memset(cam_rgb888_buffer, 0x0, (Cam_convert_to_resolution(cam_resolution_mode, true) * Cam_convert_to_resolution(cam_resolution_mode, false) * 3));
			cam_png_buffer = (uint8_t*)malloc(0x80000);
			memset(cam_png_buffer, 0x0, 0x80000);
			if (cam_png_buffer == NULL)
			{
				Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), cam_encode_thread_string, OUT_OF_MEMORY);
				Log_log_save(cam_encode_thread_string, Err_query_template_summary(OUT_OF_MEMORY), OUT_OF_MEMORY, false);
				Err_set_error_show_flag(true);
			}
			else if (cam_capture_buffer != NULL)
			{
				Draw_rgb565_to_abgr888_rgb888(cam_capture_buffer, cam_rgb888_buffer, Cam_convert_to_resolution(cam_resolution_mode, true), Cam_convert_to_resolution(cam_resolution_mode, false), true);
				free(cam_capture_buffer);
				cam_capture_buffer = NULL;

				if (cam_encode_format_mode == 0)
				{
					log_num = Log_log_save(cam_encode_thread_string, "stbi_write_png_to_mem()...", 1234567890, false);
					cam_png_buffer = stbi_write_png_to_mem((const unsigned char*)cam_rgb888_buffer, 0, Cam_convert_to_resolution(cam_resolution_mode, true), Cam_convert_to_resolution(cam_resolution_mode, false), 3, &file_size);
					Log_log_add(log_num, "size : " + std::to_string(file_size / 1024) + "KB. ", 1234567890, false);
					if (cam_png_buffer != NULL)
					{
						log_num = Log_log_save(cam_encode_thread_string, "File_save_to_file()...", 1234567890, false);
						result = File_save_to_file(file_name, (u8*)cam_png_buffer, file_size, dir_patch, true);
						Log_log_add(log_num, result.string, result.code, false);
					}
				}
				else if (cam_encode_format_mode == 1)
				{
					log_num = Log_log_save(cam_encode_thread_string, "stbi_write_jpg_to_mem()...", 1234567890, false);
					stbi_result = stbi_write_jpg_to_mem((void*)cam_png_buffer, Cam_convert_to_resolution(cam_resolution_mode, true), Cam_convert_to_resolution(cam_resolution_mode, false), 3, (const void*)cam_rgb888_buffer, cam_selected_jpg_quality, &file_size);
					Log_log_add(log_num, "size : " + std::to_string(file_size / 1024) + "KB. ", 1234567890, false);
					if (stbi_result != 0)
					{
						log_num = Log_log_save(cam_encode_thread_string, "File_save_to_file()...", 1234567890, false);
						result = File_save_to_file(file_name, (u8*)cam_png_buffer, file_size, dir_patch, true);
						Log_log_add(log_num, result.string, result.code, false);
					}
				}

				free(cam_png_buffer);
				cam_png_buffer = NULL;
			}
			free(cam_rgb888_buffer);
			cam_rgb888_buffer = NULL;
			log_num = Log_log_save(cam_encode_thread_string, "APT_SetAppCpuTimeLimit()...", 1234567890, FORCE_DEBUG);
			result.code = APT_SetAppCpuTimeLimit(30);
			if (result.code == 0)
				Log_log_add(log_num, Err_query_template_summary(0), result.code, FORCE_DEBUG);
			else
				Log_log_add(log_num, Err_query_template_summary(-1024), result.code, FORCE_DEBUG);

			cam_encode_request = false;
		}
		else if(cam_encode_video_request)
		{
			if(cam_request_resolution_mode == 0 || cam_request_resolution_mode == 1)
				fps_value = 5;
			else if(cam_request_resolution_mode == 2 || cam_request_resolution_mode == 3)
				fps_value = 10;
			else if(cam_request_resolution_mode == 5)
				fps_value = 15;
			else if(cam_request_resolution_mode == 6)
				fps_value = 20;
			else if(cam_request_resolution_mode == 7 || cam_request_resolution_mode == 8)
				fps_value = 30;

			aptSetSleepAllowed(false);
			cam_mic_buffer[1] = (u8*)malloc(cam_mic_buffer_size);
			if (cam_mic_buffer[1] == NULL)
			{
				Err_set_error_message("[Error] Out of memory.", "Couldn't allocate memory.", cam_encode_thread_string, OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				Log_log_save(cam_encode_thread_string, "[Error] Out of memory.", OUT_OF_MEMORY, false);
				cam_take_video_request = false;
			}
			else
			{
				memset(cam_mic_buffer[1], 0x0, cam_mic_buffer_size);
				sample_size = micGetSampleDataSize();
				buffer_offset = 0;
				buffer_pos = 0;
			}

			log_num = Log_log_save(cam_encode_thread_string, "Util_create_output_file()...", result.code, false);
			result = Util_create_output_file("/test.mp4", UTIL_ENCODER_1);
			Log_log_add(log_num, result.string + result.error_description, result.code, false);

			if(result.code == 0)
			{
				log_num = Log_log_save(cam_encode_thread_string, "Util_init_video_encoder()...", result.code, false);
				result = Util_init_video_encoder(AV_CODEC_ID_MPEG4, Cam_convert_to_resolution(cam_resolution_mode, true), Cam_convert_to_resolution(cam_resolution_mode, false), fps_value, UTIL_ENCODER_1);
				Log_log_add(log_num, result.string + result.error_description, result.code, false);
			}
			else
				cam_take_video_request = false;

			if(result.code == 0)
			{
				/*log_num = Log_log_save(cam_encode_thread_string, "Util_init_audio_encoder()...", 1234567890, false);
				result = Util_init_audio_encoder(AV_CODEC_ID_AAC, 32730, 32000, 128000, UTIL_ENCODER_1);
				Log_log_add(log_num, result.string + result.error_description, result.code, false);*/
			}
			else
				cam_take_video_request = false;

			if(result.code == 0)
			{
				Log_log_save(cam_encode_thread_string, "Util_write_header()...", result.code, false);
				result = Util_write_header(UTIL_ENCODER_1);
				Log_log_add(log_num, result.string + result.error_description, result.code, false);
			}
			else
				cam_take_video_request = false;

			if(result.code == 0)
			{
				rgb565_buffer = (u8*)malloc(Cam_convert_to_resolution(cam_resolution_mode, true) * Cam_convert_to_resolution(cam_resolution_mode, false) * 2);
				if(rgb565_buffer == NULL)
				{
					Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), cam_encode_thread_string, OUT_OF_MEMORY);
					Err_set_error_show_flag(true);
					cam_take_video_request = false;
				}
				else
				{
					log_num = Log_log_save(cam_encode_thread_string, "MICU_StartSampling()...", 1234567890, false);
					result.code = MICU_StartSampling(MICU_ENCODING_PCM16_SIGNED, MICU_SAMPLE_RATE_32730, 0, sample_size, true);
					Log_log_add(log_num, "", result.code, false);
					if(result.code != 0)
					{
						Err_set_error_show_flag(true);
						Err_set_error_message("MICU_StartSampling() failed", "", cam_encode_thread_string, result.code);
						cam_take_video_request = false;
					}

					while(cam_take_video_request)
					{
						if(cam_encode_video_request)
						{
							memcpy_asm(rgb565_buffer, cam_capture_buffer, Cam_convert_to_resolution(cam_resolution_mode, true) * Cam_convert_to_resolution(cam_resolution_mode, false) * 2);
							cam_encode_video_request = false;
							
							log_num = Log_log_save(cam_capture_thread_string, "Util_encode_video()...", 1234567890, false);
							result = Util_encode_video(rgb565_buffer, UTIL_ENCODER_1);
							Log_log_add(log_num, result.string + result.error_description, result.code, false);
						}
						else
						{
							usleep(1000);
							if (buffer_pos != micGetLastSampleOffset())
							{
								if(buffer_pos > micGetLastSampleOffset())
								{
									/*log_num = Log_log_save(cam_encode_thread_string, "Util_encode_audio()...", 1234567890, false);
									result = Util_encode_audio(buffer_offset, cam_mic_buffer[1], UTIL_ENCODER_1);
									Log_log_add(log_num, result.string + result.error_description, result.code, false);*/
									if(result.code != 0)
										cam_take_video_request = false;

									buffer_offset = 0;
									buffer_pos = 0;
								}
								else
								{
									buffer_pos = micGetLastSampleOffset();
									memcpy((void*)(cam_mic_buffer[1] + buffer_offset), (void*)(cam_mic_buffer[0] + buffer_offset), (buffer_pos - buffer_offset));
									buffer_offset += (buffer_pos - buffer_offset);
								}
							}
						}
					}
				}
			}
			log_num = Log_log_save(cam_encode_thread_string, "MICU_StopSampling()...", 1234567890, false);
			result.code = MICU_StopSampling();
			Log_log_add(log_num, "", result.code, false);

			free(rgb565_buffer);
			free(cam_mic_buffer[1]);
			rgb565_buffer = NULL;
			cam_mic_buffer[1] = NULL;
			Util_close_output_file(UTIL_ENCODER_1);
			Util_exit_video_encoder(UTIL_ENCODER_1);
			//Util_exit_audio_encoder(UTIL_ENCODER_1);
			aptSetSleepAllowed(true);
			cam_encode_video_request = false;
		}
		else
			usleep(ACTIVE_THREAD_SLEEP_TIME);

		while (cam_thread_suspend)
			usleep(INACTIVE_THREAD_SLEEP_TIME);
	}
	Log_log_save(cam_encode_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Cam_capture_thread(void* arg)
{
	Log_log_save(cam_capture_thread_string, "Thread started.", 1234567890, false);
	bool out_left = false;
	int log_num;
	int fps_value = 10;
	int buffer_num[2] = { 2, 3, };
	float time = 0;
	Result_with_string result;
	TickCounter stopwatch;
	osTickCounterStart(&stopwatch);

	while (cam_capture_thread_run)
	{
		if (cam_take_request && !cam_encode_request)
		{
			log_num = Log_log_save(cam_capture_thread_string , "Cam_take_picture()...", 1234567890, false);
			cam_capture_buffer = (u8*)malloc(Cam_convert_to_resolution(cam_resolution_mode, true) * Cam_convert_to_resolution(cam_resolution_mode, false) * 2);
			if (cam_capture_buffer == NULL)
			{
				Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), cam_capture_thread_string, OUT_OF_MEMORY);
				Log_log_add(log_num, Err_query_template_summary(OUT_OF_MEMORY), OUT_OF_MEMORY, false);
				Err_set_error_show_flag(true);
			}
			else
			{
				if (cam_camera_mode == 1)
					out_left = true;
				else
					out_left = false;

				result = Cam_take_picture(cam_capture_buffer, (Cam_convert_to_resolution(cam_resolution_mode, true) * Cam_convert_to_resolution(cam_resolution_mode, false) * 2), out_left, cam_shutter_sound_mode);

				Log_log_add(log_num, result.string, result.code, false);
				if (result.code != 0 && result.code != 0x9401BFE)
				{
					Err_set_error_message(result.string, result.error_description, cam_capture_thread_string , result.code);
					Err_set_error_show_flag(true);
				}
			}
			cam_take_request = false;
			cam_encode_request = true;
		}
		else if (cam_take_video_request)
		{
			if(cam_request_resolution_mode == 0 || cam_request_resolution_mode == 1)
				fps_value = 5;
			else if(cam_request_resolution_mode == 2 || cam_request_resolution_mode == 3)
				fps_value = 10;
			else if(cam_request_resolution_mode == 5)
				fps_value = 15;
			else if(cam_request_resolution_mode == 6)
				fps_value = 20;
			else if(cam_request_resolution_mode == 7 || cam_request_resolution_mode == 8)
				fps_value = 30;

			osTickCounterUpdate(&stopwatch);
			while(cam_take_video_request)
			{				
				log_num = Log_log_save(cam_capture_thread_string , "Cam_take_picture()...", 1234567890, false);
				cam_capture_buffer = (u8*)malloc(Cam_convert_to_resolution(cam_resolution_mode, true) * Cam_convert_to_resolution(cam_resolution_mode, false) * 2);
				if (cam_capture_buffer == NULL)
				{
					Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), cam_capture_thread_string, OUT_OF_MEMORY);
					Log_log_add(log_num, Err_query_template_summary(OUT_OF_MEMORY), OUT_OF_MEMORY, false);
					Err_set_error_show_flag(true);
				}
				else
				{
					if (cam_camera_mode == 1)
						out_left = true;
					else
						out_left = false;

					result = Cam_take_picture(cam_capture_buffer, (Cam_convert_to_resolution(cam_resolution_mode, true) * Cam_convert_to_resolution(cam_resolution_mode, false) * 2), out_left, cam_shutter_sound_mode);
					Log_log_add(log_num, result.string, result.code, false);
					if (result.code != 0 && result.code != 0x9401BFE)
					{
						Err_set_error_message(result.string, result.error_description, cam_capture_thread_string , result.code);
						Err_set_error_show_flag(true);
					}
					else if(result.code == 0)
					{
						osTickCounterUpdate(&stopwatch);
						time = osTickCounterRead(&stopwatch);
						if(time < 1000.0 / fps_value)
							usleep(((1000.0 / fps_value) - time) * 1000);

						cam_encode_video_request = true;

						memcpy_asm(cam_capture_frame_buffer[buffer_num[cam_current_img_num]], cam_capture_buffer, (Cam_convert_to_resolution(cam_resolution_mode, true) * Cam_convert_to_resolution(cam_resolution_mode, false) * 2));
						//update preview
						if (cam_current_img_num == 0)
							cam_current_img_num = 1;
						else
							cam_current_img_num = 0;

						cam_fps++;
						cam_parse_request = true;

						while(cam_encode_video_request)
							usleep(500);
					}
				}
				free(cam_capture_buffer);
				cam_capture_buffer = NULL;
			}
		}
		else if (cam_change_resolution_request || cam_change_fps_request || cam_change_contrast_request || cam_change_white_balance_request
			|| cam_change_lens_correction_request || cam_change_camera_request || cam_change_exposure_request || cam_change_noise_filter_request)
		{
			if (cam_change_resolution_request)
			{
				log_num = Log_log_save(cam_capture_thread_string, "Cam_set_capture_size()...", 1234567890, false);
				result = Cam_set_capture_size(Cam_convert_to_resolution(cam_request_resolution_mode, true), Cam_convert_to_resolution(cam_request_resolution_mode, false), &cam_buffer_size);
			}
			else if (cam_change_fps_request)
			{
				log_num = Log_log_save(cam_capture_thread_string, "Cam_set_capture_fps()...", 1234567890, false);
				result = Cam_set_capture_fps(cam_request_fps_mode);
			}
			else if (cam_change_contrast_request)
			{
				log_num = Log_log_save(cam_capture_thread_string, "CAMU_SetContrast()...", 1234567890, false);
				result = Cam_set_capture_contrast(cam_request_contrast_mode);
			}
			else if (cam_change_white_balance_request)
			{
				log_num = Log_log_save(cam_capture_thread_string, "Cam_set_capture_white_balance()...", 1234567890, false);
				result = Cam_set_capture_white_balance(cam_request_white_balance_mode);
			}
			else if (cam_change_lens_correction_request)
			{
				log_num = Log_log_save(cam_capture_thread_string, "Cam_set_capture_lens_correction()...", 1234567890, false);
				result = Cam_set_capture_lens_correction(cam_request_lens_correction_mode);
			}
			else if (cam_change_camera_request)
			{
				log_num = Log_log_save(cam_capture_thread_string, "Cam_set_capture_camera()...", 1234567890, false);
				result = Cam_set_capture_camera(cam_request_camera_mode, Cam_convert_to_resolution(cam_resolution_mode, true), Cam_convert_to_resolution(cam_resolution_mode, false), &cam_buffer_size);
			}
			else if (cam_change_exposure_request)
			{
				log_num = Log_log_save(cam_capture_thread_string, "Cam_set_capture_exposure()...", 1234567890, false);
				result = Cam_set_capture_exposure(cam_request_exposure_mode);
			}
			else if (cam_change_noise_filter_request)
			{
				log_num = Log_log_save(cam_capture_thread_string, "Cam_set_capture_noise_filter()...", 1234567890, false);
				result = Cam_set_capture_noise_filter(cam_request_noise_filter_mode);
			}
			else
				log_num = 0;

			Log_log_add(log_num, result.string, result.code, false);

			if (result.code != 0)
			{
				cam_request_resolution_mode = cam_resolution_mode;
				Err_set_error_message(result.string, result.error_description, cam_capture_thread_string, result.code);
				Err_set_error_show_flag(true);
			}
			else
			{
				if (cam_change_resolution_request)
				{
					cam_display_img_num = -1;
					cam_resolution_mode = cam_request_resolution_mode;
				}
				else if (cam_change_fps_request)
					cam_fps_mode = cam_request_fps_mode;
				else if (cam_change_contrast_request)
					cam_contrast_mode = cam_request_contrast_mode;
				else if (cam_change_white_balance_request)
					cam_white_balance_mode = cam_request_white_balance_mode;
				else if (cam_change_lens_correction_request)
					cam_lens_correction_mode = cam_request_lens_correction_mode;
				else if (cam_change_camera_request)
					cam_camera_mode = cam_request_camera_mode;
				else if (cam_change_exposure_request)
					cam_exposure_mode = cam_request_exposure_mode;
				else if (cam_change_noise_filter_request)
					cam_noise_filter_mode = cam_request_noise_filter_mode;
			}

			if (cam_change_resolution_request)
				cam_change_resolution_request = false;
			else if (cam_change_fps_request)
				cam_change_fps_request = false;
			else if (cam_change_contrast_request)
				cam_change_contrast_request = false;
			else if (cam_change_white_balance_request)
				cam_change_white_balance_request = false;
			else if (cam_change_lens_correction_request)
				cam_change_lens_correction_request = false;
			else if (cam_change_camera_request)
				cam_change_camera_request = false;
			else if (cam_change_exposure_request)
				cam_change_exposure_request = false;
			else if (cam_change_noise_filter_request)
				cam_change_noise_filter_request = false;
		}
		else
		{
			if (cam_camera_mode == 1)
				out_left = true;
			else
				out_left = false;

			result = Cam_take_picture(cam_capture_frame_buffer[buffer_num[cam_current_img_num]], (Cam_convert_to_resolution(cam_resolution_mode, true) * Cam_convert_to_resolution(cam_resolution_mode, false) * 2), out_left, false);
			if (result.code == 0)
			{
				if (cam_current_img_num == 0)
					cam_current_img_num = 1;
				else
					cam_current_img_num = 0;

				cam_fps++;
				cam_parse_request = true;
			}
			else
			{
				Log_log_save(cam_capture_thread_string, result.string, result.code, false);
				usleep(100);
			}
		}

		while (cam_thread_suspend)
			usleep(INACTIVE_THREAD_SLEEP_TIME);
	}

	result.code = CAMU_Activate(SELECT_NONE);

	Log_log_save(cam_capture_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Cam_parse_thread(void* arg)
{
	Log_log_save(cam_parse_thread_string, "Thread started.", 1234567890, false);
	int previous_num = 0;
	int processing_num[2] = { 0, 2, };
	int tex_size = 512;
	int width = 0;
	int height = 0;
	Result_with_string result;

	while (cam_parse_thread_run)
	{
		if (cam_parse_request)
		{
			cam_parse_request = false;

			if (previous_num!= cam_current_img_num)
			{
				previous_num = cam_current_img_num;
				width = Cam_convert_to_resolution(cam_resolution_mode, true);
				height = Cam_convert_to_resolution(cam_resolution_mode, false);

				if (cam_current_img_num == 0)
				{
					//unstable!
					//GX_TextureCopy((u32*)cam_capture_frame_buffer[3], 0, (u32*)cam_capture_frame_buffer[0], 0, (u32)(width * height * 2), 0x8);
					//GSPGPU_FlushDataCache(cam_capture_frame_buffer[3], (width * height * 2));
					//GSPGPU_InvalidateDataCache(cam_capture_frame_buffer[0], (width * height * 2));

					memcpy_asm(cam_capture_frame_buffer[0], cam_capture_frame_buffer[3], (width * height * 2));
				}
				else
				{
					//unstable!
					//GX_TextureCopy((u32*)cam_capture_frame_buffer[2], 0, (u32*)cam_capture_frame_buffer[1], 0, (u32)(width * height * 2), 0x8);
					//GSPGPU_FlushDataCache(cam_capture_frame_buffer[2], (width * height * 2));
					//GSPGPU_InvalidateDataCache(cam_capture_frame_buffer[1], (width * height * 2));

					memcpy_asm(cam_capture_frame_buffer[1], cam_capture_frame_buffer[2], (width * height * 2));
				}
				gspWaitForPPF();

				if (cam_current_img_num == 0)
				{
					processing_num[0] = 0;
					processing_num[1] = 0;
				}
				else
				{
					processing_num[0] = 1;
					processing_num[1] = 2;
				}

				for (int i = 0; i < 2; i++)
				{
					linearFree(cam_capture_image[processing_num[1] + i].tex->data);
					cam_capture_image[processing_num[1] + i].tex->data = NULL;
				}

				if(cam_resolution_mode >= 0 && cam_resolution_mode <= 5)
					tex_size = 512;
				else if(cam_resolution_mode >= 6)
					tex_size = 256;

				result = Draw_create_texture(cam_capture_image[processing_num[1]].tex, (Tex3DS_SubTexture*)cam_capture_image[processing_num[1]].subtex, cam_capture_frame_buffer[processing_num[0]], (width * height * 2), width, height, 2, 0, 0, tex_size, tex_size, GPU_RGB565);
				if(cam_resolution_mode == 0)
					result = Draw_create_texture(cam_capture_image[processing_num[1] + 1].tex, (Tex3DS_SubTexture*)cam_capture_image[processing_num[1] + 1].subtex, cam_capture_frame_buffer[processing_num[0]], (width * height * 2), width, height, 2, 512, 0, 128, 512, GPU_RGB565);
	
				if(cam_resolution_mode != 0 && cam_resolution_mode != 1)
					C3D_TexSetFilter(cam_capture_image[processing_num[1]].tex, GPU_NEAREST, GPU_NEAREST);


				if (result.code == 0)
				{
					if (cam_current_img_num == 0)
						cam_display_img_num = 1;
					else
						cam_display_img_num = 0;
				}
				else if(result.code != WRONG_PARSING_POS)
				{
					Err_set_error_message(result.string, result.error_description, cam_parse_thread_string, result.code);
					Err_set_error_show_flag(true);
					usleep(1000000);
				}
			}
		}
		else
			usleep(3000);

		while (cam_thread_suspend)
			usleep(INACTIVE_THREAD_SLEEP_TIME);
	}
	
	Log_log_save(cam_parse_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

bool Cam_query_init_flag(void)
{
	return cam_already_init;
}

bool Cam_query_running_flag(void)
{
	return cam_main_run;
}

void Cam_resume(void)
{
	cam_thread_suspend = false;
	cam_main_run = true;
	cam_need_reflesh = true;
	Menu_suspend();
}

void Cam_suspend(void)
{
	cam_thread_suspend = true;
	cam_main_run = false;
	Menu_resume();
}

int Cam_convert_to_resolution(int resolution_mode, bool width)
{
	if (resolution_mode == 0)
	{
		if (width)
			return 640;
		else
			return 480;
	}
	else if (resolution_mode == 1)
	{
		if (width)
			return 512;
		else
			return 384;
	}
	else if (resolution_mode == 2)
	{
		if (width)
			return 400;
		else
			return 240;
	}
	else if (resolution_mode == 3)
	{
		if (width)
			return 352;
		else
			return 288;
	}
	else if (resolution_mode == 5)
	{
		if (width)
			return 320;
		else
			return 240;
	}
	else if (resolution_mode == 6)
	{
		if (width)
			return 256;
		else
			return 192;
	}
	else if (resolution_mode == 7)
	{
		if (width)
			return 176;
		else
			return 144;
	}
	else if (resolution_mode == 8)
	{
		if (width)
			return 160;
		else
			return 120;
	}
	else
		return -1;
}

int Cam_query_framerate(void)
{
	return cam_fps;
}

void Cam_reset_framerate(void)
{
	cam_fps = 0;
}

void Cam_set_msg(int msg_num, int msg_type, std::string msg)
{
	if (msg_type == CAM_MSG && msg_num >= 0 && msg_num < CAM_NUM_OF_MSG)
		cam_msg[msg_num] = msg;
	else if (msg_type == CAM_OPTION_MSG && msg_num >= 0 && msg_num < CAM_NUM_OF_OPTION_MSG)
	{
		if (msg_num >= 0 && msg_num <= 8)
			cam_resolution_list[msg_num] = msg;
		else if (msg_num >= 9 && msg_num <= 14)
			cam_exposure_list[msg_num - 9] = msg;
		else if (msg_num >= 15 && msg_num <= 17)
			cam_lens_correction_list[msg_num - 15] = msg;
		else if (msg_num >= 18 && msg_num <= 23)
			cam_white_balance_list[msg_num - 18] = msg;
		else if (msg_num >= 24 && msg_num <= 34)
			cam_contrast_list[msg_num - 24] = msg;
		else if (msg_num >= 35 && msg_num <= 49)
			cam_framelate_list[msg_num - 35] = msg;
		else if (msg_num >= 50 && msg_num <= 51)
			cam_format_name_list[msg_num - 50] = msg;
	}
}

Result_with_string Cam_set_capture_camera(int camera_num, int width, int height, u32* out_buffer_size)
{
	int camera = 0;
	Result_with_string result;

	result = Cam_set_capture_size(width, height, out_buffer_size);

	if (result.code == 0)
	{
		CAMU_Activate(SELECT_NONE);

		switch (camera_num)
		{
			case 0:
				camera = SELECT_OUT1;
			break;
			case 1:
				camera = SELECT_OUT2;
			break;
			case 2:
				camera = SELECT_IN1;
			break;
			default:
				result.code = INVALID_ARG;
		}

		if(result.code != INVALID_ARG)
			result.code = CAMU_Activate(camera);

		if (result.code != 0)
			result.string = "[Error] CAMU_Activate failed. ";
	}
	return result;
}

Result_with_string Cam_set_capture_contrast(int contrast_num)
{
	CAMU_Contrast contrast = CONTRAST_PATTERN_01;
	Result_with_string result;

	switch(contrast_num)
	{
		case 0:
			contrast = CONTRAST_PATTERN_01;
			break;
		case 1:
			contrast = CONTRAST_PATTERN_02;
			break;
  	case 2:
			contrast = CONTRAST_PATTERN_03;
			break;
		case 3:
			contrast = CONTRAST_PATTERN_04;
			break;
		case 4:
			contrast = CONTRAST_PATTERN_05;
			break;
		case 5:
			contrast = CONTRAST_PATTERN_06;
			break;
		case 6:
			contrast = CONTRAST_PATTERN_07;
			break;
		case 7:
			contrast = CONTRAST_PATTERN_08;
			break;
		case 8:
			contrast = CONTRAST_PATTERN_09;
			break;
		case 9:
			contrast = CONTRAST_PATTERN_10;
			break;
		case 10:
			contrast = CONTRAST_PATTERN_11;
			break;
		default:
			result.code = INVALID_ARG;
	}

	if(result.code != INVALID_ARG)
		result.code = CAMU_SetContrast(SELECT_ALL, contrast);

	if (result.code != 0)
		result.string = "[Error] CAMU_SetContrast failed. ";

	return result;
}

Result_with_string Cam_set_capture_exposure(int exposure_num)
{
	Result_with_string result;

	if (exposure_num >= 0 && exposure_num <= 5)
		result.code = CAMU_SetExposure(SELECT_ALL, (s8)exposure_num);
	else
		result.code = INVALID_ARG;

	if (result.code != 0)
		result.string = "[Error] CAMU_SetExposure failed. ";

	return result;
}

Result_with_string Cam_set_capture_fps(int fps_num)
{
	CAMU_FrameRate framerate = FRAME_RATE_15;
	Result_with_string result;

	switch (fps_num)
	{
		case 0:
			framerate = FRAME_RATE_15;
			break;
		case 1:
			framerate = FRAME_RATE_15_TO_5;
			break;
		case 2:
			framerate = FRAME_RATE_15_TO_2;
			break;
		case 3:
			framerate = FRAME_RATE_10;
			break;
		case 4:
			framerate = FRAME_RATE_8_5;
			break;
		case 5:
			framerate = FRAME_RATE_5;
			break;
		case 6:
			framerate = FRAME_RATE_20;
			break;
		case 7:
			framerate = FRAME_RATE_20_TO_5;
			break;
		case 8:
			framerate = FRAME_RATE_30;
			break;
		case 9:
			framerate = FRAME_RATE_30_TO_5;
			break;
		case 10:
			framerate = FRAME_RATE_15_TO_10;
			break;
		case 11:
			framerate = FRAME_RATE_20_TO_10;
			break;
		case 12:
			framerate = FRAME_RATE_30_TO_10;
			break;
		default:
			result.code = INVALID_ARG;
	}

	if (result.code != INVALID_ARG)
		result.code = CAMU_SetFrameRate(SELECT_ALL, framerate);

	if (result.code != 0)
		result.string = "[Error] CAMU_SetFrameRate failed. ";

	return result;
}

Result_with_string Cam_set_capture_lens_correction(int lens_correction_num)
{
	CAMU_LensCorrection lens_correction = LENS_CORRECTION_OFF;
	Result_with_string result;

	if (lens_correction_num == 0)
		lens_correction = LENS_CORRECTION_OFF;
	else if (lens_correction_num == 1)
		lens_correction = LENS_CORRECTION_ON_70;
	else if (lens_correction_num == 2)
		lens_correction = LENS_CORRECTION_ON_90;
	else
		result.code = INVALID_ARG;

	if (result.code != INVALID_ARG)
		result.code = CAMU_SetLensCorrection(SELECT_ALL, lens_correction);

	if (result.code != 0)
		result.string = "[Error] CAMU_SetLensCorrection failed. ";

	return result;
}

Result_with_string Cam_set_capture_noise_filter(int noise_filter_num)
{
	Result_with_string result;

	if (noise_filter_num >= 0 && noise_filter_num <= 1)
		result.code = CAMU_SetNoiseFilter(SELECT_ALL, noise_filter_num);
	else
		result.code = INVALID_ARG;

	if (result.code != 0)
		result.string = "[Error] CAMU_SetNoiseFilter failed. ";

	return result;
}

Result_with_string Cam_set_capture_size(int width, int height, u32* out_buffer_size)
{
	CAMU_Size size = SIZE_VGA;
	bool failed = false;
	u32 buffer_size;
	Result_with_string result;

	if (width == 640 && height == 480)
		size = SIZE_VGA;
	else if (width == 512 && height == 384)
		size = SIZE_DS_LCDx4;
	else if (width == 400 && height == 240)
		size = SIZE_CTR_TOP_LCD;
	else if (width == 352 && height == 288)
		size = SIZE_CIF;
	else if (width == 320 && height == 240)
		size = SIZE_QVGA;
	else if (width == 256 && height == 192)
		size = SIZE_DS_LCD;
	else if (width == 176 && height == 144)
		size = SIZE_QCIF;
	else if (width == 160 && height == 120)
		size = SIZE_QQVGA;
	else
		result.code = INVALID_ARG;

	if (result.code != INVALID_ARG)
		result.code = CAMU_SetSize(SELECT_ALL, size, CONTEXT_BOTH);

	if (result.code != 0)
	{
		failed = true;
		result.string = "[Error] CAMU_SetSize failed. ";
	}

	if (!failed)
	{
		result.code = CAMU_GetMaxBytes(&buffer_size, width, height);
		*out_buffer_size = buffer_size;
		if (result.code != 0)
		{
			failed = true;
			result.string = "[Error] CAMU_GetMaxBytes failed. ";
		}
	}

	if (!failed)
	{
		result.code = CAMU_SetTransferBytes(PORT_BOTH, buffer_size, width, height);
		if (result.code != 0)
			result.string = "[Error] CAMU_SetTransferBytes failed. ";
	}

	return result;
}

Result_with_string Cam_set_capture_white_balance(int white_balance_num)
{
	CAMU_WhiteBalance white_balance = WHITE_BALANCE_AUTO;
	Result_with_string result;

	if (white_balance_num == 0)
		white_balance = WHITE_BALANCE_AUTO;
	else if (white_balance_num == 1)
		white_balance = WHITE_BALANCE_3200K;
	else if (white_balance_num == 2)
		white_balance = WHITE_BALANCE_4150K;
	else if (white_balance_num == 3)
		white_balance = WHITE_BALANCE_5200K;
	else if (white_balance_num == 4)
		white_balance = WHITE_BALANCE_6000K;
	else if (white_balance_num == 5)
		white_balance = WHITE_BALANCE_7000K;
	else
		result.code = INVALID_ARG;

	if (result.code != INVALID_ARG)
		result.code = CAMU_SetWhiteBalance(SELECT_ALL, white_balance);

	if (result.code != 0)
		result.string = "[Error] CAMU_SetWhiteBalance failed. ";

	return result;
}

Result_with_string Cam_take_picture(u8* capture_buffer, int size, bool out_cam_1, bool play_shutter_sound)
{
	bool failed = false;
	Handle receive = 0;
	Result_with_string result;

	memset(capture_buffer, 0x0, size);

	if (!failed)
	{
		result.code = CAMU_StartCapture(PORT_BOTH);
		if (result.code != 0)
		{
			result.string = "[Error] CAMU_StartCapture failed. ";
			failed = true;
		}
	}

	if (!failed)
	{
		if (out_cam_1)
			result.code = CAMU_SetReceiving(&receive, capture_buffer, PORT_CAM2, size, (s16)cam_buffer_size);
		else
			result.code = CAMU_SetReceiving(&receive, capture_buffer, PORT_CAM1, size, (s16)cam_buffer_size);
		if (result.code != 0)
		{
			result.string = "[Error] CAMU_SetReceiving failed. ";
			failed = true;
		}
	}

	if (!failed)
	{
		result.code = svcWaitSynchronization(receive, 500000000);
		if (result.code != 0)
		{
			result.string = "[Error] svcWaitSynchronization failed. ";
			failed = true;
		}
	}

	if (play_shutter_sound && !failed)
	{
		result.code = CAMU_PlayShutterSound(SHUTTER_SOUND_TYPE_NORMAL);
		if (result.code != 0)
		{
			result.string = "[Error] CAMU_PlayShutterSound failed. ";
			failed = true;
		}
	}

	svcCloseHandle(receive);

	return result;
}

Result_with_string Cam_cam_init(int camera_moide, int resolution_mode, int noise_filter_mode, int exposure_mode,
	int white_balance_mode, int fps_mode, int contrast_mode, int lens_correction_mode, u32* buffer_size)
{
	Result_with_string result;
	bool failed = false;

	result.code = CAMU_SetOutputFormat(SELECT_ALL, OUTPUT_RGB_565, CONTEXT_BOTH);
	if (result.code != 0)
		failed = true;

	if (!failed)
	{
		result = Cam_set_capture_noise_filter(noise_filter_mode);
		if (result.code != 0)
			failed = true;
	}

	if (!failed)
	{
		result = Cam_set_capture_exposure(exposure_mode);
		if (result.code != 0)
			failed = true;
	}

	if (!failed)
	{
		result = Cam_set_capture_white_balance(white_balance_mode);
		if (result.code != 0)
			failed = true;
	}

	if (!failed)
	{
		result.code = CAMU_SetPhotoMode(SELECT_ALL, PHOTO_MODE_NORMAL);
		if (result.code != 0)
			failed = true;
	}

	if (!failed)
	{
		result.code = CAMU_SetTrimming(PORT_BOTH, false);
		if (result.code != 0)
			failed = true;
	}

	if (!failed)
	{
		result = Cam_set_capture_fps(fps_mode);
		if (result.code != 0)
			failed = true;
	}

	if (!failed)
	{
		result = Cam_set_capture_contrast(contrast_mode);
		if (result.code != 0)
			failed = true;
	}

	if(!failed)
	{
		result = Cam_set_capture_lens_correction(lens_correction_mode);
		if (result.code != 0)
			failed = true;
	}

	if (!failed)
	{
		result = Cam_set_capture_camera(camera_moide, Cam_convert_to_resolution(resolution_mode, true), Cam_convert_to_resolution(resolution_mode, false), buffer_size);
		if (result.code != 0)
			failed = true;
	}

	return result;
}
