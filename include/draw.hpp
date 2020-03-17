#pragma once
#include "citro2d.h"
#include "share_function.hpp"

void Draw(std::string text, float x, float y, float text_size_x, float text_size_y, float r, float g, float b, float a);

Result_with_string Draw_load_texture(std::string file_name, int sheet_map_num, C2D_Image return_image[], int start_num, int num_of_array);

void Draw_texture(C2D_Image image[], C2D_ImageTint tint, int image_num, float x, float y, float x_size, float y_size);

void Draw_with_specific_language(std::string text, int lang_num, float x, float y, float text_size_x, float text_size_y, float r, float g, float b, float a);

void Draw_init(void);

void Draw_load_system_font(int system_font_num);

void Draw_free_system_font(int system_font_num);

void Draw_free_texture(int sheet_map_num);

void Draw_exit(void);

void Draw_set_draw_mode(int mode);

void Draw_screen_ready_to_draw(int screen, bool screen_clear, int screen_clear_ver, float red, float green, float blue);

void Draw_apply_draw(void);

//bool Moded_C3D_TexInitWithParams(C3D_Tex* tex, C3D_TexCube* cube, C3D_TexInitParams p);
