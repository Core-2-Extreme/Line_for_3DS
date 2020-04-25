#include <3ds.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include "hid.hpp"
#include "share_function.hpp"
#include "draw.hpp"
#include "httpc.hpp"
#include "google_translation.hpp"
#include "setting_menu.hpp"
#include "external_font.hpp"
#include "error.hpp"
#include "menu.hpp"

bool gtr_already_init;
bool gtr_main_run;
bool gtr_tr_thread_run;
bool gtr_thread_suspend;
bool gtr_type_text_request;
bool gtr_tr_request = false;
int gtr_current_history_num = 0;
int gtr_selected_history_num;
double gtr_text_pos_x;
std::string gtr_sorce_lang = "en";
std::string gtr_target_lang = "ja";
std::string gtr_input_text;
std::string gtr_history[17] = { "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a" };

Thread gtr_tr_thread;

bool Gtr_query_init_flag(void)
{
	return gtr_already_init;
}

bool Gtr_query_running_flag(void)
{
	return gtr_main_run;
}

int Gtr_query_selected_num(void)
{
	return gtr_selected_history_num;
}

double Gtr_query_text_pos_x(void)
{
	return gtr_text_pos_x;
}

std::string Gtr_query_tr_history(int num)
{
	if (num >= 0 && num <= 16)
		return gtr_history[num];
	else
		return "";
}

void Gtr_set_selected_num(int num)
{
	gtr_selected_history_num = num;
}

void Gtr_set_operation_flag(int operation_num, bool flag)
{
	if (operation_num == GTR_TYPE_TEXT_REQUEST)
		gtr_type_text_request = true;
}

void Gtr_set_text_pos_x(double x)
{
	gtr_text_pos_x = x;
}

void Gtr_suspend(void)
{
	gtr_thread_suspend = true;
	gtr_main_run = false;
	Menu_resume();
}

void Gtr_resume(void)
{
	Menu_suspend();
	gtr_thread_suspend = false;
	gtr_main_run = true;
}

std::string Gtr_get_lang_name(std::string short_name)
{
	int num_of_lang = 2;
	std::string lang_short_list[2] = { "en", "ja" };
	std::string lang_list[2] = { "English", "Japanese(日本語)" };

	for (int i = 0; i < num_of_lang; i++)
	{
		if (lang_short_list[i] == short_name)
			return lang_list[i];
	}
	return "Unknown";
}

void Gtr_tr_thread(void* arg)
{
	S_log_save("Gtr/Tr thread", "Thread started.", 1234567890, false);

	u8* httpc_buffer;
	u32 dl_size;
	u32 status_code;
	std::string send_data;
	std::string url = "https://script.google.com/macros/s/AKfycbwbL6swXIeycS-WpVBrfKLh40bXg2CAv1PdAzuIhalqq2SGrJM/exec";
	Result_with_string result;
	
	httpc_buffer = (u8*)malloc(0x10000);

	while (gtr_tr_thread_run)
	{
		if (gtr_thread_suspend)
			usleep(500000);
		else if (gtr_tr_request)
		{
			send_data = "{ \"text\": \"" + gtr_input_text + "\",\"sorce\" : \"" + gtr_sorce_lang + "\",\"target\" : \"" + gtr_target_lang + "\" }";
		
			result = Httpc_post_and_dl_data(url, (char*)send_data.c_str(), send_data.length(), httpc_buffer, 0x10000, &dl_size, &status_code, true);

			if (result.code == 0)
			{
				gtr_current_history_num++;
				if (gtr_current_history_num >= 16)
					gtr_current_history_num = 0;
				gtr_history[gtr_current_history_num] = (char*)httpc_buffer;
			}

			gtr_tr_request = false;
		}
		usleep(100000);
	}
	S_log_save("Gtr/Tr thread", "Thread exit.", 1234567890, false);
}

void Gtr_init(void)
{
	S_log_save("Gtr/init", "Initializing...", 1234567890, s_debug_slow);

	Draw_progress("0/0 [Gtr] Starting threads...");
	gtr_tr_thread_run = true;
	gtr_tr_thread = threadCreate(Gtr_tr_thread, (void*)(""), STACKSIZE, 0x26, -1, true);

	Gtr_resume();
	gtr_already_init = true;
	S_log_save("Gtr/init", "Initialized", 1234567890, s_debug_slow);
}

void Gtr_exit(void)
{
	S_log_save("Gtr/Exit", "Exiting...", 1234567890, s_debug_slow);
	u64 time_out = 10000000000;
	int log_num;
	bool failed = false;
	Result_with_string result;


	Draw_progress("0/0 [Gtr] Exiting threads...");
	gtr_already_init = false;
	gtr_tr_thread_run = false;
	gtr_thread_suspend = false;

	log_num = S_log_save("Gtr/Exit", "Exiting thread...", 1234567890, s_debug_slow);
	result.code = threadJoin(gtr_tr_thread, time_out);
	if (result.code == 0)
		S_log_add(log_num, "[Success] ", result.code, s_debug_slow);
	else
	{
		failed = true;
		S_log_add(log_num, "[Error] ", result.code, s_debug_slow);
	}

	if (failed)
		S_log_save("Gtr/Exit", "[Warn] Some function returned error.", 1234567890, s_debug_slow);

	S_log_save("Gtr/Exit", "Exited.", 1234567890, s_debug_slow);
}

