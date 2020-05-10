#pragma once

void Exfont_init(void);

void Exfont_init_sample(int num_of_loop, std::string samples[], int byte_0_start, int byte_1_start, int byte_2_start, int byte_3_start, int bytes_per_char, int invalid_list[], int num_of_invalid);

std::string Exfont_query_font_name(int exfont_num);

std::string Exfont_text_sort(std::string sorce_part_string[]);

void Exfont_text_parse(std::string sorce_string, std::string part_string[]);

void Exfont_draw_external_fonts(std::string string, float texture_x, float texture_y, float texture_size_x, float texture_size_y, float red, float green, float blue, float alpha);

Result_with_string Exfont_load_exfont(int exfont_num);

void Exfont_unload_exfont(int exfont_num);
