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
#include "log.hpp"

/**/

void Init(void)
{
	u8* init_buffer;
	u32 init_read_size = 0;
	int init_log_num_return;
	int init_setting_parse_start_num = 0;
	int init_setting_parse_end_num = 0;
	std::string init_setting_parse_start_text;
	std::string init_setting_parse_end_text;
	Handle init_fs_handle = 0;
	FS_Archive init_fs_archive = 0;
	Result_with_string init_result;
	init_result.code = 0;
	init_result.string = "[Success] ";
	Log_start_up_time_timer();
	init_buffer = (u8*)malloc(0x2000);
	memset(init_buffer, 0x0, 0x2000);
	Log_log_save("Main/Init", "Initializing...", 1234567890, false);
	Log_log_save("Main/Init/ver", s_app_ver, 1234567890, false);

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
	init_log_num_return = Log_log_save("Main/Init", "fsInit...", 1234567890, s_debug_slow);
	init_result.code = fsInit();
	if (init_result.code == 0)
	{
		Log_log_add(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
		s_fs_success = true;
	}
	else
		Log_log_add(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);


	init_log_num_return = Log_log_save("Main/Init", "acInit...", 1234567890, s_debug_slow);
	init_result.code = acInit();
	if (init_result.code == 0)
	{
		Log_log_add(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
		s_ac_success = true;
	}
	else
		Log_log_add(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);


	init_log_num_return = Log_log_save("Main/Init", "aptInit...", 1234567890, s_debug_slow);
	init_result.code = aptInit();
	if (init_result.code == 0)
	{
		Log_log_add(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
		s_apt_success = true;
	}
	else
		Log_log_add(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);


	init_log_num_return = Log_log_save("Main/Init", "mcuInit...", 1234567890, s_debug_slow);
	init_result.code = mcuHwcInit();
	if (init_result.code == 0)
	{
		Log_log_add(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
		s_mcu_success = true;
	}
	else
		Log_log_add(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);


	init_log_num_return = Log_log_save("Main/Init", "ptmuInit...", 1234567890, s_debug_slow);
	init_result.code = ptmuInit();
	if (init_result.code == 0)
	{
		Log_log_add(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
		s_ptmu_success = true;
	}
	else
		Log_log_add(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);

	init_log_num_return = Log_log_save("Main/Init", "httpcInit...", 1234567890, s_debug_slow);
	init_result.code = httpcInit(0x500000);
	if (init_result.code == 0)
	{
		Log_log_add(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
		s_httpc_success = true;
	}
	else
		Log_log_add(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);


	init_log_num_return = Log_log_save("Main/Init", "romfsInit...", 1234567890, s_debug_slow);
	init_result.code = romfsInit();
	if (init_result.code == 0)
	{
		Log_log_add(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
		s_rom_success = true;
	}
	else
		Log_log_add(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);


	init_log_num_return = Log_log_save("Main/Init", "cfguInit...", 1234567890, s_debug_slow);
	init_result.code = cfguInit();
	if (init_result.code == 0)
	{
		Log_log_add(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
		s_cfg_success = true;
	}
	else
		Log_log_add(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);

	init_log_num_return = Log_log_save("Main/Init", "amInit...", 1234567890, s_debug_slow);
	init_result.code = amInit();
	if (init_result.code == 0)
	{
		Log_log_add(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
		s_am_success = true;
	}
	else
		Log_log_add(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);

	init_log_num_return = Log_log_save("Main/Init/apt", "APT_SetAppCpuTimeLimit_30...", 1234567890, s_debug_slow);
	init_result.code = APT_SetAppCpuTimeLimit(30);
	if (init_result.code == 0)
		Log_log_add(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
	else
		Log_log_add(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);

	aptSetSleepAllowed(true);

	init_log_num_return = Log_log_save("Main/Init/nwm", "Wifi_enable...", 1234567890, s_debug_slow);
	init_result.code = Wifi_enable();
	if (init_result.code == 0)
		Log_log_add(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
	else
		Log_log_add(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);
	
	s_wifi_enabled = true;

	Draw_progress("1/3 [Main] Loading settings...");
	init_log_num_return = Log_log_save("Main/Init/fs", "Share_load_from_file(Setting.txt)...", 1234567890, s_debug_slow);
	init_result = Share_load_from_file("Setting.txt", init_buffer, 0x2000, &init_read_size, "/Line/", init_fs_handle, init_fs_archive);
	Log_log_add(init_log_num_return, init_result.string, init_result.code, s_debug_slow);

	if (init_result.code == 0)
		s_setting[0] = (char*)init_buffer;

	for (int i = 1; i <= 20; i++)
	{
		init_log_num_return = Log_log_save("Main/Init/fs", "setting value" + std::to_string(i) + " : ", 1234567890, s_debug_slow);
		init_setting_parse_start_text = "<" + std::to_string(i - 1) + ">";
		init_setting_parse_start_num = s_setting[0].find(init_setting_parse_start_text);
		init_setting_parse_end_text = "</" + std::to_string(i - 1) + ">";
		init_setting_parse_end_num = s_setting[0].find(init_setting_parse_end_text);

		if (init_setting_parse_end_num == -1 || init_setting_parse_start_num == -1)
		{
			Log_log_save("Main/Init/fs", "Failed to load settings. Default values has been applied.", 1234567890, s_debug_slow);

			if (i <= 1)
				s_setting[1] = "en";
			if (i <= 2)
				s_setting[2] = "100";
			if (i <= 3)
				s_setting[3] = "1500";
			if (i <= 4)
				s_setting[4] = "10";
			if (i <= 5)
				s_setting[5] = "false";
			if (i <= 6)
				s_setting[6] = "0.5";
			if (i <= 7)
				s_setting[7] = "deny";
			if (i <= 8)
				s_setting[8] = "0";
			if (i <= 9)
				s_setting[9] = "false";
			if (i <= 10)
				s_setting[10] = "true";
			if (i <= 11)
				s_setting[11] = "false";
			if (i <= 12)
				s_setting[12] = "0.65";
			if (i <= 13)
				s_setting[13] = "35.0";
			if (i <= 14)
				s_setting[14] = std::to_string(0x200000);
			if (i <= 15)
				s_setting[15] = std::to_string(0x200000);
			if (i <= 16)
				s_setting[16] = std::to_string(0x700000);
			if (i <= 17)
				s_setting[17] = std::to_string(0x200000);
			if (i <= 18)
				s_setting[18] = std::to_string(0x200000);
			if (i <= 19)
				s_setting[19] = std::to_string(0x200000);
			if (i <= 20)
				s_setting[20] = std::to_string(0x500000);

			break;
		}
		init_setting_parse_start_num += init_setting_parse_start_text.length();
		init_setting_parse_end_num -= init_setting_parse_start_num;
		s_setting[i] = s_setting[0].substr(init_setting_parse_start_num, init_setting_parse_end_num);
		Log_log_add(init_log_num_return, s_setting[i], 1234567890, s_debug_slow);
	}

	if (std::all_of(s_setting[2].cbegin(), s_setting[2].cend(), isdigit) && !(s_setting[2] == ""))
		s_lcd_brightness = stoi(s_setting[2]);
	else
		s_lcd_brightness = 100;

	if (std::all_of(s_setting[3].cbegin(), s_setting[3].cend(), isdigit) && !(s_setting[3] == ""))
		s_time_to_enter_afk = stoi(s_setting[3]);
	else
		s_time_to_enter_afk = 1500;

	if (std::all_of(s_setting[4].cbegin(), s_setting[4].cend(), isdigit) && !(s_setting[4] == ""))
		s_afk_lcd_brightness = stoi(s_setting[4]);
	else
		s_afk_lcd_brightness = 10;

	if (s_setting[5] == "true")
		s_system_setting_menu_show = true;
	else
		s_system_setting_menu_show = false;

	if (!(s_setting[6] == "") && stod(s_setting[6]) >= 0.01 && stod(s_setting[12]) <= 1.1)
		s_scroll_speed = stod(s_setting[6]);
	else
		s_scroll_speed = 0.5;
	
	if (s_setting[7] == "allow")
		s_allow_send_app_info = true;
	else
		s_allow_send_app_info = false;

	if (std::all_of(s_setting[8].cbegin(), s_setting[8].cend(), isdigit) && !(s_setting[8] == ""))
		s_num_of_app_start = stoi(s_setting[8]);
	else
		s_num_of_app_start = 0;

	if (s_setting[9] == "true")
		s_night_mode = true;
	else
		s_night_mode = false;

	if (s_setting[10] == "true")
		s_draw_vsync_mode = true;
	else
		s_draw_vsync_mode = false;

	if (s_setting[11] == "true")
		Line_set_setting(LINE_HIDE_ID, true);
	else
		Line_set_setting(LINE_HIDE_ID, false);

	if(!(s_setting[12] == "") && stod(s_setting[12]) >= 0.25 && stod(s_setting[12]) <= 3.0)
		Line_set_x_y_size_interval(LINE_TEXT_SIZE, stod(s_setting[12]));
	else
		Line_set_x_y_size_interval(LINE_TEXT_SIZE, 0.66);

	if (!(s_setting[13] == "") && stod(s_setting[13]) >= 10.0 && stod(s_setting[13]) <= 250.0)
		Line_set_x_y_size_interval(LINE_TEXT_INTERVAL, stod(s_setting[13]));
	else
		Line_set_x_y_size_interval(LINE_TEXT_INTERVAL, 35.0);
	
	if (std::all_of(s_setting[14].cbegin(), s_setting[14].cend(), isdigit) && !(s_setting[14] == "") && stoi(s_setting[14]) >= 0x40000 && stoi(s_setting[14]) <= 0xA000000)
		Line_set_buffer_size(LINE_HTTPC_BUFFER, stoi(s_setting[14]));
	else
		Line_set_buffer_size(LINE_HTTPC_BUFFER, 0x200000);

	if (std::all_of(s_setting[15].cbegin(), s_setting[15].cend(), isdigit) && !(s_setting[15] == "") && stoi(s_setting[15]) >= 0x40000 && stoi(s_setting[15]) <= 0xA000000)
		Line_set_buffer_size(LINE_FS_BUFFER, stoi(s_setting[15]));
	else
		Line_set_buffer_size(LINE_FS_BUFFER, 0x200000);

	if (std::all_of(s_setting[16].cbegin(), s_setting[16].cend(), isdigit) && !(s_setting[16] == "") && stoi(s_setting[16]) >= 0x40000 && stoi(s_setting[16]) <= 0x700000)
		Spt_set_buffer_size(SPT_HTTPC_BUFFER, stoi(s_setting[16]));
	else
		Spt_set_buffer_size(SPT_HTTPC_BUFFER, 0x700000);

	if (std::all_of(s_setting[17].cbegin(), s_setting[17].cend(), isdigit) && !(s_setting[17] == "") && stoi(s_setting[17]) >= 0x40000 && stoi(s_setting[17]) <= 0x500000)
		Imv_set_buffer_size(IMV_HTTPC_BUFFER, stoi(s_setting[17]));
	else
		Imv_set_buffer_size(IMV_HTTPC_BUFFER, 0x200000);

	if (std::all_of(s_setting[18].cbegin(), s_setting[18].cend(), isdigit) && !(s_setting[18] == "") && stoi(s_setting[18]) >= 0x40000 && stoi(s_setting[18]) <= 0x500000)
		Imv_set_buffer_size(IMV_FS_BUFFER, stoi(s_setting[18]));
	else
		Imv_set_buffer_size(IMV_FS_BUFFER, 0x200000);

	if (std::all_of(s_setting[19].cbegin(), s_setting[19].cend(), isdigit) && !(s_setting[19] == "") && stoi(s_setting[19]) >= 0x40000 && stoi(s_setting[19]) <= 0x4C0000)
		Line_set_buffer_size(LINE_SEND_FS_CACHE_BUFFER, stoi(s_setting[19]));
	else
		Line_set_buffer_size(LINE_SEND_FS_CACHE_BUFFER, 0x200000);

	if (std::all_of(s_setting[20].cbegin(), s_setting[20].cend(), isdigit) && !(s_setting[20] == "") && stoi(s_setting[20]) >= 0x40000 && stoi(s_setting[20]) <= 0x1400000)
		Line_set_buffer_size(LINE_SEND_FS_BUFFER, stoi(s_setting[20]));
	else
		Line_set_buffer_size(LINE_SEND_FS_BUFFER, 0x500000);


	Draw_progress("2/3 [Main] Starting threads...");
	Hid_init();
	Expl_init();

	Draw_progress("3/3 [Main] Loading textures...");
	/*init_log_num_return = Log_log_save("Main/Init/c2d", "Loading texture (background.t3x)...", 1234567890, s_debug_slow);
	init_result = Draw_load_texture("romfs:/gfx/background.t3x", 0, Background_image, 0, 2);
	Log_log_add(init_log_num_return, init_result.string, init_result.code, s_debug_slow);*/

	init_log_num_return = Log_log_save("Main/Init/c2d", "Loading texture (wifi_signal.t3x)...", 1234567890, s_debug_slow);
	init_result = Draw_load_texture("romfs:/gfx/wifi_signal.t3x", 1, Wifi_icon_image, 0, 9);
	Log_log_add(init_log_num_return, init_result.string, init_result.code, s_debug_slow);

	init_log_num_return = Log_log_save("Main/Init/c2d", "Loading texture (battery_level.t3x)...", 1234567890, s_debug_slow);
	init_result = Draw_load_texture("romfs:/gfx/battery_level.t3x", 2, Battery_level_icon_image, 0, 21);
	Log_log_add(init_log_num_return, init_result.string, init_result.code, s_debug_slow);

	init_log_num_return = Log_log_save("Main/Init/c2d", "Loading texture (battery_charge.t3x)...", 1234567890, s_debug_slow);
	init_result = Draw_load_texture("romfs:/gfx/battery_charge.t3x", 3, Battery_charge_icon_image, 0, 1);
	Log_log_add(init_log_num_return, init_result.string, init_result.code, s_debug_slow);

	init_log_num_return = Log_log_save("Main/Init/c2d", "Loading texture (square.t3x)...", 1234567890, s_debug_slow);
	init_result = Draw_load_texture("romfs:/gfx/square.t3x", 4, Square_image, 0, 1);
	Log_log_add(init_log_num_return, init_result.string, init_result.code, s_debug_slow);
	
	/*init_log_num_return = Log_log_save("Main/Init/c2d", "Loading texture (sem_help.t3x)...", 1234567890, true);
	init_result = Draw_load_texture("romfs:/gfx/sem_help.t3x", 51, sem_help_image, 0, 7);
	Log_log_add(init_log_num_return, init_result.string, init_result.code, true);*/
	dammy_tint.corners[0].color = 56738247;
	if (s_night_mode)
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
	svcSetThreadPriority(CUR_THREAD_HANDLE, 0x26);

	free(init_buffer);
	Log_log_save("Main/Init", "Initialized.", 1234567890, false);
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

	int exit_app_log_num_return;
	Handle exit_fs_handle = 0;
	FS_Archive exit_fs_archive = 0;
	Result_with_string exit_result;

	exit_app_log_num_return = Log_log_save("Main/Fs", "Share_save_to_file(Setting.txt)...", 1234567890, s_debug_slow);
	s_setting[2] = std::to_string(s_lcd_brightness);
	s_setting[3] = std::to_string(s_time_to_enter_afk);
	s_setting[4] = std::to_string(s_afk_lcd_brightness);
	if (s_system_setting_menu_show)
		s_setting[5] = "true";
	else
		s_setting[5] = "false";
	s_setting[6] = std::to_string(s_scroll_speed);
	if (s_allow_send_app_info)
		s_setting[7] = "allow";
	else
		s_setting[7] = "deny";

	s_setting[8] = std::to_string(s_num_of_app_start + 1);

	if (s_night_mode)
		s_setting[9] = "true";
	else
		s_setting[9] = "false";

	if (s_draw_vsync_mode)
		s_setting[10] = "true";
	else
		s_setting[10] = "false";

	if (Line_query_setting(LINE_HIDE_ID))
		s_setting[11] = "true";
	else
		s_setting[11] = "false";

	s_setting[12] = std::to_string(Line_query_x_y_size_interval(LINE_TEXT_SIZE));
	s_setting[13] = std::to_string(Line_query_x_y_size_interval(LINE_TEXT_INTERVAL));
	s_setting[14] = std::to_string(Line_query_buffer_size(LINE_HTTPC_BUFFER));
	s_setting[15] = std::to_string(Line_query_buffer_size(LINE_FS_BUFFER));
	s_setting[16] = std::to_string(Spt_query_buffer_size(SPT_HTTPC_BUFFER));
	s_setting[17] = std::to_string(Imv_query_buffer_size(IMV_HTTPC_BUFFER));
	s_setting[18] = std::to_string(Imv_query_buffer_size(IMV_FS_BUFFER));
	s_setting[19] = std::to_string(Line_query_buffer_size(LINE_SEND_FS_CACHE_BUFFER));
	s_setting[20] = std::to_string(Line_query_buffer_size(LINE_SEND_FS_BUFFER));
	s_setting[0] = "";

	for (int i = 0; i <= 19; i++)
		s_setting[0] += "<" + std::to_string(i) + ">" + s_setting[i + 1] + "</" + std::to_string(i) + ">";

	exit_result = Share_save_to_file("Setting.txt", (u8*)s_setting[0].c_str(), s_setting[0].length(), "/Line/", true, exit_fs_handle, exit_fs_archive);
	Log_log_add(exit_app_log_num_return, exit_result.string, exit_result.code, s_debug_slow);

	Log_log_save("Main/Svc", "amExit...", 1234567890, s_debug_slow);
	amExit();
	Log_log_save("Main/Svc", "aptExit...", 1234567890, s_debug_slow);
	aptExit();
	Log_log_save("Main/Svc", "acExit...", 1234567890, s_debug_slow);
	acExit();
	Log_log_save("Main/Svc", "mcuExit...", 1234567890, s_debug_slow);
	mcuHwcExit();
	Log_log_save("Main/Svc", "ptmuExit...", 1234567890, s_debug_slow);
	ptmuExit();
	Log_log_save("Main/Svc", "nsExit...", 1234567890, s_debug_slow);
	nsExit();
	Log_log_save("Main/Svc", "httpcExit...", 1234567890, s_debug_slow);
	httpcExit();
	Log_log_save("Main/Svc", "fsExit...", 1234567890, s_debug_slow);
	fsExit();
	Log_log_save("Main/Svc", "", 1234567890, s_debug_slow);
	Draw_exit();
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	return 0;
}