#include <3ds.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include "citro2d.h"
#include "stb_image.h"

#include "draw.hpp"
#include "file.hpp"
#include "change_setting.hpp"
#include "share_function.hpp"
#include "speedtest.hpp"
#include "line.hpp"
#include "setting_menu.hpp"
#include "google_translation.hpp"

#define STACKSIZE (24 * 1024)


//Debug cache
bool share_wifi_connected = false;
bool debug_unknown_char = false;

u32 wifi_connection_succes;

float texture_size_x = 1.0;
float texture_size_y = 1.0;
float debug_string_x = 0.0;
float debug_string_y = 0.0;
float debug_legacy_string_x = 0.0;
float debug_legacy_string_y = 0.0;
int debug_string_length = 0;
int debug_parse_string_length = 0;
int debug_sample_num = 0;

char* debug_string_char;
std::string debug_string;
std::string debug_part_string[8192];


//#0020~#007F Invalid #007F
std::string share_font_english_sample[96] = { "\u0000",
"\u0020", "\u0021", "\u0022", "\u0023", "\u0024", "\u0025", "\u0026", "\u0027", "\u0028", "\u0029", "\u002A", "\u002B", "\u002C", "\u002D", "\u002E", "\u002F",
"\u0030", "\u0031", "\u0032", "\u0033", "\u0034", "\u0035", "\u0036", "\u0037", "\u0038", "\u0039", "\u003A", "\u003B", "\u003C", "\u003D", "\u003E", "\u003F",
"\u0040", "\u0041", "\u0042", "\u0043", "\u0044", "\u0045", "\u0046", "\u0047", "\u0048", "\u0049", "\u004A", "\u004B", "\u004C", "\u004D", "\u004E", "\u004F",
"\u0050", "\u0051", "\u0052", "\u0053", "\u0054", "\u0055", "\u0056", "\u0057", "\u0058", "\u0059", "\u005A", "\u005B", "\u005C", "\u005D", "\u005E", "\u005F",
"\u0060", "\u0061", "\u0062", "\u0063", "\u0064", "\u0065", "\u0066", "\u0067", "\u0068", "\u0069", "\u006A", "\u006B", "\u006C", "\u006D", "\u006E", "\u006F",
"\u0070", "\u0071", "\u0072", "\u0073", "\u0074", "\u0075", "\u0076", "\u0077", "\u0078", "\u0079", "\u007A", "\u007B", "\u007C", "\u007D", "\u007E"  /*"\u007F"*/ };


float share_english_font_interval[96] = {100.0,
 20.0,  7.0, 20.0, 38.0, 34.0, 54.0, 44.0,  7.0, 17.0, 17.0, 23.0, 34.0,  7.0, 20.0,  7.0, 20.0,
 34.0, 19.0, 34.0, 34.0, 35.0, 34.0, 34.0, 34.0, 34.0, 34.0,  7.0,  7.0, 34.0, 34.0, 34.0, 34.0, 
 67.0, 50.0, 39.0, 45.0, 43.0, 38.0, 35.0, 47.0, 40.0,  6.0, 28.0, 43.0, 32.0, 48.0, 40.0, 50.0, 
 39.0, 51.0, 45.0, 41.0, 40.0, 40.0, 47.0, 66.0, 48.0, 47.0, 41.0, 14.0, 20.0, 14.0, 30.0, 36.0, 
 13.0, 34.0, 32.0, 33.0, 32.0, 34.0, 22.0, 32.0, 30.0,  6.0, 14.0, 31.0,  6.0, 50.0, 30.0, 34.0, 
 32.0, 32.0, 20.0, 31.0, 19.0, 30.0, 34.0, 53.0, 35.0, 35.0, 33.0, 20.0,  6.0, 20.0, 36.0/*007F*/};

//#0600~#06FF Invalid #061D
float share_arabic_font_interval[255] = {
 63.0, 90.0, 40.0, 88.0, 89.0, 72.0, 44.0, 44.0, 51.0, 45.0, 61.0, 21.0,  7.0, 15.0, 49.0, 36.0,
 18.0, 14.0, 21.0, 24.0, 23.0, 18.0, 27.0, 14.0, 10.0,  9.0, 10.0,  7.0, 72.0,/*061D*/19.0,33.0,
 42.0, 33.0, 21.0, 10.0, 38.0, 10.0, 48.0,  5.0, 48.0, 29.0, 48.0, 48.0, 37.0, 37.0, 37.0, 24.0,
 24.0, 31.0, 31.0, 77.0, 77.0, 86.0, 86.0, 52.0, 52.0, 37.0, 37.0, 57.0, 57.0, 42.0, 42.0, 42.0,
 21.0, 61.0, 52.0, 47.0, 42.0, 34.0, 42.0, 29.0, 38.0, 48.0, 48.0, 20.0, 20.0, 20.0, 20.0, 20.0,
 20.0, 21.0, 14.0, 22.0, 16.0, 16.0,  4.0, 22.0,  9.0, 14.0, 20.0, 11.0,  8.0, 12.0, 13.0, 13.0,
 16.0, 12.0, 29.0, 31.0, 28.0, 29.0, 31.0, 31.0, 31.0, 31.0, 34.0, 15.0,  7.0, 28.0, 51.0, 39.0,
  3.0, 20.0, 17.0, 17.0, 10.0, 21.0, 48.0, 49.0, 54.0, 48.0, 48.0, 48.0, 48.0, 48.0, 48.0, 48.0,
 48.0, 37.0, 37.0, 37.0, 37.0, 37.0, 37.0, 37.0, 24.0, 24.0, 24.0, 24.0, 24.0, 24.0, 24.0, 24.0,
 24.0, 31.0, 31.0, 31.0, 33.0, 37.0, 33.0, 31.0, 31.0, 31.0, 77.0, 77.0, 77.0, 86.0, 86.0, 52.0,
 37.0, 61.0, 61.0, 61.0, 61.0, 61.0, 61.0, 52.0, 52.0, 56.0, 68.0, 56.0, 47.0, 47.0, 47.0, 56.0,
 56.0, 56.0, 56.0, 56.0, 56.0, 42.0, 42.0, 42.0, 33.0, 38.0, 42.0, 42.0, 42.0, 42.0, 45.0, 32.0,
 29.0, 29.0, 29.0, 29.0, 38.0, 38.0, 38.0, 38.0, 38.0, 38.0, 38.0, 38.0, 48.0, 58.0, 48.0, 31.0,
 48.0, 48.0, 45.0, 45.0, 20.0, 29.0, 48.0, 36.0, 18.0, 21.0, 23.0, 23.0, 47.0, 41.0, 61.0,  9.0,
  9.0, 25.0, 21.0, 47.0, 13.0, 22.0, 26.0, 26.0, 24.0, 31.0, 11.0, 11.0, 11.0, 21.0, 21.0, 32.0,
 16.0, 12.0, 29.0, 31.0, 31.0, 31.0, 23.0, 31.0, 31.0, 31.0, 64.0, 70.0, 29.0, 16.0, 21.0, 29.0};

