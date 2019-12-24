#include <string>
#include <cstring>
#include <unistd.h>
#include <3ds.h>

#include "unicodetochar.h"
#include "file.hpp"
#include "draw.hpp"
#include "line.hpp"
#include "share_function.hpp"

int try_left = 3;
int number_of_message = 0;
int update_progress = 0;
int room_select_num = 0;
int log_num = 0;

double update_time[6];
float text_x = 0.0;
float text_y = 0.0;
float text_interval = 35;
float text_size = 0.45;
float fps_est = 0;
float background_color = 1.0;
float line_text_red = 0.0f;
float line_text_green = 0.0f;
float line_text_blue = 0.0f;
int line_parse_string_length = 0;

u32 read_size;

bool line_update_thread_run = false;
bool line_log_download_thread_run = false;
bool line_message_send_thread_run = false;
bool line_parse_thread_run = false;

bool line_data_parse_request = false;
bool line_data_parse_end = false;
bool line_auth_success = false;

bool log_update = false;
bool message_send = false;
bool mcu_fail = false;
bool message_send_check_need = false;

bool pic_test = false;

char frame_time_message[32];
char frame_rate_message[32];
char update_time_string[512];
char message_send_confirm[16384] = "";
char debug_log[100];
char debug_log_2[100];
char message_select_num_string[32];

std::string main_url;
//std::string pic_content_url = "https://script.google.com/macros/s/AKfycbzhEY7n0Il-fUyWPJH8fJef8q5cys6TplkzmY-kkE5lT6TNzUQ/exec?id=10735025435494";
//std::string pic_content_url_1 = "https://script.google.com/macros/s/AKfycbzhEY7n0Il-fUyWPJH8fJef8q5cys6TplkzmY-kkE5lT6TNzUQ/exec?id=10735025435494";
std::string message_log[305];
std::string message_log_short[60000];
std::string id[100];
std::string input_text;
char battery_level_message[8] = "?";
std::string battery_charge_message[4] = { "Discharging", "Charging", "放電中", "充電中" };
std::string update_time_message[12] =
{ "初期化に", "接続に", "結果URL受信に", "接続に", "ダウンロードに", "解析に" , "to initialize", "to connection", "to get result url" , "to connection" , "to download" , "to parse download data" };

std::string lite_mode_message[4] = { "Litemode : OFF", "Litemode : ON", "軽量モード : OFF" ,"軽量モード : ON" };
std::string lite_mode_change_message[4] = { "Litemode has been disabled.", "Litemode has been enabled.", "軽量モードは無効になっています" ,"軽量モードは有効になっています" };
std::string night_mode_message[4] = { "Night mode : OFF", "Night mode : ON", "夜モード : OFF", "夜モード : ON" };
std::string draw_vsync_mode_message[4] = { "Vsync : OFF", "Vsync : ON", "垂直同期 : OFF", "垂直同期 : ON" };
std::string language_message = "日本語/English";
std::string line_log_data = "";
std::string line_auto_update_mode_message[4] = { "Auto update : OFF", "Auto update : ON", "自動更新 : OFF", "自動更新 : ON" };
std::string line_frame_time_mode_message[4] = { "Frame time : OFF", "Frame time : ON", "フレームタイム : OFF", "フレームタイム : ON" };
std::string line_update_detail_show_mode_message[4] = { "Debug log : OFF", "Debug log : ON", "デバッグログ : OFF", "デバッグログ : ON" };

std::string language_select_string[7] = { "JP/US/EU/AU", "CH", "KO", "TW" };
char* message_log_cache[305];

Thread line_log_download_thread, line_message_send_thread, line_log_parse_thread, line_update_thread;
Result line_function_result;
FS_DirectoryEntry fs_id_entry;
httpcContext line_message_log;
TickCounter update_spended_time;
SwkbdState swkbd_send_message, swkbd_add, swkbd_password;
SwkbdStatusData swkbd_send_message_status;