void Gtr_main(void)
{
	osTickCounterUpdate(&s_tcount_frame_time);
	Draw_set_draw_mode(s_draw_vsync_mode);
	if (s_night_mode)
		Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);

	Draw_texture(Square_image, black_tint, 0, 0.0, 0.0, 400.0, 15.0);
	Draw_texture(Wifi_icon_image, dammy_tint, s_wifi_signal, 360.0, 0.0, 15.0, 15.0);
	Draw_texture(Battery_level_icon_image, dammy_tint, s_battery_level / 5, 330.0, 0.0, 30.0, 15.0);
	if (s_battery_charge)
		Draw_texture(Battery_charge_icon_image, dammy_tint, 0, 310.0, 0.0, 20.0, 15.0);
	Draw(s_status, 0.0f, 0.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 1.0f);
	Draw(s_battery_level_string, 337.5f, 1.25f, 0.4f, 0.4f, 0.0f, 0.0f, 0.0f, 0.5f);

	Draw(gtr_history[gtr_current_history_num], gtr_text_pos_x, 20.0, 0.45, 0.45, 0.25, 0.0, 0.5, 1.0);
	if (s_debug_mode)
		Draw_debug_info();
	if (s_app_logs_show)
	{
		for (int i = 0; i < 23; i++)
			Draw(s_app_logs[s_app_log_view_num + i], s_app_log_x, 10.0f + (i * 10), 0.4f, 0.4f, 0.0f, 0.5f, 1.0f, 1.0f);
	}

	if (s_night_mode)
		Draw_screen_ready_to_draw(1, true, 2, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(1, true, 2, 1.0, 1.0, 1.0);

	Draw(s_gtr_ver, 0.0, 0.0, 0.45, 0.45, 0.0, 1.0, 0.0, 1.0);
	for (int i = 0; i <= 15; i++)
	{
		if(i == gtr_selected_history_num)
			Draw(gtr_history[i], gtr_text_pos_x, 20.0 + (i * 10), 0.45, 0.45, 0.25, 0.0, 0.5, 1.0);
		else
			Draw(gtr_history[i], gtr_text_pos_x, 20.0 + (i * 10), 0.45, 0.45, 0.75, 0.5, 0.0, 1.0);
	}

	if (Sem_query_font_flag(SEM_USE_EXTERNAL_FONT))
	{
		Exfont_draw_external_fonts("Sorce : " + Gtr_get_lang_name(gtr_sorce_lang), 0.0, 190.0, 0.45, 0.45, false);
		Exfont_draw_external_fonts("Target : " + Gtr_get_lang_name(gtr_target_lang), 160.0, 190.0, 0.45, 0.45, false);
	}
	else if (Sem_query_font_flag(SEM_USE_SYSTEM_SPEIFIC_FONT))
	{
		Draw_with_specific_language("Sorce : " + Gtr_get_lang_name(gtr_sorce_lang), Sem_query_selected_num(SEM_SELECTED_LANG_NUM), 0.0, 190.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);
		Draw_with_specific_language("Target : " + Gtr_get_lang_name(gtr_target_lang), Sem_query_selected_num(SEM_SELECTED_LANG_NUM), 160.0, 190.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);
	}
	else if(Sem_query_font_flag(SEM_USE_DEFAULT_FONT))
	{
		Draw("Sorce : " + Gtr_get_lang_name(gtr_sorce_lang), 0.0, 190.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);
		Draw("Target : " + Gtr_get_lang_name(gtr_target_lang), 160.0, 190.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);
	}

	Draw_texture(Square_image, black_tint, 0, 0.0, 225.0, 320.0, 15.0);
	Draw(s_bot_button_string[1], 30.0f, 220.0f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 1.0f);

	if (Hid_query_key_held_state(KEY_H_TOUCH))
		Draw(s_circle_string, Hid_query_touch_pos(true), Hid_query_touch_pos(false), 0.20f, 0.20f, 1.0f, 0.0f, 0.0f, 1.0f);
	
	Draw_apply_draw();
	s_fps += 1;
	s_frame_time = osTickCounterRead(&s_tcount_frame_time);

	Hid_set_disable_flag(true);
	if (gtr_type_text_request)
	{
		memset(s_swkb_input_text, 0x0, 8192);
		swkbdInit(&s_swkb, SWKBD_TYPE_NORMAL, 2, 8192);
		swkbdSetHintText(&s_swkb, "メッセージを入力 / Type message here.");
		swkbdSetValidation(&s_swkb, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
		swkbdSetFeatures(&s_swkb, SWKBD_PREDICTIVE_INPUT);
		swkbdSetInitialText(&s_swkb, s_clipboards[0].c_str());
		swkbdSetLearningData(&s_swkb, &s_swkb_learn_data, true, true);
		s_swkb_press_button = swkbdInputText(&s_swkb, s_swkb_input_text, 8192);

		if (s_swkb_press_button == SWKBD_BUTTON_RIGHT)
		{
			gtr_input_text = s_swkb_input_text;
			gtr_tr_request = true;
		}
		gtr_type_text_request = false;
	}
}