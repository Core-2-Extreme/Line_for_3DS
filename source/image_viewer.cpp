#include <3ds.h>
#include <string>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include "hid.hpp"
#include "draw.hpp"
#include "file.hpp"
#include "httpc.hpp"
#include "image_viewer.hpp"
#include "error.hpp"
#include "menu.hpp"
#include "log.hpp"
#include "types.hpp"
#include "setting_menu.hpp"
#include "swkbd.hpp"
#include "explorer.hpp"

/*For draw*/
bool imv_need_reflesh = false;
bool imv_pre_select_file_request = false;
bool imv_pre_enable[64];
int imv_pre_image_height = 0;
int imv_pre_image_width = 0;
int imv_pre_img_dl_progress = 0;
int imv_pre_img_pos_x = 0;
int imv_pre_img_pos_y = 0;
double imv_pre_img_zoom = 0.5;
/*---------------------------------------------*/

bool imv_already_init = false;
bool imv_main_run = false;
bool imv_dl_thread_run = false;
bool imv_load_thread_run = false;
bool imv_parse_thread_run = false;
bool imv_thread_suspend = true;
bool imv_adjust_url_request = false;
bool imv_img_dl_request = false;
bool imv_img_dl_and_parse_request = false;
bool imv_img_load_request = false;
bool imv_img_load_and_parse_request = false;
bool imv_img_parse_request = false;
bool imv_select_file_request = false;
bool imv_enable[64];
bool imv_first = true;
u8* imv_img_buffer = NULL;
u32 imv_img_size = 0;
int imv_img_height = 0;
int imv_img_width = 0;
int imv_img_dl_progress = 0;
int imv_img_pos_x = 0;
int imv_img_pos_y = 0;
int imv_max_buffer_size = 0x200000;
double imv_x_size[64];
double imv_y_size[64];
double imv_img_zoom = 0.5;
const char* imv_failed_reason;
std::string imv_img_load_dir_name = "";
std::string imv_img_load_file_name = "";
std::string imv_img_url = "";
std::string imv_msg[IMV_NUM_OF_MSG];
std::string imv_img_parse_thread_string = "Imv/Img parse thread";
std::string imv_img_load_thread_string = "Imv/Img load thread";
std::string imv_img_dl_thread_string = "Imv/Img dl thread";
std::string imv_init_string = "Imv/init";
std::string imv_exit_string = "Imv/exit";
std::string imv_ver = "v1.0.6";
Thread imv_parse_img_thread, imv_dl_img_thread, imv_load_img_thread;
C2D_Image imv_c2d_image[64];








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

int Imv_query_max_buffer_size(void)
{
	return imv_max_buffer_size;
}

void Imv_set_max_buffer_size(int size)
{
	imv_max_buffer_size = size;
}

void Imv_set_msg(int msg_num, std::string msg)
{
	if (msg_num >= 0 && msg_num < IMV_NUM_OF_MSG)
		imv_msg[msg_num] = msg;
}

void Imv_set_url(std::string url)
{
	imv_img_url = url;
	imv_img_dl_and_parse_request = true;
}

void Imv_set_load_file(std::string file_name, std::string dir_name)
{
	imv_img_load_file_name = file_name;
	imv_img_load_dir_name = dir_name;
	imv_select_file_request = false;
	imv_img_load_and_parse_request = true;
}

void Imv_cancel_select_file(void)
{
	imv_select_file_request = false;
}

void Imv_suspend(void)
{
	imv_thread_suspend = true;
	imv_main_run = false;
	Menu_resume();
}

void Imv_resume(void)
{
	Menu_suspend();
	imv_thread_suspend = false;
	imv_main_run = true;
	imv_need_reflesh = true;
}

Result_with_string Imv_load_msg(std::string lang)
{
	u8* fs_buffer = NULL;
	u32 read_size;
	std::string setting_data[128];
	Result_with_string result;
	fs_buffer = (u8*)malloc(0x2000);

	result = File_load_from_rom("imv_" + lang + ".txt", fs_buffer, 0x2000, &read_size, "romfs:/gfx/msg/");
	if (result.code != 0)
	{
		free(fs_buffer);
		return result;
	}

	result = Sem_parse_file((char*)fs_buffer, IMV_NUM_OF_MSG, setting_data);
	if (result.code != 0)
	{
		free(fs_buffer);
		return result;
	}

	for (int k = 0; k < IMV_NUM_OF_MSG; k++)
		Imv_set_msg(k, setting_data[k]);

	free(fs_buffer);
	return result;
}

