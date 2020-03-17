#include <string>
#include <cstring>
#include <unistd.h>
#include <3ds.h>

#include "draw.hpp"
#include "speedtest.hpp"
#include "httpc.hpp"
#include "share_function.hpp"

int speedtest_total_download_size = 0;
float speedtest_total_download_time = 0.0;
float speedtest_test_result = 0.0;

Thread speedtest_network_thread;

void Speedtest_init(void)
{
	Share_app_log_save("Spt/Init", "Initializing...", 1234567890, s_debug_slow);
	s_spt_already_init = true;

	s_spt_thread_run = true;
	speedtest_network_thread = threadCreate(Speedtest_network, (void*)(""), STACKSIZE, 0x26, -1, true);
	Share_app_log_save("Spt/Init", "Initialized.", 1234567890, s_debug_slow);
}

void Speedtest_main(void)
{
	float text_red;
	float text_green;
	float text_blue;
	float text_alpha;

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

	osTickCounterUpdate(&s_tcount_frame_time);

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

	if (s_setting[1] == "en")
	{
		Draw(s_spt_message_en[0] + std::to_string(speedtest_total_download_size / (1024 * 1024)) + "MB(" + std::to_string(speedtest_total_download_size / 1024) + "KB)", 0.0f, 20.0f, 0.75f, 0.75f, 0.25f, 0.0f, 1.0f, 1.0f);
		Draw(s_spt_message_en[1] + std::to_string(speedtest_total_download_time) + " ms", 0.0f, 40.0f, 0.75f, 0.75f, 0.25f, 0.0f, 1.0f, 1.0f);
		Draw(s_spt_message_en[2] + std::to_string((speedtest_test_result / (1024 * 1024)) * 8) + "Mbps", 0.0f, 60.0f, 1.0f, 1.0f, 0.25f, 0.0f, 1.0f, 1.0f);
	}
	else if (s_setting[1] == "jp")
	{
		Draw(s_spt_message_jp[0] + std::to_string(speedtest_total_download_size / (1024 * 1024)) + "MB(" + std::to_string(speedtest_total_download_size / 1024) + "KB)", 0.0f, 20.0f, 0.75f, 0.75f, 0.25f, 0.0f, 1.0f, 1.0f);
		Draw(s_spt_message_jp[1] + std::to_string(speedtest_total_download_time) + " ms", 0.0f, 40.0f, 0.75f, 0.75f, 0.25f, 0.0f, 1.0f, 1.0f);
		Draw(s_spt_message_jp[2] + std::to_string((speedtest_test_result / (1024 * 1024)) * 8) + "Mbps", 0.0f, 60.0f, 1.0f, 1.0f, 0.25f, 0.0f, 1.0f, 1.0f);
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

	if (s_night_mode)
		Draw_screen_ready_to_draw(1, true, 1, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(1, true, 1, 1.0, 1.0, 1.0);

	Draw(s_spt_ver, 0.0, 0.0, 0.45, 0.45, 0.0, 1.0, 0.0, 1.0);

	if (s_setting[1] == "en")
	{
		Draw(s_spt_message_en[3], 70.0, 10.0, 0.75, 0.75, 0.0, 0.0, 0.0, 1.0);
		for (int i = 0; i < 7; i++)
		{
			if (s_spt_data_size  == i)
				Draw(s_spt_message_en[4 + i], 125.0, 40.0 + (i * 20), 0.5, 0.5, 1.0, 0.0, 0.5, 1.0);
			else
				Draw(s_spt_message_en[4 + i], 125.0, 40.0 + (i * 20), 0.5, 0.5, 0.0, 1.0, 1.0, 1.0);
		}
		Draw(s_spt_message_en[11], 150.0, 190.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
	}
	else if (s_setting[1] == "jp")
	{
		Draw(s_spt_message_jp[3], 75.0, 10.0, 0.75, 0.75, 0.0, 0.0, 0.0, 1.0);
		for (int i = 0; i < 7; i++)
		{
			if (s_spt_data_size  == i)
				Draw(s_spt_message_jp[4 + i], 135.0, 40.0 + (i * 20), 0.5, 0.5, 1.0, 0.0, 0.5, 1.0);
			else
				Draw(s_spt_message_jp[4 + i], 135.0, 40.0 + (i * 20), 0.5, 0.5, 0.0, 1.0, 1.0, 1.0);
		}
		Draw(s_spt_message_jp[11], 150.0, 190.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
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

void Speedtest_network(void* arg)
{
	Share_app_log_save("Spt/Spt thread", "Thread started.", 1234567890, false);

	u8* speedtest_httpc_buffer;
	u32 download_size_u32;
	u32 status_code;
	int speedtest_log_num_return;
	float speedtest_download_time;
	std::string speedtest_url[8] = { "http://v2.musen-lan.com/flash/test_001.swf", "http://v2.musen-lan.com/flash/test_002.swf", "http://v2.musen-lan.com/flash/test_004.swf", "http://v2.musen-lan.com/flash/test_008.swf", "http://v2.musen-lan.com/flash/test_016.swf", "http://v2.musen-lan.com/flash/test_032.swf", "http://v2.musen-lan.com/flash/test_064.swf", "http://v2.musen-lan.com/flash/test_128.swf" };
	Result_with_string speedtest_result;
	TickCounter speedtest_time;

	while (s_spt_thread_run)
	{
		if (s_spt_start_request)
		{
			download_size_u32 = 0;
			status_code = 0;
			speedtest_total_download_time = 0;
			speedtest_total_download_size = 0;
			speedtest_httpc_buffer = (u8*)malloc(s_spt_spt_httpc_buffer_size);

			if (speedtest_httpc_buffer == NULL)
			{
				Share_clear_error_message();
				Share_set_error_message("[Error] Out of memory.", "Couldn't malloc to 'speedtest_httpc_buffer'(" + std::to_string(s_spt_spt_httpc_buffer_size / 1024) + "KB).", "Spt/Spt thread", OUT_OF_MEMORY);
				s_error_display = true;
				Share_app_log_save("Spt/Spt thread", "[Error] Out of memory.", OUT_OF_MEMORY, false);
			}
			else
			{
				for (int i = 0; i <= 9; i++)
				{
					speedtest_download_time = 0.0000001;
					speedtest_log_num_return = Share_app_log_save("Spt/Spt thread/httpc", "Downloading test data(" + std::to_string(i) + "/9)...", 1234567890, false);
					osTickCounterStart(&speedtest_time);
					speedtest_result = Httpc_dl_data(speedtest_url[s_spt_data_size], speedtest_httpc_buffer, s_spt_spt_httpc_buffer_size, &download_size_u32, &status_code, true);
					osTickCounterUpdate(&speedtest_time);
					speedtest_download_time = osTickCounterRead(&speedtest_time);
					Share_app_log_add_result(speedtest_log_num_return, speedtest_result.string, speedtest_result.code, false);

					speedtest_total_download_size += download_size_u32;
					speedtest_total_download_time += speedtest_download_time;
					speedtest_test_result = speedtest_total_download_size / (speedtest_total_download_time / 1000);
					if (speedtest_result.code != 0)
					{
						Share_clear_error_message();
						Share_set_error_message(speedtest_result.string, speedtest_result.error_description, "Spt/Spt thread/httpc", speedtest_result.code);
						s_error_display = true;
						break;
					}
				}
			}

			free(speedtest_httpc_buffer);
			speedtest_httpc_buffer = NULL;
			s_spt_start_request = false;
		}
		usleep(100000);

		while (s_spt_thread_suspend)
			usleep(250000);
	}
	Share_app_log_save("Spt/Spt thread", "Thread exit.", 1234567890, false);
}

void Speedtest_exit(void)
{
	s_spt_already_init = false;
	s_spt_thread_suspend = false;
	s_spt_thread_run = false;
}