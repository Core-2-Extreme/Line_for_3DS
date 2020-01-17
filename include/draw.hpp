#pragma once
#include "citro2d.h"
#include "share_function.hpp"

void Draw(std::string text, float x, float y, float text_size_x, float text_size_y, float r, float g, float b, float a);

Result_with_string Draw_load_texture(std::string file_name, C2D_SpriteSheet sheet_texture, C2D_Image return_image[], int num_of_array);

void Draw_texture(C2D_Image image[], int image_num, float x, float y, float x_size, float y_size);

void Draw_with_specific_language(std::string text, int lang_num, float x, float y, float text_size_x, float text_size_y, float r, float g, float b, float a);

void Draw_init(void);

void Draw_load_system_fonts(void);

void Draw_exit(void);

void Draw_set_draw_mode(int mode);

void Draw_screen_ready_to_draw(int screen, bool screen_clear, int screen_clear_ver, float red, float green, float blue);

void Draw_apply_draw(void);
