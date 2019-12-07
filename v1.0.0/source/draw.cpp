#include <string>
#include "citro2d.h"
#include "share_function.hpp"

C2D_Font Share_fonts[6];
C2D_Image Background_image[2], Wifi_icon_image[9], Battery_level_icon_image[21], Battery_charge_icon_image, Chinese_font_image[2], Arabic_font_image[256], Armenian_font_image[92], English_font_image[96];
C2D_SpriteSheet Background_texture, Wifi_icon_texture, Battery_level_icon_texture, Battery_charge_icon_texture, Chinese_font_texture, Arabic_font_texture, Armenian_font_texture, English_font_texture;
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

void Draw_load_texture(void)
{
	Background_texture = C2D_SpriteSheetLoad("romfs:/gfx/background.t3x");
	Wifi_icon_texture = C2D_SpriteSheetLoad("romfs:/gfx/wifi_signal.t3x");
	Battery_level_icon_texture = C2D_SpriteSheetLoad("romfs:/gfx/battery_level.t3x");
	Battery_charge_icon_texture = C2D_SpriteSheetLoad("romfs:/gfx/battery_charge.t3x");
	Chinese_font_texture = C2D_SpriteSheetLoad("romfs:/gfx/chinese_font.t3x");
	Arabic_font_texture = C2D_SpriteSheetLoad("romfs:/gfx/arabic_font.t3x");
	Armenian_font_texture = C2D_SpriteSheetLoad("romfs:/gfx/armenian_font.t3x");
	English_font_texture = C2D_SpriteSheetLoad("romfs:/gfx/english_font.t3x");

	Background_image[0] = C2D_SpriteSheetGetImage(Background_texture, 0);
	Background_image[1] = C2D_SpriteSheetGetImage(Background_texture, 1);
	for (int i = 0; i <= 8; i++)
	{
		Wifi_icon_image[i] = C2D_SpriteSheetGetImage(Wifi_icon_texture, i);
	}
	for (int i = 0; i <= 20; i++)
	{
		Battery_level_icon_image[i] = C2D_SpriteSheetGetImage(Battery_level_icon_texture, i);
	}
	Battery_charge_icon_image = C2D_SpriteSheetGetImage(Battery_charge_icon_texture, 0);

	Chinese_font_image[0] = C2D_SpriteSheetGetImage(Chinese_font_texture, 0);
	Chinese_font_image[1] = C2D_SpriteSheetGetImage(Chinese_font_texture, 1);

	for (int i = 0; i < 256; i++)
	{
		Arabic_font_image[i] = C2D_SpriteSheetGetImage(Arabic_font_texture, i);
	}
	
	for (int i = 0; i < 96; i++)
	{
		English_font_image[i] = C2D_SpriteSheetGetImage(English_font_texture, i);
	}

	for (int i = 0; i < 92; i++)
	{
		Armenian_font_image[i] = C2D_SpriteSheetGetImage(Armenian_font_texture, i);
	}
}

void Draw_texture(int texture_type, int num, float x, float y)
{
	if (texture_type == TEXTURE_BACKGROUND)
		C2D_DrawImageAt(Background_image[num], x, y, 0.0, NULL, 1.0, 1.0);
	else if (texture_type == TEXTURE_WIFI_ICON)
		C2D_DrawImageAt(Wifi_icon_image[num], x, y, 0.0, NULL, 1.0, 1.0);
	else if (texture_type == TEXTURE_BATTERY_LEVEL_ICON)
		C2D_DrawImageAt(Battery_level_icon_image[num], x, y, 0.0, NULL, 1.0, 1.0);
	else if (texture_type == TEXTURE_BATTERY_CHARGE_ICON)
		C2D_DrawImageAt(Battery_charge_icon_image, x, y, 0.0, NULL, 1.0, 1.0);
	else if (texture_type == TEXTURE_CHINESE_FONT)
		C2D_DrawImageAt(Chinese_font_image[num], x, y, 0.0, NULL, 1.0, 1.0);
	else if (texture_type == TEXTURE_ARABIC_FONT)
		C2D_DrawImageAt(Arabic_font_image[num], x, y, 0.0, NULL, 1.0, 1.0);
	else if (texture_type == TEXTURE_ENGLISH_FONT)
		C2D_DrawImageAt(English_font_image[num], x, y, 0.0, NULL, 1.0, 1.0);
	else if (texture_type == TEXTURE_ARMENIAN_FONT)
		C2D_DrawImageAt(Armenian_font_image[num], x, y, 0.0, NULL, 1.0, 1.0);
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
	for (int i = 0; i >= 3; i++)
		C2D_FontFree(Share_fonts[i]);
	C2D_SpriteSheetFree(Background_texture);
	C2D_SpriteSheetFree(Wifi_icon_texture);
	C2D_SpriteSheetFree(Battery_level_icon_texture);
	C2D_SpriteSheetFree(Battery_charge_icon_texture);
	C2D_SpriteSheetFree(Chinese_font_texture);
}

void Draw_set_draw_mode(int mode)
{
	if (mode == 0)
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	else if (mode == 1)
		C3D_FrameBegin(C3D_FRAME_NONBLOCK);
}

void Draw_screen_ready_to_draw(int screen, bool screen_clear, int screen_clear_ver)
{
	if (screen == 0)
	{
		C2D_SceneBegin(Screen_top_);
		if (screen_clear)
		{
			if (screen_clear_ver == 1)
				Draw(screen_clear_text, -30.0, -100.0, 20.0, 17.5, 1.0, 1.0, 1.0, 1.0);
			else
				C2D_TargetClear(Screen_top_, C2D_Color32f(0, 0, 0, 0));
		}
			
	}
	else if(screen == 1)
	{
		C2D_SceneBegin(Screen_bot_);
		if (screen_clear)
		{
			if (screen_clear_ver == 1)
				Draw(screen_clear_text, -30.0, -100.0, 20.0, 17.5, 1.0, 1.0, 1.0, 1.0);
			else
				C2D_TargetClear(Screen_bot_, C2D_Color32f(0, 0, 0, 0));
		}
	}
}

void Draw_apply_draw(void)
{
	C3D_FrameEnd(0);
}
