#include <string>
#include <3ds.h>
#include "setting_menu.hpp"
#include "draw.hpp"
#include "external_font.hpp"
#include "log.hpp"
#include "types.hpp"
#include "file.hpp"
#include <unistd.h>


//#0600~#06FF
double exfont_arabic_font_interval[255] = {
 16, 23, 10, 22, 22, 18, 11, 11, 13, 11, 15, 5,  2,  4, 12, 9,
  5,  4, 5, 6, 6,  5, 7,  4,  3,  2,  3,  2, 18,      4, 8,
 11, 8, 5,  3, 10,  3, 12,  1, 12, 7, 12, 12, 9, 9, 9, 6,
 6, 8, 8, 19, 19, 22, 22, 13, 13, 9, 9, 14, 14, 11, 11, 11,
 5, 15, 13, 12, 11, 9, 11, 7, 10, 12, 12, 5, 5, 5, 5, 5,
 5, 5,  4, 6,  4,  4,  1, 6,  2,  4, 5,  3,  2,  3,  3,  3,
  4,  3, 7, 8, 7, 7, 8, 8, 8, 8, 9,  4,  2, 7, 13, 10,
  1, 5,  4,  4,  3, 5, 12, 12, 14, 12, 12, 12, 12, 12, 12, 12,
 12, 9, 9, 9, 9, 9, 9, 9, 6, 6, 6, 6, 6, 6, 6, 6,
 6, 8, 8, 8, 8, 9, 8, 8, 8, 8, 19, 19, 19, 22, 22, 13,
 9, 15, 15, 15, 15, 15, 15, 13, 13, 14, 17, 14, 12, 12, 12, 14,
 14, 14, 14, 14, 14, 11, 11, 11, 8, 10, 11, 11, 11, 11, 11, 8,
 7, 7, 7, 7, 10, 10, 10, 10, 10, 10, 10, 10, 12, 15, 12, 8,
 12, 12, 11, 11, 5, 7, 12, 9,  5, 5, 6, 6, 12, 10, 15,  2,
  2, 6, 5, 12,  3, 6, 7, 7, 6, 8,  3,  3,  3, 5, 5, 8,
  4,  3, 7, 8, 8, 8, 6, 8, 8, 8, 16, 18, 7,  4, 5, 7,
};
//#0531~#058
double exfont_armenian_font_interval[91] = {
	 11, 7, 10, 10, 8, 8, 7, 7, 11, 10, 9, 6, 9, 12, 8,
 7, 9, 10, 8, 11, 10, 10, 11, 7, 8, 10, 8, 10, 8, 11, 7,
 7, 7, 9, 9, 11, 8, 12,          2,  2,  2,  5,  2,  5, 6,
 15, 10, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  5, 8, 7, 7,
 7, 7, 7, 7, 7,  3, 6, 6, 7,  4, 10, 7, 7, 7, 7, 10,
 7, 7,  5, 7, 5, 5, 8, 8, 12,  2, 8,         11, 11, 9,
};
//#2500~#257F
double exfont_box_drawing_font_interval[128] = {
 12, 12,  2,  4, 12, 12,  2,  4, 12, 12,  2,  4, 7, 7, 8, 8,
 7, 7, 8, 8, 7, 7, 8, 8, 7, 7, 8, 8, 7, 7, 8, 8,
 8, 8, 8, 8, 7, 7, 8, 8, 8, 8, 8, 8, 12, 12, 12, 12,
 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  2,  4,
 12,  5, 7, 8, 8, 7, 8, 8, 7, 8, 8, 7, 8, 8, 7, 8,
 8, 7, 8, 8, 12, 12, 12, 12, 12, 12, 12, 12, 12, 7, 7, 7,
 7, 11, 11, 11, 5,  2, 5,  2, 5,  4, 5,  4, 12,  4, 12,  4,
};
//#3000~#303F
double exfont_cjk_symbol_and_punctuation_font_interval[64] = {
 25,  5, 5, 7, 16, 12, 13, 15, 6, 6, 8, 8, 7, 7, 7, 7,
 6, 6, 12, 13, 6, 6, 7, 7,  5,  5, 5, 5, 17, 7, 7, 7,
 15,  2, 6, 9, 12, 9, 12, 12, 12, 12, 5, 5, 5, 5,  3,  3,
 16, 8, 12, 11, 14, 9, 15, 16, 16, 16, 16, 6, 13, 13, 17, 10,
};
//
double exfont_devanagari_font_interval[128] = {
 7, 9, 8, 14, 12, 13, 16, 10, 10, 11, 13, 18, 13, 12, 12, 12,
 12, 16, 16, 16, 16, 13, 14, 11, 12, 12, 13, 13, 13, 13, 13, 10,
 10, 11, 10, 13, 11, 11, 10, 11, 11, 11, 11, 13, 11, 12, 12, 11,
 9, 9, 13, 14, 14, 11, 12, 12, 12, 10,  2,  5, 8, 7, 16, 16,
 16, 9, 11, 9, 9, 9, 9, 9, 8, 16, 16, 16, 16, 10,  3, 10,
 15, 8, 12, 8, 8, 7, 8, 8, 13, 14, 11, 13, 11, 10, 13, 11,
 17, 13, 10, 11,  2, 5, 7, 5, 8, 7, 11, 8, 10, 10, 9, 8,
  5,  3, 12, 13, 16, 16, 13, 13, 18, 13, 11, 12, 15, 8, 14, 14,
};
//#2700~#27BF
double exfont_dingbats_font_interval[192] = {
 18, 16, 16, 16, 18, 15, 12, 14, 14, 12, 13, 13, 9, 14, 15, 16,
 15, 16, 16, 13, 14, 13, 13, 11, 12, 13, 13, 13, 13, 8, 10, 9,
 9, 12, 14, 14, 14, 13, 13, 13, 17, 14, 14, 14, 14, 14, 14, 14,
 15, 13, 13, 12, 13, 13, 12, 13, 13, 13, 13, 12, 12, 12, 13, 14,
 14, 13, 13, 12, 12, 12, 13, 14, 13, 12, 12, 14, 11, 15, 13, 13,
 13, 13, 13, 12, 12, 6, 13, 5,  2,  4, 7, 5, 5, 11, 11, 5,
 11, 11, 8, 9, 15, 12, 12, 14, 6, 6, 7, 7, 6, 6, 8, 8,
 7, 7,  5,  5, 6, 6, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
 14, 14, 14, 14, 16, 13, 13, 13, 11, 16, 9, 16, 16, 14, 14, 14,
 15, 15, 16, 16, 14, 16, 16, 7, 16, 15, 14, 15, 15, 12, 12, 15,
 14, 15, 14, 17, 13, 15, 13, 13, 16, 13, 14, 15, 14, 17, 16, 21,
};
//#0020~#007F
double exfont_basic_latin_font_interval[96] = { 13,
 5,  2, 5, 10, 9, 14, 11,  2,  4,  4, 6, 9,  2, 5,  2, 5,
 9,  5, 9, 9, 9, 9, 9, 9, 9, 9,  2,  2, 9, 9, 9, 9,
 17, 13, 10, 11, 11, 10, 9, 12, 10,  2, 7, 11, 8, 12, 10, 13,
 10, 13, 11, 10, 10, 10, 12, 17, 12, 12, 10,  4, 5,  4, 8, 9,
  3, 9, 8, 8, 8, 9, 6, 8, 8,  2,  4, 8,  2, 13, 8, 9,
 8, 8, 5, 8,  5, 8, 9, 13, 9, 9, 8, 5,  2, 5, 9,
};
//#25A0~#25F0
double exfont_geometric_shapes_font_interval[96] = {
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  4,  4, 8, 8,  4,  4,
 8, 8, 8, 8, 8, 8, 8, 8, 5, 5, 9, 9, 8, 8, 6, 6,
 8, 8, 5, 5, 9, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8,
 8, 8, 8, 8, 8, 8,  4,  5, 11, 11, 11, 11,  4,  4,  4,  4,
 8, 8, 8, 8, 8, 8,  4, 8, 8, 8, 8, 8, 8, 8, 8, 11,
 13, 13, 13, 13, 14, 14, 14, 14, 13, 13, 13, 11, 11, 11, 11, 14,
};
//
double exfont_telugu_font_interval[96] = {
 8, 16, 20, 15,     11, 13, 9, 20, 11, 17, 22, 15,     10, 10,
 11,     11, 11, 16, 9, 11, 9, 16, 10, 11, 11, 10, 20, 13, 14,
 9, 11, 11, 13, 11, 11, 11, 11, 10,     10, 10, 11, 12, 16, 20,
 9, 14, 10, 10, 13, 10, 14, 14, 11, 16,             11, 21, 15,
 15, 15, 22, 15, 22,     21, 21, 23,     24, 24, 25, 17,
					 14, 14,     13, 13, 11,
 25, 23, 6, 9,         10, 13, 13, 8, 11, 14, 13, 9, 14, 13,
								 9,  2, 7, 12, 13, 13, 13, 13,
};
//#A490~#A4C6
double exfont_yi_radicals_font_interval[55] = {
  2, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10, 7, 11, 11, 11, 7,
 11, 11, 11, 11, 11, 11, 11, 11, 11, 7, 11, 11, 11, 11, 11, 9,
 10, 10, 8, 11, 11, 11, 11, 11, 10, 11, 11, 10, 11, 11, 11, 11,
 11, 11, 11, 11, 11, 7,
};
//#3040~#309F
double exfont_hiragana_font_interval[93] = {
	 11, 14, 11, 14, 11, 13, 12, 14, 12, 14, 14, 16, 14, 16, 12,
 14, 14, 16, 13, 14, 14, 15, 12, 13, 14, 15, 15, 16, 15, 16, 14,
 15, 13, 15, 11, 14, 15, 14, 15, 12, 14, 16, 14, 15, 15, 14, 15,
 16, 16, 15, 15, 15, 16, 16, 16, 15, 15, 15, 15, 15, 16, 19, 14,
 15, 14, 14, 12, 15, 11, 14, 12, 14, 12, 11, 14, 15, 14, 12, 14,
 14, 15, 14, 15, 16, 12, 11,          4,  5,  4,  5, 12, 13, 11,
};
//#30A0~#30FF
double exfont_katakana_font_interval[96] = {
 10, 12, 16, 10, 14, 11, 14, 11, 15, 12, 16, 15, 15, 16, 16, 14,
 16, 16, 16, 14, 15, 15, 17, 15, 15, 16, 16, 15, 15, 13, 14, 14,
 16, 15, 16, 11, 14, 15, 16, 16, 11, 10, 15, 15, 14, 16, 14, 16,
 17, 17, 13, 15, 15, 13, 15, 16, 16, 16, 16, 16, 16, 16, 14, 15,
 16, 13, 15, 11, 14, 12, 16, 11, 14, 15, 11, 16, 13, 13, 10, 14,
 15, 16, 14, 15, 14, 11, 12, 16, 16, 16, 15,  3, 15, 9, 9, 10,
};
//to do
double exfont_halfwidth_and_fullwidth_forms_font_interval = 12;
double exfont_mathematical_operators_font_interval = 12;
double exfont_latin_1_supplement_font_interval = 12;
double exfont_miscellaneous_symbols_font_interval = 12;
double exfont_greek_and_coptic_font_interval = 12;
double exfont_cyrillic_font_interval = 12;
double exfont_ipa_extensions_font_interval = 12;
double exfont_gurmukhi_font_interval = 12;
double exfont_general_punctuation_font_interval = 12;
double exfont_block_elements_font_interval = 12;
double exfont_phonetic_extensions_font_interval = 12;
double exfont_thai_font_interval = 12;
double exfont_georgian_font_interval = 12;
double exfont_hangul_compatibility_jamo_font_interval = 12;
double exfont_miscellaneous_technical_font_interval = 12;
double exfont_tamil_font_interval = 12;
double exfont_spacing_modifier_letters_font_interval = 12;
double exfont_tibetan_font_interval = 12;
double exfont_arrows_font_interval = 12;
double exfont_cyrillic_supplement_font_interval = 12;
double exfont_superscripts_and_subscripts_font_interval = 12;
double exfont_cjk_compatibility_forms_font_interval = 12;
double exfont_combining_diacritical_marks_font_interval = 12;
double exfont_optical_character_recognition_font_interval = 12;
double exfont_greek_extended_font_interval = 12;
double exfont_miscellaneous_symbols_and_pictographs_font_interval = 12;
double exfont_unified_canadian_aboriginal_syllabics_font_interval = 12;
double exfont_hebrew_font_interval = 12;
double exfont_lao_font_interval = 12;
double exfont_kannada_font_interval = 12;
double exfont_combining_diacritical_marks_for_symbols_font_interval = 12;
double exfont_combining_diacritical_marks_supplement_font_interval = 12;
double exfont_sinhala_font_interval = 12;
double exfont_yi_syllables_font_interval = 12;
double exfont_font_pos;

