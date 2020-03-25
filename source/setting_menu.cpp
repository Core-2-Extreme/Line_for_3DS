#include <string>
#include <cstring>
#include <unistd.h>
#include <3ds.h>

#include "draw.hpp"
#include "file.hpp"
#include "setting_menu.hpp"
#include "share_function.hpp"
#include "httpc.hpp"

int update_progress = -1;
int check_update_progress = 0;

bool setting_main_run = false;
bool new_version_available = false;
bool need_gas_update = false;

std::string setting_string[96];
std::string setting_help_string[64];
std::string newest_ver_data[11];
std::string sem_message_en[87] = {
	"Language : ",
	"Night mode : ",
	"",
	"",
	"Vsync : ",
	"",
	"",
	"",
	"",
	"",
	"",
	"Screen brightness : ",
	"",
	"",
	"",
	"",
	"",
	"",
	"Time to enter sleep mode : ",
	"s",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"Screen brightness when sleep : ",
	"",
	"",
	"Scroll speed : ",
	"Allow send app info : ",
	"Debug mode : ",
	"Font : ",
	"English",
	"日本語",
	"off",
	"on",
	"deny",
	"allow",
	"Flash",
	"OK",
	"NG",
	"Default",
	"External",
	"System specific",
	"JPN",
	"CHN",
	"KOR",
	"TWN",
	"Load all",
	"Unload all",
	"* Only 124MB and 178MB of usable RAM are New 3DS's.",
	"* The .3dsx edition usually has 64MB of usable RAM on",
	"the Old 3DS, and 124MB of usable RAM on the New 3DS.",
	"(It may depend on what exploit point you are using(?))",
	"File will be saved to ",
	"Google app script update isn't required.",
	"Google app script update is required.",
	".3dsx", 
	".cia  32MB of usable RAM" , 
	".cia  64MB of usable RAM" , 
	".cia  72MB of usable RAM" ,
	".cia  80MB of usable RAM" , 
	".cia  96MB of usable RAM" , 
	".cia  124MB of usable RAM" ,
	".cia  178MB of usable RAM", 
	"(not available)",
	"Downloading...",
	"Installing...",
	"Success",
	"Failed",
	"Please restart the app to apply updates.",
	"Up to date",
	"New version available",
	"Check for updates",
	"Checking...",
	"Check failed.",
	"Go back (B)",
	"Go to editions select (A)",
	"Download & install (X)",
	"Line log httpc buffer size : ",
	"Line log fs buffer size : ",
	"Spt spt httpc buffer size : ",
	"Imv image httpc buffer size : ",
	"Increase",
	"Decrease",
}; 
std::string sem_message_jp[87] = {
	"言語 : ",
	"夜モード : ",
	"",
	"",
	"垂直同期 : ",
	"",
	"",
	"",
	"",
	"",
	"",
	"画面光度 : ",
	"",
	"",
	"",
	"",
	"",
	"",
	"スリープモードへ入るまでの時間 : ",
	"秒",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"スリープモード時の画面光度 : ",
	"",
	"",
	"スクロール速度 : ",
	"アプリ情報送信を許可する : ",
	"デバッグモード : ",
	"フォント : ",
	"English",
	"日本語",
	"オフ",
	"オン",
	"拒否",
	"許可",
	"点滅",
	"OK",
	"NG",
	"デフォルト",
	"外部",
	"システム指定",
	"日本",
	"中国",
	"韓国",
	"台湾",
	"全て読み込み",
	"全て廃棄",
	"* 124MBと178MBのRAMを使用可能はNew 3DSのみです。",
	"* .3dsx版は普通、Old 3DS上では64MBのRAMを使用可能、",
	"New 3DS上では124MBのRAMを使用可能です。",
	"(何のexploit pointを使用しているかに依存します(?))",
	"ファイルは以下に保存されます。",
	"Google app scriptの更新は不要です。",
	"Google app scriptの更新が必要です。",
	".3dsx",
	".cia  32MBのRAMを使用可能" ,
	".cia  64MBのRAMを使用可能" ,
	".cia  72MBのRAMを使用可能" ,
	".cia  80MBのRAMを使用可能" ,
	".cia  96MBのRAMを使用可能" ,
	".cia  124MBのRAMを使用可能" ,
	".cia  178MBのRAMを使用可能",
	"(利用不可)",
	"ダウンロード中...",
	"インストール中...",
	"成功",
	"失敗",
	"更新を適用するためにアプリを再起動してください。",
	"最新です。",
	"新バージョンが利用可能です。",
	"更新確認",
	"確認中...",
	"確認失敗。",
	"戻る (B)",
	"エディション選択へ (A)",
	"ダウンロードとインストール (X)",
	"Line log httpc bufferのサイズ : ",
	"Line log fs bufferのサイズ : ",
	"Spt spt httpc bufferのサイズ : ",
	"Imv image httpc bufferのサイズ : ",
	"増やす",
	"減らす",
};

Thread check_update_thread;

void Setting_menu_init(void)
{
	s_update_check_thread_run = true;
	check_update_thread = threadCreate(Setting_menu_update_check, (void*)(""), STACKSIZE, 0x28, -1, true);
	s_sem_already_init = true;
}

