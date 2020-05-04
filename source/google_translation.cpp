#include <3ds.h>
#include <string>
#include <unistd.h>

#include "hid.hpp"
#include "share_function.hpp"
#include "draw.hpp"
#include "httpc.hpp"
#include "google_translation.hpp"
#include "setting_menu.hpp"
#include "error.hpp"
#include "menu.hpp"
#include "log.hpp"
#include "types.hpp"

bool gtr_already_init = false;
bool gtr_main_run = false;
bool gtr_tr_thread_run = false;
bool gtr_thread_suspend = false;
bool gtr_type_text_request = false;
bool gtr_tr_request = false;
bool gtr_select_sorce_lang_request = false;
bool gtr_select_target_lang_request = false;
int gtr_current_history_num = 9;
int gtr_selected_history_num = 0;
double gtr_selected_sorce_lang_num = 0;
double gtr_selected_target_lang_num = 0;
double gtr_sorce_lang_offset = 0;
double gtr_target_lang_offset = 0;
double gtr_text_pos_x = 0.0;
std::string gtr_sorce_lang = "en";
std::string gtr_target_lang = "ja";
std::string gtr_input_text = "n/a";
std::string gtr_history[10] = { "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", };
std::string gtr_lang_short_list[105] = { 
	"auto",
	"af",
	"sq", 
	"am", 
	"ar", 
	"hy", 
	"az", 
	"eu", 
	"be",
	"bn", 
	"bs", 
	"bg", 
	"ca", 
	"ceb", 
	"zh-CN",
	"zh-TW",
	"co",
	"hr", 
	"cs", 
	"da", 
	"nl", 
	"en", 
	"eo", 
	"et", 
	"fi", 
	"fr", 
	"fy", 
	"gl", 
	"ka", 
	"de", 
	"el", 
	"gu", 
	"ht", 
	"ha", 
	"haw", 
	"he", 
	"hi", 
	"hmn",
	"hu", 
	"is",
	"ig", 
	"id", 
	"ga", 
	"it", 
	"ja", 
	"jv", 
	"kn", 
	"kk", 
	"km", 
	"ko", 
	"ku", 
	"ky", 
	"lo", 
	"la", 
	"lv", 
	"lt", 
	"lb", 
	"mk", 
	"mg", 
	"ms", 
	"ml", 
	"mt", 
	"mi", 
	"mr", 
	"mn", 
	"my",
	"ne",
	"no", 
	"ny", 
	"ps", 
	"fa", 
	"pl", 
	"pt", 
	"pa", 
	"ro", 
	"ru", 
	"sm", 
	"gd", 
	"sr", 
	"st", 
	"sn", 
	"sd", 
	"si", 
	"sk", 
	"sl", 
	"so", 
	"es", 
	"su", 
	"sw", 
	"sv", 
	"tl", 
	"tg",
	"ta",
	"te", 
	"th", 
	"tr", 
	"uk", 
	"ur", 
	"uz", 
	"vi", 
	"cy", 
	"xh", 
	"yi", 
	"yo",
	"zu",
 };