void Line_init(void)
{
	Share_app_log_save("Line/Init", "Initializing...", 1234567890, true);
	Result_with_string init_result;
	init_result.code = 0;
	init_result.string = "[Success] ";
	int init_log_num_return;
	share_line_already_init = true;
	hid_disabled = true;
	std::string auth_code = "";
	FS_Archive init_fs_archive = 0;
	Handle init_fs_handle = 0;

	for (int i = 0; i <= 304; i++)
		message_log_cache[i] = (char*)malloc(0x5000);

	Share_app_log_save("Line/Init/c2d", "Loading fonts...", 1234567890, true);
	Draw_load_system_fonts();

	init_log_num_return = Share_app_log_save("Line/Init/fs", "Share_load_from_file...", 1234567890, true);
	init_result = Share_load_from_file("auth", "/Line/", init_fs_handle, init_fs_archive);
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, true);
		auth_code = init_result.string;
	}
	else
	{
		Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, true);
		std::string input_cache;
		while (true)
		{
			memset(share_swkb_input_text, 0x0, 8192);
			swkbdInit(&swkbd_password, SWKBD_TYPE_QWERTY, 1, 256);
			swkbdSetHintText(&swkbd_password, "パスワードを入力 / Type password here.");
			swkbdSetValidation(&swkbd_password, SWKBD_ANYTHING, 0, 0);
			swkbdSetPasswordMode(&swkbd_password, SWKBD_PASSWORD_HIDE);

			swkbdInputText(&swkbd_password, share_swkb_input_text, 256);
			input_cache = share_swkb_input_text;

			memset(share_swkb_input_text, 0x0, 8192);
			swkbdInit(&swkbd_password, SWKBD_TYPE_QWERTY, 1, 256);
			swkbdSetHintText(&swkbd_password, "パスワードを入力(再度) / Type password here.(again)");
			swkbdSetValidation(&swkbd_password, SWKBD_ANYTHING, 0, 0);
			swkbdSetPasswordMode(&swkbd_password, SWKBD_PASSWORD_HIDE);

			swkbdInputText(&swkbd_password, share_swkb_input_text, 256);

			if (input_cache == share_swkb_input_text)
			{
				init_log_num_return = Share_app_log_save("Line/Init/fs", "Save_to_file...", 1234567890, true);
				init_result = Share_save_to_file("auth", share_swkb_input_text, "/Line/", true, init_fs_handle, init_fs_archive);
				Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, true);

				init_log_num_return = Share_app_log_save("Line/Fs", "Share_load_from_file...", 1234567890, true);
				init_result = Share_load_from_file("auth", "/Line/", init_fs_handle, init_fs_archive);
				if (init_result.code == 0)
				{
					Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, true);
					auth_code = init_result.string;
				}
				else
					Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, true);
				break;
			}
		}
	}

	Share_app_log_save("Line/Init/auth", "Please enter password", 1234567890, true);
	if (auth_code == "")
	{
		Share_app_log_save("Line/Init/auth", "Password is not set", 1234567890, true);
		line_auth_success = true;
	}
	else
	{
		usleep(150000);
		for (int i = 0; i < try_left; i++)
		{
			swkbdInit(&swkbd_password, SWKBD_TYPE_QWERTY, 1, 256);
			swkbdSetHintText(&swkbd_password, "パスワードを入力 / Type password here.");
			swkbdSetValidation(&swkbd_password, SWKBD_ANYTHING, 0, 0);
			swkbdSetPasswordMode(&swkbd_password, SWKBD_PASSWORD_HIDE);

			swkbdInputText(&swkbd_password, share_swkb_input_text, 256);
			if (auth_code == share_swkb_input_text)
			{
				line_auth_success = true;
				Share_app_log_save("Line/Init/auth", "Password is correct", 1234567890, true);
				break;
			}
			else
			{
				Share_app_log_save("Line/Init/auth", "Password is incorrect", 1234567890, true);
			}
		}
	}

	if (!line_auth_success)
	{
		Share_app_log_save("Line/Init/auth", "Auth failed, rebooting...", 1234567890, true);
		usleep(500000);
		APT_HardwareResetAsync();
	}

	//Start threads
	line_update_thread_run = true;
	line_parse_thread_run = true;
	line_message_send_thread_run = true;
	line_log_download_thread_run = true;

	line_update_thread = threadCreate(Line_update_thread, (void*)(""), STACKSIZE, 0x26, -1, true);
	line_log_parse_thread = threadCreate(Line_log_parse_thread, (void*)(""), STACKSIZE, 0x30, -1, true);
	line_message_send_thread = threadCreate(Line_send_message, (void*)(""), STACKSIZE, 0x30, -1, true);
	line_log_download_thread = threadCreate(Line_log_download, (void*)(""), STACKSIZE, 0x30, -1, true);
	
	init_log_num_return = Share_app_log_save("Line/Init/fs", "Share_load_from_file...", 1234567890, true);
	init_result = Share_load_from_file("main_url.txt", "/Line/", init_fs_handle, init_fs_archive);
	if (init_result.code == 0)
	{
		main_url = init_result.string;
		Share_app_log_add_result(init_log_num_return, "[Success] ", init_result.code, true);
	}
	else
		Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, true);

	
	init_log_num_return = Share_app_log_save("Line/Init/fs", "Read_id...", 1234567890, false);
	init_result = Line_read_id("/Line/to/");
	Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, true);

	usleep(250000);
	hid_disabled = false;
	Share_app_log_save("Line/Init", "Initialized", 1234567890, true);
}

