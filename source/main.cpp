#include <3ds.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include "citro2d.h"

#include "draw.hpp"
#include "file.hpp"
#include "change_setting.hpp"
#include "share_function.hpp"
#include "speedtest.hpp"
#include "image_viewer.hpp"
#include "line.hpp"
#include "setting_menu.hpp"
#include "google_translation.hpp"

float text_red;
float text_green;
float text_blue;
float text_alpha;
std::string main_menu_menu_string[10] = {"Line", "  Google \ntranslation", "    Speed test", "      Image viewer"};

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
	osTickCounterStart(&s_tcount_up_time);
	init_buffer = (u8*)malloc(0x2000);
	memset(init_buffer, 0x0, 0x2000);
	Share_app_log_save("Main/Init", "Initializing...", 1234567890, false);
	Share_app_log_save("Main/Init/ver", s_app_ver, 1234567890, false);

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

	//Init svc
	init_log_num_return = Share_app_log_save("Main/Init", "fsInit...", 1234567890, s_debug_slow);
	init_result.code = fsInit();
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
		s_fs_success = true;
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);


	init_log_num_return = Share_app_log_save("Main/Init", "acInit...", 1234567890, s_debug_slow);
	init_result.code = acInit();
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
		s_ac_success = true;
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);


	init_log_num_return = Share_app_log_save("Main/Init", "aptInit...", 1234567890, s_debug_slow);
	init_result.code = aptInit();
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
		s_apt_success = true;
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);


	init_log_num_return = Share_app_log_save("Main/Init", "mcuInit...", 1234567890, s_debug_slow);
	init_result.code = mcuHwcInit();
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
		s_mcu_success = true;
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);


	init_log_num_return = Share_app_log_save("Main/Init", "ptmuInit...", 1234567890, s_debug_slow);
	init_result.code = ptmuInit();
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
		s_ptmu_success = true;
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);

	init_log_num_return = Share_app_log_save("Main/Init", "httpcInit...", 1234567890, s_debug_slow);
	init_result.code = httpcInit(0x100000);
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
		s_httpc_success = true;
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);


	init_log_num_return = Share_app_log_save("Main/Init", "romfsInit...", 1234567890, s_debug_slow);
	init_result.code = romfsInit();
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
		s_rom_success = true;
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);


	init_log_num_return = Share_app_log_save("Main/Init", "cfguInit...", 1234567890, s_debug_slow);
	init_result.code = cfguInit();
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
		s_cfg_success = true;
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);

	init_log_num_return = Share_app_log_save("Main/Init", "amInit...", 1234567890, s_debug_slow);
	init_result.code = amInit();
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
		s_am_success = true;
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);

	init_log_num_return = Share_app_log_save("Main/Init/apt", "APT_SetAppCpuTimeLimit_30...", 1234567890, s_debug_slow);
	init_result.code = APT_SetAppCpuTimeLimit(30);
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);

	aptSetSleepAllowed(true);

	init_log_num_return = Share_app_log_save("Main/Init/nwm", "Wifi_enable...", 1234567890, s_debug_slow);
	init_result.code = Wifi_enable();
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, s_debug_slow);
	}
	else
		Share_app_log_add_result(init_log_num_return, "[Error] ", init_result.code, s_debug_slow);
	s_wifi_enabled = true;

	init_log_num_return = Share_app_log_save("Main/Init/fs", "Share_load_from_file...", 1234567890, s_debug_slow);
	init_result = Share_load_from_file("Setting.txt", init_buffer, 0x2000, &init_read_size, "/Line/", init_fs_handle, init_fs_archive);

	if (init_result.code == 0)
		s_setting[0] = (char*)init_buffer;

	for (int i = 1; i <= 17; i++)
	{
		init_log_num_return = Share_app_log_save("Main/Init/fs", "setting value" + std::to_string(i) + " : ", 1234567890, s_debug_slow);
		init_setting_parse_start_text = "<" + std::to_string(i - 1) + ">";
		init_setting_parse_start_num = s_setting[0].find(init_setting_parse_start_text);
		init_setting_parse_end_text = "</" + std::to_string(i - 1) + ">";
		init_setting_parse_end_num = s_setting[0].find(init_setting_parse_end_text);

		if (init_setting_parse_end_num == -1 || init_setting_parse_start_num == -1)
		{
			Share_app_log_save("Main/Init/fs", "Failed to load settings. Default values has been applied.", 1234567890, s_debug_slow);

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

			break;
		}
		init_setting_parse_start_num += init_setting_parse_start_text.length();
		init_setting_parse_end_num -= init_setting_parse_start_num;
		s_setting[i] = s_setting[0].substr(init_setting_parse_start_num, init_setting_parse_end_num);
		Share_app_log_add_result(init_log_num_return, s_setting[i], 1234567890, s_debug_slow);
	}

	s_lcd_brightness = stoi(s_setting[2]);
	s_time_to_enter_afk = stoi(s_setting[3]);
	s_afk_lcd_brightness = stoi(s_setting[4]);
	if (s_setting[5] == "true")
		s_system_setting_menu_show = true;
	else
		s_system_setting_menu_show = false;

	s_scroll_speed = stod(s_setting[6]);
	if (s_setting[7] == "allow")
		s_allow_send_app_info = true;
	else
		s_allow_send_app_info = false;

	s_num_of_app_start = stoi(s_setting[8]);
	
	if (s_setting[9] == "true")
		s_night_mode = true;
	else
		s_night_mode = false;

	if (s_setting[10] == "true")
		s_draw_vsync_mode = true;
	else
		s_draw_vsync_mode = false;

	if (s_setting[11] == "true")
		s_line_hide_id = true;
	else
		s_line_hide_id = false;

	text_size_cache = stod(s_setting[12]);
	text_interval_cache = stod(s_setting[13]);
	s_line_log_httpc_buffer_size = stoi(s_setting[14]);
	s_line_log_fs_buffer_size = stoi(s_setting[15]);
	s_spt_spt_httpc_buffer_size = stoi(s_setting[16]);
	s_imv_image_httpc_buffer_size = stoi(s_setting[17]);

	if (text_size_cache > 3.0)
		text_size_cache = 3.0;
	else if (text_size_cache < 0.25)
		text_size_cache = 0.25;
	if (text_interval_cache > 250)
		text_interval_cache = 250;
	else if (text_interval_cache < 10)
		text_interval_cache = 10;
	if (s_line_log_httpc_buffer_size > 0x1000000)
		s_line_log_httpc_buffer_size = 0x1000000;
	else if (s_line_log_httpc_buffer_size < 0x100000)
		s_line_log_httpc_buffer_size = 0x100000;
	if (s_line_log_fs_buffer_size > 0x1000000)
		s_line_log_fs_buffer_size = 0x1000000;
	else if (s_line_log_fs_buffer_size < 0x100000)
		s_line_log_fs_buffer_size = 0x100000;
	if (s_spt_spt_httpc_buffer_size > 0x1000000)
		s_spt_spt_httpc_buffer_size = 0x1000000;
	else if (s_spt_spt_httpc_buffer_size < 0x100000)
		s_spt_spt_httpc_buffer_size = 0x100000;
	if (s_imv_image_httpc_buffer_size > 0x1000000)
		s_imv_image_httpc_buffer_size = 0x1000000;
	else if (s_imv_image_httpc_buffer_size < 0x100000)
		s_imv_image_httpc_buffer_size = 0x100000;

	svcSetThreadPriority(CUR_THREAD_HANDLE, 0x26);

	s_update_thread_run = true;
	s_hid_thread_run = true;
	s_connect_test_thread_run = true;

	s_update_thread = threadCreate(Share_update_thread, (void*)(""), STACKSIZE, 0x24, -1, true);
	s_hid_thread = threadCreate(Share_scan_hid_thread, (void*)(""), STACKSIZE, 0x25, -1, true);
	s_connect_test_thread = threadCreate(Share_connectivity_check_thread, (void*)(""), STACKSIZE, 0x30, -1, true);	

	init_log_num_return = Share_app_log_save("Main/Init/c2d", "Loading texture (background.t3x)...", 1234567890, s_debug_slow);
	init_result = Draw_load_texture("romfs:/gfx/background.t3x", 0, Background_image, 0, 2);
	Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, s_debug_slow);

	init_log_num_return = Share_app_log_save("Main/Init/c2d", "Loading texture (wifi_signal.t3x)...", 1234567890, s_debug_slow);
	init_result = Draw_load_texture("romfs:/gfx/wifi_signal.t3x", 1, Wifi_icon_image, 0, 9);
	Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, s_debug_slow);

	init_log_num_return = Share_app_log_save("Main/Init/c2d", "Loading texture (battery_level.t3x)...", 1234567890, s_debug_slow);
	init_result = Draw_load_texture("romfs:/gfx/battery_level.t3x", 2, Battery_level_icon_image, 0, 21);
	Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, s_debug_slow);

	init_log_num_return = Share_app_log_save("Main/Init/c2d", "Loading texture (battery_charge.t3x)...", 1234567890, s_debug_slow);
	init_result = Draw_load_texture("romfs:/gfx/battery_charge.t3x", 3, Battery_charge_icon_image, 0, 1);
	Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, s_debug_slow);

	init_log_num_return = Share_app_log_save("Main/Init/c2d", "Loading texture (common.t3x)...", 1234567890, s_debug_slow);
	init_result = Draw_load_texture("romfs:/gfx/common.t3x", 4, Square_image, 0, 14);
	Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, s_debug_slow);

	/*init_log_num_return = Share_app_log_save("Main/Init/c2d", "Loading texture (sem_help.t3x)...", 1234567890, true);
	init_result = Draw_load_texture("romfs:/gfx/sem_help.t3x", 51, sem_help_image, 0, 7);
	Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, true);*/

	for (int i = 0; i < 46; i++)
		s_use_external_font[i] = false;

	wifi_state = (u8*)malloc(0x1);
	memset(wifi_state, 0xff, 0x1);
	wifi_state_internet_sample = (u8*)malloc(0x1);
	memset(wifi_state_internet_sample, 0x2, 0x1);

	dammy_tint.corners[0].color = 56738247;
	if(s_night_mode)
		C2D_PlainImageTint(&texture_tint, C2D_Color32f(1.0, 1.0, 1.0, 0.75), true);
	else
		C2D_PlainImageTint(&texture_tint, C2D_Color32f(0.0, 0.0, 0.0, 1.0), true);

	if (s_allow_send_app_info)
	{
		for (int i = 1; i <= 1000; i++)
		{
			if (s_num_of_app_start == i * 10)
			{
				s_send_app_info_thread = threadCreate(Share_send_app_info_thread, (void*)(""), STACKSIZE, 0x30, -1, true);
				break;
			}
		}
	}
	free(init_buffer);

	Share_app_log_save("Main/Init", "Initialized.", 1234567890, false);
}

