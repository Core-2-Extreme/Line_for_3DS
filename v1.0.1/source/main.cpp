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

//\u0000", "\u0020", "\u0028", "\u0029", "\u0030", "\u0031" , "\u0032" , "\u0033" , "\u0034" , "\u0035" , "\u0036" , "\u0037" , "\u0038" , "\u0039" , "\u0061", "\u0062", "\u0063" ,


//#0020~#007F Invalid #007F
std::string share_font_english_sample[96] = {"\u0000",
"\u0020", "\u0021", "\u0022", "\u0023", "\u0024", "\u0025", "\u0026", "\u0027", "\u0028", "\u0029", "\u002A", "\u002B", "\u002C", "\u002D", "\u002E", "\u002F",
"\u0030", "\u0031", "\u0032", "\u0033", "\u0034", "\u0035", "\u0036", "\u0037", "\u0038", "\u0039", "\u003A", "\u003B", "\u003C", "\u003D", "\u003E", "\u003F", 
"\u0040", "\u0041", "\u0042", "\u0043", "\u0044", "\u0045", "\u0046", "\u0047", "\u0048", "\u0049", "\u004A", "\u004B", "\u004C", "\u004D", "\u004E", "\u004F", 
"\u0050", "\u0051", "\u0052", "\u0053", "\u0054", "\u0055", "\u0056", "\u0057", "\u0058", "\u0059", "\u005A", "\u005B", "\u005C", "\u005D", "\u005E", "\u005F", 
"\u0060", "\u0061", "\u0062", "\u0063", "\u0064", "\u0065", "\u0066", "\u0067", "\u0068", "\u0069", "\u006A", "\u006B", "\u006C", "\u006D", "\u006E", "\u006F", 
"\u0070", "\u0071", "\u0072", "\u0073", "\u0074", "\u0075", "\u0076", "\u0077", "\u0078", "\u0079", "\u007A", "\u007B", "\u007C", "\u007D", "\u007E"  /*"\u007F"*/};

float share_english_font_interval = 11.5;

//#0600~#06FF Invalid #061D
int share_arabic_font_interval[256] = {22,
16, 22, 11, 22, 22, 18, 12, 12, 14, 12, 16,  8,  4,  6, 13, 11,
 7,  6,  8,  8,  8,  7,  9,  6,  5,  5,  5,  4, 16, /*061D*/  7, 10,
12, 10,  8,  6, 11,  6, 13,  4, 14, 10, 14, 14, 11, 11, 11,  8,
 8, 10, 10, 20, 20, 22, 22, 14, 14, 11, 12, 16, 16, 12, 12, 12,
 8, 16, 14, 14, 12, 10, 12, 10, 12, 13, 13,  7,  8,  7,  7,  7,
 7,  7,  7,  7,  6,  6,  4,  8,  4,  6,  6,  6,  6,  5,  6,  5,
 6,  6, 10, 10, 10, 10, 10, 10, 10, 10, 10,  6,  4,  8, 14, 12,
 4,  8,  7,  6,  5,  7, 14, 14, 15, 14, 14, 14, 14, 14, 14, 14,
14, 11, 11, 11, 11, 11, 11, 11,  8,  8,  8,  8,  8,  8,  8,  8,
 8, 10, 10, 10, 10, 11, 10, 10, 10, 10, 20, 20, 20, 22, 22, 14,
10, 16, 16, 16, 16, 16, 16, 14, 14, 15, 18, 15, 14, 14, 14, 15,
15, 15, 15, 15, 15, 12, 12, 12, 10, 12, 12, 12, 12, 12, 13, 10,
10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 13, 14, 13, 10,
13, 13, 13, 13,  8, 10, 13, 11,  7,  7,  8,  8, 13, 14, 16,  5,
 5,  8,  8, 13,  5,  8,  9,  9,  9, 10,  6,  6,  6,  8,  7,  9,
 6,  6, 10, 10, 10, 10,  8, 10, 10, 10, 18, 18, 10,  6,  8, 10};