void Line_main(void)
{
	FS_Archive main_fs_archive = 0;
	Handle main_fs_handle = 0;
	int main_log_num_return;
	Result_with_string main_result;
	osTickCounterUpdate(&share_total_frame_time);

	text_size = text_size_cache;
	text_interval = text_interval_cache;
	text_x = text_x_cache;
	text_y = text_y_cache;

	if (night_mode)
	{
		background_color = 0.0f;
		line_text_red = 1.0;
		line_text_green = 1.0;
		line_text_blue = 1.0;
	}
	else
	{
		background_color = 1.0f;
		line_text_red = 0.0;
		line_text_green = 0.0;
		line_text_blue = 0.0;
	}

	Draw_set_draw_mode(draw_vsync_mode);

	Draw_screen_ready_to_draw(0, true, 1);
	Draw_texture(Background_image, 0, 0.0, 0.0, 400.0, 15.0);
	Draw_texture(Wifi_icon_image, share_wifi_signal, 360.0, 0.0, 15.0, 15.0);
	Draw_texture(Battery_level_icon_image, share_battery_level / 5, 330.0, 0.0, 30.0, 15.0);
	if (share_battery_charge)
		Draw_texture(Battery_charge_icon_image, 0, 310.0, 0.0, 20.0, 15.0);
	Draw(share_status, 0.0f, 0.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 1.0f);
	Draw(share_battery_level_string, 337.5f, 1.25f, 0.4f, 0.4f, 0.0f, 0.0f, 0.0f, 0.5f);

	for (int i = 0; i <= 59999; i++)
	{
		if (text_y + text_interval * i <= 0)
		{
		}
		else if (text_y + text_interval * i >= 220)
		{
			break;
		}
		else
		{
			if (i == message_select_num)
			{
				line_text_green = 1.0;
			}
			else
			{
				if (night_mode)
				{
					line_text_green = 1.0;
				}
				else
				{
					line_text_green = 0.0;
				}
			}

			Draw_with_specific_language(message_log_short[i], language_select_num, text_x, text_y + text_interval * i, text_size, text_size, line_text_red, line_text_green, line_text_blue, 1.0f);
		}
	}

	if (share_app_logs_show)
	{
		int y_i = 0;
		for (int i = share_app_log_view_num; i < share_app_log_view_num + 23; i++)
		{
			Draw(share_app_logs[i], share_app_log_x, 10.0f + (y_i * 10), 0.4f, 0.4f, 0.0f, 0.5f, 1.0f, 1.0f);
			y_i++;
		}
	}

	if (share_setting[1] == "jp")
	{
		sprintf(debug_log_2, "%d 個のメッセージが検出されました。", number_of_message);
		sprintf(update_time_string, "%s %.2fms 費やしました。\n%s %.2fms 費やしました。\n%s %.2fms 費やしました。\n%s %.2fms 費やしました。\n%s %.2fms 費や しました(%uKB)。\n%s %.2fms 費やしました。"
			, update_time_message[0].c_str(), update_time[0], update_time_message[1].c_str(), update_time[1], update_time_message[2].c_str()
			, update_time[2], update_time_message[3].c_str(), update_time[3], update_time_message[4].c_str(), update_time[4]
			, (int)read_size / 1000, update_time_message[5].c_str(), update_time[5]);
		sprintf(message_send_confirm, "%s へ \n message %s\n を送信しますか?\n Aで続ける Bで中止", id[room_select_num].c_str(), input_text.c_str());
	}
	else
	{
		sprintf(debug_log_2, "%d message(s) found.", number_of_message);
		sprintf(update_time_string, "Spent %.2fms %s\nSpent %.2fms %s\nSpent %.2fms %s\nSpent %.2fms %s\nSpent %.2fms %s(%uKB).\nSpent %.2fms %s"
			, update_time[0], update_time_message[6].c_str(), update_time[1], update_time_message[7].c_str(), update_time[2], update_time_message[8].c_str()
			, update_time[3], update_time_message[9].c_str(), update_time[4], update_time_message[10].c_str()
			, (int)read_size / 1000, update_time[5], update_time_message[11].c_str());
		sprintf(message_send_confirm, "to : %s \n message : %s\n Do you want to send message?\n A to continue B to abort", id[room_select_num].c_str(), input_text.c_str());
	}
	sprintf(debug_log, "id = %s", id[room_select_num].c_str());

	Draw("■", -25.0f, 12.5f, 0.0f + update_progress, 0.15f, 0.0f, 1.0f, 1.0f, 0.75f);

	Draw_screen_ready_to_draw(1, true, 1);

	if (night_mode)
	{
		Draw("設定", 280.0f, 40.0f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.7f);
	}
	else
	{
		Draw("設定", 280.0f, 40.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f);
	}
	Draw("Drawing time : " + std::to_string(C3D_GetProcessingTime()) + "ms", 0.0, 50.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);

	Draw(debug_log, 0.0f, 10.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
	Draw(debug_log_2, 0.0f, 40.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);

	Draw(share_line_ver, 0.0f, 0.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 1.0f);

	Draw(language_message, 0.0f, 200.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
	if (share_line_setting_menu_show)
	{
		//setting menu
		if (share_setting[1] == "jp")
		{
			Draw(line_auto_update_mode_message[line_auto_update_mode + 2], 0.0f, 60.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
			Draw(night_mode_message[night_mode + 2], 0.0f, 100.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
			Draw(line_frame_time_mode_message[line_frame_time_show + 2], 0.0f, 120.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
			Draw(line_update_detail_show_mode_message[line_update_detail_show + 2], 0.0f, 140.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
			Draw(draw_vsync_mode_message[draw_vsync_mode + 2], 0.0f, 160.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
		}
		else
		{
			Draw(line_auto_update_mode_message[line_auto_update_mode], 0.0f, 60.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
			Draw(night_mode_message[night_mode], 0.0f, 100.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
			Draw(line_frame_time_mode_message[line_frame_time_show], 0.0f, 120.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
			Draw(line_update_detail_show_mode_message[line_update_detail_show], 0.0f, 140.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
			Draw(draw_vsync_mode_message[draw_vsync_mode], 0.0f, 160.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
		}

		for (int i = 0; i <= 3; i++)
		{
			if (i == language_select_num)
				Draw(language_select_string[i], 160.0f, 80.0f + (i * 10), 0.5f, 0.5f, 1.0f, 0.5f, 0.0f, 1.0f);
			else
				Draw(language_select_string[i], 160.0f, 80.0f + (i * 10), 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
		}
	}
	else
	{
		if (line_frame_time_show)
		{
			if (night_mode)
			{
				Draw("■", -15.0f, 80.0f, 10.0f, 6.5f, 0.5f, 0.5f, 0.5f, 0.5f);
			}
			else
			{
				Draw("■", -15.0f, 80.0f, 10.0f, 6.5f, 0.0f, 0.0f, 0.0f, 0.7f);
			}
			for (int i = 118; i >= 0; i--)
			{

				Draw("●", i + 60.0f, 220.0f - share_drawing_time_history[i], 0.05f, 0.05f, 0.0f, 1.0f, 1.0f, 1.0f);
			}

		}

		if (line_update_detail_show)
		{
			Draw(update_time_string, 0.0f, 60.0f, 0.45f, 0.45f, 0.0f, 0.0f, 1.0f, 1.0f);
		}
	}

	if (message_send_check_need)
	{
		Draw(message_send_confirm, 15.0f, 115.0f, 0.45f, 0.45f, 1.0f, 1.0f, 0.0f, 1.0f);
	}

	sprintf(message_select_num_string, "msg select = %d", message_select_num);
	Draw(message_select_num_string, 0.0f, 20.0f, 0.5f, 0.5f, 1.0f, 0.5f, 0.0f, 1.0f);

	Draw_texture(Background_image, 1, 0.0, 225.0, 320.0, 15.0);
	Draw(share_bot_button_string, 30.0f, 220.0f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 1.0f);

	if (share_key_touch_held)
		Draw("●", touch_pos.px, touch_pos.py, 0.2f, 0.2f, 1.0f, 0.0f, 0.0f, 1.0f);

	Draw_apply_draw();

	share_frame_time_point[0] = osTickCounterRead(&share_total_frame_time);
	share_fps += 1;
	fps_est = 1000 / share_frame_time_point[0];

	hid_disabled = true;
	if (message_send_check_need)
	{
		if (share_key_A_press)
		{
			message_send = true;
			message_send_check_need = false;
		}
		if (share_key_B_press)
			message_send_check_need = false;
	}
	else
	{
		/*if (share_key_touch_held && kDown & KEY_ZR)
		{
			pic_content_url = "https://script.google.com/macros/s/AKfycbzhEY7n0Il-fUyWPJH8fJef8q5cys6TplkzmY-kkE5lT6TNzUQ/exec?id=10735025435494";;
			pic_test = true;
			log_update = true;
		}
		else if (kHeld & KEY_ZR && kDown & KEY_ZL)
		{
			pic_content_url = "https://script.google.com/macros/s/AKfycbzhEY7n0Il-fUyWPJH8fJef8q5cys6TplkzmY-kkE5lT6TNzUQ/exec?id=10735050955459";;
			pic_test = true;
			log_update = true;
		}*/
		if (share_key_A_press)
		{
			memset(share_swkb_input_text, 0x0, 8192);
			swkbdInit(&swkbd_send_message, SWKBD_TYPE_NORMAL, 2, 8192);
			swkbdSetHintText(&swkbd_send_message, "メッセージを入力 / Type message here.");
			swkbdSetValidation(&swkbd_send_message, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
			swkbdSetFeatures(&swkbd_send_message, SWKBD_PREDICTIVE_INPUT);
			swkbdSetInitialText(&swkbd_send_message, share_clipboard[0].c_str());
			swkbdSetDictWord(&share_swkb_words[0], "ぬべ", "壁|՞ةڼ)イーヒヒヒヒヒヒｗｗｗｗｗｗｗｗｗｗｗ");
			swkbdSetDictWord(&share_swkb_words[1], "ぬべ", "┌(☝┌՞ ՞)☝キエェェェエェェwwwww");
			swkbdSetDictWord(&share_swkb_words[2], "ぬべ", "┌(┌ ՞ةڼ)┐<ｷｴｪｪｪｴｴｪｪｪ");
			swkbdSetDictWord(&share_swkb_words[3], "ぬべ", "└(՞ةڼ◔)」");
			swkbdSetDictWord(&share_swkb_words[4], "ぬべ", "(  ՞ةڼ  )");
			swkbdSetDictWord(&share_swkb_words[5], "ぬべ", "└(՞ةڼ◔)」");
			swkbdSetDictWord(&share_swkb_words[6], "びぇ", "。゜( ;⊃՞ةڼ⊂; )゜。びぇぇえええんｗｗｗｗ");
			swkbdSetDictWord(&share_swkb_words[7], "うえ", "(✌ ՞ةڼ ✌ )");
			swkbdSetDictionary(&swkbd_send_message, share_swkb_words, 8);

			swkbdSetStatusData(&swkbd_send_message, &swkbd_send_message_status, true, true);
			swkbdSetLearningData(&swkbd_send_message, &share_swkb_learn_data, true, true);
			share_swkb_press_button = swkbdInputText(&swkbd_send_message, share_swkb_input_text, 8192);
			if (share_swkb_press_button == SWKBD_BUTTON_RIGHT)
			{
				input_text = share_swkb_input_text;
				if (input_text.length() > 2000)
				{
					input_text = input_text.substr(0, 1990);
				}
				message_send_check_need = true;
			}
		}
		else if (share_key_B_press)
		{
			log_update = true;
		}
		else if (share_key_X_press)
		{
			char input_text[512];
			swkbdInit(&swkbd_add, SWKBD_TYPE_QWERTY, 2, 512);
			swkbdSetHintText(&swkbd_add, "URLを入力 / Type your url here.");
			swkbdSetValidation(&swkbd_add, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
			share_swkb_press_button = swkbdInputText(&swkbd_add, input_text, 512);
			if (!share_swkb_press_button == SWKBD_BUTTON_LEFT)
			{
				main_log_num_return = Share_app_log_save("Line/Main/fs", "Save_to_file(main_url.txt)...", 1234567890, false);
				main_result = Share_save_to_file("main_url.txt", input_text, "/Line/", true, main_fs_handle, main_fs_archive);
				Share_app_log_add_result(main_log_num_return, main_result.string, main_result.code, true);
			}

			main_log_num_return = Share_app_log_save("Line/Init/fs", "Share_load_from_file...", 1234567890, true);
			main_result = Share_load_from_file("main_url.txt", "/Line/", main_fs_handle, main_fs_archive);
			if (main_result.code == 0)
			{
				main_url = main_result.string;
				Share_app_log_add_result(main_log_num_return, "[Success] ", main_result.code, true);
			}
			else
				Share_app_log_add_result(main_log_num_return, main_result.string, main_result.code, true);
		}
		else if (share_key_Y_press)
		{
			char input_text[64];
			swkbdInit(&swkbd_add, SWKBD_TYPE_QWERTY, 2, 39);
			swkbdSetHintText(&swkbd_add, "IDを入力 / Type id here.");
			swkbdSetInitialText(&swkbd_add, share_clipboard[0].c_str());
			swkbdSetValidation(&swkbd_add, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
			share_swkb_press_button = swkbdInputText(&swkbd_add, input_text, 39);
			if (!share_swkb_press_button == SWKBD_BUTTON_LEFT)
			{
				main_log_num_return = Share_app_log_save("Line/Main/fs", "Save_new_id...", 1234567890, false);
				main_result = Line_save_new_id(input_text, "/Line/to/");
				Share_app_log_add_result(main_log_num_return, main_result.string, main_result.code, true);
			}
			main_log_num_return = Share_app_log_save("Line/Main/fs", "Read_id...", 1234567890, false);
			main_result = Line_read_id("/Line/to/");
			Share_app_log_add_result(main_log_num_return, main_result.string, main_result.code, true);
		}
		else if (share_key_DRIGHT_press || (share_key_touch_press && share_touch_pos_x >= 210 && share_touch_pos_x <= 320 && share_touch_pos_y >= 220 && share_touch_pos_y <= 240))
		{
			room_select_num += 1;
			if (room_select_num > 99)
			{
				room_select_num = 99;
			}

			std::string file_name;
			char cache[512];
			file_name = id[room_select_num].substr(0, 16);

			sprintf(cache, "Load_from_log_sd(%s)=", file_name.c_str());
			line_function_result = Line_load_from_log_sd(file_name);
			Share_app_log_save("Line/Main/fs", cache, line_function_result, false);
		}
		else if (share_key_DLEFT_press || (share_key_touch_press && share_touch_pos_x >= 0 && share_touch_pos_x <= 110 && share_touch_pos_y >= 220 && share_touch_pos_y <= 240))
		{
			room_select_num -= 1;
			if (room_select_num < 0)
			{
				room_select_num = 0;
			}

			std::string file_name;
			char cache[512];
			file_name = id[room_select_num].substr(0, 16);

			sprintf(cache, "Load_from_log_sd(%s)=", file_name.c_str());
			line_function_result = Line_load_from_log_sd(file_name);
			Share_app_log_save("Line/Main/fs", cache, line_function_result, false);
		}
		else if (share_key_ZL_press)
		{
			if (line_message_select_mode)
				line_message_select_mode = false;
			else
				line_message_select_mode = true;
		}
		else if (share_key_ZR_press)
		{
			share_clipboard[0] = message_log[message_select_num];
		}
		else if (share_key_START_press || (share_key_touch_press && share_touch_pos_x >= 110 && share_touch_pos_x <= 230 && share_touch_pos_y >= 220 && share_touch_pos_y <= 240))
		{
			share_line_main_run = false;
			share_menu_main_run = true;
			share_line_thread_suspend = true;
		}
	}
}

void Line_log_download(void* arg)
{
	Share_app_log_save("Line/Log download thread", "Thread started.", 1234567890, true);
	osTickCounterStart(&update_spended_time);
	u8* line_log_buffer;
	int log_download_log_num_return;
	bool function_fail = false;
	char* moved_url;
	std::string file_name;
	std::string url;
	FS_Archive log_dl_fs_archive = 0;
	Handle log_dl_fs_handle = 0;
	Result_with_string log_download_result;

	while (line_log_download_thread_run)
	{
		if (share_line_thread_suspend)
			usleep(250000);
		else
		{
			if (log_update)
			{
				log_download_log_num_return = Share_app_log_save("Line/Log download thread/httpc", "Downloading log(1/2)...", 1234567890, false);
				osTickCounterUpdate(&update_spended_time);
				for (int i = 0; i < 6; i++)
				{
					update_time[i] = 0;
				}
				read_size = 0;
				function_fail = false;
				moved_url = (char*)malloc(0x1000);
				line_log_buffer = (u8*)malloc(0x650000);
				log_download_result.code = 0;
				log_download_result.string = "[Success] ";
				
				memset(line_log_buffer, 0x0, 0x650000);

				update_progress = 0;

				update_progress++;
				osTickCounterUpdate(&update_spended_time);
				update_time[0] = osTickCounterRead(&update_spended_time);

				url = main_url + "?id=" + id[room_select_num];
				/*if (!pic_test)
					url = main_url +"?id=" + id[room_select_num];
				else
					url = pic_content_url;*/

				log_download_result.code = httpcOpenContext(&line_message_log, HTTPC_METHOD_GET, url.c_str(), 0);
				if (log_download_result.code != 0)
				{
					log_download_result.string = "[Error] httpcOpenContext failed. ";
					function_fail = true;
				}
				update_progress++;

				if (!function_fail)
				{
					log_download_result.code = httpcSetSSLOpt(&line_message_log, SSLCOPT_DisableVerify);
					if (log_download_result.code != 0)
					{
						log_download_result.string = "[Error] httpcSetSSLOpt failed. ";
						function_fail = true;
					}
				}
				update_progress++;

				if (!function_fail)
				{
					log_download_result.code = httpcSetKeepAlive(&line_message_log, HTTPC_KEEPALIVE_ENABLED);
					if (log_download_result.code != 0)
					{
						log_download_result.string = "[Error] httpcSetKeepAlive failed. ";
						function_fail = true;
					}
				}
				update_progress++;

				if (!function_fail)
				{
					httpcAddRequestHeaderField(&line_message_log, "Connection", "Keep-Alive");
					httpcAddRequestHeaderField(&line_message_log, "User-Agent", "Line for 3DS v0.3.2");
					log_download_result.code = httpcBeginRequest(&line_message_log);
					if (log_download_result.code != 0)
					{
						log_download_result.string = "[Error] httpcBeginRequest failed. ";
						function_fail = true;
					}
				}
				update_progress++;

				osTickCounterUpdate(&update_spended_time);
				update_time[1] = osTickCounterRead(&update_spended_time);

				if (!function_fail)
				{
					log_download_result.code = httpcGetResponseHeader(&line_message_log, "Location", moved_url, 0x1000);
					if (log_download_result.code != 0)
					{
						log_download_result.string = "[Error] httpcGetResponseHeader failed. ";
						function_fail = true;
					}
				}
				update_progress++;

				httpcCloseContext(&line_message_log);
				update_progress++;
				Share_app_log_add_result(log_download_log_num_return, log_download_result.string, log_download_result.code, true);

				osTickCounterUpdate(&update_spended_time);
				update_time[2] = osTickCounterRead(&update_spended_time);

				log_download_log_num_return = Share_app_log_save("Line/Log download thread/httpc", "Downloading log(2/2)...", 1234567890, false);
				
				if (!function_fail)
				{
					log_download_result.code = httpcOpenContext(&line_message_log, HTTPC_METHOD_GET, moved_url, 0);
					if (log_download_result.code != 0)
					{
						log_download_result.string = "[Error] httpcOpenContext failed. ";
						function_fail = true;
					}
				}
				update_progress++;

				if (!function_fail)
				{
					log_download_result.code = httpcSetSSLOpt(&line_message_log, SSLCOPT_DisableVerify);
					if (log_download_result.code != 0)
					{
						log_download_result.string = "[Error] httpcSetSSLOpt failed. ";
						function_fail = true;
					}
				}
				update_progress++;

				if (!function_fail)
				{
					log_download_result.code = httpcSetSSLOpt(&line_message_log, SSLCOPT_DisableVerify);
					if (log_download_result.code != 0)
					{
						log_download_result.string = "[Error] httpcSetSSLOpt failed. ";
						function_fail = true;
					}
				}
				update_progress++;

				if (!function_fail)
				{
					httpcAddRequestHeaderField(&line_message_log, "Connection", "Keep-Alive");
					httpcAddRequestHeaderField(&line_message_log, "User-Agent", "Line for 3DS v0.3.2");
					log_download_result.code = httpcBeginRequest(&line_message_log);
					if (log_download_result.code != 0)
					{
						log_download_result.string = "[Error] httpcBeginRequest failed. ";
						function_fail = true;
					}
				}
				update_progress++;

				osTickCounterUpdate(&update_spended_time);
				update_time[3] = osTickCounterRead(&update_spended_time);

				if (!function_fail)
				{
					log_download_result.code = httpcDownloadData(&line_message_log, line_log_buffer, 0x650000, &read_size);
					if (log_download_result.code != 0)
					{
						log_download_result.string = "[Error] httpcDownloadData failed. ";
						function_fail = true;
					}
				}
				update_progress++;

				httpcCloseContext(&line_message_log);
				update_progress++;
				Share_app_log_add_result(log_download_log_num_return, log_download_result.string, log_download_result.code, true);

				if (!function_fail)
				{
					osTickCounterUpdate(&update_spended_time);
					update_time[4] = osTickCounterRead(&update_spended_time);
					for (int i = 0; i <= 299; i++)
						message_log[i] = "";
					for (int i = 0; i <= 59999; i++)
						message_log_short[i] = "";

					update_progress++;

					if (!pic_test)
					{
						file_name = id[room_select_num].substr(0, 16);
						log_download_log_num_return = Share_app_log_save("Line/Log download thread/fs", "Save_to_file...", 1234567890, false);
						log_download_result = Share_save_to_file(file_name, (char*)line_log_buffer, "/Line/", true, log_dl_fs_handle, log_dl_fs_archive);
						Share_app_log_add_result(log_download_log_num_return, log_download_result.string, log_download_result.code, true);

						line_log_data = (char*)line_log_buffer;
						line_data_parse_request = true;
						line_data_parse_end = false;

						while (!line_data_parse_end)
							usleep(100000);

						osTickCounterUpdate(&update_spended_time);
						update_time[5] = osTickCounterRead(&update_spended_time);
						update_progress++;
					}
					else
					{
						message_log[0] = (char*)line_log_buffer;
						u8* pic_frame_buf = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
						memcpy(pic_frame_buf, line_log_buffer, 0x38400);

						for (int i = 0; i <= 500; i++)
						{
							memcpy(pic_frame_buf, line_log_buffer, 0x38400);
							usleep(16000);
						}
					}
				}

				free(moved_url);
				free(line_log_buffer);
				pic_test = false;
				log_update = false;
				update_progress++;
			}
			usleep(100000);
		}
	}
	Share_app_log_save("Line/Log download thread", "Thread exit.", 1234567890, true);
}

void Line_update_thread(void* arg)
{
	Share_app_log_save("Line/Update thread", "Thread started.", 1234567890, true);
	while (line_update_thread_run)
	{
		if (share_line_thread_suspend)
			usleep(250000);
		else
		{
			usleep(1000000);
			if (line_auto_update_mode)
				log_update = true;
			
			share_drawing_time_history[0] = C3D_GetProcessingTime();

			for (int i = 118; i >= 0; i--)
				share_drawing_time_history[i + 1] = share_drawing_time_history[i];
		}
	}
	Share_app_log_save("Line/Update thread", "Thread exit.", 1234567890, true);
}

void Line_send_message(void* arg)
{
	Share_app_log_save("Line/Message send thread", "Thread started.", 1234567890, true);
	u8* message_send_buffer;
	u32 read_size = 0;
	int message_send_log_num_return;
	bool function_fail = false;
	char send_data[8192];
	Result_with_string message_send_result;
	message_send_buffer = (u8*)malloc(0x5000);

	httpcContext message_send_httpc;
	while (line_message_send_thread_run)
	{
		if (share_line_thread_suspend)
			usleep(250000);
		else
		{
			if (message_send)
			{
				message_send_log_num_return = Share_app_log_save("Line/Message send thread/httpc", "Sending message...", 1234567890, false);
				read_size = 0;
				function_fail = false;
				memset(send_data, 0x0, 8192);
				sprintf(send_data, "{ \"type\": \"send\",\"id\" : \"%s\",\"message\" : \"%s\" }", id[room_select_num].c_str(), input_text.c_str());

				message_send_result.code = 0;
				message_send_result.string = "[Success] ";

				message_send_result.code = httpcOpenContext(&message_send_httpc, HTTPC_METHOD_POST, main_url.c_str(), 0);
				if (message_send_result.code != 0)
				{
					message_send_result.string = "[Error] httpcOpenContext failed. ";
					function_fail = true;
				}

				if (!function_fail)
				{
					message_send_result.code = httpcSetSSLOpt(&message_send_httpc, SSLCOPT_DisableVerify);
					if (message_send_result.code != 0)
					{
						message_send_result.string = "[Error] httpcSetSSLOpt failed. ";
						function_fail = true;
					}
				}
				
				if (!function_fail)
				{
					message_send_result.code = httpcSetKeepAlive(&message_send_httpc, HTTPC_KEEPALIVE_ENABLED);
					if (message_send_result.code != 0)
					{
						message_send_result.string = "[Error] httpcSetKeepAlive failed. ";
						function_fail = true;
					}
				}

				if (!function_fail)
				{
					httpcAddRequestHeaderField(&message_send_httpc, "Connection", "Keep-Alive");
					httpcAddRequestHeaderField(&message_send_httpc, "Content-Type", "application/json");
					httpcAddRequestHeaderField(&message_send_httpc, "User-Agent", "Line for 3DS v0.3.2");
					httpcAddPostDataRaw(&message_send_httpc, (u32*)send_data, strlen(send_data));
					message_send_result.code = httpcBeginRequest(&message_send_httpc);
					if (message_send_result.code != 0)
					{
						message_send_result.string = "[Error] httpcBeginRequest failed. ";
						function_fail = true;
					}
				}

				if (!function_fail)
				{
					message_send_result.code = httpcDownloadData(&message_send_httpc, message_send_buffer, 0x5000, &read_size);
					if (message_send_result.code != 0)
					{
						message_send_result.string = "[Error] httpcDownloadData failed. ";
						function_fail = true;
					}
				}

				httpcCloseContext(&message_send_httpc);
				Share_app_log_add_result(message_send_log_num_return, message_send_result.string, message_send_result.code, true);

				message_send = false;
			}
			usleep(100000);
		}
	}
	Share_app_log_save("Line/Message send thread", "Thread exit.", 1234567890, true);
}

Result_with_string Line_save_new_id(std::string id, std::string dir_path)
{
	bool function_fail = false;
	FS_Archive fs_save_id_archive = 0;
	Result_with_string save_id_result;
	save_id_result.code = 0;
	save_id_result.string = "[Success] ";

	save_id_result.code = FSUSER_OpenArchive(&fs_save_id_archive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
	if (save_id_result.code != 0)
	{
		save_id_result.string = "[Error] FSUSER_OpenArchive failed. ";
		function_fail = true;
	}

	if (!function_fail)
	{
		save_id_result.code = FSUSER_CreateDirectory(fs_save_id_archive, fsMakePath(PATH_ASCII, dir_path.c_str()), FS_ATTRIBUTE_DIRECTORY);
		if (save_id_result.code != 0 && save_id_result.code != (s32)0xC82044BE)//#0xC82044BE directory already exist
		{
			save_id_result.string = "[Error] FSUSER_CreateDirectory failed. ";
			function_fail = true;
		}
	}

	if (!function_fail)
	{
		dir_path += id;
		save_id_result.code = FSUSER_CreateFile(fs_save_id_archive, fsMakePath(PATH_ASCII, dir_path.c_str()), FS_ATTRIBUTE_ARCHIVE, 0);
		if (save_id_result.code != 0)
		{
			save_id_result.string = "[Error] FSUSER_CreateFile failed. ";
			function_fail = true;
		}
	}

	FSUSER_CloseArchive(fs_save_id_archive);

	return save_id_result;
}

Result_with_string Line_read_id(std::string dir_path)
{
	bool function_fail = false;
	char* id_cache = (char*)malloc(0x1000);
	Handle fs_dir_read_handle = 0;
	FS_Archive fs_dir_read_archive = 0;
	Result_with_string read_id_result;
	read_id_result.code = 0;
	read_id_result.string = "[Success] ";

	read_id_result.code = FSUSER_OpenArchive(&fs_dir_read_archive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
	if (read_id_result.code != 0)
	{
		read_id_result.string = "[Error] FSUSER_OpenArchive failed. ";
		function_fail = true;
	}

	if (!function_fail)
	{
		read_id_result.code = FSUSER_OpenDirectory(&fs_dir_read_handle, fs_dir_read_archive, fsMakePath(PATH_ASCII, dir_path.c_str()));
		if (read_id_result.code != 0)
		{
			read_id_result.string = "[Error] FSUSER_OpenDirectory failed. ";
			function_fail = true;
		}
	}

	if (!function_fail)
	{
		int count = 0;
		u32 read_entry = 0;
		u32 read_entry_count = 1;
		while (true)
		{
			read_id_result.code = FSDIR_Read(fs_dir_read_handle, &read_entry, read_entry_count, (FS_DirectoryEntry*)&fs_id_entry);
			if (read_entry == 0)
				break;
			unicodeToChar(id_cache, fs_id_entry.name, 512);
			id[count] = id_cache;
			count++;
		}
	}

	FSDIR_Close(fs_dir_read_handle);
	FSUSER_CloseArchive(fs_dir_read_archive);

	free(id_cache);
	return read_id_result;
}

int Line_load_from_log_sd(std::string file_name)
{
	FS_Archive fs_log_sd_archive = 0;
	Handle fs_log_sd_handle = 0;
	Result_with_string load_log_sd_result;
	line_log_data = "";
	load_log_sd_result = Share_load_from_file(file_name, "/Line/", fs_log_sd_handle, fs_log_sd_archive);

	if (load_log_sd_result.code == 0)
	{
		line_log_data = load_log_sd_result.string;
		line_data_parse_request = true;
		line_data_parse_end = false;
	}
	return load_log_sd_result.code;
}

void Line_log_parse_thread(void* arg)
{
	Share_app_log_save("Line/Log parse thread", "Thread started.", 1234567890, true);
	int debug_length_count = 0;
	int line_cut_length = 80;
	int text_length = 0;
	int log_parse_log_num_return;
	bool function_fail = false;

	char* log_data;
	char* parse_cache;

	while (line_parse_thread_run)
	{
		if (share_line_thread_suspend)
			usleep(250000);
		else
		{
			if (line_data_parse_request)
			{
				log_parse_log_num_return = Share_app_log_save("Line/Log parse thread", "Parsing logs...", 1234567890, false);
				usleep(50000);
				function_fail = false;
				number_of_message = 0;
				log_num = 0;
				debug_length_count = 0;
				line_cut_length = 80;
				text_length = 0;
				line_data_parse_end = false;
				log_data = (char*)malloc(0x750000);
				parse_cache = (char*)malloc(0x10000);
				memset(log_data, 0x0, strlen(log_data));
				strcpy(log_data, line_log_data.c_str());

				for (int i = 0; i <= 299; i++)
					message_log[i] = "";
				for (int i = 0; i <= 59999; i++)
					message_log_short[i] = "";

				message_log_cache[0] = strtok(log_data, "");
				message_log[0] = message_log_cache[0];
				number_of_message++;
				for (int i = 1; i <= 299; i++)
				{
					message_log_cache[i] = strtok(NULL, "");
					if (message_log_cache[i] == NULL)
						break;
					else
					{
						message_log[i] = message_log_cache[i];
						number_of_message++;
					}
				}

				for (int i = 0; i < number_of_message; i++)
				{
					memset(parse_cache, 0x0, 0x10000);
					strcpy(parse_cache, message_log[i].c_str());
					text_length = message_log[i].length();

					if (text_length <= 80)
					{
						log_num++;
						message_log_short[log_num] = message_log[i];
					}
					else
					{
						while (true)
						{
							if (log_num >= 59990)
							{
								function_fail = true;
								break;
							}

							if (debug_length_count + line_cut_length >= text_length)
							{
								line_cut_length = text_length - debug_length_count;
								log_num++;
								message_log_short[log_num] = message_log[i].substr(debug_length_count, line_cut_length);
								break;
							}

							int check_length = mblen(&parse_cache[debug_length_count + line_cut_length], 3);
							if (check_length >= 1)
							{
								log_num++;
								message_log_short[log_num] = message_log[i].substr(debug_length_count, line_cut_length);
								debug_length_count += line_cut_length;
								line_cut_length = 80;
							}
							else
								line_cut_length++;

						}
					}
					log_num++;
					debug_length_count = 0;
					line_cut_length = 80;
				}

				if (function_fail)
					Share_app_log_add_result(log_parse_log_num_return, "[Error] Parsing aborted due to too many messages.", 1234567890, true);
				else
					Share_app_log_add_result(log_parse_log_num_return, "[Success] ", 1234567890, true);

				free(log_data);
				free(parse_cache);
				line_data_parse_end = true;
				line_data_parse_request = false;
			}
			usleep(100000);
		}
	}
	Share_app_log_save("Line/Data parse thread", "Thread exit.", 1234567890, true);
}

void Line_exit(void)
{
	Share_app_log_save("Line/Exit", "Exiting...", 1234567890, true);
	u64 time_out = 10000000000;
	int exit_log_num_return;
	bool function_fail = false;
	Result_with_string exit_result;
	exit_result.code = 0;
	exit_result.string = "[Success] ";

	share_line_already_init = false;

	line_update_thread_run = false;
	line_parse_thread_run = false;
	line_message_send_thread_run = false;
	line_log_download_thread_run = false;

	exit_log_num_return = Share_app_log_save("Line/Exit", "Thread exiting(0/3)...", 1234567890, true);
	exit_result.code = threadJoin(line_update_thread, time_out);
	if(exit_result.code == 0)
		Share_app_log_add_result(exit_log_num_return, "[Success] ", exit_result.code, true);
	else
	{
		function_fail = true;
		Share_app_log_add_result(exit_log_num_return, "[Error] ", exit_result.code, true);
	}

	
	exit_log_num_return = Share_app_log_save("Line/Exit", "Thread exiting(1/3)...", 1234567890, true);
	exit_result.code = threadJoin(line_log_parse_thread, time_out);
	if (exit_result.code == 0)
		Share_app_log_add_result(exit_log_num_return, "[Success] ", exit_result.code, true);
	else
	{
		function_fail = true;
		Share_app_log_add_result(exit_log_num_return, "[Error] ", exit_result.code, true);
	}


	exit_log_num_return = Share_app_log_save("Line/Exit", "Thread exiting(2/3)...", 1234567890, true);
	exit_result.code = threadJoin(line_message_send_thread, time_out);
	if (exit_result.code == 0)
		Share_app_log_add_result(exit_log_num_return, "[Success] ", exit_result.code, true);
	else
	{
		function_fail = true;
		Share_app_log_add_result(exit_log_num_return, "[Error] ", exit_result.code, true);
	}


	exit_log_num_return = Share_app_log_save("Line/Exit", "Thread exiting(3/3)...", 1234567890, true);
	exit_result.code = threadJoin(line_log_download_thread, time_out);
	if (exit_result.code == 0)
		Share_app_log_add_result(exit_log_num_return, "[Success] ", exit_result.code, true);
	else
	{
		function_fail = true;
		Share_app_log_add_result(exit_log_num_return, "[Error] ", exit_result.code, true);
	}

	for (int i = 0; i <= 304; i++)
		free(message_log_cache[i]);

	if(function_fail)
		Share_app_log_save("Line/Exit", "[Warn] Some function returned error.", 1234567890, true);
	
	Share_app_log_save("Line/Exit", "Exited.", 1234567890, true);
}
//985