int main()
{
	Init();

	// Main loop
	while (aptMainLoop())
	{
		s_hid_disabled = false;
		s_app_log_x = app_log_x_cache;
		s_app_log_view_num = s_app_log_view_num_cache;
		Share_get_system_info();
		sprintf(s_status, "%dfps %.1fms %02d/%02d %02d:%02d:%02d "
			, s_fps_show, s_frame_time, s_months, s_days, s_hours, s_minutes, s_seconds);
		
		if (s_menu_main_run)
		{
			if (s_night_mode)
			{
				text_red = 1.0f;
				text_green = 1.0f;
				text_blue = 1.0f;
				text_alpha = 0.75f;
			}
			else
			{
				text_red = 0.0f;
				text_green = 0.0f;
				text_blue = 0.0f;
				text_alpha = 1.0f;
			}

			Draw_set_draw_mode(s_draw_vsync_mode);
			if (s_night_mode)
				Draw_screen_ready_to_draw(0, true, 1, 0.0, 0.0, 0.0);
			else
				Draw_screen_ready_to_draw(0, true, 1, 1.0, 1.0, 1.0);

			Draw_texture(Background_image, dammy_tint, 0, 0.0, 0.0, 400.0, 15.0);
			Draw_texture(Wifi_icon_image, dammy_tint, s_wifi_signal, 360.0, 0.0, 15.0, 15.0);
			Draw_texture(Battery_level_icon_image, dammy_tint, s_battery_level / 5, 330.0, 0.0, 30.0, 15.0);
			if(s_battery_charge)
				Draw_texture(Battery_charge_icon_image, dammy_tint, 0, 310.0, 0.0, 20.0, 15.0);
			Draw(s_status, 0.0f, 0.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 1.0f);
			Draw(s_battery_level_string, 337.5f, 1.25f, 0.4f, 0.4f, 0.0f, 0.0f, 0.0f, 0.5f);

			if (s_debug_mode)
			{
				Draw_texture(Square_image, dammy_tint, 9, 0.0, 30.0, 230.0, 150.0);
				Draw("Key A press : " + std::to_string(s_key_A_press) + " Key A held : " + std::to_string(s_key_A_held), 0.0, 30.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
				Draw("Key B press : " + std::to_string(s_key_B_press) + " Key B held : " + std::to_string(s_key_B_held), 0.0, 40.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
				Draw("Key X press : " + std::to_string(s_key_X_press) + " Key X held : " + std::to_string(s_key_X_held), 0.0, 50.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
				Draw("Key Y press : " + std::to_string(s_key_Y_press) + " Key Y held : " + std::to_string(s_key_Y_held), 0.0, 60.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
				Draw("Key CPAD DOWN held : " + std::to_string(s_key_CPAD_DOWN_held), 0.0, 70.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
				Draw("Key CPAD RIGHT held : " + std::to_string(s_key_CPAD_RIGHT_held), 0.0, 80.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
				Draw("Key CPAD UP held : " + std::to_string(s_key_CPAD_UP_held), 0.0, 90.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
				Draw("Key CPAD LEFT held : " + std::to_string(s_key_CPAD_LEFT_held), 0.0, 100.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
				Draw("Touch pos x : " + std::to_string(s_touch_pos_x) + " Touch pos y : " + std::to_string(s_touch_pos_y), 0.0, 110.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
				Draw("X moved value : " + std::to_string(s_touch_pos_x_moved) + " Y moved value : " + std::to_string(s_touch_pos_y_moved), 0.0, 120.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
				Draw("Held time : " + std::to_string(s_held_time), 0.0, 130.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
				Draw("Drawing time(CPU/per frame) : " + std::to_string(C3D_GetProcessingTime()) + "ms", 0.0, 140.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
				Draw("Drawing time(GPU/per frame) : " + std::to_string(C3D_GetDrawingTime()) + "ms", 0.0, 150.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
				Draw("Free RAM (estimate) " + std::to_string(s_free_ram) + " MB", 0.0f, 160.0f, 0.4f, 0.4, text_red, text_green, text_blue, text_alpha);
				Draw("Free linear RAM (estimate) " + std::to_string(s_free_linear_ram) + " MB", 0.0f, 170.0f, 0.4f, 0.4, text_red, text_green, text_blue, text_alpha);
			}


			if (s_night_mode)
				Draw_screen_ready_to_draw(1, true, 1, 0.0, 0.0, 0.0);
			else
				Draw_screen_ready_to_draw(1, true, 1, 1.0, 1.0, 1.0);
		
			for (int i = 0; i <= 3; i++)
				Draw_texture(Square_image, dammy_tint, 11, (80.0 * i), 0.0, 60.0, 60.0);
			if (s_app_logs_show)
			{
				for (int i = 0; i < 23; i++)
					Draw(s_app_logs[s_app_log_view_num + i], s_app_log_x, 10.0f + (i * 10), 0.4f, 0.4f, 0.0f, 0.5f, 1.0f, 1.0f);
			}
			Draw(main_menu_menu_string[0], 20.0, 20.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw(main_menu_menu_string[1], 85.0, 15.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw(main_menu_menu_string[2], 150.0, 20.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw(main_menu_menu_string[3], 215.0, 20.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);


			Draw("■", 252.5, 162.5, 3.0, 3.0, 0.0, 0.0, 0.0, 0.25);
			Draw("Setting", 270.0, 205.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			
			if(s_line_already_init)
				Draw("X", 50.0, 0.0, 0.65, 0.5, 1.0, 0.0, 0.0, 0.5);

			if (s_imv_already_init)
				Draw("X", 290.0, 0.0, 0.65, 0.5, 1.0, 0.0, 0.0, 0.5);

			if (s_error_display)
				Share_draw_error();

			Draw_texture(Background_image, dammy_tint, 1, 0.0, 225.0, 320.0, 15.0);
			Draw(s_bot_button_string[1], 30.0f, 220.0f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 1.0f);

			if (s_key_touch_held)
				Draw(s_circle_string, s_touch_pos_x, s_touch_pos_y, 0.20f, 0.20f, 1.0f, 0.0f, 0.0f, 1.0f);
			s_fps += 1;
			
			Draw_apply_draw();
			osTickCounterUpdate(&s_tcount_frame_time);
			s_frame_time = osTickCounterRead(&s_tcount_frame_time);	


			if (s_key_SELECT_press)
			{
				if (s_app_logs_show)
					s_app_logs_show = false;
				else
					s_app_logs_show = true;
			}
			else if (s_key_START_press || (s_key_touch_press && s_touch_pos_x >= 110 && s_touch_pos_x <= 230 && s_touch_pos_y >= 220 && s_touch_pos_y <= 240))
			{
				if (Share_exit_check())
					break;
			}
			else if (s_key_touch_press)
			{				
				if (s_touch_pos_x > 50 && s_touch_pos_x < 60 && s_touch_pos_y > 0 && s_touch_pos_y < 15 && s_line_already_init)
					Line_exit();
				else if (s_touch_pos_x > 0 && s_touch_pos_x < 60 && s_touch_pos_y > 0 && s_touch_pos_y < 60)
				{
					if(!s_line_already_init)
						Line_init();
					else
					{
						s_line_thread_suspend = false;
						s_line_main_run = true;
						s_menu_main_run = false;
					}
				}
				else if (s_touch_pos_x > 80 && s_touch_pos_x < 140 && s_touch_pos_y > 0 && s_touch_pos_y < 60)
				{
					s_gtr_thread_suspend = false;
					s_gtr_main_run = true;
					s_menu_main_run = false;
					if (!s_gtr_already_init)
						Google_tr_init();
				}
				else if (s_touch_pos_x > 160 && s_touch_pos_x < 220 && s_touch_pos_y > 0 && s_touch_pos_y < 60)
				{
					s_spt_thread_suspend = false;
					s_spt_main_run = true;
					s_menu_main_run = false;
					if (!s_spt_already_init)
						Speedtest_init();
				}
				else if (s_touch_pos_x > 290 && s_touch_pos_x < 300 && s_touch_pos_y > 0 && s_touch_pos_y < 15 && s_imv_already_init)
					Image_viewer_exit();
				else if (s_touch_pos_x > 240 && s_touch_pos_x < 300 && s_touch_pos_y > 0 && s_touch_pos_y < 60)
				{
					if (!s_imv_already_init)
						Image_viewer_init();
					else
					{
						s_imv_thread_suspend = false;
						s_imv_main_run = true;
						s_menu_main_run = false;
					}
				}
				else if (s_touch_pos_x > 250 && s_touch_pos_x < 320 && s_touch_pos_y > 175 && s_touch_pos_y < 240)
				{
					s_sem_main_run = true;
					s_menu_main_run = false;

					if(!s_sem_already_init)
						Setting_menu_init();
				}
			}
		}
		else if(s_line_main_run)
			Line_main();
		else if (s_gtr_main_run)
			Google_translation_main();
		else if(s_sem_main_run)
			Setting_menu_main();
		else if (s_spt_main_run)
			Speedtest_main();
		else if (s_imv_main_run)
			Image_viewer_main();

		Share_key_flag_reset();
		s_hid_disabled = true;
	}

	if (s_line_already_init)
		Line_exit();
	if (s_imv_already_init)
		Image_viewer_exit();
	if (s_sem_already_init)
		Setting_menu_exit();

	u64 time_out = 5000000000; //5s
	int exit_app_log_num_return;
	Handle exit_fs_handle = 0;
	FS_Archive exit_fs_archive = 0;
	Result_with_string exit_result;

	s_spt_thread_run = false;
	s_gtr_tr_thread_run = false;
	s_update_thread_run = false;
	s_hid_thread_run = false;
	s_connect_test_thread_run = false;

	exit_app_log_num_return = Share_app_log_save("Main/Fs", "Share_save_to_file(Setting.txt)...", 1234567890, s_debug_slow);
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

	if (s_line_hide_id)
		s_setting[11] = "true";
	else
		s_setting[11] = "false";

	s_setting[12] = std::to_string(text_size_cache);
	s_setting[13] = std::to_string(text_interval_cache);
	s_setting[14] = std::to_string(s_line_log_httpc_buffer_size);
	s_setting[15] = std::to_string(s_line_log_fs_buffer_size);
	s_setting[16] = std::to_string(s_spt_spt_httpc_buffer_size);
	s_setting[17] = std::to_string(s_imv_image_httpc_buffer_size);

	s_setting[0] = "";

	for (int i = 0; i < 18; i++)
		s_setting[0] += "<" + std::to_string(i) + ">" + s_setting[i + 1] + "</" + std::to_string(i) + ">";

	exit_result = Share_save_to_file("Setting.txt", (u8*)s_setting[0].c_str(), s_setting[0].length(), "/Line/", true, exit_fs_handle, exit_fs_archive);
	Share_app_log_add_result(exit_app_log_num_return, exit_result.string, exit_result.code, s_debug_slow);

	//exit
	Share_app_log_save("Main", "Exiting...", 1234567890, s_debug_slow);

	Share_app_log_add_result(exit_app_log_num_return, "", exit_result.code, s_debug_slow);
	exit_app_log_num_return = Share_app_log_save("Main/Thread", "Update thread exit...", 1234567890, s_debug_slow);
	exit_result.code = threadJoin(s_update_thread, time_out);
	exit_app_log_num_return = Share_app_log_save("Main/Thread", "Scan hid thread exit...", 1234567890, s_debug_slow);
	exit_result.code = threadJoin(s_hid_thread, time_out);
	Share_app_log_add_result(exit_app_log_num_return, "", exit_result.code, s_debug_slow);
	exit_app_log_num_return = Share_app_log_save("Main/Thread", "Connect test thread exit...", 1234567890, s_debug_slow);
	exit_result.code = threadJoin(s_connect_test_thread, time_out);
	Share_app_log_add_result(exit_app_log_num_return, "", exit_result.code, s_debug_slow);

	Share_app_log_save("Main/Svc", "amExit...", 1234567890, s_debug_slow);
	amExit();
	Share_app_log_save("Main/Svc", "aptExit...", 1234567890, s_debug_slow);
	aptExit();
	Share_app_log_save("Main/Svc", "acExit...", 1234567890, s_debug_slow);
	acExit();
	Share_app_log_save("Main/Svc", "mcuExit...", 1234567890, s_debug_slow);
	mcuHwcExit();
	Share_app_log_save("Main/Svc", "ptmuExit...", 1234567890, s_debug_slow);
	ptmuExit();
	Share_app_log_save("Main/Svc", "nsExit...", 1234567890, s_debug_slow);
	nsExit();
	Share_app_log_save("Main/Svc", "httpcExit...", 1234567890, s_debug_slow);
	httpcExit();
	Share_app_log_save("Main/Svc", "fsExit...", 1234567890, s_debug_slow);
	fsExit();
	Share_app_log_save("Main/Svc", "", 1234567890, s_debug_slow);
	Draw_exit();
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	return 0;
}