//#0600~#06FF Invalid #061D
std::string debug_arabic_sample[255] = {
"\u0600", "\u0601", "\u0602", "\u0603", "\u0604", "\u0605", "\u0606", "\u0607", "\u0608", "\u0609", "\u060A", "\u060B", "\u060C", "\u060D", "\u060E", "\u060F",
"\u0610", "\u0611", "\u0612", "\u0613", "\u0614", "\u0615", "\u0616", "\u0617", "\u0618", "\u0619", "\u061A", "\u061B", "\u061C", /*"\u061D"*/ "\u061E", "\u061F", 
"\u0620", "\u0621", "\u0622", "\u0623", "\u0624", "\u0625", "\u0626", "\u0627", "\u0628", "\u0629", "\u062A", "\u062B", "\u062C", "\u062D", "\u062E", "\u062F",
"\u0630", "\u0631", "\u0632", "\u0633", "\u0634", "\u0635", "\u0636", "\u0637", "\u0638", "\u0639", "\u063A", "\u063B", "\u063C", "\u063D", "\u063E", "\u063F",
"\u0640", "\u0641", "\u0642", "\u0643", "\u0644", "\u0645", "\u0646", "\u0647", "\u0648", "\u0649", "\u064A", "\u064B", "\u064C", "\u064D", "\u064E", "\u064F",
"\u0650", "\u0651", "\u0652", "\u0653", "\u0654", "\u0655", "\u0656", "\u0657", "\u0658", "\u0659", "\u065A", "\u065B", "\u065C", "\u065D", "\u065E", "\u065F",
"\u0660", "\u0661", "\u0662", "\u0663", "\u0664", "\u0665", "\u0666", "\u0667", "\u0668", "\u0669", "\u066A", "\u066B", "\u066C", "\u066D", "\u066E", "\u066F",
"\u0670", "\u0671", "\u0672", "\u0673", "\u0674", "\u0675", "\u0676", "\u0677", "\u0678", "\u0679", "\u067A", "\u067B", "\u067C", "\u067D", "\u067E", "\u067F",
"\u0680", "\u0681", "\u0682", "\u0683", "\u0684", "\u0685", "\u0686", "\u0687", "\u0688", "\u0689", "\u068A", "\u068B", "\u068C", "\u068D", "\u068E", "\u068F",
"\u0690", "\u0691", "\u0692", "\u0693", "\u0694", "\u0695", "\u0696", "\u0697", "\u0698", "\u0699", "\u069A", "\u069B", "\u069C", "\u069D", "\u069E", "\u069F",
"\u06A0", "\u06A1", "\u06A2", "\u06A3", "\u06A4", "\u06A5", "\u06A6", "\u06A7", "\u06A8", "\u06A9", "\u06AA", "\u06AB", "\u06AC", "\u06AD", "\u06AE", "\u06AF",
"\u06B0", "\u06B1", "\u06B2", "\u06B3", "\u06B4", "\u06B5", "\u06B6", "\u06B7", "\u06B8", "\u06B9", "\u06BA", "\u06BB", "\u06BC", "\u06BD", "\u06BE", "\u06BF",
"\u06C0", "\u06C1", "\u06C2", "\u06C3", "\u06C4", "\u06C5", "\u06C6", "\u06C7", "\u06C8", "\u06C9", "\u06CA", "\u06CB", "\u06CC", "\u06CD", "\u06CE", "\u06CF",
"\u06D0", "\u06D1", "\u06D2", "\u06D3", "\u06D4", "\u06D5", "\u06D6", "\u06D7", "\u06D8", "\u06D9", "\u06DA", "\u06DB", "\u06DC", "\u06DD", "\u06DE", "\u06DF",
"\u06E0", "\u06E1", "\u06E2", "\u06E3", "\u06E4", "\u06E5", "\u06E6", "\u06E7", "\u06E8", "\u06E9", "\u06EA", "\u06EB", "\u06EC", "\u06ED", "\u06EE", "\u06EF",
"\u06F0", "\u06F1", "\u06F2", "\u06F3", "\u06F4", "\u06F5", "\u06F6", "\u06F7", "\u06F8", "\u06F9", "\u06fA", "\u06FB", "\u06FC", "\u06FD", "\u06FE", "\u06FF"};

