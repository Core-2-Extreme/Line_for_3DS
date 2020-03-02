#include <3ds.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include "share_function.hpp"
#include "draw.hpp"

char google_tr_language_sorce[16] = "en";
char google_tr_language_target[16] = "ja";
bool google_tr_translation_request = false;
int google_tr_current_history_num = -1;
int google_tr_select_history_num = 0;
float google_tr_text_x = 0.0f;
std::string google_tr_history[17] = { "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a" };

Thread google_tr_translation_thread;

void Google_tr_translation(void* arg)
{
	Share_app_log_save("Google tr/Thread", "Google tr translation thread started.", 1234567890, false);

	u8* google_tr_httpc_buffer;
	u32 google_tr_download_size;
	char* google_tr_moved_url;
	char google_tr_send_data[8314];
	char google_tr_main_url[512] = "https://script.google.com/macros/s/AKfycbwbL6swXIeycS-WpVBrfKLh40bXg2CAv1PdAzuIhalqq2SGrJM/exec";
	Result google_tr_function_result;
	httpcContext httpc_google_tr;
	
	google_tr_httpc_buffer = (u8*)malloc(0x10000);
	google_tr_moved_url = (char*)malloc(0x500);

	while (s_gtr_tr_thread_run)
	{
		if (s_gtr_thread_suspend)
			usleep(500000);
		else if (google_tr_translation_request)
		{
			sprintf(google_tr_send_data, "{ \"text\": \"%s\",\"sorce\" : \"%s\",\"target\" : \"%s\" }", s_swkb_input_text, google_tr_language_sorce, google_tr_language_target);
			memset(google_tr_httpc_buffer, 0x0, 0x10000);

			httpcOpenContext(&httpc_google_tr, HTTPC_METHOD_POST, google_tr_main_url, 0);
			httpcSetSSLOpt(&httpc_google_tr, SSLCOPT_DisableVerify);
			httpcSetKeepAlive(&httpc_google_tr, HTTPC_KEEPALIVE_ENABLED);
			httpcAddRequestHeaderField(&httpc_google_tr, "Connection", "Keep-Alive");
			httpcAddRequestHeaderField(&httpc_google_tr, "Content-Type", "application/json");
			httpcAddRequestHeaderField(&httpc_google_tr, "User-Agent", s_httpc_user_agent.c_str());
			httpcAddPostDataRaw(&httpc_google_tr, (u32*)google_tr_send_data, strlen(google_tr_send_data));
			google_tr_function_result = httpcBeginRequest(&httpc_google_tr);
			if(google_tr_function_result != 0)
				Share_app_log_save("Google tr/Network", "httpcBeginRequest=", google_tr_function_result, false);


			google_tr_function_result = httpcGetResponseHeader(&httpc_google_tr, "Location", google_tr_moved_url, 0x500);
			if (google_tr_function_result != 0)
				Share_app_log_save("Google tr/Network", "httpcGetResponseHeader(Location)=", google_tr_function_result, false);
			httpcCloseContext(&httpc_google_tr);

			httpcOpenContext(&httpc_google_tr, HTTPC_METHOD_GET, google_tr_moved_url, 0);
			httpcSetSSLOpt(&httpc_google_tr, SSLCOPT_DisableVerify);
			httpcSetKeepAlive(&httpc_google_tr, HTTPC_KEEPALIVE_ENABLED);
			httpcAddRequestHeaderField(&httpc_google_tr, "Connection", "Keep-Alive");
			httpcAddRequestHeaderField(&httpc_google_tr, "User-Agent", s_httpc_user_agent.c_str());
			google_tr_function_result = httpcBeginRequest(&httpc_google_tr);
			if (google_tr_function_result != 0)
				Share_app_log_save("Google tr/Network", "httpcBeginRequest=", google_tr_function_result, false);

			
			google_tr_function_result = httpcDownloadData(&httpc_google_tr, google_tr_httpc_buffer, 0x10000, &google_tr_download_size);
			if (google_tr_function_result != 0)
				Share_app_log_save("Google tr/Network", "httpcDownloadData=", google_tr_function_result, false);
			httpcCloseContext(&httpc_google_tr);

			google_tr_current_history_num++;
			if (google_tr_current_history_num >= 16)
				google_tr_current_history_num = 0;
			google_tr_history[google_tr_current_history_num] = (char*)google_tr_httpc_buffer;			

			google_tr_translation_request = false;
		}
		usleep(100000);
	}
	Share_app_log_save("Google tr/Thread", "Google tr translation thread exit.", 1234567890, false);
}

void Google_tr_init(void)
{
	Share_app_log_save("Google tr/init", "Initializing...", 1234567890, true);
	s_gtr_tr_thread_run = true;
	google_tr_translation_thread = threadCreate(Google_tr_translation, (void*)(""), STACKSIZE, 0x26, -1, true);
	s_gtr_already_init = true;
	Share_app_log_save("Google tr/init", "Initialized", 1234567890, true);
}

