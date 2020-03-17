#include <string>
#include <cstring>
#include <unistd.h>
#include <3ds.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "draw.hpp"
#include "file.hpp"
#include "httpc.hpp"
#include "image_viewer.hpp"
#include "share_function.hpp"

u8* image_buffer;
u32 image_size;
int image_height = 0;
int image_width = 0;
int image_download_progress = 0;
bool parse_thread_started = false;
bool enable[64];
std::string image_url;

Thread image_parse_thread, image_dl_thread;
stbi_uc* stbi_image;
C2D_Image Downloaded_image[64];
C3D_Tex* c3d_cache_tex[64];
Tex3DS_SubTexture* c3d_cache_subtex[64];

/*bool s_debug_video = true;
double images_frame_num = 0.0;
u8* images_buffer;
int images_height[64];
int images_width[64];
stbi_uc* stbi_images[64];*/

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

Result_with_string Draw_C3DTexToC2DImage(C3D_Tex* c3d_tex[], Tex3DS_SubTexture* c3d_subtex[], int tex_num, int sub_tex_num, u8* buf, u32 size, u32 width, u32 height, int parse_start_width, int parse_start_height, int tex_size, GPU_TEXCOLOR format)
{
	bool init_result = false;
	Result_with_string c3d_to_c2d_result;
	c3d_to_c2d_result.code = 0;
	c3d_to_c2d_result.string = "[Success] ";

	u32 subtex_width = width;
	u32 subtex_height = height;
	u32 pixel_size = (size / width / height);

	if (subtex_width > (u32)tex_size)
		subtex_width = (u32)tex_size;
	if (subtex_height > (u32)tex_size)
		subtex_height = (u32)tex_size;

	c3d_subtex[sub_tex_num]->width = (u16)subtex_width;
	c3d_subtex[sub_tex_num]->height = (u16)subtex_height;
	c3d_subtex[sub_tex_num]->left = 0.0f;
	c3d_subtex[sub_tex_num]->top = 1.0f;
	c3d_subtex[sub_tex_num]->right = subtex_width / (float)tex_size;
	c3d_subtex[sub_tex_num]->bottom = 1.0 - subtex_height / (float)tex_size;

	if (parse_start_width > (int)width || parse_start_height > (int)height)
	{
		if (parse_start_width > (int)width && parse_start_height > (int)height)
		{
			c3d_to_c2d_result.code = -1;
			c3d_to_c2d_result.string = "[Error] Parse's " + std::to_string(parse_start_width) + " is bigger than image's width " + std::to_string(width) + ", Parse_start_height " + std::to_string(parse_start_height) + " is bigger than image's height " + std::to_string(height) + " ";
		}
		else if (parse_start_width > (int)width)
		{
			c3d_to_c2d_result.code = -2;
			c3d_to_c2d_result.string = "[Error] Parse's " + std::to_string(parse_start_width) + " is bigger than image's width " + std::to_string(width) + " ";
		}
		else if (parse_start_height > (int)height)
		{
			c3d_to_c2d_result.code = -3;
			c3d_to_c2d_result.string = "[Error] Parse's " + std::to_string(parse_start_height) + " is bigger than image's height " + std::to_string(height) + " ";
		}
		return c3d_to_c2d_result;
	}

	if ((linearSpaceFree() / 1024 / 1024) < 2)
	{
		c3d_to_c2d_result.code = -5;
		c3d_to_c2d_result.string = "[Error] C3D_TexInit aborted. ";
		return c3d_to_c2d_result;
	}
	else
		init_result = C3D_TexInit(c3d_tex[tex_num], (u16)tex_size, (u16)tex_size, format);

	if (!init_result)
	{
		c3d_to_c2d_result.code = -4;
		c3d_to_c2d_result.string = "[Error] C3D_TexInit failed. ";
		return c3d_to_c2d_result;
	}

	memset(c3d_tex[tex_num]->data, 0x0, c3d_tex[tex_num]->size);
	C3D_TexSetFilter(c3d_tex[tex_num], GPU_NEAREST, GPU_NEAREST);

	for (u32 y = 0; y <= (height - (u32)parse_start_height); y++)
	{
		for (u32 x = 0; x <= (width - (u32)parse_start_width); x++)
		{
			u32 dst_pos = ((((y >> 3)* ((u32)tex_size >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3)))* pixel_size;
			u32 src_pos = (((y + parse_start_height) * width + (x + parse_start_width))) * pixel_size;

			memcpy(&((u8*)c3d_tex[tex_num]->data)[dst_pos], &((u8*)buf)[src_pos], pixel_size);
		}
	}

	C3D_TexFlush(c3d_tex[tex_num]);
	c3d_tex[tex_num]->border = 0xFFFFFF;
	C3D_TexSetWrap(c3d_tex[tex_num], GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);

	return c3d_to_c2d_result;
}

void Image_viewer_init(void)
{
	Share_app_log_save("Imv/Init", "Initializing...", 1234567890, s_debug_slow);

	for (int i = 0; i < 64; i++)
	{
		c3d_cache_tex[i] = (C3D_Tex*)linearAlloc(sizeof(C3D_Tex));
		c3d_cache_subtex[i] = (Tex3DS_SubTexture*)linearAlloc(sizeof(Tex3DS_SubTexture));
		enable[i] = false;
	}

	/*images_buffer = (u8*)malloc(0x30000);
	memset(images_buffer, 0x0, 0x30000);*/

	image_buffer = (u8*)malloc(0x500000);
	if (image_buffer == NULL)
	{
		Share_clear_error_message();
		Share_set_error_message("Out of memory.", "Couldn't malloc to 'image_buffer'(" + std::to_string(0x500000 / 1024) + "KB).", "Imv/Init", OUT_OF_MEMORY);
		Share_app_log_save("Imv/Init", "[Error] Out of memory. ", OUT_OF_MEMORY, s_debug_slow);
		s_error_display = true;

		Image_viewer_exit();
		s_imv_thread_suspend = true;
		s_imv_main_run = false;
		s_menu_main_run = true;
		return;
	}
	else
		memset(image_buffer, 0x0, 0x500000);

	s_imv_download_thread_run = true;
	s_imv_parse_thread_run = true;
	image_dl_thread = threadCreate(Image_viewer_dl_thread, (void*)(""), STACKSIZE, 0x30, -1, true);
	image_parse_thread = threadCreate(Image_viewer_parse_thread, (void*)(""), STACKSIZE, 0x27, -1, true);
	/*for (int i = 0; i < 200; i++)//2s
	{
		usleep(10000);
		if (parse_thread_started)
			break;
	}

	if(!parse_thread_started)
		image_parse_thread = threadCreate(image_parse_thread, (void*)(""), STACKSIZE, 0x28, 1, true);
	*/

	s_imv_thread_suspend = false;
	s_imv_main_run = true;
	s_menu_main_run = false;
	s_imv_already_init = true;
	Share_app_log_save("Imv/Init", "Initialized.", 1234567890, s_debug_slow);
}

void Image_viewer_main(void)
{
	int image_size_x = 0;
	int image_size_y = 0;
	int image_pos_x_cache = 0;
	int image_pos_y_cache = 0;
	//int main_log_num_return = 0;
	float text_red;
	float text_green;
	float text_blue;
	float text_alpha;
	/*FS_Archive main_fs_archive = 0;
	Handle main_fs_handle = 0;*/
	SwkbdState main_swkbd;
	SwkbdStatusData main_swkbd_status;
	Result_with_string main_result;

	if (s_imv_image_dl_request)
		image_download_progress = Httpc_query_dl_progress();

	if (s_night_mode)
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

	if (image_width >= 512)
		image_size_x = 512 / 10;
	else
		image_size_x = image_width / 10;

	if (image_height >= 512)
		image_size_y = 512 / 10;
	else
		image_size_y = image_height / 10;

	image_size_x *= s_imv_image_zoom;
	image_size_y *= s_imv_image_zoom;

	osTickCounterUpdate(&s_tcount_frame_time);
	Draw_set_draw_mode(s_draw_vsync_mode);

	if(s_night_mode)
		Draw_screen_ready_to_draw(0, true, 1, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(0, true, 1, 1.0, 1.0, 1.0);

	/*if (s_debug_video)
		Draw_texture(Downloaded_image, dammy_tint, (int)images_frame_num, (s_imv_image_pos_x + image_pos_x_cache), (s_imv_image_pos_y + image_pos_y_cache), image_size_x, image_size_y);
	else
	{*/
		for (int i = 0; i < 64; i++)
		{
			if (image_pos_x_cache > image_size_x * 7)
			{
				image_pos_x_cache = 0;
				image_pos_y_cache += image_size_y;
			}
			image_pos_x_cache += image_size_x;
			if(enable[i])
				Draw_texture(Downloaded_image, dammy_tint, i, (s_imv_image_pos_x + image_pos_x_cache), (s_imv_image_pos_y + image_pos_y_cache), image_size_x, image_size_y);
		}
	//}

	Draw_texture(Background_image, dammy_tint, 0, 0.0, 0.0, 400.0, 15.0);
	Draw_texture(Wifi_icon_image, dammy_tint, s_wifi_signal, 360.0, 0.0, 15.0, 15.0);
	Draw_texture(Battery_level_icon_image, dammy_tint, s_battery_level / 5, 330.0, 0.0, 30.0, 15.0);
	if (s_battery_charge)
		Draw_texture(Battery_charge_icon_image, dammy_tint, 0, 310.0, 0.0, 20.0, 15.0);
	Draw(s_status, 0.0f, 0.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 1.0f);
	Draw(s_battery_level_string, 337.5f, 1.25f, 0.4f, 0.4f, 0.0f, 0.0f, 0.0f, 0.5f);
	Draw_texture(Square_image, dammy_tint, 10, 0.0, 15.0, 50.0 * image_download_progress, 3.0);

	if (s_debug_mode)
	{
		Draw_texture(Square_image, dammy_tint, 9, 0.0, 30.0, 230.0, 150.0);
		Draw("Key A press : " + std::to_string(s_key_A_press) + " Key A held : " + std::to_string(s_key_A_held), 0.0, 30.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key B press : " + std::to_string(s_key_B_press) + " Key B held : " + std::to_string(s_key_B_held), 0.0, 40.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key X press : " + std::to_string(s_key_X_press) + " Key X held : " + std::to_string(s_key_X_held), 0.0, 50.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key Y press : " + std::to_string(s_key_Y_press) + " Key Y held : " + std::to_string(s_key_Y_held), 0.0, 60.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key CPAD DOWN held : " + std::to_string(s_key_CPAD_DOWN_held), 0.0, 70.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key CPAD RIGHT held : " + std::to_string(s_key_CPAD_RIGHT_held), 0.0, 80.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key CPAD UP held : " + std::to_string(s_key_CPAD_UP_held), 0.0, 90.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key CPAD LEFT held : " + std::to_string(s_key_CPAD_LEFT_held), 0.0, 100.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Touch pos x : " + std::to_string(s_touch_pos_x) + " Touch pos y : " + std::to_string(s_touch_pos_y), 0.0, 110.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("X moved value : " + std::to_string(s_touch_pos_x_moved) + " Y moved value : " + std::to_string(s_touch_pos_y_moved), 0.0, 120.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Held time : " + std::to_string(s_held_time), 0.0, 130.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Drawing time(CPU/per frame) : " + std::to_string(C3D_GetProcessingTime()) + "ms", 0.0, 140.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Drawing time(GPU/per frame) : " + std::to_string(C3D_GetDrawingTime()) + "ms", 0.0, 150.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Free RAM (estimate) " + std::to_string(s_free_ram) + " MB", 0.0f, 160.0f, 0.4f, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Free linear RAM (estimate) " + std::to_string(s_free_linear_ram) + " MB", 0.0f, 170.0f, 0.4f, 0.4, text_red, text_green, text_blue, text_alpha);
	}
	if (s_app_logs_show)
	{
		for (int i = 0; i < 23; i++)
			Draw(s_app_logs[s_app_log_view_num + i], s_app_log_x, 10.0f + (i * 10), 0.4f, 0.4f, 0.0f, 0.5f, 1.0f, 1.0f);
	}

	if (s_night_mode)
		Draw_screen_ready_to_draw(1, true, 1, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(1, true, 1, 1.0, 1.0, 1.0);

	image_pos_x_cache = 0;
	image_pos_y_cache = 0;
	/*if (s_debug_video)
	{
		Draw_texture(Downloaded_image, dammy_tint, (int)images_frame_num, (s_imv_image_pos_x + image_pos_x_cache - 40), (s_imv_image_pos_y + image_pos_y_cache - 240), image_size_x, image_size_y);
		images_frame_num += 0.5;
		if (images_frame_num >= 64.0)
			images_frame_num = 0.0;
	}
	else
	{*/
		for (int i = 0; i < 64; i++)
		{
			if (image_pos_x_cache > image_size_x * 7)
			{
				image_pos_x_cache = 0;
				image_pos_y_cache += image_size_y;
			}
			image_pos_x_cache += image_size_x;
			if (enable[i])
				Draw_texture(Downloaded_image, dammy_tint, i, (s_imv_image_pos_x + image_pos_x_cache - 40), (s_imv_image_pos_y + image_pos_y_cache - 240), image_size_x, image_size_y);
		}
	//}

	Draw(s_imv_ver, 0.0, 0.0, 0.45, 0.45, 0.0, 1.0, 0.0, 1.0);
	Draw_texture(Square_image, dammy_tint, 11, 15.0, 175.0, 50.0, 20.0);
	Draw_texture(Square_image, dammy_tint, 11, 75.0, 175.0, 50.0, 20.0);
	Draw_texture(Square_image, dammy_tint, 11, 135.0, 175.0, 50.0, 20.0);
	Draw_texture(Square_image, dammy_tint, 11, 195.0, 175.0, 50.0, 20.0);
	Draw_texture(Square_image, dammy_tint, 11, 255.0, 175.0, 50.0, 20.0);
	Draw_texture(Square_image, dammy_tint, 11, 145.0, 200.0, 75.0, 20.0);
	Draw_texture(Square_image, dammy_tint, 11, 245.0, 200.0, 75.0, 20.0);

	if (s_setting[1] == "en")
	{
		Draw(s_imv_message_en[0], 17.5, 175.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(s_imv_message_en[1], 77.5, 175.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(s_imv_message_en[2], 137.5, 175.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(s_imv_message_en[3], 197.5, 175.0, 0.35, 0.35, text_red, text_green, text_blue, text_alpha);
		Draw(s_imv_message_en[4], 257.5, 175.0, 0.35, 0.35, text_red, text_green, text_blue, text_alpha);
		Draw(s_imv_message_en[5], 147.5, 200.0, 0.325, 0.325, text_red, text_green, text_blue, text_alpha);
		Draw(s_imv_message_en[6], 247.5, 200.0, 0.325, 0.325, text_red, text_green, text_blue, text_alpha);
		Draw(s_imv_message_en[7] + std::to_string(s_imv_clipboard_select_num) + "\n" + s_clipboards[s_imv_clipboard_select_num], 0.0, 200.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	}
	else if (s_setting[1] == "jp")
	{
		Draw(s_imv_message_jp[0], 17.5, 175.0, 0.35, 0.35, text_red, text_green, text_blue, text_alpha);
		Draw(s_imv_message_jp[1], 77.5, 175.0, 0.35, 0.35, text_red, text_green, text_blue, text_alpha);
		Draw(s_imv_message_jp[2], 137.5, 175.0, 0.35, 0.35, text_red, text_green, text_blue, text_alpha);
		Draw(s_imv_message_jp[3], 197.5, 175.0, 0.35, 0.35, text_red, text_green, text_blue, text_alpha);
		Draw(s_imv_message_jp[4], 257.5, 175.0, 0.35, 0.35, text_red, text_green, text_blue, text_alpha);
		Draw(s_imv_message_jp[5], 147.5, 200.0, 0.325, 0.325, text_red, text_green, text_blue, text_alpha);
		Draw(s_imv_message_jp[6], 247.5, 200.0, 0.325, 0.325, text_red, text_green, text_blue, text_alpha);
		Draw(s_imv_message_jp[7] + std::to_string(s_imv_clipboard_select_num) + "\n" + s_clipboards[s_imv_clipboard_select_num], 0.0, 200.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	}

	if (s_error_display)
		Share_draw_error();

	Draw_texture(Background_image, dammy_tint, 1, 0.0, 225.0, 320.0, 15.0);
	Draw(s_bot_button_string[1], 30.0f, 220.0f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 1.0f);
	if (s_key_touch_held)
		Draw(s_circle_string, s_touch_pos_x, s_touch_pos_y, 0.20f, 0.20f, 1.0f, 0.0f, 0.0f, 1.0f);

	Draw_apply_draw();
	s_fps += 1;
	s_frame_time = osTickCounterRead(&s_tcount_frame_time);

	s_hid_disabled = true;
	if (s_imv_adjust_url_request)
	{
		memset(s_swkb_input_text, 0x0, 8192);
		swkbdInit(&main_swkbd, SWKBD_TYPE_NORMAL, 2, 8192);
		swkbdSetHintText(&main_swkbd, "‰æ‘œURL‚ð“ü—Í / Type image url here.");
		swkbdSetValidation(&main_swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
		swkbdSetFeatures(&main_swkbd, SWKBD_PREDICTIVE_INPUT);
		swkbdSetInitialText(&main_swkbd, s_clipboards[s_imv_clipboard_select_num].c_str());

		swkbdSetStatusData(&main_swkbd, &main_swkbd_status, true, true);
		swkbdSetLearningData(&main_swkbd, &s_swkb_learn_data, true, true);
		s_swkb_press_button = swkbdInputText(&main_swkbd, s_swkb_input_text, 8192);
		if (s_swkb_press_button == SWKBD_BUTTON_RIGHT)
			s_clipboards[s_imv_clipboard_select_num] = s_swkb_input_text;

		s_imv_adjust_url_request = false;
	}
}

void Image_viewer_parse_thread(void* arg)
{
	Share_app_log_save("Imv/Parse thread", "Thread started.", 1234567890, false);
	parse_thread_started = true;
	int image_parse_log_num_return;
	int image_parse_start_pos_x;
	int image_parse_start_pos_y;
	const char* failed_msg = (char*)malloc(0x100);
	Result_with_string image_parse_result;

	/*if (s_debug_video)
	{
		u32 image_parse_read_size;
		FS_Archive image_parse_fs_archive = 0;
		Handle image_parse_fs_handle = 0;

		for (int i = 0; i < 64; i++)
		{
			image_parse_log_num_return = Share_app_log_save("Imv/Parse thread", "Share_load_from_file...", 1234567890, false);
			for (;;)
			{
				image_parse_result = Share_load_from_file("debug_video_" + std::to_string(i) + ".bmp", images_buffer, 0x30000, &image_parse_read_size, "/Line/video/", image_parse_fs_handle, image_parse_fs_archive);
				if (image_parse_result.code == 0)
					break;
				else
				{
					Share_set_error_message(image_parse_result.string, image_parse_result.error_description, "Imv/Image parse thread", image_parse_result.code);
					s_error_display = true;
				}
			}
			Share_app_log_add_result(image_parse_log_num_return, image_parse_result.string, image_parse_result.code, false);
		
			free(stbi_images[i]);
			stbi_images[i] = NULL;

			image_parse_log_num_return = Share_app_log_save("Imv/Parse thread", "Image data size : " + std::to_string(image_parse_read_size / 1024) + "KB" + " (" + std::to_string(image_parse_read_size) + "B)", 1234567890, false);
			stbi_images[i] = stbi_load_from_memory((stbi_uc const*)images_buffer, (int)image_parse_read_size, &images_width[i], &images_height[i], NULL, STBI_rgb_alpha);
			Share_app_log_add_result(image_parse_log_num_return, " w : " + std::to_string(images_width[i]) + " h : " + std::to_string(images_height[i]), 1234567890, false);
			if (stbi_images[i] == NULL)
			{
				failed_msg = stbi_failure_reason();
				Share_clear_error_message();
				if (failed_msg == NULL)
				{
					Share_set_error_message("[Error] Unknown error.", "N/A", "Imv/Image parse thread", 1234567890);
					Share_app_log_save("Imv/Parse thread", "Unknown error", 1234567890, false);
				}
				else
				{
					Share_set_error_message(failed_msg, "N/A", "Imv/Image parse thread", 1234567890);
					Share_app_log_save("Imv/Parse thread", failed_msg, 1234567890, false);
				}
				s_error_display = true;
			}
			else
			{
				C3D_TexDelete(c3d_cache_tex[i]);
				memset(c3d_cache_tex[i]->data, 0x0, c3d_cache_tex[i]->size);
				Downloaded_image[i].tex = c3d_cache_tex[i];
				Downloaded_image[i].subtex = c3d_cache_subtex[i];
				RGBA_to_ABGR(stbi_images[i], (u32)images_width[i], (u32)images_height[i]);

				image_parse_log_num_return = Share_app_log_save("Imv/Parse thread", "Draw_C3DTexToC2DImage...", 1234567890, false);
				image_parse_result = Draw_C3DTexToC2DImage(c3d_cache_tex, c3d_cache_subtex, i, i, stbi_images[i], (u32)(images_width[i] * images_height[i] * 4), (u32)images_width[i], (u32)images_height[i], 0, 0, 256, GPU_RGBA8);
				Share_app_log_add_result(image_parse_log_num_return, image_parse_result.string, image_parse_result.code, false);
				if (image_parse_result.code != 0)
				{
					Share_clear_error_message();
					Share_set_error_message(image_parse_result.string, image_parse_result.error_description, "Imv/Image parse thread", image_parse_result.code);
					s_error_display = true;
				}
			}
		}
		image_width = 200.0;
		image_height = 200.0;
	}*/

	while (s_imv_parse_thread_run)
	{
		if (s_imv_thread_suspend)
			usleep(500000);
		else
		{
			if (s_imv_image_parse_request)
			{
				image_parse_log_num_return = Share_app_log_save("Imv/Parse thread", "APT_SetAppCpuTimeLimit_80...", 1234567890, false);
				image_parse_result.code = APT_SetAppCpuTimeLimit(80);
				if (image_parse_result.code == 0)
					Share_app_log_add_result(image_parse_log_num_return, "[Success] ", image_parse_result.code, false);
				else
					Share_app_log_add_result(image_parse_log_num_return, "[Error] ", image_parse_result.code, false);

				image_height = 0;
				image_width = 0;

				image_parse_log_num_return = Share_app_log_save("Imv/Parse thread", "Image data size : " + std::to_string(image_size / 1024) + "KB" + " (" + std::to_string(image_size) + "B)", 1234567890, false);
				free(stbi_image);
				stbi_image = NULL;
				stbi_image = stbi_load_from_memory((stbi_uc const*)image_buffer, (int)image_size, &image_width, &image_height, NULL, STBI_rgb_alpha);
				Share_app_log_add_result(image_parse_log_num_return, " w : " + std::to_string(image_width) + " h : " + std::to_string(image_height), 1234567890, false);


				if (stbi_image == NULL)
				{
					failed_msg = stbi_failure_reason();
					Share_clear_error_message();
					if (failed_msg == NULL)
					{
						Share_set_error_message("[Error] Unknown error.", "N/A", "Imv/Image parse thread", 1234567890);
						Share_app_log_save("Imv/Parse thread", "Unknown error", 1234567890, false);
					}
					else
					{
						Share_set_error_message(failed_msg, "N/A", "Imv/Image parse thread", 1234567890);
						Share_app_log_save("Imv/Parse thread", failed_msg, 1234567890, false);
					}
					s_error_display = true;
				}
				else
				{
					for (int i = 0; i < 64; i++)
					{
						C3D_TexDelete(c3d_cache_tex[i]);
						memset(c3d_cache_tex[i]->data, 0x0, c3d_cache_tex[i]->size);						
						Downloaded_image[i].tex = c3d_cache_tex[i];
						Downloaded_image[i].subtex = c3d_cache_subtex[i];
						enable[i] = false;
					}
					RGBA_to_ABGR(stbi_image, (u32)image_width, (u32)image_height);

					image_parse_start_pos_x = 0;
					image_parse_start_pos_y = 0;

					for (int i = 0; i < 64; i++)
					{
						if (!(image_parse_start_pos_x > image_width || image_parse_start_pos_y > image_height))
						{
							enable[i] = true;
							image_parse_log_num_return = Share_app_log_save("Imv/Parse thread", "Draw_C3DTexToC2DImage...", 1234567890, false);
							image_parse_result = Draw_C3DTexToC2DImage(c3d_cache_tex, c3d_cache_subtex, i, i, stbi_image, (u32)(image_width * image_height * 4), (u32)image_width, (u32)image_height, image_parse_start_pos_x, image_parse_start_pos_y, 512, GPU_RGBA8);
							Share_app_log_add_result(image_parse_log_num_return, image_parse_result.string, image_parse_result.code, false);
							if (image_parse_result.code != 0)
							{
								Share_clear_error_message();
								Share_set_error_message(image_parse_result.string, image_parse_result.error_description, "Imv/Image parse thread", image_parse_result.code);
								s_error_display = true;
							}
						}

						image_parse_start_pos_x += 512;
						if (image_parse_start_pos_x >= 4096)
						{
							image_parse_start_pos_x = 0;
							image_parse_start_pos_y += 512;
						}
					}

				}
				image_parse_log_num_return = Share_app_log_save("Imv/Parse thread", "APT_SetAppCpuTimeLimit_30...", 1234567890, false);
				image_parse_result.code = APT_SetAppCpuTimeLimit(30);
				if (image_parse_result.code == 0)
					Share_app_log_add_result(image_parse_log_num_return, "[Success] ", image_parse_result.code, false);
				else
					Share_app_log_add_result(image_parse_log_num_return, "[Error] ", image_parse_result.code, false);

				s_imv_image_parse_request = false;
			}
		}
		usleep(100000);
	}
	parse_thread_started = false;
	Share_app_log_save("Imv/Parse thread", "Thread exit.", 1234567890, false);
}

void Image_viewer_dl_thread(void* arg)
{
	Share_app_log_save("Imv/Image dl thread", "Thread started.", 1234567890, false);
	u8* image_httpc_buffer;
	u32 downloaded_image_size;
	u32 status_code;
	int image_download_log_num_return;
	std::string downloaded_data_string;
	//FS_Archive image_download_fs_archive = 0;
	//Handle image_download_fs_handle = 0;
	Result_with_string image_download_result;

	while (s_imv_download_thread_run)
	{
		if (s_imv_image_dl_request)
		{
			downloaded_image_size = 0;
			status_code = 0;
			image_httpc_buffer = (u8*)malloc(s_imv_image_httpc_buffer_size);
			if (image_httpc_buffer == NULL)
			{
				Share_clear_error_message();
				Share_set_error_message("[Error] Out of memory.", "Couldn't malloc to 'image_httpc_buffer'(" + std::to_string(s_imv_image_httpc_buffer_size / 1024) + "KB)", "Imv/Image dl thread", OUT_OF_MEMORY);
				s_error_display = true;
				Share_app_log_save("Imv/Image dl thread", "[Error] Out of memory.", OUT_OF_MEMORY, false);
			}
			else
			{
				image_download_log_num_return = Share_app_log_save("Imv/Image dl thread", "Downloading image....", 1234567890, false);
				image_download_result = Httpc_dl_data(s_clipboards[s_imv_clipboard_select_num], image_httpc_buffer, s_imv_image_httpc_buffer_size, &downloaded_image_size, &status_code, true);
				Share_app_log_add_result(image_download_log_num_return, image_download_result.string + std::to_string(downloaded_image_size / 1024) + "KB (" + std::to_string(downloaded_image_size) + "B) ", image_download_result.code, false);
				image_download_progress = Httpc_query_dl_progress();
				if (image_download_result.code == 0)
				{
					image_size = downloaded_image_size;
					memcpy(image_buffer, image_httpc_buffer, image_size);
				}
				else
				{
					Share_clear_error_message();
					Share_set_error_message(image_download_result.string, image_download_result.error_description, "Imv/Image dl thread", image_download_result.code);
					s_error_display = true;
				}
			}

			/*image_download_log_num_return = Share_app_log_save("Imv/Download thread/fs", "Save_to_file...", 1234567890, false);
			image_download_result = Share_save_to_file(std::to_string(s_months) + "_" + std::to_string(s_days) + "_" + std::to_string(s_hours) + "_" + std::to_string(s_minutes) + "_" + std::to_string(s_seconds) + ".jpg", image_buffer, (int)downloaded_image_size, "/Line/images/", true, image_download_fs_handle, image_download_fs_archive);
			Share_app_log_add_result(image_download_log_num_return, image_download_result.string, image_download_result.code, false);*/

			free(image_httpc_buffer);
			image_httpc_buffer = NULL;
			s_imv_image_dl_request = false;
		}
		usleep(100000);

		while (s_imv_thread_suspend)
			usleep(250000);
	}
	Share_app_log_save("Imv/Image dl thread", "Thread exit.", 1234567890, false);
}

void Image_viewer_exit(void)
{
	Share_app_log_save("Imv/Exit", "Exiting...", 1234567890, s_debug_slow);
	u64 time_out = 10000000000;
	int exit_log_num_return;
	bool function_fail = false;
	Result_with_string exit_result;
	exit_result.code = 0;
	exit_result.string = "[Success] ";

	s_imv_already_init = false;
	s_imv_thread_suspend = false;
	s_imv_download_thread_run = false;
	s_imv_parse_thread_run = false;

	exit_log_num_return = Share_app_log_save("Imv/Exit", "Thread exiting(0/1)...", 1234567890, s_debug_slow);
	exit_result.code = threadJoin(image_dl_thread, time_out);
	if (exit_result.code == 0)
		Share_app_log_add_result(exit_log_num_return, "[Success] ", exit_result.code, s_debug_slow);
	else
	{
		function_fail = true;
		Share_app_log_add_result(exit_log_num_return, "[Error] ", exit_result.code, s_debug_slow);
	}

	exit_log_num_return = Share_app_log_save("Imv/Exit", "Thread exiting(1/1)...", 1234567890, s_debug_slow);
	exit_result.code = threadJoin(image_parse_thread, time_out);
	if (exit_result.code == 0)
		Share_app_log_add_result(exit_log_num_return, "[Success] ", exit_result.code, s_debug_slow);
	else
	{
		function_fail = true;
		Share_app_log_add_result(exit_log_num_return, "[Error] ", exit_result.code, s_debug_slow);
	}

	for (int i = 0; i < 64; i++)
	{
		C3D_TexDelete(c3d_cache_tex[i]);
		memset(c3d_cache_tex[i]->data, 0x0, c3d_cache_tex[i]->size);
		linearFree(c3d_cache_tex[i]);
		linearFree(c3d_cache_subtex[i]);
		c3d_cache_tex[i] = NULL;
		c3d_cache_subtex[i] = NULL;
	}
	free(image_buffer);
	free(stbi_image);
	image_buffer = NULL;
	stbi_image = NULL;

	if (function_fail)
		Share_app_log_save("Imv/Exit", "[Warn] Some function returned error.", 1234567890, s_debug_slow);

	Share_app_log_save("Imv/Exit", "Exited.", 1234567890, s_debug_slow);
}
