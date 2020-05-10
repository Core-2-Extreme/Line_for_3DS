#include <string>
#include "citro2d.h"

#include "share_function.hpp"
#include "hid.hpp"
#include "draw.hpp"
#include "error.hpp"
#include "external_font.hpp"
#include "log.hpp"
#include "types.hpp"
#include "setting_menu.hpp"

bool draw_do_not_draw = false;
C2D_Font Share_fonts[4];
C3D_RenderTarget* Screen_top;
C3D_RenderTarget* Screen_bot;
C2D_SpriteSheet sheet_texture[128];
std::string screen_clear_text = "\u25a0";//¡

void Draw_set_do_not_draw_flag(bool flag)
{
	draw_do_not_draw = flag;
}

void Draw(std::string text, int type, float x, float y, float text_size_x, float text_size_y, float r, float g, float b, float a)
{
	if (type >= 0 && type <= 4)
	{
		C2D_Text c2d_text;
		C2D_TextBuf c2d_buf = C2D_TextBufNew(4096);

		if (text.length() > 4096)
			text = text.substr(0, 4096);

		if (type == 0)
			C2D_TextParse(&c2d_text, c2d_buf, text.c_str());
		else if (type >= 1 && type <= 4)
			C2D_TextFontParse(&c2d_text, Share_fonts[type - 1], c2d_buf, text.c_str());

		if (type == 2)
		{
			text_size_x = text_size_x * 1.15;
			text_size_y = text_size_y * 1.15;
		}
		else if (type == 4)
		{
			text_size_x = text_size_x * 1.4;
			text_size_y = text_size_y * 1.4;
		}

		C2D_TextOptimize(&c2d_text);
		C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.0f, text_size_x, text_size_y, C2D_Color32f(r, g, b, a));
		C2D_TextBufDelete(c2d_buf);
	}
	else if (type == 5)
	{
		text_size_x = text_size_x * 1.65;
		text_size_y = text_size_y * 1.65;
		Exfont_draw_external_fonts(text, x, y, text_size_x, text_size_y, r, g, b, a);
	}
}

Result_with_string Draw_load_texture(std::string file_name, int sheet_map_num, C2D_Image return_image[], int start_num, int num_of_array)
{
	size_t num_of_images;
	bool function_fail = false;
	Result_with_string load_texture_result;
	load_texture_result.code = 0;
	load_texture_result.string = "[Success] ";

	sheet_texture[sheet_map_num] = C2D_SpriteSheetLoad(file_name.c_str());
	if (sheet_texture[sheet_map_num] == NULL)
	{
		load_texture_result.code = -1;
		load_texture_result.string = "[Error] Couldn't load texture file : " + file_name + " ";
		function_fail = true;
	}

	if (!function_fail)
	{
		num_of_images = C2D_SpriteSheetCount(sheet_texture[sheet_map_num]);
		if ((int)num_of_images < num_of_array)
		{
			load_texture_result.code = -2;
			load_texture_result.string = "[Error] num of arry " + std::to_string(num_of_array) + " is bigger than spritesheet has num of image(s) " + std::to_string(num_of_images) + " ";
			function_fail = true;
		}
	}

	if (!function_fail)
	{
		for (int i = 0; i <= (num_of_array - 1); i++)
		{
			return_image[start_num + i] = C2D_SpriteSheetGetImage(sheet_texture[sheet_map_num], i);
		}
	}
	return load_texture_result;
}

void Draw_texture(C2D_Image image[], C2D_ImageTint tint, int num, float x, float y, float x_size, float y_size)
{
	C2D_DrawParams c2d_parameter =
	{
		{
			x,
			y, 
			x_size,
			y_size
		},
		{
			0,
			0
		},
		0.0f,
		0.0f
	};

	if (!(image[num].tex == NULL))
	{
		if(tint.corners[0].color == dammy_tint.corners[0].color)
			C2D_DrawImage(image[num], &c2d_parameter, NULL);
		else
			C2D_DrawImage(image[num], &c2d_parameter, &tint);

	}
}

