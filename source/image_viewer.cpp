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
u8* imv_image_buffer;
u32 imv_image_size = 0;
int imv_image_height = 0;
int imv_image_width = 0;
int imv_img_dl_progress = 0;
int imv_img_pos_x = 0;
int imv_img_pos_y = 0;
int imv_clipboard_selected_num = 0;
int imv_img_httpc_buffer_size;
int imv_img_fs_buffer_size;
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
std::string imv_ver = "v1.0.4";
Thread imv_parse_img_thread, imv_dl_img_thread, imv_load_img_thread;
C2D_Image imv_c2d_image[64];

bool Imv_query_init_flag(void)
{
	return imv_already_init;
}

bool Imv_query_running_flag(void)
{
	return imv_main_run;
}

int Imv_query_buffer_size(int buffer_num)
{
	if (buffer_num == IMV_HTTPC_BUFFER)
		return imv_img_httpc_buffer_size;
	else if (buffer_num == IMV_FS_BUFFER)
		return imv_img_fs_buffer_size;
	else
		return -1;
}

int Imv_query_clipboard_num(void)
{
	return imv_clipboard_selected_num;
}

bool Imv_query_operation_flag(int operation_num)
{
	if (operation_num == IMV_IMG_LOAD_REQUEST)
		return imv_img_load_request;
	else if (operation_num == IMV_IMG_LOAD_AND_PARSE_REQUEST)
		return imv_img_load_and_parse_request;
	else if (operation_num == IMV_IMG_DL_REQUEST)
		return imv_img_dl_request;
	else if (operation_num == IMV_IMG_DL_AND_PARSE_REQUEST)
		return imv_img_dl_and_parse_request;
	else if (operation_num == IMV_IMG_PARSE_REQUEST)
		return imv_img_parse_request;
	else if (operation_num == IMV_ADJUST_URL_REQUEST)
		return imv_adjust_url_request;
	else if (operation_num == IMV_SELECT_FILE_REQUEST)
		return imv_select_file_request;
	else
		return false;
}

double Imv_query_img_pos_x(void)
{
	return imv_img_pos_x;
}

double Imv_query_img_pos_y(void)
{
	return imv_img_pos_y;
}

double Imv_query_img_zoom(void)
{
	return imv_img_zoom;
}

void Imv_set_buffer_size(int buffer_num, int size)
{
	if (buffer_num == IMV_HTTPC_BUFFER)
		imv_img_httpc_buffer_size = size;
	else if (buffer_num == IMV_FS_BUFFER)
		imv_img_fs_buffer_size = size;
}

void Imv_set_clipboard_num(int num)
{
	imv_clipboard_selected_num = num;
}

void Imv_set_msg(int msg_num, std::string msg)
{
	if (msg_num >= 0 && msg_num < IMV_NUM_OF_MSG)
		imv_msg[msg_num] = msg;
}

void Imv_set_operation_flag(int operation_num, bool flag)
{
	if (operation_num == IMV_IMG_LOAD_REQUEST)
		imv_img_load_request = flag;
	else if (operation_num == IMV_IMG_LOAD_AND_PARSE_REQUEST)
		imv_img_load_and_parse_request = flag;
	else if (operation_num == IMV_IMG_DL_REQUEST)
		imv_img_dl_request = flag;
	else if (operation_num == IMV_IMG_DL_AND_PARSE_REQUEST)
		imv_img_dl_and_parse_request = flag;
	else if (operation_num == IMV_IMG_PARSE_REQUEST)
		imv_img_parse_request = flag;
	else if (operation_num == IMV_ADJUST_URL_REQUEST)
		imv_adjust_url_request = flag;
	else if (operation_num == IMV_SELECT_FILE_REQUEST)
		imv_select_file_request = flag;
}

void Imv_set_url(std::string url)
{
	imv_img_url = url;
}

void Imv_set_img_pos_x(double x)
{
	imv_img_pos_x = x;
}

void Imv_set_img_pos_y(double y)
{
	imv_img_pos_y = y;
}

void Imv_set_img_zoom(double size)
{
	imv_img_zoom = size;
}

void Imv_set_load_file_name(std::string file_name)
{
	imv_img_load_file_name = file_name;
}

