#pragma once

std::string Exfont_query_font_name(int exfont_num);

std::string Exfont_text_sort(std::string sorce_part_string[]);

void Exfont_text_parse(std::string sorce_string, std::string part_string[]);

void Exfont_draw_external_fonts(std::string string, float texture_x, float texture_y, float texture_size_x, float texture_size_y, bool follow_new_line);

Result_with_string Exfont_load_exfont(int exfont_num);

void Exfont_unload_exfont(int exfont_num);