std::string share_arabic_right_to_left_sample[235] = {
"\u0600", "\u0601", "\u0602", "\u0603", "\u0604", "\u0605", "\u0606", "\u0607", "\u0608", "\u0609", "\u060A", "\u060B", "\u060C", "\u060D", "\u060E", "\u060F",
"\u0610", "\u0611", "\u0612", "\u0613", "\u0614", "\u0615", "\u0616", "\u0617", "\u0618", "\u0619", "\u061A", "\u061B", "\u061C", /*"\u061D"*/ "\u061E", "\u061F",
"\u0620", "\u0621", "\u0622", "\u0623", "\u0624", "\u0625", "\u0626", "\u0627", "\u0628", "\u0629", "\u062A", "\u062B", "\u062C", "\u062D", "\u062E", "\u062F",
"\u0630", "\u0631", "\u0632", "\u0633", "\u0634", "\u0635", "\u0636", "\u0637", "\u0638", "\u0639", "\u063A", "\u063B", "\u063C", "\u063D", "\u063E", "\u063F",
"\u0640", "\u0641", "\u0642", "\u0643", "\u0644", "\u0645", "\u0646", "\u0647", "\u0648", "\u0649", "\u064A", "\u064B", "\u064C", "\u064D", "\u064E", "\u064F",
"\u0650", "\u0651", "\u0652", "\u0653", "\u0654", "\u0655", "\u0656", "\u0657", "\u0658", "\u0659", "\u065A", "\u065B", "\u065C", "\u065D", "\u065E", "\u065F",
"\u066A", "\u066B", "\u066C", "\u066D", "\u066E", "\u066F",
"\u0670", "\u0671", "\u0672", "\u0673", "\u0674", "\u0675", "\u0676", "\u0677", "\u0678", "\u0679", "\u067A", "\u067B", "\u067C", "\u067D", "\u067E", "\u067F",
"\u0680", "\u0681", "\u0682", "\u0683", "\u0684", "\u0685", "\u0686", "\u0687", "\u0688", "\u0689", "\u068A", "\u068B", "\u068C", "\u068D", "\u068E", "\u068F",
"\u0690", "\u0691", "\u0692", "\u0693", "\u0694", "\u0695", "\u0696", "\u0697", "\u0698", "\u0699", "\u069A", "\u069B", "\u069C", "\u069D", "\u069E", "\u069F",
"\u06A0", "\u06A1", "\u06A2", "\u06A3", "\u06A4", "\u06A5", "\u06A6", "\u06A7", "\u06A8", "\u06A9", "\u06AA", "\u06AB", "\u06AC", "\u06AD", "\u06AE", "\u06AF",
"\u06B0", "\u06B1", "\u06B2", "\u06B3", "\u06B4", "\u06B5", "\u06B6", "\u06B7", "\u06B8", "\u06B9", "\u06BA", "\u06BB", "\u06BC", "\u06BD", "\u06BE", "\u06BF",
"\u06C0", "\u06C1", "\u06C2", "\u06C3", "\u06C4", "\u06C5", "\u06C6", "\u06C7", "\u06C8", "\u06C9", "\u06CA", "\u06CB", "\u06CC", "\u06CD", "\u06CE", "\u06CF",
"\u06D0", "\u06D1", "\u06D2", "\u06D3", "\u06D4", "\u06D5", "\u06D6", "\u06D7", "\u06D8", "\u06D9", "\u06DA", "\u06DB", "\u06DC", "\u06DD", "\u06DE", "\u06DF",
"\u06E0", "\u06E1", "\u06E2", "\u06E3", "\u06E4", "\u06E5", "\u06E6", "\u06E7", "\u06E8", "\u06E9", "\u06EA", "\u06EB", "\u06EC", "\u06ED", "\u06EE", "\u06EF",
"\u06fA", "\u06FB", "\u06FC", "\u06FD", "\u06FE", "\u06FF" };
//#0531~#058F Invalid #0557,#0558,#058B,#058C
float share_armenian_font_interval[92] = {100.0,
      43.0, 28.0, 39.0, 39.0, 31.0, 32.0, 28.0, 28.0, 45.0, 41.0, 34.0, 24.0, 37.0, 46.0, 31.0,
28.0, 35.0, 38.0, 30.0, 43.0, 38.0, 41.0, 42.0, 27.0, 30.0, 38.0, 32.0, 39.0, 31.0, 45.0, 28.0,
26.0, 29.0, 34.0, 36.0, 45.0, 33.0, 46.0,/*0557,0558*/ 9.0,  9.0,  6.0, 19.0,  9.0, 18.0, 25.0,
60.0, 39.0, 26.0, 26.0, 29.0, 27.0, 26.0, 26.0, 27.0, 28.0, 28.0, 27.0, 19.0, 31.0, 26.0, 27.0,
27.0, 26.0, 27.0, 28.0, 27.0, 13.0, 23.0, 23.0, 27.0, 17.0, 39.0, 26.0, 29.0, 27.0, 27.0, 39.0,
27.0, 26.0, 19.0, 39.0, 21.0, 20.0, 32.0, 33.0, 46.0,  9.0, 33.0,/*058B,058C*/42.0, 42.0, 37.0};

//#0531~#058F Invalid #0557,#0558,#058B,#058C
std::string debug_armenian_sample[92] = {"\u0000",
"\u0531", "\u0532", "\u0533", "\u0534", "\u0535", "\u0536", "\u0537", "\u0538", "\u0539", "\u053A", "\u053B", "\u053C", "\u053D", "\u053E", "\u053F",
"\u0540", "\u0541", "\u0542", "\u0543", "\u0544", "\u0545", "\u0546", "\u0547", "\u0548", "\u0549", "\u054A", "\u054B", "\u054C", "\u054D", "\u054E", "\u054F",
"\u0550", "\u0551", "\u0552", "\u0553", "\u0554", "\u0555", "\u0556", /*u0557     0558 */ "\u0559", "\u055A", "\u055B", "\u055C", "\u055D", "\u055E", "\u055F",
"\u0560", "\u0561", "\u0562", "\u0563", "\u0564", "\u0565", "\u0566", "\u0567", "\u0568", "\u0569", "\u056A", "\u056B", "\u056C", "\u056D", "\u056E", "\u056F",
"\u0570", "\u0571", "\u0572", "\u0573", "\u0574", "\u0575", "\u0576", "\u0577", "\u0578", "\u0579", "\u057A", "\u057B", "\u057C", "\u057D", "\u057E", "\u057F",
"\u0580", "\u0581", "\u0582", "\u0583", "\u0584", "\u0585", "\u0586", "\u0587", "\u0588", "\u0589", "\u058A", /*"\u058B" "\u058C"*/ "\u058D", "\u058E", "\u058F"};