void Imv_set_load_dir_name(std::string dir_name)
{
	imv_img_load_dir_name = dir_name;
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

void Imv_init(void)
{
	Log_log_save(imv_init_string, "Initializing...", 1234567890, DEBUG);
	bool failed = false;

	imv_image_buffer = (u8*)malloc(0x500000);
	if (imv_image_buffer == NULL)
	{
		Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), imv_init_string, OUT_OF_MEMORY);
		Err_set_error_show_flag(true);
		Log_log_save(imv_init_string, Err_query_template_summary(OUT_OF_MEMORY), OUT_OF_MEMORY, DEBUG);
		failed = true;
	}
	else
		memset(imv_image_buffer, 0x0, 0x500000);

	if (imv_first && !failed)
	{
		imv_failed_reason = (const char*)malloc(0x100);
		imv_first = false;
	}

	Draw_progress("0/0 [Imv] Starting threads...");

	if (!failed)
	{
		imv_dl_thread_run = true;
		imv_load_thread_run = true;
		imv_parse_thread_run = true;
		imv_dl_img_thread = threadCreate(Imv_img_dl_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, -1, false);
		imv_load_img_thread = threadCreate(Imv_img_load_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, -1, false);
		imv_parse_img_thread = threadCreate(Imv_img_parse_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, -1, false);
	}

	Imv_resume();
	imv_already_init = true;
	Log_log_save(imv_init_string, "Initialized.", 1234567890, DEBUG);
}