void Imv_init(void)
{
	Log_log_save(imv_init_string, "Initializing...", 1234567890, FORCE_DEBUG);
	bool failed = false;

	if (imv_first && !failed)
	{
		imv_failed_reason = (const char*)malloc(0x100);
		imv_first = false;
	}

	Draw_progress("[Imv] Starting threads...");

	if (!failed)
	{
		imv_dl_thread_run = true;
		imv_load_thread_run = true;
		imv_parse_thread_run = true;
		imv_dl_img_thread = threadCreate(Imv_img_dl_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, 0, false);
		imv_load_img_thread = threadCreate(Imv_img_load_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, 0, false);
		imv_parse_img_thread = threadCreate(Imv_img_parse_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, 0, false);
	}

	for (int i = 0; i < 64; i++)
	{
		imv_c2d_image[i].tex = (C3D_Tex*)linearAlloc(sizeof(C3D_Tex));
		imv_c2d_image[i].subtex = (Tex3DS_SubTexture*)linearAlloc(sizeof(Tex3DS_SubTexture));
		imv_enable[i] = false;
		imv_x_size[i] = 0.0;
		imv_y_size[i] = 0.0;
	}

	Imv_resume();
	imv_already_init = true;
	Log_log_save(imv_init_string, "Initialized.", 1234567890, FORCE_DEBUG);
}