float share_pe_font_interval[73] = {
 17.0, 14.0, 20.0, 87.0, 64.0, 52.0, 14.0, 12.0, 12.0, 17.0, 17.0, 19.0, 21.0, 25.0, 24.0, 32.0,
 27.0, 17.0, 22.0, 27.0, 28.0, 23.0, 39.0, 20.0, 16.0, 33.0, 21.0, 20.0, 36.0, 23.0, 23.0, 23.0,
 36.0, 46.0, 46.0, 16.0, 23.0, 41.0, 23.0, 17.0, 17.0, 37.0, 30.0, 23.0, 32.0, 35.0, 27.0, 20.0,
 20.0, 26.0, 27.0, 20.0, 26.0, 26.0, 27.0, 72.0, 65.0, 66.0, 66.0, 65.0, 65.0, 65.0, 66.0, 65.0,
 62.0, 10.0, 12.0, 10.0, 17.0, 14.0, 20.0, 28.0, 25.0};

std::string share_font_pe_sample[73] = {
"\u0029", "\u005D", "\u007D", "\u0F3B", "\u0F3D", "\u169C", "\u2046", "\u207E", "\u208E", "\u2309", "\u230B", "\u232A", "\u2769", "\u276B", "\u276D", "\u276F",
"\u2771", "\u2773", "\u2775", "\u27C6", "\u27E7", "\u27E9", "\u27EB", "\u27ED", "\u27EF", "\u2984", "\u2986", "\u2988", "\u298A", "\u298C", "\u298E", "\u2990",
"\u2992", "\u2994", "\u2996", "\u2998", "\u29D9", "\u29DB", "\u29FD", "\u2E23", "\u2E25", "\u2E27", "\u2E29", "\u3009", "\u300B", "\u300D", "\u300F", "\u3011",
"\u3015", "\u3017", "\u3019", "\u301B", "\u301E", "\u301F", "\uFD3E", "\uFE18", "\uFE36", "\uFE38", "\uFE3A", "\uFE3C", "\uFE3E", "\uFE40", "\uFE42", "\uFE44",
"\uFE48", "\uFE5A", "\uFE5C", "\uFE5E", "\uFF09", "\uFF3D", "\uFF5D", "\uFF60", "\uFF63"};


//Share

bool share_setting_load_failed = false;

int share_app_log_num_return = 0;
int share_system_setting_menu_show_time_left = 20;
int share_setting_parse_start_num = 0;
int share_setting_parse_end_num = 0;

std::string share_setting_parse_text;

Handle Share_fs_handle, Share_log_fs_handle;
FS_Archive Share_fs_archive, Share_log_fs_archive;

//Main menu
std::string main_menu_menu_string[10] = {"\nLine", "  Google \ntranslation", "    Speed test"};


Thread bgm_thread, hid_thread, share_update_thread, share_send_app_info_thread;
Result function_result;

Handle music_h;
FS_Archive music_fs_a;
/**/

s32 function_test_as_root(void)
{
	//memset((void*)0x1FF81066, 0x0, 0x4);

	//memset((void*)0x1FF81067, 0x3, 0x1);

	//memset(ADDRESS, 0xFF, 0x38400);
	//memset((void*)0x1E6000, 0xFF, 0x38400);
	//*(vu32*)0x1FF81067 = 0x3;

	return 0;
}

void Share_function_test(void)
{
	//svcBackdoor(function_test_as_root);

	int current_text_x_pos = 0;
	for (int i = 0; i <= 1023; i++)
	{
		debug_unknown_char = true;

		if (memcmp((void*)debug_part_string[i].c_str(), (void*)share_font_english_sample[0].c_str(), 0x2) == 0)
		{
			break;
		}

		if (debug_part_string[i].length() == 1) //en
		{
			for (int j = 1; j < 96; j++)
			{
				if (memcmp((void*)debug_part_string[i].c_str(), (void*)share_font_english_sample[j].c_str(), 0x1) == 0)
				{
					current_text_x_pos += (share_english_font_interval[j] + 10) * texture_size_x / 2;
					Draw_texture(English_font_image, j, debug_string_x + current_text_x_pos, debug_string_y + 150.0, 100 * texture_size_x, 100 * texture_size_y);
					debug_unknown_char = false;
					current_text_x_pos += (share_english_font_interval[j] + 10) * texture_size_x / 2;
					break;
				}
			}
		}
		else if (debug_part_string[i].length() == 3)
		{
			for (int j = 1; j < 93; j++) //pe
			{
				if (memcmp((void*)debug_part_string[i].c_str(), (void*)share_font_pe_sample[j].c_str(), 0x3) == 0)
				{
					current_text_x_pos += (share_pe_font_interval[j] + 10) * texture_size_x / 2;
					Draw_texture(Punctuation_close_font_image, j, debug_string_x + current_text_x_pos, debug_string_y + 150.0, 100 * texture_size_x, 100 * texture_size_y);
					debug_unknown_char = false;
					current_text_x_pos += (share_pe_font_interval[j] + 10) * texture_size_x / 2;
					break;
				}
			}
		}
		else if (debug_part_string[i].length() == 2)
		{
			for (int j = 0; j < 255; j++) //arabic
			{
				if (memcmp((void*)debug_part_string[i].c_str(), (void*)debug_arabic_sample[j].c_str(), 0x2) == 0)
				{
					current_text_x_pos += (share_arabic_font_interval[j] + 10) * texture_size_x / 2;
					Draw_texture(Arabic_font_image, j, debug_string_x + current_text_x_pos, debug_string_y + 150.0, 100 * texture_size_x, 100 * texture_size_y);
					debug_unknown_char = false;
					current_text_x_pos += (share_arabic_font_interval[j] + 10) * texture_size_x / 2;
					break;
				}
			}

			for (int j = 1; j < 92; j++) //armenian
			{
				if (memcmp((void*)debug_part_string[i].c_str(), (void*)debug_armenian_sample[j].c_str(), 0x2) == 0)
				{
					current_text_x_pos += (share_armenian_font_interval[j] + 10) * texture_size_x / 2;
					Draw_texture(Armenian_font_image, j, debug_string_x + current_text_x_pos, debug_string_y + 150.0, 100 * texture_size_x, 100 * texture_size_y);
					debug_unknown_char = false;
					current_text_x_pos += (share_armenian_font_interval[j] + 10) * texture_size_x / 2;
					break;
				}
			}
		}

		if (debug_unknown_char)
		{
			current_text_x_pos += (100 + 10) * texture_size_x / 2;
			Draw_texture(English_font_image, 0, debug_string_x + current_text_x_pos, debug_string_y + 150.0, 100 * texture_size_x, 100 * texture_size_y);
			current_text_x_pos += (100 + 10) * texture_size_x / 2;
		}
	}

}

