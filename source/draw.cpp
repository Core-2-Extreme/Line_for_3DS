#include <string>
#include "citro2d.h"
#include "share_function.hpp"

C2D_Font Share_fonts[6];
C3D_RenderTarget* Screen_top_;
C3D_RenderTarget* Screen_bot_;
std::string screen_clear_text = "\u25a0";//Å°

void Draw(std::string text, float x, float y, float text_size_x, float text_size_y, float r, float g, float b, float a)
{
	C2D_Text c2d_text;
	C2D_TextBuf c2d_buf = C2D_TextBufNew(8192);
	C2D_TextParse(&c2d_text, c2d_buf, text.c_str());
	C2D_TextOptimize(&c2d_text);
	C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.0f, text_size_x, text_size_y, C2D_Color32f(r, g, b, a));
	C2D_TextBufDelete(c2d_buf);
}

Result_with_string Draw_load_texture(std::string file_name, C2D_SpriteSheet sheet_texture, C2D_Image return_image[], int num_of_array)
{
	size_t num_of_images;
	bool function_fail = false;
	Result_with_string load_texture_result;
	load_texture_result.code = 0;
	load_texture_result.string = "[Success] ";

	sheet_texture = C2D_SpriteSheetLoad(file_name.c_str());
	if (sheet_texture == NULL)
	{
		load_texture_result.code = -1;
		load_texture_result.string = "[Error] Can't load texture file : " + file_name + " ";
		function_fail = true;
	}

	if (!function_fail)
	{
		num_of_images = C2D_SpriteSheetCount(sheet_texture);
		if ((int)num_of_images < num_of_array)
		{
			load_texture_result.code = -2;
			load_texture_result.string = "[Error] num_of_arry " + std::to_string(num_of_array) + " is bigger than spritesheet has num of image(s) " + std::to_string(num_of_images) + " ";
			function_fail = true;
		}
	}

	if (!function_fail)
	{
		for (int i = 0; i <= (num_of_array - 1); i++)
		{
			return_image[i] = C2D_SpriteSheetGetImage(sheet_texture, i);
		}
	}

	return load_texture_result;
}

void Draw_texture(C2D_Image image[], int num, float x, float y, float x_size, float y_size)
{
	C2D_DrawParams custom_font_c2d_parameter =
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

	if(!(image[num].tex == NULL))
		C2D_DrawImage(image[num], &custom_font_c2d_parameter, NULL);
}

void Draw_with_specific_language(std::string text, int lang_num, float x, float y, float text_size_x, float text_size_y, float r, float g, float b, float a)
{
	if (lang_num <= -1 || lang_num >= 7)
		lang_num = 0;

	C2D_Text c2d_text;
	C2D_TextBuf c2d_buf = C2D_TextBufNew(8192);
	C2D_TextFontParse(&c2d_text, Share_fonts[lang_num], c2d_buf, text.c_str());
	C2D_TextOptimize(&c2d_text);
	C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.0f, text_size_x, text_size_y, C2D_Color32f(r, g, b, a));
	C2D_TextBufDelete(c2d_buf);
}

void Draw_init(void)
{
	C2D_Prepare();
	Screen_top_ = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	Screen_bot_ = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	C2D_TargetClear(Screen_top_, C2D_Color32f(0, 0, 0, 0));
	C2D_TargetClear(Screen_bot_, C2D_Color32f(0, 0, 0, 0));	
}

void Draw_load_system_fonts(void)
{
	Share_fonts[0] = C2D_FontLoadSystem(CFG_REGION_JPN);
	Share_fonts[1] = C2D_FontLoadSystem(CFG_REGION_CHN);
	Share_fonts[2] = C2D_FontLoadSystem(CFG_REGION_KOR);
	Share_fonts[3] = C2D_FontLoadSystem(CFG_REGION_TWN);
}

void Draw_exit(void)
{
	//C2D_SpriteSheetFree(sheet_texture);
	for (int i = 0; i >= 3; i++)
		C2D_FontFree(Share_fonts[i]);
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
	if (screen == 0)
	{
		C2D_SceneBegin(Screen_top_);
		if (screen_clear)
		{
			if (screen_clear_ver == 1)
				Draw(screen_clear_text, -30.0, -100.0, 20.0, 17.5, red, green, blue, 1.0);
			else
				C2D_TargetClear(Screen_top_, C2D_Color32f(red, green, blue, 0));
		}
			
	}
	else if(screen == 1)
	{
		C2D_SceneBegin(Screen_bot_);
		if (screen_clear)
		{
			if (screen_clear_ver == 1)
				Draw(screen_clear_text, -30.0, -100.0, 20.0, 17.5, red, green, blue, 1.0);
			else
				C2D_TargetClear(Screen_bot_, C2D_Color32f(red, green, blue, 0));
		}
	}
}

void Draw_apply_draw(void)
{
	C3D_FrameEnd(0);
}
