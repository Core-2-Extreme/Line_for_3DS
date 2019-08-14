#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <time.h>
#include "citro2d.h"
#include <string>
#include "unicodetochar.h"

#define STACKSIZE (24 * 1024)

int mode_change_show_time = 90;
int number_of_message = 0;
int update_progress = 0;
int hours = -1;
int minutes = -1;
int seconds = -1;
int day = -1;
int month = -1;
int fps = 0;
int fps_show = 0;
int room_select_num = 0;
int message_select_num = 0;
int log_num = 0;
int app_log_num = 0;
int app_log_view_num = 0;
int app_log_view_num_cache= 0;
double update_time[7];
double up_time_ms;
float app_log_x = 0.0;
float text_x = 0.0;
float text_y = 0.0;
float text_interval = 35;
float text_size = 0.45;
float app_log_x_cache = 0.0;
float text_x_cache = 0.0;
float text_y_cache = 0.0;
float text_interval_cache = 35;
float text_size_cache = 0.45;
float frame_time_point[270];
u8* line_log_buffer;
u8* line_send_message_buffer;
u8 battery_charge = -1;
u8 wifi_signal = -1;
u32 kDown;
u32 kHeld;

bool update_thread_run = false;
bool log_download_thread_run = false;
bool hid_thread_run = false;
bool update_detail_show = false;
bool frame_time_show = false;
bool setting_menu_show = false;
bool app_logs_show = false;
bool auto_update_mode = false;
bool night_mode = false;
bool message_select_mode = false;
bool dont_draw_outside_mode = false;
bool message_jp_mode = false;
bool message_jump_mode = false;
bool send_debug_info = false;
bool log_update = false;
bool mcu_fail = false;

char ver[] = "v0.3.0";
char swkb_input_text[8192];
char read_data[0x800000];
char* message_log_cache[305];

std::string main_url;
std::string message_send_url;
std::string message_log[305];
std::string message_log_short[60000];
std::string id[100];
std::string clipboard[5];
std::string input_text;
std::string app_logs[512];
char battery_level_message[8] = "?";
std::string battery_charge_message[4] = { "Discharging", "Charging", "放電中", "充電中" };
std::string update_time_message[14] =
{ "初期化に", "接続に", "新規URL取得に", "接続に", "ダウンロードに", "旧データ削除に", "解析に" , "to initialize", "to connection", "to get result url" , "to connection" , "to download" , "to delete old data" , "to parse download data" };
std::string update_progress_message[21] = {" " , 
"Initializing..." , 
"Initialized. Connecting to server..." ,  
"Connected to server. Receiving result url...", 
"Received result url. Connecting to server..." , 
"Connected to server. Downloading data..." , 
"Download completed. Deleting old data..." , 
"Deleted old data. Parse download data..." , 
"Parse download data completed." ,
"Downloading data failed.",
"Receiving result url failed.",
"初期化中...",
"初期化完了。 サーバーに接続中...",
"サーバーへ接続完了。 結果URLを受信中...", 
"結果URL受信完了。 サーバーに接続中...", 
"サーバーへ接続完了。 データをダウンロード中...", 
"ダウンロード完了。 古いデータを削除中...", 
"古いデータ削除完了。 ダウンロードデータを解析中...", 
"ダウンロードデータ解析完了。", 
"データダウンロード失敗。",
"結果URL受信失敗。", };
std::string auto_update_message[4] = { "Auto update : OFF", "Auto update : ON", "自動更新 : OFF", "自動更新 : ON" };
std::string lite_mode_message[4] =   { "Litemode : OFF", "Litemode : ON", "軽量モード : OFF" ,"軽量モード : ON" };
std::string lite_mode_change_message[4] = { "Litemode has been disabled.", "Litemode has been enabled.", "軽量モードは無効になっています" ,"軽量モードは有効になっています" };
std::string night_mode_message[4] = { "Night mode : OFF", "Night mode : ON", "夜モード : OFF", "夜モード : ON" };
std::string frame_time_message[4] = { "Frame time : OFF", "Frame time : ON", "フレームタイム : OFF", "フレームタイム : ON" };
std::string debug_log_message[4] =  { "Debug log : OFF", "Debug log : ON", "デバッグログ : OFF", "デバッグログ : ON" };
std::string language_message = "日本語/English";

Thread update_thread, log_download_thread, message_send_thread, hid_thread;
httpcContext line_message_send;
C2D_Text c2d_info, c2d_bot_debug, c2d_bot_debug_3, c2d_bot_debug_4, c2d_confirm, c2d_log, c2d_bot_lite_mode, c2d_bot_touch_pos, c2d_mode_change, c2d_top_back_color, c2d_center_back_color;
SwkbdState swkbd_send_message, swkbd_add, swkbd_password;
SwkbdLearningData learn_data;
SwkbdDictWord words[8];
SwkbdStatusData swkbd_send_message_status;
SwkbdButton press_button;
touchPosition touch_pos;
Result function_result;
Handle  fs_dir_read_handle;
FS_Archive fs_write_archive, fs_read_archive, fs_dir_read_archive;
FS_DirectoryEntry fs_id_entry;
C3D_RenderTarget* Screen_top;
C3D_RenderTarget* Screen_bot;
httpcContext line_message_log;
TickCounter update_spended_time, total_frame_time, up_time;

void Draw(std::string text, float x, float y, float text_size_x, float text_size_y, float r, float g, float b, float a)
{
	C2D_TextBuf c2d_buf = C2D_TextBufNew(16384);
	C2D_TextParse(&c2d_confirm, c2d_buf, text.c_str());
	C2D_TextOptimize(&c2d_confirm);
	C2D_DrawText(&c2d_confirm, C2D_WithColor, x, y, 0.0f, text_size_x, text_size_y, C2D_Color32f(r, g, b, a));
	C2D_TextBufDelete(c2d_buf);
}

void App_log_save(std::string type, std::string text, Result result, bool draw)
{
	double time_cache;
	char app_log_cache[1024];
	osTickCounterUpdate(&up_time);
	time_cache = osTickCounterRead(&up_time);
	up_time_ms = up_time_ms + time_cache;
	time_cache = up_time_ms / 1000;

	if (result == 1234567890)
	{
		sprintf(app_log_cache, "[%.5f][%s] %s", time_cache, type.c_str(), text.c_str());
	}
	else
	{
		sprintf(app_log_cache, "[%.5f][%s] %s0x%lx", time_cache, type.c_str(), text.c_str(), result);
	}

	app_logs[app_log_num] = app_log_cache;
	app_log_num++;
	if (app_log_num >= 512)
	{
		app_log_num = 0;
	}

	if (app_log_num < 23)
	{
		app_log_view_num_cache = 0;
	}
	else
	{
		app_log_view_num_cache = app_log_num - 23;
	}

	if (draw)
	{
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

		if (night_mode)
		{
			C2D_TargetClear(Screen_top, C2D_Color32f(0, 0, 0, 0));
		}
		else
		{
			C2D_TargetClear(Screen_top, C2D_Color32f(8, 8, 8, 0));
		}

		C2D_SceneBegin(Screen_top);
		int y_i = 0;
		for (int i = app_log_view_num_cache; i < app_log_view_num_cache + 23; i++)
		{
			Draw(app_logs[i], 0.0f, 10.0f + (y_i * 10), 0.4f, 0.4f, 0.0f, 0.5f, 1.0f, 1.0f);
			y_i++;
		}
		C3D_FrameEnd(0);
	}
}

