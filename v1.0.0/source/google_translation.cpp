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
	u8* google_tr_httpc_buffer;
	u32 google_tr_download_size;
	char* google_tr_moved_url;
	char google_tr_send_data[8314];
	char google_tr_main_url[512] = "https://script.google.com/macros/s/AKfycbwbL6swXIeycS-WpVBrfKLh40bXg2CAv1PdAzuIhalqq2SGrJM/exec";
	Result google_tr_function_result;
	httpcContext httpc_google_tr;
	
	google_tr_httpc_buffer = (u8*)malloc(0x10000);
	google_tr_moved_url = (char*)malloc(0x500);

	Share_app_log_save("Google tr/Thread", "Google tr translation thread start", 1234567890, true);
	while (share_google_tr_tr_thread_run)
	{
		if (share_google_tr_thread_suspend)
			usleep(500000);
		else if (google_tr_translation_request)
		{
			sprintf(google_tr_send_data, "{ \"text\": \"%s\",\"sorce\" : \"%s\",\"target\" : \"%s\" }", share_swkb_input_text, google_tr_language_sorce, google_tr_language_target);
			memset(google_tr_httpc_buffer, 0x0, 0x10000);

			httpcOpenContext(&httpc_google_tr, HTTPC_METHOD_POST, google_tr_main_url, 0);
			httpcSetSSLOpt(&httpc_google_tr, SSLCOPT_DisableVerify);
			httpcSetKeepAlive(&httpc_google_tr, HTTPC_KEEPALIVE_ENABLED);
			httpcAddRequestHeaderField(&httpc_google_tr, "Connection", "Keep-Alive");
			httpcAddRequestHeaderField(&httpc_google_tr, "Content-Type", "application/json");
			httpcAddRequestHeaderField(&httpc_google_tr, "User-Agent", "Google trnslation for 3DS v1.0.0");
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
			httpcAddRequestHeaderField(&httpc_google_tr, "User-Agent", "Google trnslation for 3DS v1.0.0");
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
	Share_app_log_save("Google tr/Thread", "Google tr translation thread exit", 1234567890, true);
}

void Google_tr_init(void)
{
	share_google_tr_already_init = true;
	share_google_tr_tr_thread_run = true;
	google_tr_translation_thread = threadCreate(Google_tr_translation, (void*)(""), STACKSIZE, 0x26, -1, true);
}

void Google_tr_exit(void)
{
	Result google_tr_function_result;
	int google_tr_log_num_return = -1;

	google_tr_log_num_return = Share_app_log_save("Main/Thread", "Google tr translation thread exit...", 1234567890, true);
	google_tr_function_result = threadJoin(google_tr_translation_thread, share_thread_exit_time_out);
	Share_app_log_add_result(google_tr_log_num_return, "", google_tr_function_result, true);
}

void Google_translation_main(void)
{
	osTickCounterUpdate(&share_total_frame_time);
	Draw_set_draw_mode(0);
	Draw_screen_ready_to_draw(0, true, 1);

	Draw_texture(TEXTURE_BACKGROUND, 0, 0.0, 0.0);
	Draw_texture(TEXTURE_WIFI_ICON, share_wifi_signal, 360.0, 0.0);
	Draw_texture(TEXTURE_BATTERY_LEVEL_ICON, share_battery_level / 5, 330.0, 0.0);
	if (share_battery_charge)
		Draw_texture(TEXTURE_BATTERY_CHARGE_ICON, 0, 310.0, 0.0);
	Draw(share_status, 0.0f, 0.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 1.0f);
	Draw(share_battery_level_string, 337.5f, 1.25f, 0.4f, 0.4f, 0.0f, 0.0f, 0.0f, 0.5f);

	Draw(google_tr_history[google_tr_current_history_num], google_tr_text_x, 20.0, 0.45, 0.45, 0.25, 0.0, 0.5, 1.0);
	if (share_app_logs_show)
	{
		for (int i = share_app_log_view_num; i < share_app_log_view_num + 23; i++)
			Draw(share_app_logs[i], share_app_log_x, 10.0f + (i * 10), 0.4f, 0.4f, 0.0f, 0.5f, 1.0f, 1.0f);
	}

	Draw_screen_ready_to_draw(1, true, 1);
	Draw(share_google_translation_ver, 0.0, 0.0, 0.45, 0.45, 0.0, 1.0, 0.0, 1.0);
	for (int i = 0; i <= 15; i++)
	{
		if(i == google_tr_select_history_num)
			Draw(google_tr_history[i], google_tr_text_x, 20.0 + (i * 10), 0.45, 0.45, 0.25, 0.0, 0.5, 1.0);
		else
			Draw(google_tr_history[i], google_tr_text_x, 20.0 + (i * 10), 0.45, 0.45, 0.75, 0.5, 0.0, 1.0);
	}
	Draw_texture(TEXTURE_BACKGROUND, 1, 0.0, 225.0);
	Draw(share_bot_button_string, 30.0f, 220.0f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 1.0f);

	if (share_key_touch_held)
		Draw("●", touch_pos.px, touch_pos.py, 0.20f, 0.20f, 1.0f, 0.0f, 0.0f, 1.0f);
	
	Draw_apply_draw();
	share_fps += 1;
	share_frame_time_point[0] = osTickCounterRead(&share_total_frame_time);

	if (share_key_A_press)
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

		swkbdSetLearningData(&share_swkb, &share_swkb_learn_data, true, true);
		share_swkb_press_button = swkbdInputText(&share_swkb, share_swkb_input_text, 8192);

		if (share_swkb_press_button == SWKBD_BUTTON_RIGHT)
			google_tr_translation_request = true;
	}
	else if (share_key_DRIGHT_held)
		google_tr_text_x -= 1.0;
	else if (share_key_DLEFT_held)
		google_tr_text_x += 1.0;
	else if (share_key_DUP_held)
	{
		google_tr_select_history_num--;
		if (google_tr_select_history_num <= -1)
			google_tr_select_history_num = 0;
	}
	else if (share_key_DDOWN_held)
	{
		google_tr_select_history_num++;
		if (google_tr_select_history_num >= 16)
			google_tr_select_history_num = 15;
	}
	else if (share_key_ZL_press)
		share_clipboard[0] = google_tr_history[google_tr_select_history_num];
	else if (share_key_START_press || (share_key_touch_press && share_touch_pos_x >= 110 && share_touch_pos_x <= 230 && share_touch_pos_y >= 220 && share_touch_pos_y <= 240))
	{
		share_google_tr_thread_suspend = true;
		share_menu_main_run = true;
		share_google_tr_main_run = false;
	}
}