void Setting_menu_main(void)
{
	float text_red;
	float text_green;
	float text_blue;
	float text_alpha;
	float draw_x;
	float draw_y;
	int texture_color;

	osTickCounterUpdate(&s_tcount_frame_time);

	if (s_setting[1] == "en")
	{
		setting_string[0] = sem_message_en[0] + sem_message_en[34];
		setting_string[1] = sem_message_en[1] + sem_message_en[36 + s_night_mode];
		setting_string[2] = sem_message_en[4] + sem_message_en[36 + s_draw_vsync_mode];
		setting_string[3] = sem_message_en[11] + std::to_string(s_lcd_brightness);
		setting_string[4] = sem_message_en[18] + std::to_string(s_time_to_enter_afk / 10) + sem_message_en[19];
		setting_string[5] = sem_message_en[27] + std::to_string(s_afk_lcd_brightness);
		setting_string[6] = sem_message_en[30] + std::to_string(s_scroll_speed);
		setting_string[7] = sem_message_en[31] + sem_message_en[38 + s_allow_send_app_info];
		setting_string[8] = sem_message_en[32] + sem_message_en[36 + s_debug_mode];

		if (!s_use_external_font[0] && !s_use_specific_system_font)
			setting_string[9] = sem_message_en[33] + sem_message_en[43];
		else  if (s_use_external_font[0])
			setting_string[9] = sem_message_en[33] + sem_message_en[44];
		else  if (s_use_specific_system_font)
			setting_string[9] = sem_message_en[33] + sem_message_en[45];

		setting_string[10] = sem_message_en[45];
		setting_string[11] = sem_message_en[44];

		for (int i = 20; i < 27; i++)
			setting_string[i] = sem_message_en[i + 14];

		for (int i = 27; i < 41; i++)
			setting_string[i] = sem_message_en[i + 16];
		
		if(need_gas_update)
			setting_string[41] = sem_message_en[58];
		else
			setting_string[41] = sem_message_en[57];

		for (int i = 42; i < 51; i++)
			setting_string[i] = sem_message_en[i + 17];

		setting_string[51] = sem_message_en[68 + update_progress];
		setting_string[52] = sem_message_en[72];
		setting_string[53] = sem_message_en[73 + new_version_available];

		for (int i = 54; i < 60; i++)
			setting_string[i] = sem_message_en[i + 21];	
		
		setting_string[60] = sem_message_en[81] + std::to_string(s_line_log_httpc_buffer_size / 1024 / 1024) + "MB";
		setting_string[61] = sem_message_en[82] + std::to_string(s_line_log_fs_buffer_size / 1024 / 1024) + "MB";
		setting_string[62] = sem_message_en[83] + std::to_string(s_spt_spt_httpc_buffer_size / 1024 / 1024) + "MB";
		setting_string[63] = sem_message_en[84] + std::to_string(s_imv_image_httpc_buffer_size / 1024 / 1024) + "MB";
		setting_string[64] = sem_message_en[85];
		setting_string[65] = sem_message_en[86];

		setting_help_string[0] = sem_message_en[1];
		setting_help_string[1] = sem_message_en[2];
		setting_help_string[2] = sem_message_en[3];
		setting_help_string[3] = sem_message_en[36];
		setting_help_string[4] = sem_message_en[37];

		for (int i = 5; i < 19; i++)
			setting_help_string[i] = sem_message_en[i - 1];

		setting_help_string[19] = sem_message_en[41];
		setting_help_string[20] = sem_message_en[42];
		setting_help_string[21] = sem_message_en[18];

		for (int i = 22; i < 32; i++)
			setting_help_string[i] = sem_message_en[i - 2];
	}
	else if (s_setting[1] == "jp")
	{
		setting_string[0] = sem_message_jp[0] + sem_message_jp[34];
		setting_string[1] = sem_message_jp[1] + sem_message_jp[36 + s_night_mode];
		setting_string[2] = sem_message_jp[4] + sem_message_jp[36 + s_draw_vsync_mode];
		setting_string[3] = sem_message_jp[11] + std::to_string(s_lcd_brightness);
		setting_string[4] = sem_message_jp[18] + std::to_string(s_time_to_enter_afk / 10) + sem_message_jp[19];
		setting_string[5] = sem_message_jp[27] + std::to_string(s_afk_lcd_brightness);
		setting_string[6] = sem_message_jp[30] + std::to_string(s_scroll_speed);
		setting_string[7] = sem_message_jp[31] + sem_message_jp[38 + s_allow_send_app_info];
		setting_string[8] = sem_message_jp[32] + sem_message_jp[36 + s_debug_mode];

		if (!s_use_external_font[0] && !s_use_specific_system_font)
			setting_string[9] = sem_message_jp[33] + sem_message_jp[43];
		else  if (s_use_external_font[0])
			setting_string[9] = sem_message_jp[33] + sem_message_jp[44];
		else  if (s_use_specific_system_font)
			setting_string[9] = sem_message_jp[33] + sem_message_jp[45];

		setting_string[10] = sem_message_jp[45];
		setting_string[11] = sem_message_jp[44];

		for (int i = 20; i < 27; i++)
			setting_string[i] = sem_message_jp[i + 14];

		for (int i = 27; i < 41; i++)
			setting_string[i] = sem_message_jp[i + 16];

		if (need_gas_update)
			setting_string[41] = sem_message_jp[58];
		else
			setting_string[41] = sem_message_jp[57];

		for (int i = 42; i < 51; i++)
			setting_string[i] = sem_message_jp[i + 17];

		setting_string[51] = sem_message_jp[68 + update_progress];
		setting_string[52] = sem_message_jp[72];
		setting_string[53] = sem_message_jp[73 + new_version_available];

		for (int i = 54; i < 60; i++)
			setting_string[i] = sem_message_jp[i + 21];

		setting_string[60] = sem_message_jp[81] + std::to_string(s_line_log_httpc_buffer_size / 1024 / 1024) + "MB";
		setting_string[61] = sem_message_jp[82] + std::to_string(s_line_log_fs_buffer_size / 1024 / 1024) + "MB";
		setting_string[62] = sem_message_jp[83] + std::to_string(s_spt_spt_httpc_buffer_size / 1024 / 1024) + "MB";
		setting_string[63] = sem_message_jp[84] + std::to_string(s_imv_image_httpc_buffer_size / 1024 / 1024) + "MB";
		setting_string[64] = sem_message_jp[85];
		setting_string[65] = sem_message_jp[86];

		setting_help_string[0] = sem_message_jp[1];
		setting_help_string[1] = sem_message_jp[2];
		setting_help_string[2] = sem_message_jp[3];
		setting_help_string[3] = sem_message_jp[36];
		setting_help_string[4] = sem_message_jp[37];

		for (int i = 5; i < 19; i++)
			setting_help_string[i] = sem_message_jp[i - 1];

		setting_help_string[19] = sem_message_jp[41];
		setting_help_string[20] = sem_message_jp[42];
		setting_help_string[21] = sem_message_jp[18];

		for (int i = 22; i < 32; i++)
			setting_help_string[i] = sem_message_jp[i - 2];
	}

	if (s_night_mode)
	{
		text_red = 1.0;
		text_green = 1.0;
		text_blue = 1.0;
		text_alpha = 0.75;
		texture_color = 12;
	}
	else
	{
		text_red = 0.0;
		text_green = 0.0;
		text_blue = 0.0;
		text_alpha = 1.0;
		texture_color = 0;
	}

	Draw_set_draw_mode(s_draw_vsync_mode);
	if (s_night_mode)
		Draw_screen_ready_to_draw(0, true, 1, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(0, true, 1, 1.0, 1.0, 1.0);

	Draw_texture(Background_image, dammy_tint, 0, 0.0, 0.0, 400.0, 15.0);
	Draw_texture(Wifi_icon_image, dammy_tint, s_wifi_signal, 360.0, 0.0, 15.0, 15.0);
	Draw_texture(Battery_level_icon_image, dammy_tint, s_battery_level / 5, 330.0, 0.0, 30.0, 15.0);
	if (s_battery_charge)
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
	if (s_app_logs_show)
	{
		for (int i = 0; i < 23; i++)
			Draw(s_app_logs[s_app_log_view_num + i], s_app_log_x, 10.0f + (i * 10), 0.4f, 0.4f, 0.0f, 0.5f, 1.0f, 1.0f);
	}

	/*if (s_sem_help_mode_num == 0)
	{
		Draw(setting_help_string[0], 0.0, 15.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[1], 0.0, 30.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[2], 0.0, 40.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[3], 0.0, 80.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[4], 200.0, 80.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw_texture(sem_help_image, dammy_tint, 0, 10.0, 100.0, 180.0, 135.0);
		Draw_texture(sem_help_image, dammy_tint, 1, 210.0, 100.0, 180.0, 135.0);
	}
	else if (s_sem_help_mode_num == 1)
	{
		Draw(setting_help_string[5], 0.0, 15.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[6], 0.0, 30.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[7], 0.0, 40.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[8], 0.0, 50.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[9], 0.0, 70.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[10], 0.0, 80.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[11], 0.0, 90.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[3], 0.0, 130.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[4], 200.0, 130.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw_texture(sem_help_image, dammy_tint, 2, 10.0, 145.0, 180.0, 90.0);
		Draw_texture(sem_help_image, dammy_tint, 3, 210.0, 145.0, 180.0, 90.0);
	}
	else if (s_sem_help_mode_num == 2)
	{
		Draw(setting_help_string[12], 0.0, 15.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[13], 0.0, 30.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[14], 112.5, 40.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[15], 0.0, 110.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[16], 0.0, 120.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[17], 0.0, 130.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[18], 0.0, 140.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[19], 0.0, 170.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[20], 200.0, 170.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw_texture(sem_help_image, dammy_tint, 4, 5.5, 185.0, 189.0, 54.0);
		Draw_texture(sem_help_image, dammy_tint, 5, 205.5, 185.0, 189.0, 54.0);
	}
	else if (s_sem_help_mode_num == 3)
	{
		Draw(setting_help_string[21], 0.0, 15.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[22], 0.0, 30.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[23], 0.0, 40.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[24], 0.0, 50.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[25], 0.0, 60.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[26], 0.0, 70.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[27], 0.0, 80.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[28], 0.0, 90.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		Draw_texture(sem_help_image, dammy_tint, 6, 50.0, 140.0, 300.0, 100.0);
	}
	else if (s_sem_help_mode_num == 4)
	{
		Draw(setting_help_string[29], 0.0, 15.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[30], 0.0, 30.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		Draw(setting_help_string[31], 0.0, 40.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
	}*/

	if (s_night_mode)
		Draw_screen_ready_to_draw(1, true, 1, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(1, true, 1, 1.0, 1.0, 1.0);

	Draw_texture(Square_image, dammy_tint, 0, 312.5, 0.0, 7.5, 15.0);
	Draw_texture(Square_image, dammy_tint, 0, 312.5, 215.0, 7.5, 10.0);
	Draw_texture(Square_image, dammy_tint, 8, 312.5, 15.0 + (195 * (-s_sem_y_offset / 1500.0)), 7.5, 5.0);

	//Check for updates
	draw_y = 15.0;
	Draw_texture(Square_image, dammy_tint, 11, 10.0, draw_y + s_sem_y_offset, 200.0, 20.0);
	Draw(setting_string[54], 10.0, draw_y + s_sem_y_offset - 2.5, 0.75, 0.75, text_red, text_green, text_blue, text_alpha);

	//Language
	draw_y = 40.0;
	Draw(setting_string[0], 0.0, draw_y + s_sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw_texture(Square_image, dammy_tint, 11, 10.0, draw_y + s_sem_y_offset + 15.0, 90.0, 20.0);
	Draw_texture(Square_image, dammy_tint, 11, 110.0, draw_y + s_sem_y_offset + 15.0, 90.0, 20.0);
	if (s_setting[1] == "en")
	{
		Draw(setting_string[20], 10.0, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, 1.0, 0.0, 0.0, 1.0);
		Draw(setting_string[21], 110.0, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, text_red, text_green, text_blue, text_alpha);
	}
	else
	{
		Draw(setting_string[20], 10.0, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, text_red, text_green, text_blue, text_alpha);
		Draw(setting_string[21], 110.0, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, 1.0, 0.0, 0.0, 1.0);
	}

	//Night mode
	draw_y = 80.0;
	Draw(setting_string[1], 0.0, draw_y + s_sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw_texture(Square_image, dammy_tint, 11, 10.0, draw_y + s_sem_y_offset + 15.0, 90.0, 20.0);
	Draw_texture(Square_image, dammy_tint, 11, 110.0, draw_y + s_sem_y_offset + 15.0, 90.0, 20.0);
	Draw_texture(Square_image, dammy_tint, 11, 210.0, draw_y + s_sem_y_offset + 15.0, 40.0, 20.0);
	if (s_night_mode)
	{
		Draw(setting_string[23], 10.0, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, 1.0, 0.0, 0.0, 1.0);
		Draw(setting_string[22], 110.0, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, text_red, text_green, text_blue, text_alpha);
	}
	else
	{
		Draw(setting_string[23], 10.0, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, text_red, text_green, text_blue, text_alpha);
		Draw(setting_string[22], 110.0, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, 1.0, 0.0, 0.0, 1.0);
	}
	if (s_flash_mode)
		Draw(setting_string[26], 210.0, draw_y + s_sem_y_offset + 15.0, 0.5, 0.5, 1.0, 0.0, 0.0, 1.0);
	else
		Draw(setting_string[26], 210.0, draw_y + s_sem_y_offset + 15.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);

	//Vsync
	draw_y = 120.0;
	Draw(setting_string[2], 0.0, draw_y + s_sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw_texture(Square_image, dammy_tint, 11, 10.0, draw_y + s_sem_y_offset + 15.0, 90.0, 20.0);
	Draw_texture(Square_image, dammy_tint, 11, 110.0, draw_y + s_sem_y_offset + 15.0, 90.0, 20.0);
	if (s_draw_vsync_mode)
	{
		Draw(setting_string[23], 10.0, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, 1.0, 0.0, 0.0, 1.0);
		Draw(setting_string[22], 110.0, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, text_red, text_green, text_blue, text_alpha);
	}
	else
	{
		Draw(setting_string[23], 10.0, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, text_red, text_green, text_blue, text_alpha);
		Draw(setting_string[22], 110.0, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, 1.0, 0.0, 0.0, 1.0);
	}

	//Screen brightness
	draw_y = 160.0;
	Draw(setting_string[3], 0.0, draw_y + s_sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw_texture(Square_image, dammy_tint, texture_color, (s_lcd_brightness - 10) * 2, draw_y + s_sem_y_offset + 15.0, 4.0, 20.0);

	//Time to enter sleep mode
	draw_y = 200.0;
	Draw(setting_string[4], 0.0, draw_y + s_sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw_texture(Square_image, dammy_tint, texture_color, (s_time_to_enter_afk / 10), draw_y + s_sem_y_offset + 15.0, 4.0, 20.0);

	//Screen brightness when sleep
	draw_y = 240.0;
	Draw(setting_string[5], 0.0, draw_y + s_sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw_texture(Square_image, dammy_tint, texture_color, (s_afk_lcd_brightness - 10) * 2, draw_y + s_sem_y_offset + 15.0, 4.0, 20.0);

	//Scroll speed
	draw_y = 280.0;
	Draw(setting_string[6], 0.0, draw_y + s_sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw_texture(Square_image, dammy_tint, texture_color, (s_scroll_speed * 300), draw_y + s_sem_y_offset + 15.0, 4.0, 20.0);

	//Allow send app info
	draw_y = 320.0;
	Draw(setting_string[7], 0.0, draw_y + s_sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw_texture(Square_image, dammy_tint, 11, 10.0, draw_y + s_sem_y_offset + 15.0, 90.0, 20.0);
	Draw_texture(Square_image, dammy_tint, 11, 110.0, draw_y + s_sem_y_offset + 15.0, 90.0, 20.0);
	if (s_allow_send_app_info)
	{
		Draw(setting_string[25], 10.0, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, 1.0, 0.0, 0.0, 1.0);
		Draw(setting_string[24], 110.0, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, text_red, text_green, text_blue, text_alpha);
	}
	else
	{
		Draw(setting_string[25], 10.0, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, text_red, text_green, text_blue, text_alpha);
		Draw(setting_string[24], 110.0, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, 1.0, 0.0, 0.0, 1.0);
	}

	//Debug mode
	draw_y = 360.0;
	Draw(setting_string[8], 0.0, draw_y + s_sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw_texture(Square_image, dammy_tint, 11, 10.0, draw_y + s_sem_y_offset + 15.0, 90.0, 20.0);
	Draw_texture(Square_image, dammy_tint, 11, 110.0, draw_y + s_sem_y_offset + 15.0, 90.0, 20.0);
	if (s_debug_mode)
	{
		Draw(setting_string[23], 10.0, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, 1.0, 0.0, 0.0, 1.0);
		Draw(setting_string[22], 110.0, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, text_red, text_green, text_blue, text_alpha);
	}
	else
	{
		Draw(setting_string[23], 10.0, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, text_red, text_green, text_blue, text_alpha);
		Draw(setting_string[22], 110.0, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, 1.0, 0.0, 0.0, 1.0);
	}

	//Font
	draw_y = 400.0;
	Draw(setting_string[9], 0.0, 400.0 + s_sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw_texture(Square_image, dammy_tint, 11, 10.0, draw_y + s_sem_y_offset + 15.0, 90.0, 20.0);
	Draw_texture(Square_image, dammy_tint, 11, 110.0, draw_y + s_sem_y_offset + 15.0, 90.0, 20.0);
	Draw_texture(Square_image, dammy_tint, 11, 210.0, draw_y + s_sem_y_offset + 15.0, 90.0, 20.0);
	if (!s_use_external_font[0] && !s_use_specific_system_font)
	{
		Draw(setting_string[27], 10.0, draw_y + s_sem_y_offset + 12.5, 0.5, 0.5, 1.0, 0.0, 0.0, 1.0);
		Draw(setting_string[28], 110.0, draw_y + s_sem_y_offset + 12.5, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(setting_string[29], 210.0, draw_y + s_sem_y_offset + 12.5, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	}
	else if (s_use_specific_system_font)
	{
		Draw(setting_string[27], 10.0, draw_y + s_sem_y_offset + 12.5, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(setting_string[28], 110.0, draw_y + s_sem_y_offset + 12.5, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(setting_string[29], 210.0, draw_y + s_sem_y_offset + 12.5, 0.5, 0.5, 1.0, 0.0, 0.0, 1.0);
	}
	else if (s_use_external_font[0])
	{
		Draw(setting_string[27], 10.0, draw_y + s_sem_y_offset + 12.5, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(setting_string[28], 110.0, draw_y + s_sem_y_offset + 12.5, 0.5, 0.5, 1.0, 0.0, 0.0, 1.0);
		Draw(setting_string[29], 210.0, draw_y + s_sem_y_offset + 12.5, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	}

	//Font, System specific
	draw_x = 10.0;
	draw_y = 440.0;
	Draw(setting_string[10], 0.0, draw_y + s_sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	for (int i = 0; i < 4; i++)
	{
		Draw_texture(Square_image, dammy_tint, 11, draw_x, draw_y + s_sem_y_offset + 15.0, 70.0, 20.0);
		if (s_lang_select_num == i)
			Draw(setting_string[30 + i], draw_x, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, 1.0, 0.0, 0.0, 1.0);
		else
			Draw(setting_string[30 + i], draw_x, draw_y + s_sem_y_offset + 12.5, 0.75, 0.75, text_red, text_green, text_blue, text_alpha);

		draw_x += 75.0;
	}

	//Font, External
	draw_y = 480.0;
	Draw(setting_string[11], 0.0, draw_y + s_sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw_texture(Square_image, dammy_tint, 3, 10.0, draw_y + s_sem_y_offset + 15.0, 100.0, 20.0);
	Draw_texture(Square_image, dammy_tint, 5, 110.0, draw_y + s_sem_y_offset + 15.0, 100.0, 20.0);
	Draw(setting_string[34], 10.0, draw_y + s_sem_y_offset + 12.5, 0.65, 0.65, text_red, text_green, text_blue, text_alpha);
	Draw(setting_string[35], 110.0, draw_y + s_sem_y_offset + 12.5, 0.65, 0.65, text_red, text_green, text_blue, text_alpha);
	draw_x = 10.0;
	draw_y = 515.0;
	for (int i = 0; i < 46; i++)
	{
		Draw_texture(Square_image, dammy_tint, 11, draw_x, draw_y + s_sem_y_offset, 200.0, 20.0);
		if (s_use_external_font[i + 1])
			Draw(s_font_file_name[i], draw_x, draw_y + s_sem_y_offset - 2.5, 0.45, 0.45, 1.0, 0.0, 0.0, 1.0);
		else
			Draw(s_font_file_name[i], draw_x, draw_y + s_sem_y_offset - 2.5, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);

		draw_y += 20.0;
	}

	//Buffer size
	draw_y = 1435.0;
	for (int i = 0; i < 4; i++)
	{
		Draw(setting_string[60 + i], 0.0, draw_y + s_sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw_texture(Square_image, dammy_tint, 11, 10.0, draw_y + s_sem_y_offset + 15.0, 90.0, 20.0);
		Draw_texture(Square_image, dammy_tint, 11, 110.0, draw_y + s_sem_y_offset + 15.0, 90.0, 20.0);
		Draw(setting_string[64], 10.0, draw_y + s_sem_y_offset + 12.5, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(setting_string[65], 110.0, draw_y + s_sem_y_offset + 12.5, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		draw_y += 40.0;
	}

	/*
	Draw_texture(Square_image, dammy_tint, 5, 130.0, 45.0 + s_sem_y_offset, 50.0, 10.0);
	Draw_texture(Square_image, dammy_tint, 5, 130.0, 85.0 + s_sem_y_offset, 50.0, 10.0);
	Draw_texture(Square_image, dammy_tint, 5, 150.0, 125.0 + s_sem_y_offset, 50.0, 10.0);
	Draw_texture(Square_image, dammy_tint, 5, 240.0, 165.0 + s_sem_y_offset, 50.0, 10.0);
	Draw_texture(Square_image, dammy_tint, 5, 240.0, 205.0 + s_sem_y_offset, 50.0, 10.0);
	*/

	/*
	if (s_sem_help_mode_num == 0)
		Draw(" ", 130.0, 45.0 + s_sem_y_offset, 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
	else
		Draw(" ", 130.0, 45.0 + s_sem_y_offset, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	*/
	/*
	if (s_sem_help_mode_num == 1)
		Draw(" ", 130.0, 85.0 + s_sem_y_offset, 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
	else
		Draw(" ", 130.0, 85.0 + s_sem_y_offset, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	*/
	/*
	if (s_sem_help_mode_num == 2)
		Draw(" ", 150.0, 125.0 + s_sem_y_offset, 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
	else
		Draw(" ", 150.0, 125.0 + s_sem_y_offset, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);

	if (s_sem_help_mode_num == 3)
		Draw(" ", 240.0, 165.0 + s_sem_y_offset, 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
	else
		Draw(" ", 240.0, 165.0 + s_sem_y_offset, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);

	if (s_sem_help_mode_num == 4)
		Draw(" ", 240.0, 205.0 + s_sem_y_offset, 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
	else
		Draw(" ", 240.0, 205.0 + s_sem_y_offset, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	*/

	if (s_sem_show_newest_ver_data)
	{
		Draw_texture(Square_image, dammy_tint, 8, 15.0, 15.0, 290.0, 200.0);
		Draw_texture(Square_image, dammy_tint, 11, 15.0, 200.0, 145.0, 15.0);
		Draw_texture(Square_image, dammy_tint, 5, 160.0, 200.0, 145.0, 15.0);

		if(check_update_progress == 0)
			Draw(setting_string[55], 17.5, 15.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		else if(check_update_progress == 2)
			Draw(setting_string[56], 17.5, 15.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		else if (check_update_progress == 1)
		{
			Draw(setting_string[53], 17.5, 15.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
			Draw(setting_string[41], 17.5, 30.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
			Draw(newest_ver_data[10], 17.5, 45.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		}
		Draw(setting_string[58], 17.5, 200.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(setting_string[57], 162.5, 200.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
	}
	if (s_sem_select_ver)
	{
		Draw_texture(Square_image, dammy_tint, 8, 15.0, 15.0, 290.0, 200.0);
		Draw_texture(Square_image, dammy_tint, 5, 15.0, 200.0, 145.0, 15.0);
		Draw_texture(Square_image, dammy_tint, 11, 160.0, 200.0, 145.0, 15.0);

		draw_y = 15.0;
		for (int i = 0; i < 8; i++)
		{
			if(s_sem_available_ver[i] && s_sem_selected_edition_num == i)
				Draw(setting_string[42 + i], 17.5, draw_y, 0.45, 0.45, 1.0, 0.0, 0.0, 1.0);
			else if (s_sem_available_ver[i])
				Draw(setting_string[42 + i], 17.5, draw_y, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
			else
				Draw(setting_string[42 + i] + setting_string[50], 17.5, draw_y, 0.45, 0.45, text_red, text_green, text_blue, 0.25);

			draw_y += 10.0;
		}

		Draw(setting_string[36], 17.5, 100.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(setting_string[37], 17.5, 110.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(setting_string[38], 17.5, 120.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(setting_string[39], 17.5, 130.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);

		if (s_sem_selected_edition_num == 0)
		{
			Draw(setting_string[40], 17.5, 140.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
			Draw("sdmc:/Line/ver_" + newest_ver_data[0] + "/Line_for_3DS.3dsx", 17.5, 150.0, 0.45, 0.45, 1.0, 0.0, 0.0, 1.0);
		}

		if (update_progress != -1)
			Draw(setting_string[51], 17.5, 160.0, 0.75, 0.75, text_red, text_green, text_blue, text_alpha);

		if (update_progress == 2)
			Draw(setting_string[52], 17.5, 180.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);

		if (s_sem_available_ver[s_sem_selected_edition_num])
			Draw(setting_string[59], 162.5, 200.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		else
			Draw(setting_string[59], 162.5, 200.0, 0.4, 0.4, text_red, text_green, text_blue, 0.25);

		Draw(setting_string[57], 17.5, 200.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
	}

	if (s_error_display)
		Share_draw_error();

	Draw_texture(Background_image, dammy_tint, 1, 0.0, 225.0, 320.0, 15.0);
	Draw(s_bot_button_string[1], 30.0f, 220.0f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 1.0f);

	if (s_key_touch_held)
		Draw(s_circle_string, s_touch_pos_x, s_touch_pos_y, 0.20f, 0.20f, 1.0f, 0.0f, 0.0f, 1.0f);

	Draw_apply_draw();
	s_fps += 1;
	s_frame_time = osTickCounterRead(&s_tcount_frame_time);
}

void Setting_menu_update_check(void* arg)
{
	u8* check_update_buffer;	
	u32 downloaded_size;
	u32 status_code;
	u32 write_size;
	int check_update_log_num_return;
	int newest_ver;
	size_t parse_start_pos;
	size_t parse_end_pos;
	std::string url;
	std::string parse_cache;
	std::string parse_start[11] = {"<newest>", "<3dsx_available>", "<cia_32mb_ram_available>", 
	"<cia_64mb_ram_available>", "<cia_72mb_ram_available>", "<cia_80mb_ram_available>", 
	"<cia_96mb_ram_available>", "<cia_124mb_ram_available>", "<cia_178mb_ram_available>", 
	"<gas_ver>", "<patch_note>", };
	std::string parse_end[11] = { "</newest>", "</3dsx_available>", "</cia_32mb_ram_available>",
	"</cia_64mb_ram_available>", "</cia_72mb_ram_available>", "</cia_80mb_ram_available>",
	"</cia_96mb_ram_available>", "</cia_124mb_ram_available>", "</cia_178mb_ram_available>",
	"</gas_ver>", "</patch_note>", };
	std::string editions[8] = { ".3dsx", "_32mb.cia", "_64mb.cia", "_72mb.cia",
	"_80mb.cia", "_96mb.cia", "_124mb.cia", "_178mb.cia", };
	FS_Archive check_update_fs_archive = 0;
	Handle check_update_fs_handle = 0;
	Handle check_update_am_handle = 0;
	Result_with_string check_update_result;

	while (s_update_check_thread_run)
	{
		if (s_sem_update_check_request || s_sem_file_download_request)
		{
			if (s_sem_update_check_request)
			{
				url = "https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/version/newest.txt";
				check_update_progress = 0;
				new_version_available = false;
				for(int i = 0; i < 8; i++)
					s_sem_available_ver[i] = false;
				for (int i = 0; i < 11; i++)
					newest_ver_data[i] = "";
			}
			else if (s_sem_file_download_request)
			{
				url = "https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/version/Line_for_3DS" + editions[s_sem_selected_edition_num];
				update_progress = 0;
			}

			newest_ver = -1;
			check_update_buffer = (u8*)malloc(0x300000);
			if (check_update_buffer == NULL)
			{
				Share_clear_error_message();
				Share_set_error_message("[Error] Out of memory.", "Couldn't malloc to 'check_update_buffer'(" + std::to_string(0x300000 / 1024) + "KB).", "Sem/Check update thread", OUT_OF_MEMORY);
				s_error_display = true;
				Share_app_log_save("Sem/Check update thread", "[Error] Out of memory.", OUT_OF_MEMORY, false);
			}
			else
			{
				if (s_sem_update_check_request)
					check_update_log_num_return = Share_app_log_save("Sem/Check update thread/httpc", "Checking for update...", 1234567890, false);
				else if (s_sem_file_download_request)
					check_update_log_num_return = Share_app_log_save("Sem/Check update thread/httpc", "Downloading file...", 1234567890, false);
				else
					check_update_log_num_return = Share_app_log_save("Sem/Check update thread/httpc", "", 1234567890, false);

				check_update_result = Httpc_dl_data(url, check_update_buffer, 0x300000, &downloaded_size, &status_code, true);
				Share_app_log_add_result(check_update_log_num_return, check_update_result.string + std::to_string(downloaded_size / 1024) + "KB (" + std::to_string(downloaded_size) + "B)", check_update_result.code, false);

				if (check_update_result.code != 0)
				{
					Share_clear_error_message();
					Share_set_error_message(check_update_result.string, check_update_result.error_description, "Sem/Check update thread/httpc", check_update_result.code);
					s_error_display = true;

					if (s_sem_update_check_request)
						check_update_progress = 2;
					else if (s_sem_file_download_request)
						update_progress = 3;
				}
				else
				{
					if (s_sem_update_check_request)
					{
						parse_cache = (char*)check_update_buffer;

						for (int i = 0; i < 11; i++)
						{
							parse_start_pos = std::string::npos;
							parse_end_pos = std::string::npos;
							parse_start_pos = parse_cache.find(parse_start[i]);
							parse_end_pos = parse_cache.find(parse_end[i]);

							parse_start_pos += parse_start[i].length();
							parse_end_pos -= parse_start_pos;
							if (parse_start_pos != std::string::npos && parse_end_pos != std::string::npos)
								newest_ver_data[i] = parse_cache.substr(parse_start_pos, parse_end_pos);
							else
							{
								check_update_progress = 2;
								break;
							}

							if (i == 0)
								newest_ver = stoi(newest_ver_data[i]);
							else if (i > 0 && i < 8)
								s_sem_available_ver[i - 1] = stoi(newest_ver_data[i]);
							else if (i == 9)
							{
								if (s_current_gas_ver == stoi(newest_ver_data[i]))
									need_gas_update = false;
								else
									need_gas_update = true;
							}
						}

						if (s_current_app_ver < newest_ver)
						{
							new_version_available = true;
							Share_app_log_save("Sem/Check update thread", "New version available " + std::to_string(newest_ver), 1234567890, false);
						}
						else
						{
							new_version_available = false;
							Share_app_log_save("Sem/Check update thread", "Up to date ", 1234567890, false);
						}
						check_update_progress = 1;
					}
					else if (s_sem_file_download_request)
					{
						update_progress = 1;
						if (s_sem_selected_edition_num == 0)
						{
							check_update_log_num_return = Share_app_log_save("Sem/Check update thread/fs", "Save_to_file...", 1234567890, false);
							check_update_result = Share_save_to_file("Line_for_3DS_ver.3dsx", (u8*)check_update_buffer, downloaded_size, "/Line/ver_" + newest_ver_data[0] + "/", true, check_update_fs_handle, check_update_fs_archive);
							Share_app_log_add_result(check_update_log_num_return, check_update_result.string, check_update_result.code, false);
							if (check_update_result.code == 0)
								update_progress = 2;
						}

						if (s_sem_selected_edition_num > 0 && s_sem_selected_edition_num < 8)
						{
							if (s_am_success)
							{
								check_update_result.code = AM_StartCiaInstall(MEDIATYPE_SD, &check_update_am_handle);
								check_update_log_num_return = Share_app_log_save("Sem/Check update thread/am", "AM_StartCiaInstall...", check_update_result.code, false);

								check_update_result.code = FSFILE_Write(check_update_am_handle, &write_size, 0, (u8*)check_update_buffer, downloaded_size, FS_WRITE_FLUSH);
								check_update_log_num_return = Share_app_log_save("Sem/Check update thread/fs", "FSFILE_Write...", check_update_result.code, false);

								check_update_result.code = AM_FinishCiaInstall(check_update_am_handle);
								check_update_log_num_return = Share_app_log_save("Sem/Check update thread/am", "AM_FinishCiaInstall...", check_update_result.code, false);
								if (check_update_result.code == 0)
									update_progress = 2;
								else
									update_progress = 3;
							}
							else
								update_progress = 3;
						}
					}
				}				
			}

			free(check_update_buffer);

			if(s_sem_update_check_request)
				s_sem_update_check_request = false;
			else if(s_sem_file_download_request)
				s_sem_file_download_request = false;
		}
		else
			usleep(100000);
	}
}

void Setting_menu_exit(void)
{
	Share_app_log_save("Sem/Exit", "Exiting...", 1234567890, s_debug_slow);
	u64 time_out = 10000000000;
	int exit_log_num_return;
	bool function_fail = false;
	Result_with_string exit_result;
	exit_result.code = 0;
	exit_result.string = "[Success] ";

	s_sem_already_init = false;
	s_update_check_thread_run = false;

	exit_log_num_return = Share_app_log_save("Sem/Exit", "Thread exiting(0/0)...", 1234567890, s_debug_slow);
	exit_result.code = threadJoin(check_update_thread, time_out);
	if (exit_result.code == 0)
		Share_app_log_add_result(exit_log_num_return, "[Success] ", exit_result.code, s_debug_slow);
	else
	{
		function_fail = true;
		Share_app_log_add_result(exit_log_num_return, "[Error] ", exit_result.code, s_debug_slow);
	}

	if (function_fail)
		Share_app_log_save("Sem/Exit", "[Warn] Some function returned error.", 1234567890, s_debug_slow);

	Share_app_log_save("Sem/Exit", "Exited.", 1234567890, s_debug_slow);
}