void Imv_main(void)
{
	int tex_size_x = 0;
	int tex_size_y = 0;
	int img_pos_x_offset = 0;
	int img_pos_y_offset = 0;
	double text_red, text_green, text_blue, text_alpha;
	double draw_x, draw_y;
	double scroll_speed = 0.5;
	std::string swkbd_data;
	Hid_info key;

	if (imv_img_dl_request || imv_img_dl_and_parse_request)
		imv_img_dl_progress = Httpc_query_dl_progress(IMV_HTTP_PORT0);

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

	if (imv_img_width >= 512)
		tex_size_x = 512;
	else
		tex_size_x = imv_img_width;

	if (imv_img_height >= 512)
		tex_size_y = 512;
	else
		tex_size_y = imv_img_height;

	tex_size_x *= imv_img_zoom;
	tex_size_y *= imv_img_zoom;
	
	if((imv_img_width > 400 && imv_img_height > 240 && tex_size_x >= 400 && tex_size_y >= 240) || (imv_img_width < 400 || imv_img_height < 240))
	{
		//Log_log_save("", "N", 1234567890, false);
		for(int i = 0; i < 64; i++)
		{
			if(imv_enable[i])
				C3D_TexSetFilter(imv_c2d_image[i].tex, GPU_NEAREST, GPU_NEAREST);
		}
	}
	else
	{
		//Log_log_save("", "L", 1234567890, false);
		for(int i = 0; i < 64; i++)
		{
			if(imv_enable[i])
				C3D_TexSetFilter(imv_c2d_image[i].tex, GPU_LINEAR, GPU_LINEAR);
		}
	}



	for(int i = 0; i < 64; i++)
	{
		if(imv_pre_enable[i] != imv_enable[i])
		{
			imv_need_reflesh = true;
			break;
		}
	}

	if(imv_need_reflesh || imv_pre_img_pos_x != imv_img_pos_x || imv_pre_img_pos_y != imv_img_pos_y || imv_pre_img_zoom != imv_img_zoom
	|| imv_pre_image_width != imv_img_width || imv_pre_image_height != imv_img_height || imv_pre_img_dl_progress != imv_img_dl_progress
	|| imv_pre_select_file_request != imv_select_file_request)
	{
		for(int i = 0; i < 64; i++)
			imv_pre_enable[i] = imv_enable[i];

		imv_pre_img_pos_x = imv_img_pos_x;
		imv_pre_img_pos_y = imv_img_pos_y;
		imv_pre_img_zoom = imv_img_zoom;
		imv_pre_image_width = imv_img_width;
		imv_pre_image_height = imv_img_height;
		imv_pre_img_dl_progress = imv_img_dl_progress;
		imv_pre_select_file_request = imv_select_file_request;
		imv_need_reflesh = true;
	}

	scroll_speed = Sem_query_settings_d(SEM_SCROLL_SPEED);
	Hid_query_key_state(&key);
	scroll_speed *= key.count;
	Log_main();
	if(Draw_query_need_reflesh() || !Sem_query_settings(SEM_ECO_MODE) || Expl_query_need_reflesh())
		imv_need_reflesh = true;

	if(!imv_select_file_request)
		Hid_key_flag_reset();

	if(imv_need_reflesh)
	{
		Draw_frame_ready();

		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);

		for (int i = 0; i < 64; i++)
		{
			if (img_pos_x_offset > tex_size_x * 7)
			{
				img_pos_x_offset = 0;
				img_pos_y_offset += tex_size_y;
			}
			if (imv_enable[i])
				Draw_texture(imv_c2d_image, dammy_tint, i, (imv_img_pos_x + img_pos_x_offset), (imv_img_pos_y + img_pos_y_offset), imv_x_size[i] * imv_img_zoom, imv_y_size[i] * imv_img_zoom);

			img_pos_x_offset += tex_size_x;
		}
		if(imv_img_dl_request || imv_img_dl_and_parse_request)
			Draw_texture(Square_image, aqua_tint, 0, 0.0, 15.0, 50.0 * imv_img_dl_progress, 3.0);

		Draw_top_ui();

		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(1, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(1, true, 2, 1.0, 1.0, 1.0);

		img_pos_x_offset = 0;
		img_pos_y_offset = 0;
		for (int i = 0; i < 64; i++)
		{
			if (img_pos_x_offset > tex_size_x * 7)
			{
				img_pos_x_offset = 0;
				img_pos_y_offset += tex_size_y;
			}
			if (imv_enable[i])
				Draw_texture(imv_c2d_image, dammy_tint, i, (imv_img_pos_x + img_pos_x_offset - 40), (imv_img_pos_y + img_pos_y_offset - 240), imv_x_size[i] * imv_img_zoom, imv_y_size[i] * imv_img_zoom);

			img_pos_x_offset += tex_size_x;
		}

		Draw_texture(Square_image, weak_red_tint, 0, 5.0, 170.0, 310.0, 55.0);
		Draw(imv_ver, 0, 10.0, 170.0, 0.45, 0.45, 0.0, 1.0, 0.0, 1.0);
		draw_x = 10.0;
		draw_y = 185.0;
		for(int i = 0; i < 4; i++)
		{
			Draw_texture(Square_image, weak_aqua_tint, 0, draw_x, draw_y, 65.0, 13.0);
			Draw(imv_msg[i], 0, (draw_x + 2.5), draw_y, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);

			draw_y += 20.0;
			if(draw_y > 205.0)
			{
				draw_x += 80.0;
				draw_y = 185.0;
			}
		}

		draw_x = 170.0;
		draw_y = 185.0;
		for(int i = 0; i < 2; i++)
		{
			Draw_texture(Square_image, weak_aqua_tint, 0, draw_x, draw_y, 140.0, 13.0);
			Draw(imv_msg[i + 4], 0, (draw_x + 2.5), draw_y, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			draw_y += 20.0;
		}

		if (imv_select_file_request)
			Expl_draw(imv_msg[6]);

		Draw_bot_ui();
		Draw_touch_pos();

		Draw_apply_draw();
		imv_need_reflesh = false;
	}
	else
		gspWaitForVBlank();

	if (Err_query_error_show_flag())
	{
		if (key.p_touch && key.touch_x >= 150 && key.touch_x <= 170 && key.touch_y >= 150 && key.touch_y < 170)
			Err_set_error_show_flag(false);
	}
	else if (imv_select_file_request)
		Expl_main();
	else
	{
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
			imv_touch_x_move_left -= (imv_touch_x_move_left * 0.025) * key.count;
			imv_touch_y_move_left -= (imv_touch_y_move_left * 0.025) * key.count;
			if (imv_touch_x_move_left < 0.5 && imv_touch_x_move_left > -0.5)
				imv_touch_x_move_left = 0;
			if (imv_touch_y_move_left < 0.5 && imv_touch_y_move_left > -0.5)
				imv_touch_y_move_left = 0;
		}

		if (key.p_start || (key.p_touch && key.touch_x >= 110 && key.touch_x <= 230 && key.touch_y >= 220 && key.touch_y <= 240))
			Imv_suspend();
		if (key.h_c_up || key.h_c_down)
		{
			if(key.held_time > 240)
				imv_img_pos_y += ((double)key.cpad_y * scroll_speed) * 0.125;
			else
				imv_img_pos_y += ((double)key.cpad_y * scroll_speed) * 0.0625;
		}
		if (key.h_c_left || key.h_c_right)
		{
			if(key.held_time > 240)
				imv_img_pos_x -= ((double)key.cpad_x * scroll_speed) * 0.125;
			else
				imv_img_pos_x -= ((double)key.cpad_x * scroll_speed) * 0.0625;
		}
		else if (key.p_a || (key.p_touch && key.touch_x >= 10 && key.touch_x <= 74 && key.touch_y >= 185 && key.touch_y <= 197))
			imv_img_parse_request = true;
		else if (key.p_b || (key.p_touch && key.touch_x >= 10 && key.touch_x <= 74 && key.touch_y >= 205 && key.touch_y <= 217))
			imv_img_dl_request = true;
		else if (key.p_x || (key.p_touch && key.touch_x >= 90 && key.touch_x <= 154 && key.touch_y >= 185 && key.touch_y <= 197))
		{
			void (*callback)(std::string, std::string);
			void (*cancel_callback)(void);
			callback = Imv_set_load_file;
			cancel_callback = Imv_cancel_select_file;
			Expl_set_callback(callback);
			Expl_set_cancel_callback(cancel_callback);
			Expl_set_operation_flag(EXPL_READ_DIR_REQUEST, true);
			imv_select_file_request = true;
		}
		else if (key.p_y || (key.p_touch && key.touch_x >= 90 && key.touch_x <= 154 && key.touch_y >= 205 && key.touch_y <= 217))
		{
			Hid_set_disable_flag(true);
			Swkbd_set_parameter(SWKBD_TYPE_NORMAL, SWKBD_NOTEMPTY_NOTBLANK, SWKBD_PREDICTIVE_INPUT, -1, 2, 8192, "画像URLを入力 / Type image url here.", Menu_query_clipboard());
			if (Swkbd_launch(8192, &swkbd_data, SWKBD_BUTTON_RIGHT))
				imv_img_url = swkbd_data;
		}
		else if (key.p_l || (key.p_touch && key.touch_x >= 170 && key.touch_x <= 309 && key.touch_y >= 185 && key.touch_y <= 197))
			imv_button_selected[0] = true;
		else if (key.p_r || (key.p_touch && key.touch_x >= 170 && key.touch_x <= 309 && key.touch_y >= 205 && key.touch_y <= 217))
			imv_button_selected[1] = true;
		else if ((key.h_l || (key.h_touch && key.touch_x >= 170 && key.touch_x <= 309 && key.touch_y >= 185 && key.touch_y <= 197 && imv_button_selected[0])) && (imv_img_zoom - 0.01) > 0.04)
			imv_img_zoom -= 0.01;
		else if (key.h_r || (key.h_touch && key.touch_x >= 170 && key.touch_x <= 309 && key.touch_y >= 205 && key.touch_y <= 217 && imv_button_selected[1]))
			imv_img_zoom += 0.01;

		imv_img_pos_x -= (imv_touch_x_move_left * scroll_speed);
		imv_img_pos_y -= (imv_touch_y_move_left * scroll_speed);
	}
}

