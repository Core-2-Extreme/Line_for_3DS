#include <3ds.h>
#include <string>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "hid.hpp"
#include "draw.hpp"
#include "file.hpp"
#include "httpc.hpp"
#include "image_viewer.hpp"
#include "share_function.hpp"
#include "error.hpp"
#include "menu.hpp"
#include "explorer.hpp"
#include "log.hpp"
#include "types.hpp"
#include "setting_menu.hpp"

bool imv_already_init;
bool imv_main_run;
bool imv_dl_thread_run;
bool imv_load_thread_run;
bool imv_parse_thread_run;
bool imv_thread_suspend;
bool imv_adjust_url_request;
bool imv_img_dl_request;
bool imv_img_dl_and_parse_request;
bool imv_img_load_request;
bool imv_img_load_and_parse_request;
bool imv_img_parse_request;
bool imv_select_file_request;
bool imv_enable[64];
u8* imv_image_buffer;
u32 imv_image_size;
int imv_image_height = 0;
int imv_image_width = 0;
int imv_img_dl_progress = 0;
int imv_img_pos_x;
int imv_img_pos_y;
int imv_clipboard_selected_num;
int imv_img_httpc_buffer_size;
int imv_img_fs_buffer_size;
double imv_img_zoom;
std::string imv_img_load_dir_name;
std::string imv_img_load_file_name;
std::string imv_message_en[10] = {
	"Rendering(A)",
	"Download(B)",
	"Adjust url(Y)",
	"Up(D UP)",
	"Down(D DOWN)",
	"Decrease size(L)",
	"Increase size(R)",
	"Selected clipboard num ",
	"Select(A) Go back(B) Move(DPAD ↑,↓,→,←) Close(Y)",
	"Select file(X)",
};
std::string imv_message_jp[10] = {
	"レンダリング(A)",
	"ダウンロード(B)",
	"url調節(Y)",
	"上(十字上)",
	"下(十字下)",
	"サイズを小さく(L)",
	"サイズを大きく(R)",
	"選択されたクリップボード番号 ",
	"選択(A) 戻る(B) 移動(十字 ↑,↓,→,←) 閉じる(Y)",
	"ファイル選択(X)",
};

Thread imv_parse_img_thread, imv_dl_img_thread, imv_load_img_thread;
C2D_Image imv_c2d_image[64];

/*bool s_debug_video = true;
double images_frame_num = 0.0;
u8* images_buffer;
int images_height[64];
int images_width[64];
stbi_uc* stb_images[64];*/

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
}

void RGBA_to_ABGR(u8* buf, u32 width, u32 height)
{
	// RGBA -> ABGR
	for (u32 row = 0; row < width; row++) {
		for (u32 col = 0; col < height; col++) {
			u32 z = (row + col * width) * 4;

			u8 r = *(u8*)(buf + z);
			u8 g = *(u8*)(buf + z + 1);
			u8 b = *(u8*)(buf + z + 2);
			u8 a = *(u8*)(buf + z + 3);

			*(buf + z) = a;
			*(buf + z + 1) = b;
			*(buf + z + 2) = g;
			*(buf + z + 3) = r;
		}
	}
}

