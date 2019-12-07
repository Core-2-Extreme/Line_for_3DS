#pragma once
#include "citro2d.h"

void Draw(std::string text, float x, float y, float text_size_x, float text_size_y, float r, float g, float b, float a);

void Draw_load_texture(void);

void Draw_texture(int texture_type, int image_num, float x, float y);

void Draw_with_specific_language(std::string text, int lang_num, float x, float y, float text_size_x, float text_size_y, float r, float g, float b, float a);

void Draw_init(void);

void Draw_load_system_fonts(void);

void Draw_exit(void);

void Draw_set_draw_mode(int mode);

void Draw_screen_ready_to_draw(int screen, bool screen_clear, int screen_clear_ver);

void Draw_apply_draw(void);
