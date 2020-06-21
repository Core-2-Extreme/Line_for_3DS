#include <3ds.h>
#include <algorithm>
#include <unistd.h>
#include <string>
#include "citro2d.h"

#include "hid.hpp"
#include "draw.hpp"
#include "file.hpp"
#include "change_setting.hpp"
#include "share_function.hpp"
#include "speedtest.hpp"
#include "image_viewer.hpp"
#include "line.hpp"
#include "setting_menu.hpp"
#include "google_translation.hpp"
#include "error.hpp"
#include "menu.hpp"
#include "explorer.hpp"
#include "external_font.hpp"
#include "log.hpp"
#include "camera.hpp"
#include "music_player.hpp"

std::string main_init_string = "Main/Init";
std::string main_exit_string = "Main/Exit";
std::string main_svc_name_list[9] = { "fs", "ac", "apt", "mcuHwc", "ptmu", "httpc", "romfs", "cfgu", "am", };
/**/

void Init(void)
{
	u8* init_buffer;
	u32 read_size;
	int log_num;
	int buffer_max_size[10] = { 0xA00000, 0xA00000, 0x700000, 0x500000, 0x500000, 0x4C0000, 0x1400000, 0xA00000, 0x200000,};
	int num_of_msg_list[4] = { GTR_NUM_OF_LANG_LIST_MSG, GTR_NUM_OF_LANG_SHORT_LIST_MSG, CAM_NUM_OF_OPTION_MSG, EXFONT_NUM_OF_FONT_NAME, };
	std::string setting_data[128];
	std::string texture_name_list[4] = { "wifi_signal", "battery_level", "battery_charge", "square", };
	std::string file_name_list[4] = { "gtr_lang_list", "gtr_short_lang_list", "cam_options", "font_name", };
	Result_with_string result;
	result.code = 0;
	result.string = s_success;
	Log_start_up_time_timer();
	init_buffer = (u8*)malloc(0x2000);
	memset(init_buffer, 0x0, 0x2000);
	Log_log_save(main_init_string , "Initializing...", 1234567890, false);
	Log_log_save(main_init_string, s_app_ver, 1234567890, false);

	osSetSpeedupEnable(true);
	gfxInitDefault();

	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);

	Draw_init();
	Draw_set_draw_mode(1);
	Draw_screen_ready_to_draw(0, true, 0, 1.0, 1.0, 1.0);
	Draw_screen_ready_to_draw(1, true, 0, 1.0, 1.0, 1.0);
	Draw_apply_draw();
	osTickCounterStart(&s_tcount_frame_time);


	Draw_progress("0/3 [Main] Initializing service...");

	for (int i = 0; i < 9; i++)
	{
		log_num = Log_log_save(main_init_string, main_svc_name_list[i] + "Init...", 1234567890, s_debug_slow);

		if(i == 0)
			result.code = fsInit();
		else if (i == 1)
			result.code = acInit();
		else if (i == 2)
			result.code = aptInit();
		else if (i == 3)
			result.code = mcuHwcInit();
		else if (i == 4)
			result.code = ptmuInit();
		else if (i == 5)
			result.code = httpcInit(0x500000);
		else if (i == 6)
			result.code = romfsInit();
		else if (i == 7)
			result.code = cfguInit();
		else if (i == 8)
			result.code = amInit();

		if (result.code == 0)
		{
			Log_log_add(log_num, s_success, result.code, s_debug_slow);
			if(i == 3)
				s_mcu_success = true;
		}
		else
			Log_log_add(log_num, s_error, result.code, s_debug_slow);
	}

	log_num = Log_log_save(main_init_string, "APT_SetAppCpuTimeLimit_30...", 1234567890, s_debug_slow);
	result.code = APT_SetAppCpuTimeLimit(30);
	if (result.code == 0)
		Log_log_add(log_num, s_success, result.code, s_debug_slow);
	else
		Log_log_add(log_num,s_error, result.code, s_debug_slow);

	aptSetSleepAllowed(true);
	
	Draw_progress("1/3 [Main] Loading settings...");

	log_num = Log_log_save(main_init_string , "Sem_load_setting(app)...", 1234567890, s_debug_slow);
	result = Sem_load_setting("Setting.txt", "/Line/", 22, s_setting);
	Log_log_add(log_num, result.string, result.code, s_debug_slow);

	for (int i = 0; i < 4; i++)
	{
		log_num = Log_log_save(main_init_string, "File_load_from_rom(" + file_name_list[i] + ".txt)...", 1234567890, s_debug_slow);
		result = File_load_from_rom(file_name_list[i] + ".txt", init_buffer, 0x2000, &read_size, "romfs:/gfx/msg/");
		Log_log_add(log_num, result.string, result.code, false);
		log_num = Log_log_save(main_init_string, "Sem_load_setting()...", 1234567890, s_debug_slow);
		result = Sem_parse_file((char*)init_buffer, num_of_msg_list[i], setting_data);
		Log_log_add(log_num, result.string, result.code, false);
		if (result.code == 0)
		{
			for (int k = 0; k < num_of_msg_list[i]; k++)
			{
				if(i == 0)
					Gtr_set_msg(k, GTR_LANG_LIST, setting_data[k]);
				else if (i == 1)
					Gtr_set_msg(k, GTR_SHORT_LANG_LIST, setting_data[k]);
				else if (i == 2)
					Cam_set_msg(k, CAM_OPTION_MSG, setting_data[k]);
				else if (i == 3)
					Exfont_set_msg(k, setting_data[k]);
			}
		}
	}

	s_lcd_brightness = 100;
	s_time_to_enter_afk = 1500;
	s_afk_lcd_brightness = 10;

	for (int i = 1; i < 4; i++)
	{
		if (std::all_of(s_setting[i].cbegin(), s_setting[i].cend(), isdigit) && !(s_setting[i] == ""))
		{
			if(i == 1)
				s_lcd_brightness = stoi(s_setting[i]);
			else if (i == 2)
				s_time_to_enter_afk = stoi(s_setting[i]);
			else if (i == 3)
				s_afk_lcd_brightness = stoi(s_setting[i]);
		}
	}

	if (s_setting[4] == "true")
		s_system_setting_menu_show = true;
	else
		s_system_setting_menu_show = false;

	if (!(s_setting[5] == "") && stod(s_setting[5]) >= 0.01 && stod(s_setting[5]) <= 1.1)
		s_scroll_speed = stod(s_setting[5]);
	else
		s_scroll_speed = 0.5;
	
	if (s_setting[6] == "allow")
		Sem_set_settings(SEM_ALLOW_SEND_APP_INFO, true);
	else
		Sem_set_settings(SEM_ALLOW_SEND_APP_INFO, false);

	if (std::all_of(s_setting[7].cbegin(), s_setting[7].cend(), isdigit) && !(s_setting[7] == ""))
		s_num_of_app_start = stoi(s_setting[7]);
	else
		s_num_of_app_start = 0;

	if (s_setting[8] == "true")
		Sem_set_settings(SEM_NIGHT_MODE, true);
	else
		Sem_set_settings(SEM_NIGHT_MODE, false);

	if (s_setting[9] == "true")
		Sem_set_settings(SEM_VSYNC_MODE, true);
	else
		Sem_set_settings(SEM_VSYNC_MODE, false);

	if (s_setting[10] == "true")
		Line_set_setting(LINE_HIDE_ID, true);
	else
		Line_set_setting(LINE_HIDE_ID, false);

	if(!(s_setting[11] == "") && stod(s_setting[11]) >= 0.25 && stod(s_setting[11]) <= 3.0)
		Line_set_x_y_size_interval(LINE_TEXT_SIZE, stod(s_setting[11]));
	else
		Line_set_x_y_size_interval(LINE_TEXT_SIZE, 0.66);

	if (!(s_setting[12] == "") && stod(s_setting[12]) >= 10.0 && stod(s_setting[12]) <= 250.0)
		Line_set_x_y_size_interval(LINE_TEXT_INTERVAL, stod(s_setting[12]));
	else
		Line_set_x_y_size_interval(LINE_TEXT_INTERVAL, 35.0);
	
	Line_set_buffer_size(LINE_HTTPC_BUFFER, 0x100000);
	Line_set_buffer_size(LINE_FS_BUFFER, 0x100000);
	Spt_set_buffer_size(SPT_HTTPC_BUFFER, 0x700000);
	Imv_set_buffer_size(IMV_HTTPC_BUFFER, 0x200000);
	Imv_set_buffer_size(IMV_FS_BUFFER, 0x200000);
	Line_set_buffer_size(LINE_SEND_FS_CACHE_BUFFER, 0x200000);
	Line_set_buffer_size(LINE_SEND_FS_BUFFER, 0x500000);
	Mup_set_buffer_size(MUP_FS_OUT_BUFFER, 0x300000);
	Mup_set_buffer_size(MUP_FS_IN_BUFFER, 0x100000);

	for (int i = 13; i < 22; i++)
	{
		if (std::all_of(s_setting[i].cbegin(), s_setting[i].cend(), isdigit) && !(s_setting[i] == "") && stoi(s_setting[i]) >= 0x40000 && stoi(s_setting[i]) <= buffer_max_size[i - 13])
		{
			if(i == 13)
				Line_set_buffer_size(LINE_HTTPC_BUFFER, stoi(s_setting[i]));
			else if (i == 14)
				Line_set_buffer_size(LINE_FS_BUFFER, stoi(s_setting[i]));
			else if (i == 15)
				Spt_set_buffer_size(SPT_HTTPC_BUFFER, stoi(s_setting[i]));
			else if (i == 16)
				Imv_set_buffer_size(IMV_HTTPC_BUFFER, stoi(s_setting[i]));
			else if (i == 17)
				Imv_set_buffer_size(IMV_FS_BUFFER, stoi(s_setting[i]));
			else if (i == 18)
				Line_set_buffer_size(LINE_SEND_FS_CACHE_BUFFER, stoi(s_setting[i]));
			else if (i == 19)
				Line_set_buffer_size(LINE_SEND_FS_BUFFER, stoi(s_setting[i]));
			else if (i == 20)
				Mup_set_buffer_size(MUP_FS_OUT_BUFFER, stoi(s_setting[i]));
			else if (i == 21)
				Mup_set_buffer_size(MUP_FS_IN_BUFFER, stoi(s_setting[i]));
		}
	}

	Draw_progress("2/3 [Main] Starting threads...");
	Hid_init();
	Sem_init();
	Expl_init();
	Sem_set_operation_flag(SEM_RELOAD_MSG_REQUEST, true);
	Sem_set_operation_flag(SEM_CHANGE_WIFI_STATE_REQUEST, true);
	Sem_suspend();

	Draw_progress("3/3 [Main] Loading textures...");

	for (int i = 0; i < 4; i++)
	{
		log_num = Log_log_save(main_init_string, "Loading texture (" + texture_name_list[i] + ".t3x)...", 1234567890, s_debug_slow);

		if(i == 0)
			result = Draw_load_texture("romfs:/gfx/" + texture_name_list[i] + ".t3x", i, Wifi_icon_image, 0, 9);
		else if(i == 1)
			result = Draw_load_texture("romfs:/gfx/" + texture_name_list[i] + ".t3x", i, Battery_level_icon_image, 0, 21);
		else if (i == 2)
			result = Draw_load_texture("romfs:/gfx/" + texture_name_list[i] + ".t3x", i, Battery_charge_icon_image, 0, 1);
		else if (i == 3)
			result = Draw_load_texture("romfs:/gfx/" + texture_name_list[i] + ".t3x", i, Square_image, 0, 1);
		
		Log_log_add(log_num, result.string, result.code, s_debug_slow);
	}
	
	dammy_tint.corners[0].color = 56738247;
	if (Sem_query_settings(SEM_NIGHT_MODE))
		C2D_PlainImageTint(&texture_tint, C2D_Color32f(1.0, 1.0, 1.0, 0.75), true);
	else
		C2D_PlainImageTint(&texture_tint, C2D_Color32f(0.0, 0.0, 0.0, 1.0), true);

	C2D_PlainImageTint(&white_tint, C2D_Color32f(1.0, 1.0, 1.0, 1.0), true);
	C2D_PlainImageTint(&weak_white_tint, C2D_Color32f(1.0, 1.0, 1.0, 0.3), true);
	C2D_PlainImageTint(&red_tint, C2D_Color32f(1.0, 0.0, 0.0, 1.0), true);
	C2D_PlainImageTint(&weak_red_tint, C2D_Color32f(1.0, 0.0, 0.0, 0.3), true);
	C2D_PlainImageTint(&aqua_tint, C2D_Color32f(0.0, 0.75, 1.0, 1.0), true);
	C2D_PlainImageTint(&weak_aqua_tint, C2D_Color32f(0.0, 0.75, 1.0, 0.3), true);
	C2D_PlainImageTint(&yellow_tint, C2D_Color32f(0.5, 0.5, 0.0, 1.0), true);
	C2D_PlainImageTint(&weak_yellow_tint, C2D_Color32f(0.5, 0.5, 0.0, 0.3), true);
	C2D_PlainImageTint(&blue_tint, C2D_Color32f(0.0, 0.0, 1.0, 1.0), true);
	C2D_PlainImageTint(&weak_blue_tint, C2D_Color32f(0.0, 0.0, 1.0, 0.3), true);
	C2D_PlainImageTint(&black_tint, C2D_Color32f(0.0, 0.0, 0.0, 1.0), true);
	C2D_PlainImageTint(&weak_black_tint, C2D_Color32f(0.0, 0.0, 0.0, 0.3), true);

	wifi_state = (u8*)malloc(0x1);
	memset(wifi_state, 0xff, 0x1);
	wifi_state_internet_sample = (u8*)malloc(0x1);
	memset(wifi_state_internet_sample, 0x2, 0x1);
	svcSetThreadPriority(CUR_THREAD_HANDLE, 0x22);

	free(init_buffer);
	Log_log_save(main_init_string , "Initialized.", 1234567890, false);
}

