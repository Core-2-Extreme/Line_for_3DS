#include <string>
#include <cstring>
#include <unistd.h>
#include <3ds.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "draw.hpp"
#include "file.hpp"
#include "image_viewer.hpp"
#include "share_function.hpp"

u8* image_buffer;
u32 image_size;
int pic_height = 0;
int pic_width = 0;
int image_download_progress = 0;
bool pic_download_request = false;
bool pic_parse_request = false;
bool parse_thread_started = false;
std::string image_url;

Thread image_viewer_parse_thread, image_viewer_download_thread;
stbi_uc* stbi_image;
C2D_Image Downloaded_image[16];
C3D_Tex* c3d_cache_tex[16];
Tex3DS_SubTexture* c3d_cache_subtex[16];

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
			c3d_to_c2d_result.string = "[Error] Parse's " + std::to_string(parse_start_width) + " is bigger than pic's width " + std::to_string(width) + ", Parse_start_height " + std::to_string(parse_start_height) + " is bigger than pic's height " + std::to_string(height) + " ";
		}
		else if (parse_start_width > (int)width)
		{
			c3d_to_c2d_result.code = -2;
			c3d_to_c2d_result.string = "[Error] Parse's " + std::to_string(parse_start_width) + " is bigger than pic's width " + std::to_string(width) + " ";
		}
		else if (parse_start_height > (int)height)
		{
			c3d_to_c2d_result.code = -3;
			c3d_to_c2d_result.string = "[Error] Parse's " + std::to_string(parse_start_height) + " is bigger than pic's height " + std::to_string(height) + " ";
		}
		return c3d_to_c2d_result;
	}

	bool init_result = C3D_TexInit(c3d_tex[tex_num], (u16)tex_size, (u16)tex_size, format);
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
	Share_app_log_save("Imv/Init", "Initializing...", 1234567890, true);
	s_imv_already_init = true;

	for (int i = 0; i <= 15; i++)
	{
		c3d_cache_tex[i] = (C3D_Tex*)linearAlloc(sizeof(C3D_Tex));
		c3d_cache_subtex[i] = (Tex3DS_SubTexture*)linearAlloc(sizeof(Tex3DS_SubTexture));
	}

	image_buffer = (u8*)malloc(0x100000);
	memset(image_buffer, 0x0, 0x100000);

	s_imv_download_thread_run = true;
	s_imv_parse_thread_run = true;
	image_viewer_download_thread = threadCreate(Image_viewer_download_thread, (void*)(""), STACKSIZE, 0x30, -1, true);
	image_viewer_parse_thread = threadCreate(Image_viewer_parse_thread, (void*)(""), STACKSIZE, 0x28, -1, true);
	/*for (int i = 0; i < 200; i++)//2s
	{
		usleep(10000);
		if (parse_thread_started)
			break;
	}

	if(!parse_thread_started)
		image_viewer_parse_thread = threadCreate(Image_viewer_parse_thread, (void*)(""), STACKSIZE, 0x28, 1, true);
	*/
	Share_app_log_save("Imv/Init", "Initialized.", 1234567890, true);
}