std::string gtr_lang_list[105] = {
	"Auto",
	"Afrikaans",
	"Albanian(shqip)",
	"Amharic(አማርኛ)",
	"Arabic(اَلْعَرَبِيَّةُ)",
	"Armenian(հայերեն)",
	"Azerbaijani(Azərbaycan)",
	"Basque(Euskara)",
	"Belarusian(беларуская)",
	"Bengali(বাংলা)",
	"Bosnian(bosanski)",
	"Bulgarian(български)",
	"Catalan(Català)",
	"Cebuano(Sebwano)",
	"Chinese Simplified(汉语)",
	"Chinese Traditional(漢語)",
	"Corsican(Corsu)",
	"Croatian(hrvatski)",
	"Czech(čeština)",
	"Danish(dansk)",
	"Dutch(Nederlands)",
	"English",
	"Esperanto",
	"Estonian(eesti keel)",
	"Finnish(suomen kieli)",
	"French(français)",
	"Frisian(Frysk)",
	"Galician(galego)",
	"Georgian(ქართული)",
	"German(Deutsch)",
	"Greek(Ελληνικά)",
	"Gujarati(ગુજરાતી)",
	"Haitian Creole(Kreyòl Ayisyen)",
	"Hausa",
	"Hawaiian",
	"Hebrew(עברית)",
	"Hindi(हिन्दी)",
	"Hmong",
	"Hungarian(Magyar)",
	"Icelandic(Íslenska)",
	"Igbo(Asụsụ Igbo)",
	"Indonesian(bahasa Indonesia)",
	"Irish(Gaeilge)",
	"Italian(italiano)",
	"Japanese(日本語)",
	"Javanese(Basa Jawa)",
	"Kannada(ಕನ್ನಡ)",
	"Kazakh(қазақ тілі)",
	"Khmer(ខ្មែរ)",
	"Korean(한국어)",
	"Kurdish(Kurdî)",
	"Kyrgyz(Кыргызча)",
	"Lao(ລາວ)",
	"Latin(lingua latīna)",
	"Latvian(latviešu valoda)",
	"Lithuanian(lietuvių kalba)",
	"Luxembourgish(Lëtzebuergesch)",
	"Macedonian(македонски)",
	"Malagasy",
	"Malay(Bahasa melayu)",
	"Malayalam(മലയാളം)",
	"Maltese(Maltese)",
	"Maori(Māori)",
	"Marathi(मराठी)",
	"Mongolian(монгол хэл)",
	"Myanmar (Burmese)(မြန်မာဘာသာ)",
	"Nepali(नेपाली)",
	"Norwegian(norsk)",
	"Nyanja (Chichewa)",
	"Pashto(پښتو)",
	"Persian(فارسی)",
	"Polish(Polski)",
	"Portuguese (Portugal, Brazil)",
	"Punjabi(ਪੰਜਾਬੀ)",
	"Romanian(limba română)",
	"Russian(русский язык)",
	"Samoan(Gagana Samoa)",
	"Scots Gaelic(Gàidhlig)",
	"Serbian(Српски)", 
	"Sesotho", 
	"Shona", 
	"Sindhi(سنڌي)", 
	"Sinhala(සිංහල)", 
	"Slovak(Slovenský jazyk)", 
	"Slovenian(Slovenski jezik)", 
	"Somali(Soomaaliga)", 
	"Spanish(Español)", 
	"Sundanese(Sunda)", 
	"Swahili(Kiswahili)", 
	"Swedish(svenska)", 
	"Tagalog (Filipino)", 
	"Tajik(Забони тоҷикӣ)", 
	"Tamil(தமிழ்)", 
	"Telugu(తెలుగు)",
	"Thai(ภาษาไทย)",
	"Turkish(Türk Dili)",
	"Ukrainian(Українська)",
	"Urdu(اردو)",
	"Uzbek(o‘zbek tili)",
	"Vietnamese(Tiếng Việt)",
	"Welsh(Cymraeg)",
	"Xhosa(isiXhosa)",
	"Yiddish(יידיש)",
	"Yoruba(Èdè Yorùbá)",
	"Zulu(isiZulu)",
};

std::string gtr_msg_en[10] =
{
	"Select sorce language",
	"Select target language",
	"Select(A) Move(DPAD ↑,↓,→,←) Close(Y)",
	"Translating...",
	"Translate(A)",
	"Copy(X)",
	"Up(DPAD ↑)",
	"Down(DPAD ↓)",
	"Change sorce language(L)",
	"Change target language(R)",
};

std::string gtr_msg_jp[10] =
{
	"元の言語を選択してください",
	"目的の言語を選択してください",
	"選択(A) 移動(十字 ↑,↓,→,←) 閉じる(Y)", 
	"翻訳中...",
	"翻訳(A)",
	"コピー(X)",
	"上(DPAD ↑)",
	"下(DPAD ↓)",
	"元の言語変更(L)",
	"目的の言語変更(R)",
};