int main()
{
	Init();
	Menu_init();

	// Main loop
	while (aptMainLoop())
	{
		Hid_set_disable_flag(false);
		Draw_set_do_not_draw_flag(false);

		if (Menu_query_must_exit_flag())
			break;

		Menu_main();

		Hid_set_disable_flag(true);
		Draw_set_do_not_draw_flag(true);
	}

	Menu_exit();
	Hid_exit();
	Expl_exit();

	int log_num;
	std::string settings_data;
	Handle fs_handle = 0;
	FS_Archive fs_archive = 0;
	Result_with_string result;

	log_num = Log_log_save(main_exit_string, "File_save_to_file(Setting.txt)...", 1234567890, s_debug_slow);
	s_setting[1] = std::to_string(s_lcd_brightness);
	s_setting[2] = std::to_string(s_time_to_enter_afk);
	s_setting[3] = std::to_string(s_afk_lcd_brightness);
	if (s_system_setting_menu_show)
		s_setting[4] = "true";
	else
		s_setting[4] = "false";
	s_setting[5] = std::to_string(s_scroll_speed);
	if (Sem_query_settings(SEM_ALLOW_SEND_APP_INFO))
		s_setting[6] = "allow";
	else
		s_setting[6] = "deny";

	s_setting[7] = std::to_string(s_num_of_app_start + 1);

	if (Sem_query_settings(SEM_NIGHT_MODE))
		s_setting[8] = "true";
	else
		s_setting[8] = "false";

	if (Sem_query_settings(SEM_VSYNC_MODE))
		s_setting[9] = "true";
	else
		s_setting[9] = "false";

	if (Line_query_setting(LINE_HIDE_ID))
		s_setting[10] = "true";
	else
		s_setting[10] = "false";

	s_setting[11] = std::to_string(Line_query_x_y_size_interval(LINE_TEXT_SIZE));
	s_setting[12] = std::to_string(Line_query_x_y_size_interval(LINE_TEXT_INTERVAL));
	s_setting[13] = std::to_string(Line_query_buffer_size(LINE_HTTPC_BUFFER));
	s_setting[14] = std::to_string(Line_query_buffer_size(LINE_FS_BUFFER));
	s_setting[15] = std::to_string(Spt_query_buffer_size(SPT_HTTPC_BUFFER));
	s_setting[16] = std::to_string(Imv_query_buffer_size(IMV_HTTPC_BUFFER));
	s_setting[17] = std::to_string(Imv_query_buffer_size(IMV_FS_BUFFER));
	s_setting[18] = std::to_string(Line_query_buffer_size(LINE_SEND_FS_CACHE_BUFFER));
	s_setting[19] = std::to_string(Line_query_buffer_size(LINE_SEND_FS_BUFFER));
	s_setting[20] = std::to_string(Mup_query_buffer_size(MUP_FS_OUT_BUFFER));
	s_setting[21] = std::to_string(Mup_query_buffer_size(MUP_FS_IN_BUFFER));

	for (int i = 0; i < 22; i++)
		settings_data += "<" + std::to_string(i) + ">" + s_setting[i] + "</" + std::to_string(i) + ">";

	result = File_save_to_file("Setting.txt", (u8*)settings_data.c_str(), settings_data.length(), "/Line/", true, fs_handle, fs_archive);
	Log_log_add(log_num, result.string, result.code, s_debug_slow);

	for (int i = 0; i < 9; i++)
	{
		Log_log_save(main_exit_string, main_svc_name_list[i] + "Exit...", 1234567890, s_debug_slow);
	
		if (i == 0)
			fsExit();
		else if (i == 1)
			acExit();
		else if (i == 2)
			aptExit();
		else if (i == 3)
			mcuHwcExit();
		else if (i == 4)
			ptmuExit();
		else if (i == 5)
			httpcExit();
		else if (i == 6)
			romfsExit();
		else if (i == 7)
			cfguExit();
		else if (i == 8)
			amExit();
	}

	Draw_exit();
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	return 0;
}