//#0000~#007F
std::string exfont_basic_latin_font_sample[128];
//#0080~#00FF
std::string exfont_latin_1_supplement_font_sample[128];
//#0250~#02AF
std::string exfont_ipa_extensions_font_sample[96];
//#02B0~#02FF
std::string exfont_spacing_modifier_letters_font_sample[80];
//#0300~#036F
std::string exfont_combining_diacritical_marks_font_sample[112];
//#0370~#03FF
std::string exfont_greek_and_coptic_font_sample[135];
//#0400~#04FF
std::string exfont_cyrillic_font_sample[256];
//#0500~#052F
std::string exfont_cyrillic_supplement_font_sample[48];
//#0530~#058F
std::string exfont_armenian_sample[91];
//#0590~#05FF
std::string exfont_hebrew_font_sample[88];
//#0600~#06FF
std::string exfont_arabic_sample[255];
std::string exfont_arabic_right_to_left_sample[235];
//#0900~#097F
std::string exfont_devanagari_font_sample[128];
//#0A00~#0A7F
std::string exfont_gurmukhi_font_sample[79];
//#0B80~#0BFF
std::string exfont_tamil_font_sample[72];
//#0C00~0C7F
std::string exfont_telugu_font_sample[96];
//#0C80~#0CFF
std::string exfont_kannada_font_sample[88];
//#0D80~#0DFF
std::string exfont_sinhala_font_sample[90];
//#0E00~#0E7F
std::string exfont_thai_font_sample[87];
//#0E80~#0EFF
std::string exfont_lao_font_sample[67];
//#0F00~#0FFF
std::string exfont_tibetan_font_sample[211];
//#10A0~#10FF
std::string exfont_georgian_font_sample[88];
//#1400~#167F
std::string exfont_unified_canadian_aboriginal_syllabics_font_sample[640];
//#1D00~#1D7F
std::string exfont_phonetic_extensions_font_sample[128];
//#1DC0~#1DFF
std::string exfont_combining_diacritical_marks_supplement_font_sample[63];
//#1F00~#1FFF
std::string exfont_greek_extended_font_sample[233];
//#2000~#206F
std::string exfont_general_punctuation_font_sample[111];
//#2070~#209F
std::string exfont_superscripts_and_subscripts_font_sample[42];
//#20D0~#20FF
std::string exfont_combining_diacritical_marks_for_symbols_font_sample[33];
//#2190~#21FF
std::string exfont_arrows_font_sample[112];
//#2200~#22FF
std::string exfont_mathematical_operators_font_sample[256];
//#2300~#23FF
std::string exfont_miscellaneous_technical_font_sample[256];
//#2440~#245F
std::string exfont_optical_character_recognition_font_sample[11];
//#2500~#257F
std::string exfont_box_drawing_font_sample[128];
//#2580~#259F
std::string exfont_block_elements_font_sample[32];
//#25A0~#25F0
std::string exfont_geometric_shapes_font_sample[96];
//#2600~#26FF
std::string exfont_miscellaneous_symbols_font_sample[256];
//#2700~#27BF
std::string exfont_dingbats_font_sample[192];
//#3000~#303F
std::string exfont_cjk_symbol_and_punctuation_font_sample[64];
//#3040~#309F
std::string exfont_hiragana_font_sample[93];
//#30A0~#30FF
std::string exfont_katakana_font_sample[96];
//#3130~#318F
std::string exfont_hangul_compatibility_jamo_font_sample[94];
//#A000~#A48F
std::string exfont_yi_syllables_font_sample[1165];
//#A490~#A4C6
std::string exfont_yi_radicals_font_sample[55];
//#FE30~#FE4F
std::string exfont_cjk_compatibility_forms_font_sample[32];
//#FF00~#FFEF
std::string exfont_halfwidth_and_fullwidth_forms_font_sample[225];
//#1F300~#1F5FF
std::string exfont_miscellaneous_symbols_and_pictographs_font_sample[768];