void Draw_error(void)
{
	Draw_texture(Square_image, aqua_tint, 0, 20.0, 30.0, 280.0, 150.0);
	Draw_texture(Square_image, weak_yellow_tint, 0, 150.0, 150.0, 20.0, 20.0);

	Draw("Summary : ", 0, 22.5, 40.0, 0.45, 0.45, 1.0, 0.0, 0.0, 1.0);
	Draw(Err_query_error_data(ERR_SUMMARY), 0, 22.5, 50.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);
	Draw("Description : ", 0, 22.5, 60.0, 0.45, 0.45, 1.0, 0.0, 0.0, 1.0);
	Draw(Err_query_error_data(ERR_DESCRIPTION), 0, 22.5, 70.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
	Draw("Place : ", 0, 22.5, 90.0, 0.45, 0.45, 1.0, 0.0, 0.0, 1.0);
	Draw(Err_query_error_data(ERR_PLACE), 0, 22.5, 100.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);
	Draw("Error code : ", 0, 22.5, 110.0, 0.45, 0.45, 1.0, 0.0, 0.0, 1.0);
	Draw(Err_query_error_data(ERR_CODE), 0, 22.5, 120.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);
	Draw("OK", 0, 152.5, 152.5, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);
}

void Draw_progress(std::string message)
{
	if (draw_do_not_draw)
		return;

	Draw_set_draw_mode(1);
	if (Sem_query_settings(SEM_NIGHT_MODE))
		Draw_screen_ready_to_draw(0, true, 0, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(0, true, 0, 1.0, 1.0, 1.0);

	Draw(message, 0, 80.0, 110.0, 0.75, 0.75, 0.0, 0.5, 1.0, 1.0);

	Draw_apply_draw();

	Draw_set_draw_mode(1);
	if (Sem_query_settings(SEM_NIGHT_MODE))
		Draw_screen_ready_to_draw(0, true, 0, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(0, true, 0, 1.0, 1.0, 1.0);

	Draw(message, 0, 80.0, 110.0, 0.75, 0.75, 0.0, 0.5, 1.0, 1.0);

	Draw_apply_draw();
}

void Draw_log(void)
{
	Draw_set_draw_mode(1);
	if (Sem_query_settings(SEM_NIGHT_MODE))
		Draw_screen_ready_to_draw(0, true, 0, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(0, true, 0, 1.0, 1.0, 1.0);

	for (int i = 0; i < 23; i++)
		Draw(Log_query_log(Log_query_y() + i), 0, Log_query_x(), 10.0f + (i * 10), 0.4, 0.4, 0.0, 0.5, 1.0, 1.0);

	Draw_apply_draw();
}

void Draw_debug_info(void)
{
	float text_red;
	float text_green;
	float text_blue;
	float text_alpha;
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

	Draw_texture(Square_image, weak_blue_tint, 0, 0.0, 30.0, 140.0, 130.0);
	Draw("A press : " + std::to_string(Hid_query_key_press_state(KEY_P_A)) + " held : " + std::to_string(Hid_query_key_held_state(KEY_H_A)), 0, 0.0, 30.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	Draw("B press : " + std::to_string(Hid_query_key_press_state(KEY_P_B)) + " held : " + std::to_string(Hid_query_key_held_state(KEY_H_B)), 0, 0.0, 40.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	Draw("X press : " + std::to_string(Hid_query_key_press_state(KEY_P_X)) + " held : " + std::to_string(Hid_query_key_held_state(KEY_H_X)), 0, 0.0, 50.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	Draw("Y press : " + std::to_string(Hid_query_key_press_state(KEY_P_Y)) + " held : " + std::to_string(Hid_query_key_held_state(KEY_H_Y)), 0, 0.0, 60.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	Draw("C DOWN held : " + std::to_string(Hid_query_key_held_state(KEY_H_C_DOWN)), 0, 0.0, 70.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	Draw("C RIGHT held : " + std::to_string(Hid_query_key_held_state(KEY_H_C_RIGHT)), 0, 0.0, 80.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	Draw("C UP held : " + std::to_string(Hid_query_key_held_state(KEY_H_C_UP)), 0, 0.0, 90.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	Draw("C LEFT held : " + std::to_string(Hid_query_key_held_state(KEY_H_C_LEFT)), 0, 0.0, 100.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	Draw("x pos : " + std::to_string(Hid_query_touch_pos(true)) + " y pos : " + std::to_string(Hid_query_touch_pos(false)), 0, 0.0, 110.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	Draw("CPU : " + std::to_string(C3D_GetProcessingTime()) + "ms", 0, 0.0, 120.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	Draw("GPU : " + std::to_string(C3D_GetDrawingTime()) + "ms", 0, 0.0, 130.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	Draw("Free RAM " + std::to_string((double)s_free_ram / 10.0).substr(0, 5) + " MB", 0, 0.0f, 140.0f, 0.4f, 0.4, text_red, text_green, text_blue, text_alpha);
	Draw("Free linear RAM " + std::to_string((double)s_free_linear_ram / 1024.0 / 1024.0).substr(0, 5) +" MB", 0, 0.0f, 150.0f, 0.4f, 0.4, text_red, text_green, text_blue, text_alpha);
}

void Draw_init(void)
{
	C2D_Prepare();
	Screen_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	Screen_bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	C2D_TargetClear(Screen_top, C2D_Color32f(0, 0, 0, 0));
	C2D_TargetClear(Screen_bot, C2D_Color32f(0, 0, 0, 0));	
}

void Draw_load_system_font(int system_font_num)
{
	if (system_font_num == 0)
		Share_fonts[0] = C2D_FontLoadSystem(CFG_REGION_JPN);
	else if (system_font_num == 1)
		Share_fonts[1] = C2D_FontLoadSystem(CFG_REGION_CHN);
	else if (system_font_num == 2)
		Share_fonts[2] = C2D_FontLoadSystem(CFG_REGION_KOR);
	else if (system_font_num == 3)
		Share_fonts[3] = C2D_FontLoadSystem(CFG_REGION_TWN);
}

void Draw_free_system_font(int system_font_num)
{
	if (system_font_num >= 0 && system_font_num <= 3)
	{
		if (Share_fonts[system_font_num] != NULL)
		{
			C2D_FontFree(Share_fonts[system_font_num]);
			Share_fonts[system_font_num] = NULL;
		}
	}
}

void Draw_free_texture(int sheet_map_num)
{
	if (sheet_texture[sheet_map_num] != NULL)
	{
		C2D_SpriteSheetFree(sheet_texture[sheet_map_num]);
		sheet_texture[sheet_map_num] = NULL;
	}
}

void Draw_exit(void)
{
	for (int i = 0; i < 128; i++)
		Draw_free_texture(i);
	for (int i = 0; i < 4; i++)
		Draw_free_system_font(i);
}

void Draw_set_draw_mode(int mode)
{
	if (mode == 0)
		C3D_FrameBegin(C3D_FRAME_NONBLOCK);
	else if (mode == 1)
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
}

void Draw_screen_ready_to_draw(int screen, bool screen_clear, int screen_clear_ver, float red, float green, float blue)
{
	C2D_ImageTint tint;
	C2D_PlainImageTint(&tint, C2D_Color32f(red, green, blue, 1.0), true);
	if (screen == 0)
	{
		C2D_SceneBegin(Screen_top);
		if (screen_clear)
		{
			if (screen_clear_ver == 1)
				Draw(screen_clear_text, 0, -50.0, -300.0, 40.0, 35.0, red, green, blue, 1.0);
			else if (screen_clear_ver == 2)
				Draw_texture(Square_image, tint, 0, 0.0, 0.0, 400.0, 240.0);
			else
				C2D_TargetClear(Screen_top, C2D_Color32f(red, green, blue, 0));
		}
			
	}
	else if(screen == 1)
	{
		C2D_SceneBegin(Screen_bot);
		if (screen_clear)
		{
			if (screen_clear_ver == 1)
				Draw(screen_clear_text, 0, -50.0, -300.0, 40.0, 30.0, red, green, blue, 1.0);
			else if (screen_clear_ver == 2)
				Draw_texture(Square_image, tint, 0, 0.0, 0.0, 320.0, 240.0);
			else
				C2D_TargetClear(Screen_bot, C2D_Color32f(red, green, blue, 0));
		}
	}
}

void Draw_apply_draw(void)
{
	C3D_FrameEnd(0);
}

/*static inline size_t fmtSize(GPU_TEXCOLOR fmt)
{
	switch (fmt)
	{
	case GPU_RGBA8:
		return 32;
	case GPU_RGB8:
		return 24;
	case GPU_RGBA5551:
	case GPU_RGB565:
	case GPU_RGBA4:
	case GPU_LA8:
	case GPU_HILO8:
		return 16;
	case GPU_L8:
	case GPU_A8:
	case GPU_LA4:
	case GPU_ETC1A4:
		return 8;
	case GPU_L4:
	case GPU_A4:
	case GPU_ETC1:
		return 4;
	default:
		return 0;
	}
}

bool Moded_C3D_TexInitWithParams(C3D_Tex* tex, C3D_TexCube* cube, C3D_TexInitParams p)
{
	u32 size = fmtSize(p.format);
	if (!size) return false;
	size *= (u32)p.width * p.height / 8;
	u32 total_size = C3D_TexCalcTotalSize(size, p.maxLevel);
	
	s_error_description = std::to_string(total_size);
	Err_set_error_show_flag(true);
	
	tex->data = malloc(total_size);
	if (!tex->data) return false;

	tex->width = p.width;
	tex->height = p.height;
	tex->param = GPU_TEXTURE_MODE(p.type);
	if (p.format == GPU_ETC1)
		tex->param |= GPU_TEXTURE_ETC1_PARAM;
	if (p.type == GPU_TEX_SHADOW_2D || p.type == GPU_TEX_SHADOW_CUBE)
		tex->param |= GPU_TEXTURE_SHADOW_PARAM;
	tex->fmt = p.format;
	tex->size = size;
	tex->border = 0;
	tex->lodBias = 0;
	tex->maxLevel = p.maxLevel;
	tex->minLevel = 0;
	return true;
}*/