std::string gtr_msg[10];


Thread gtr_tr_thread;

bool Gtr_query_init_flag(void)
{
	return gtr_already_init;
}

double Gtr_query_offset(int item_num)
{
	if (item_num == GTR_SORCE_LANG_OFFSET)
		return gtr_sorce_lang_offset;
	else if (item_num == GTR_TARGET_LANG_OFFSET)
		return gtr_target_lang_offset;
	else
		return -1;
}

bool Gtr_query_operation_flag(int operation_num)
{
	if (operation_num == GTR_TYPE_TEXT_REQUEST)
		return gtr_type_text_request;
	else if (operation_num == GTR_SELECT_SORCE_LANG_REQUEST)
		return gtr_select_sorce_lang_request;
	else if (operation_num == GTR_SELECT_TARGET_LANG_REQUEST)
		return gtr_select_target_lang_request;
	else
		return false;
}

bool Gtr_query_running_flag(void)
{
	return gtr_main_run;
}

int Gtr_query_selected_num(int item_num)
{
	if (item_num == GTR_SELECTED_HISTORY_NUM)
		return gtr_selected_history_num;
	else
		return -1;
}

double Gtr_query_selected_num_d(int item_num)
{
	if (item_num == GTR_SELECTED_SORCE_LANG_NUM_D)
		return gtr_selected_sorce_lang_num;
	else if (item_num == GTR_SELECTED_TARGET_LANG_NUM_D)
		return gtr_selected_target_lang_num;
	else
		return -1;
}

double Gtr_query_text_pos_x(void)
{
	return gtr_text_pos_x;
}

std::string Gtr_query_tr_history(int num)
{
	if (num >= 0 && num <= 9)
		return gtr_history[num];
	else
		return "";
}

void Gtr_set_offset(int item_num, double value)
{
	if (item_num == GTR_SORCE_LANG_OFFSET)
		gtr_sorce_lang_offset = value;
	else if (item_num == GTR_TARGET_LANG_OFFSET)
		gtr_target_lang_offset = value;
}

void Gtr_set_operation_flag(int operation_num, bool flag)
{
	if (operation_num == GTR_TYPE_TEXT_REQUEST)
		gtr_type_text_request = flag;
	else if (operation_num == GTR_SELECT_SORCE_LANG_REQUEST)
		gtr_select_sorce_lang_request = flag;
	else if (operation_num == GTR_SELECT_TARGET_LANG_REQUEST)
		gtr_select_target_lang_request = flag;
}

void Gtr_set_sorce_lang(int lang_num)
{
	if (lang_num >= 0 && lang_num <= 103)
		gtr_sorce_lang = gtr_lang_short_list[lang_num];
}

void Gtr_set_target_lang(int lang_num)
{
	if(lang_num >= 0 && lang_num <= 103)
		gtr_target_lang = gtr_lang_short_list[lang_num];
}

void Gtr_set_selected_num(int item_num, int value)
{
	if (item_num == GTR_SELECTED_HISTORY_NUM)
		gtr_selected_history_num = value;
}

void Gtr_set_selected_num_d(int item_num, double value)
{
	if (item_num == GTR_SELECTED_SORCE_LANG_NUM_D)
		gtr_selected_sorce_lang_num = value;
	else if (item_num == GTR_SELECTED_TARGET_LANG_NUM_D)
		gtr_selected_target_lang_num = value;
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
	int num_of_lang = 105;

	for (int i = 0; i < num_of_lang; i++)
	{
		if (gtr_lang_short_list[i] == short_name)
			return gtr_lang_list[i];
	}
	return "Unknown";
}