std::string exfont_font_samples[62] = {
 "\u0080","\u0400","\u0800","\u0C00","\u1000","\u1400","\u1800","\u1C00","\u2000","\u2400","\u2800","\u2C00","\u3000","\u3400","\u3800","\u3C00",
 "\u4000","\u4400","\u4800","\u4C00","\u5000","\u5400","\u5800","\u5C00","\u6000","\u6400","\u6800","\u6C00","\u7000","\u7400","\u7800","\u7C00",
 "\u8000","\u8400","\u8800","\u8C00","\u9000","\u9400","\u9800","\u9C00","\uA000","\uA400","\uA800","\uAC00","\uB000","\uB400","\uB800","\uBC00",
 "\uC000","\uC400","\uC800","\uCC00","\uD000","\uD400",/*uD800    uDC00*/"\uE000","\uE400","\uE800","\uEC00","\uF000","\uF400","\uF800","\uFC00",
};

std::string exfont_font_name[EXFONT_NUM_OF_FONT_NAME];

/*
   0 ~   95   (96) Basic latin
  96 ~  223  (128) Latin 1 supplement
 224 ~  319   (96) Ipa extensions
 320 ~  399   (80) Spacing modifier letters
 400 ~  511  (112) Combining diacritical marks
 512 ~  646  (135) Greek and coptic
 647 ~  902  (256) Cyrillic
 903 ~  950   (48) Cyrillic supplement
 951 ~ 1041   (92) Armenian
1042 ~ 1129   (88) Hebrew
1130 ~ 1384  (255) Arabic
1385 ~ 1512  (128) Devanagari
1513 ~ 1591   (79) Gurmukhi
1592 ~ 1663   (72) Tamil
1664 ~ 1759   (96) Telugu
1760 ~ 1847   (89) Kannada
1848 ~ 1937   (90) Sinhala
1938 ~ 2024   (87) Thai
2025 ~ 2091   (67) Lao
2092 ~ 2302  (211) Tibetan
2303 ~ 2390   (88) Georgian
2391 ~ 3031  (640) Unified canadian aboriginal syllabics
3032 ~ 3158  (128) Phonetic extensions
3159 ~ 3221   (63) Combining diacritical marks supplement
3222 ~ 3454  (233) Greek extended
3455 ~ 3565  (111) General punctuation
3566 ~ 3607   (42) Superscripts and subscripts
3608 ~ 3640   (33) Combining diacritical marks for symbols
3641 ~ 3752  (112) Arrows
3753 ~ 4008  (256) Mathematical operators
4009 ~ 4264  (256) Miscellaneous technical
4265 ~ 4275   (11) Optical character recognition
4276 ~ 4403  (128) Box drawing
4404 ~ 4435   (32) Block elements
4436 ~ 4531   (96) Geometric shapes
4532 ~ 4787  (256) Miscellaneous_symbols
4788 ~ 4979  (192) Dingbats
4980 ~ 5043   (64) Cjk symbol and punctuation
5044 ~ 5136   (93) Hiragana
5137 ~ 5232   (96) Katakana
5233 ~ 5326   (94) Hangul compatibility jamo
5327 ~ 6491 (1165) Yi syllables
6492 ~ 6546   (55) Yi radicals
6547 ~ 6578   (32) Cjk compatibility forms
6579 ~ 6803  (225) Halfwidth and fullwidth forms
6804 ~ 7571  (768) Miscellaneous symbols and pictographs
*/
C2D_Image exfont_font_images[7572];

int exfont_font_characters[46] = {
  96, 128,  96,  80, 112, 135, 256,  48,  91,  88,  255, 128,  79,  72,  96,  88,
  90,  87,  67, 211,  88, 640, 128,  63, 233,  111,  42,  33, 112, 256, 256,  11,
 128,  32,  96, 256, 192,  64,  93,  96,  94, 1165,  55,  32, 225, 768,
};
int exfont_font_start_num[46];

void Exfont_init_sample(int num_of_loop, std::string samples[], int byte_0_start, int byte_1_start, int byte_2_start, int byte_3_start, int bytes_per_char, int invalid_list[], int num_of_invalid)
{
	int count = 0;
	int invalid_count = 0;
	int byte_0 = byte_0_start;
	int byte_1 = byte_1_start;
	int byte_2 = byte_2_start;
	int byte_3 = byte_3_start;

	if (bytes_per_char == 1)
	{
		for (int i = 0; i < num_of_loop; i++)
		{
			if (num_of_invalid <= invalid_count)
			{
				samples[count].reserve(2);
				memset((void*)samples[count].c_str(), byte_0, 0x1);
				count++;
			}
			else
			{
				if (i == invalid_list[invalid_count])
					invalid_count++;
				else
				{
					samples[count].reserve(2);
					memset((void*)samples[count].c_str(), byte_0, 0x1);
					count++;
				}
			}

			if (byte_0 + 0x1 > 0x7F)
				break;
			else
				byte_0 += 0x1;
		}
	}
	else if (bytes_per_char == 2)
	{
		for (int i = 0; i < num_of_loop; i++)
		{
			if (num_of_invalid <= invalid_count)
			{
				samples[count].reserve(3);
				memset((void*)samples[count].c_str(), byte_0, 0x1);
				memset((void*)(samples[count].c_str() + 1), byte_1, 0x1);
				count++;
			}
			else
			{
				if (i == invalid_list[invalid_count])
					invalid_count++;
				else
				{
					samples[count].reserve(3);
					memset((void*)samples[count].c_str(), byte_0, 0x1);
					memset((void*)(samples[count].c_str() + 1), byte_1, 0x1);
					count++;
				}
			}

			if (byte_1 + 0x1 > 0xBF)
			{
				if (byte_0 + 0x1 > 0xDF)
					break;

				byte_0 += 0x1;
				byte_1 = 0x80;
			}
			else
				byte_1 += 0x1;
		}
	}
	else if (bytes_per_char == 3)
	{
		for (int i = 0; i < num_of_loop; i++)
		{
			if (num_of_invalid <= invalid_count)
			{
				samples[count].reserve(4);
				memset((void*)samples[count].c_str(), byte_0, 0x1);
				memset((void*)(samples[count].c_str() + 1), byte_1, 0x1);
				memset((void*)(samples[count].c_str() + 2), byte_2, 0x1);
				count++;
			}
			else
			{
				if (i == invalid_list[invalid_count])
					invalid_count++;
				else
				{
					samples[count].reserve(4);
					memset((void*)samples[count].c_str(), byte_0, 0x1);
					memset((void*)(samples[count].c_str() + 1), byte_1, 0x1);
					memset((void*)(samples[count].c_str() + 2), byte_2, 0x1);
					count++;
				}
			}

			if (byte_2 + 0x1 > 0xBF)
			{
				if (byte_1 + 0x1 > 0xBF)
				{
					if (byte_0 + 0x1 > 0xEF)
						break;

					byte_0 += 0x1;
					byte_1 = 0x80;
					byte_2 = 0x80;
				}
				else
				{
					byte_1 += 0x1;
					byte_2 = 0x80;
				}
			}
			else
				byte_2 += 0x1;
		}
	}
	else if (bytes_per_char == 4)
	{
		for (int i = 0; i < num_of_loop; i++)
		{
			if (num_of_invalid <= invalid_count)
			{
				samples[count].reserve(5);
				memset((void*)samples[count].c_str(), byte_0, 0x1);
				memset((void*)(samples[count].c_str() + 1), byte_1, 0x1);
				memset((void*)(samples[count].c_str() + 2), byte_2, 0x1);
				memset((void*)(samples[count].c_str() + 3), byte_3, 0x1);
				count++;
			}
			else
			{
				if (i == invalid_list[invalid_count])
					invalid_count++;
				else
				{
					samples[count].reserve(5);
					memset((void*)samples[count].c_str(), byte_0, 0x1);
					memset((void*)(samples[count].c_str() + 1), byte_1, 0x1);
					memset((void*)(samples[count].c_str() + 2), byte_2, 0x1);
					memset((void*)(samples[count].c_str() + 3), byte_3, 0x1);
					count++;
				}
			}

			if (byte_3 + 0x1 > 0xBF)
			{
				if (byte_2 + 0x1 > 0xBF)
				{
					if (byte_1 + 0x1 > 0xBF)
					{
						if (byte_0 + 0x1 > 0xFF)
							break;

						byte_0 += 0x1;
						byte_1 = 0x80;
						byte_2 = 0x80;
						byte_3 = 0x80;
					}
					else
					{
						byte_1 += 0x1;
						byte_2 = 0x80;
						byte_3 = 0x80;
					}
				}
				else
				{
					byte_2 += 0x1;
					byte_3 = 0x80;
				}
			}
			else
				byte_3 += 0x1;
		}
	}
}

