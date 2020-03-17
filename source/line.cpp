#include <string>
#include <cstring>
#include <unistd.h>
#include <3ds.h>

#include "unicodetochar.h"
#include "draw.hpp"
#include "file.hpp"
#include "httpc.hpp"
#include "line.hpp"
#include "share_function.hpp"

int log_download_progress = 0;
int number_of_message = 0;
int number_of_lines = 0;
float text_x = 0.0;
float text_y = 0.0;
float text_interval = 15;
float text_size = 0.45;

bool data_parse_request = false;

char message_send_confirm[16384] = "";
std::string id[100];
std::string input_text;
std::string main_url;
std::string message_log_short[60000];
std::string line_log_data;

Thread log_download_thread, log_load_thread, message_send_thread, log_parse_thread;

void Line_init(void)
{
	Share_app_log_save("Line/Init", "Initializing...", 1234567890, s_debug_slow);
	bool init_auth_success = false;
	u8* init_buffer;
	u32 init_read_size = 0;
	int init_log_num_return;
	std::string auth_code = "";
	std::string input_string;
	FS_Archive init_fs_archive = 0;
	Handle init_fs_handle = 0;
	SwkbdState init_swkbd;
	Result_with_string init_result;

	s_hid_disabled = true;
	init_buffer = (u8*)malloc(0x2000);
	init_result.code = 0;
	init_result.string = "[Success] ";

	memset(init_buffer, 0x0, 0x2000);
	init_log_num_return = Share_app_log_save("Line/Init/fs", "Share_load_from_file...", 1234567890, s_debug_slow);
	init_result = Share_load_from_file("auth", init_buffer, 0x2000, &init_read_size, "/Line/",  init_fs_handle, init_fs_archive);
	if (init_result.code == 0)
	{
		Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, s_debug_slow);
		auth_code = (char*)init_buffer;
	}
	else
	{
		Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, s_debug_slow);
		while (true)
		{
			memset(s_swkb_input_text, 0x0, 8192);
			swkbdInit(&init_swkbd, SWKBD_TYPE_QWERTY, 1, 256);
			swkbdSetHintText(&init_swkbd, "パスワードを入力 / Type password here.");
			swkbdSetValidation(&init_swkbd, SWKBD_ANYTHING, 0, 0);
			swkbdSetPasswordMode(&init_swkbd, SWKBD_PASSWORD_HIDE);
			swkbdInputText(&init_swkbd, s_swkb_input_text, 256);
			input_string = s_swkb_input_text;

			memset(s_swkb_input_text, 0x0, 8192);
			swkbdInit(&init_swkbd, SWKBD_TYPE_QWERTY, 1, 256);
			swkbdSetHintText(&init_swkbd, "パスワードを入力(再度) / Type password here.(again)");
			swkbdSetValidation(&init_swkbd, SWKBD_ANYTHING, 0, 0);
			swkbdSetPasswordMode(&init_swkbd, SWKBD_PASSWORD_HIDE);
			swkbdInputText(&init_swkbd, s_swkb_input_text, 256);

			if (input_string == s_swkb_input_text)
			{
				init_log_num_return = Share_app_log_save("Line/Init/fs", "Save_to_file...", 1234567890, s_debug_slow);
				init_result = Share_save_to_file("auth", (u8*)s_swkb_input_text, sizeof(s_swkb_input_text), "/Line/", true, init_fs_handle, init_fs_archive);
				Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, s_debug_slow);
				auth_code = s_swkb_input_text;

				break;
			}
		}
	}

	Share_app_log_save("Line/Init/auth", "Please enter password", 1234567890, s_debug_slow);
	if (auth_code == "")
	{
		Share_app_log_save("Line/Init/auth", "Password is not set", 1234567890, s_debug_slow);
		init_auth_success = true;
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			swkbdInit(&init_swkbd, SWKBD_TYPE_QWERTY, 1, 256);
			swkbdSetHintText(&init_swkbd, "パスワードを入力 / Type password here.");
			swkbdSetValidation(&init_swkbd, SWKBD_ANYTHING, 0, 0);
			swkbdSetPasswordMode(&init_swkbd, SWKBD_PASSWORD_HIDE);

			swkbdInputText(&init_swkbd, s_swkb_input_text, 256);
			if (auth_code == s_swkb_input_text)
			{
				init_auth_success = true;
				Share_app_log_save("Line/Init/auth", "Password is correct", 1234567890, s_debug_slow);
				break;
			}
			else
				Share_app_log_save("Line/Init/auth", "Password is incorrect", 1234567890, s_debug_slow);
		}
	}

	if (!init_auth_success)
	{
		Share_app_log_save("Line/Init/auth", "Auth failed, rebooting...", 1234567890, s_debug_slow);
		usleep(500000);
		APT_HardwareResetAsync();
	}

	//Start threads
	s_line_log_download_thread_run = true;
	s_line_log_load_thread_run = true;
	s_line_log_parse_thread_run = true;
	s_line_message_send_thread_run = true;
	s_line_update_thread_run = true;
	
	log_download_thread = threadCreate(Line_log_download_thread, (void*)(""), STACKSIZE, 0x30, -1, s_debug_slow);
	log_load_thread = threadCreate(Line_log_load_thread, (void*)(""), STACKSIZE, 0x30, -1, s_debug_slow);
	log_parse_thread = threadCreate(Line_log_parse_thread, (void*)(""), STACKSIZE, 0x32, -1, s_debug_slow);
	message_send_thread = threadCreate(Line_send_message_thread, (void*)(""), STACKSIZE, 0x30, -1, s_debug_slow);

	memset(init_buffer, 0x0, 0x2000);
	init_log_num_return = Share_app_log_save("Line/Init/fs", "Share_load_from_file...", 1234567890, s_debug_slow);
	init_result = Share_load_from_file("main_url.txt", init_buffer, 0x2000, &init_read_size, "/Line/", init_fs_handle, init_fs_archive);
	Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, s_debug_slow);
	if (init_result.code == 0)
		main_url = (char*)init_buffer;


	init_log_num_return = Share_app_log_save("Line/Init/fs", "Read_id...", 1234567890, false);
	init_result = Line_read_id("/Line/to/");
	Share_app_log_add_result(init_log_num_return, init_result.string, init_result.code, s_debug_slow);

	free(init_buffer);
	s_line_thread_suspend = false;
	s_line_main_run = true;
	s_menu_main_run = false;
	s_line_already_init = true;
	s_hid_disabled = false;

	Share_app_log_save("Line/Init", "Initialized", 1234567890, s_debug_slow);
}