//#0600~#06FF Invalid #061D
std::string debug_arabic_sample[256] = {"\u0000",
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

//#0531~#058F Invalid #0557,#0558,#058B,#058C
int share_armenian_font_interval[92] = {22,
    12,  9, 12, 12, 10, 10,  8, 10, 13, 12, 10,  8, 12, 13, 13,
10, 10, 12, 10, 12, 12, 12, 12, 10, 10, 12, 10, 11, 10, 13,  9,
 9, 10, 10, 12, 13, 10, 13,/*0557,0558*/ 4,  5,  4,  8,  5,  8,  8,
14, 12,  8,  9,  9,  8,  8,  8,  8,  8,  9,  8,  6, 10,  8,  8,
 8,  8,  8,  8,  9,  6,  8,  8,  8,  7, 12,  9,  9,  8,  8, 11,
 8,  8,  7, 12,  4,  8, 11, 10, 11,  4, 10, /*058B 058C*/ 12, 12, 11};

//#0531~#058F Invalid #0557,#0558,#058B,#058C
std::string debug_armenian_sample[92] = {"\u0000",
"\u0531", "\u0532", "\u0533", "\u0534", "\u0535", "\u0536", "\u0537", "\u0538", "\u0539", "\u053A", "\u053B", "\u053C", "\u053D", "\u053E", "\u053F",
"\u0540", "\u0541", "\u0542", "\u0543", "\u0544", "\u0545", "\u0546", "\u0547", "\u0548", "\u0549", "\u054A", "\u054B", "\u054C", "\u054D", "\u054E", "\u054F",
"\u0550", "\u0551", "\u0552", "\u0553", "\u0554", "\u0555", "\u0556", /*u0557     0558 */ "\u0559", "\u055A", "\u055B", "\u055C", "\u055D", "\u055E", "\u055F",
"\u0560", "\u0561", "\u0562", "\u0563", "\u0564", "\u0565", "\u0566", "\u0567", "\u0568", "\u0569", "\u056A", "\u056B", "\u056C", "\u056D", "\u056E", "\u056F",
"\u0570", "\u0571", "\u0572", "\u0573", "\u0574", "\u0575", "\u0576", "\u0577", "\u0578", "\u0579", "\u057A", "\u057B", "\u057C", "\u057D", "\u057E", "\u057F",
"\u0580", "\u0581", "\u0582", "\u0583", "\u0584", "\u0585", "\u0586", "\u0587", "\u0588", "\u0589", "\u058A", /*"\u058B" "\u058C"*/ "\u058D", "\u058E", "\u058F"};
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


Thread bgm_thread, hid_thread, share_update_thread;
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

		if (memcmp((void*)debug_part_string[i].c_str(), (void*)debug_arabic_sample[0].c_str(), 0x2) == 0)
		{
			break;
		}

		if (debug_part_string[i].length() == 1) //en
		{
			for (int j = 1; j < 96; j++)
			{
				if (memcmp((void*)debug_part_string[i].c_str(), (void*)share_font_english_sample[j].c_str(), 0x1) == 0)
				{
					Draw_texture(TEXTURE_ENGLISH_FONT, j, debug_string_x + current_text_x_pos, debug_string_y + 150.0);
					debug_unknown_char = false;
					current_text_x_pos += share_english_font_interval;
					break;
				}
			}
		}
		else if (debug_part_string[i].length() >= 2)
		{
			for (int j = 1; j < 256; j++) //arabic
			{
				if (memcmp((void*)debug_part_string[i].c_str(), (void*)debug_arabic_sample[j].c_str(), 0x2) == 0)
				{
					Draw_texture(TEXTURE_ARABIC_FONT, j, debug_string_x + current_text_x_pos, debug_string_y + 150.0);
					debug_unknown_char = false;
					current_text_x_pos += share_arabic_font_interval[j];
					break;
				}
			}

			for (int j = 1; j < 92; j++) //armenian
			{
				if (memcmp((void*)debug_part_string[i].c_str(), (void*)debug_armenian_sample[j].c_str(), 0x2) == 0)
				{
					Draw_texture(TEXTURE_ARMENIAN_FONT, j, debug_string_x + current_text_x_pos, debug_string_y + 150.0);
					debug_unknown_char = false;
					current_text_x_pos += share_armenian_font_interval[j];
					break;
				}
			}
		}

		if (debug_unknown_char)
		{
			Draw_texture(TEXTURE_ARABIC_FONT, 0, debug_string_x + current_text_x_pos, debug_string_y + 150.0);
			current_text_x_pos += 23;
		}
	}

	/*
#define SAMPLERATE 22050
#define SAMPLESPERBUF (SAMPLERATE / 30)
#define BYTESPERSAMPLE 4

//----------------------------------------------------------------------------
void fill_buffer(void* audioBuffer, size_t offset, size_t size, int frequency)
{
	//----------------------------------------------------------------------------

	u32* dest = (u32*)audioBuffer;

	for (int i = 0; i < size; i++) {

		s16 sample = INT16_MAX * sin(frequency * (2 * M_PI) * (offset + i) / SAMPLERATE);

		dest[i] = (sample << 16) | (sample & 0xffff);
	}

	DSP_FlushDataCache(audioBuffer, size);

}

void Share_bgm(void *arg)
{
	ndspInit();

	ndspWaveBuf waveBuf;
	u32* audioBuffer = (u32*)linearAlloc(SAMPLESPERBUF * BYTESPERSAMPLE * 2);

	std::string debug_music;
	debug_music = Share_load_from_file("music.wav", "/Line", music_h, music_fs_a);

	audioBuffer = (u32*)debug_music.c_str();

	ndspSetOutputMode(NDSP_OUTPUT_STEREO);

	ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
	ndspChnSetRate(0, SAMPLERATE);
	ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);

	int notefreq[] = {
		261, 261, 392, 392, 440, 440,
		392, 349, 349, 329, 329, 294, 294, 261, 261
	};

	memset(&waveBuf, 0, sizeof(waveBuf));
	waveBuf.data_vaddr = &audioBuffer[0];
	waveBuf.nsamples = SAMPLESPERBUF;

	size_t stream_offset = 0;

	fill_buffer(audioBuffer, stream_offset, SAMPLESPERBUF * 2, notefreq[0]);

	stream_offset += SAMPLESPERBUF;

	ndspChnWaveBufAdd(0, &waveBuf);

	for (int i = 0; i < 15; i++) {
		for (int note = 0; note < 10; note++) {

			if (waveBuf.status == NDSP_WBUF_DONE) {

				fill_buffer(waveBuf.data_pcm16, stream_offset, waveBuf.nsamples, notefreq[i]);
				ndspChnWaveBufAdd(0, &waveBuf);
				stream_offset += waveBuf.nsamples;
			}
			usleep(20000);

		}
	}


	linearFree(audioBuffer);
	ndspExit();
}*/

	/*

	std::string debug_music;
	debug_music = Share_load_from_file("music.wav", "/Line", music_h, music_fs_a);

	function_result = ndspInit();
	Share_app_log_save("debug", "ndspInit();", function_result, true);

	ndspChnWaveBufClear(0x0);
	ndspChnReset(0x0);
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	ndspChnSetInterp(0x0, NDSP_INTERP_NONE);
	ndspChnSetRate(0x0, 22050);
	ndspChnSetFormat(0x0, NDSP_FORMAT_STEREO_PCM16);

	u32* audio_buffer = (u32*)linearAlloc((22050 / 30) * 22050 * 2);

	ndspWaveBuf audio_wave_buffer[2];

//	memcpy(audio_buffer, (void*)debug_music.c_str(), (22050 / 30));
	Share_app_log_save("debug", "size = " + std::to_string(debug_music.length()), function_result, true);
	usleep(1000000);

	//memset(audio_wave_buffer, 0, sizeof(audio_buffer));
	audio_wave_buffer[0].data_vaddr = (void*)debug_music.c_str();
	audio_wave_buffer[0].nsamples = (22050 / 30);
	//audio_wave_buffer[1].data_vaddr = &audio_buffer[(22050 / 30)];
	//audio_wave_buffer[1].nsamples = (22050 / 30);

	usleep(1000000);
	DSP_FlushDataCache(audio_buffer, (22050 / 30) * 2);

	ndspChnWaveBufAdd(0, &audio_wave_buffer[0]);*/
	//ndspChnWaveBufAdd(0, &audio_wave_buffer[1]);*/
	/*
		if (audio_wave_buffer[0].status == NDSP_WBUF_DONE)
		{
			DSP_FlushDataCache(&audio_wave_buffer, (22050 / 30) * 2);
			ndspChnWaveBufAdd(0, &audio_wave_buffer[0]);
		}*/
	

	/*
	Handle share_connect_to_ap;
	svcCreateEvent(&share_connect_to_ap, RESET_ONESHOT);

	char share_result_char[512];
	Share_app_log_save("Debug", "", 1234567890, true);
	Handle share_ac_handle;
	char* share_ssid;
	char* share_mac_address;
	share_ssid = (char*)malloc(0x100);
	share_mac_address = (char*)malloc(0x100);
	sprintf(share_ssid, "└(՞ةڼ◔)」_Free_WiFi");
	sprintf(share_mac_address, "0xFF 0xFF 0xFF 0xFF 0xFF 0xFF");
	srvGetServiceHandle(&share_ac_handle, "nwm::INF");
	//c8a06c0c
	u32* cache_buffer = getThreadCommandBuffer();
	cache_buffer[0] = 0x00080302;//IPC_MakeHeader(0x36, 0, 0); // 0x00360000
	cache_buffer[1] = (u32)share_ssid;
	cache_buffer[9] = strlen(share_ssid);
	cache_buffer[10] = 0;// (u32)share_mac_address;
	cache_buffer[14] = 0;//share_connect_to_ap;

	svcSendSyncRequest(share_ac_handle);
	svcCloseHandle(share_ac_handle);

	for (int i = 1; i <= 8; i++)
	{
		sprintf(share_result_char, "%ld, 0x%lx, %s", cache_buffer[i], cache_buffer[i], std::to_string(cache_buffer[i]).c_str());
		Share_app_log_save("Debug", share_result_char, 1234567890, false);
	}
	*/
	/*
		char share_result_char[512];
		Share_app_log_save("Debug", "", 1234567890, true);
		Handle share_ac_handle;

		srvGetServiceHandle(&share_ac_handle, "nwm::INF");

		u32* cache_buffer = getThreadCommandBuffer();
		cache_buffer[0] = 0x00080302;//IPC_MakeHeader(0x36, 0, 0); // 0x00360000
		cache_buffer[1] = (u32)".Wi2_Free_at_[SK.GROUP]";
		cache_buffer[9] = strlen(".Wi2_Free_at_[SK.GROUP]");
		cache_buffer[10] = 0xFF;

		svcSendSyncRequest(share_ac_handle);
		svcCloseHandle(share_ac_handle);

		for (int i = 1; i <= 8; i++)
		{
			sprintf(share_result_char, "%ld, 0x%lx, %s", cache_buffer[i], cache_buffer[i], std::to_string(cache_buffer[i]).c_str());
			Share_app_log_save("Debug", share_result_char, 1234567890, false);
		}*/

}