void Gtr_tr_thread(void* arg)
{
	Log_log_save("Gtr/Tr thread", "Thread started.", 1234567890, false);

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
			if (gtr_current_history_num + 1 > 9)
				gtr_current_history_num = 0;
			else
				gtr_current_history_num++;
			gtr_history[gtr_current_history_num] = gtr_msg[3];

			send_data = "{ \"text\": \"" + gtr_input_text + "\",\"sorce\" : \"" + gtr_sorce_lang + "\",\"target\" : \"" + gtr_target_lang + "\" }";
			
			memset(httpc_buffer, 0x0, 0x10000);
			result = Httpc_post_and_dl_data(url, (char*)send_data.c_str(), send_data.length(), httpc_buffer, 0x10000, &dl_size, &status_code, true);

			if (result.code == 0)
				gtr_history[gtr_current_history_num] = (char*)httpc_buffer;
			else
			{
				gtr_history[gtr_current_history_num] = "***Translation failed.***";
				Err_set_error_message(result.string, result.error_description, "Gtr/Tr thread/httpc", result.code);
				Err_set_error_show_flag(true);
			}

			gtr_tr_request = false;
		}
		usleep(100000);
	}
	Log_log_save("Gtr/Tr thread", "Thread exit.", 1234567890, false);
}

void Gtr_init(void)
{
	Log_log_save("Gtr/init", "Initializing...", 1234567890, s_debug_slow);

	gtr_current_history_num = 9;
	gtr_text_pos_x = 0.0;

	Draw_progress("0/0 [Gtr] Starting threads...");
	gtr_tr_thread_run = true;
	gtr_tr_thread = threadCreate(Gtr_tr_thread, (void*)(""), STACKSIZE, 0x26, -1, true);

	Gtr_resume();
	gtr_already_init = true;
	Log_log_save("Gtr/init", "Initialized", 1234567890, s_debug_slow);
}

void Gtr_exit(void)
{
	Log_log_save("Gtr/Exit", "Exiting...", 1234567890, s_debug_slow);
	u64 time_out = 10000000000;
	int log_num;
	bool failed = false;
	Result_with_string result;


	Draw_progress("0/0 [Gtr] Exiting threads...");
	gtr_already_init = false;
	gtr_tr_thread_run = false;
	gtr_thread_suspend = false;

	log_num = Log_log_save("Gtr/Exit", "Exiting thread...", 1234567890, s_debug_slow);
	result.code = threadJoin(gtr_tr_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, "[Success] ", result.code, s_debug_slow);
	else
	{
		failed = true;
		Log_log_add(log_num, "[Error] ", result.code, s_debug_slow);
	}

	Draw_progress("1/1 [Gtr] Cleaning up...");
	gtr_input_text = "n/a";
	gtr_input_text.reserve(10);

	for (int i = 0; i < 10; i++)
	{
		gtr_history[i] = "n/a";
		gtr_history[i].reserve(10);
	}

	if (failed)
		Log_log_save("Gtr/Exit", "[Warn] Some function returned error.", 1234567890, s_debug_slow);

	Log_log_save("Gtr/Exit", "Exited.", 1234567890, s_debug_slow);
}