int Save_new_id(std::string id)
{
	char dir_path[25] = "/Line/to";
	FS_Path fs_path_dir = fsMakePath(PATH_ASCII, dir_path);
	FS_Path fs_path_empty = fsMakePath(PATH_EMPTY, "");
	FSUSER_OpenArchive(&fs_write_archive, ARCHIVE_SDMC, fs_path_empty);
	FSUSER_CreateDirectory(fs_write_archive, fs_path_dir, FS_ATTRIBUTE_DIRECTORY);

	strcat(dir_path, "/");
	char* file_path = strcat(dir_path, id.c_str());

	FS_Path path_file = fsMakePath(PATH_ASCII, file_path);
	function_result = FSUSER_CreateFile(fs_write_archive, path_file, FS_ATTRIBUTE_ARCHIVE, 0);

	if (function_result == 0)
	{
		return 0;
	}
	else
	{
		return function_result;
	}
}

int Read_id(void)
{
	int count = 0;
	u32 read_entry = 0;
	u32 read_entry_count = 1;
	char* cache;
	cache = (char*)malloc(0x1000);

	FS_Path fs_path_empty = fsMakePath(PATH_EMPTY, "");
	function_result = FSUSER_OpenArchive(&fs_dir_read_archive, ARCHIVE_SDMC, fs_path_empty);
	if (function_result != 0)
	{
		return function_result;
	}

	FS_Path fs_path_dir = fsMakePath(PATH_ASCII, "/Line/to");
	function_result = FSUSER_OpenDirectory(&fs_dir_read_handle, fs_dir_read_archive, fs_path_dir);
	if (function_result != 0)
	{
		return function_result;
	}

	while (true)
	{
		read_entry = 0;
		FSDIR_Read(fs_dir_read_handle, &read_entry, read_entry_count, (FS_DirectoryEntry*)& fs_id_entry);
		if (read_entry == 0)
		{
			break;
		}
		unicodeToChar(cache, fs_id_entry.name, 512);
		id[count] = cache;
		count++;
	}

	free(cache);
	return 0;
}

int Save_to_file(std::string file_name, std::string data, bool delete_old_file)
{
	Handle fs_write_handle;
	char dir_path[25] = "/Line/";
	u32 written_size = 0;

	FS_Path fs_path_dir = fsMakePath(PATH_ASCII, dir_path);
	FS_Path fs_path_empty = fsMakePath(PATH_EMPTY, "");
	FSUSER_OpenArchive(&fs_write_archive, ARCHIVE_SDMC, fs_path_empty);
	FSUSER_CreateDirectory(fs_write_archive, fs_path_dir, FS_ATTRIBUTE_DIRECTORY);

	char* file_path = strcat(dir_path, file_name.c_str());

	FS_Path path_file = fsMakePath(PATH_ASCII, file_path);
	if (delete_old_file)
	{
		FSUSER_DeleteFile(fs_write_archive, path_file);
	}

	FSUSER_CreateFile(fs_write_archive, path_file, FS_ATTRIBUTE_ARCHIVE, 0);
	FSUSER_OpenFile(&fs_write_handle, fs_write_archive, path_file, FS_OPEN_WRITE, FS_ATTRIBUTE_ARCHIVE);
	function_result = FSFILE_Write(fs_write_handle, &written_size, 0, data.c_str(), data.length(), FS_WRITE_FLUSH);

	FSFILE_Close(fs_write_handle);

	if (function_result == 0)
	{
		return 0;
	}
	else
	{
		return function_result;
	}
}

std::string Load_from_file(std::string file_name)
{
	Handle fs_read_handle;
	std::string read_data_return;
	char cache[1024];
	char dir_path[25] = "/Line/";
	u32 read_size = 0;
	
	memset(read_data, 0x0, strlen(read_data));
	
	FS_Path fs_path_empty = fsMakePath(PATH_EMPTY, "");
	FSUSER_OpenArchive(&fs_read_archive, ARCHIVE_SDMC, fs_path_empty);

	char* file_path = strcat(dir_path, file_name.c_str());
	FS_Path fs_path_dir = fsMakePath(PATH_ASCII, file_path);

	FSUSER_OpenFile(&fs_read_handle, fs_read_archive, fs_path_dir, FS_OPEN_WRITE, FS_ATTRIBUTE_ARCHIVE);
	function_result = FSFILE_Read(fs_read_handle, &read_size, 0, read_data, 0x800000);
	FSFILE_Close(fs_read_handle);

	sprintf(cache, "Load_from_file(%s)=", file_name.c_str());
	App_log_save("I/O", cache, function_result, true);

	if (function_result == 0)
	{
		return read_data;
	}
	else
	{
		return "file read error";
	}
}

void Get_system_info(void)
{
	u8 battery_level;
	PTMU_GetBatteryChargeState(&battery_charge);//battery charge
	if (!mcu_fail)
	{
		MCUHWC_GetBatteryLevel(&battery_level);//battery level(%)
		sprintf(battery_level_message, "%u", battery_level);
	}
	else
	{
		sprintf(battery_level_message, "?");
	}
	wifi_signal = osGetWifiStrength();

	//Get time
	time_t unixTime = time(NULL);
	struct tm* timeStruct = gmtime((const time_t*)& unixTime);
	month = timeStruct->tm_mon;
	month = month + 1;
	day = timeStruct->tm_mday;
	hours = timeStruct->tm_hour;
	minutes = timeStruct->tm_min;
	seconds = timeStruct->tm_sec;
}

void Update_thread(void* arg)
{
	App_log_save("Thread", "Update thread start", 1234567890, true);
	while (update_thread_run)
	{
		acWaitInternetConnection();
		usleep(1000000);
		fps_show = fps;
		fps = 0;
		if (auto_update_mode)
		{
			log_update = true;
		}
	}
}

