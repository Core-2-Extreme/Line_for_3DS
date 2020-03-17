#include <string>
#include "citro2d.h"
#include "share_function.hpp"

C2D_Font Share_fonts[6];
C3D_RenderTarget* Screen_top;
C3D_RenderTarget* Screen_bot;
C2D_SpriteSheet sheet_texture[128];
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
//	C2D_SpriteSheetFree((C2D_SpriteSheet)font_textures[0]);
	//C2D_SpriteSheetFree(sheet_texture);
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
	if (screen == 0)
	{
		C2D_SceneBegin(Screen_top);
		if (screen_clear)
		{
			if (screen_clear_ver == 1)
				Draw(screen_clear_text, -30.0, -100.0, 20.0, 17.5, red, green, blue, 1.0);
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
				Draw(screen_clear_text, -30.0, -100.0, 20.0, 17.5, red, green, blue, 1.0);
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
	s_error_display = true;
	
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
