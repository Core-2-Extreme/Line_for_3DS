#include <string>
#include <cstring>
#include <unistd.h>
#include <3ds.h>

#include "draw.hpp"
#include "speedtest.hpp"
#include "share_function.hpp"
#define STACKSIZE (24 * 1024)

bool speed_test_start_request = false;
int speed_test_data_size = 0;
int speed_test_total_download_size = 0;
float speed_test_total_download_time = 0.0;
float speed_test_test_result = 0.0;

Thread speed_test_network_thread;

void Speed_test_init(void)
{
	Share_app_log_save("Speedtest/Init", "Initializing...", 1234567890, true);
	s_spt_already_init = true;

	s_spt_thread_run = true;
	speed_test_network_thread = threadCreate(Speed_test_network, (void*)(""), STACKSIZE, 0x26, -1, true);
	Share_app_log_save("Speedtest/Init", "Initialized.", 1234567890, true);
}

void Speed_test_main(void)
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
		Draw(s_spt_message_en[0] + std::to_string(speed_test_total_download_size / (1024 * 1024)) + "MB(" + std::to_string(speed_test_total_download_size / 1024) + "KB)", 0.0f, 20.0f, 0.75f, 0.75f, 0.25f, 0.0f, 1.0f, 1.0f);
		Draw(s_spt_message_en[1] + std::to_string(speed_test_total_download_time) + " ms", 0.0f, 40.0f, 0.75f, 0.75f, 0.25f, 0.0f, 1.0f, 1.0f);
		Draw(s_spt_message_en[2] + std::to_string((speed_test_test_result / (1024 * 1024)) * 8) + "Mbps", 0.0f, 60.0f, 1.0f, 1.0f, 0.25f, 0.0f, 1.0f, 1.0f);
	}
	else if (s_setting[1] == "jp")
	{
		Draw(s_spt_message_jp[0] + std::to_string(speed_test_total_download_size / (1024 * 1024)) + "MB(" + std::to_string(speed_test_total_download_size / 1024) + "KB)", 0.0f, 20.0f, 0.75f, 0.75f, 0.25f, 0.0f, 1.0f, 1.0f);
		Draw(s_spt_message_jp[1] + std::to_string(speed_test_total_download_time) + " ms", 0.0f, 40.0f, 0.75f, 0.75f, 0.25f, 0.0f, 1.0f, 1.0f);
		Draw(s_spt_message_jp[2] + std::to_string((speed_test_test_result / (1024 * 1024)) * 8) + "Mbps", 0.0f, 60.0f, 1.0f, 1.0f, 0.25f, 0.0f, 1.0f, 1.0f);
	}

	if (s_debug_mode)
	{
		Draw_texture(Square_image, dammy_tint, 9, 0.0, 50.0, 230.0, 140.0);
		Draw("Key A press : " + std::to_string(s_key_A_press) + " Key A held : " + std::to_string(s_key_A_held), 0.0, 50.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key B press : " + std::to_string(s_key_B_press) + " Key B held : " + std::to_string(s_key_B_held), 0.0, 60.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key X press : " + std::to_string(s_key_X_press) + " Key X held : " + std::to_string(s_key_X_held), 0.0, 70.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key Y press : " + std::to_string(s_key_Y_press) + " Key Y held : " + std::to_string(s_key_Y_held), 0.0, 80.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key CPAD DOWN held : " + std::to_string(s_key_CPAD_DOWN_held), 0.0, 90.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key CPAD RIGHT held : " + std::to_string(s_key_CPAD_RIGHT_held), 0.0, 100.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key CPAD UP held : " + std::to_string(s_key_CPAD_UP_held), 0.0, 110.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Key CPAD LEFT held : " + std::to_string(s_key_CPAD_LEFT_held), 0.0, 120.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Touch pos x : " + std::to_string(s_touch_pos_x) + " Touch pos y : " + std::to_string(s_touch_pos_y), 0.0, 130.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("X moved value : " + std::to_string(s_touch_pos_x_moved) + " Y moved value : " + std::to_string(s_touch_pos_y_moved), 0.0, 140.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Held time : " + std::to_string(s_held_time), 0.0, 150.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Drawing time(CPU/per frame) : " + std::to_string(C3D_GetProcessingTime()) + "ms", 0.0, 160.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Drawing time(GPU/per frame) : " + std::to_string(C3D_GetDrawingTime()) + "ms", 0.0, 170.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw("Free RAM (estimate) " + std::to_string(s_free_ram) + " MB", 0.0f, 180.0f, 0.4f, 0.4, text_red, text_green, text_blue, text_alpha);
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
		for (int i = 1; i <= 7; i++)
		{
			if ((speed_test_data_size + 1) == i)
				Draw(s_spt_message_en[3 + i], 125.0, 20.0 + (i * 20), 0.5, 0.5, 1.0, 0.0, 0.5, 1.0);
			else
				Draw(s_spt_message_en[3 + i], 125.0, 20.0 + (i * 20), 0.5, 0.5, 0.0, 1.0, 1.0, 1.0);
		}
		Draw(s_spt_message_en[11], 150.0, 190.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
	}
	else if (s_setting[1] == "jp")
	{
		Draw(s_spt_message_jp[3], 75.0, 10.0, 0.75, 0.75, 0.0, 0.0, 0.0, 1.0);
		for (int i = 1; i <= 7; i++)
		{
			if ((speed_test_data_size + 1) == i)
				Draw(s_spt_message_jp[3 + i], 135.0, 20.0 + (i * 20), 0.5, 0.5, 1.0, 0.0, 0.5, 1.0);
			else
				Draw(s_spt_message_jp[3 + i], 135.0, 20.0 + (i * 20), 0.5, 0.5, 0.0, 1.0, 1.0, 1.0);
		}
		Draw(s_spt_message_jp[11], 150.0, 190.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
	}

	Draw_texture(Background_image, dammy_tint, 1, 0.0, 225.0, 320.0, 15.0);
	Draw(s_bot_button_string[1], 30.0f, 220.0f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 1.0f);
	if (s_key_touch_held)
		Draw(s_circle_string, s_touch_pos_x, s_touch_pos_y, 0.20f, 0.20f, 1.0f, 0.0f, 0.0f, 1.0f);

	Draw_apply_draw();
	s_fps += 1;
	s_frame_time = osTickCounterRead(&s_tcount_frame_time);
	if (s_key_A_press || (s_touch_pos_x >= 150 && s_touch_pos_x <= 170 && s_touch_pos_y >= 190 && s_touch_pos_y <= 209))
		speed_test_start_request = true;
	if (s_key_touch_held)
	{
		if (s_touch_pos_x >= 100 && s_touch_pos_x <= 230 && s_touch_pos_y >= 40 && s_touch_pos_y <= 59)
			speed_test_data_size = 0;
		else if (s_touch_pos_x >= 100 && s_touch_pos_x <= 230 && s_touch_pos_y >= 60 && s_touch_pos_y <= 79)
			speed_test_data_size = 1;
		else if (s_touch_pos_x >= 100 && s_touch_pos_x <= 230 && s_touch_pos_y >= 80 && s_touch_pos_y <= 99)
			speed_test_data_size = 2;
		else if (s_touch_pos_x >= 100 && s_touch_pos_x <= 230 && s_touch_pos_y >= 100 && s_touch_pos_y <= 119)
			speed_test_data_size = 3;
		else if (s_touch_pos_x >= 100 && s_touch_pos_x <= 230 && s_touch_pos_y >= 120 && s_touch_pos_y <= 139)
			speed_test_data_size = 4;
		else if (s_touch_pos_x >= 100 && s_touch_pos_x <= 230 && s_touch_pos_y >= 140 && s_touch_pos_y <= 159)
			speed_test_data_size = 5;
		else if (s_touch_pos_x >= 100 && s_touch_pos_x <= 230 && s_touch_pos_y >= 160 && s_touch_pos_y <= 179)
			speed_test_data_size = 6;
	}
	if (s_key_START_press || (s_key_touch_press && s_touch_pos_x >= 110 && s_touch_pos_x <= 230 && s_touch_pos_y >= 220 && s_touch_pos_y <= 240))
	{
		s_spt_thread_suspend = true;
		s_menu_main_run = true;
		s_spt_main_run = false;
	}
}

void Speed_test_network(void* arg)
{
	Share_app_log_save("Speedtest/Speedtest thread", "Thread started.", 1234567890, false);

	u8* speed_test_httpc_buffer;
	u32 speed_test_download_size_u32;
	int speed_test_log_num_return;
	float speed_test_download_time;
	bool function_fail = false;
	std::string speed_test_url[8] = { "http://v2.musen-lan.com/flash/test_001.swf", "http://v2.musen-lan.com/flash/test_002.swf", "http://v2.musen-lan.com/flash/test_004.swf", "http://v2.musen-lan.com/flash/test_008.swf", "http://v2.musen-lan.com/flash/test_016.swf", "http://v2.musen-lan.com/flash/test_032.swf", "http://v2.musen-lan.com/flash/test_064.swf", "http://v2.musen-lan.com/flash/test_128.swf" };
	Result_with_string speed_test_result;
	httpcContext speed_test_httpc;
	TickCounter speed_test_time;

	while (s_spt_thread_run)
	{
		if (s_spt_thread_suspend || (!speed_test_start_request))
			usleep(500000);
		else
		{
			speed_test_httpc_buffer = (u8*)malloc(0x1000000);
			memset(speed_test_httpc_buffer, 0x0, 0x1000000);
			speed_test_download_size_u32 = 0;
			speed_test_total_download_time = 0;
			speed_test_total_download_size = 0;
			
			for (int i = 0; i <= 9; i++)
			{
				speed_test_download_time = 0;
				function_fail = false;
				speed_test_result.code = 0;
				speed_test_result.string = "[Success] ";
				speed_test_log_num_return = Share_app_log_save("Speedtest/Speedtest thread/httpc", "Downloading test data(" + std::to_string(i) + "/9)...", 1234567890, false);

				speed_test_result.code = httpcOpenContext(&speed_test_httpc, HTTPC_METHOD_GET, speed_test_url[speed_test_data_size].c_str(), 0);
				if (speed_test_result.code != 0)
				{
					function_fail = true;
					speed_test_result.string = "[Error] httpcOpenContext failed. ";
				}

				if (!function_fail)
				{
					speed_test_result.code = httpcSetSSLOpt(&speed_test_httpc, SSLCOPT_DisableVerify);
					if (speed_test_result.code != 0)
					{
						function_fail = true;
						speed_test_result.string = "[Error] httpcOpenContext failed. ";
					}
				}

				if (!function_fail)
				{
					speed_test_result.code = httpcSetKeepAlive(&speed_test_httpc, HTTPC_KEEPALIVE_ENABLED);
					if (speed_test_result.code != 0)
					{
						function_fail = true;
						speed_test_result.string = "[Error] httpcOpenContext failed. ";
					}
				}

				if(!function_fail)
				{
					httpcAddRequestHeaderField(&speed_test_httpc, "Connection", "Keep-Alive");
					httpcAddRequestHeaderField(&speed_test_httpc, "User-Agent", s_httpc_user_agent.c_str());
					httpcAddRequestHeaderField(&speed_test_httpc, "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3");
					httpcAddRequestHeaderField(&speed_test_httpc, "Accept-Encoding", "en,en-US;q=1,ja;q=0.9");
					httpcAddRequestHeaderField(&speed_test_httpc, "Accept-Language", "gzip, deflate, br");

					speed_test_result.code = httpcBeginRequest(&speed_test_httpc);
					if (speed_test_result.code != 0)
					{
						function_fail = true;
						speed_test_result.string = "[Error] httpcOpenContext failed. ";
					}
				}

				if (!function_fail)
				{
					osTickCounterStart(&speed_test_time);
					speed_test_result.code = httpcDownloadData(&speed_test_httpc, speed_test_httpc_buffer, 0x1000000, &speed_test_download_size_u32);
					osTickCounterUpdate(&speed_test_time);
					speed_test_download_time = osTickCounterRead(&speed_test_time);
					if (speed_test_result.code != 0)
					{
						function_fail = true;
						speed_test_result.string = "[Error] httpcDownloadData failed. ";
					}
				}

				speed_test_total_download_size += speed_test_download_size_u32;
				speed_test_total_download_time += speed_test_download_time;
				speed_test_test_result = speed_test_total_download_size / (speed_test_total_download_time / 1000);

				httpcCloseContext(&speed_test_httpc);
				Share_app_log_add_result(speed_test_log_num_return, speed_test_result.string, speed_test_result.code, false);
			}
			speed_test_start_request = false;
			free(speed_test_httpc_buffer);
		}
	}
	Share_app_log_save("Speedtest/Speedtest thread", "Thread exit.", 1234567890, false);
}

void Speed_test_exit(void)
{
	s_spt_already_init = false;
	s_spt_thread_run = false;
}