void Exfont_init(void)
{
	int dammy_start = 0x0;
	int dammy_invalid[1] = { 0x0, };

	int basic_latin_byte_0_start = 0x00;

	int latin_1_supplement_byte_0_start = 0xC2;
	int latin_1_supplement_byte_1_start = 0x80;

	int ipa_extensions_byte_0_start = 0xC9;
	int ipa_extensions_byte_1_start = 0x90;

	int spacing_modifier_letters_byte_0_start = 0xCA;
	int spacing_modifier_letters_byte_1_start = 0xB0;

	int combining_diacritical_marks_byte_0_start = 0xCC;
	int combining_diacritical_marks_byte_1_start = 0x80;

	int greek_and_coptic_num_of_invalid = 9;
	int greek_and_coptic_byte_0_start = 0xCD;
	int greek_and_coptic_byte_1_start = 0xB0;
	int greek_and_coptic_invalid_list[9] = { 8, 9, 16, 17, 18, 19, 27, 29, 50, };

	int cyrillic_byte_0_start = 0xD0;
	int cyrillic_byte_1_start = 0x80;

	int cyrillic_supplement_byte_0_start = 0xD4;
	int cyrillic_supplement_byte_1_start = 0x80;

	int armenian_num_of_invalid = 5;
	int armenian_byte_0_start = 0xD4;
	int armenian_byte_1_start = 0xB0;
	int armenian_invalid_list[5] = { 0, 39, 40, 91, 92, };

	int hebrew_num_of_invalid = 24;
	int hebrew_byte_0_start = 0xD6;
	int hebrew_byte_1_start = 0x90;
	int hebrew_invalid_list[24] = { 0,  56,  57,  58,  59,  60,  61,  62,  63,  91,  92,  93,  94, 101, 102, 103,
									104, 105, 106, 107, 108, 109, 110, 111, };

	int arabic_num_of_invalid = 1;
	int arabic_byte_0_start = 0xD8;
	int arabic_byte_1_start = 0x80;
	int arabic_invalid_list[1] = { 29, };

	int arabic_right_to_left_num_of_invalid = 21;
	int arabic_right_to_left_invalid_list[21] = { 29,  96,  97,  98,  99, 100, 101 ,102, 103, 104, 105, 240, 241, 242, 243, 244,
												  245, 246, 247, 248, 249, };

	int devanagari_byte_0_start = 0xE0;
	int devanagari_byte_1_start = 0xA4;
	int devanagari_byte_2_start = 0x80;

	int gurmukhi_num_of_invalid = 49;
	int gurmukhi_byte_0_start = 0xE0;
	int gurmukhi_byte_1_start = 0xA8;
	int gurmukhi_byte_2_start = 0x80;
	int gurmukhi_invalid_list[49] = { 0,   4,  11,  12,  13,  14,  17,  18,  41,  49,  52,  55,  58,  59,  61,  67,
									68,  69,  70,  73,  74,  78,  79,  80,  82,  83,  84,  85,  86,  87,  88,  93,
									95,  96,  97,  98,  99, 100, 101, 118, 119, 120, 121, 122, 123, 124, 125, 126,
								   127, };

	int tamil_num_of_invalid = 56;
	int tamil_byte_0_start = 0xE0;
	int tamil_byte_1_start = 0xAE;
	int tamil_byte_2_start = 0x80;
	int tamil_invalid_list[56] = { 0,   1,   4,  11,  12,  13,  17,  22,  23,  24,  27,  29,  32,  33,  34,  37,
									38,  39,  43,  44,  45,  58,  59,  60,  61,  67,  68,  69,  73,  78,  79,  81,
									82,  83,  84,  85,  86,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,
									99, 100, 101, 123, 124, 125, 126, 127, };

	int telugu_num_of_invalid = 32;
	int telugu_byte_0_start = 0xE0;
	int telugu_byte_1_start = 0xB0;
	int telugu_byte_2_start = 0x80;
	int telugu_invalid_list[32] = { 4,  13,  17,  41,  58,  59,  60,  69,  73,  78,  79,  80,  81,  82,  83,  84,
									 87,  91,  92,  93,  94,  95, 100, 101, 112, 113, 114, 115, 116, 117, 118, 119, };

	int kannada_num_of_invalid = 40;
	int kannada_byte_0_start = 0xE0;
	int kannada_byte_1_start = 0xB2;
	int kannada_byte_2_start = 0x80;
	int kannada_invalid_list[40] = { 4,  13,  17,  41,  52,  58,  59,  69,  73,  78,  79,  80,  81,  82,  83,  84,
									  87,  88,  89,  90,  91,  92,  93,  95, 100, 101, 112, 115, 116, 117, 118, 119,
									 120, 121, 122, 123, 124, 125, 126, 127, };

	int sinhala_num_of_invalid = 38;
	int sinhala_byte_0_start = 0xE0;
	int sinhala_byte_1_start = 0xB6;
	int sinhala_byte_2_start = 0x80;
	int sinhala_invalid_list[38] = { 0,   1,   4,  23,  24,  25,  50,  60,  62,  63,  71,  72,  73,  75,  76,  77,
									  78,  85,  87,  96,  97,  98,  99, 100, 101, 112, 113, 117, 118, 119, 120, 121,
									 122, 123, 124, 125, 126, 127, };

	int thai_num_of_invalid = 41;
	int thai_byte_0_start = 0xE0;
	int thai_byte_1_start = 0xB8;
	int thai_byte_2_start = 0x80;
	int thai_invalid_list[41] = { 0,  59,  60,  61,  62,  92,  93,  94,  95,  96,  97,  98,  99, 100, 101, 102,
								  103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118,
								  119, 120, 121, 122, 123, 124, 125, 126, 127, };

	int lao_num_of_invalid = 61;
	int lao_byte_0_start = 0xE0;
	int lao_byte_1_start = 0xBA;
	int lao_byte_2_start = 0x80;
	int lao_invalid_list[61] = { 0,   3,   5,   6,   9,  11,  12,  14,  15,  16,  17,  18,  19,  24,  32,  36,
								  38,  40,  41,  44,  58,  62,  63,  69,  71,  78,  79,  90,  91,  96,  97,  98,
								  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114,
								 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, };

	int tibetan_num_of_invalid = 45;
	int tibetan_byte_0_start = 0xE0;
	int tibetan_byte_1_start = 0xBC;
	int tibetan_byte_2_start = 0x80;
	int tibetan_invalid_list[45] = { 72, 109, 110, 111, 112, 152, 189, 205, 219, 220, 221, 222, 223, 224, 225, 226,
									 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242,
									 243, 244, 245, 246, 247, 238, 249, 250, 251, 252, 253, 254, 255, };

	int georgian_num_of_invalid = 8;
	int georgian_byte_0_start = 0xE1;
	int georgian_byte_1_start = 0x82;
	int georgian_byte_2_start = 0xA0;
	int georgian_invalid_list[8] = { 38,  40,  41,  42,  43,  44,  46,  47, };

	int unified_canadian_aboriginal_syllabics_byte_0_start = 0xE1;
	int unified_canadian_aboriginal_syllabics_byte_1_start = 0x90;
	int unified_canadian_aboriginal_syllabics_byte_2_start = 0x80;

	int phonetic_extensions_byte_0_start = 0xE1;
	int phonetic_extensions_byte_1_start = 0xB4;
	int phonetic_extensions_byte_2_start = 0x80;

	int combining_diacritical_marks_supplement_num_of_invalid = 1;
	int combining_diacritical_marks_supplement_byte_0_start = 0xE1;
	int combining_diacritical_marks_supplement_byte_1_start = 0xB7;
	int combining_diacritical_marks_supplement_byte_2_start = 0x80;
	int combining_diacritical_marks_supplement_invalid_list[1] = { 58, };

	int greek_extended_num_of_invalid = 23;
	int greek_extended_byte_0_start = 0xE1;
	int greek_extended_byte_1_start = 0xBC;
	int greek_extended_byte_2_start = 0x80;
	int greek_extended_invalid_list[23] = { 22,  23,  30,  31,  70,  71,  78,  79,  88,  90,  92,  94, 126, 127, 181, 197,
											212, 213, 220, 240, 241, 245, 255, };

	int general_punctuation_num_of_invalid = 1;
	int general_punctuation_byte_0_start = 0xE2;
	int general_punctuation_byte_1_start = 0x80;
	int general_punctuation_byte_2_start = 0x80;
	int general_punctuation_invalid_list[1] = { 101, };

	int superscripts_and_subscripts_num_of_invalid = 6;
	int superscripts_and_subscripts_byte_0_start = 0xE2;
	int superscripts_and_subscripts_byte_1_start = 0x81;
	int superscripts_and_subscripts_byte_2_start = 0xB0;
	int superscripts_and_subscripts_invalid_list[6] = { 2,  3,  31,  45,  46,  47, };

	int combining_diacritical_marks_for_symbols_num_of_invalid = 15;
	int combining_diacritical_marks_for_symbols_byte_0_start = 0xE2;
	int combining_diacritical_marks_for_symbols_byte_1_start = 0x83;
	int combining_diacritical_marks_for_symbols_byte_2_start = 0x90;
	int combining_diacritical_marks_for_symbols_invalid_list[15] = { 33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47, };

	int arrows_byte_0_start = 0xE2;
	int arrows_byte_1_start = 0x86;
	int arrows_byte_2_start = 0x90;

	int mathematical_operators_byte_0_start = 0xE2;
	int mathematical_operators_byte_1_start = 0x88;
	int mathematical_operators_byte_2_start = 0x80;

	int miscellaneous_technical_byte_0_start = 0xE2;
	int miscellaneous_technical_byte_1_start = 0x8C;
	int miscellaneous_technical_byte_2_start = 0x80;

	int optical_character_recognition_byte_0_start = 0xE2;
	int optical_character_recognition_byte_1_start = 0x91;
	int optical_character_recognition_byte_2_start = 0x80;

	int box_drawing_byte_0_start = 0xE2;
	int box_drawing_byte_1_start = 0x94;
	int box_drawing_byte_2_start = 0x80;

	int block_elements_byte_0_start = 0xE2;
	int block_elements_byte_1_start = 0x96;
	int block_elements_byte_2_start = 0x80;

	int geometric_shapes_byte_0_start = 0xE2;
	int geometric_shapes_byte_1_start = 0x96;
	int geometric_shapes_byte_2_start = 0xA0;

	int miscellaneous_symbols_byte_0_start = 0xE2;
	int miscellaneous_symbols_byte_1_start = 0x98;
	int miscellaneous_symbols_byte_2_start = 0x80;

	int dingbats_byte_0_start = 0xE2;
	int dingbats_byte_1_start = 0x9C;
	int dingbats_byte_2_start = 0x80;

	int cjk_symbol_and_punctuation_byte_0_start = 0xE3;
	int cjk_symbol_and_punctuation_byte_1_start = 0x80;
	int cjk_symbol_and_punctuation_byte_2_start = 0x80;

	int hiragana_num_of_invalid = 3;
	int hiragana_byte_0_start = 0xE3;
	int hiragana_byte_1_start = 0x81;
	int hiragana_byte_2_start = 0x80;
	int hiragana_invalid_list[3] = { 0,  87,  88, };

	int katakana_byte_0_start = 0xE3;
	int katakana_byte_1_start = 0x82;
	int katakana_byte_2_start = 0xA0;

	int hangul_compatibility_jamo_num_of_invalid = 2;
	int hangul_compatibility_jamo_byte_0_start = 0xE3;
	int hangul_compatibility_jamo_byte_1_start = 0x84;
	int hangul_compatibility_jamo_byte_2_start = 0xB0;
	int hangul_compatibility_jamo_invalid_list[2] = { 0,  95, };

	int yi_syllables_num_of_invalid = 3;
	int yi_syllables_byte_0_start = 0xEA;
	int yi_syllables_byte_1_start = 0x80;
	int yi_syllables_byte_2_start = 0x80;
	int yi_syllables_invalid_list[3] = { 1165, 1166, 1167, };

	int yi_radicals_num_of_invalid = 9;
	int yi_radicals_byte_0_start = 0xEA;
	int yi_radicals_byte_1_start = 0x92;
	int yi_radicals_byte_2_start = 0x90;
	int yi_radicals_invalid_list[9] = { 55,  56,  57,  58,  59,  60,  61,  62,  63, };

	int cjk_compatibility_forms_byte_0_start = 0xEF;
	int cjk_compatibility_forms_byte_1_start = 0xB8;
	int cjk_compatibility_forms_byte_2_start = 0xB0;

	int halfwidth_and_fullwidth_forms_num_of_invalid = 15;
	int halfwidth_and_fullwidth_forms_byte_0_start = 0xEF;
	int halfwidth_and_fullwidth_forms_byte_1_start = 0xBC;
	int halfwidth_and_fullwidth_forms_byte_2_start = 0x80;
	int halfwidth_and_fullwidth_forms_invalid_list[15] = { 0, 191, 192, 193, 200, 201, 208, 209, 216, 217, 221, 222, 223, 231, 239, };

	int miscellaneous_symbols_and_pictographs_byte_0_start = 0xF0;
	int miscellaneous_symbols_and_pictographs_byte_1_start = 0x9F;
	int miscellaneous_symbols_and_pictographs_byte_2_start = 0x8C;
	int miscellaneous_symbols_and_pictographs_byte_3_start = 0x80;

	exfont_font_start_num[0] = 0;
	for (int i = 1; i < 46; i++)
		exfont_font_start_num[i] = exfont_font_start_num[i - 1] + exfont_font_characters[i - 1];

	Exfont_init_sample(128, exfont_basic_latin_font_sample, basic_latin_byte_0_start, dammy_start, dammy_start, dammy_start, 1, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[1], exfont_latin_1_supplement_font_sample, latin_1_supplement_byte_0_start, latin_1_supplement_byte_1_start, dammy_start, dammy_start, 2, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[2], exfont_ipa_extensions_font_sample, ipa_extensions_byte_0_start, ipa_extensions_byte_1_start, dammy_start, dammy_start, 2, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[3], exfont_spacing_modifier_letters_font_sample, spacing_modifier_letters_byte_0_start, spacing_modifier_letters_byte_1_start, dammy_start, dammy_start, 2, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[4], exfont_combining_diacritical_marks_font_sample, combining_diacritical_marks_byte_0_start, combining_diacritical_marks_byte_1_start, dammy_start, dammy_start, 2, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[5] + greek_and_coptic_num_of_invalid, exfont_greek_and_coptic_font_sample, greek_and_coptic_byte_0_start, greek_and_coptic_byte_1_start, dammy_start, dammy_start, 2, greek_and_coptic_invalid_list, greek_and_coptic_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[6], exfont_cyrillic_font_sample, cyrillic_byte_0_start, cyrillic_byte_1_start, dammy_start, dammy_start, 2, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[7], exfont_cyrillic_supplement_font_sample, cyrillic_supplement_byte_0_start, cyrillic_supplement_byte_1_start, dammy_start, dammy_start, 2, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[8] + armenian_num_of_invalid, exfont_armenian_sample, armenian_byte_0_start, armenian_byte_1_start, dammy_start, dammy_start, 2, armenian_invalid_list, armenian_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[9] + hebrew_num_of_invalid, exfont_hebrew_font_sample, hebrew_byte_0_start, hebrew_byte_1_start, dammy_start, dammy_start, 2, hebrew_invalid_list, hebrew_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[10] + arabic_num_of_invalid, exfont_arabic_sample, arabic_byte_0_start, arabic_byte_1_start, dammy_start, dammy_start, 2, arabic_invalid_list, arabic_num_of_invalid);
	Exfont_init_sample(235 + arabic_right_to_left_num_of_invalid, exfont_arabic_right_to_left_sample, arabic_byte_0_start, arabic_byte_1_start, dammy_start, dammy_start, 2, arabic_right_to_left_invalid_list, arabic_right_to_left_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[11], exfont_devanagari_font_sample, devanagari_byte_0_start, devanagari_byte_1_start, devanagari_byte_2_start, dammy_start, 3, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[12] + gurmukhi_num_of_invalid, exfont_gurmukhi_font_sample, gurmukhi_byte_0_start, gurmukhi_byte_1_start, gurmukhi_byte_2_start, dammy_start, 3, gurmukhi_invalid_list, gurmukhi_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[13] + tamil_num_of_invalid, exfont_tamil_font_sample, tamil_byte_0_start, tamil_byte_1_start, tamil_byte_2_start, dammy_start, 3, tamil_invalid_list, tamil_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[14] + telugu_num_of_invalid, exfont_telugu_font_sample, telugu_byte_0_start, telugu_byte_1_start, telugu_byte_2_start, dammy_start, 3, telugu_invalid_list, telugu_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[15] + kannada_num_of_invalid, exfont_kannada_font_sample, kannada_byte_0_start, kannada_byte_1_start, kannada_byte_2_start, dammy_start, 3, kannada_invalid_list, kannada_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[16] + sinhala_num_of_invalid, exfont_sinhala_font_sample, sinhala_byte_0_start, sinhala_byte_1_start, sinhala_byte_2_start, dammy_start, 3, sinhala_invalid_list, sinhala_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[17] + thai_num_of_invalid, exfont_thai_font_sample, thai_byte_0_start, thai_byte_1_start, thai_byte_2_start, dammy_start, 3, thai_invalid_list, thai_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[18] + lao_num_of_invalid, exfont_lao_font_sample, lao_byte_0_start, lao_byte_1_start, lao_byte_2_start, dammy_start, 3, lao_invalid_list, lao_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[19] + tibetan_num_of_invalid, exfont_tibetan_font_sample, tibetan_byte_0_start, tibetan_byte_1_start, tibetan_byte_2_start, dammy_start, 3, tibetan_invalid_list, tibetan_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[20] + georgian_num_of_invalid, exfont_georgian_font_sample, georgian_byte_0_start, georgian_byte_1_start, georgian_byte_2_start, dammy_start, 3, georgian_invalid_list, georgian_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[21], exfont_unified_canadian_aboriginal_syllabics_font_sample, unified_canadian_aboriginal_syllabics_byte_0_start, unified_canadian_aboriginal_syllabics_byte_1_start, unified_canadian_aboriginal_syllabics_byte_2_start, dammy_start, 3, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[22], exfont_phonetic_extensions_font_sample, phonetic_extensions_byte_0_start, phonetic_extensions_byte_1_start, phonetic_extensions_byte_2_start, dammy_start, 3, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[23] + combining_diacritical_marks_supplement_num_of_invalid, exfont_combining_diacritical_marks_supplement_font_sample, combining_diacritical_marks_supplement_byte_0_start, combining_diacritical_marks_supplement_byte_1_start, combining_diacritical_marks_supplement_byte_2_start, dammy_start, 3, combining_diacritical_marks_supplement_invalid_list, combining_diacritical_marks_supplement_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[24] + greek_extended_num_of_invalid, exfont_greek_extended_font_sample, greek_extended_byte_0_start, greek_extended_byte_1_start, greek_extended_byte_2_start, dammy_start, 3, greek_extended_invalid_list, greek_extended_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[25] + general_punctuation_num_of_invalid, exfont_general_punctuation_font_sample, general_punctuation_byte_0_start, general_punctuation_byte_1_start, general_punctuation_byte_2_start, dammy_start, 3, general_punctuation_invalid_list, general_punctuation_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[26] + superscripts_and_subscripts_num_of_invalid, exfont_superscripts_and_subscripts_font_sample, superscripts_and_subscripts_byte_0_start, superscripts_and_subscripts_byte_1_start, superscripts_and_subscripts_byte_2_start, dammy_start, 3, superscripts_and_subscripts_invalid_list, superscripts_and_subscripts_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[27] + combining_diacritical_marks_for_symbols_num_of_invalid, exfont_combining_diacritical_marks_for_symbols_font_sample, combining_diacritical_marks_for_symbols_byte_0_start, combining_diacritical_marks_for_symbols_byte_1_start, combining_diacritical_marks_for_symbols_byte_2_start, dammy_start, 3, combining_diacritical_marks_for_symbols_invalid_list, combining_diacritical_marks_for_symbols_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[28], exfont_arrows_font_sample, arrows_byte_0_start, arrows_byte_1_start, arrows_byte_2_start, dammy_start, 3, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[29], exfont_mathematical_operators_font_sample, mathematical_operators_byte_0_start, mathematical_operators_byte_1_start, mathematical_operators_byte_2_start, dammy_start, 3, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[30], exfont_miscellaneous_technical_font_sample, miscellaneous_technical_byte_0_start, miscellaneous_technical_byte_1_start, miscellaneous_technical_byte_2_start, dammy_start, 3, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[31], exfont_optical_character_recognition_font_sample, optical_character_recognition_byte_0_start, optical_character_recognition_byte_1_start, optical_character_recognition_byte_2_start, dammy_start, 3, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[32], exfont_box_drawing_font_sample, box_drawing_byte_0_start, box_drawing_byte_1_start, box_drawing_byte_2_start, dammy_start, 3, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[33], exfont_block_elements_font_sample, block_elements_byte_0_start, block_elements_byte_1_start, block_elements_byte_2_start, dammy_start, 3, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[34], exfont_geometric_shapes_font_sample, geometric_shapes_byte_0_start, geometric_shapes_byte_1_start, geometric_shapes_byte_2_start, dammy_start, 3, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[35], exfont_miscellaneous_symbols_font_sample, miscellaneous_symbols_byte_0_start, miscellaneous_symbols_byte_1_start, miscellaneous_symbols_byte_2_start, dammy_start, 3, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[36], exfont_dingbats_font_sample, dingbats_byte_0_start, dingbats_byte_1_start, dingbats_byte_2_start, dammy_start, 3, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[37], exfont_cjk_symbol_and_punctuation_font_sample, cjk_symbol_and_punctuation_byte_0_start, cjk_symbol_and_punctuation_byte_1_start, cjk_symbol_and_punctuation_byte_2_start, dammy_start, 3, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[38] + hiragana_num_of_invalid, exfont_hiragana_font_sample, hiragana_byte_0_start, hiragana_byte_1_start, hiragana_byte_2_start, dammy_start, 3, hiragana_invalid_list, hiragana_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[39], exfont_katakana_font_sample, katakana_byte_0_start, katakana_byte_1_start, katakana_byte_2_start, dammy_start, 3, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[40] + hangul_compatibility_jamo_num_of_invalid, exfont_hangul_compatibility_jamo_font_sample, hangul_compatibility_jamo_byte_0_start, hangul_compatibility_jamo_byte_1_start, hangul_compatibility_jamo_byte_2_start, dammy_start, 3, hangul_compatibility_jamo_invalid_list, hangul_compatibility_jamo_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[41] + yi_syllables_num_of_invalid, exfont_yi_syllables_font_sample, yi_syllables_byte_0_start, yi_syllables_byte_1_start, yi_syllables_byte_2_start, dammy_start, 3, yi_syllables_invalid_list, yi_syllables_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[42] + yi_radicals_num_of_invalid, exfont_yi_radicals_font_sample, yi_radicals_byte_0_start, yi_radicals_byte_1_start, yi_radicals_byte_2_start, dammy_start, 3, yi_radicals_invalid_list, yi_radicals_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[43], exfont_cjk_compatibility_forms_font_sample, cjk_compatibility_forms_byte_0_start, cjk_compatibility_forms_byte_1_start, cjk_compatibility_forms_byte_2_start, dammy_start, 3, dammy_invalid, 0);
	Exfont_init_sample(exfont_font_characters[44] + halfwidth_and_fullwidth_forms_num_of_invalid, exfont_halfwidth_and_fullwidth_forms_font_sample, halfwidth_and_fullwidth_forms_byte_0_start, halfwidth_and_fullwidth_forms_byte_1_start, halfwidth_and_fullwidth_forms_byte_2_start, dammy_start, 3, halfwidth_and_fullwidth_forms_invalid_list, halfwidth_and_fullwidth_forms_num_of_invalid);
	Exfont_init_sample(exfont_font_characters[45], exfont_miscellaneous_symbols_and_pictographs_font_sample, miscellaneous_symbols_and_pictographs_byte_0_start, miscellaneous_symbols_and_pictographs_byte_1_start, miscellaneous_symbols_and_pictographs_byte_2_start, miscellaneous_symbols_and_pictographs_byte_3_start, 4, dammy_invalid, 0);
}