void Gtr_main(void)
{
	int log_y = Log_query_y();
	int font_num = 0;
	double log_x = Log_query_x();
	float red;
	float green;
	float blue;
	float alpha;
	osTickCounterUpdate(&s_tcount_frame_time);

	if (s_setting[1] == "en")
	{
		gtr_msg[0] = gtr_msg_en[0];
		gtr_msg[1] = gtr_msg_en[1];
		gtr_msg[2] = gtr_msg_en[2];
		gtr_msg[3] = gtr_msg_en[3];
		gtr_msg[4] = gtr_msg_en[4];
		gtr_msg[5] = gtr_msg_en[5];
		gtr_msg[6] = gtr_msg_en[6];
		gtr_msg[7] = gtr_msg_en[7];
		gtr_msg[8] = gtr_msg_en[8];
		gtr_msg[9] = gtr_msg_en[9];
	}
	else if (s_setting[1] == "jp")
	{
		gtr_msg[0] = gtr_msg_jp[0];
		gtr_msg[1] = gtr_msg_jp[1];
		gtr_msg[2] = gtr_msg_jp[2];
		gtr_msg[3] = gtr_msg_jp[3];
		gtr_msg[4] = gtr_msg_jp[4];
		gtr_msg[5] = gtr_msg_jp[5];
		gtr_msg[6] = gtr_msg_jp[6];
		gtr_msg[7] = gtr_msg_jp[7];
		gtr_msg[8] = gtr_msg_jp[8];
		gtr_msg[9] = gtr_msg_jp[9];
	}

	if (Sem_query_font_flag(SEM_USE_DEFAULT_FONT))
		font_num = 0;
	else if (Sem_query_font_flag(SEM_USE_SYSTEM_SPEIFIC_FONT))
		font_num = Sem_query_selected_num(SEM_SELECTED_LANG_NUM) + 1;
	else if (Sem_query_font_flag(SEM_USE_EXTERNAL_FONT))
		font_num = 5;

	Draw_set_draw_mode(s_draw_vsync_mode);
	if (s_night_mode)
	{
		red = 1.0;
		green = 1.0;
		blue = 1.0;
		alpha = 0.75;
		Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
	}
	else
	{
		red = 0.0;
		green = 0.0;
		blue = 0.0;
		alpha = 1.0;
		Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);
	}

	Draw_texture(Square_image, black_tint, 0, 0.0, 0.0, 400.0, 15.0);
	Draw_texture(Wifi_icon_image, dammy_tint, s_wifi_signal, 360.0, 0.0, 15.0, 15.0);
	Draw_texture(Battery_level_icon_image, dammy_tint, s_battery_level / 5, 330.0, 0.0, 30.0, 15.0);
	if (s_battery_charge)
		Draw_texture(Battery_charge_icon_image, dammy_tint, 0, 310.0, 0.0, 20.0, 15.0);
	Draw(s_status, 0, 0.0f, 0.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 1.0f);
	Draw(s_battery_level_string, 0, 337.5f, 1.25f, 0.4f, 0.4f, 0.0f, 0.0f, 0.0f, 0.5f);
	
	Draw("Sorce : " + Gtr_get_lang_name(gtr_sorce_lang), font_num, 0.0, 20.0, 0.6, 0.6, red, green, blue, alpha);
	Draw(gtr_input_text, font_num, gtr_text_pos_x, 50.0, 0.6, 0.6, 0.25, 0.0, 0.5, 1.0);
	Draw("Target : " + Gtr_get_lang_name(gtr_target_lang), font_num, 0.0, 100.0, 0.6, 0.6, red, green, blue, alpha);
	Draw(gtr_history[gtr_current_history_num], font_num, gtr_text_pos_x, 130.0, 0.6, 0.6, 0.25, 0.0, 0.5, 1.0);

	if (s_debug_mode)
		Draw_debug_info();
	if (Log_query_log_show_flag())
	{
		for (int i = 0; i < 23; i++)
			Draw(Log_query_log(log_y + i), 0, log_x, 10.0f + (i * 10), 0.4, 0.4, 0.0, 0.5, 1.0, 1.0);
	}

	if (s_night_mode)
		Draw_screen_ready_to_draw(1, true, 2, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(1, true, 2, 1.0, 1.0, 1.0);

	Draw(s_gtr_ver, 0, 0.0, 0.0, 0.45, 0.45, 0.0, 1.0, 0.0, 1.0);
	for (int i = 0; i < 10; i++)
	{
		if(i == gtr_selected_history_num)
			Draw(gtr_history[i], font_num, gtr_text_pos_x, 10.0 + (i * 17.5), 0.6, 0.6, 0.25, 0.0, 0.5, 1.0);
		else
			Draw(gtr_history[i], font_num, gtr_text_pos_x, 10.0 + (i * 17.5), 0.6, 0.6, 0.75, 0.5, 0.0, 1.0);
	}

	Draw_texture(Square_image, weak_aqua_tint, 0, 10.0, 190.0, 75.0, 15.0);
	Draw_texture(Square_image, weak_aqua_tint, 0, 10.0, 210.0, 75.0, 15.0);
	Draw_texture(Square_image, weak_aqua_tint, 0, 100.0, 190.0, 75.0, 15.0);
	Draw_texture(Square_image, weak_aqua_tint, 0, 100.0, 210.0, 75.0, 15.0);
	Draw_texture(Square_image, weak_aqua_tint, 0, 190.0, 190.0, 120.0, 15.0);
	Draw_texture(Square_image, weak_aqua_tint, 0, 190.0, 210.0, 120.0, 15.0);
	Draw(gtr_msg[4], 0, 12.5, 190.0, 0.4, 0.4, red, green, blue, alpha);
	Draw(gtr_msg[5], 0, 12.5, 210.0, 0.4, 0.4, red, green, blue, alpha);
	Draw(gtr_msg[6], 0, 102.5, 190.0, 0.4, 0.4, red, green, blue, alpha);
	Draw(gtr_msg[7], 0, 102.5, 210.0, 0.4, 0.4, red, green, blue, alpha);
	Draw(gtr_msg[8], 0, 192.5, 190.0, 0.375, 0.375, red, green, blue, alpha);
	Draw(gtr_msg[9], 0, 192.5, 210.0, 0.375, 0.375, red, green, blue, alpha);

	if (gtr_select_sorce_lang_request)
	{
		Draw_texture(Square_image, aqua_tint, 0, 25.0, 10.0, 270.0, 205.0);
		Draw(gtr_msg[0], 0, 27.5, 10.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);
		Draw(gtr_msg[2], 0, 27.5, 200.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);

		for (int i = 0; i < 10; i++)
		{
			if(i == (int)gtr_selected_sorce_lang_num)
				Draw(gtr_lang_list[(int)gtr_sorce_lang_offset + i], font_num, 27.5, 20.0 + (i * 18.0), 0.6, 0.6, 1.0, 0.0, 0.0, 1.0);
			else
				Draw(gtr_lang_list[(int)gtr_sorce_lang_offset + i], font_num, 27.5, 20.0 + (i * 18.0), 0.6, 0.6, 0.0, 0.0, 0.0, 1.0);
		}
	}
	else if (gtr_select_target_lang_request)
	{
		Draw_texture(Square_image, aqua_tint, 0, 25.0, 10.0, 270.0, 205.0);
		Draw(gtr_msg[1], 0, 27.5, 10.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);
		Draw(gtr_msg[2], 0, 27.5, 200.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);

		for (int i = 0; i < 10; i++)
		{
			if (i == (int)gtr_selected_target_lang_num)
				Draw(gtr_lang_list[(int)gtr_target_lang_offset + i], font_num, 27.5, 20.0 + (i * 18.0), 0.6, 0.6, 1.0, 0.0, 0.0, 1.0);
			else
				Draw(gtr_lang_list[(int)gtr_target_lang_offset + i], font_num, 27.5, 20.0 + (i * 18.0), 0.6, 0.6, 0.0, 0.0, 0.0, 1.0);
		}
	}

	if (Err_query_error_show_flag())
		Draw_error();

	Draw_texture(Square_image, black_tint, 0, 0.0, 225.0, 320.0, 15.0);
	Draw(s_bot_button_string[1], 0, 30.0f, 220.0f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 1.0f);

	if (Hid_query_key_held_state(KEY_H_TOUCH))
		Draw(s_circle_string, 0, Hid_query_touch_pos(true), Hid_query_touch_pos(false), 0.20f, 0.20f, 1.0f, 0.0f, 0.0f, 1.0f);
	
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