void Imv_img_parse_thread(void* arg)
{
	Log_log_save(imv_img_parse_thread_string, "Thread started.", 1234567890, false);
	int log_num;
	int parse_start_pos_x;
	int parse_start_pos_y;
	double ideal_zoom_size;
	Result_with_string result;
	stbi_uc* stb_image = NULL;

	while (imv_parse_thread_run)
	{
		if (imv_img_parse_request)
		{
			for (int i = 0; i < 64; i++)
			{
				imv_x_size[i] = 0.0;
				imv_y_size[i] = 0.0;
			}
			log_num = Log_log_save(imv_img_parse_thread_string, "APT_SetAppCpuTimeLimit()...", 1234567890, false);
			result.code = APT_SetAppCpuTimeLimit(80);
			if (result.code == 0)
				Log_log_add(log_num, Err_query_template_summary(0), result.code, false);
			else
				Log_log_add(log_num, Err_query_template_summary(-1024), result.code, false);

			imv_img_height = 0;
			imv_img_width = 0;

			log_num = Log_log_save(imv_img_parse_thread_string, "stbi_load_from_memory()...", 1234567890, false);
			free(stb_image);
			stb_image = NULL;
			if(imv_img_buffer != NULL)
				stb_image = stbi_load_from_memory((stbi_uc const*)imv_img_buffer, (int)imv_img_size, &imv_img_width, &imv_img_height, NULL, STBI_rgb_alpha);
	
			Log_log_add(log_num, " w : " + std::to_string(imv_img_width) + " h : " + std::to_string(imv_img_height), 1234567890, false);

			if (stb_image == NULL)
			{
				imv_failed_reason = stbi_failure_reason();
				if (imv_failed_reason == NULL)
				{
					Err_set_error_message("[Error] Unknown error.", "", imv_img_parse_thread_string, STB_IMG_RETURNED_NOT_SUCCESS);
					Log_log_add(log_num, "Unknown error ", STB_IMG_RETURNED_NOT_SUCCESS, false);
				}
				else
				{
					Err_set_error_message(Err_query_template_summary(STB_IMG_RETURNED_NOT_SUCCESS), imv_failed_reason, imv_img_parse_thread_string, STB_IMG_RETURNED_NOT_SUCCESS);
					Log_log_add(log_num, imv_failed_reason, STB_IMG_RETURNED_NOT_SUCCESS, false);
				}
				Err_set_error_show_flag(true);
			}
			else
			{
				for (int i = 0; i < 64; i++)
				{
					imv_enable[i] = false;
					linearFree(imv_c2d_image[i].tex->data);
					imv_c2d_image[i].tex->data = NULL;
				}
				Draw_rgba_to_abgr(stb_image, (u32)imv_img_width, (u32)imv_img_height);
				parse_start_pos_x = 0;
				parse_start_pos_y = 0;

				for (int i = 0; i < 64; i++)
				{
					if (!(parse_start_pos_x > imv_img_width || parse_start_pos_y > imv_img_height))
					{
						imv_x_size[i] = imv_img_width - parse_start_pos_x;
						imv_y_size[i] = imv_img_height - parse_start_pos_y;
						if(imv_x_size[i] > 512)
							imv_x_size[i] = 512;
						if(imv_y_size[i] > 512)
							imv_y_size[i] = 512;
						
						imv_enable[i] = true;
						log_num = Log_log_save(imv_img_parse_thread_string, "Draw_create_texture()...", 1234567890, false);
						result = Draw_create_texture(imv_c2d_image[i].tex, (Tex3DS_SubTexture*)imv_c2d_image[i].subtex, stb_image, (u32)(imv_img_width * imv_img_height * 4), imv_img_width, imv_img_height, 4, parse_start_pos_x, parse_start_pos_y, 512, 512, GPU_RGBA8);


						Log_log_add(log_num, result.string, result.code, false);
						if (result.code != 0)
						{
							Err_set_error_message(result.string, result.error_description, imv_img_parse_thread_string, result.code);
							Err_set_error_show_flag(true);
						}
					}

					parse_start_pos_x += 512;
					if (parse_start_pos_x >= 4096)
					{
						parse_start_pos_x = 0;
						parse_start_pos_y += 512;
					}
				}

				ideal_zoom_size = 1.0;
				while (true)
				{
					if (imv_img_width > imv_img_height)
					{
						if (((double)imv_img_width * ideal_zoom_size) > 399.0 && ((double)imv_img_width * ideal_zoom_size) < 400.1)
							break;
						else if (((double)imv_img_width * ideal_zoom_size) > 399.0)
							ideal_zoom_size -= 0.0001;
						else
							ideal_zoom_size += 0.0001;
					}
					else
					{
						if (((double)imv_img_height * ideal_zoom_size) > 224.0 && ((double)imv_img_height * ideal_zoom_size) < 225.1)
							break;
						else if (((double)imv_img_height * ideal_zoom_size) > 224.0)
							ideal_zoom_size -= 0.0001;
						else
							ideal_zoom_size += 0.0001;
					}
				}
				if (imv_img_width > imv_img_height)
				{
					while (true)
					{
						if ((double)imv_img_height * ideal_zoom_size < 225.1)
							break;
						else
							ideal_zoom_size -= 0.0001;
					}
				}

				imv_img_zoom = ideal_zoom_size;
				imv_img_pos_x = 0.0;
				imv_img_pos_y = 13.5;
			}
			log_num = Log_log_save(imv_img_parse_thread_string, "APT_SetAppCpuTimeLimit()...", 1234567890, false);
			result.code = APT_SetAppCpuTimeLimit(5);
			if (result.code == 0)
				Log_log_add(log_num, Err_query_template_summary(0), result.code, false);
			else
				Log_log_add(log_num, Err_query_template_summary(-1024), result.code, false);

			imv_img_parse_request = false;
		}
		else
			usleep(ACTIVE_THREAD_SLEEP_TIME);

		while (imv_thread_suspend)
			usleep(INACTIVE_THREAD_SLEEP_TIME);
	}

	free(stb_image);
	stb_image = NULL;

	Log_log_save(imv_img_parse_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Imv_img_load_thread(void* arg)
{
	Log_log_save(imv_img_load_thread_string, "Thread started.", 1234567890, false);
	u32 read_size;
	int log_num;
	Result_with_string result;

	while (imv_load_thread_run)
	{
		if (imv_img_load_request || imv_img_load_and_parse_request)
		{
			free(imv_img_buffer);
			imv_img_buffer = NULL;
		
			imv_img_buffer = (u8*)malloc(imv_max_buffer_size);
			if (imv_img_buffer == NULL)
			{
				Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), imv_img_load_thread_string, OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				Log_log_save(imv_img_load_thread_string, Err_query_template_summary(OUT_OF_MEMORY), OUT_OF_MEMORY, false);
			}
			else
			{
				log_num = Log_log_save(imv_img_load_thread_string, "File_load_from_file()...", 1234567890, false);
				result = File_load_from_file(imv_img_load_file_name, imv_img_buffer, imv_max_buffer_size, &read_size, imv_img_load_dir_name);
				Log_log_add(log_num, result.string, result.code, false);

				if (result.code == 0)
				{
					imv_img_size = read_size;
					if (imv_img_load_and_parse_request)
						imv_img_parse_request = true;
				}
				else
				{
					Err_set_error_message(result.string, result.error_description, imv_img_load_thread_string, result.code);
					Err_set_error_show_flag(true);
				}
			}

			if (imv_img_load_and_parse_request)
				imv_img_load_and_parse_request = false;
			else if (imv_img_load_request)
				imv_img_load_request = false;
		}
		else
			usleep(ACTIVE_THREAD_SLEEP_TIME);

		while (imv_thread_suspend)
			usleep(INACTIVE_THREAD_SLEEP_TIME);
	}
	Log_log_save(imv_img_load_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Imv_img_dl_thread(void* arg)
{
	Log_log_save(imv_img_dl_thread_string, "Thread started.", 1234567890, false);
	u32 dl_size;
	u32 status_code;
	int log_num;
	size_t cut_pos[2];
	std::string file_name;
	std::string last_url;
	Result_with_string result;

	while (imv_dl_thread_run)
	{
		if (imv_img_dl_request || imv_img_dl_and_parse_request)
		{
			dl_size = 0;
			status_code = 0;
			cut_pos[0] = std::string::npos;
			cut_pos[1] = std::string::npos;

			free(imv_img_buffer);
			imv_img_buffer = NULL;

			imv_img_buffer = (u8*)malloc(imv_max_buffer_size);
			if (imv_img_buffer == NULL)
			{
				Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), imv_img_dl_thread_string, OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				Log_log_save(imv_img_dl_thread_string, Err_query_template_summary(OUT_OF_MEMORY), OUT_OF_MEMORY, false);
			}
			else
			{
				file_name = imv_img_url;
				log_num = Log_log_save(imv_img_dl_thread_string, "Httpc_dl_data()....", 1234567890, false);
				result = Httpc_dl_data(imv_img_url, imv_img_buffer, imv_max_buffer_size, &dl_size, &status_code, true, &last_url, false, 100, IMV_HTTP_PORT0);
				Log_log_add(log_num, result.string + std::to_string(dl_size / 1024) + "KB ", result.code, false);
				imv_img_dl_progress = Httpc_query_dl_progress(IMV_HTTP_PORT0);
				if (result.code == 0)
				{
					imv_img_size = dl_size;
					if (imv_img_dl_and_parse_request)
						imv_img_parse_request = true;

					cut_pos[0] = file_name.find("&id=");
					cut_pos[1] = file_name.find("lh3.googleusercontent.com/d/");
					if (cut_pos[0] == std::string::npos && cut_pos[1] == std::string::npos)
						file_name = Menu_query_time(0);
					else if(!(cut_pos[0] == std::string::npos))
						file_name = file_name.substr(cut_pos[0] + 4);
					else if (!(cut_pos[1] == std::string::npos))
						file_name = file_name.substr(cut_pos[1] + 28);

					if (file_name.length() > 33)
						file_name = file_name.substr(0, 33);

					log_num = Log_log_save(imv_img_dl_thread_string, "File_save_to_file()...", 1234567890, false);
					result = File_save_to_file(file_name + ".jpg", imv_img_buffer, (int)dl_size, "/Line/images/", true);
					Log_log_add(log_num, result.string, result.code, false);
				}
				else
				{
					Err_set_error_message(result.string, result.error_description, imv_img_dl_thread_string, result.code);
					Err_set_error_show_flag(true);
				}
			}

			if (imv_img_dl_and_parse_request)
				imv_img_dl_and_parse_request = false;
			else if (imv_img_dl_request)
				imv_img_dl_request = false;
		}
		else
			usleep(ACTIVE_THREAD_SLEEP_TIME);

		while (imv_thread_suspend)
			usleep(INACTIVE_THREAD_SLEEP_TIME);
	}
	Log_log_save(imv_img_dl_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Imv_exit(void)
{
	Log_log_save(imv_exit_string, "Exiting...", 1234567890, FORCE_DEBUG);
	u64 time_out = 10000000000;
	int log_num;
	Result_with_string result;

	Draw_progress("[Imv] Exiting...");
	imv_already_init = false;
	imv_dl_thread_run = false;
	imv_load_thread_run = false;
	imv_parse_thread_run = false;
	imv_thread_suspend = false;

	for(int i = 0; i < 3; i++)
	{
		log_num = Log_log_save(imv_exit_string, "threadJoin()...", 1234567890, FORCE_DEBUG);

		if(i == 0)
			result.code = threadJoin(imv_parse_img_thread, time_out);
		else	if(i == 1)
			result.code = threadJoin(imv_dl_img_thread, time_out);
		else	if(i == 2)
			result.code = threadJoin(imv_load_img_thread, time_out);

		if (result.code == 0)
			Log_log_add(log_num, Err_query_template_summary(0), result.code, FORCE_DEBUG);
		else
			Log_log_add(log_num, Err_query_template_summary(-1024), result.code, FORCE_DEBUG);
	}

	threadFree(imv_parse_img_thread);
	threadFree(imv_dl_img_thread);
	threadFree(imv_load_img_thread);

	for (int i = 0; i < 64; i++)
	{
		imv_enable[i] = false;
		linearFree(imv_c2d_image[i].tex->data);
		linearFree(imv_c2d_image[i].tex);
		linearFree((void*)imv_c2d_image[i].subtex);
		imv_c2d_image[i].tex->data = NULL;
		imv_c2d_image[i].tex = NULL;
		imv_c2d_image[i].subtex = NULL;
	}

	free(imv_img_buffer);
	imv_img_buffer = NULL;

	Log_log_save(imv_exit_string, "Exited.", 1234567890, FORCE_DEBUG);
}