int main()
{
	osTickCounterStart(&share_tick_counter_up_time);
	osSetSpeedupEnable(true);
	gfxInitDefault();

	romfsInit();
	cfguInit();

	Share_app_log_save("Main/Ver", share_app_ver, 1234567890, false);

	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);	
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	
	Draw_init();

	Draw_set_draw_mode(1);
	Draw_screen_ready_to_draw(0, true, 0);
	Draw_screen_ready_to_draw(1, true, 0);

	osTickCounterStart(&share_total_frame_time);
	
	share_app_log_num_return = Share_app_log_save("Main/Svc", "fsInit...", 1234567890, true);
	function_result = fsInit();
	Share_app_log_add_result(share_app_log_num_return, "", function_result, true);
	if (function_result == 0)
		share_fs_success = true;
	share_app_log_num_return = Share_app_log_save("Main/Svc", "acInit...", 1234567890, true);
	function_result = acInit();
	Share_app_log_add_result(share_app_log_num_return, "", function_result, true);
	if (function_result == 0)
		share_ac_success = true;
	share_app_log_num_return = Share_app_log_save("Main/Svc", "aptInit...", 1234567890, true);
	function_result = aptInit();
	Share_app_log_add_result(share_app_log_num_return, "", function_result, true);
	if (function_result == 0)
		share_apt_success = true;
	share_app_log_num_return = Share_app_log_save("Main/Svc", "mcuInit...", 1234567890, true);
	function_result = mcuHwcInit();
	Share_app_log_add_result(share_app_log_num_return, "", function_result, true);
	if (function_result == 0)
		share_mcu_success = true;
	share_app_log_num_return = Share_app_log_save("Main/Svc", "ptmuInit...", 1234567890, true);
	function_result = ptmuInit();
	Share_app_log_add_result(share_app_log_num_return, "", function_result, true);
	if (function_result == 0)
		share_ptmu_success = true;
	share_app_log_num_return = Share_app_log_save("Main/Svc", "httpcInit...", 1234567890, true);
	function_result = httpcInit(0x500000);
	Share_app_log_add_result(share_app_log_num_return, "", function_result, true);
	if (function_result == 0)
		share_httpc_success = true;
	share_app_log_num_return = Share_app_log_save("Main/Svc", "nsInit...", 1234567890, true);
	function_result = nsInit();
	Share_app_log_add_result(share_app_log_num_return, "", function_result, true);
	if (function_result == 0)
		share_ns_success = true;
	share_app_log_num_return = Share_app_log_save("Main/Svc", "APT_SetAppCpuTimeLimit...", 1234567890, true);
	function_result = APT_SetAppCpuTimeLimit(30);
	Share_app_log_add_result(share_app_log_num_return, "", function_result, true);
	aptSetSleepAllowed(true);

	share_connected_ssid = (char*)malloc(0x200);

	share_app_log_num_return = Share_app_log_save("HW/LAN", "Wifi_enable...", 1234567890, true);
	function_result = Wifi_enable();
	Share_app_log_add_result(share_app_log_num_return, "", function_result, true);
	share_wifi_enabled = true;
	
	share_app_log_num_return = Share_app_log_save("Main/Fs", "Share_load_from_file...", 1234567890, true);
	share_setting[0] = Share_load_from_file("Setting.txt", "/Line/", Share_fs_handle, Share_fs_archive);
	if (share_setting[0] == "file read error")
		Share_app_log_add_result(share_app_log_num_return, "", -1, true);
	else
		Share_app_log_add_result(share_app_log_num_return, "", 0, true);

	for (int i = 0; i <= 5; i++)
	{
		share_app_log_num_return = Share_app_log_save("Main/Fs", "setting value" + std::to_string(i) + " : ", 1234567890, false);
		share_setting_parse_text = "<" + std::to_string(i);
		share_setting_parse_start_num = share_setting[0].find(share_setting_parse_text);
		share_setting_parse_text = std::to_string(i) + ">";
		share_setting_parse_end_num = share_setting[0].find(share_setting_parse_text);

		if (share_setting_parse_end_num == -1 || share_setting_parse_start_num == -1)
		{
			Share_app_log_save("Main/Fs", "Failed to load settings. Default values has been applied.", 1234567890, false);
			share_setting[1] = "en";
			share_setting[2] = "100";
			share_setting[3] = "600";
			share_setting[4] = "10";
			share_setting[5] = "1";
			share_setting[6] = "0.5";
			share_setting_load_failed = true;
			break;
		}
		share_setting_parse_end_num -= share_setting_parse_start_num;
		share_setting_parse_start_num += share_setting_parse_text.length();
		share_setting[i + 1] = share_setting[0].substr(share_setting_parse_start_num, share_setting_parse_end_num - share_setting_parse_text.length());
		Share_app_log_add_result(share_app_log_num_return, share_setting[i + 1], 1234567890, true);
	}

	if (!share_setting_load_failed)
	{
		share_lcd_bridgeness = stoi(share_setting[2]);
		share_time_to_enter_afk = stoi(share_setting[3]);
		share_afk_lcd_bridgeness = stoi(share_setting[4]);
		share_system_setting_menu_show = stoi(share_setting[5]);
		share_scroll_speed = stod(share_setting[6]);
	}

	share_update_thread_run = true;
	share_update_thread = threadCreate(Share_update_thread, (void*)(""), STACKSIZE, 0x24, -1, true);
	share_hid_thread_run = true;
	hid_thread = threadCreate(Share_scan_hid, (void*)(""), STACKSIZE, 0x20, -1, true);
	share_connect_test_thread_run = true;
	share_connect_test_thread = threadCreate(Share_connectivity_check_thread, (void*)(""), STACKSIZE, 0x30, -1, true);

	wifi_state = (u8*)malloc(0x1);
	memset(wifi_state, 0xff, 0x1);
	wifi_state_internet_sample = (u8*)malloc(0x1);
	memset(wifi_state_internet_sample, 0x2, 0x1);
	debug_string_char = (char*)malloc(0x2048);
	memset(debug_string_char, 0x0, 0x2048);

	Draw_load_texture();
	
	// Main loop
	while (aptMainLoop())
	{
		hid_disabled = false;
		Share_get_system_info();
		sprintf(share_status, "%dfps %.1fms %02d/%02d %02d:%02d:%02d "
			, share_fps_show, share_frame_time_point[0], share_month, share_day, share_hours, share_minutes, share_seconds);
		
		if (share_menu_main_run)
		{
			Draw_set_draw_mode(1);
			Draw_screen_ready_to_draw(0, true, 1);			

			Draw_texture(TEXTURE_BACKGROUND, 0, 0.0, 0.0);
			Draw_texture(TEXTURE_WIFI_ICON, share_wifi_signal, 360.0, 0.0);
			Draw_texture(TEXTURE_BATTERY_LEVEL_ICON, share_battery_level / 5, 330.0, 0.0);
			if(share_battery_charge)
				Draw_texture(TEXTURE_BATTERY_CHARGE_ICON, 0, 310.0, 0.0);
			Draw(share_status, 0.0f, 0.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 1.0f);
			Draw(share_battery_level_string, 337.5f, 1.25f, 0.4f, 0.4f, 0.0f, 0.0f, 0.0f, 0.5f);
		

		/*	Draw("Key A press : " + std::to_string(share_key_A_press) + " Key A held : " + std::to_string(share_key_A_held), 0.0, 50.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
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
			
			Draw_texture(TEXTURE_BACKGROUND, 1, 0.0, 225.0);
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

			if (share_key_CPAD_UP_held)
				debug_legacy_string_y -= 1.0;
			else if (share_key_CPAD_DOWN_held)
				debug_legacy_string_y += 1.0;
			else if (share_key_CPAD_RIGHT_held)
				debug_legacy_string_x += 5.0;
			else if (share_key_CPAD_LEFT_held)
				debug_legacy_string_x -= 5.0;

			if (share_key_A_press)
			{
				debug_string_length = debug_string.length();
				memset(debug_string_char, 0x0, 0x2048);
				memcpy(debug_string_char, (void*)debug_string.c_str(), debug_string_length);
				for (int i = 0; i <= 8191; i++)
					debug_part_string[i] = "";

				int i = 0;
				int std_num = 0;
				while(true)
				{
					debug_parse_string_length = mblen(&debug_string_char[i], 16);

					if (i >= debug_string_length)
						break;
					else if (debug_parse_string_length >= 1)
					{
						debug_part_string[std_num] = debug_string.substr(i, debug_parse_string_length);
						i += debug_parse_string_length;
						std_num++;
					}
					else
						i++;
				}
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
				swkbdInputText(&share_swkb, share_swkb_input_text, 32);
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
					{
						Share_app_log_save("Speedtest/init", "Initializing...", 1234567890, true);
						Speed_test_init();
						Share_app_log_save("Speedtest/init", "Initialized", 1234567890, true);
					}
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

	u64 time_out = 3000000000; //3s
	share_line_thread_suspend = false;
	share_google_tr_tr_thread_run = false;
	//share_line_update_thread_run = false;
	//share_line_message_send_thread_run = false;
	//share_line_log_download_thread_run = false;
	share_hid_thread_run = false;
	share_connect_test_thread_run = false;

	share_app_log_num_return = Share_app_log_save("Main/Fs", "Share_save_to_file(Setting.txt)...", 1234567890, true);
	share_setting[2] = std::to_string(share_lcd_bridgeness);
	share_setting[3] = std::to_string(share_time_to_enter_afk);
	share_setting[4] = std::to_string(share_afk_lcd_bridgeness);
	share_setting[5] = std::to_string(share_system_setting_menu_show);
	share_setting[6] = std::to_string(share_scroll_speed);
	share_setting[0] = "<0" + share_setting[1] + "0>" + "<1" + share_setting[2] + "1>" + "<2" + share_setting[3] + "2>" + "<3" + share_setting[4] + "3>"
		+ "<4" + share_setting[5] + "4>" + "<5" + share_setting[6] + "5>";
	function_result = Share_save_to_file("Setting.txt", share_setting[0], "/Line/", true, Share_fs_handle, Share_fs_archive);
	Share_app_log_add_result(share_app_log_num_return, "", function_result, true);

	//exit
	Share_app_log_save("Main", "Exiting...", 1234567890, true);

	//share_app_log_num_return = Share_app_log_save("Main/Thread", "Update thread exit...", 1234567890, true);
	//function_result = threadJoin(update_thread, time_out);
	//Share_app_log_add_result(share_app_log_num_return, "", function_result, true);
	//share_app_log_num_return = Share_app_log_save("Main/Thread", "Send message thread exit...", 1234567890, true);
	//function_result = threadJoin(message_send_thread, time_out);
	//Share_app_log_add_result(share_app_log_num_return, "", function_result, true);
	//share_app_log_num_return = Share_app_log_save("Main/Thread", "Log download thread exit...", 1234567890, true);
	//function_result = threadJoin(log_download_thread, time_out);
	//Share_app_log_add_result(share_app_log_num_return, "", function_result, true);
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