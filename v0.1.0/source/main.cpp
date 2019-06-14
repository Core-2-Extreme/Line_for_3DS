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
int log_num = 0;
float red_log = 0.0f;
float green_log = 0.0f;
float blue_log = 0.0f;
float text_x = 10.0;
float text_y = 10.0;
float text_interval = 35;
float text_size = 0.45;
u8* line_log_buffer;
u8* line_send_message_buffer;
u8 battery_level = -1;
u8 battery_charge = -1;
u32 read_size;
u32 written_size;

bool update_thread_run = false;
bool log_update_thread_run = false;
bool log_update = false;
bool dont_draw_outside = false;
bool message_jp = false;

char ver[] = "v0.1.0";
char dir_path[25] = "/Line";
char swkb_input_text[512];
char* message_log_cache[300];
std::string send_data;
std::string main_url;
std::string message_send_url;
std::string message_log[300];
std::string message_log_short[60000];
std::string battery_charge_string[4] = {"Discharging", "Charging", "放電している", "充電している"};
std::string lite_mode_string[4] = { "Litemode disable", "Litemode enable", "軽量モード無効" ,"軽量モード有効" };
std::string lite_mode_change_string[4] = { "Litemode has been disabled.", "Litemode has been enabled.", "軽量モードは無効になっています" ,"軽量モードは有効になっています" };
std::string id[100];

std::string update_progress_message[19] = {" " , 
"Initializing..." , 
"Initialized. Connecting to server..." ,  
"Connected to server. Getting new url...", 
"Got new url. Connecting to server..." , 
"Connected to server. Downloading data..." , 
"Download completed. Deleting old data..." , 
"Deleted old data. Parse download data..." , 
"Parse download data completed." ,
"Downloading data failed.",
"初期化している...",
"初期化された。 サーバーに接続している...",
"サーバーに接続された。 新規URLを取得している...", 
"新規URLが取得された。 サーバーに接続している...", 
"サーバーに接続された。 データをダウンロードしている...", 
"ダウンロード完了。 古いデータを削除している...", 
"古いデータが削除された。 ダウンロードデータを解析している...", 
"ダウンロードデータ解析完了。", 
"データダウンロード失敗。", };

Thread update_thread, log_download_thread, message_send_thread;
httpcContext line_message_send, line_message_log;
C2D_Text c2d_info, c2d_bot_debug, c2d_bot_debug_3, c2d_bot_debug_4, c2d_log, c2d_bot_lite_mode, c2d_bot_lite_mode_2, c2d_bot_touch_pos, c2d_mode_change, c2d_top_back_color, c2d_center_back_color;
SwkbdState swkbd_send_message, swkbd_add;
SwkbdLearningData learn_data;
SwkbdDictWord words[8];
SwkbdStatusData swkbd_send_message_status;
SwkbdButton press_button;
Result debug_result;
touchPosition touch_pos;
Result apt_result, mcu_result, ptmu_result, fs_result, http_result, add_id_result;
Handle fs_write_handle, fs_read_handle, fs_dir_read_handle;
FS_Archive fs_write_archive, fs_read_archive, fs_dir_read_archive;
FS_DirectoryEntry fs_id_entry;