void Imv_main(void)
{
	int img_size_x = 0;
	int img_size_y = 0;
	int img_pos_x_offset = 0;
	int img_pos_y_offset = 0;
	double text_red, text_green, text_blue, text_alpha;
	double draw_x, draw_y;
	std::string swkbd_data;

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

	if (imv_image_width >= 512)
		img_size_x = 512;
	else
		img_size_x = imv_image_width;

	if (imv_image_height >= 512)
		img_size_y = 512;
	else
		img_size_y = imv_image_height;

	img_size_x *= imv_img_zoom;
	img_size_y *= imv_img_zoom;

	for(int i = 0; i < 64; i++)
	{
		if(imv_pre_enable[i] != imv_enable[i])
		{
			imv_need_reflesh = true;
			break;
		}
	}

	if(imv_need_reflesh || imv_pre_img_pos_x != imv_img_pos_x || imv_pre_img_pos_y != imv_img_pos_y || imv_pre_img_zoom != imv_img_zoom
	|| imv_pre_image_width != imv_image_width || imv_pre_image_height != imv_image_height || imv_pre_img_dl_progress != imv_img_dl_progress
	|| imv_pre_select_file_request != imv_select_file_request)
	{
		for(int i = 0; i < 64; i++)
			imv_pre_enable[i] = imv_enable[i];

		imv_pre_img_pos_x = imv_img_pos_x;
		imv_pre_img_pos_y = imv_img_pos_y;
		imv_pre_img_zoom = imv_img_zoom;
		imv_pre_image_width = imv_image_width;
		imv_pre_image_height = imv_image_height;
		imv_pre_img_dl_progress = imv_img_dl_progress;
		imv_need_reflesh = true;
	}

	if(Draw_query_need_reflesh() || !Sem_query_settings(SEM_ECO_MODE) || Expl_query_need_reflesh())
		imv_need_reflesh = true;

	if(imv_need_reflesh)
	{
		Draw_set_draw_mode(Sem_query_settings(SEM_VSYNC_MODE));

		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);

		for (int i = 0; i < 64; i++)
		{
			if (img_pos_x_offset > img_size_x * 7)
			{
				img_pos_x_offset = 0;
				img_pos_y_offset += img_size_y;
			}
			if (imv_enable[i])
				Draw_texture(imv_c2d_image, dammy_tint, i, (imv_img_pos_x + img_pos_x_offset), (imv_img_pos_y + img_pos_y_offset), img_size_x, img_size_y);

			img_pos_x_offset += img_size_x;
		}
		Draw_texture(Square_image, weak_aqua_tint, 0, 0.0, 15.0, 50.0 * imv_img_dl_progress, 3.0);
		Draw_top_ui();

		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(1, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(1, true, 2, 1.0, 1.0, 1.0);

		img_pos_x_offset = 0;
		img_pos_y_offset = 0;
		for (int i = 0; i < 64; i++)
		{
			if (img_pos_x_offset > img_size_x * 7)
			{
				img_pos_x_offset = 0;
				img_pos_y_offset += img_size_y;
			}
			if (imv_enable[i])
				Draw_texture(imv_c2d_image, dammy_tint, i, (imv_img_pos_x + img_pos_x_offset - 40), (imv_img_pos_y + img_pos_y_offset - 240), img_size_x, img_size_y);

			img_pos_x_offset += img_size_x;
		}

		Draw(imv_ver, 0, 0.0, 0.0, 0.45, 0.45, 0.0, 1.0, 0.0, 1.0);

		draw_x = 10.0;
		draw_y = 175.0;
		for(int i = 0; i < 4; i++)
		{
			Draw_texture(Square_image, weak_aqua_tint, 0, draw_x, draw_y, 65.0, 13.0);
			Draw(imv_msg[i], 0, (draw_x + 2.5), draw_y, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);

			draw_y += 20.0;
			if(draw_y > 195.0)
			{
				draw_x += 80.0;
				draw_y = 175.0;
			}
		}

		draw_x = 170.0;
		draw_y = 175.0;
		for(int i = 0; i < 2; i++)
		{
			Draw_texture(Square_image, weak_aqua_tint, 0, draw_x, draw_y, 140.0, 13.0);
			Draw(imv_msg[i + 4], 0, (draw_x + 2.5), draw_y, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			draw_y += 20.0;
		}

		if (imv_select_file_request)
			Draw_expl(imv_msg[6]);

		Draw_bot_ui();
		Draw_touch_pos();

		Draw_apply_draw();
		imv_need_reflesh = false;
	}
	else
		gspWaitForVBlank();

	Hid_set_disable_flag(true);
	if (imv_adjust_url_request)
	{
		Swkbd_set_parameter(SWKBD_TYPE_NORMAL, SWKBD_NOTEMPTY_NOTBLANK, SWKBD_PREDICTIVE_INPUT, -1, 2, 8192, "画像URLを入力 / Type image url here.", Menu_query_clipboard());
		if (Swkbd_launch(8192, &swkbd_data, SWKBD_BUTTON_RIGHT))
			imv_img_url = swkbd_data;

		imv_adjust_url_request = false;
	}
}

void Imv_img_parse_thread(void* arg)
{
	Log_log_save(imv_img_parse_thread_string, "Thread started.", 1234567890, false);
	int log_num;
	int parse_start_pos_x;
	int parse_start_pos_y;
	double ideal_zoom_size;
	C3D_Tex* c3d_cache_tex[64];
	Tex3DS_SubTexture* c3d_cache_subtex[64];
	Result_with_string result;
	stbi_uc* stb_image;

	for (int i = 0; i < 64; i++)
	{
		c3d_cache_tex[i] = (C3D_Tex*)linearAlloc(sizeof(C3D_Tex));
		c3d_cache_subtex[i] = (Tex3DS_SubTexture*)linearAlloc(sizeof(Tex3DS_SubTexture));
		imv_enable[i] = false;
	}
	stb_image = NULL;

	while (imv_parse_thread_run)
	{
		if (imv_img_parse_request)
		{
			log_num = Log_log_save(imv_img_parse_thread_string, "APT_SetAppCpuTimeLimit()...", 1234567890, false);
			result.code = APT_SetAppCpuTimeLimit(80);
			if (result.code == 0)
				Log_log_add(log_num, Err_query_template_summary(0), result.code, false);
			else
				Log_log_add(log_num, Err_query_template_summary(-1024), result.code, false);

			imv_image_height = 0;
			imv_image_width = 0;

			log_num = Log_log_save(imv_img_parse_thread_string, "stbi_load_from_memory()...", 1234567890, false);
			free(stb_image);
			stb_image = NULL;
			stb_image = stbi_load_from_memory((stbi_uc const*)imv_image_buffer, (int)imv_image_size, &imv_image_width, &imv_image_height, NULL, STBI_rgb_alpha);
			Log_log_add(log_num, " w : " + std::to_string(imv_image_width) + " h : " + std::to_string(imv_image_height), 1234567890, false);

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
					linearFree(c3d_cache_tex[i]->data);
					imv_c2d_image[i].tex = c3d_cache_tex[i];
					imv_c2d_image[i].subtex = c3d_cache_subtex[i];
					imv_enable[i] = false;
				}
				Draw_rgba_to_abgr(stb_image, (u32)imv_image_width, (u32)imv_image_height);

				parse_start_pos_x = 0;
				parse_start_pos_y = 0;

				for (int i = 0; i < 64; i++)
				{
					if (!(parse_start_pos_x > imv_image_width || parse_start_pos_y > imv_image_height))
					{
						imv_enable[i] = true;
						log_num = Log_log_save(imv_img_parse_thread_string, "Draw_c3dtex_to_c2dimage()...", 1234567890, false);
						result = Draw_c3dtex_to_c2dimage(c3d_cache_tex[i], c3d_cache_subtex[i], stb_image, (u32)(imv_image_width * imv_image_height * 4), imv_image_width, imv_image_height, parse_start_pos_x, parse_start_pos_y, 512, 512, GPU_RGBA8);
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
					if (imv_image_width > imv_image_height)
					{
						if (((double)imv_image_width * ideal_zoom_size) > 399.0 && ((double)imv_image_width * ideal_zoom_size) < 400.1)
							break;
						else if (((double)imv_image_width * ideal_zoom_size) > 399.0)
							ideal_zoom_size -= 0.0001;
						else
							ideal_zoom_size += 0.0001;
					}
					else
					{
						if (((double)imv_image_height * ideal_zoom_size) > 224.0 && ((double)imv_image_height * ideal_zoom_size) < 225.1)
							break;
						else if (((double)imv_image_height * ideal_zoom_size) > 224.0)
							ideal_zoom_size -= 0.0001;
						else
							ideal_zoom_size += 0.0001;
					}
				}
				if (imv_image_width > imv_image_height)
				{
					while (true)
					{
						if ((double)imv_image_height * ideal_zoom_size < 225.1)
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
			result.code = APT_SetAppCpuTimeLimit(30);
			if (result.code == 0)
				Log_log_add(log_num, Err_query_template_summary(0), result.code, false);
			else
				Log_log_add(log_num, Err_query_template_summary(-1024), result.code, false);

			imv_img_parse_request = false;
		}
		else
			usleep(ACTIW_THREAD_SLEEP_TIME);

		while (imv_thread_suspend)
			usleep(INACTIW_THREAD_SLEEP_TIME);
	}

	for (int i = 0; i < 64; i++)
	{
		linearFree(c3d_cache_tex[i]->data);
		linearFree(c3d_cache_tex[i]);
		linearFree(c3d_cache_subtex[i]);
		c3d_cache_tex[i]->data = NULL;
		imv_enable[i] = false;
	}
	free(stb_image);
	stb_image = NULL;

	Log_log_save(imv_img_parse_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Imv_img_load_thread(void* arg)
{
	Log_log_save(imv_img_load_thread_string, "Thread started.", 1234567890, false);
	u8* fs_buffer;
	u32 read_size;
	int log_num;
	FS_Archive fs_archive = 0;
	Handle fs_handle = 0;
	Result_with_string result;

	while (imv_load_thread_run)
	{
		if (imv_img_load_request || imv_img_load_and_parse_request)
		{
			fs_buffer = (u8*)malloc(imv_img_fs_buffer_size);
			if (fs_buffer == NULL)
			{
				Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), imv_img_load_thread_string, OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				Log_log_save(imv_img_load_thread_string, Err_query_template_summary(OUT_OF_MEMORY), OUT_OF_MEMORY, false);
			}
			else
			{
				log_num = Log_log_save(imv_img_load_thread_string, "File_load_from_file()...", 1234567890, false);
				result = File_load_from_file(imv_img_load_file_name, fs_buffer, imv_img_fs_buffer_size, &read_size, imv_img_load_dir_name, fs_handle, fs_archive);
				Log_log_add(log_num, result.string, result.code, false);

				if (result.code == 0)
				{
					imv_image_size = read_size;
					memcpy(imv_image_buffer, fs_buffer, imv_image_size);
					if (imv_img_load_and_parse_request)
						imv_img_parse_request = true;
				}
				else
				{
					Err_set_error_message(result.string, result.error_description, imv_img_load_thread_string, result.code);
					Err_set_error_show_flag(true);
				}
			}

			free(fs_buffer);
			fs_buffer = NULL;
			if (imv_img_load_and_parse_request)
				imv_img_load_and_parse_request = false;
			else if (imv_img_load_request)
				imv_img_load_request = false;
		}
		else
			usleep(ACTIW_THREAD_SLEEP_TIME);

		while (imv_thread_suspend)
			usleep(INACTIW_THREAD_SLEEP_TIME);
	}
	Log_log_save(imv_img_load_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Imv_img_dl_thread(void* arg)
{
	Log_log_save(imv_img_dl_thread_string, "Thread started.", 1234567890, false);
	u8* httpc_buffer;
	u32 dl_size;
	u32 status_code;
	int log_num;
	size_t cut_pos[2];
	std::string file_name;
	std::string last_url;
	FS_Archive fs_archive = 0;
	Handle fs_handle = 0;
	Result_with_string result;

	while (imv_dl_thread_run)
	{
		if (imv_img_dl_request || imv_img_dl_and_parse_request)
		{
			dl_size = 0;
			status_code = 0;
			cut_pos[0] = std::string::npos;
			cut_pos[1] = std::string::npos;
			httpc_buffer = (u8*)malloc(imv_img_httpc_buffer_size);
			if (httpc_buffer == NULL)
			{
				Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), imv_img_dl_thread_string, OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				Log_log_save(imv_img_dl_thread_string, Err_query_template_summary(OUT_OF_MEMORY), OUT_OF_MEMORY, false);
			}
			else
			{
				file_name = imv_img_url;
				log_num = Log_log_save(imv_img_dl_thread_string, "Httpc_dl_data()....", 1234567890, false);
				result = Httpc_dl_data(imv_img_url, httpc_buffer, imv_img_httpc_buffer_size, &dl_size, &status_code, true, &last_url, false, 100, IMV_HTTP_PORT0);
				Log_log_add(log_num, result.string + std::to_string(dl_size / 1024) + "KB ", result.code, false);
				imv_img_dl_progress = Httpc_query_dl_progress(IMV_HTTP_PORT0);
				if (result.code == 0)
				{
					imv_image_size = dl_size;
					memcpy(imv_image_buffer, httpc_buffer, imv_image_size);
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
					result = File_save_to_file(file_name + ".jpg", imv_image_buffer, (int)dl_size, "/Line/images/", true, fs_handle, fs_archive);
					Log_log_add(log_num, result.string, result.code, false);
				}
				else
				{
					Err_set_error_message(result.string, result.error_description, imv_img_dl_thread_string, result.code);
					Err_set_error_show_flag(true);
				}
			}

			free(httpc_buffer);
			httpc_buffer = NULL;
			if (imv_img_dl_and_parse_request)
				imv_img_dl_and_parse_request = false;
			else if (imv_img_dl_request)
				imv_img_dl_request = false;
		}
		else
			usleep(ACTIW_THREAD_SLEEP_TIME);

		while (imv_thread_suspend)
			usleep(INACTIW_THREAD_SLEEP_TIME);
	}
	Log_log_save(imv_img_dl_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Imv_exit(void)
{
	Log_log_save(imv_exit_string, "Exiting...", 1234567890, DEBUG);
	u64 time_out = 10000000000;
	int log_num;
	bool failed = false;
	Result_with_string result;

	Draw_progress("[Imv] Exiting...");
	imv_already_init = false;
	imv_dl_thread_run = false;
	imv_load_thread_run = false;
	imv_parse_thread_run = false;
	imv_thread_suspend = false;

	for(int i = 0; i < 3; i++)
	{
		log_num = Log_log_save(imv_exit_string, "threadJoin()...", 1234567890, DEBUG);

		if(i == 0)
			result.code = threadJoin(imv_parse_img_thread, time_out);
		else	if(i == 1)
			result.code = threadJoin(imv_dl_img_thread, time_out);
		else	if(i == 2)
			result.code = threadJoin(imv_load_img_thread, time_out);

		if (result.code == 0)
			Log_log_add(log_num, Err_query_template_summary(0), result.code, DEBUG);
		else
		{
			failed = true;
			Log_log_add(log_num, Err_query_template_summary(-1024), result.code, DEBUG);
		}
	}

	threadFree(imv_parse_img_thread);
	threadFree(imv_dl_img_thread);
	threadFree(imv_load_img_thread);

	free(imv_image_buffer);
	imv_image_buffer = NULL;

	if (failed)
		Log_log_save(imv_exit_string, "[Warn] Some function returned error.", 1234567890, DEBUG);

	Log_log_save(imv_exit_string, "Exited.", 1234567890, DEBUG);
}