std::string Share_text_sort(std::string sorce_part_string[8192])
{
	int arabic_pos = -1;
	bool arabic_found = false;
	std::string result_string = "";
	for (int i = 0; i <= 8191; i++)
	{
		for (int j = 0; j < 235; j++)
		{
			arabic_found = false;
			if (memcmp((void*)sorce_part_string[i].c_str(), (void*)share_arabic_right_to_left_sample[j].c_str(), 0x2) == 0)//arabic文字検出時
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

void Share_text_parse(std::string sorce_string, std::string part_string[8192])
{
	char* sorce_string_char = (char*)malloc(0x10000);
	int sorce_string_length = sorce_string.length();
	int i = 0;
	int std_num = 0;
	int parse_string_length = 0;

	memset(sorce_string_char, 0x0, 0x10000);
	memcpy(sorce_string_char, (void*)sorce_string.c_str(), sorce_string_length);
	for (int i = 0; i <= 8191; i++)
		part_string[i] = ""; 

	while (true)
	{
		parse_string_length = mblen(&sorce_string_char[i], 16);

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
}

void Init(void)
{
	int init_log_num_return;
	Result_with_string init_result;
	init_result.code = 0;
	init_result.string = "[Success] ";
	osTickCounterStart(&share_tick_counter_up_time);
	Share_app_log_save("Main/Init", "Initializing...", 1234567890, false);
	Share_app_log_save("Main/Init/ver", share_app_ver, 1234567890, false);

	osSetSpeedupEnable(true);
	gfxInitDefault();

	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);

	Draw_init();
	Draw_set_draw_mode(1);
	Draw_screen_ready_to_draw(0, true, 0);
	Draw_screen_ready_to_draw(1, true, 0);

	osTickCounterStart(&share_total_frame_time);

	//Init svc
	init_log_num_return = Share_app_log_save("Main/Init", "fsInit...", 1234567890, true);
	init_result.code = fsInit();
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, true);
		share_fs_success = true;
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, true);
		

	init_log_num_return = Share_app_log_save("Main/Init", "acInit...", 1234567890, true);
	init_result.code = acInit();
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, true);
		share_ac_success = true;
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, true);


	init_log_num_return = Share_app_log_save("Main/Init", "aptInit...", 1234567890, true);
	init_result.code = aptInit();
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, true);
		share_apt_success = true;
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, true);

	
	init_log_num_return = Share_app_log_save("Main/Init", "mcuInit...", 1234567890, true);
	init_result.code = mcuHwcInit();
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, true);
		share_mcu_success = true;
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, true);


	init_log_num_return = Share_app_log_save("Main/Init", "ptmuInit...", 1234567890, true);
	init_result.code = ptmuInit();
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, true);
		share_ptmu_success = true;
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, true);

	init_log_num_return = Share_app_log_save("Main/Init", "httpcInit...", 1234567890, true);
	init_result.code = httpcInit(0x500000);
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, true);
		share_httpc_success = true;
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, true);


	init_log_num_return = Share_app_log_save("Main/Init", "romfsInit...", 1234567890, true);
	init_result.code = romfsInit();
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, true);
		share_rom_success = true;
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, true);


	init_log_num_return = Share_app_log_save("Main/Init", "cfguInit...", 1234567890, true);
	init_result.code = cfguInit();
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, true);
		share_cfg_success = true;
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, true);

	init_log_num_return = Share_app_log_save("Main/Init/apt", "APT_SetAppCpuTimeLimit...", 1234567890, true);
	init_result.code = APT_SetAppCpuTimeLimit(30);
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, true);
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, true);

	aptSetSleepAllowed(true);

	share_connected_ssid = (char*)malloc(0x200);

	init_log_num_return = Share_app_log_save("Main/Init/nwm", "Wifi_enable...", 1234567890, true);
	init_result.code = Wifi_enable();
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, true);
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, true); 
	share_wifi_enabled = true;

	init_log_num_return = Share_app_log_save("Main/Init/fs", "Share_load_from_file...", 1234567890, true);
	init_result = Share_load_from_file("Setting.txt", "/Line/", Share_fs_handle, Share_fs_archive);
	if (init_result.code == 0)
	{
		share_setting[0] = init_result.string;
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, true);
	}
	else
		Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, true);

	for (int i = 0; i <= 7; i++)
	{
		init_log_num_return = Share_app_log_save("Main/Init/fs", "setting value" + std::to_string(i) + " : ", 1234567890, false);
		share_setting_parse_text = "<" + std::to_string(i);
		share_setting_parse_start_num = share_setting[0].find(share_setting_parse_text);
		share_setting_parse_text = std::to_string(i) + ">";
		share_setting_parse_end_num = share_setting[0].find(share_setting_parse_text);

		if (share_setting_parse_end_num == -1 || share_setting_parse_start_num == -1)
		{
			Share_app_log_save("Main/Init/fs", "Failed to load settings. Default values has been applied.", 1234567890, false);
			share_setting[1] = "en";
			share_setting[2] = "100";
			share_setting[3] = "1500";
			share_setting[4] = "10";
			share_setting[5] = "false";
			share_setting[6] = "0.5";
			share_setting[7] = "deny";
			share_setting[8] = "0";
			share_setting_load_failed = true;
			break;
		}
		share_setting_parse_end_num -= share_setting_parse_start_num;
		share_setting_parse_start_num += share_setting_parse_text.length();
		share_setting[i + 1] = share_setting[0].substr(share_setting_parse_start_num, share_setting_parse_end_num - share_setting_parse_text.length());
		Share_app_log_add_result(init_log_num_return, share_setting[i + 1], 1234567890, true);
	}

	if (!share_setting_load_failed)
	{
		share_lcd_brightness = stoi(share_setting[2]);
		share_time_to_enter_afk = stoi(share_setting[3]);
		share_afk_lcd_brightness = stoi(share_setting[4]);
		if (share_setting[5] == "true")
			share_system_setting_menu_show = true;
		else
			share_system_setting_menu_show = false;

		share_scroll_speed = stod(share_setting[6]);
		if(share_setting[7] == "allow")
			share_allow_send_app_info = true;
		else 
			share_allow_send_app_info = false;

		share_num_of_app_start = stoi(share_setting[8]);
	}

	share_update_thread_run = true;
	share_update_thread = threadCreate(Share_update_thread, (void*)(""), STACKSIZE, 0x24, -1, true);
	share_hid_thread_run = true;
	hid_thread = threadCreate(Share_scan_hid, (void*)(""), STACKSIZE, 0x20, -1, true);
	share_connect_test_thread_run = true;
	share_connect_test_thread = threadCreate(Share_connectivity_check_thread, (void*)(""), STACKSIZE, 0x30, -1, true);

	init_log_num_return = Share_app_log_save("Main/Init/c2d", "Loading texture (background.t3x)...", 1234567890, true);
	init_result = Draw_load_texture("romfs:/gfx/background.t3x", Background_texture, Background_image, 2);
	Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, true);

	init_log_num_return = Share_app_log_save("Main/Init/c2d", "Loading texture (wifi_signal.t3x)...", 1234567890, true);
	init_result = Draw_load_texture("romfs:/gfx/wifi_signal.t3x", Wifi_icon_texture, Wifi_icon_image, 9);
	Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, true);

	init_log_num_return = Share_app_log_save("Main/Init/c2d", "Loading texture (battery_level.t3x)...", 1234567890, true);
	init_result = Draw_load_texture("romfs:/gfx/battery_level.t3x", Battery_level_icon_texture, Battery_level_icon_image, 21);
	Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, true);

	init_log_num_return = Share_app_log_save("Main/Init/c2d", "Loading texture (battery_charge.t3x)...", 1234567890, true);
	init_result = Draw_load_texture("romfs:/gfx/battery_charge.t3x", Battery_charge_icon_texture, Battery_charge_icon_image, 1);
	Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, true);

	init_log_num_return = Share_app_log_save("Main/Init/c2d", "Loading texture (bar.t3x)...", 1234567890, true);
	init_result = Draw_load_texture("romfs:/gfx/bar.t3x", Setting_bar_texture, Setting_bar_image, 1);
	Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, true);

	/*
	Chinese_font_texture = C2D_SpriteSheetLoad("romfs:/gfx/chinese_font.t3x");
	Arabic_font_texture[0] = C2D_SpriteSheetLoad("romfs:/gfx/arabic_font_0.t3x");
	Arabic_font_texture[1] = C2D_SpriteSheetLoad("romfs:/gfx/arabic_font_1.t3x");
	Arabic_font_texture[2] = C2D_SpriteSheetLoad("romfs:/gfx/arabic_font_2.t3x");
	Armenian_font_texture = C2D_SpriteSheetLoad("romfs:/gfx/armenian_font.t3x");
	English_font_texture = C2D_SpriteSheetLoad("romfs:/gfx/english_font.t3x");
	Punctuation_close_font_texture = C2D_SpriteSheetLoad("romfs:/gfx/pe_font.t3x");
	*/

	wifi_state = (u8*)malloc(0x1);
	memset(wifi_state, 0xff, 0x1);
	wifi_state_internet_sample = (u8*)malloc(0x1);
	memset(wifi_state_internet_sample, 0x2, 0x1);
	debug_string_char = (char*)malloc(0x2048);
	memset(debug_string_char, 0x0, 0x2048);

	if (share_allow_send_app_info)
	{
		for (int i = 1; i <= 1000; i++)
		{
			if (share_num_of_app_start == i * 10)
			{
				share_send_app_info_thread = threadCreate(Share_send_app_info, (void*)(""), STACKSIZE, 0x30, -1, true);
				break;
			}
		}
	}

	Share_app_log_save("Main/Init", "Initialized.", 1234567890, false);
}