std::string Exfont_query_font_name(int exfont_num)
{
	if (exfont_num >= 0 && exfont_num < EXFONT_NUM_OF_FONT_NAME)
		return exfont_font_name[exfont_num];
	else
		return "";
}

void Exfont_set_msg(int msg_num, std::string msg)
{
	if (msg_num >= 0 && msg_num < EXFONT_NUM_OF_FONT_NAME)
		exfont_font_name[msg_num] = msg;
}

std::string Exfont_text_sort(std::string sorce_part_string[], int max_loop)
{
	int arabic_pos = -1;
	bool arabic_found = false;
	std::string result_string = "";
	for (int i = 0; i < max_loop; i++)
	{
		if (memcmp((void*)sorce_part_string[i].c_str(), (void*)exfont_basic_latin_font_sample[0].c_str(), 0x1) == 0)
			break;

		for (int j = 0; j < 235; j++)
		{
			arabic_found = false;
			if (memcmp((void*)sorce_part_string[i].c_str(), (void*)exfont_arabic_right_to_left_sample[j].c_str(), 0x2) == 0)
			{
				arabic_found = true;
				if (arabic_pos <= -1)
					arabic_pos = result_string.length();

				result_string.insert(arabic_pos, sorce_part_string[i]);
				break;
			}
		}

		if (!arabic_found)
		{
			result_string += sorce_part_string[i];
			arabic_pos = -1;
		}
	}
	return result_string;
}