Result_with_string Draw_C3DTexToC2DImage(C3D_Tex* c3d_tex, Tex3DS_SubTexture* c3d_subtex, u8* buf, u32 size, u32 width, u32 height, int parse_start_width, int parse_start_height, int tex_size, GPU_TEXCOLOR format)
{
	bool init_result = false;
	u32 x_max;
	u32 y_max;
	Result_with_string result;
	result.code = 0;
	result.string = "[Success] ";

	u32 subtex_width = width;
	u32 subtex_height = height;
	u32 pixel_size = (size / width / height);

	if (subtex_width > (u32)tex_size)
		subtex_width = (u32)tex_size;
	if (subtex_height > (u32)tex_size)
		subtex_height = (u32)tex_size;

	if (parse_start_width > (int)width || parse_start_height > (int)height)
	{
		if (parse_start_width > (int)width && parse_start_height > (int)height)
		{
			result.code = WRONG_PARSING_POS;
			result.string = "[Error] Parse's " + std::to_string(parse_start_width) + " is bigger than image's width " + std::to_string(width) + ", Parse's " + std::to_string(parse_start_height) + " is bigger than image's height " + std::to_string(height) + " ";
		}
		else if (parse_start_width > (int)width)
		{
			result.code = WRONG_PARSING_POS;
			result.string = "[Error] Parse's " + std::to_string(parse_start_width) + " is bigger than image's width " + std::to_string(width) + " ";
		}
		else if (parse_start_height > (int)height)
		{
			result.code = WRONG_PARSING_POS;
			result.string = "[Error] Parse's " + std::to_string(parse_start_height) + " is bigger than image's height " + std::to_string(height) + " ";
		}
		return result;
	}

	if ((linearSpaceFree() / 1024 / 1024) < 1)
	{
		result.code = OUT_OF_LINEAR_MEMORY;
		result.string = "[Error] C3D_TexInit aborted. ";
		return result;
	}
	else
		init_result = C3D_TexInit(c3d_tex, (u16)tex_size, (u16)tex_size, format);

	if (!init_result)
	{
		result.code = OUT_OF_LINEAR_MEMORY;
		result.string = "[Error] C3D_TexInit failed. ";
		return result;
	}

	c3d_subtex->width = (u16)subtex_width;
	c3d_subtex->height = (u16)subtex_height;
	c3d_subtex->left = 0.0f;
	c3d_subtex->top = 1.0f;
	c3d_subtex->right = subtex_width / (float)tex_size;
	c3d_subtex->bottom = 1.0 - subtex_height / (float)tex_size;

	memset(c3d_tex->data, 0x0, c3d_tex->size);
	C3D_TexSetFilter(c3d_tex, GPU_NEAREST, GPU_NEAREST);

	y_max = height - (u32)parse_start_height;
	x_max = width - (u32)parse_start_width;
	if ((u32)tex_size < y_max)
		y_max = tex_size;
	if ((u32)tex_size < x_max)
		x_max = tex_size;

	for (u32 y = 0; y <= y_max; y++)
	{
		for (u32 x = 0; x <= x_max; x++)
		{
			u32 dst_pos = ((((y >> 3)* ((u32)tex_size >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3)))* pixel_size;
			if (dst_pos <= c3d_tex->size)
			{
				u32 src_pos = (((y + parse_start_height) * width + (x + parse_start_width))) * pixel_size;
				memcpy(&((u8*)c3d_tex->data)[dst_pos], &((u8*)buf)[src_pos], pixel_size);
			}
		}
	}

	C3D_TexFlush(c3d_tex);
	c3d_tex->border = 0xFFFFFF;
	C3D_TexSetWrap(c3d_tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);

	return result;
}

void Imv_init(void)
{
	Log_log_save("Imv/Init", "Initializing...", 1234567890, s_debug_slow);

	/*images_buffer = (u8*)malloc(0x30000);
	memset(images_buffer, 0x0, 0x30000);*/

	imv_image_buffer = (u8*)malloc(0x500000);
	if (imv_image_buffer == NULL)
	{
		Err_set_error_message("Out of memory.", "Couldn't allocate 'imv image buffer'(" + std::to_string(0x500000 / 1024) + "KB).", "Imv/Init", OUT_OF_MEMORY);
		Log_log_save("Imv/Init", "[Error] Out of memory. ", OUT_OF_MEMORY, s_debug_slow);
		Err_set_error_show_flag(true);
	}
	else
		memset(imv_image_buffer, 0x0, 0x500000);

	Draw_progress("0/0 [Imv] Starting threads...");
	imv_dl_thread_run = true;
	imv_load_thread_run = true;
	imv_parse_thread_run = true;
	imv_dl_img_thread = threadCreate(Imv_img_dl_thread, (void*)(""), STACKSIZE, 0x30, -1, true);
	imv_load_img_thread = threadCreate(Imv_img_load_thread, (void*)(""), STACKSIZE, 0x30, -1, true);
	imv_parse_img_thread = threadCreate(Imv_img_parse_thread, (void*)(""), STACKSIZE, 0x27, -1, true);

	Imv_resume();
	imv_already_init = true;
	Log_log_save("Imv/Init", "Initialized.", 1234567890, s_debug_slow);
}

void Imv_main(void)
{
	int log_y = Log_query_y();
	int img_size_x = 0;
	int img_size_y = 0;
	int img_pos_x = imv_img_pos_x;
	int img_pos_y = imv_img_pos_y;
	int img_pos_x_offset = 0;
	int img_pos_y_offset = 0;
	double log_x = Log_query_x();
	float text_red;
	float text_green;
	float text_blue;
	float text_alpha;
	SwkbdState main_swkbd;
	SwkbdStatusData main_swkbd_status;

	if (imv_img_dl_request || imv_img_dl_and_parse_request)
		imv_img_dl_progress = Httpc_query_dl_progress();

	if (Sem_query_settings(SEM_NIGHT_MODE))
	{
		text_red = 1.0f;
		text_green = 1.0f;
		text_blue = 1.0f;
		text_alpha = 0.75f;
	}
	else
	{
		text_red = 0.0f;
		text_green = 0.0f;
		text_blue = 0.0f;
		text_alpha = 1.0f;
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

	osTickCounterUpdate(&s_tcount_frame_time);
	Draw_set_draw_mode(Sem_query_settings(SEM_VSYNC_MODE));

	if(Sem_query_settings(SEM_NIGHT_MODE))
		Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);

	/*if (s_debug_video)
		Draw_texture(imv_c2d_image, dammy_tint, (int)images_frame_num, (img_pos_x + img_pos_x_offset), (img_pos_y + img_pos_y_offset), img_size_x, img_size_y);
	else
	{*/
		for (int i = 0; i < 64; i++)
		{
			if (img_pos_x_offset > img_size_x * 7)
			{
				img_pos_x_offset = 0;
				img_pos_y_offset += img_size_y;
			}
			if(imv_enable[i])
				Draw_texture(imv_c2d_image, dammy_tint, i, (img_pos_x + img_pos_x_offset), (img_pos_y + img_pos_y_offset), img_size_x, img_size_y);

			img_pos_x_offset += img_size_x;
		}
	//}

	Draw_texture(Square_image, black_tint, 0, 0.0, 0.0, 400.0, 15.0);
	Draw_texture(Wifi_icon_image, dammy_tint, s_wifi_signal, 360.0, 0.0, 15.0, 15.0);
	Draw_texture(Battery_level_icon_image, dammy_tint, s_battery_level / 5, 330.0, 0.0, 30.0, 15.0);
	if (s_battery_charge)
		Draw_texture(Battery_charge_icon_image, dammy_tint, 0, 310.0, 0.0, 20.0, 15.0);
	Draw(s_status, 0, 0.0f, 0.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 1.0f);
	Draw(s_battery_level_string, 0, 337.5f, 1.25f, 0.4f, 0.4f, 0.0f, 0.0f, 0.0f, 0.5f);
	Draw_texture(Square_image, weak_aqua_tint, 0, 0.0, 15.0, 50.0 * imv_img_dl_progress, 3.0);

	if (Sem_query_settings(SEM_DEBUG_MODE))
		Draw_debug_info();
	if (Log_query_log_show_flag())
	{
		for (int i = 0; i < 23; i++)
			Draw(Log_query_log(log_y + i), 0, log_x, 10.0f + (i * 10), 0.4, 0.4, 0.0, 0.5, 1.0, 1.0);
	}

	if (Sem_query_settings(SEM_NIGHT_MODE))
		Draw_screen_ready_to_draw(1, true, 2, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(1, true, 2, 1.0, 1.0, 1.0);

	img_pos_x_offset = 0;
	img_pos_y_offset = 0;
	/*if (s_debug_video)
	{
		Draw_texture(imv_c2d_image, dammy_tint, (int)images_frame_num, (img_pos_x + img_pos_x_offset - 40), (img_pos_y + img_pos_y_offset - 240), img_size_x, img_size_y);
		images_frame_num += 0.5;
		if (images_frame_num >= 64.0)
			images_frame_num = 0.0;
	}
	else
	{*/
		for (int i = 0; i < 64; i++)
		{
			if (img_pos_x_offset > img_size_x * 7)
			{
				img_pos_x_offset = 0;
				img_pos_y_offset += img_size_y;
			}
			if (imv_enable[i])
				Draw_texture(imv_c2d_image, dammy_tint, i, (img_pos_x + img_pos_x_offset - 40), (img_pos_y + img_pos_y_offset - 240), img_size_x, img_size_y);

			img_pos_x_offset += img_size_x;
		}
	//}

	Draw(s_imv_ver, 0, 0.0, 0.0, 0.45, 0.45, 0.0, 1.0, 0.0, 1.0);
	Draw_texture(Square_image, weak_aqua_tint, 0, 10.0, 175.0, 65.0, 13.0);
	Draw_texture(Square_image, weak_aqua_tint, 0, 10.0, 195.0, 65.0, 13.0);
	Draw_texture(Square_image, weak_aqua_tint, 0, 90.0, 175.0, 65.0, 13.0);
	Draw_texture(Square_image, weak_aqua_tint, 0, 90.0, 195.0, 65.0, 13.0);
	Draw_texture(Square_image, weak_aqua_tint, 0, 170.0, 175.0, 65.0, 13.0);
	Draw_texture(Square_image, weak_aqua_tint, 0, 170.0, 195.0, 65.0, 13.0);
	Draw_texture(Square_image, weak_aqua_tint, 0, 250.0, 175.0, 65.0, 13.0);
	Draw_texture(Square_image, weak_aqua_tint, 0, 250.0, 195.0, 65.0, 13.0);

	if (s_setting[1] == "en")
	{
		Draw(imv_message_en[0], 0, 12.5, 175.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(imv_message_en[1], 0, 12.5, 195.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(imv_message_en[9], 0, 92.5, 175.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(imv_message_en[2], 0, 92.5, 195.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(imv_message_en[3], 0, 172.5, 175.0, 0.35, 0.35, text_red, text_green, text_blue, text_alpha);
		Draw(imv_message_en[4], 0, 172.5, 195.0, 0.35, 0.35, text_red, text_green, text_blue, text_alpha);
		Draw(imv_message_en[5], 0, 252.5, 175.0, 0.325, 0.325, text_red, text_green, text_blue, text_alpha);
		Draw(imv_message_en[6], 0, 252.5, 195.0, 0.325, 0.325, text_red, text_green, text_blue, text_alpha);
		Draw(imv_message_en[7] + std::to_string(imv_clipboard_selected_num) + "\n" + s_clipboards[imv_clipboard_selected_num], 0, 0.0, 200.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	}
	else if (s_setting[1] == "jp")
	{
		Draw(imv_message_jp[0], 0, 12.5, 175.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(imv_message_jp[1], 0, 12.5, 195.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(imv_message_jp[9], 0, 92.5, 175.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(imv_message_jp[2], 0, 92.5, 195.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(imv_message_jp[3], 0, 172.5, 175.0, 0.35, 0.35, text_red, text_green, text_blue, text_alpha);
		Draw(imv_message_jp[4], 0, 172.5, 195.0, 0.35, 0.35, text_red, text_green, text_blue, text_alpha);
		Draw(imv_message_jp[5], 0, 252.5, 175.0, 0.325, 0.325, text_red, text_green, text_blue, text_alpha);
		Draw(imv_message_jp[6], 0, 252.5, 195.0, 0.325, 0.325, text_red, text_green, text_blue, text_alpha);
		Draw(imv_message_jp[7] + std::to_string(imv_clipboard_selected_num) + "\n" + s_clipboards[imv_clipboard_selected_num], 0, 0.0, 200.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	}

	if (imv_select_file_request)
	{
		Draw_texture(Square_image, aqua_tint, 10, 10.0, 20.0, 300.0, 190.0);

		if (s_setting[1] == "en")
			Draw(imv_message_en[8], 0, 12.5, 185.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
		else if (s_setting[1] == "jp")
			Draw(imv_message_jp[8], 0, 12.5, 185.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);

		Draw(Expl_query_current_patch(), 0, 12.5, 195.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);
		for (int i = 0; i < 16; i++)
		{
			if (i == (int)Expl_query_selected_num(EXPL_SELECTED_FILE_NUM))
				Draw(Expl_query_file_name(i + (int)Expl_query_view_offset_y()) + "(" + Expl_query_type(i + (int)Expl_query_view_offset_y()) + ")", 0, 12.5, 20.0 + (i * 10.0), 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
			else
				Draw(Expl_query_file_name(i + (int)Expl_query_view_offset_y()) + "(" + Expl_query_type(i + (int)Expl_query_view_offset_y()) + ")", 0, 12.5, 20.0 + (i * 10.0), 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
		}
	}
	
	if (Err_query_error_show_flag())
		Draw_error();

	Draw_texture(Square_image, black_tint, 0, 0.0, 225.0, 320.0, 15.0);
	Draw(s_bot_button_string[1], 0, 30.0f, 220.0f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 1.0f);
	if (Hid_query_key_held_state(KEY_H_TOUCH))
		Draw(s_circle_string, 0, Hid_query_touch_pos(true), Hid_query_touch_pos(false), 0.20f, 0.20f, 1.0f, 0.0f, 0.0f, 1.0f);

	Draw_apply_draw();
	s_fps += 1;
	s_frame_time = osTickCounterRead(&s_tcount_frame_time);

	Hid_set_disable_flag(true);
	if (imv_adjust_url_request)
	{
		memset(s_swkb_input_text, 0x0, 8192);
		swkbdInit(&main_swkbd, SWKBD_TYPE_NORMAL, 2, 8192);
		swkbdSetHintText(&main_swkbd, "画像URLを入力 / Type image url here.");
		swkbdSetValidation(&main_swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
		swkbdSetFeatures(&main_swkbd, SWKBD_PREDICTIVE_INPUT);
		swkbdSetInitialText(&main_swkbd, s_clipboards[imv_clipboard_selected_num].c_str());

		swkbdSetStatusData(&main_swkbd, &main_swkbd_status, true, true);
		swkbdSetLearningData(&main_swkbd, &s_swkb_learn_data, true, true);
		s_swkb_press_button = swkbdInputText(&main_swkbd, s_swkb_input_text, 8192);
		if (s_swkb_press_button == SWKBD_BUTTON_RIGHT)
			s_clipboards[imv_clipboard_selected_num] = s_swkb_input_text;

		imv_adjust_url_request = false;
	}
}

void Imv_img_parse_thread(void* arg)
{
	Log_log_save("Imv/Img parse thread", "Thread started.", 1234567890, false);
	int log_num;
	int parse_start_pos_x;
	int parse_start_pos_y;
	double ideal_zoom_size;
	const char* failed_reason = (char*)malloc(0x100);
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

	/*int resize_result = 0;
	Handle image_parse_fs_handle = 0;
	FS_Archive image_parse_fs_archive = 0;
	if (s_debug_video)
	{
		u32 image_parse_read_size;
		FS_Archive image_parse_fs_archive = 0;
		Handle image_parse_fs_handle = 0;

		for (int i = 0; i < 64; i++)
		{
			log_num = Log_log_save("Imv/Img parse thread", "Share_load_from_file...", 1234567890, false);
			for (;;)
			{
				result = Share_load_from_file("debug_video_" + std::to_string(i) + ".bmp", images_buffer, 0x30000, &image_parse_read_size, "/Line/video/", image_parse_fs_handle, image_parse_fs_archive);
				if (result.code == 0)
					break;
				else
				{
					Err_set_error_message(result.string, result.error_description, "Imv/Image parse thread", result.code);
					Err_set_error_show_flag(true);
				}
			}
			Log_log_add(log_num, result.string, result.code, false);
		
			free(stb_images[i]);
			stb_images[i] = NULL;

			log_num = Log_log_save("Imv/Img parse thread", "Image data size : " + std::to_string(image_parse_read_size / 1024) + "KB" + " (" + std::to_string(image_parse_read_size) + "B)", 1234567890, false);
			stb_images[i] = stbi_load_from_memory((stbi_uc const*)images_buffer, (int)image_parse_read_size, &images_width[i], &images_height[i], NULL, STBI_rgb_alpha);
			Log_log_add(log_num, " w : " + std::to_string(images_width[i]) + " h : " + std::to_string(images_height[i]), 1234567890, false);
			if (stb_images[i] == NULL)
			{
				failed_reason = stbi_failure_reason();
				Err_clear_error_message();
				if (failed_reason == NULL)
				{
					Err_set_error_message("[Error] Unknown error.", "N/A", "Imv/Image parse thread", 1234567890);
					Log_log_save("Imv/Img parse thread", "Unknown error", 1234567890, false);
				}
				else
				{
					Err_set_error_message(failed_reason, "N/A", "Imv/Image parse thread", 1234567890);
					Log_log_save("Imv/Img parse thread", failed_reason, 1234567890, false);
				}
				Err_set_error_show_flag(true);
			}
			else
			{
				C3D_TexDelete(c3d_cache_tex[i]);
				memset(c3d_cache_tex[i]->data, 0x0, c3d_cache_tex[i]->size);
				imv_c2d_image[i].tex = c3d_cache_tex[i];
				imv_c2d_image[i].subtex = c3d_cache_subtex[i];
				RGBA_to_ABGR(stb_images[i], (u32)images_width[i], (u32)images_height[i]);

				log_num = Log_log_save("Imv/Img parse thread", "Draw_C3DTexToC2DImage...", 1234567890, false);
				result = Draw_C3DTexToC2DImage(c3d_cache_tex, c3d_cache_subtex, i, i, stb_images[i], (u32)(images_width[i] * images_height[i] * 4), (u32)images_width[i], (u32)images_height[i], 0, 0, 256, GPU_RGBA8);
				Log_log_add(log_num, result.string, result.code, false);
				if (result.code != 0)
				{
					Err_clear_error_message();
					Err_set_error_message(result.string, result.error_description, "Imv/Image parse thread", result.code);
					Err_set_error_show_flag(true);
				}
			}
		}
		imv_image_width = 200.0;
		imv_image_height = 200.0;
	}*/

	while (imv_parse_thread_run)
	{
		if (imv_thread_suspend)
			usleep(500000);
		else
		{
			if (imv_img_parse_request)
			{
				log_num = Log_log_save("Imv/Img parse thread", "APT_SetAppCpuTimeLimit_80...", 1234567890, false);
				result.code = APT_SetAppCpuTimeLimit(80);
				if (result.code == 0)
					Log_log_add(log_num, "[Success] ", result.code, false);
				else
					Log_log_add(log_num, "[Error] ", result.code, false);

				imv_image_height = 0;
				imv_image_width = 0;

				log_num = Log_log_save("Imv/Img parse thread", "Image data size : " + std::to_string(imv_image_size / 1024) + "KB" + " (" + std::to_string(imv_image_size) + "B)", 1234567890, false);
				free(stb_image);
				stb_image = NULL;
				stb_image = stbi_load_from_memory((stbi_uc const*)imv_image_buffer, (int)imv_image_size, &imv_image_width, &imv_image_height, NULL, STBI_rgb_alpha);
				Log_log_add(log_num, " w : " + std::to_string(imv_image_width) + " h : " + std::to_string(imv_image_height), 1234567890, false);

				if (stb_image == NULL)
				{
					failed_reason = stbi_failure_reason();
					Err_clear_error_message();
					if (failed_reason == NULL)
					{
						Err_set_error_message("[Error] Unknown error.", "N/A", "Imv/Image parse thread/stb_image", STB_IMG_RETURNED_NOT_SUCCESS);
						Log_log_save("Imv/Img parse thread/stb_image", "Unknown error ", STB_IMG_RETURNED_NOT_SUCCESS, false);
					}
					else
					{
						Err_set_error_message(failed_reason, "N/A", "Imv/Image parse thread/stb_image", STB_IMG_RETURNED_NOT_SUCCESS);
						Log_log_save("Imv/Img parse thread/stb_image", failed_reason, STB_IMG_RETURNED_NOT_SUCCESS, false);
					}
					Err_set_error_show_flag(true);
				}
				else
				{
					/*log_num = Log_log_save("Imv/Img parse thread", "Share_save_to_file(test)...", 1234567890, false);
					result = Share_save_to_file("test", (u8*)stb_image, (imv_image_width * imv_image_height * 4), "/Line/image/", true, image_parse_fs_handle, image_parse_fs_archive);
					Log_log_add(log_num, result.string, result.code, false);*/

					/*image_resize_width = imv_image_width / 2;
					image_resize_height = imv_image_height / 2;
					log_num = Log_log_save("Imv/Img parse thread", "Resizing...", 1234567890, false);
					free(stb_resized);
					stb_resized = (unsigned char*)malloc(image_resize_width * image_resize_height * 4);
					resize_result = stbir_resize_uint8(stb_image, imv_image_width, imv_image_height, 0, stb_resized, image_resize_width, image_resize_height, 0, 4);
					Log_log_add(log_num, "", resize_result, false);*/

					for (int i = 0; i < 64; i++)
					{
						C3D_TexDelete(c3d_cache_tex[i]);
						imv_c2d_image[i].tex = c3d_cache_tex[i];
						imv_c2d_image[i].subtex = c3d_cache_subtex[i];
						imv_enable[i] = false;
					}
					RGBA_to_ABGR(stb_image, (u32)imv_image_width, (u32)imv_image_height);

					parse_start_pos_x = 0;
					parse_start_pos_y = 0;

					for (int i = 0; i < 64; i++)
					{
						if (!(parse_start_pos_x > imv_image_width || parse_start_pos_y > imv_image_height))
						{
							imv_enable[i] = true;
							log_num = Log_log_save("Imv/Img parse thread", "Draw_C3DTexToC2DImage...", 1234567890, false);
							result = Draw_C3DTexToC2DImage(c3d_cache_tex[i], c3d_cache_subtex[i], stb_image, (u32)(imv_image_width * imv_image_height * 4), imv_image_width, imv_image_height, parse_start_pos_x, parse_start_pos_y, 512, GPU_RGBA8);
							Log_log_add(log_num, result.string, result.code, false);
							if (result.code != 0)
							{
								Err_set_error_message(result.string, result.error_description, "Imv/Image parse thread", result.code);
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
				log_num = Log_log_save("Imv/Img parse thread", "APT_SetAppCpuTimeLimit_30...", 1234567890, false);
				result.code = APT_SetAppCpuTimeLimit(30);
				if (result.code == 0)
					Log_log_add(log_num, "[Success] ", result.code, false);
				else
					Log_log_add(log_num, "[Error] ", result.code, false);
				
				imv_img_parse_request = false;
			}
		}
		usleep(100000);
	}

	for (int i = 0; i < 64; i++)
	{
		C3D_TexDelete(c3d_cache_tex[i]);
		linearFree(c3d_cache_tex[i]);
		linearFree(c3d_cache_subtex[i]);
		c3d_cache_tex[i] = NULL;
		c3d_cache_subtex[i] = NULL;
	}
	free(stb_image);
	stb_image = NULL;

	Log_log_save("Imv/Img parse thread", "Thread exit.", 1234567890, false);
}

void Imv_img_load_thread(void* arg)
{
	Log_log_save("Imv/Img load thread", "Thread started.", 1234567890, false);
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
				Err_set_error_message("[Error] Out of memory.", "Couldn't allocate 'fs buffer'(" + std::to_string(imv_img_fs_buffer_size / 1024) + "KB). ", "Imv/Img load thread", OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				Log_log_save("Imv/Img load thread", "[Error] Out of memory. ", OUT_OF_MEMORY, false);
			}
			else
			{
				log_num = Log_log_save("Imv/Img load thread/fs", "Share_load_from_file(" + imv_img_load_file_name + ")...", 1234567890, false);
				result = Share_load_from_file(imv_img_load_file_name, fs_buffer, imv_img_fs_buffer_size, &read_size, imv_img_load_dir_name, fs_handle, fs_archive);
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
					Err_set_error_message(result.string, result.error_description, "Imv/Img load thread/fs", result.code);
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
		usleep(100000);

		while (imv_thread_suspend)
			usleep(250000);
	}
	Log_log_save("Imv/Img load thread", "Thread exit.", 1234567890, false);

}

void Imv_img_dl_thread(void* arg)
{
	Log_log_save("Imv/Img dl thread", "Thread started.", 1234567890, false);
	u8* httpc_buffer;
	u32 dl_size;
	u32 status_code;
	int log_num;
	size_t cut_pos[2];
	std::string file_name;
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
				Err_set_error_message("[Error] Out of memory.", "Couldn't allocate 'httpc buffer'(" + std::to_string(imv_img_httpc_buffer_size / 1024) + "KB). ", "Imv/Img dl thread", OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				Log_log_save("Imv/Img dl thread", "[Error] Out of memory. ", OUT_OF_MEMORY, false);
			}
			else
			{
				file_name = s_clipboards[imv_clipboard_selected_num];
				log_num = Log_log_save("Imv/Img dl thread/httpc", "Downloading image....", 1234567890, false);
				result = Httpc_dl_data(s_clipboards[imv_clipboard_selected_num], httpc_buffer, imv_img_httpc_buffer_size, &dl_size, &status_code, true);
				Log_log_add(log_num, result.string + std::to_string(dl_size / 1024) + "KB (" + std::to_string(dl_size) + "B) ", result.code, false);
				imv_img_dl_progress = Httpc_query_dl_progress();
				if (result.code == 0)
				{
					imv_image_size = dl_size;
					memcpy(imv_image_buffer, httpc_buffer, imv_image_size);
					if (imv_img_dl_and_parse_request)
						imv_img_parse_request = true;

					cut_pos[0] = file_name.find("&id=");
					cut_pos[1] = file_name.find("lh3.googleusercontent.com/d/");
					if (cut_pos[0] == std::string::npos && cut_pos[1] == std::string::npos)
						file_name = std::to_string(s_months) + "_" + std::to_string(s_days) + "_" + std::to_string(s_hours) + "_" + std::to_string(s_minutes) + "_" + std::to_string(s_seconds);
					else if(!(cut_pos[0] == std::string::npos))
						file_name = file_name.substr(cut_pos[0] + 4);
					else if (!(cut_pos[1] == std::string::npos))
						file_name = file_name.substr(cut_pos[1] + 28);

					if (file_name.length() > 33)
						file_name = file_name.substr(0, 33);

					log_num = Log_log_save("Imv/Download thread/fs", "Save_to_file(" + file_name + ".jpg)...", 1234567890, false);
					result = Share_save_to_file(file_name + ".jpg", imv_image_buffer, (int)dl_size, "/Line/images/", true, fs_handle, fs_archive);
					Log_log_add(log_num, result.string, result.code, false);
				}
				else
				{
					Err_set_error_message(result.string, result.error_description, "Imv/Img dl thread/httpc", result.code);
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
		usleep(100000);

		while (imv_thread_suspend)
			usleep(250000);
	}
	Log_log_save("Imv/Img dl thread", "Thread exit.", 1234567890, false);
}

void Imv_exit(void)
{
	Log_log_save("Imv/Exit", "Exiting...", 1234567890, s_debug_slow);
	u64 time_out = 10000000000;
	int log_num;
	bool failed = false;
	Result_with_string result;

	Draw_progress("0/1 [Imv] Exiting threads...");
	imv_already_init = false;
	imv_dl_thread_run = false;
	imv_load_thread_run = false;
	imv_parse_thread_run = false;
	imv_thread_suspend = false;

	log_num = Log_log_save("Imv/Exit", "Exiting thread(0/2)...", 1234567890, s_debug_slow);
	result.code = threadJoin(imv_dl_img_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, "[Success] ", result.code, s_debug_slow);
	else
	{
		failed = true;
		Log_log_add(log_num, "[Error] ", result.code, s_debug_slow);
	}

	log_num = Log_log_save("Imv/Exit", "Exiting thread(1/2)...", 1234567890, s_debug_slow);
	result.code = threadJoin(imv_load_img_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, "[Success] ", result.code, s_debug_slow);
	else
	{
		failed = true;
		Log_log_add(log_num, "[Error] ", result.code, s_debug_slow);
	}

	log_num = Log_log_save("Imv/Exit", "Exiting thread(2/2)...", 1234567890, s_debug_slow);
	result.code = threadJoin(imv_parse_img_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, "[Success] ", result.code, s_debug_slow);
	else
	{
		failed = true;
		Log_log_add(log_num, "[Error] ", result.code, s_debug_slow);
	}

	Draw_progress("1/1 [Imv] Cleaning up...");
	free(imv_image_buffer);
	imv_image_buffer = NULL;

	if (failed)
		Log_log_save("Imv/Exit", "[Warn] Some function returned error.", 1234567890, s_debug_slow);

	Log_log_save("Imv/Exit", "Exited.", 1234567890, s_debug_slow);
}