void Get_log(void* arg)
{	
	while (log_update_thread_run)
	{
		usleep(100000);
		if (log_update)
		{
			log_update = true;
			red_log = 0.0f;
			blue_log = 1.0f;
			number_of_message = 0;
			log_num = 0;

			int text_length = 0;
			char* moved_url;
			std::string url;
			moved_url = (char*)malloc(0x1000);			
			if(message_jp)
			{ 
				update_progress = 10;
			}
			else
			{
				update_progress = 1;
			}			

			update_progress++;
			url = main_url;
			url += id[room_select_num];
			httpcOpenContext(&line_message_log, HTTPC_METHOD_GET, url.c_str(), 0);
			httpcSetSSLOpt(&line_message_log, SSLCOPT_DisableVerify);
			httpcSetKeepAlive(&line_message_log, HTTPC_KEEPALIVE_ENABLED);
			httpcAddRequestHeaderField(&line_message_log, "User-Agent", "Line for 3DS v0.1.1");
			httpcAddRequestHeaderField(&line_message_log, "Connection", "Keep-Alive");
			httpcBeginRequest(&line_message_log);

			update_progress++;
			httpcGetResponseHeader(&line_message_log, "Location", moved_url, 0x1000);
			httpcCloseContext(&line_message_log);

			update_progress++;
			httpcOpenContext(&line_message_log, HTTPC_METHOD_GET, moved_url, 0);
			httpcSetSSLOpt(&line_message_log, SSLCOPT_DisableVerify);
			httpcSetKeepAlive(&line_message_log, HTTPC_KEEPALIVE_ENABLED);
			httpcAddRequestHeaderField(&line_message_log, "User-Agent", "Line for 3DS v0.1.1");
			httpcAddRequestHeaderField(&line_message_log, "Connection", "Keep-Alive");
			httpcBeginRequest(&line_message_log);

			update_progress++;
			debug_result = httpcDownloadData(&line_message_log, line_log_buffer, 0x500000, &read_size);
			httpcCloseContext(&line_message_log);
			if (debug_result != 0)
			{
				if (message_jp)
				{
					update_progress = 18;
				}
				else
				{
					update_progress = 9;
				}
				blue_log = 0.0f;
				red_log = 1.0f;
				log_update = false;
			}
			else
			{
				update_progress++;
				for (int i = 0; i <= 299; i++)
				{
					message_log[i] = "";
				}
				for (int i = 0; i <= 59999; i++)
				{
					message_log_short[i] = "";
				}
				update_progress++;
				message_log_cache[0] = strtok((char*)line_log_buffer, "");
				message_log[0] = message_log_cache[0];
				number_of_message++;
				for (int i = 1; i <= 299; i++)
				{
					message_log_cache[i] = strtok(NULL, "");
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

				if (dont_draw_outside)
				{
					for (int i = 0; i < number_of_message; i++)
					{
						text_length = message_log[i].length();
						if (text_length > 160)
						{
							log_num++;
							message_log_short[log_num] = message_log[i].substr(0, 80);
							for (int k = 1; k <= 200; k++)
							{
								if (text_length <= 80 * k)
								{
									break;
								}
								log_num++;
								message_log_short[log_num] = message_log[i].substr(80 * k, 80);
							}
						}
						else
						{
							log_num++;
							if (text_length <= 80)
							{
								message_log_short[log_num] = message_log[i];
							}
							else if (text_length >= 81 && text_length <= 160)
							{
								message_log_short[log_num] = message_log[i].substr(0, 80);
								log_num++;
								message_log_short[log_num] = message_log[i].substr(80, text_length - 80);
							}
						}
					}

					for (int i = log_num + 1; i < 59999; i++)
					{
						message_log_short[i] = "";
					}
					update_progress++;
					text_y = (log_num - 5) * -text_interval;
				}
				else
				{
					update_progress++;
					text_y = (number_of_message - 5) * -text_interval;
				}

				free(moved_url);
				log_update = false;
			}
		}
	}	
}

void Send_message(void* arg)
{
	std::string send_data = "{ \"to\": \"";
	send_data += id[room_select_num];
	send_data += "\",\"message\" : \"";
	send_data += swkb_input_text;
	send_data += "\" }";

	httpcOpenContext(&line_message_send, HTTPC_METHOD_POST, message_send_url.c_str(), 0);
	httpcSetSSLOpt(&line_message_send, SSLCOPT_DisableVerify);
	httpcSetKeepAlive(&line_message_send, HTTPC_KEEPALIVE_ENABLED);
	httpcAddRequestHeaderField(&line_message_send, "Connection", "Keep-Alive");
	httpcAddRequestHeaderField(&line_message_send, "Content-Type", "application/json");
	httpcAddRequestHeaderField(&line_message_send, "User-Agent", "Line for 3DS v0.1.1");
	httpcAddPostDataRaw(&line_message_send, (u32*)send_data.c_str(), strlen(send_data.c_str()));
	httpcBeginRequest(&line_message_send);
	httpcDownloadData(&line_message_send, line_send_message_buffer, 0x5000, &read_size);
	httpcCloseContext(&line_message_send);

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
	add_id_result = FSUSER_CreateFile(fs_write_archive, path_file, FS_ATTRIBUTE_ARCHIVE, 0);

	if (add_id_result == 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

void Read_id(void)
{
	int count = 0;
	u32 read_entry = 0;
	u32 read_entry_count = 1;
	char* cache;
	cache = (char*)malloc(0x1000);

	FS_Path fs_path_empty = fsMakePath(PATH_EMPTY, "");
	FSUSER_OpenArchive(&fs_dir_read_archive, ARCHIVE_SDMC, fs_path_empty);
	FS_Path fs_path_dir = fsMakePath(PATH_ASCII, "/Line/to");
	FSUSER_OpenDirectory(&fs_dir_read_handle, fs_dir_read_archive, fs_path_dir);

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
}

int Save_to_file(std::string file_name, std::string data)
{
	char dir_path[25] = "/Line";
	FS_Path fs_path_dir = fsMakePath(PATH_ASCII, dir_path);
	FS_Path fs_path_empty = fsMakePath(PATH_EMPTY, "");
	FSUSER_OpenArchive(&fs_write_archive, ARCHIVE_SDMC, fs_path_empty);
	FSUSER_CreateDirectory(fs_write_archive, fs_path_dir, FS_ATTRIBUTE_DIRECTORY);

	strcat(dir_path, "/");
	char* file_path = strcat(dir_path, file_name.c_str());

	FS_Path path_file = fsMakePath(PATH_ASCII, file_path);
	FSUSER_CreateFile(fs_write_archive, path_file, FS_ATTRIBUTE_ARCHIVE, 0);
	FSUSER_OpenFile(&fs_write_handle, fs_write_archive, path_file, FS_OPEN_WRITE, FS_ATTRIBUTE_ARCHIVE);
	Result write_result = FSFILE_Write(fs_write_handle, &written_size, 0, data.c_str(), strlen(data.c_str()), FS_WRITE_FLUSH);
	FSFILE_Close(fs_write_handle);

	if (write_result == 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

std::string Load_from_file(std::string file_name)
{
	char dir_path[25] = "/Line";
	char read_data[1024] = "";
	FS_Path fs_path_empty = fsMakePath(PATH_EMPTY, "");
	FSUSER_OpenArchive(&fs_read_archive, ARCHIVE_SDMC, fs_path_empty);

	strcat(dir_path, "/");
	char* file_path = strcat(dir_path, file_name.c_str());
	FS_Path fs_path_dir = fsMakePath(PATH_ASCII, file_path);

	FSUSER_OpenFile(&fs_read_handle, fs_read_archive, fs_path_dir, FS_OPEN_WRITE, FS_ATTRIBUTE_ARCHIVE);
	Result read_result = FSFILE_Read(fs_read_handle, &read_size, 0, read_data, 1024);
	FSFILE_Close(fs_read_handle);

	if (read_result == 0)
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
	PTMU_GetBatteryChargeState(&battery_charge);//battery charge	
	MCUHWC_GetBatteryLevel(&battery_level);//battery level(%)
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
	while (update_thread_run)
	{
		usleep(1000000);
		fps_show = fps;
		fps = 0;
	}
}

bool Exit_check(void)
{
	while (true)
	{
		hidScanInput();
		u32 key = hidKeysDown();
		if (key & KEY_A)
		{
			return true;
		}
		else if (key & KEY_B)
		{
			return false;
		}
	}
}

int main()
{
	osSetSpeedupEnable(true); 
	gfxInitDefault();
	std::string cache;

	float info_red = 0.5f;
	float info_green = 0.5f;
	float info_blue = 0.5f;
	int press_time = 0;
	char info[100];
	char debug_log[100];
	char debug_log_2[100];
	line_send_message_buffer = (u8*)malloc(0x5000);
	line_log_buffer = (u8*)malloc(0x500000);
	for (int i = 0; i <= 299; i++)
	{
		message_log_cache[i] = (char*)malloc(0x2000);
		message_log[i] = (char*)malloc(0x2000);
	}
	for (int i = 0; i <= 59999; i++)
	{
		message_log_short[i] = (char*)malloc(0x100);
	}

	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	C3D_RenderTarget* Screen_top;
	C3D_RenderTarget* Screen_bot;

	Screen_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	Screen_bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	apt_result = aptInit();
	mcu_result = mcuHwcInit();
	ptmu_result = ptmuInit();
	fs_result = fsInit();
	http_result = httpcInit(0x50000);

	APT_SetAppCpuTimeLimit(30);
	aptSetSleepAllowed(true);

	update_thread_run = true;
	update_thread = threadCreate(Update_thread, (void*)(""), STACKSIZE, 0x24, -1, false);
	log_update_thread_run = true;
	log_download_thread = threadCreate(Get_log, (void*)(""), STACKSIZE, 0x24, 0, false);

	message_send_url = Load_from_file("post_url.txt");
	main_url = Load_from_file("main_url.txt");	
	Read_id();

	// Main loop
	while (aptMainLoop())
	{
		//Your code goes here
		hidScanInput();
		hidTouchRead(&touch_pos);
		Get_system_info();
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();

		C2D_TextBuf c2d_buf_debug_log = C2D_TextBufNew(8192);
		C2D_TextBuf c2d_buf_info = C2D_TextBufNew(4096);
		C2D_TextBuf c2d_buf_back_color = C2D_TextBufNew(4096);

		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(Screen_top, C2D_Color32f(8, 8, 8, 0));
		C2D_SceneBegin(Screen_top);

		C2D_TextBuf c2d_buf_log = C2D_TextBufNew(16384);
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
				if (dont_draw_outside)
				{
					C2D_TextParse(&c2d_log, c2d_buf_log, message_log_short[i].c_str());
				}
				else
				{
					if (i > 299)
					{
						break;
					}
					C2D_TextParse(&c2d_log, c2d_buf_log, message_log[i].c_str());
				}
				C2D_TextOptimize(&c2d_log);
				C2D_DrawText(&c2d_log, C2D_WithColor, text_x, text_y + text_interval * i, 0.0f, text_size, text_size, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f));
			}
		}
		C2D_TextBufDelete(c2d_buf_log);

		if (mode_change_show_time > 0)
		{
			C2D_TextParse(&c2d_center_back_color, c2d_buf_back_color, "■");
			C2D_TextOptimize(&c2d_center_back_color);
			C2D_DrawText(&c2d_center_back_color, C2D_WithColor, 85.0f, 115.0f, 0.0f, 10.0f, 0.55f, C2D_Color32f(0.0f, 0.0f, 0.0f, 0.5f));

			if (message_jp)
			{
				C2D_TextParse(&c2d_mode_change, c2d_buf_debug_log, lite_mode_change_string[dont_draw_outside + 2].c_str());
				C2D_TextOptimize(&c2d_mode_change);
			}
			else
			{
				C2D_TextParse(&c2d_mode_change, c2d_buf_debug_log, lite_mode_change_string[dont_draw_outside].c_str());
				C2D_TextOptimize(&c2d_mode_change);
			}

			if (dont_draw_outside)
			{				
				C2D_DrawText(&c2d_mode_change, C2D_WithColor, 105.0f, 115.0f, 0.0f, 0.55f, 0.55f, C2D_Color32f(0.0f, 1.0f, 1.0f, 1.0f));
			}
			else
			{
				C2D_DrawText(&c2d_mode_change, C2D_WithColor, 105.0f, 115.0f, 0.0f, 0.55f, 0.55f, C2D_Color32f(1.0f, 0.0f, 0.0f, 1.0f));
			}
			mode_change_show_time--;
		}
		
		if (message_jp)
		{
			sprintf(info, "%d fps CPU:%.0fms GPU:%.0fms /frame %02d/%02d %02d:%02d:%02d %02d%% %s"
				, fps_show, C3D_GetProcessingTime(), C3D_GetDrawingTime(), month, day, hours, minutes, seconds, battery_level, battery_charge_string[(battery_charge + 2)].c_str());
		}
		else
		{
			sprintf(info, "%d fps CPU:%.0fms GPU:%.0fms /frame %02d/%02d %02d:%02d:%02d %02d%% %s"
				, fps_show, C3D_GetProcessingTime(), C3D_GetDrawingTime(), month, day, hours, minutes, seconds, battery_level, battery_charge_string[battery_charge].c_str());
		}
		if (fps_show > 54)
		{
			info_red = 0.0f;
			info_green = 1.0f;
			info_blue = 0.0f;
		}
		else if (fps_show < 55 && fps_show > 22)
		{
			info_red = 1.0f;
			info_green = 1.0f;
			info_blue = 0.0f;
		}
		else
		{
			info_red = 1.0f;
			info_green = 0.0f;
			info_blue = 0.0f;
		}

		C2D_TextParse(&c2d_top_back_color, c2d_buf_back_color, "■");
		C2D_TextParse(&c2d_info, c2d_buf_info, info);
		C2D_TextOptimize(&c2d_top_back_color);
		C2D_TextOptimize(&c2d_info);
		C2D_DrawText(&c2d_top_back_color, C2D_WithColor, -50.0f, -5.0f, 0.0f, 30.0f, 0.7f, C2D_Color32f(0.0f, 0.0f, 0.0f, 0.95f));
		C2D_DrawText(&c2d_info, C2D_WithColor, 0.0f, 0.0f, 0.0f, 0.45f, 0.45f, C2D_Color32f(info_red, info_green, info_blue, 1.0f));

		C2D_TargetClear(Screen_bot, C2D_Color32f(8, 8, 8, 0));
		C2D_SceneBegin(Screen_bot);

		sprintf(debug_log, "id = %s", id[room_select_num].c_str());
		sprintf(debug_log_2, "%d message(s) found.", number_of_message);
		C2D_TextParse(&c2d_bot_debug, c2d_buf_debug_log, debug_log);
		C2D_TextParse(&c2d_bot_debug_3, c2d_buf_debug_log, update_progress_message[update_progress].c_str());
		C2D_TextParse(&c2d_bot_debug_4, c2d_buf_debug_log, debug_log_2);

		if (message_jp)
		{
			C2D_TextParse(&c2d_bot_lite_mode, c2d_buf_debug_log, lite_mode_string[dont_draw_outside + 2].c_str());
		}
		else
		{
			C2D_TextParse(&c2d_bot_lite_mode, c2d_buf_debug_log, lite_mode_string[dont_draw_outside].c_str());
		}
		C2D_TextParse(&c2d_bot_touch_pos, c2d_buf_debug_log, "●");
		C2D_TextOptimize(&c2d_bot_debug);
		C2D_TextOptimize(&c2d_bot_debug_3);
		C2D_TextOptimize(&c2d_bot_debug_4);
		C2D_TextOptimize(&c2d_bot_lite_mode);
		C2D_TextOptimize(&c2d_bot_touch_pos);
		C2D_DrawText(&c2d_bot_debug, C2D_WithColor, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f, C2D_Color32f(0.0f, 0.0f, 1.0f, 1.0f));
		C2D_DrawText(&c2d_bot_debug_3, C2D_WithColor, 0.0f, 30.0f, 0.0f, 0.5f, 0.5f, C2D_Color32f(red_log, green_log, blue_log, 1.0f));
		C2D_DrawText(&c2d_bot_debug_4, C2D_WithColor, 0.0f, 40.0f, 0.0f, 0.5f, 0.5f, C2D_Color32f(0.0f, 0.0f, 1.0f, 1.0f));
		C2D_DrawText(&c2d_bot_lite_mode, C2D_WithColor, 0.0f, 220.0f, 0.0f, 0.5f, 0.5f, C2D_Color32f(0.0f, 0.0f, 1.0f, 1.0f));

		if (kHeld & KEY_TOUCH)
		{
			C2D_DrawText(&c2d_bot_touch_pos, C2D_WithColor, touch_pos.px, touch_pos.py, 0.0f, 0.15f, 0.15f, C2D_Color32f(1.0f, 0.0f, 0.0f, 1.0f));
		}

		C3D_FrameEnd(0);
		C2D_TextBufDelete(c2d_buf_debug_log);
		C2D_TextBufDelete(c2d_buf_info);
		C2D_TextBufDelete(c2d_buf_back_color);

		fps += 1;

		if (kDown & KEY_START)
		{
			if (Exit_check())
			{
				break; //Break in order to return to hbmenu
			}
		}
		else if (kDown & KEY_A)
		{
			swkbdInit(&swkbd_send_message, SWKBD_TYPE_NORMAL, 2, 512);
			swkbdSetHintText(&swkbd_send_message, "メッセージを入力 / Type message here.");
			swkbdSetValidation(&swkbd_send_message, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
			swkbdSetFeatures(&swkbd_send_message, SWKBD_PREDICTIVE_INPUT);

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
			press_button = swkbdInputText(&swkbd_send_message, swkb_input_text, 2048);
			if (!press_button == SWKBD_BUTTON_LEFT)
			{
				message_send_thread = threadCreate(Send_message, (void*)(""), STACKSIZE, 0x24, -1, false);
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
			swkbdSetHintText(&swkbd_add, "中心URLを入力 / Main url type here.");
			swkbdSetValidation(&swkbd_add, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
			press_button = swkbdInputText(&swkbd_add, input_text, 512);
			if (!press_button == SWKBD_BUTTON_LEFT)
			{
				Save_to_file("main_url.txt", input_text);
			}

			swkbdInit(&swkbd_add, SWKBD_TYPE_QWERTY, 2, 512);
			swkbdSetHintText(&swkbd_add, "投稿URLを入力 / Post url type here.");
			swkbdSetValidation(&swkbd_add, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
			press_button = swkbdInputText(&swkbd_add, input_text, 512);
			if (!press_button == SWKBD_BUTTON_LEFT)
			{
				Save_to_file("post_url.txt", input_text);
			}
		}
		else if (kDown & KEY_Y)
		{
			char input_text[512];

			swkbdInit(&swkbd_add, SWKBD_TYPE_QWERTY, 2,	128);
			swkbdSetHintText(&swkbd_add, "IDを入力 / Type id here.");
			swkbdSetValidation(&swkbd_add, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
			press_button = swkbdInputText(&swkbd_add, input_text, 128);
			if (!press_button == SWKBD_BUTTON_LEFT)
			{
				Save_new_id(input_text);
			}
		}

		press_time++;
		if (kHeld & KEY_CPAD_UP)
		{
			if (press_time > 600)
			{
				text_y += 50.0f;
			}
			else if (press_time > 240)
			{
				text_y += 10.0f;
			}
			else
			{
				text_y += 5.0f;
			}
		}
		else if (kHeld & KEY_CPAD_DOWN)
		{
			if (press_time > 600)
			{
				text_y -= 50.0f;
			}
			else if (press_time > 240)
			{
				text_y -= 10.0f;
			}
			else
			{
				text_y -= 5.0f;
			}
		}
		else if (kHeld & KEY_CPAD_LEFT)
		{
			text_x += 5.0f;
		}
		else if (kHeld & KEY_CPAD_RIGHT)
		{
			text_x -= 5.0f;
		}
		else if (kHeld & KEY_R)
		{
			text_size += 0.005f;
			if (text_size > 2.5)
			{
				text_size = 2.5f;
			}
		}
		else if (kHeld & KEY_L)
		{
			text_size -= 0.005f;
			if (text_size < 0.25)
			{
				text_size = 0.25f;
			}
		}
		else if (kHeld & KEY_DDOWN)
		{
			text_interval -= 0.5;
			if (text_interval < 10)
			{
				text_interval = 10;
			}
			if (dont_draw_outside)
			{
				text_y = (log_num - 2) * - text_interval;
			}
			else
			{
				text_y = (number_of_message - 2) * - text_interval;
			}
		}
		else if (kHeld & KEY_DUP)
		{
			text_interval += 0.5;
			if (text_interval > 50)
			{
				text_interval = 50;
			}
			if (dont_draw_outside)
			{
				text_y = (log_num - 2) * - text_interval;
			}
			else
			{
				text_y = (number_of_message - 2) * -text_interval;
			}
		}
		else
		{
			press_time = 0;
		}

		if (kDown & KEY_DRIGHT)
		{
			room_select_num += 1;
			if (room_select_num > 99)
			{
				room_select_num = 99;
			}
		}
		else if (kDown & KEY_DLEFT)
		{
			room_select_num -= 1;
			if (room_select_num < 0)
			{
				room_select_num = 0;
			}
		}

		if (kDown & KEY_TOUCH)
		{
			if (touch_pos.px > 0 && touch_pos.px < 120 && touch_pos.py > 220 && touch_pos.py < 240)
			{
				mode_change_show_time = 90;
				if (dont_draw_outside)
				{
					dont_draw_outside = false;
				}
				else
				{
					dont_draw_outside = true;
				}
			}
			if (touch_pos.px > 0 && touch_pos.px < 120 && touch_pos.py > 200 && touch_pos.py < 220)
			{
				if (message_jp)
				{
					message_jp = false;
				}
				else
				{
					message_jp = true;
				}
			}
		}
	}

	aptExit();
	mcuHwcExit();
	ptmuExit();
	fsExit();
	gfxExit();
	return 0;
}