bool Exit_check(void)
{
	while (true)
	{
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		if (night_mode)
		{
			C2D_TargetClear(Screen_top, C2D_Color32f(0, 0, 0, 0));
		}
		else
		{
			C2D_TargetClear(Screen_top, C2D_Color32f(8, 8, 8, 0));
		}
		C2D_SceneBegin(Screen_top);

		if (message_jp_mode)
		{
			Draw("ソフトを終了しますか?", 90.0, 105.0f, 0.8, 0.8, 0.0, 0.0, 0.0, 1.0);
			Draw("A で終了", 130.0, 140.0f, 0.5, 0.5, 0.0, 1.0, 0.0, 1.0);
			Draw("B で戻る", 210.0, 140.0f, 0.5, 0.5, 1.0, 0.0, 0.0, 1.0);
		}
		else
		{
			Draw("Do you want to exit this software?", 90.0, 105.0f, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
			Draw("A to close", 130.0, 140.0f, 0.5, 0.5, 0.0, 1.0, 0.0, 1.0);
			Draw("B to back", 210.0, 140.0f, 0.5, 0.5, 1.0, 0.0, 0.0, 1.0);
		}

		C3D_FrameEnd(0);
		if (kDown & KEY_A)
		{
			return true;
		}
		else if (kDown& KEY_B)
		{
			return false;
		}
	}
}

void Get_log(void* arg)
{
	App_log_save("Thread", "Get log thread start", 1234567890, true);
	osTickCounterStart(&update_spended_time);
	while (log_download_thread_run)
	{
		if (log_update)
		{
			log_update = true;

			osTickCounterUpdate(&update_spended_time);
			for (int i = 0; i < 7; i++)
			{
				update_time[i] = 0;
			}
			number_of_message = 0;
			log_num = 0;
			int text_length = 0;
			u32 read_size = 0;
			char* moved_url;
			std::string file_name;
			std::string url;
			moved_url = (char*)malloc(0x1000);
			line_log_buffer = (u8*)malloc(0x750000);
			
			memset(line_log_buffer , 0x0, 0x750000);
			
			if (message_jp_mode)
			{
				update_progress = 11;
			}
			else
			{
				update_progress = 1;
			}

			update_progress++;
			osTickCounterUpdate(&update_spended_time);
			update_time[0] = osTickCounterRead(&update_spended_time);
			url = main_url;
			url += id[room_select_num];
			httpcOpenContext(&line_message_log, HTTPC_METHOD_GET, url.c_str(), 0);
			httpcSetSSLOpt(&line_message_log, SSLCOPT_DisableVerify);
			httpcSetKeepAlive(&line_message_send, HTTPC_KEEPALIVE_ENABLED);
			httpcAddRequestHeaderField(&line_message_send, "Connection", "Keep-Alive");
			httpcAddRequestHeaderField(&line_message_log, "User-Agent", "Line for 3DS v0.3.0");
			function_result = httpcBeginRequest(&line_message_log);
			App_log_save("Network", "httpcBeginRequest=", function_result, false);

			update_progress++;
			osTickCounterUpdate(&update_spended_time);
			update_time[1] = osTickCounterRead(&update_spended_time);
			function_result = httpcGetResponseHeader(&line_message_log, "Location", moved_url, 0x1000);
			App_log_save("Network", "httpcGetResponseHeader(Location)=", function_result, false);
			httpcCloseContext(&line_message_log);
			if (function_result != 0)
			{
				if (message_jp_mode)
				{
					update_progress = 20;
				}
				else
				{
					update_progress = 10;
				}
			}
			else
			{
				update_progress++;
				osTickCounterUpdate(&update_spended_time);
				update_time[2] = osTickCounterRead(&update_spended_time);
				httpcOpenContext(&line_message_log, HTTPC_METHOD_GET, moved_url, 0);
				httpcSetSSLOpt(&line_message_log, SSLCOPT_DisableVerify);
				httpcSetKeepAlive(&line_message_send, HTTPC_KEEPALIVE_ENABLED);
				httpcAddRequestHeaderField(&line_message_send, "Connection", "Keep-Alive");
				httpcAddRequestHeaderField(&line_message_log, "User-Agent", "Line for 3DS v0.3.0");
				function_result = httpcBeginRequest(&line_message_log);
				App_log_save("Network", "httpcBeginRequest=", function_result, false);

				update_progress++;
				osTickCounterUpdate(&update_spended_time);
				update_time[3] = osTickCounterRead(&update_spended_time);
				function_result = httpcDownloadData(&line_message_log, line_log_buffer, 0x750000, &read_size);
				App_log_save("Network", "httpcDownloadData=", function_result, false);
				httpcCloseContext(&line_message_log);
				if (function_result != 0)
				{
					if (message_jp_mode)
					{
						update_progress = 18;
					}
					else
					{
						update_progress = 9;
					}
				}
				else
				{
					update_progress++;
					osTickCounterUpdate(&update_spended_time);
					update_time[4] = osTickCounterRead(&update_spended_time);
					for (int i = 0; i <= 299; i++)
					{
						message_log[i] = "";
					}
					for (int i = 0; i <= 59999; i++)
					{
						message_log_short[i] = "";
					}
					update_progress++;
					osTickCounterUpdate(&update_spended_time);
					update_time[5] = osTickCounterRead(&update_spended_time);

					file_name = id[room_select_num].substr(0, 16);
					function_result = Save_to_file(file_name, (char*)line_log_buffer, true);
					App_log_save("I/O", "Save_to_file()", function_result, false);

					message_log_cache[0] = strtok((char*)line_log_buffer, "");
					message_log[0] = message_log_cache[0];
					number_of_message++;

					for (int i = 1; i <= 299; i++)
					{
						message_log_cache[i] = strtok(NULL, "");
						if (message_log_cache[i] == NULL)
						{
							message_log[i] = "";
							break;
						}
						else
						{
							message_log[i] = message_log_cache[i];
							number_of_message++;
						}
					}

					if (dont_draw_outside_mode)
					{
						for (int i = 0; i < number_of_message; i++)
						{
							text_length = message_log[i].length();
							if (text_length > 160)
							{
								log_num++;
								message_log_short[log_num] = message_log[i].substr(0, 84);
								for (int k = 1; k <= 200; k++)
								{
									if (text_length <= 80 * k)
									{
										break;
									}
									log_num++;
									message_log_short[log_num] = message_log[i].substr(80 * k, 84);
								}
							}
							else
							{
								log_num++;
								if (text_length <= 80)
								{
									message_log_short[log_num] = message_log[i];
								}
								else if (text_length >= 85 && text_length <= 160)
								{
									message_log_short[log_num] = message_log[i].substr(0, 84);
									log_num++;
									message_log_short[log_num] = message_log[i].substr(84, text_length - 80);
								}
							}
						}

						for (int i = log_num + 1; i < 59999; i++)
						{
							message_log_short[i] = "";
						}
						update_progress++;
						if (message_jump_mode)
						{
							text_y = (log_num - 5) * -text_interval;
							message_jump_mode = false;
						}
					}
					else
					{
						update_progress++;
						if (message_jump_mode)
						{
							text_y = (number_of_message - 5) * -text_interval;
							message_jump_mode = false;
						}
					}
					osTickCounterUpdate(&update_spended_time);
					update_time[6] = osTickCounterRead(&update_spended_time);
				}
			}


			free(moved_url);
			free(line_log_buffer);
			log_update = false;
		}
		usleep(100000);
	}
}

void Send_message(void* arg)
{
	u32 read_size = 0;
	std::string send_data = "{ \"to\": \"";
	send_data += id[room_select_num];
	send_data += "\",\"message\" : \"";
	send_data += input_text;
	send_data += "\" }";

	httpcOpenContext(&line_message_send, HTTPC_METHOD_POST, message_send_url.c_str(), 0);
	httpcSetSSLOpt(&line_message_send, SSLCOPT_DisableVerify);
	httpcSetKeepAlive(&line_message_send, HTTPC_KEEPALIVE_ENABLED);
	httpcAddRequestHeaderField(&line_message_send, "Connection", "Keep-Alive");
	httpcAddRequestHeaderField(&line_message_send, "Content-Type", "application/json");
	httpcAddRequestHeaderField(&line_message_send, "User-Agent", "Line for 3DS v0.3.0");
	httpcAddPostDataRaw(&line_message_send, (u32*)send_data.c_str(), strlen(send_data.c_str()));
	function_result = httpcBeginRequest(&line_message_send);
	App_log_save("Network", "httpcBeginRequest", function_result, false);
	function_result = httpcDownloadData(&line_message_send, line_send_message_buffer, 0x5000, &read_size);
	App_log_save("Network", "httpcDownloadData", function_result, false);
	httpcCloseContext(&line_message_send);
}

void Scan_hid(void* arg)
{
	App_log_save("Thread", "Hid thread start", 1234567890, true);
	int press_time = 0;

	while (hid_thread_run)
	{
		hidScanInput();
		hidTouchRead(&touch_pos);
		kHeld = hidKeysHeld();
		kDown = hidKeysDown();

		usleep(16700);
		press_time++;
		if (kHeld & KEY_CPAD_UP)
		{
			if (app_logs_show)
			{
				app_log_view_num_cache--;
				if (app_log_view_num_cache < 0)
				{
					app_log_view_num_cache = 0;
				}
			}
			else
			{
				if (press_time > 600)
				{
					text_y_cache += 50.0f;
				}
				else if (press_time > 240)
				{
					text_y_cache += 10.0f;
				}
				else
				{
					text_y_cache += 5.0f;
				}
			}
		}
		else if (kHeld & KEY_CPAD_DOWN)
		{
			if (app_logs_show)
			{
				app_log_view_num_cache++;
				if (app_log_view_num_cache > 512)
				{
					app_log_view_num_cache = 512;
				}
			}
			else
			{
				if (press_time > 600)
				{
					text_y_cache -= 50.0f;
				}
				else if (press_time > 240)
				{
					text_y_cache -= 10.0f;
				}
				else
				{
					text_y_cache -= 5.0f;
				}
			}
		}
		else if (kHeld & KEY_CPAD_LEFT)
		{
			if (app_logs_show)
			{
				app_log_x_cache += 5.0f;
				if (app_log_x_cache > 0.0)
				{
					app_log_x_cache = 0.0f;
				}
			}
			else
			{
				if (press_time > 240)
				{
					text_x_cache += 20.0f;
				}
				else
				{
					text_x_cache += 5.0f;
				}

				if (text_x_cache > 0.0)
				{
					text_x_cache = 0.0f;
				}
			}
		}
		else if (kHeld & KEY_CPAD_RIGHT)
		{
			if (app_logs_show)
			{
				app_log_x_cache -= 5.0f;				
			}
			else
			{
				if (press_time > 240)
				{
					text_x_cache -= 20.0f;
				}
				else
				{
					text_x_cache -= 5.0f;
				}
			}
		}
		else if (kHeld & KEY_R)
		{
			text_size_cache += 0.005f;
			if (text_size_cache > 2.5)
			{
				text_size_cache = 2.5f;
			}
		}
		else if (kHeld & KEY_L)
		{
			text_size_cache -= 0.005f;
			if (text_size_cache < 0.25)
			{
				text_size_cache = 0.25f;
			}
		}
		else if (kHeld & KEY_DDOWN)
		{
			if (message_select_mode)
			{
				message_select_num--;
				if (message_select_num < 0)
				{
					message_select_num = 0;
				}
			}
			else
			{
				text_interval_cache -= 0.5;
				if (text_interval_cache < 10)
				{
					text_interval_cache = 10;
				}
				if (dont_draw_outside_mode)
				{
					text_y_cache = (log_num - 2) * -text_interval_cache;
				}
				else
				{
					text_y_cache = (number_of_message - 2) * -text_interval_cache;
				}
			}
		}
		else if (kHeld & KEY_DUP)
		{
			if (message_select_mode)
			{
				message_select_num++;
				if (message_select_num > 299)
				{
					message_select_num = 299;
				}
			}
			else
			{
				text_interval_cache += 0.5;
				if (text_interval_cache > 150)
				{
					text_interval_cache = 150;
				}
				if (dont_draw_outside_mode)
				{
					text_y_cache = (log_num - 2) * -text_interval_cache;
				}
				else
				{
					text_y_cache = (number_of_message - 2) * -text_interval_cache;
				}
			}
		}
		else
		{
			press_time = 0;
		}
	}
}

int Load_from_log_sd(std::string file_name)
{
	std::string log_data_cache = "";
	char* log_data;
	int text_length = 0;
	log_data = (char*)malloc(0x750000);
	log_data_cache = Load_from_file(file_name);
	
	for (int i = 0; i <= 299; i++)
	{
		message_log[i] = "";
	}
	for (int i = 0; i <= 59999; i++)
	{
		message_log_short[i] = "";
	}

	number_of_message = 0;
	if ("file read error" == log_data_cache)
	{	
		free(log_data);
		return -1;
	}
	
	memset(log_data, 0x0, strlen(log_data));
	strcpy(log_data,log_data_cache.c_str());

	message_log_cache[0] = strtok(log_data, "");
	message_log[0] = message_log_cache[0];
	number_of_message++;
	for (int i = 1; i <= 299; i++)
	{
		message_log_cache[i] = strtok(NULL, "");
		if (message_log_cache[i] == NULL)
		{
			break;
		}
		else
		{
			message_log[i] = message_log_cache[i];
			number_of_message++;
		}
	}

	if (dont_draw_outside_mode)
	{
		for (int i = 0; i < number_of_message; i++)
		{
			text_length = message_log[i].length();
			if (text_length > 160)
			{
				log_num++;
				message_log_short[log_num] = message_log[i].substr(0, 84);
				for (int k = 1; k <= 200; k++)
				{
					if (text_length <= 80 * k)
					{
						break;
					}
					log_num++;
					message_log_short[log_num] = message_log[i].substr(80 * k, 84);
				}
			}
			else
			{
				log_num++;
				if (text_length <= 80)
				{
					message_log_short[log_num] = message_log[i];
				}
				else if (text_length >= 85 && text_length <= 160)
				{
					message_log_short[log_num] = message_log[i].substr(0, 84);
					log_num++;
					message_log_short[log_num] = message_log[i].substr(84, text_length - 80);
				}
			}
		}

		for (int i = log_num + 1; i < 59999; i++)
		{
			message_log_short[i] = "";
		}
	}

	free(log_data);
	return 0;
}

int main()
{
	osTickCounterStart(&up_time);
	App_log_save("Ver", ver, 1234567890, false);
	osSetSpeedupEnable(true);
	gfxInitDefault();
	
	std::string auth_code;
	bool message_send_confirm_succes = true;
	bool auth_success = false;
	int try_left = 3;
	int read_size = 0;
	char info[100];
	char update_time_string[512];
	char message_send_confirm[16384] = "";
	char debug_log[100];
	char debug_log_2[100];
	char message_select_num_string[32];
	line_send_message_buffer = (u8*)malloc(0x5000);
	line_log_buffer = (u8*)malloc(0x750000);
	for (int i = 0; i <= 304; i++)
	{
		message_log_cache[i] = (char*)malloc(0x2000);
		message_log[i] = (char*)malloc(0x2000);
	}
	for (int i = 0; i <= 59999; i++)
	{
		message_log_short[i] = (char*)malloc(0x50);
	}

	function_result = C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	App_log_save("Init", "C3D_Init=", function_result, false);

	function_result = C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	App_log_save("Init", "C2D_Init=", function_result, false);
	C2D_Prepare();

	Screen_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	Screen_bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	C2D_TargetClear(Screen_top, C2D_Color32f(1, 1, 1, 0));
	function_result = acInit();
	App_log_save("Init", "acInit=", function_result, true);
	function_result = aptInit();
	App_log_save("Init", "aptInit=", function_result, true);
	function_result = mcuHwcInit();
	App_log_save("Init", "mcuInit=", function_result, true);
	if (!function_result == 0)
	{
		mcu_fail = true;
	}
	function_result = ptmuInit();
	App_log_save("Init", "ptmuInit=", function_result, true);
	function_result = fsInit();
	App_log_save("Init", "fsInit=", function_result, true);
	function_result = httpcInit(0x500000);
	App_log_save("Init", "httpcInit=", function_result, true);

	function_result = APT_SetAppCpuTimeLimit(30);
	App_log_save("Apt", "Set_cpu_limit=", function_result, true);
	aptSetSleepAllowed(true);
	osTickCounterStart(&total_frame_time);

	update_thread_run = true;
	update_thread = threadCreate(Update_thread, (void*)(""), STACKSIZE, 0x24, -1, false);
	log_download_thread_run = true;
	log_download_thread = threadCreate(Get_log, (void*)(""), STACKSIZE, 0x24, -1, false);
	hid_thread_run = true;
	hid_thread = threadCreate(Scan_hid, (void*)(""), STACKSIZE, 0x24, -1, false);

	auth_code = Load_from_file("auth");
	if (auth_code == "file read error")
	{
		std::string input_cache;
		while (true)
		{
			swkbdInit(&swkbd_password, SWKBD_TYPE_QWERTY, 1, 256);
			swkbdSetHintText(&swkbd_password, "パスワードを入力 / Type password here.");
			swkbdSetValidation(&swkbd_password, SWKBD_ANYTHING, 0, 0);
			swkbdSetPasswordMode(&swkbd_password, SWKBD_PASSWORD_HIDE);

			swkbdInputText(&swkbd_password, swkb_input_text, 256);
			input_cache = swkb_input_text;

			swkbdInit(&swkbd_password, SWKBD_TYPE_QWERTY, 1, 256);
			swkbdSetHintText(&swkbd_password, "パスワードを入力(再度) / Type password here.(again)");
			swkbdSetValidation(&swkbd_password, SWKBD_ANYTHING, 0, 0);
			swkbdSetPasswordMode(&swkbd_password, SWKBD_PASSWORD_HIDE);

			swkbdInputText(&swkbd_password, swkb_input_text, 256);

			if (input_cache == swkb_input_text)
			{
				function_result = Save_to_file("auth", swkb_input_text, true);
				App_log_save("I/O", "Save_to_file(auth)=", function_result, true);
				break;
			}
		}
	}

	App_log_save("Auth", "Please enter password", 1234567890, true);
	usleep(150000);
	if (auth_code == "")
	{
		App_log_save("Auth", "Password is not set", 1234567890, true);
		auth_success = true;
	}
	else
	{
		for (int i = 0; i < try_left; i++)
		{
			swkbdInit(&swkbd_password, SWKBD_TYPE_QWERTY, 1, 256);
			swkbdSetHintText(&swkbd_password, "パスワードを入力 / Type password here.");
			swkbdSetValidation(&swkbd_password, SWKBD_ANYTHING, 0, 0);
			swkbdSetPasswordMode(&swkbd_password, SWKBD_PASSWORD_HIDE);

			swkbdInputText(&swkbd_password, swkb_input_text, 256);
			if (auth_code == swkb_input_text)
			{
				auth_success = true;
				App_log_save("Auth", "Password is correct", 1234567890, true);
				break;
			}
			else
			{
				App_log_save("Auth", "Password is incorrect", 1234567890, true);
			}
		}
	}

	if (!auth_success)
	{
		App_log_save("Auth", "Failed to auth, rebooting...", 1234567890, true);
		usleep(500000);
		APT_HardwareResetAsync();
	}
	message_send_url = Load_from_file("post_url.txt");
	main_url = Load_from_file("main_url.txt");
	function_result = Read_id();
	App_log_save("I/O", "Read_id=", function_result, true);

	usleep(250000);
	// Main loop
	while (aptMainLoop())
	{
		//Your code goes here
		osTickCounterUpdate(&total_frame_time);
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

		Get_system_info();

		text_size = text_size_cache;
		text_interval = text_interval_cache;
		app_log_x = app_log_x_cache;
		app_log_view_num = app_log_view_num_cache;
		text_x = text_x_cache;
		text_y = text_y_cache;

		if (night_mode)
		{
			C2D_TargetClear(Screen_top, C2D_Color32f(0, 0, 0, 0));
		}
		else
		{
			C2D_TargetClear(Screen_top, C2D_Color32f(8, 8, 8, 0));
		}
		C2D_SceneBegin(Screen_top);

		for (int i = 0; i <= 59999; i++)
		{
			if (text_y + text_interval * i <= 10)
			{
			}
			else if (text_y + text_interval * i >= 220)
			{
			}
			else
			{
				if (dont_draw_outside_mode && night_mode)
				{
					Draw(message_log_short[i], text_x, text_y + text_interval * i, text_size, text_size, 0.9f, 0.9f, 0.9f, 1.0f);
				}
				else if (dont_draw_outside_mode)
				{
					Draw(message_log_short[i], text_x, text_y + text_interval * i, text_size, text_size, 0.0f, 0.0f, 0.0f, 1.0f);
				}
				else if (night_mode)
				{
					if (i > 299)
					{
						break;
					}
					Draw(message_log[i], text_x, text_y + text_interval * i, text_size, text_size, 0.9f, 0.9f, 0.9f, 1.0f);
				}
				else
				{
					if (i > 299)
					{
						break;
					}
					Draw(message_log[i], text_x, text_y + text_interval * i, text_size, text_size, 0.0f, 0.0f, 0.0f, 1.0f);
				}
			}
		}


		if (mode_change_show_time > 0)
		{
			Draw("■", 85.0f, 115.0f, 10.0f, 0.55f, 0.0f, 0.0f, 0.0f, 0.7f);
			if (message_jp_mode && dont_draw_outside_mode)
			{
				Draw(lite_mode_change_message[dont_draw_outside_mode + 2], 105.0f, 115.0f, 0.55f, 0.55f, 0.0f, 1.0f, 1.0f, 1.0f);
			}
			else if (message_jp_mode)
			{
				Draw(lite_mode_change_message[dont_draw_outside_mode + 2], 105.0f, 115.0f, 0.55f, 0.55f, 1.0f, 0.5f, 0.0f, 1.0f);
			}
			else if (dont_draw_outside_mode)
			{
				Draw(lite_mode_change_message[dont_draw_outside_mode], 105.0f, 115.0f, 0.55f, 0.55f, 0.0f, 1.0f, 1.0f, 1.0f);
			}
			else
			{
				Draw(lite_mode_change_message[dont_draw_outside_mode], 105.0f, 115.0f, 0.55f, 0.55f, 1.0f, 0.5f, 0.0f, 1.0f);
			}
			mode_change_show_time--;
		}

		if (app_logs_show)
		{
			int y_i = 0;
			for (int i = app_log_view_num; i < app_log_view_num + 23; i++)
			{
				Draw(app_logs[i], app_log_x, 10.0f + (y_i * 10), 0.4f, 0.4f, 0.0f, 0.5f, 1.0f, 1.0f);
				y_i++;
			}
		}

		if (message_jp_mode)
		{
			sprintf(info, "%dfps %.1fms/per frame %02d/%02d %02d:%02d:%02d %s%% %s wifi signal:%d"
				, fps_show, frame_time_point[0], month, day, hours, minutes, seconds, battery_level_message, battery_charge_message[(battery_charge + 2)].c_str(), wifi_signal);
			sprintf(debug_log_2, "%d 個のメッセージが検出されました。", number_of_message);
			sprintf(update_time_string, "%s %.2fms 費やしました。\n%s %.2fms 費やしました。\n%s %.2fms 費やしました。\n%s %.2fms 費やしました。\n%s %.2fms 費やしました(%uKB)。\n%s %.2fms 費やしました。\n%s %.2fms 費やしました。"
				, update_time_message[0].c_str(), update_time[0], update_time_message[1].c_str(), update_time[1], update_time_message[2].c_str()
				, update_time[2], update_time_message[3].c_str(), update_time[3], update_time_message[4].c_str(), update_time[4]
				, read_size / 1000, update_time_message[5].c_str(), update_time[5], update_time_message[6].c_str(), update_time[6]);
			sprintf(message_send_confirm, "%s へ \n message %s\n を送信しますか?\n Aで持続 Bで中止", id[room_select_num].c_str(), input_text.c_str());
		}
		else
		{
			sprintf(info, "%dfps %.1fms/per frame %02d/%02d %02d:%02d:%02d %s%% %s wifi signal:%d"
				, fps_show, frame_time_point[0], month, day, hours, minutes, seconds, battery_level_message, battery_charge_message[battery_charge].c_str(), wifi_signal);
			sprintf(debug_log_2, "%d message(s) found.", number_of_message);
			sprintf(update_time_string, "Spent %.2fms %s\nSpent %.2fms %s\nSpent %.2fms %s\nSpent %.2fms %s\nSpent %.2fms %s(%uKB)。\nSpent %.2fms %s\nSpent %.2fms %s\n"
				, update_time[0], update_time_message[7].c_str(), update_time[1], update_time_message[8].c_str(), update_time[2], update_time_message[9].c_str()
				, update_time[3], update_time_message[10].c_str(), update_time[4], update_time_message[11].c_str()
				, read_size / 1000, update_time[5], update_time_message[12].c_str(), update_time[6], update_time_message[13].c_str());
			sprintf(message_send_confirm, "to : %s \n message : %s\n Do you want to send message?\n A to continue B to abort", id[room_select_num].c_str(), input_text.c_str());
		}
		sprintf(debug_log, "id = %s", id[room_select_num].c_str());

		Draw("■", -30.0f, -2.75f, 20.0f, 0.6f, 0.0f, 0.0f, 0.0f, 0.7f);
		Draw(info, 0.0f, 0.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 1.0f);

		if (night_mode)
		{
			C2D_TargetClear(Screen_bot, C2D_Color32f(0, 0, 0, 0));
		}
		else
		{
			C2D_TargetClear(Screen_bot, C2D_Color32f(8, 8, 8, 0));
		}
		C2D_SceneBegin(Screen_bot);

		if (night_mode)
		{
			Draw("設定", 280.0f, 40.0f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.7f);
		}
		else
		{
			Draw("設定", 280.0f, 40.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		Draw(debug_log, 0.0f, 10.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
		Draw(debug_log_2, 0.0f, 40.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);

		Draw(ver, 0.0f, 0.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 1.0f);

		Draw(language_message, 0.0f, 220.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
		if (setting_menu_show)
		{
			//setting menu
			if (message_jp_mode)
			{
				Draw(auto_update_message[auto_update_mode + 2], 0.0f, 60.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
				Draw(lite_mode_message[dont_draw_outside_mode + 2], 0.0f, 80.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
				Draw(night_mode_message[night_mode + 2], 0.0f, 100.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
				Draw(frame_time_message[frame_time_show + 2], 0.0f, 120.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
				Draw(debug_log_message[update_detail_show + 2], 0.0f, 140.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
			}
			else
			{
				Draw(auto_update_message[auto_update_mode], 0.0f, 60.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
				Draw(lite_mode_message[dont_draw_outside_mode], 0.0f, 80.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
				Draw(night_mode_message[night_mode], 0.0f, 100.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
				Draw(frame_time_message[frame_time_show], 0.0f, 120.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
				Draw(debug_log_message[update_detail_show], 0.0f, 140.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
			}
		}
		else
		{
			if (frame_time_show)
			{
				sprintf(info, "%.1fms", frame_time_point[0]);
				if (night_mode)
				{
					Draw("■", -20.0f, 50.0f, 16.0f, 6.0f, 0.5f, 0.5f, 0.5f, 0.5f);
				}
				else
				{
					Draw("■", -20.0f, 50.0f, 16.0f, 6.0f, 0.0f, 0.0f, 0.0f, 0.7f);
				}
				for (int i = 268; i >= 0; i--)
				{
					frame_time_point[i + 1] = frame_time_point[i];
					if (190.0f - frame_time_point[i] <= 60)
					{
						Draw("●", i + 45.0f, 90.0f, 0.15f, 0.15f, 1.0f, 0.0f, 1.0f, 1.0f);
					}
					else
					{
						Draw("●", i + 45.0f, 220.0f - frame_time_point[i], 0.05f, 0.05f, 0.0f, 1.0f, 1.0f, 1.0f);
					}
				}
				if (220.0f - frame_time_point[0] <= 90)
				{
					Draw(info, 0.0f, 90.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.5f, 1.0f);
				}
				else
				{
					Draw(info, 0.0f, 220.0f - frame_time_point[0], 0.45f, 0.45f, 0.0f, 1.0f, 0.5f, 1.0f);
				}
			}

			if (update_detail_show)
			{
				Draw(update_time_string, 0.0f, 60.0f, 0.45f, 0.45f, 0.0f, 0.0f, 1.0f, 1.0f);
			}

			if (update_progress >= 9 || update_progress >= 19)
			{
				Draw(update_progress_message[update_progress], 0.0f, 30.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f);
			}
			else
			{
				Draw(update_progress_message[update_progress], 0.0f, 30.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
			}
		}

		if (!message_send_confirm_succes)
		{
			Draw(message_send_confirm, 15.0f, 115.0f, 0.45f, 0.45f, 1.0f, 1.0f, 0.0f, 1.0f);
		}

		if (kHeld & KEY_TOUCH)
		{
			Draw("●", touch_pos.px, touch_pos.py, 0.25f, 0.25f, 1.0f, 0.0f, 0.0f, 1.0f);
		}

		sprintf(message_select_num_string, "msg select = %d", message_select_num);
		Draw(message_select_num_string, 150.0f, 40.0f, 0.5f, 0.5f, 1.0f, 0.5f, 0.0f, 1.0f);

		if (send_debug_info)
		{
			Draw("Do you want to send debug info? A to continue B to abort", 0.0, 80.0, 0.5, 0.5, 0.0, 0.0, 1.0, 1.0);
			if (kDown & KEY_A)
			{
				char cache[2048];
				sprintf(cache, " ~~Start of debug data~~ ver : %s status : %s text size : %f text interval : %f app_log_x : %f app_log_view_num : %d text_x : %f text_y : %f touch.x : %d touch.y : %d current selected id : %s ~~End of debug data~~ ",
					ver, info, text_size, text_interval, app_log_x, app_log_view_num, text_x, text_y, touch_pos.px, touch_pos.py, id[room_select_num].c_str());
				input_text = cache;

				message_send_thread = threadCreate(Send_message, (void*)(""), STACKSIZE, 0x24, -1, false);
				send_debug_info = false;
			}
			if (kDown & KEY_B)
			{
				send_debug_info = false;
			}
		}



		C3D_FrameEnd(0);
		frame_time_point[0] = osTickCounterRead(&total_frame_time);
		fps += 1;

		if (!message_send_confirm_succes)
		{
			if (kDown & KEY_A)
			{
				message_send_confirm_succes = true;
				message_send_thread = threadCreate(Send_message, (void*)(""), STACKSIZE, 0x24, -1, false);
			}
			if (kDown & KEY_B)
			{
				message_send_confirm_succes = true;
			}
		}
		else
		{
			if (kDown & KEY_START)
			{
				if (Exit_check())
				{
					u64 time_out = 3000000000; //3s
					update_thread_run = false;
					function_result = threadJoin(update_thread, time_out);
					App_log_save("Thread", "Update thread exit=", function_result, true);
					log_download_thread_run = false;
					function_result = threadJoin(log_download_thread, time_out);
					App_log_save("Thread", "Get log thread exit=", function_result, true);
					hid_thread_run = false;
					function_result = threadJoin(hid_thread, time_out);
					App_log_save("Thread", "Hid thread exit=", function_result, true);
					break; //Break in order to return to hbmenu
				}
			}
			else if (kDown & KEY_SELECT)
			{
				message_jump_mode = true;
			}
			else if (kDown & KEY_A)
			{
				message_send_confirm_succes = false;
				swkbdInit(&swkbd_send_message, SWKBD_TYPE_NORMAL, 2, 8192);
				swkbdSetHintText(&swkbd_send_message, "メッセージを入力 / Type message here.");
				swkbdSetValidation(&swkbd_send_message, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
				swkbdSetFeatures(&swkbd_send_message, SWKBD_PREDICTIVE_INPUT);
				swkbdSetInitialText(&swkbd_send_message, clipboard[0].c_str());
				swkbdSetDictWord(&words[0], "ぬべ", "壁|՞ةڼ)イーヒヒヒヒヒヒｗｗｗｗｗｗｗｗｗｗｗ");
				swkbdSetDictWord(&words[1], "ぬべ", "┌(☝┌՞ ՞)☝キエェェェエェェwwwww");
				swkbdSetDictWord(&words[2], "ぬべ", "┌(┌ ՞ةڼ)┐<ｷｴｪｪｪｴｴｪｪｪ");
				swkbdSetDictWord(&words[3], "ぬべ", "└(՞ةڼ◔)」");
				swkbdSetDictWord(&words[4], "ぬべ", "(  ՞ةڼ  )");
				swkbdSetDictWord(&words[5], "ぬべ", "└(՞ةڼ◔)」");
				swkbdSetDictWord(&words[6], "びぇ", "。゜( ;⊃՞ةڼ⊂; )゜。びぇぇえええんｗｗｗｗ");
				swkbdSetDictWord(&words[7], "うえ", "(✌ ՞ةڼ ✌ )");
				swkbdSetDictionary(&swkbd_send_message, words, 8);

				swkbdSetStatusData(&swkbd_send_message, &swkbd_send_message_status, true, true);
				swkbdSetLearningData(&swkbd_send_message, &learn_data, true, true);
				press_button = swkbdInputText(&swkbd_send_message, swkb_input_text, 8192);
				input_text = swkb_input_text;
				if (input_text.length() > 2000)
				{
					input_text = input_text.substr(0, 1990);
				}
				if (press_button == SWKBD_BUTTON_LEFT)
				{
					message_send_confirm_succes = true;
				}
			}
			else if (kDown & KEY_B)
			{
				log_update = true;
			}
			else if (kDown & KEY_X)
			{
				char input_text[512];
				swkbdInit(&swkbd_add, SWKBD_TYPE_QWERTY, 2, 512);
				swkbdSetHintText(&swkbd_add, "メインURLを入力 / Main url type here.");
				swkbdSetValidation(&swkbd_add, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
				press_button = swkbdInputText(&swkbd_add, input_text, 512);
				if (!press_button == SWKBD_BUTTON_LEFT)
				{
					function_result = Save_to_file("main_url.txt", input_text, true);
					App_log_save("I/O", "Save_to_file(main_url.txt)=", function_result, false);
				}

				swkbdInit(&swkbd_add, SWKBD_TYPE_QWERTY, 2, 512);
				swkbdSetHintText(&swkbd_add, "投稿用URLを入力 / Post url type here.");
				swkbdSetValidation(&swkbd_add, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
				press_button = swkbdInputText(&swkbd_add, input_text, 512);
				if (!press_button == SWKBD_BUTTON_LEFT)
				{
					function_result = Save_to_file("post_url.txt", input_text, true);
					App_log_save("I/O", "Save_to_file(post_url.txt)=", function_result, false);
				}
			}
			else if ((kHeld & KEY_L && kDown & KEY_R) || (kDown & KEY_L && kHeld & KEY_R))
			{
				if (app_logs_show)
				{
					app_logs_show = false;
				}
				else
				{
					app_logs_show = true;
				}
			}
			else if (kDown & KEY_Y)
			{
				char input_text[512];

				swkbdInit(&swkbd_add, SWKBD_TYPE_QWERTY, 2, 256);
				swkbdSetHintText(&swkbd_add, "IDを入力 / Type id here.");
				swkbdSetInitialText(&swkbd_add, clipboard[0].c_str());
				swkbdSetValidation(&swkbd_add, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
				press_button = swkbdInputText(&swkbd_add, input_text, 256);
				if (!press_button == SWKBD_BUTTON_LEFT)
				{
					function_result = Save_new_id(input_text);
					App_log_save("I/O", "Save_new_id=", function_result, false);
				}
				function_result = Read_id();
				App_log_save("I/O", "Read_id=", function_result, false);
			}
			else if (kDown & KEY_DRIGHT)
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
				function_result = Load_from_log_sd(file_name);
				App_log_save("I/O", cache, function_result, false);
			}
			else if (kDown & KEY_DLEFT)
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
				function_result = Load_from_log_sd(file_name);
				App_log_save("I/O", cache, function_result, false);
			}
			else if (kDown & KEY_ZL)
			{
				if (message_select_mode)
				{
					message_select_mode = false;
				}
				else
				{
					message_select_mode = true;
				}
			}
			else if (kDown & KEY_ZR)
			{
				clipboard[0] = message_log[message_select_num];
			}
		}

		if (kDown & KEY_TOUCH)
		{
			if (setting_menu_show)
			{
				if (touch_pos.px > 0 && touch_pos.px < 140 && touch_pos.py > 60 && touch_pos.py < 80)
				{
					if (auto_update_mode)
					{
						auto_update_mode = false;
					}
					else
					{
						auto_update_mode = true;
					}
				}
				else if (touch_pos.px > 0 && touch_pos.px < 140 && touch_pos.py > 80 && touch_pos.py < 100)
				{
					mode_change_show_time = 90;
					if (dont_draw_outside_mode)
					{
						dont_draw_outside_mode = false;
					}
					else
					{
						dont_draw_outside_mode = true;
					}
				}
				else if (touch_pos.px > 0 && touch_pos.px < 140 && touch_pos.py > 100 && touch_pos.py < 120)
				{
					if (night_mode)
					{
						night_mode = false;
					}
					else
					{
						night_mode = true;
					}
				}
				else if (touch_pos.px > 0 && touch_pos.px < 140 && touch_pos.py > 120 && touch_pos.py < 140)
				{
					if (frame_time_show)
					{
						frame_time_show = false;
					}
					else
					{
						frame_time_show = true;
					}
				}
				else if (touch_pos.px > 0 && touch_pos.px < 140 && touch_pos.py > 140 && touch_pos.py < 160)
				{
					if (update_detail_show)
					{
						update_detail_show = false;
					}
					else
					{
						update_detail_show = true;
					}
				}
			}
			if (touch_pos.px > 0 && touch_pos.px < 140 && touch_pos.py > 220 && touch_pos.py < 240)
			{
				if (message_jp_mode)
				{
					message_jp_mode = false;
				}
				else
				{
					message_jp_mode = true;
				}
			}
			else if (touch_pos.px > 280 && touch_pos.px < 300 && touch_pos.py > 40 && touch_pos.py < 60)
			{
				if (setting_menu_show)
				{
					setting_menu_show = false;
				}
				else
				{
					setting_menu_show = true;
				}
			}
			else if (touch_pos.px > 280 && touch_pos.px < 300 && touch_pos.py > 60 && touch_pos.py < 80)
			{
				if (send_debug_info)
				{
					send_debug_info = false;
				}
				else
				{
					send_debug_info = true;
				}
			}
		}
	}

	aptExit();
	App_log_save("Exit", "aptExit...", 1234567890, true);
	acExit();
	App_log_save("Exit", "acInit...", 1234567890, true);
	mcuHwcExit();
	App_log_save("Exit", "mcuExit...", 1234567890, true);
	ptmuExit();
	App_log_save("Exit", "ptmuExit...", 1234567890, true);
	fsExit();
	App_log_save("Exit", "fsExit...", 1234567890, true);
	httpcExit();
	App_log_save("Exit", "httpcExit...", 1234567890, true);
	App_log_save("Exit", "Exiting...", 1234567890, true);
	usleep(500000);
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	return 0;
}