void Image_viewer_main(void)
{
	int pic_size_x = 0;
	int pic_size_y = 0;
	int pic_pos_x_cache = 0;
	int pic_pos_y_cache = 0;
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

	if (pic_width >= 512)
		pic_size_x = 512 / 10;
	else
		pic_size_x = pic_width / 10;

	if (pic_height >= 512)
		pic_size_y = 512 / 10;
	else
		pic_size_y = pic_height / 10;

	pic_size_x *= s_imv_image_zoom;
	pic_size_y *= s_imv_image_zoom;

	osTickCounterUpdate(&s_tcount_frame_time);

	Draw_set_draw_mode(s_draw_vsync_mode);

	if(s_night_mode)
		Draw_screen_ready_to_draw(0, true, 1, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(0, true, 1, 1.0, 1.0, 1.0);

	for (int i = 0; i <= 15; i++)
	{
		if (pic_pos_x_cache > pic_size_x * 3)
		{
			pic_pos_x_cache = 0;
			pic_pos_y_cache += pic_size_y;
		}
		pic_pos_x_cache += pic_size_x;
		Draw_texture(Downloaded_image, dammy_tint, i, (s_imv_image_pos_x + pic_pos_x_cache), (s_imv_image_pos_y + pic_pos_y_cache), pic_size_x, pic_size_y);
	}

	Draw_texture(Background_image, dammy_tint, 0, 0.0, 0.0, 400.0, 15.0);
	Draw_texture(Wifi_icon_image, dammy_tint, s_wifi_signal, 360.0, 0.0, 15.0, 15.0);
	Draw_texture(Battery_level_icon_image, dammy_tint, s_battery_level / 5, 330.0, 0.0, 30.0, 15.0);
	if (s_battery_charge)
		Draw_texture(Battery_charge_icon_image, dammy_tint, 0, 310.0, 0.0, 20.0, 15.0);
	Draw(s_status, 0.0f, 0.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 1.0f);
	Draw(s_battery_level_string, 337.5f, 1.25f, 0.4f, 0.4f, 0.0f, 0.0f, 0.0f, 0.5f);
	Draw_texture(Square_image, dammy_tint, 10, 0.0, 15.0, 50 * image_download_progress, 3.5);

	if (s_debug_mode)
	{
		Draw_texture(Square_image, dammy_tint, 9, 0.0, 50.0, 230.0, 140.0);
		Draw("Key A press : " + std::to_string(s_key_A_press) + " Key A held : " + std::to_string(s_key_A_held), 0.0, 50.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key B press : " + std::to_string(s_key_B_press) + " Key B held : " + std::to_string(s_key_B_held), 0.0, 60.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key X press : " + std::to_string(s_key_X_press) + " Key X held : " + std::to_string(s_key_X_held), 0.0, 70.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key Y press : " + std::to_string(s_key_Y_press) + " Key Y held : " + std::to_string(s_key_Y_held), 0.0, 80.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key CPAD DOWN held : " + std::to_string(s_key_CPAD_DOWN_held), 0.0, 90.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key CPAD RIGHT held : " + std::to_string(s_key_CPAD_RIGHT_held), 0.0, 100.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key CPAD UP held : " + std::to_string(s_key_CPAD_UP_held), 0.0, 110.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key CPAD LEFT held : " + std::to_string(s_key_CPAD_LEFT_held), 0.0, 120.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Touch pos x : " + std::to_string(s_touch_pos_x) + " Touch pos y : " + std::to_string(s_touch_pos_y), 0.0, 130.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("X moved value : " + std::to_string(s_touch_pos_x_moved) + " Y moved value : " + std::to_string(s_touch_pos_y_moved), 0.0, 140.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Held time : " + std::to_string(s_held_time), 0.0, 150.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Drawing time(CPU/per frame) : " + std::to_string(C3D_GetProcessingTime()) + "ms", 0.0, 160.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Drawing time(GPU/per frame) : " + std::to_string(C3D_GetDrawingTime()) + "ms", 0.0, 170.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Free RAM (estimate) " + std::to_string(s_free_ram) + " MB", 0.0f, 180.0f, 0.4f, 0.4, text_red, text_green, text_blue, text_alpha);
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

	pic_pos_x_cache = 0;
	pic_pos_y_cache = 0;
	for (int i = 0; i <= 15; i++)
	{
		if (pic_pos_x_cache > pic_size_x * 3)
		{
			pic_pos_x_cache = 0;
			pic_pos_y_cache += pic_size_y;
		}
		pic_pos_x_cache += pic_size_x;
		Draw_texture(Downloaded_image, dammy_tint, i, (s_imv_image_pos_x + pic_pos_x_cache - 40), (s_imv_image_pos_y + pic_pos_y_cache - 240), pic_size_x, pic_size_y);
	}

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

	Draw_texture(Background_image, dammy_tint, 1, 0.0, 225.0, 320.0, 15.0);
	Draw(s_bot_button_string[1], 30.0f, 220.0f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 1.0f);
	if (s_key_touch_held)
		Draw(s_circle_string, s_touch_pos_x, s_touch_pos_y, 0.20f, 0.20f, 1.0f, 0.0f, 0.0f, 1.0f);

	Draw_apply_draw();
	s_fps += 1;
	s_frame_time = osTickCounterRead(&s_tcount_frame_time);

	s_hid_disabled = true;
	if (s_key_A_press || (s_key_touch_press && s_touch_pos_x > 15 && s_touch_pos_x < 65 && s_touch_pos_y > 175 && s_touch_pos_y < 195))
		pic_parse_request = true;
	if (s_key_B_press || (s_key_touch_press && s_touch_pos_x > 75 && s_touch_pos_x < 125 && s_touch_pos_y > 175 && s_touch_pos_y < 195))
		pic_download_request = true;
	if (s_key_Y_press || (s_key_touch_press && s_touch_pos_x > 135 && s_touch_pos_x < 185 && s_touch_pos_y > 175 && s_touch_pos_y < 195))
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
	}

}

void Image_viewer_parse_thread(void* arg)
{
	Share_app_log_save("Imv/Parse thread", "Thread started.", 1234567890, false);
	parse_thread_started = true;
	int pic_parse_log_num_return;
	int pic_parse_start_pos_x;
	int pic_parse_start_pos_y;
	const char* failed_msg = (char*)malloc(0x100);
	Result_with_string pic_parse_result;

	while (s_imv_parse_thread_run)
	{
		if (s_imv_thread_suspend)
			usleep(500000);
		else
		{
			if (pic_parse_request)
			{
				pic_parse_log_num_return = Share_app_log_save("Imv/Parse thread", "APT_SetAppCpuTimeLimit_80...", 1234567890, false);
				pic_parse_result.code = APT_SetAppCpuTimeLimit(80);
				if (pic_parse_result.code == 0)
				{
					Share_app_log_add_result(pic_parse_log_num_return, "[Success] ", pic_parse_result.code, false);
				}
				else
					Share_app_log_add_result(pic_parse_log_num_return, "[Error] ", pic_parse_result.code, false);

				pic_parse_request = false;
				pic_height = 0;
				pic_width = 0;

				pic_parse_log_num_return = Share_app_log_save("Imv/Parse thread", "Pic data size : " + std::to_string(image_size / 1024) + "KB" + " (" + std::to_string(image_size) + "B)", 1234567890, false);
				free(stbi_image);
				stbi_image = NULL;
				stbi_image = stbi_load_from_memory((stbi_uc const*)image_buffer, (int)image_size, &pic_width, &pic_height, NULL, STBI_rgb_alpha);
				Share_app_log_add_result(pic_parse_log_num_return, " w : " + std::to_string(pic_width) + " h : " + std::to_string(pic_height), 1234567890, false);


				if (stbi_image == NULL)
				{
					failed_msg = stbi_failure_reason();
					if (failed_msg == NULL)
						Share_app_log_save("Imv/Parse thread", "unknown error", 1234567890, false);
					else
						Share_app_log_save("Imv/Parse thread", failed_msg, 1234567890, false);
				}
				else
				{
					for (int i = 0; i < 16; i++)
					{
						C3D_TexDelete(c3d_cache_tex[i]);
						memset(c3d_cache_tex[i]->data, 0x0, c3d_cache_tex[i]->size);						
						Downloaded_image[i].tex = c3d_cache_tex[i];
						Downloaded_image[i].subtex = c3d_cache_subtex[i];
					}
					RGBA_to_ABGR(stbi_image, (u32)pic_width, (u32)pic_height);

					pic_parse_start_pos_x = 0;
					pic_parse_start_pos_y = 0;

					for (int i = 0; i < 16; i++)
					{
						pic_parse_log_num_return = Share_app_log_save("Imv/Parse thread", "Draw_C3DTexToC2DImage...", 1234567890, false);
						pic_parse_result = Draw_C3DTexToC2DImage(c3d_cache_tex, c3d_cache_subtex, i, i, stbi_image, (u32)(pic_width * pic_height * 4), (u32)pic_width, (u32)pic_height, pic_parse_start_pos_x, pic_parse_start_pos_y, 512, GPU_RGBA8);
						Share_app_log_add_result(pic_parse_log_num_return, pic_parse_result.string, pic_parse_result.code, false);

						pic_parse_start_pos_x += 512;
						if (pic_parse_start_pos_x >= 2048)
						{
							pic_parse_start_pos_x = 0;
							pic_parse_start_pos_y += 512;
						}
					}

				}
				pic_parse_log_num_return = Share_app_log_save("Imv/Parse thread", "APT_SetAppCpuTimeLimit_30...", 1234567890, false);
				pic_parse_result.code = APT_SetAppCpuTimeLimit(30);
				if (pic_parse_result.code == 0)
				{
					Share_app_log_add_result(pic_parse_log_num_return, "[Success] ", pic_parse_result.code, false);
				}
				else
					Share_app_log_add_result(pic_parse_log_num_return, "[Error] ", pic_parse_result.code, false);
			}
		}
		usleep(100000);
	}
	parse_thread_started = false;
	Share_app_log_save("Imv/Parse thread", "Thread exit.", 1234567890, false);
}

void Image_viewer_download_thread(void* arg)
{
	Share_app_log_save("Imv/Download thread", "Thread started.", 1234567890, false);
	u8* downloaded_image_buffer;
	u32 downloaded_image_size;
	int image_download_log_num_return;
	bool function_fail = false;
	bool redirect = false;
	char* moved_url;
	std::string url;
	std::string downloaded_data_string;
	httpcContext image_viewer_httpc;
	//FS_Archive image_download_fs_archive = 0;
	//Handle image_download_fs_handle = 0;
	Result_with_string pic_download_result;

	while (s_imv_download_thread_run)
	{
		if (s_imv_thread_suspend)
			usleep(250000);
		else
		{
			if (pic_download_request)
			{
				function_fail = false;
				url = s_clipboards[s_imv_clipboard_select_num];

				moved_url = (char*)malloc(0x1000);
				downloaded_image_buffer = (u8*)malloc(0x500000);
				memset(downloaded_image_buffer, 0x0, 0x500000);

				while (true)
				{
					downloaded_image_size = 0;
					image_download_progress = 0;
					redirect = false;
					pic_download_result.code = 0;
					pic_download_result.string = "[Success] ";
					image_download_log_num_return = Share_app_log_save("Imv/Download thread/httpc", "Downloading image...", 1234567890, false);
					usleep(25000);

					pic_download_result.code = httpcOpenContext(&image_viewer_httpc, HTTPC_METHOD_GET, url.c_str(), 0);
					if (pic_download_result.code != 0)
					{
						pic_download_result.string = "[Error] httpcOpenContext failed. ";
						function_fail = true;
					}
					image_download_progress++;

					if (!function_fail)
					{
						pic_download_result.code = httpcSetSSLOpt(&image_viewer_httpc, SSLCOPT_DisableVerify);
						if (pic_download_result.code != 0)
						{
							pic_download_result.string = "[Error] httpcSetSSLOpt failed. ";
							function_fail = true;
						}
					}
					image_download_progress++;

					if (!function_fail)
					{
						pic_download_result.code = httpcSetKeepAlive(&image_viewer_httpc, HTTPC_KEEPALIVE_ENABLED);
						if (pic_download_result.code != 0)
						{
							pic_download_result.string = "[Error] httpcSetKeepAlive failed. ";
							function_fail = true;
						}
					}
					image_download_progress++;

					if (!function_fail)
					{
						httpcAddRequestHeaderField(&image_viewer_httpc, "Connection", "Keep-Alive");
						httpcAddRequestHeaderField(&image_viewer_httpc, "User-Agent", s_httpc_user_agent.c_str());
						pic_download_result.code = httpcBeginRequest(&image_viewer_httpc);
						if (pic_download_result.code != 0)
						{
							pic_download_result.string = "[Error] httpcBeginRequest failed. ";
							function_fail = true;
						}
					}
					image_download_progress++;

					if (!function_fail)
					{
						pic_download_result.code = httpcGetResponseHeader(&image_viewer_httpc, "Location", moved_url, 0x1000);
						if(pic_download_result.code == 0)
						{
							url = moved_url;
							redirect = true;
							pic_download_result.string = "[Success] Redirecting...";
							Share_app_log_add_result(image_download_log_num_return, pic_download_result.string, pic_download_result.code, false);
						}
					}
					image_download_progress++;

					if (!function_fail && !redirect)
					{
						pic_download_result.code = httpcDownloadData(&image_viewer_httpc, downloaded_image_buffer, 0x500000, &downloaded_image_size);
						if (pic_download_result.code != 0)
						{
							pic_download_result.string = "[Error] httpcDownloadData failed. ";
							function_fail = true;
						}
						else
						{
							downloaded_data_string = (char*)downloaded_image_buffer;
							if (downloaded_data_string.substr(0, 4) == "http")
							{
								url = downloaded_data_string;
								redirect = true;
								pic_download_result.string = "[Success] Redirecting...";
								Share_app_log_add_result(image_download_log_num_return, pic_download_result.string, pic_download_result.code, false);
							}
						}
					}
					image_download_progress++;

					httpcCloseContext(&image_viewer_httpc);
					image_download_progress++;

					if (function_fail || !redirect)
						break;
				}
				Share_app_log_add_result(image_download_log_num_return, pic_download_result.string, pic_download_result.code, false);

				if (!function_fail)
				{
					image_size = downloaded_image_size;
					memcpy(image_buffer, downloaded_image_buffer, downloaded_image_size);
				}
				/*image_download_log_num_return = Share_app_log_save("Imv/Download thread/fs", "Save_to_file...", 1234567890, false);
				pic_download_result = Share_save_to_file(std::to_string(s_months) + "_" + std::to_string(s_days) + "_" + std::to_string(s_hours) + "_" + std::to_string(s_minutes) + "_" + std::to_string(s_seconds) + ".jpg", image_buffer, (int)downloaded_image_size, "/Line/images/", true, image_download_fs_handle, image_download_fs_archive);
				Share_app_log_add_result(image_download_log_num_return, pic_download_result.string, pic_download_result.code, false);*/
				
				image_download_progress++;

				free(moved_url);
				free(downloaded_image_buffer);
				pic_download_request = false;
			}
			usleep(100000);
		}
	}
	Share_app_log_save("Imv/Download thread", "Thread exit.", 1234567890, false);
}

void Image_viewer_exit(void)
{
	Share_app_log_save("Imv/Exit", "Exiting...", 1234567890, true);
	u64 time_out = 10000000000;
	int exit_log_num_return;
	bool function_fail = false;
	Result_with_string exit_result;
	exit_result.code = 0;
	exit_result.string = "[Success] ";

	s_imv_already_init = false;
	s_imv_download_thread_run = false;
	s_imv_parse_thread_run = false;

	exit_log_num_return = Share_app_log_save("Imv/Exit", "Thread exiting(0/1)...", 1234567890, true);
	exit_result.code = threadJoin(image_viewer_download_thread, time_out);
	if (exit_result.code == 0)
		Share_app_log_add_result(exit_log_num_return, "[Success] ", exit_result.code, true);
	else
	{
		function_fail = true;
		Share_app_log_add_result(exit_log_num_return, "[Error] ", exit_result.code, true);
	}

	exit_log_num_return = Share_app_log_save("Imv/Exit", "Thread exiting(1/1)...", 1234567890, true);
	exit_result.code = threadJoin(image_viewer_parse_thread, time_out);
	if (exit_result.code == 0)
		Share_app_log_add_result(exit_log_num_return, "[Success] ", exit_result.code, true);
	else
	{
		function_fail = true;
		Share_app_log_add_result(exit_log_num_return, "[Error] ", exit_result.code, true);
	}

	for (int i = 0; i < 16; i++)
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
		Share_app_log_save("Imv/Exit", "[Warn] Some function returned error.", 1234567890, true);

	Share_app_log_save("Imv/Exit", "Exited.", 1234567890, true);
}