void Line_main(void)
{
	u8* main_buffer;
	u32 main_read_size = 0;
	float text_red;
	float text_green;
	float text_blue;
	float text_alpha;
	float texture_size;
	float scroll_bar_y_size = 5.0;
	float scroll_bar_y_pos = (195.0 / (-s_line_bottom_y / -text_y));
	int main_log_num_return;
	std::string status;
	std::string hidden_id;
	FS_Archive main_fs_archive = 0;
	Handle main_fs_handle = 0;
	SwkbdState main_swkbd;
	SwkbdStatusData main_swkbd_status;
	Result_with_string main_result;

	osTickCounterUpdate(&s_tcount_frame_time);

	text_size = text_size_cache;
	text_interval = text_interval_cache;
	text_x = text_x_cache;
	text_y = text_y_cache;
	texture_size = text_size_cache;

	if (s_line_auto_update_mode || s_line_log_update_request)
		log_download_progress = Httpc_query_dl_progress();

	if (s_night_mode)
	{
		text_red = 1.0;
		text_green = 1.0;
		text_blue = 1.0;
		text_alpha = 0.75;
	}
	else
	{
		text_red = 0.0;
		text_green = 0.0;
		text_blue = 0.0;
		text_alpha = 1.0;
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

	for (int i = 0; i <= 59999; i++)
	{
		if (i > number_of_lines || text_y + text_interval * i >= 240)
			break;
		else if (text_y + text_interval * i <= -1000)
		{
			if ((text_y + text_interval * (i + 100)) <= 10)
				i += 100;
		}
		else if (text_y + text_interval * i <= 10)
		{
		}
		else
		{
			if (s_use_external_font[0])
				Share_draw_external_fonts(message_log_short[i], text_x, text_y + text_interval * i, (texture_size * 1.5), (texture_size * 1.5), false);
			else if (s_use_specific_system_font)
				Draw_with_specific_language(message_log_short[i], s_lang_select_num, text_x, text_y + text_interval * i, text_size, text_size, text_red, text_green, text_blue, text_alpha);
			else
				Draw(message_log_short[i], text_x, text_y + text_interval * i, text_size, text_size, text_red, text_green, text_blue, text_alpha);
		}
	}

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

	if (s_line_hide_id && id[s_line_room_select_num].length() >= 5)
	{
		hidden_id = id[s_line_room_select_num].substr(0, 5);

		for (size_t i = 6; i <= id[s_line_room_select_num].length(); i++)
			hidden_id += "*";
	}
	else
		hidden_id = id[s_line_room_select_num];

	if (s_setting[1] == "jp")
	{
		status = "id = " + hidden_id + "\n"
			+ std::to_string(number_of_message) + s_line_message_jp[0] + std::to_string(number_of_lines) + s_line_message_jp[1];
		sprintf(message_send_confirm, "%s へ \n message %s\n を送信しますか?\n Aで続ける Bで中止", hidden_id.c_str(), input_text.c_str());
	}
	else
	{
		status = "ID = " + hidden_id + "\n"
			+ std::to_string(number_of_message) + s_line_message_en[0] + std::to_string(number_of_lines) + s_line_message_en[1];
		sprintf(message_send_confirm, "to : %s \n message : %s\n Do you want to send message?\n A to continue B to abort", hidden_id.c_str(), input_text.c_str());
	}

	Draw_texture(Square_image, dammy_tint, 10, 0.0, 15.0, 50.0 * log_download_progress, 3.0);

	if (s_night_mode)
		Draw_screen_ready_to_draw(1, true, 1, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(1, true, 1, 1.0, 1.0, 1.0);

	for (int i = 0; i <= 59999; i++)
	{
		if (i > number_of_lines || (text_y + text_interval * i) - 240 >= 125)
			break;
		else if (text_y + text_interval * i <= -1000)
		{
			if ((text_y + text_interval * (i + 100)) <= 10)
				i += 100;
		}
		else if ((text_y + text_interval * i) - 240 <= -20)
		{
		}
		else
		{
			if (s_use_external_font[0])
				Share_draw_external_fonts(message_log_short[i], text_x - 40, (text_y + text_interval * i) - 240, (texture_size * 1.5), (texture_size * 1.5), false);
			else if (s_use_specific_system_font)
				Draw_with_specific_language(message_log_short[i], s_lang_select_num, text_x - 40, (text_y + text_interval * i) - 240, text_size, text_size, text_red, text_green, text_blue, text_alpha);
			else
				Draw(message_log_short[i], text_x - 40, (text_y + text_interval * i) - 240, text_size, text_size, text_red, text_green, text_blue, text_alpha);
		}
	}

	Draw_texture(Square_image, dammy_tint, 0, 312.5, 0.0, 7.5, 15.0);
	Draw_texture(Square_image, dammy_tint, 0, 312.5, 215.0, 7.5, 10.0);

	if (scroll_bar_y_pos <= 0.0)
		Draw_texture(Square_image, dammy_tint, 2, 312.5, 15.0, 7.5, scroll_bar_y_size);
	else if (scroll_bar_y_pos >= 195.0)
		Draw_texture(Square_image, dammy_tint, 2, 312.5, 210.0, 7.5, scroll_bar_y_size);
	else
		Draw_texture(Square_image, dammy_tint, 8, 312.5, 15.0 + scroll_bar_y_pos, 7.5, scroll_bar_y_size);

	if (s_line_menu_mode != 1)
	{
		Draw(status, 12.5f, 140.0f, 0.4f, 0.4f, text_red, text_green, text_blue, text_alpha);
		Draw("hide ID", 260.0f, 140.0f, 0.45f, 0.45f, text_red, text_green, text_blue, text_alpha);
		Draw(s_line_ver, 260.0f, 150.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 1.0f);
	}

	Draw_texture(Square_image, dammy_tint, 11, 10.0, 170.0, 300.0, 60.0);
	Draw_texture(Square_image, dammy_tint, 9, 10.0, 170.0, 50.0, 10.0);
	Draw_texture(Square_image, dammy_tint, 5, 60.0, 170.0, 50.0, 10.0);
	Draw_texture(Square_image, dammy_tint, 11, 110.0, 170.0, 100.0, 10.0);
	Draw_texture(Square_image, dammy_tint, 3, 210.0, 170.0, 100.0, 10.0);

	if (s_line_menu_mode == 0)
	{
		Draw_texture(Square_image, dammy_tint, 8, 10.0, 170.0, 50.0, 10.0);

		Draw_texture(Square_image, dammy_tint, 7, 20.0, 185.0, 80.0, 30.0);
		Draw_texture(Square_image, dammy_tint, 7, 120.0, 185.0, 80.0, 30.0);
		Draw_texture(Square_image, dammy_tint, 7, 220.0, 185.0, 80.0, 30.0);

		if (s_setting[1] == "en")
		{
			Draw(s_line_message_en[6], 22.5f, 185.0f, 0.45f, 0.45f, text_red, text_green, text_blue, text_alpha);
			Draw(s_line_message_en[7], 122.5f, 185.0f, 0.5f, 0.5f, text_red, text_green, text_blue, text_alpha);
			Draw(s_line_message_en[8 + s_line_auto_update_mode], 222.5f, 185.0f, 0.5f, 0.5f, text_red, text_green, text_blue, text_alpha);
		}
		else if (s_setting[1] == "jp")
		{
			Draw(s_line_message_jp[6], 22.5f, 185.0f, 0.45f, 0.45f, text_red, text_green, text_blue, text_alpha);
			Draw(s_line_message_jp[7], 122.5f, 185.0f, 0.5f, 0.5f, text_red, text_green, text_blue, text_alpha);
			Draw(s_line_message_jp[8 + s_line_auto_update_mode], 222.5f, 185.0f, 0.5f, 0.5f, text_red, text_green, text_blue, text_alpha);
		}
	}
	else if (s_line_menu_mode == 1)
	{
		Draw_texture(Square_image, dammy_tint, 4, 60.0, 170.0, 50.0, 10.0);

		Draw_texture(Square_image, dammy_tint, 9, 10.0, 140.0, 300.0, 25.0);
		Draw_texture(Square_image, dammy_tint, 7, 20.0, 185.0, 130.0, 30.0);
		Draw_texture(Square_image, dammy_tint, 7, 170.0, 185.0, 60.0, 30.0);
		Draw_texture(Square_image, dammy_tint, 7, 240.0, 185.0, 60.0, 30.0);

		if (s_setting[1] == "en")
		{
			if (s_use_external_font[0])
				Share_draw_external_fonts((s_line_message_en[10] + std::to_string((int)message_select_num) + s_line_message_en[11] + s_line_message_log[(int)message_select_num].substr(0, 100)), 12.5f, 140.0f, 0.8f, 0.8f, true);
			else if (s_use_specific_system_font)
				Draw_with_specific_language(s_line_message_en[10] + std::to_string((int)message_select_num) + s_line_message_en[11] + s_line_message_log[(int)message_select_num].substr(0, 100), s_lang_select_num, 12.5f, 140.0f, 0.45f, 0.45f, text_red, text_green, text_blue, 1.0f);
			else
				Draw(s_line_message_en[10] + std::to_string((int)message_select_num) + s_line_message_en[11] + s_line_message_log[(int)message_select_num].substr(0, 100), 12.5f, 140.0f, 0.4f, 0.4f, text_red, text_green, text_blue, 1.0f);

			Draw(s_line_message_en[12], 22.5f, 185.0f, 0.5f, 0.5f, text_red, text_green, text_blue, text_alpha);
			Draw(s_line_message_en[13], 172.5f, 185.0f, 0.35f, 0.35f, text_red, text_green, text_blue, text_alpha);
			Draw(s_line_message_en[14], 242.5f, 185.0f, 0.45f, 0.45f, text_red, text_green, text_blue, text_alpha);
		}
		else if (s_setting[1] == "jp")
		{
			if (s_use_external_font[0])
				Share_draw_external_fonts((s_line_message_jp[10] + std::to_string((int)message_select_num) + s_line_message_jp[11] + s_line_message_log[(int)message_select_num].substr(0, 100)), 12.5f, 140.0f, 0.8f, 0.8f, true);
			else if (s_use_specific_system_font)
				Draw_with_specific_language(s_line_message_jp[10] + std::to_string((int)message_select_num) + s_line_message_jp[11] + s_line_message_log[(int)message_select_num].substr(0, 100), s_lang_select_num, 12.5f, 140.0f, 0.45f, 0.45f, text_red, text_green, text_blue, 1.0f);
			else
				Draw(s_line_message_jp[10] + std::to_string((int)message_select_num) + s_line_message_jp[11] + s_line_message_log[(int)message_select_num].substr(0, 100), 12.5f, 140.0f, 0.4f, 0.4f, text_red, text_green, text_blue, 1.0f);

			Draw(s_line_message_jp[12], 22.5f, 185.0f, 0.5f, 0.5f, text_red, text_green, text_blue, text_alpha);
			Draw(s_line_message_jp[13], 172.5f, 185.0f, 0.4f, 0.4f, text_red, text_green, text_blue, text_alpha);
			Draw(s_line_message_jp[14], 242.5f, 185.0f, 0.4f, 0.4f, text_red, text_green, text_blue, text_alpha);
		}
	}
	else if (s_line_menu_mode == 2)
	{
		Draw_texture(Square_image, dammy_tint, 10, 110.0, 170.0, 100.0, 10.0);

		Draw_texture(Square_image, dammy_tint, 7, 20.0, 185.0, 60.0, 30.0);
		Draw_texture(Square_image, dammy_tint, 7, 90.0, 185.0, 60.0, 30.0);
		Draw_texture(Square_image, dammy_tint, 7, 170.0, 185.0, 60.0, 30.0);
		Draw_texture(Square_image, dammy_tint, 7, 240.0, 185.0, 60.0, 30.0);
		if (s_setting[1] == "en")
		{
			Draw(s_line_message_en[15], 22.5f, 185.0f, 0.35f, 0.35f, text_red, text_green, text_blue, text_alpha);
			Draw(s_line_message_en[16], 92.5f, 185.0f, 0.35f, 0.35f, text_red, text_green, text_blue, text_alpha);
			Draw(s_line_message_en[17], 172.5f, 185.0f, 0.4f, 0.4f, text_red, text_green, text_blue, text_alpha);
			Draw(s_line_message_en[18], 242.5f, 185.0f, 0.4f, 0.4f, text_red, text_green, text_blue, text_alpha);
		}
		else if (s_setting[1] == "jp")
		{
			Draw(s_line_message_jp[15], 22.5f, 185.0f, 0.4f, 0.4f, text_red, text_green, text_blue, text_alpha);
			Draw(s_line_message_jp[16], 92.5f, 185.0f, 0.4f, 0.4f, text_red, text_green, text_blue, text_alpha);
			Draw(s_line_message_jp[17], 172.5f, 185.0f, 0.4f, 0.4f, text_red, text_green, text_blue, text_alpha);
			Draw(s_line_message_jp[18], 242.5f, 185.0f, 0.4f, 0.4f, text_red, text_green, text_blue, text_alpha);
		}
	}
	else if (s_line_menu_mode == 3)
	{
		Draw_texture(Square_image, dammy_tint, 2, 210.0, 170.0, 100.0, 10.0);

		Draw_texture(Square_image, dammy_tint, 7, 20.0, 185.0, 130.0, 30.0);
		Draw_texture(Square_image, dammy_tint, 7, 170.0, 185.0, 130.0, 30.0);
		if (s_setting[1] == "en")
		{
			Draw(s_line_message_en[19], 22.5f, 185.0f, 0.5f, 0.5f, text_red, text_green, text_blue, text_alpha);
			Draw(s_line_message_en[20], 172.5f, 185.0f, 0.5f, 0.5f, text_red, text_green, text_blue, text_alpha);
		}
		else if (s_setting[1] == "jp")
		{
			Draw(s_line_message_jp[19], 22.5f, 185.0f, 0.5f, 0.5f, text_red, text_green, text_blue, text_alpha);
			Draw(s_line_message_jp[20], 172.5f, 185.0f, 0.5f, 0.5f, text_red, text_green, text_blue, text_alpha);
		}
	}

	if (s_setting[1] == "en")
	{
		Draw(s_line_message_en[2], 12.5f, 170.0f, 0.4f, 0.4f, text_red, text_green, text_blue, text_alpha);
		Draw(s_line_message_en[3], 62.5f, 170.0f, 0.4f, 0.4f, text_red, text_green, text_blue, text_alpha);
		Draw(s_line_message_en[4], 112.5f, 170.0f, 0.4f, 0.4f, text_red, text_green, text_blue, text_alpha);
		Draw(s_line_message_en[5], 212.5f, 170.0f, 0.4f, 0.4f, text_red, text_green, text_blue, text_alpha);
	}
	else if (s_setting[1] == "jp")
	{
		Draw(s_line_message_jp[2], 12.5f, 170.0f, 0.4f, 0.4f, text_red, text_green, text_blue, text_alpha);
		Draw(s_line_message_jp[3], 62.5f, 170.0f, 0.4f, 0.4f, text_red, text_green, text_blue, text_alpha);
		Draw(s_line_message_jp[4], 112.5f, 170.0f, 0.4f, 0.4f, text_red, text_green, text_blue, text_alpha);
		Draw(s_line_message_jp[5], 212.5f, 170.0f, 0.4f, 0.4f, text_red, text_green, text_blue, text_alpha);
	}

	if (s_line_message_send_check)
	{
		Draw_texture(Square_image, dammy_tint, 8, 10.0, 110.0, 300.0, 60.0);
		Draw(message_send_confirm, 10.0f, 110.0f, 0.45f, 0.45f, text_red, text_green, text_blue, text_alpha);
	}

	if (s_error_display)
		Share_draw_error();

	Draw_texture(Background_image, dammy_tint, 1, 0.0, 225.0, 320.0, 15.0);
	Draw(s_bot_button_string[0], 30.0f, 220.0f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 1.0f);
	if (s_key_touch_held)
		Draw(s_circle_string, s_touch_pos_x, s_touch_pos_y, 0.20f, 0.20f, 1.0f, 0.0f, 0.0f, 1.0f);

	Draw_apply_draw();
	s_frame_time = osTickCounterRead(&s_tcount_frame_time);
	s_fps += 1;

	s_hid_disabled = true;
	if (s_line_type_message_request)
	{
		memset(s_swkb_input_text, 0x0, 8192);
		swkbdInit(&main_swkbd, SWKBD_TYPE_NORMAL, 2, 8192);
		swkbdSetHintText(&main_swkbd, "メッセージを入力 / Type message here.");
		swkbdSetValidation(&main_swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
		swkbdSetFeatures(&main_swkbd, SWKBD_PREDICTIVE_INPUT);
		swkbdSetInitialText(&main_swkbd, s_clipboards[0].c_str());
		swkbdSetDictWord(&s_swkb_words[0], "ぬべ", "壁|՞ةڼ)イーヒヒヒヒヒヒｗｗｗｗｗｗｗｗｗｗｗ");
		swkbdSetDictWord(&s_swkb_words[1], "ぬべ", "┌(☝┌՞ ՞)☝キエェェェエェェwwwww");
		swkbdSetDictWord(&s_swkb_words[2], "ぬべ", "┌(┌ ՞ةڼ)┐<ｷｴｪｪｪｴｴｪｪｪ");
		swkbdSetDictWord(&s_swkb_words[3], "ぬべ", "└(՞ةڼ◔)」");
		swkbdSetDictWord(&s_swkb_words[4], "ぬべ", "(  ՞ةڼ  )");
		swkbdSetDictWord(&s_swkb_words[5], "ぬべ", "└(՞ةڼ◔)」");
		swkbdSetDictWord(&s_swkb_words[6], "びぇ", "。゜( ;⊃՞ةڼ⊂; )゜。びぇぇえええんｗｗｗｗ");
		swkbdSetDictWord(&s_swkb_words[7], "うえ", "(✌ ՞ةڼ ✌ )");
		swkbdSetDictionary(&main_swkbd, s_swkb_words, 8);

		swkbdSetStatusData(&main_swkbd, &main_swkbd_status, true, true);
		swkbdSetLearningData(&main_swkbd, &s_swkb_learn_data, true, true);
		s_swkb_press_button = swkbdInputText(&main_swkbd, s_swkb_input_text, 8192);
		if (s_swkb_press_button == SWKBD_BUTTON_RIGHT)
		{
			input_text = s_swkb_input_text;
			if (input_text.length() > 2000)
				input_text = input_text.substr(0, 1990);

			s_line_message_send_check = true;
		}
		s_line_type_message_request = false;
	}
	if (s_line_type_id_request)
	{
		memset(s_swkb_input_text, 0x0, 8192);
		swkbdInit(&main_swkbd, SWKBD_TYPE_QWERTY, 2, 39);
		swkbdSetHintText(&main_swkbd, "IDを入力 / Type id here.");
		swkbdSetInitialText(&main_swkbd, s_clipboards[0].c_str());
		swkbdSetValidation(&main_swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
		s_swkb_press_button = swkbdInputText(&main_swkbd, s_swkb_input_text, 39);
		if (!s_swkb_press_button == SWKBD_BUTTON_LEFT)
		{
			main_log_num_return = Share_app_log_save("Line/Main/fs", "Save_new_id...", 1234567890, false);
			main_result = Line_save_new_id(s_swkb_input_text, "/Line/to/");
			Share_app_log_add_result(main_log_num_return, main_result.string, main_result.code, false);
		}
		main_log_num_return = Share_app_log_save("Line/Main/fs", "Read_id...", 1234567890, false);
		main_result = Line_read_id("/Line/to/");
		Share_app_log_add_result(main_log_num_return, main_result.string, main_result.code, false);
		s_line_type_id_request = false;
	}
	if (s_line_type_main_url_request)
	{
		memset(s_swkb_input_text, 0x0, 8192);
		swkbdInit(&main_swkbd, SWKBD_TYPE_QWERTY, 2, 512);
		swkbdSetHintText(&main_swkbd, "URLを入力 / Type your url here.");
		swkbdSetInitialText(&main_swkbd, main_url.c_str());
		swkbdSetValidation(&main_swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
		s_swkb_press_button = swkbdInputText(&main_swkbd, s_swkb_input_text, 512);
		if (!s_swkb_press_button == SWKBD_BUTTON_LEFT)
		{
			main_log_num_return = Share_app_log_save("Line/Main/fs", "Save_to_file(main_url.txt)...", 1234567890, false);
			main_result = Share_save_to_file("main_url.txt", (u8*)s_swkb_input_text, sizeof(s_swkb_input_text), "/Line/", true, main_fs_handle, main_fs_archive);
			Share_app_log_add_result(main_log_num_return, main_result.string, main_result.code, false);
		}

		main_buffer = (u8*)malloc(0x2000);
		memset(main_buffer, 0x0, 0x2000);
		main_log_num_return = Share_app_log_save("Line/Init/fs", "Share_load_from_file...", 1234567890, false);
		main_result = Share_load_from_file("main_url.txt", main_buffer, 0x2000, &main_read_size, "/Line/", main_fs_handle, main_fs_archive);
		Share_app_log_add_result(main_log_num_return, main_result.string, main_result.code, false);
		if (main_result.code == 0)
			main_url = (char*)main_buffer;
		free(main_buffer);
		s_line_type_main_url_request = false;
	}
}

void Line_log_download_thread(void* arg)
{
	Share_app_log_save("Line/Log dl thread", "Thread started.", 1234567890, false);
	u8* log_httpc_buffer ;
	u32 downloaded_log_size;
	u32 status_code;
	int log_download_log_num_return;
	std::string file_name;
	std::string downloaded_data_string;
	FS_Archive log_dl_fs_archive = 0;
	Handle log_dl_fs_handle = 0;
	Result_with_string log_download_result;

	while (s_line_log_download_thread_run)
	{
		if (s_line_auto_update_mode || s_line_log_update_request)
		{
			downloaded_log_size = 0;
			status_code = 0;
			log_httpc_buffer = (u8*)malloc(s_line_log_httpc_buffer_size);

			if (log_httpc_buffer == NULL)
			{
				Share_clear_error_message();
				Share_set_error_message("[Error] Out of memory.", "Couldn't malloc to 'log_httpc_buffer'(" + std::to_string(s_line_log_httpc_buffer_size / 1024) + "KB).", "Line/Log dl thread", OUT_OF_MEMORY);
				s_error_display = true;
				Share_app_log_save("Line/Log dl thread", "[Error] Out of memory. ", OUT_OF_MEMORY, false);
				s_line_auto_update_mode = false;
			}
			else
			{
				memset(log_httpc_buffer, 0x0, s_line_log_httpc_buffer_size);
				log_download_log_num_return = Share_app_log_save("Line/Log dl thread/httpc", "Downloading logs...", 1234567890, false);
				log_download_result = Httpc_dl_data(main_url + "?id=" + id[s_line_room_select_num], log_httpc_buffer, s_line_log_httpc_buffer_size, &downloaded_log_size, &status_code, true);
				Share_app_log_add_result(log_download_log_num_return, log_download_result.string + std::to_string(downloaded_log_size / 1024) + "KB (" + std::to_string(downloaded_log_size) + "B) ", log_download_result.code, false);
				log_download_progress = Httpc_query_dl_progress();
				if (log_download_result.code == 0)
				{
					line_log_data = (char*)log_httpc_buffer;
					data_parse_request = true;

					file_name = id[s_line_room_select_num].substr(0, 16);
					log_download_log_num_return = Share_app_log_save("Line/Log dl thread/fs", "Save_to_file...", 1234567890, false);
					log_download_result = Share_save_to_file(file_name, (u8*)log_httpc_buffer, downloaded_log_size, "/Line/", true, log_dl_fs_handle, log_dl_fs_archive);
					Share_app_log_add_result(log_download_log_num_return, log_download_result.string, log_download_result.code, false);
					if (log_download_result.code != 0)
					{
						Share_clear_error_message();
						Share_set_error_message(log_download_result.string, log_download_result.error_description, "Line/Log dl thread", log_download_result.code);
						s_error_display = true;
					}
				}
				else
				{
					Share_clear_error_message();
					Share_set_error_message(log_download_result.string, log_download_result.error_description, "Line/Log dl thread", log_download_result.code);
					s_error_display = true;
					s_line_auto_update_mode = false;
				}
			}
			free(log_httpc_buffer);
			log_httpc_buffer = NULL;
			s_line_log_update_request = false;
		}
		usleep(100000);

		while (s_line_thread_suspend)
			usleep(250000);
	}
	Share_app_log_save("Line/Log dl thread", "Thread exit.", 1234567890, false);
}

void Line_send_message_thread(void* arg)
{
	Share_app_log_save("Line/Message send thread", "Thread started.", 1234567890, false);
	u8* message_send_buffer;
	u32 read_size = 0;
	int message_send_log_num_return;
	bool function_fail = false;
	char send_data[8192];
	Result_with_string message_send_result;
	message_send_buffer = (u8*)malloc(0x5000);

	httpcContext message_send_httpc;
	while (s_line_message_send_thread_run)
	{
		if (s_line_message_send_request)
		{
			message_send_log_num_return = Share_app_log_save("Line/Message send thread/httpc", "Sending message...", 1234567890, false);
			read_size = 0;
			function_fail = false;
			memset(send_data, 0x0, 8192);
			sprintf(send_data, "{ \"type\": \"send\",\"id\" : \"%s\",\"message\" : \"%s\" }", id[s_line_room_select_num].c_str(), input_text.c_str());

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
				httpcAddRequestHeaderField(&message_send_httpc, "User-Agent", s_httpc_user_agent.c_str());
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
			Share_app_log_add_result(message_send_log_num_return, message_send_result.string, message_send_result.code, false);

			s_line_message_send_request = false;
		}
		usleep(100000);

		while (s_line_thread_suspend)
			usleep(250000);
	}
	Share_app_log_save("Line/Message send thread", "Thread exit.", 1234567890, false);
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
	FS_DirectoryEntry fs_id_entry;
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

Result_with_string Line_load_log_from_sd(std::string file_name)
{
	u8* log_sd_buffer;
	u32 log_sd_read_size = 0;
	FS_Archive fs_log_sd_archive = 0;
	Handle fs_log_sd_handle = 0;
	Result_with_string load_log_sd_result;
	load_log_sd_result.code = 0;
	load_log_sd_result.string = "[Success] ";
	load_log_sd_result.error_description = "N/A";

	log_sd_buffer = (u8*)malloc(s_line_log_fs_buffer_size);
	if (log_sd_buffer == NULL)
	{
		load_log_sd_result.code = OUT_OF_MEMORY;
		load_log_sd_result.string = "[Error] Out of memory.";
		load_log_sd_result.error_description = "Couldn't malloc to 'log_sd_buffer'(" + std::to_string(s_line_log_fs_buffer_size / 1024) + "KB).";
	}
	else
	{
		memset(log_sd_buffer, 0x0, s_line_log_fs_buffer_size);
		line_log_data = "";
		load_log_sd_result = Share_load_from_file(file_name, log_sd_buffer, s_line_log_fs_buffer_size, &log_sd_read_size, "/Line/", fs_log_sd_handle, fs_log_sd_archive);

		if (load_log_sd_result.code == 0)
		{
			line_log_data = (char*)log_sd_buffer;
			data_parse_request = true;
		}
	}

	free(log_sd_buffer);
	return load_log_sd_result;
}

void Line_log_parse_thread(void* arg)
{
	Share_app_log_save("Line/Log parse thread", "Thread started.", 1234567890, false);
	int length_count = 0;
	int log_parse_log_num_return;
	int cut_length = 60;
	int clipboard_num;
	int text_length = 0;
	size_t image_url_end_pos;
	size_t image_url_start_pos;
	size_t message_start_pos;
	size_t message_next_pos;
	size_t new_line_pos;
	bool function_fail = false;
	char* parse_cache;
	std::string image_url_end = "</image_url>";
	std::string image_url_start = "<image_url>";
	std::string message_start = "";

	while (s_line_log_parse_thread_run)
	{
		if (s_line_thread_suspend)
			usleep(250000);
		else
		{
			if (data_parse_request)
			{
				log_parse_log_num_return = Share_app_log_save("Line/Log parse thread", "Parsing logs...", 1234567890, false);
				usleep(50000);

				message_start_pos = std::string::npos;
				message_next_pos = std::string::npos;
				image_url_start_pos = std::string::npos;
				image_url_end_pos = std::string::npos;
				new_line_pos = std::string::npos;
				number_of_message = 0;
				number_of_lines = 10;
				length_count = 0;
				clipboard_num = 1;
				cut_length = 60;
				text_length = 0;
				function_fail = false;

				parse_cache = (char*)malloc(0x10000);
				if (parse_cache == NULL)
				{
					Share_clear_error_message();
					Share_set_error_message("[Error] Out of memory.", "Couldn't malloc to 'parse_cache'(" + std::to_string(0x10000 / 1024) + "KB).", "Line/Log parse thread", OUT_OF_MEMORY);
					s_error_display = true;
					Share_app_log_save("Line/Log parse thread", "[Error] Out of memory.", OUT_OF_MEMORY, false);
				}
				else
				{
					for (int i = 1; i <= 14; i++)
						s_clipboards[i] = "";
					for (int i = 0; i <= 299; i++)
						s_line_message_log[i] = "";
					for (int i = 0; i < 59999; i++)
						message_log_short[i] = "";

					for (int i = 0; i <= 299; i++)
					{
						message_start_pos = line_log_data.find(message_start, (message_start_pos + message_start.length()));
						if (message_start_pos == std::string::npos)
							break;

						message_next_pos = line_log_data.find(message_start, (message_start_pos + message_start.length()));
						if (message_next_pos == std::string::npos)
							s_line_message_log[i] = line_log_data.substr((message_start_pos + message_start.length()), line_log_data.length() - (message_start_pos + message_start.length()));
						else
							s_line_message_log[i] = line_log_data.substr((message_start_pos + message_start.length()), message_next_pos - (message_start_pos + message_start.length()));

						image_url_start_pos = s_line_message_log[i].find(image_url_start);
						image_url_end_pos = s_line_message_log[i].find(image_url_end);
						if (!(image_url_start_pos == std::string::npos || image_url_end_pos == std::string::npos))
						{
							s_clipboards[clipboard_num] = s_line_message_log[i].substr((image_url_start_pos + image_url_start.length()), (image_url_end_pos - (image_url_start_pos + image_url_start.length())));
							clipboard_num++;
							if (clipboard_num > 14)
								clipboard_num = 1;
						}
						number_of_message++;
					}

					for (int i = 0; i < number_of_message; i++)
					{
						memset(parse_cache, 0x0, 0x10000);
						strcpy(parse_cache, s_line_message_log[i].c_str());
						text_length = s_line_message_log[i].length();

						while (true)
						{
							if (number_of_lines >= 59990)
							{
								function_fail = true;
								break;
							}

							if (length_count + cut_length >= text_length)
							{
								message_log_short[59999] = s_line_message_log[i].substr(length_count, cut_length);
								new_line_pos = message_log_short[59999].find_first_of("\u000a");
								if (!(new_line_pos == std::string::npos))
								{
									cut_length = new_line_pos + 1;
									number_of_lines++;
									message_log_short[number_of_lines] = s_line_message_log[i].substr(length_count, cut_length);
									length_count += cut_length;
									cut_length = 60;
								}
								else
								{
									cut_length = text_length - length_count;
									number_of_lines++;
									message_log_short[number_of_lines] = s_line_message_log[i].substr(length_count, cut_length);
									break;
								}
							}
							else
							{
								int check_length = mblen(&parse_cache[length_count + cut_length], 4);
								if (check_length >= 1)
								{
									message_log_short[59999] = s_line_message_log[i].substr(length_count, cut_length);
									new_line_pos = message_log_short[59999].find_first_of("\u000a");
									if (!(new_line_pos == std::string::npos))
										cut_length = new_line_pos + 1;

									number_of_lines++;
									message_log_short[number_of_lines] = s_line_message_log[i].substr(length_count, cut_length);
									length_count += cut_length;
									cut_length = 60;
								}
								else
									cut_length++;
							}
						}
						number_of_lines++;
						length_count = 0;
						cut_length = 60;

						s_line_bottom_y = (-text_interval * number_of_lines) + 100;
						text_y_cache = s_line_bottom_y;
					}

					s_line_bottom_y = (-text_interval * number_of_lines) + 100;
					text_y_cache = s_line_bottom_y;

					if (function_fail)
						Share_app_log_add_result(log_parse_log_num_return, "[Error] Parsing aborted due to too many messages.", 1234567890, false);
					else
						Share_app_log_add_result(log_parse_log_num_return, "[Success] ", 1234567890, false);
				}

				free(parse_cache);
				parse_cache = NULL;
				data_parse_request = false;
			}
			usleep(100000);
		}
	}
	Share_app_log_save("Line/Log parse thread", "Thread exit.", 1234567890, false);
}

void Line_log_load_thread(void* arg)
{
	Share_app_log_save("Line/Log load thread", "Thread started.", 1234567890, false);
	int log_load_log_return_num = 0;
	Result_with_string log_load_result;
	while (s_line_log_load_thread_run)
	{
		if (s_line_thread_suspend)
			usleep(250000);
		else
		{
			if (s_line_log_load_request)
			{
				log_load_result.code = 0;
				log_load_result.string = "[Success] ";
				log_load_result.error_description = "N/A";

				log_load_log_return_num = Share_app_log_save("Line/Log load thread/fs", "Load_from_log_sd(" + id[s_line_room_select_num].substr(0, 16) + ")...", 1234567890, false);
				log_load_result = Line_load_log_from_sd(id[s_line_room_select_num].substr(0, 16));
				Share_app_log_add_result(log_load_log_return_num, log_load_result.string, log_load_result.code, false);
				if (log_load_result.code != 0)
				{
					Share_clear_error_message();
					Share_set_error_message(log_load_result.string, log_load_result.error_description, "Line/Log load thread", log_load_result.code);
					s_error_display = true;
				}
				s_line_log_load_request = false;
			}
			usleep(100000);
		}
	}
	Share_app_log_save("Line/Log load thread", "Thread exit.", 1234567890, false);
}

void Line_exit(void)
{
	Share_app_log_save("Line/Exit", "Exiting...", 1234567890, s_debug_slow);
	u64 time_out = 10000000000;
	int exit_log_num_return;
	bool function_fail = false;
	Result_with_string exit_result;
	exit_result.code = 0;
	exit_result.string = "[Success] ";

	s_line_already_init = false;
	s_line_thread_suspend = false;
	s_line_log_download_thread_run = false;
	s_line_log_load_thread_run = false;
	s_line_log_parse_thread_run = false;
	s_line_message_send_thread_run = false;
	s_line_update_thread_run = false;


	exit_log_num_return = Share_app_log_save("Line/Exit", "Thread exiting(0/3)...", 1234567890, s_debug_slow);
	exit_result.code = threadJoin(log_download_thread, time_out);
	if (exit_result.code == 0)
		Share_app_log_add_result(exit_log_num_return, "[Success] ", exit_result.code, s_debug_slow);
	else
	{
		function_fail = true;
		Share_app_log_add_result(exit_log_num_return, "[Error] ", exit_result.code, s_debug_slow);
	}

	exit_log_num_return = Share_app_log_save("Line/Exit", "Thread exiting(1/3)...", 1234567890, s_debug_slow);
	exit_result.code = threadJoin(log_load_thread, time_out);
	if (exit_result.code == 0)
		Share_app_log_add_result(exit_log_num_return, "[Success] ", exit_result.code, s_debug_slow);
	else
	{
		function_fail = true;
		Share_app_log_add_result(exit_log_num_return, "[Error] ", exit_result.code, s_debug_slow);
	}

	exit_log_num_return = Share_app_log_save("Line/Exit", "Thread exiting(2/3)...", 1234567890, s_debug_slow);
	exit_result.code = threadJoin(log_parse_thread, time_out);
	if (exit_result.code == 0)
		Share_app_log_add_result(exit_log_num_return, "[Success] ", exit_result.code, s_debug_slow);
	else
	{
		function_fail = true;
		Share_app_log_add_result(exit_log_num_return, "[Error] ", exit_result.code, s_debug_slow);
	}

	exit_log_num_return = Share_app_log_save("Line/Exit", "Thread exiting(3/3)...", 1234567890, s_debug_slow);
	exit_result.code = threadJoin(message_send_thread, time_out);
	if (exit_result.code == 0)
		Share_app_log_add_result(exit_log_num_return, "[Success] ", exit_result.code, s_debug_slow);
	else
	{
		function_fail = true;
		Share_app_log_add_result(exit_log_num_return, "[Error] ", exit_result.code, s_debug_slow);
	}

	for (int i = 0; i <= 299; i++)
	{
		s_line_message_log[i] = "";
		s_line_message_log[i].reserve(1);
	}
	for (int i = 0; i <= 59999; i++)
	{
		message_log_short[i] = "";
		message_log_short[i].reserve(1);
	}

	line_log_data = "";
	line_log_data.reserve(1);

	if(function_fail)
		Share_app_log_save("Line/Exit", "[Warn] Some function returned error.", 1234567890, s_debug_slow);

	Share_app_log_save("Line/Exit", "Exited.", 1234567890, s_debug_slow);
}