void Google_tr_exit(void)
{
	Result google_tr_function_result;
	int google_tr_log_num_return = -1;

	google_tr_log_num_return = Share_app_log_save("Main/Thread", "Google tr translation thread exit...", 1234567890, true);
	google_tr_function_result = threadJoin(google_tr_translation_thread, 3000000);
	Share_app_log_add_result(google_tr_log_num_return, "", google_tr_function_result, true);
}

void Google_translation_main(void)
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

	Draw(google_tr_history[google_tr_current_history_num], google_tr_text_x, 20.0, 0.45, 0.45, 0.25, 0.0, 0.5, 1.0);
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

	Draw(s_gtr_ver, 0.0, 0.0, 0.45, 0.45, 0.0, 1.0, 0.0, 1.0);
	for (int i = 0; i <= 15; i++)
	{
		if(i == google_tr_select_history_num)
			Draw(google_tr_history[i], google_tr_text_x, 20.0 + (i * 10), 0.45, 0.45, 0.25, 0.0, 0.5, 1.0);
		else
			Draw(google_tr_history[i], google_tr_text_x, 20.0 + (i * 10), 0.45, 0.45, 0.75, 0.5, 0.0, 1.0);
	}
	Draw_texture(Background_image, dammy_tint, 1, 0.0, 225.0, 320.0, 15.0);
	Draw(s_bot_button_string[1], 30.0f, 220.0f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 1.0f);

	if (s_key_touch_held)
		Draw(s_circle_string, s_touch_pos_x, s_touch_pos_y, 0.20f, 0.20f, 1.0f, 0.0f, 0.0f, 1.0f);
	
	Draw_apply_draw();
	s_fps += 1;
	s_frame_time = osTickCounterRead(&s_tcount_frame_time);

	if (s_key_A_press)
	{
		memset(s_swkb_input_text, 0x0, 8192);
		swkbdInit(&s_swkb, SWKBD_TYPE_NORMAL, 2, 8192);
		swkbdSetHintText(&s_swkb, "メッセージを入力 / Type message here.");
		swkbdSetValidation(&s_swkb, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
		swkbdSetFeatures(&s_swkb, SWKBD_PREDICTIVE_INPUT);
		swkbdSetInitialText(&s_swkb, s_clipboards[0].c_str());
		swkbdSetDictWord(&s_swkb_words[0], "ぬべ", "壁|՞ةڼ)イーヒヒヒヒヒヒｗｗｗｗｗｗｗｗｗｗｗ");
		swkbdSetDictWord(&s_swkb_words[1], "ぬべ", "┌(☝┌՞ ՞)☝キエェェェエェェwwwww");
		swkbdSetDictWord(&s_swkb_words[2], "ぬべ", "┌(┌ ՞ةڼ)┐<ｷｴｪｪｪｴｴｪｪｪ");
		swkbdSetDictWord(&s_swkb_words[3], "ぬべ", "└(՞ةڼ◔)」");
		swkbdSetDictWord(&s_swkb_words[4], "ぬべ", "(  ՞ةڼ  )");
		swkbdSetDictWord(&s_swkb_words[5], "ぬべ", "└(՞ةڼ◔)」");
		swkbdSetDictWord(&s_swkb_words[6], "びぇ", "。゜( ;⊃՞ةڼ⊂; )゜。びぇぇえええんｗｗｗｗ");
		swkbdSetDictWord(&s_swkb_words[7], "うえ", "(✌ ՞ةڼ ✌ )");
		swkbdSetDictionary(&s_swkb, s_swkb_words, 8);

		swkbdSetLearningData(&s_swkb, &s_swkb_learn_data, true, true);
		s_swkb_press_button = swkbdInputText(&s_swkb, s_swkb_input_text, 8192);

		if (s_swkb_press_button == SWKBD_BUTTON_RIGHT)
			google_tr_translation_request = true;
	}
	else if (s_key_DRIGHT_held)
		google_tr_text_x -= 1.0;
	else if (s_key_DLEFT_held)
		google_tr_text_x += 1.0;
	else if (s_key_DUP_held)
	{
		google_tr_select_history_num--;
		if (google_tr_select_history_num <= -1)
			google_tr_select_history_num = 0;
	}
	else if (s_key_DDOWN_held)
	{
		google_tr_select_history_num++;
		if (google_tr_select_history_num >= 16)
			google_tr_select_history_num = 15;
	}
	else if (s_key_ZL_press)
		s_clipboards[0] = google_tr_history[google_tr_select_history_num];
	else if (s_key_START_press || (s_key_touch_press && s_touch_pos_x >= 110 && s_touch_pos_x <= 230 && s_touch_pos_y >= 220 && s_touch_pos_y <= 240))
	{
		s_gtr_thread_suspend = true;
		s_menu_main_run = true;
		s_gtr_main_run = false;
	}
}