void Exfont_text_parse(std::string sorce_string, std::string part_string[], int max_loop)
{
	int sorce_string_length = sorce_string.length();
	int std_num = 0;
	int parse_string_length = 0;
	int i = 0;

	char* sorce_string_char = (char*)malloc(sorce_string.length() + 10);

	memset(sorce_string_char, 0x0, sorce_string.length() + 10);
	strcpy(sorce_string_char, (char*)sorce_string.c_str());

	for(int k = 0;k < max_loop; k++)
	{
		parse_string_length = mblen(&sorce_string_char[i], 4);

		if (i >= sorce_string_length)
			break;
		else if (parse_string_length >= 1)
		{
			part_string[std_num] = sorce_string.substr(i, parse_string_length);
			i += parse_string_length;
			std_num++;
		}
		else
			i++;
	}
	part_string[std_num] = "\u0000";
	free(sorce_string_char);
}

void Exfont_draw_external_fonts(std::string string, float texture_x, float texture_y, float texture_size_x, float texture_size_y, float red, float green, float blue, float alpha)
{
	int loop = 0;
	float texture_x_offset = 0;
	float interval_offset = 1.5;
	bool unknown_char = false;
	std::string part_string[1024];
	C2D_ImageTint tint;
	C2D_PlainImageTint(&tint, C2D_Color32f(red, green, blue, alpha), true);

	if (string.length() >= 1024)
		loop = 1024;
	else
		loop = string.length();

	Exfont_text_parse(string, part_string, loop);
	Exfont_text_parse(Exfont_text_sort(part_string, loop), part_string, loop);
	for (int i = 0; i < loop; i++)
	{
		unknown_char = true;

		if (memcmp((void*)part_string[i].c_str(), (void*)exfont_basic_latin_font_sample[0].c_str(), 0x1) == 0)
			break;

		if (part_string[i].length() == 1)
		{
			if (Sem_query_loaded_external_font_flag(0))
			{
				for (int j = 1; j < 128; j++) //basic latin
				{
					if (memcmp((void*)part_string[i].c_str(), (void*)exfont_basic_latin_font_sample[j].c_str(), 0x1) == 0)
					{
						if (j == 10)
						{
							texture_y += 40.0 * texture_size_y;
							texture_x_offset = 0;
							unknown_char = false;
							break;
						}

						if (j <= 32 || j == 127)
							j = 1;
						else
							j -= 31;

						texture_x_offset += (exfont_basic_latin_font_interval[j] + interval_offset) * texture_size_x / 2;
						Draw_texture(exfont_font_images, tint, exfont_font_start_num[0] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
						unknown_char = false;
						texture_x_offset += (exfont_basic_latin_font_interval[j] + interval_offset) * texture_size_x / 2;
						break;
					}
				}
			}
		}
		else if (part_string[i].length() == 2)
		{
			exfont_font_pos = -1;
			for (int k = 0; k < 2; k++)
			{
				if (memcmp((void*)part_string[i].c_str(), (void*)exfont_font_samples[k].c_str(), 0x2) >= 0)
					exfont_font_pos = k;
			}

			if (exfont_font_pos == 0)
			{
				if (Sem_query_loaded_external_font_flag(1))
				{
					for (int j = 0; j < exfont_font_characters[1]; j++) //latin 1 supplement
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_latin_1_supplement_font_sample[j].c_str(), 0x2) == 0)
						{
							texture_x_offset += (exfont_latin_1_supplement_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[1] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_latin_1_supplement_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(2))
				{
					for (int j = 0; j < exfont_font_characters[2]; j++) //ipa extensions
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_ipa_extensions_font_sample[j].c_str(), 0x2) == 0)
						{
							texture_x_offset += (exfont_ipa_extensions_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[2] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_ipa_extensions_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(3))
				{
					for (int j = 0; j < exfont_font_characters[3]; j++) //spacing modifier letters
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_spacing_modifier_letters_font_sample[j].c_str(), 0x2) == 0)
						{
							texture_x_offset += (exfont_spacing_modifier_letters_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[3] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_spacing_modifier_letters_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(4))
				{
					for (int j = 0; j < exfont_font_characters[4]; j++) //combining diacritical marks
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_combining_diacritical_marks_font_sample[j].c_str(), 0x2) == 0)
						{
							texture_x_offset += (exfont_combining_diacritical_marks_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[4] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_combining_diacritical_marks_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(5))
				{
					for (int j = 0; j < exfont_font_characters[5]; j++) //greek and coptic
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_greek_and_coptic_font_sample[j].c_str(), 0x2) == 0)
						{
							texture_x_offset += (exfont_greek_and_coptic_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[5] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_greek_and_coptic_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (exfont_font_pos == 1)
			{
				if (Sem_query_loaded_external_font_flag(6))
				{
					for (int j = 0; j < exfont_font_characters[6]; j++) //cyrillic
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_cyrillic_font_sample[j].c_str(), 0x2) == 0)
						{
							texture_x_offset += (exfont_cyrillic_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[6] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_cyrillic_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(7))
				{
					for (int j = 0; j < 48; j++) //cyrillic supplement
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_cyrillic_supplement_font_sample[j].c_str(), 0x2) == 0)
						{
							texture_x_offset += (exfont_cyrillic_supplement_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[7] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_cyrillic_supplement_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(8))
				{
					for (int j = 0; j < 91; j++) //armenian
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_armenian_sample[j].c_str(), 0x2) == 0)
						{
							texture_x_offset += (exfont_armenian_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[8] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_armenian_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(9))
				{
					for (int j = 0; j < 88; j++) //hebrew
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_hebrew_font_sample[j].c_str(), 0x2) == 0)
						{
							texture_x_offset += (exfont_hebrew_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[9] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_hebrew_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(10))
				{
					for (int j = 0; j < 255; j++) //arabic
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_arabic_sample[j].c_str(), 0x2) == 0)
						{
							texture_x_offset += (exfont_arabic_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[10] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_arabic_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (exfont_font_pos == -1)
				Log_log_save("", "2 bytes : unknown error", 1234567890, false);
		}
		else if (part_string[i].length() == 3)
		{
			exfont_font_pos = -1;
			for (int k = 2; k < 62; k++)
			{
				if (memcmp((void*)part_string[i].c_str(), (void*)exfont_font_samples[k].c_str(), 0x3) >= 0)
					exfont_font_pos = k;
			}

			if (exfont_font_pos == 2)
			{
				if (Sem_query_loaded_external_font_flag(11))
				{
					for (int j = 0; j < 128; j++) //devanagari
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_devanagari_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_devanagari_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[11] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_devanagari_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(12))
				{
					for (int j = 0; j < 79; j++) //gurmukhi
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_gurmukhi_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_gurmukhi_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[12] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_gurmukhi_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(13))
				{
					for (int j = 0; j < 72; j++) //tamil
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_tamil_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_tamil_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[13] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_tamil_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (exfont_font_pos == 3)
			{
				if (Sem_query_loaded_external_font_flag(14))
				{
					for (int j = 0; j < 96; j++) //telugu
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_telugu_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_telugu_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[14] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_telugu_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(15))
				{
					for (int j = 0; j < 88; j++) //kannada
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_kannada_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_kannada_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[15] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_kannada_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(16))
				{
					for (int j = 0; j < 90; j++) //sinhala
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_sinhala_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_sinhala_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[16] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_sinhala_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(17))
				{
					for (int j = 0; j < 87; j++) //thai
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_thai_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_thai_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[17] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_thai_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(18))
				{
					for (int j = 0; j < 67; j++) //lao
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_lao_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_lao_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[18] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_lao_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(19))
				{
					for (int j = 0; j < 211; j++) //tibetan
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_tibetan_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_tibetan_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[19] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_tibetan_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (exfont_font_pos == 4)
			{
				if (Sem_query_loaded_external_font_flag(20))
				{
					for (int j = 0; j < 88; j++) //georgian
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_georgian_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_georgian_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[20] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_georgian_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (exfont_font_pos == 5)
			{
				if (Sem_query_loaded_external_font_flag(21))
				{
					for (int j = 0; j < 640; j++) //unified canadian aboriginal syllabics
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_unified_canadian_aboriginal_syllabics_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_unified_canadian_aboriginal_syllabics_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[21] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_unified_canadian_aboriginal_syllabics_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (exfont_font_pos == 7)
			{
				if (Sem_query_loaded_external_font_flag(22))
				{
					for (int j = 0; j < 128; j++) //phonetic extensions
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_phonetic_extensions_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_phonetic_extensions_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[22] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_phonetic_extensions_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(23))
				{
					for (int j = 0; j < 63; j++) //combining diacritical marks supplement
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_combining_diacritical_marks_supplement_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_combining_diacritical_marks_supplement_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[23] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_combining_diacritical_marks_supplement_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(24))
				{
					for (int j = 0; j < 233; j++) //greek extended
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_greek_extended_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_greek_extended_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[24] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_greek_extended_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (exfont_font_pos == 8)
			{
				if (Sem_query_loaded_external_font_flag(25))
				{
					for (int j = 0; j < 111; j++) //general punctuation
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_general_punctuation_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_general_punctuation_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[25] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_general_punctuation_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(26))
				{
					for (int j = 0; j < 42; j++) //superscripts and subscripts
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_superscripts_and_subscripts_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_superscripts_and_subscripts_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[26] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_superscripts_and_subscripts_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(27))
				{
					for (int j = 0; j < 33; j++) //combining diacritical marks for symbols
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_combining_diacritical_marks_for_symbols_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_combining_diacritical_marks_for_symbols_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[27] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_combining_diacritical_marks_for_symbols_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(28))
				{
					for (int j = 0; j < 112; j++) //arrows
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_arrows_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_arrows_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[28] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_arrows_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(29))
				{
					for (int j = 0; j < 256; j++) //mathematical operators
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_mathematical_operators_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_mathematical_operators_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[29] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_mathematical_operators_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(30))
				{
					for (int j = 0; j < 256; j++) //miscellaneous technical
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_miscellaneous_technical_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_miscellaneous_technical_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[30] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_miscellaneous_technical_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (exfont_font_pos == 9)
			{
				if (Sem_query_loaded_external_font_flag(31))
				{
					for (int j = 0; j < 11; j++) //optical character recognition
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_optical_character_recognition_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_optical_character_recognition_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[31] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_optical_character_recognition_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(32))
				{
					for (int j = 0; j < 128; j++) //box drawing
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_box_drawing_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_box_drawing_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[32] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_box_drawing_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(33))
				{
					for (int j = 0; j < 32; j++) //block elements
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_block_elements_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_block_elements_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[33] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_block_elements_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(34))
				{
					for (int j = 0; j < 96; j++) //geometric shapes
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_geometric_shapes_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_geometric_shapes_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[34] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_geometric_shapes_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(35))
				{
					for (int j = 0; j < 256; j++) //miscellaneous symbols
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_miscellaneous_symbols_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_miscellaneous_symbols_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[35] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_miscellaneous_symbols_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(36))
				{
					for (int j = 0; j < 192; j++) //dingbats
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_dingbats_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_dingbats_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[36] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_dingbats_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (exfont_font_pos == 12)
			{
				if (Sem_query_loaded_external_font_flag(37))
				{
					for (int j = 0; j < 64; j++) //symbol and punctuation
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_cjk_symbol_and_punctuation_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_cjk_symbol_and_punctuation_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[37] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_cjk_symbol_and_punctuation_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(38))
				{
					for (int j = 0; j < 93; j++) //hiragana
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_hiragana_font_sample[j].c_str(), 0x3) == 0)
						{

							texture_x_offset += (exfont_hiragana_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[38] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_hiragana_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(39))
				{
					for (int j = 0; j < 96; j++) //katakana
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_katakana_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_katakana_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[39] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_katakana_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(40))
				{
					for (int j = 0; j < 94; j++) //hangul compatibility jamo
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_hangul_compatibility_jamo_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_hangul_compatibility_jamo_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[40] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_hangul_compatibility_jamo_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (exfont_font_pos == 40 || exfont_font_pos == 41)
			{
				if (Sem_query_loaded_external_font_flag(41))
				{
					for (int j = 0; j < 1165; j++) //yi syllables
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_yi_syllables_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_yi_syllables_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[41] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_yi_syllables_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(42))
				{
					for (int j = 0; j < 55; j++) //yi radicals
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_yi_radicals_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_yi_radicals_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[42] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_yi_radicals_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (exfont_font_pos == 61)
			{
				if (Sem_query_loaded_external_font_flag(43))
				{
					for (int j = 0; j < 32; j++) //cjk compatibility forms
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_cjk_compatibility_forms_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_cjk_compatibility_forms_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[43] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_cjk_compatibility_forms_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (Sem_query_loaded_external_font_flag(44))
				{
					for (int j = 0; j < 225; j++) //halfwidth and fullwidth forms
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)exfont_halfwidth_and_fullwidth_forms_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (exfont_halfwidth_and_fullwidth_forms_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(exfont_font_images, tint, exfont_font_start_num[44] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (exfont_halfwidth_and_fullwidth_forms_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (exfont_font_pos == -1)
				Log_log_save("", "3 bytes : unknown error", 1234567890, false);
		}
		else if (part_string[i].length() == 4)
		{
			if (Sem_query_loaded_external_font_flag(45))
			{
				for (int j = 0; j < 768; j++) //miscellaneous symbols and pictographs
				{
					if (memcmp((void*)part_string[i].c_str(), (void*)exfont_miscellaneous_symbols_and_pictographs_font_sample[j].c_str(), 0x4) == 0)
					{
						texture_x_offset += (exfont_miscellaneous_symbols_and_pictographs_font_interval + interval_offset) * texture_size_x / 2;
						Draw_texture(exfont_font_images, tint, exfont_font_start_num[45] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
						unknown_char = false;
						texture_x_offset += (exfont_miscellaneous_symbols_and_pictographs_font_interval + interval_offset) * texture_size_x / 2;
						break;
					}
				}
			}
		}

		if (unknown_char && Sem_query_loaded_external_font_flag(0))
		{
			texture_x_offset += (25 + interval_offset) * texture_size_x / 2;
			Draw_texture(exfont_font_images, tint, exfont_font_start_num[0] + 0, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
			texture_x_offset += (25 + interval_offset) * texture_size_x / 2;
		}
	}
}

Result_with_string Exfont_load_exfont(int exfont_num)
{
	Result_with_string result;
	if (exfont_num >= 0 && exfont_num <= 45)
	{
		result = Draw_load_texture("romfs:/gfx/font/" + exfont_font_name[exfont_num] + "_font.t3x", exfont_num + 5, exfont_font_images, exfont_font_start_num[exfont_num], exfont_font_characters[exfont_num]);

		if (result.code != 0)
			Exfont_unload_exfont(exfont_num);
	}
	else
	{
		result.code = -1;
		result.string = "Invalid exfont num";
	}

	return result;
}

void Exfont_unload_exfont(int exfont_num)
{
	if (exfont_num >= 0 && exfont_num <= 45)
	{
		Draw_free_texture(5 + exfont_num);
		for (int j = exfont_font_start_num[exfont_num]; j < exfont_font_characters[exfont_num]; j++)
			exfont_font_images[j].tex = NULL;
	}
}