int main()
{
	Init();

	// Main loop
	while (aptMainLoop())
	{
		hid_disabled = false;
		share_app_log_x = app_log_x_cache;
		share_app_log_view_num = share_app_log_view_num_cache;
		Share_get_system_info();
		sprintf(share_status, "%dfps %.1fms %02d/%02d %02d:%02d:%02d "
			, share_fps_show, share_frame_time_point[0], share_month, share_day, share_hours, share_minutes, share_seconds);
		
		if (share_menu_main_run)
		{
			Draw_set_draw_mode(1);
			Draw_screen_ready_to_draw(0, true, 1);			

			Draw_texture(Background_image, 0, 0.0, 0.0, 400.0, 15.0);
			Draw_texture(Wifi_icon_image, share_wifi_signal, 360.0, 0.0, 15.0, 15.0);
			Draw_texture(Battery_level_icon_image, share_battery_level / 5, 330.0, 0.0, 30.0, 15.0);
			if(share_battery_charge)
				Draw_texture(Battery_charge_icon_image, 0, 310.0, 0.0, 20.0, 15.0);
			Draw(share_status, 0.0f, 0.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 1.0f);
			Draw(share_battery_level_string, 337.5f, 1.25f, 0.4f, 0.4f, 0.0f, 0.0f, 0.0f, 0.5f);

			/*Draw("Key A press : " + std::to_string(share_key_A_press) + " Key A held : " + std::to_string(share_key_A_held), 0.0, 50.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
			Draw("Key B press : " + std::to_string(share_key_B_press) + " Key B held : " + std::to_string(share_key_B_held), 0.0, 60.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
			Draw("Key X press : " + std::to_string(share_key_X_press) + " Key X held : " + std::to_string(share_key_X_held), 0.0, 70.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
			Draw("Key Y press : " + std::to_string(share_key_Y_press) + " Key Y held : " + std::to_string(share_key_Y_held), 0.0, 80.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
			Draw("Key CPAD DOWN held : " + std::to_string(share_key_CPAD_DOWN_held) + " Key CPAD UP held : " + std::to_string(share_key_CPAD_UP_held), 0.0, 90.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
			Draw("Key CPAD RIGHT held : " + std::to_string(share_key_CPAD_RIGHT_held) + " Key CPAD LEFT held : " + std::to_string(share_key_CPAD_LEFT_held), 0.0, 100.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
			Draw("Touch pos x : " + std::to_string(share_touch_pos_x) + " Touch pos y : " + std::to_string(share_touch_pos_y), 0.0, 110.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
			Draw("X moved value : " + std::to_string(share_touch_pos_x_moved) + " Y moved value : " + std::to_string(share_touch_pos_y_moved), 0.0, 120.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
			Draw("Held time : " + std::to_string(share_held_time), 0.0, 130.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
			Draw("Drawing time : " + std::to_string(C3D_GetProcessingTime()) + "ms", 0.0, 160.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
			*/

			if (share_app_logs_show)
			{
				int y_i = 0;
				for (int i = share_app_log_view_num; i < share_app_log_view_num + 23; i++)
				{
					Draw(share_app_logs[i], share_app_log_x, 10.0f + (y_i * 10), 0.4f, 0.4f, 0.0f, 0.5f, 1.0f, 1.0f);
					y_i++;
				}
			}

			Draw_screen_ready_to_draw(1, true, 1);
			Draw(debug_string, debug_legacy_string_x + 0.0, debug_legacy_string_y + 150.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0);

			Share_function_test();

			//menu draw
			for (int i = 0; i <= 2; i++)
			{
				Draw("■", -5.0 + (i * 80), -15.0, 3.0, 3.0, 0.0, 0.0, 0.0, 0.25);
				Draw(main_menu_menu_string[i], 20.0 + (i * 65), 17.5, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
			}
			Draw("■", 252.5, 162.5, 3.0, 3.0, 0.0, 0.0, 0.0, 0.25);
			Draw("Setting", 270.0, 205.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
			
			Draw_texture(Background_image, 1, 0.0, 225.0, 320.0, 15.0);
			Draw(share_bot_button_string, 30.0f, 220.0f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 1.0f);

			if (share_key_touch_held)
				Draw("●", touch_pos.px, touch_pos.py, 0.20f, 0.20f, 1.0f, 0.0f, 0.0f, 1.0f);
			share_fps += 1;
			
			Draw_apply_draw();
			osTickCounterUpdate(&share_total_frame_time);
			share_frame_time_point[0] = osTickCounterRead(&share_total_frame_time);	

			if (share_key_DUP_held)
				debug_string_y -= 1.0;
			else if (share_key_DDOWN_held)
				debug_string_y += 1.0;
			else if (share_key_DRIGHT_held)
				debug_string_x += 5.0;
			else if (share_key_DLEFT_held)
				debug_string_x -= 5.0;

			if (share_key_L_held)
			{
				texture_size_x -= 0.01;
				texture_size_y -= 0.01;
			}
			if (share_key_R_held)
			{
				texture_size_x += 0.01;
				texture_size_y += 0.01;
			}

			if (share_key_CPAD_UP_held)
				debug_legacy_string_y -= 1.0;
			else if (share_key_CPAD_DOWN_held)
				debug_legacy_string_y += 1.0;
			else if (share_key_CPAD_RIGHT_held)
				debug_legacy_string_x += 5.0;
			else if (share_key_CPAD_LEFT_held)
				debug_legacy_string_x -= 5.0;

			if (share_key_X_press)
				Line_exit();

			if (share_key_A_press)
			{
				Share_text_parse(debug_string, debug_part_string);
				Share_text_parse(Share_text_sort(debug_part_string), debug_part_string);
			}
			else if (share_key_B_press)
			{
				debug_string = share_clipboard[0];
			}
			else if (share_key_Y_press)
			{
				memset(share_swkb_input_text, 0x0, 8192);
				swkbdInit(&share_swkb, SWKBD_TYPE_NORMAL, 2, 8192);
				swkbdSetHintText(&share_swkb, "メッセージを入力 / Type message here.");
				swkbdSetValidation(&share_swkb, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
				swkbdSetFeatures(&share_swkb, SWKBD_PREDICTIVE_INPUT);
				swkbdSetInitialText(&share_swkb, share_clipboard[0].c_str());
				swkbdSetDictWord(&share_swkb_words[0], "ぬべ", "壁|՞ةڼ)イーヒヒヒヒヒヒｗｗｗｗｗｗｗｗｗｗｗ");
				swkbdSetDictWord(&share_swkb_words[1], "ぬべ", "┌(☝┌՞ ՞)☝キエェェェエェェwwwww");
				swkbdSetDictWord(&share_swkb_words[2], "ぬべ", "┌(┌ ՞ةڼ)┐<ｷｴｪｪｪｴｴｪｪｪ");
				swkbdSetDictWord(&share_swkb_words[3], "ぬべ", "└(՞ةڼ◔)」");
				swkbdSetDictWord(&share_swkb_words[4], "ぬべ", "(  ՞ةڼ  )");
				swkbdSetDictWord(&share_swkb_words[5], "ぬべ", "└(՞ةڼ◔)」");
				swkbdSetDictWord(&share_swkb_words[6], "びぇ", "。゜( ;⊃՞ةڼ⊂; )゜。びぇぇえええんｗｗｗｗ");
				swkbdSetDictWord(&share_swkb_words[7], "うえ", "(✌ ՞ةڼ ✌ )");
				swkbdSetDictionary(&share_swkb, share_swkb_words, 8);

				//swkbdSetStatusData(&share_swkb, &swkbd_send_message_status, true, true);
				swkbdSetLearningData(&share_swkb, &share_swkb_learn_data, true, true);
				swkbdInputText(&share_swkb, share_swkb_input_text, 1024);
				debug_string = share_swkb_input_text;
			}
			else if (share_key_ZL_press)
			{
				if (share_app_logs_show)
					share_app_logs_show = false;
				else
					share_app_logs_show = true;
			}
			else if (share_key_START_press || (share_key_touch_press && share_touch_pos_x >= 110 && share_touch_pos_x <= 230 && share_touch_pos_y >= 220 && share_touch_pos_y <= 240))
			{
				if (Share_exit_check())
					break;
			}
			else if (share_key_touch_press)
			{
				if (share_touch_pos_x > 0 && share_touch_pos_x < 55 && share_touch_pos_y > 0 && share_touch_pos_y < 60)
				{
					share_line_thread_suspend = false;
					share_line_main_run = true;
					share_menu_main_run = false;
					if (!share_line_already_init)
						Line_init();
				}
				else if (share_touch_pos_x > 80 && share_touch_pos_x < 135 && share_touch_pos_y > 0 && share_touch_pos_y < 60)
				{
					share_google_tr_thread_suspend = false;
					share_google_tr_main_run = true;
					share_menu_main_run = false;
					if (!share_google_tr_already_init)
					{
						Share_app_log_save("Google tr/init", "Initializing...", 1234567890, true);
						Google_tr_init();
						Share_app_log_save("Google tr/init", "Initialized", 1234567890, true);
					}
				}
				else if (share_touch_pos_x > 160 && share_touch_pos_x < 215 && share_touch_pos_y > 0 && share_touch_pos_y < 60)
				{
					share_speed_test_thread_suspend = false;
					share_speed_test_main_run = true;
					share_menu_main_run = false;
					if (!share_speed_test_already_init)
						Speed_test_init();
				}
				else if (share_touch_pos_x > 250 && share_touch_pos_x < 320 && share_touch_pos_y > 175 && share_touch_pos_y < 240)
				{
					share_setting_main_run = true;
					share_menu_main_run = false;
				}
			}
		}
		else if(share_line_main_run)
			Line_main();
		else if (share_google_tr_main_run)
			Google_translation_main();
		else if(share_setting_main_run)
			Setting_menu_main();
		else if (share_speed_test_main_run)
			Speed_test_main();

		Share_key_flag_reset();
		hid_disabled = true;
	}

	Result_with_string exit_result;

	u64 time_out = 3000000000; //3s
	share_line_thread_suspend = false;
	share_google_tr_tr_thread_run = false;
	//share_line_update_thread_run = false;
	//share_line_message_send_thread_run = false;
	//share_line_log_download_thread_run = false;
	share_hid_thread_run = false;
	share_connect_test_thread_run = false;

	share_app_log_num_return = Share_app_log_save("Main/Fs", "Share_save_to_file(Setting.txt)...", 1234567890, true);
	share_setting[2] = std::to_string(share_lcd_brightness);
	share_setting[3] = std::to_string(share_time_to_enter_afk);
	share_setting[4] = std::to_string(share_afk_lcd_brightness);
	if (share_system_setting_menu_show)
		share_setting[5] = "true";
	else
		share_setting[5] = "false";
	share_setting[6] = std::to_string(share_scroll_speed);
	if (share_allow_send_app_info)
		share_setting[7] = "allow";
	else
		share_setting[7] = "deny";

	share_setting[8] = std::to_string(share_num_of_app_start + 1);

	share_setting[0] = "<0" + share_setting[1] + "0>" + "<1" + share_setting[2] + "1>" + "<2" + share_setting[3] + "2>" + "<3" + share_setting[4] + "3>"
		+ "<4" + share_setting[5] + "4>" + "<5" + share_setting[6] + "5>" + "<6" + share_setting[7] + "6>" + "<7" + share_setting[8] + "7>";
	exit_result = Share_save_to_file("Setting.txt", share_setting[0], "/Line/", true, Share_fs_handle, Share_fs_archive);
	Share_app_log_add_result(share_app_log_num_return, exit_result.string, exit_result.code, true);

	//exit
	Share_app_log_save("Main", "Exiting...", 1234567890, true);

	share_app_log_num_return = Share_app_log_save("Main/Thread", "Scan hid thread exit...", 1234567890, true);
	function_result = threadJoin(hid_thread, time_out);
	Share_app_log_add_result(share_app_log_num_return, "", function_result, true);
	share_app_log_num_return = Share_app_log_save("Main/Thread", "Connect test thread exit...", 1234567890, true);
	function_result = threadJoin(share_connect_test_thread, time_out);
	Share_app_log_add_result(share_app_log_num_return, "", function_result, true);

	Share_app_log_save("Main/Svc", "aptExit...", 1234567890, true);
	aptExit();
	Share_app_log_save("Main/Svc", "acExit...", 1234567890, true);
	acExit();
	Share_app_log_save("Main/Svc", "mcuExit...", 1234567890, true);
	mcuHwcExit();
	Share_app_log_save("Main/Svc", "ptmuExit...", 1234567890, true);
	ptmuExit();
	Share_app_log_save("Main/Svc", "nsExit...", 1234567890, true);
	nsExit();
	Share_app_log_save("Main/Svc", "httpcExit...", 1234567890, true);
	httpcExit();
	Share_app_log_save("Main/Svc", "fsExit...\n\n", 1234567890, true);
	fsExit();
	usleep(50000);
	Draw_exit();
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	return 0;
}
