#include <3ds.h>
#include <unistd.h>
#include <string>
#include "citro2d.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"
#include "hid.hpp"
#include "draw.hpp"
#include "file.hpp"
#include "log.hpp"
#include "error.hpp"
#include "menu.hpp"
#include "types.hpp"
#include "share_function.hpp"
#include "setting_menu.hpp"
#include "camera.hpp"

bool cam_main_run = false;
bool cam_already_init = false;
bool cam_capture_thread_run = false;
bool cam_encode_thread_run = false;
bool cam_parse_thread_run = false;
bool cam_thread_suspend = false;
bool cam_burst_request = false;
bool cam_take_request = false;
bool cam_parse_request = false;
bool cam_encode_request[3] = { false, false, false, };
bool cam_change_resolution_request = false;
bool cam_change_fps_request = false;
bool cam_change_contrast_request = false;
bool cam_change_white_balance_request = false;
bool cam_change_lens_correction_request = false;
bool cam_change_camera_request = false;
bool cam_change_exposure_request = false;
bool cam_change_noise_filter_request = false;
u8* cam_capture_buffer[60];
u8* cam_capture_frame_buffer[4];
u8* cam_rgba8888_frame_buffer[2];
u32 cam_buffer_size = 0;
int cam_encode_thread_job_start_num[3] = { 0, 20, 40, };
int cam_encode_thread_progress[3] = { 0, 0, 0, };
int cam_encode_thread_num_of_jobs[3] = { 20, 20, 20, };
int cam_selected_num_of_encode_threads = 1;
int cam_selected_menu_mode = 0;
int cam_selected_jpg_quality = 90;
int cam_current_img_num = 0;
int cam_current_display_img_num = -1;
int cam_current_capture_resolution_mode = 2;
int cam_request_capture_resolution_mode = 2;
int cam_current_capture_exposure_mode = 0;
int cam_request_capture_exposure_mode = 0;
int cam_current_capture_noise_filter_mode = 1;
int cam_request_capture_noise_filter_mode = 1;
int cam_current_capture_fps_mode = 8;
int cam_request_capture_fps_mode = 8;
int cam_current_capture_contrast_mode = 5;
int cam_request_capture_contrast_mode = 5;
int cam_current_capture_white_balance_mode = 0;
int cam_request_capture_white_balance_mode = 0;
int cam_current_capture_lens_correction_mode = 0;
int cam_request_capture_lens_correction_mode = 0;
int cam_current_capture_camera_mode = 0;
int cam_request_capture_camera_mode = 0;
int cam_shutter_sound_mode = 0;
int cam_fps = 0;
int cam_writing = -1;
int cam_reading = -1;
int cam_encode_format_mode = 1;
std::string cam_msg[CAM_NUM_OF_MSG];
std::string cam_resolution_list[9];
std::string cam_exposure_list[6];
std::string cam_lens_correction_list[3];
std::string cam_white_balance_list[6];
std::string cam_contrast_list[11];
std::string cam_framelate_list[15];
std::string cam_format_name_list[2];
std::string cam_capture_thread_string = "Cam/Capture thread";
std::string cam_encode_thread_string = "Cam/Encode thread";
std::string cam_parse_thread_string = "Cam/Parse thread";
std::string cam_init_string = "Cam/Init";
std::string cam_exit_string = "Cam/Exit";
Thread cam_capture_thread, cam_encode_thread[3], cam_parse_thread;
C2D_Image cam_capture_image[4];

bool Cam_query_init_flag(void)
{
	return cam_already_init;
}

bool Cam_query_running_flag(void)
{
	return cam_main_run;
}

void Cam_resume(void)
{
	cam_thread_suspend = false;
	cam_main_run = true;
	Menu_suspend();
}

void Cam_suspend(void)
{
	cam_thread_suspend = true;
	cam_main_run = false;
	Menu_resume();
}

int Cam_convert_to_resolution(int resolution_mode, bool width)
{
	if (resolution_mode == 0)
	{
		if (width)
			return 640;
		else
			return 480;
	}
	else if (resolution_mode == 1)
	{
		if (width)
			return 512;
		else
			return 384;
	}
	else if (resolution_mode == 2)
	{
		if (width)
			return 400;
		else
			return 240;
	}
	else if (resolution_mode == 3)
	{
		if (width)
			return 352;
		else
			return 288;
	}
	else if (resolution_mode == 5)
	{
		if (width)
			return 320;
		else
			return 240;
	}
	else if (resolution_mode == 6)
	{
		if (width)
			return 256;
		else
			return 192;
	}
	else if (resolution_mode == 7)
	{
		if (width)
			return 176;
		else
			return 144;
	}
	else if (resolution_mode == 8)
	{
		if (width)
			return 160;
		else
			return 120;
	}
	else
		return -1;
}

int Cam_query_cam_settings(int setting_num)
{
	if (setting_num == CAM_RESOLUTION)
		return cam_request_capture_resolution_mode;
	else if (setting_num == CAM_FPS)
		return cam_request_capture_fps_mode;
	else if (setting_num == CAM_CONTRAST)
		return cam_request_capture_contrast_mode;
	else if (setting_num == CAM_WHITE_BALANCE)
		return cam_request_capture_white_balance_mode;
	else if (setting_num == CAM_LENS_CORRECTION)
		return cam_request_capture_lens_correction_mode;
	else if (setting_num == CAM_CAMERA)
		return cam_request_capture_camera_mode;
	else if (setting_num == CAM_EXPOSURE)
		return cam_request_capture_exposure_mode;
	else if (setting_num == CAM_NOISE_FILTER)
		return cam_request_capture_noise_filter_mode;
	else if (setting_num == CAM_SHUTTER_SOUND)
		return cam_shutter_sound_mode;
	else
		return - 1;
}

int Cam_query_framerate(void)
{
	return cam_fps;
}

bool Cam_query_operation_flag(int operation_num)
{
	if (operation_num == CAM_TAKE_PICTURES_REQUEST)
		return cam_take_request;
	else if (operation_num == CAM_BURST_PICTURES_REQUEST)
		return cam_burst_request;
	else if (operation_num == CAM_ENCODE_PICTURES_REQUEST_0)
		return cam_encode_request[0];
	else if (operation_num == CAM_ENCODE_PICTURES_REQUEST_1)
		return cam_encode_request[1];
	else if (operation_num == CAM_ENCODE_PICTURES_REQUEST_2)
		return cam_encode_request[2];
	else
		return false;
}

int Cam_query_selected_num(int item_num)
{
	if (item_num == CAM_SELECTED_ENCODE_THREAD_NUM)
		return cam_selected_num_of_encode_threads;
	else if (item_num == CAM_SELECTED_MENU_MODE)
		return cam_selected_menu_mode;
	else if (item_num == CAM_SELECTED_FORMAT_MODE)
		return cam_encode_format_mode;
	else if (item_num == CAM_SELECTED_JPG_QUALITY)
		return cam_selected_jpg_quality;
	else
		return -1;
}

void Cam_reset_framerate(void)
{
	cam_fps = 0;
}

void Cam_set_cam_settings(int setting_num, int num)
{
	int random_num;
	if (setting_num == CAM_RESOLUTION && num >= 0 && num <= 8)
	{
		if (num == 4)
		{
			random_num = 4;
			srand(time(NULL));
			while (random_num == 4)
			{
				random_num = rand() % 9;
				usleep(10000);
			}
			num = random_num;
		}
		cam_request_capture_resolution_mode = num;
		cam_change_resolution_request = true;
	}
	else if (setting_num == CAM_FPS && num >= 0 && num <= 14)
	{
		if (num == 13 || num == 14)
		{
			srand(time(NULL));
			if (num == 13)
			{
				random_num = 13;
				while (random_num == 13 || random_num == 0 || random_num == 3 || random_num == 4 || random_num == 5 
					|| random_num == 6 || random_num == 8)
				{
					random_num = rand() % 13;
					usleep(10000);
				}
			}
			else if(num == 14)
			{
				random_num = 14;
				while (random_num == 14 || random_num == 1 || random_num == 2 || random_num == 7 || random_num == 9
					|| random_num == 10 || random_num == 11 || random_num == 12)
				{
					random_num = rand() % 13;
					usleep(10000);
				}
			}
			num = random_num;
		}
		cam_request_capture_fps_mode = num;
		cam_change_fps_request = true;
	}
	else if (setting_num == CAM_CONTRAST && num >= 0 && num <= 10)
	{
		cam_request_capture_contrast_mode = num;
		cam_change_contrast_request = true;
	}
	else if (setting_num == CAM_WHITE_BALANCE && num >= 0 && num <= 5)
	{
		cam_request_capture_white_balance_mode = num;
		cam_change_white_balance_request = true;
	}
	else if (setting_num == CAM_LENS_CORRECTION && num >= 0 && num <= 2)
	{
		cam_request_capture_lens_correction_mode = num;
		cam_change_lens_correction_request = true;
	}
	else if (setting_num == CAM_CAMERA && num >= 0 && num <= 2)
	{
		cam_request_capture_camera_mode = num;
		cam_change_camera_request = true;
	}
	else if (setting_num == CAM_EXPOSURE && num >= 0 && num <= 5)
	{
		cam_request_capture_exposure_mode = num;
		cam_change_exposure_request = true;
	}
	else if (setting_num == CAM_NOISE_FILTER && num >= 0 && num <= 1)
	{
		cam_request_capture_noise_filter_mode = num;
		cam_change_noise_filter_request = true;
	}
	else if (setting_num == CAM_SHUTTER_SOUND && num >= 0 && num <= 1)
		cam_shutter_sound_mode = num;
}

void Cam_set_msg(int msg_num, int msg_type, std::string msg)
{
	if (msg_type == CAM_MSG && msg_num >= 0 && msg_num < CAM_NUM_OF_MSG)
		cam_msg[msg_num] = msg;
	else if (msg_type == CAM_OPTION_MSG && msg_num >= 0 && msg_num < CAM_NUM_OF_OPTION_MSG)
	{
		if (msg_num >= 0 && msg_num <= 8)
			cam_resolution_list[msg_num] = msg;
		else if (msg_num >= 9 && msg_num <= 14)
			cam_exposure_list[msg_num - 9] = msg;
		else if (msg_num >= 15 && msg_num <= 17)
			cam_lens_correction_list[msg_num - 15] = msg;
		else if (msg_num >= 18 && msg_num <= 23)
			cam_white_balance_list[msg_num - 18] = msg;
		else if (msg_num >= 24 && msg_num <= 34)
			cam_contrast_list[msg_num - 24] = msg;
		else if (msg_num >= 35 && msg_num <= 49)
			cam_framelate_list[msg_num - 35] = msg;
		else if (msg_num >= 50 && msg_num <= 51)
			cam_format_name_list[msg_num - 50] = msg;
	}
}

void Cam_set_operation_flag(int operation_num, bool flag)
{
	if (operation_num == CAM_TAKE_PICTURES_REQUEST)
		cam_take_request = flag;
	else if (operation_num == CAM_BURST_PICTURES_REQUEST)
		cam_burst_request = flag;
	else if (operation_num == CAM_ENCODE_PICTURES_REQUEST_0)
		cam_encode_request[0] = flag;
	else if (operation_num == CAM_ENCODE_PICTURES_REQUEST_1)
		cam_encode_request[1] = flag;
	else if (operation_num == CAM_ENCODE_PICTURES_REQUEST_2)
		cam_encode_request[2] = flag;
}

void Cam_set_selected_num(int item_num, int value)
{
	if (item_num == CAM_SELECTED_ENCODE_THREAD_NUM)
		cam_selected_num_of_encode_threads = value;
	else if (item_num == CAM_SELECTED_MENU_MODE)
		cam_selected_menu_mode = value;
	else if (item_num == CAM_SELECTED_FORMAT_MODE)
		cam_encode_format_mode = value;
	else if (item_num == CAM_SELECTED_JPG_QUALITY)
		cam_selected_jpg_quality = value;
}

Result_with_string Cam_set_capture_camera(int camera_num, int width, int height, u32* out_buffer_size)
{
	bool failed = false;
	Result_with_string result;
	result.code = 0;
	result.string = s_success;

	CAMU_Activate(SELECT_NONE);

	if (camera_num == 0)
		result.code = CAMU_Activate(SELECT_OUT1);
	else if (camera_num == 1)
		result.code = CAMU_Activate(SELECT_OUT2);
	else if (camera_num == 2)
		result.code = CAMU_Activate(SELECT_IN1);
	else
		result.code = -999;

	if (result.code != 0)
	{
		failed = true;
		result.string = "[Error] CAMU_Activate failed. ";
	}

	if (!failed)
		result = Cam_set_capture_size(width, height, out_buffer_size);

	return result;
}

Result_with_string Cam_set_capture_contrast(int contrast_num)
{
	CAMU_Contrast contrast;
	Result_with_string result;
	result.code = 0;
	result.string = s_success;

	if (contrast_num == 0)
		contrast = CONTRAST_PATTERN_01;
	else if (contrast_num == 1)
		contrast = CONTRAST_PATTERN_02;
	else if (contrast_num == 2)
		contrast = CONTRAST_PATTERN_03;
	else if (contrast_num == 3)
		contrast = CONTRAST_PATTERN_04;
	else if (contrast_num == 4)
		contrast = CONTRAST_PATTERN_05;
	else if (contrast_num == 5)
		contrast = CONTRAST_PATTERN_06;
	else if (contrast_num == 6)
		contrast = CONTRAST_PATTERN_07;
	else if (contrast_num == 7)
		contrast = CONTRAST_PATTERN_08;
	else if (contrast_num == 8)
		contrast = CONTRAST_PATTERN_09;
	else if (contrast_num == 9)
		contrast = CONTRAST_PATTERN_10;
	else if (contrast_num == 10)
		contrast = CONTRAST_PATTERN_11;
	else
		result.code = -999;

	if(result.code != -999)
		result.code = CAMU_SetContrast(SELECT_ALL, contrast);


	if (result.code != 0)
		result.string = "[Error] CAMU_SetContrast failed. ";

	return result;
}

Result_with_string Cam_set_capture_exposure(int exposure_num)
{
	Result_with_string result;
	result.code = 0;
	result.string = s_success;

	if (exposure_num >= 0 && exposure_num <= 5)
		result.code = CAMU_SetExposure(SELECT_ALL, (s8)exposure_num);
	else
		result.code = -999;

	if (result.code != 0)
		result.string = "[Error] CAMU_SetExposure failed. ";

	return result;
}

Result_with_string Cam_set_capture_fps(int fps_num)
{
	CAMU_FrameRate framerate;
	Result_with_string result;
	result.code = 0;
	result.string = s_success;

	if (fps_num == 0)
		framerate = FRAME_RATE_15;
	else if (fps_num == 1)
		framerate = FRAME_RATE_15_TO_5;
	else if (fps_num == 2)
		framerate = FRAME_RATE_15_TO_2;
	else if (fps_num == 3)
		framerate = FRAME_RATE_10;
	else if (fps_num == 4)
		framerate = FRAME_RATE_8_5;
	else if (fps_num == 5)
		framerate = FRAME_RATE_5;
	else if (fps_num == 6)
		framerate = FRAME_RATE_20;
	else if (fps_num == 7)
		framerate = FRAME_RATE_20_TO_5;
	else if (fps_num == 8)
		framerate = FRAME_RATE_30;
	else if (fps_num == 9)
		framerate = FRAME_RATE_30_TO_5;
	else if (fps_num == 10)
		framerate = FRAME_RATE_15_TO_10;
	else if (fps_num == 11)
		framerate = FRAME_RATE_20_TO_10;
	else if (fps_num == 12)
		framerate = FRAME_RATE_30_TO_10;
	else
		result.code = -999;

	if (result.code != -999)
		result.code = CAMU_SetFrameRate(SELECT_ALL, framerate);

	if (result.code != 0)
		result.string = "[Error] CAMU_SetFrameRate failed. ";

	return result;
}

Result_with_string Cam_set_capture_lens_correction(int lens_correction_num)
{
	CAMU_LensCorrection lens_correction;
	Result_with_string result;
	result.code = 0;
	result.string = s_success;

	if (lens_correction_num == 0)
		lens_correction = LENS_CORRECTION_OFF;
	else if (lens_correction_num == 1)
		lens_correction = LENS_CORRECTION_ON_70;
	else if (lens_correction_num == 2)
		lens_correction = LENS_CORRECTION_ON_90;
	else
		result.code = -999;

	if (result.code != -999)
		result.code = CAMU_SetLensCorrection(SELECT_ALL, lens_correction);

	if (result.code != 0)
		result.string = "[Error] CAMU_SetLensCorrection failed. ";

	return result;
}

Result_with_string Cam_set_capture_noise_filter(int noise_filter_num)
{
	Result_with_string result;
	result.code = 0;
	result.string = s_success;

	if (noise_filter_num >= 0 && noise_filter_num <= 1)
		result.code = CAMU_SetNoiseFilter(SELECT_ALL, noise_filter_num);
	else
		result.code = -999;

	if (result.code != 0)
		result.string = "[Error] CAMU_SetNoiseFilter failed. ";

	return result;
}

Result_with_string Cam_set_capture_size(int width, int height, u32* out_buffer_size)
{
	CAMU_Size size;
	bool failed = false;
	u32 buffer_size;
	Result_with_string result;
	result.code = 0;
	result.string = s_success;

	if (width == 640 && height == 480)
		size = SIZE_VGA;
	else if (width == 512 && height == 384)
		size = SIZE_DS_LCDx4;
	else if (width == 400 && height == 240)
		size = SIZE_CTR_TOP_LCD;
	else if (width == 352 && height == 288)
		size = SIZE_CIF;
	else if (width == 320 && height == 240)
		size = SIZE_QVGA;
	else if (width == 256 && height == 192)
		size = SIZE_DS_LCD;
	else if (width == 176 && height == 144)
		size = SIZE_QCIF;
	else if (width == 160 && height == 120)
		size = SIZE_QQVGA;
	else
		result.code = -999;

	if (result.code != -999)
		result.code = CAMU_SetSize(SELECT_ALL, size, CONTEXT_BOTH);

	if (result.code != 0)
	{
		failed = true;
		result.string = "[Error] CAMU_SetSize failed. ";
	}

	if (!failed)
	{
		result.code = CAMU_GetMaxBytes(&buffer_size, width, height);
		*out_buffer_size = buffer_size;
		if (result.code != 0)
		{
			failed = true;
			result.string = "[Error] CAMU_GetMaxBytes failed. ";
		}
	}

	if (!failed)
	{
		result.code = CAMU_SetTransferBytes(PORT_BOTH, buffer_size, width, height);
		if (result.code != 0)
			result.string = "[Error] CAMU_SetTransferBytes failed. ";
	}

	return result;
}

Result_with_string Cam_set_capture_white_balance(int white_balance_num)
{
	CAMU_WhiteBalance white_balance;
	Result_with_string result;
	result.code = 0;
	result.string = s_success;

	if (white_balance_num == 0)
		white_balance = WHITE_BALANCE_AUTO;
	else if (white_balance_num == 1)
		white_balance = WHITE_BALANCE_3200K;
	else if (white_balance_num == 2)
		white_balance = WHITE_BALANCE_4150K;
	else if (white_balance_num == 3)
		white_balance = WHITE_BALANCE_5200K;
	else if (white_balance_num == 4)
		white_balance = WHITE_BALANCE_6000K;
	else if (white_balance_num == 5)
		white_balance = WHITE_BALANCE_7000K;
	else
		result.code = -999;

	if (result.code != -999)
		result.code = CAMU_SetWhiteBalance(SELECT_ALL, white_balance);

	if (result.code != 0)
		result.string = "[Error] CAMU_SetWhiteBalance failed. ";

	return result;
}

Result_with_string Cam_take_picture(u8* capture_buffer, int size, bool out_cam_1, bool play_shutter_sound)
{
	bool failed = false;
	Handle receive = 0;
	Result_with_string result;
	result.code = 0;
	result.string = s_success;

	memset(capture_buffer, 0x0, size);

	if (!failed)
	{
		result.code = CAMU_StartCapture(PORT_BOTH);
		if (result.code != 0)
		{
			result.string = "[Error] CAMU_StartCapture failed. ";
			failed = true;
		}
	}

	if (!failed)
	{
		if (out_cam_1)
			result.code = CAMU_SetReceiving(&receive, capture_buffer, PORT_CAM2, size, (s16)cam_buffer_size);
		else
			result.code = CAMU_SetReceiving(&receive, capture_buffer, PORT_CAM1, size, (s16)cam_buffer_size);
		if (result.code != 0)
		{
			result.string = "[Error] CAMU_SetReceiving failed. ";
			failed = true;
		}
	}

	if (!failed)
	{
		result.code = svcWaitSynchronization(receive, 250000000);
		if (result.code != 0)
		{
			result.string = "[Error] svcWaitSynchronization failed. ";
			failed = true;
		}
	}

	if (play_shutter_sound && !failed)
	{
		result.code = CAMU_PlayShutterSound(SHUTTER_SOUND_TYPE_NORMAL);
		if (result.code != 0)
		{
			result.string = "[Error] CAMU_PlayShutterSound failed. ";
			failed = true;
		}
	}

	svcCloseHandle(receive);

	return result;
}

void Cam_encode_thread(void* arg)
{
	int thread_num = std::stoi((char*)arg);
	Log_log_save(cam_encode_thread_string + std::to_string(thread_num), "Thread started.", 1234567890, false);
	int log_num;
	int file_size;
	int stbi_result;
	u8* cam_rgb888_buffer;
	uint8_t* cam_png_buffer;
	FS_Archive fs_archive = 0;
	Handle fs_handle = 0;
	Result_with_string result;

	File_save_to_file(".", NULL, 0, "/Line/dcim/", true, fs_handle, fs_archive);

	while (cam_encode_thread_run)
	{
		if (cam_encode_request[thread_num])
		{
			cam_rgb888_buffer = (uint8_t*)malloc((Cam_convert_to_resolution(cam_current_capture_resolution_mode, true) * Cam_convert_to_resolution(cam_current_capture_resolution_mode, false) * 3));

			for (int k = cam_encode_thread_job_start_num[thread_num]; k < cam_encode_thread_job_start_num[thread_num] + cam_encode_thread_num_of_jobs[thread_num]; k++)
			{
				memset(cam_rgb888_buffer, 0x0, (Cam_convert_to_resolution(cam_current_capture_resolution_mode, true) * Cam_convert_to_resolution(cam_current_capture_resolution_mode, false) * 3));
				cam_png_buffer = (uint8_t*)malloc(0x80000);
				memset(cam_png_buffer, 0x0, 0x80000);
				if (cam_png_buffer == NULL)
				{
					Err_set_error_message("Out of memory.", "Couldn't allocate 'cam_png_buffer'(" + std::to_string((Cam_convert_to_resolution(cam_current_capture_resolution_mode, true) * Cam_convert_to_resolution(cam_current_capture_resolution_mode, false) * 3) / 1024) + "KB).", cam_encode_thread_string + std::to_string(thread_num), OUT_OF_MEMORY);
					Log_log_save(cam_encode_thread_string + std::to_string(thread_num), "Out of memory. ", OUT_OF_MEMORY, false);
					Err_set_error_show_flag(true);
				}
				else if (cam_capture_buffer[k] != NULL)
				{
					
					Draw_rgb565_to_abgr888_rgb888(cam_capture_buffer[k], cam_rgb888_buffer, Cam_convert_to_resolution(cam_current_capture_resolution_mode, true), Cam_convert_to_resolution(cam_current_capture_resolution_mode, false), true);
					free(cam_capture_buffer[k]);
					cam_capture_buffer[k] = NULL;

					if (cam_encode_format_mode == 0)
					{
						log_num = Log_log_save("Cam/Encode thread " + std::to_string(thread_num), "stbi_write_png_to_mem()...", 1234567890, false);
						cam_png_buffer = stbi_write_png_to_mem((const unsigned char*)cam_rgb888_buffer, 0, Cam_convert_to_resolution(cam_current_capture_resolution_mode, true), Cam_convert_to_resolution(cam_current_capture_resolution_mode, false), 3, &file_size);
						Log_log_add(log_num, "size : " + std::to_string(file_size / 1024) + "KB. ", 1234567890, false);
						if (cam_png_buffer != NULL)
						{
							log_num = Log_log_save("Cam/Encode thread " + std::to_string(thread_num), "(" + Menu_query_time() + ".png)...", 1234567890, false);
							result = File_save_to_file(Menu_query_time() + ".png", (u8*)cam_png_buffer, file_size, "/Line/dcim/PNG/", true, fs_handle, fs_archive);
							Log_log_add(log_num, result.string, result.code, false);
						}
					}
					else if (cam_encode_format_mode == 1)
					{
						log_num = Log_log_save("Cam/Encode thread " + std::to_string(thread_num), "stbi_write_jpg_to_mem()...", 1234567890, false);
						stbi_result = stbi_write_jpg_to_mem((void*)cam_png_buffer, Cam_convert_to_resolution(cam_current_capture_resolution_mode, true), Cam_convert_to_resolution(cam_current_capture_resolution_mode, false), 3, (const void*)cam_rgb888_buffer, cam_selected_jpg_quality, &file_size);
						Log_log_add(log_num, "size : " + std::to_string(file_size / 1024) + "KB. ", 1234567890, false);
						if (stbi_result != 0)
						{
							log_num = Log_log_save("Cam/Encode thread " + std::to_string(thread_num), "(" + Menu_query_time() + ".jpg)...", 1234567890, false);
							result = File_save_to_file(Menu_query_time() + ".jpg", (u8*)cam_png_buffer, file_size, "/Line/dcim/JPG/", true, fs_handle, fs_archive);
							Log_log_add(log_num, result.string, result.code, false);
						}
					}
				}

				free(cam_png_buffer);
				cam_png_buffer = NULL;
				cam_encode_thread_progress[thread_num]++;
			}
			free(cam_rgb888_buffer);
			cam_rgb888_buffer = NULL;
			cam_encode_request[thread_num] = false;
		}
		usleep(250000);
	}
	Log_log_save(cam_encode_thread_string + std::to_string(thread_num), "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Cam_capture_thread(void* arg)
{
	Log_log_save(cam_capture_thread_string, "Thread started.", 1234567890, false);
	bool out_left = false;
	int log_num;
	Result_with_string result;
	result.code = CAMU_Activate(SELECT_OUT1);

	while (cam_capture_thread_run)
	{
		while (cam_thread_suspend)
			usleep(250000);

		if (cam_take_request && !cam_encode_request[0] && !cam_encode_request[1] && !cam_encode_request[2])
		{
			log_num = Log_log_save(cam_capture_thread_string , "Cam_take_picture()...", 1234567890, false);
			cam_capture_buffer[0] = (u8*)malloc(Cam_convert_to_resolution(cam_current_capture_resolution_mode, true) * Cam_convert_to_resolution(cam_current_capture_resolution_mode, false) * 2);
			if (cam_capture_buffer[0] == NULL)
			{
				Err_set_error_message("Out of memory.", "Couldn't allocate 'cam capture buffer[0]'(" + std::to_string((Cam_convert_to_resolution(cam_current_capture_resolution_mode, true) * Cam_convert_to_resolution(cam_current_capture_resolution_mode, false) * 2) / 1024) + "KB).", cam_capture_thread_string, OUT_OF_MEMORY);
				Log_log_add(log_num, "Out of memory. ", OUT_OF_MEMORY, false);
				Err_set_error_show_flag(true);
			}
			else
			{
				if (cam_request_capture_camera_mode == 1)
					out_left = true;
				else
					out_left = false;

				result = Cam_take_picture(cam_capture_buffer[0], (Cam_convert_to_resolution(cam_current_capture_resolution_mode, true) * Cam_convert_to_resolution(cam_current_capture_resolution_mode, false) * 2), out_left, cam_shutter_sound_mode);

				Log_log_add(log_num, result.string, result.code, false);
				if (result.code != 0 && result.code != 0x9401BFE)
				{
					Err_set_error_message(result.string, result.error_description, cam_capture_thread_string , result.code);
					Err_set_error_show_flag(true);
				}
			}
			cam_take_request = false;

			cam_encode_thread_job_start_num[0] = 0;
			cam_encode_thread_job_start_num[1] = 0;
			cam_encode_thread_job_start_num[2] = 0;
			cam_encode_thread_num_of_jobs[0] = 1;
			cam_encode_thread_num_of_jobs[1] = 0;
			cam_encode_thread_num_of_jobs[2] = 0;

			for (int i = 0; i < 3; i++)
				cam_encode_thread_progress[i] = 0;

			cam_encode_request[0] = true;
		}
		/*else if (cam_burst_request && !cam_encode_request[0] && !cam_encode_request[1] && !cam_encode_request[2])
		{
			for (int i = 0; i < 60; i++)
			{
				log_num = Log_log_save(cam_capture_thread_string , "Cam_take_picture()...", 1234567890, false);
				cam_capture_buffer[i] = (u8*)malloc(Cam_convert_to_resolution(cam_current_capture_resolution_mode, true) * Cam_convert_to_resolution(cam_current_capture_resolution_mode, false) * 2);
				if (cam_capture_buffer[i] == NULL)
				{
					Err_set_error_message("Out of memory.", "Couldn't allocate 'cam capture buffer[" + std::to_string(i) + "]'(" + std::to_string((Cam_convert_to_resolution(cam_current_capture_resolution_mode, true) * Cam_convert_to_resolution(cam_current_capture_resolution_mode, false) * 2) / 1024) + "KB).", cam_capture_thread_string, OUT_OF_MEMORY);
					Log_log_add(log_num, "Out of memory. ", OUT_OF_MEMORY, false);
					Err_set_error_show_flag(true);
				}
				else
				{
					if (cam_request_capture_camera_mode == 1)
						result = Cam_take_picture(cam_capture_buffer[i], (Cam_convert_to_resolution(cam_current_capture_resolution_mode, true) * Cam_convert_to_resolution(cam_current_capture_resolution_mode, false) * 2), true, cam_shutter_sound_mode);
					else
						result = Cam_take_picture(cam_capture_buffer[i], (Cam_convert_to_resolution(cam_current_capture_resolution_mode, true) * Cam_convert_to_resolution(cam_current_capture_resolution_mode, false) * 2), false, cam_shutter_sound_mode);

					Log_log_add(log_num, result.string, result.code, false);
					if (result.code != 0 && result.code != 0x9401BFE)
					{
						Err_set_error_message(result.string, result.error_description, cam_capture_thread_string , result.code);
						Err_set_error_show_flag(true);
					}
				}
			}
			cam_burst_request = false;

			if (cam_selected_num_of_encode_threads == 1)
			{
				cam_encode_thread_job_start_num[0] = 0;
				cam_encode_thread_num_of_jobs[0] = 60;
			}
			else if (cam_selected_num_of_encode_threads == 2)
			{
				cam_encode_thread_job_start_num[0] = 0;
				cam_encode_thread_job_start_num[1] = 30;
				cam_encode_thread_num_of_jobs[0] = 30;
				cam_encode_thread_num_of_jobs[1] = 30;
			}
			else if (cam_selected_num_of_encode_threads == 3)
			{
				cam_encode_thread_job_start_num[0] = 0;
				cam_encode_thread_job_start_num[1] = 20;
				cam_encode_thread_job_start_num[2] = 40;
				cam_encode_thread_num_of_jobs[0] = 20;
				cam_encode_thread_num_of_jobs[1] = 20;
				cam_encode_thread_num_of_jobs[2] = 20;
			}
			for (int i = 0; i < 3; i++)
				cam_encode_thread_progress[i] = 0;
			for (int i = 0; i < cam_selected_num_of_encode_threads; i++)
				cam_encode_request[i] = true;
		}*/
		else if (cam_change_resolution_request || cam_change_fps_request || cam_change_contrast_request || cam_change_white_balance_request
			|| cam_change_lens_correction_request || cam_change_camera_request || cam_change_exposure_request || cam_change_noise_filter_request)
		{
			if (cam_change_resolution_request)
			{
				log_num = Log_log_save(cam_capture_thread_string, "Cam_set_capture_size()...", 1234567890, false);
				result = Cam_set_capture_size(Cam_convert_to_resolution(cam_request_capture_resolution_mode, true), Cam_convert_to_resolution(cam_request_capture_resolution_mode, false), &cam_buffer_size);
			}
			else if (cam_change_fps_request)
			{
				log_num = Log_log_save(cam_capture_thread_string, "Cam_set_capture_fps()...", 1234567890, false);
				result = Cam_set_capture_fps(cam_request_capture_fps_mode);
			}
			else if (cam_change_contrast_request)
			{
				log_num = Log_log_save(cam_capture_thread_string, "CAMU_SetContrast()...", 1234567890, false);
				result = Cam_set_capture_contrast(cam_request_capture_contrast_mode);
			}
			else if (cam_change_white_balance_request)
			{
				log_num = Log_log_save(cam_capture_thread_string, "Cam_set_capture_white_balance()...", 1234567890, false);
				result = Cam_set_capture_white_balance(cam_request_capture_white_balance_mode);
			}
			else if (cam_change_lens_correction_request)
			{
				log_num = Log_log_save(cam_capture_thread_string, "Cam_set_capture_lens_correction()...", 1234567890, false);
				result = Cam_set_capture_lens_correction(cam_request_capture_lens_correction_mode);
			}
			else if (cam_change_camera_request)
			{
				log_num = Log_log_save(cam_capture_thread_string, "Cam_set_capture_camera()...", 1234567890, false);
				result = Cam_set_capture_camera(cam_request_capture_camera_mode, Cam_convert_to_resolution(cam_current_capture_resolution_mode, true), Cam_convert_to_resolution(cam_current_capture_resolution_mode, false), &cam_buffer_size);
			}
			else if (cam_change_exposure_request)
			{
				log_num = Log_log_save(cam_capture_thread_string, "Cam_set_capture_exposure()...", 1234567890, false);
				result = Cam_set_capture_exposure(cam_request_capture_exposure_mode);
			}
			else if (cam_change_noise_filter_request)
			{
				log_num = Log_log_save(cam_capture_thread_string, "Cam_set_capture_noise_filter()...", 1234567890, false);
				result = Cam_set_capture_noise_filter(cam_request_capture_noise_filter_mode);
			}
			else
				log_num = 0;

			Log_log_add(log_num, result.string, result.code, false);

			if (result.code != 0)
			{
				cam_request_capture_resolution_mode = cam_current_capture_resolution_mode;
				Err_set_error_message(result.string, result.error_description, cam_capture_thread_string, result.code);
				Err_set_error_show_flag(true);
			}
			else
			{
				if (cam_change_resolution_request)
					cam_current_capture_resolution_mode = cam_request_capture_resolution_mode;
				else if (cam_change_fps_request)
					cam_current_capture_fps_mode = cam_request_capture_fps_mode;
				else if (cam_change_contrast_request)
					cam_current_capture_contrast_mode = cam_request_capture_contrast_mode;
				else if (cam_change_white_balance_request)
					cam_current_capture_white_balance_mode = cam_request_capture_white_balance_mode;
				else if (cam_change_lens_correction_request)
					cam_current_capture_lens_correction_mode = cam_request_capture_lens_correction_mode;
				else if (cam_change_camera_request)
					cam_current_capture_camera_mode = cam_request_capture_camera_mode;
				else if (cam_change_exposure_request)
					cam_current_capture_exposure_mode = cam_request_capture_exposure_mode;
				else if (cam_change_noise_filter_request)
					cam_current_capture_noise_filter_mode = cam_request_capture_noise_filter_mode;
			}

			if (cam_change_resolution_request)
				cam_change_resolution_request = false;
			else if (cam_change_fps_request)
				cam_change_fps_request = false;
			else if (cam_change_contrast_request)
				cam_change_contrast_request = false;
			else if (cam_change_white_balance_request)
				cam_change_white_balance_request = false;
			else if (cam_change_lens_correction_request)
				cam_change_lens_correction_request = false;
			else if (cam_change_camera_request)
				cam_change_camera_request = false;
			else if (cam_change_exposure_request)
				cam_change_exposure_request = false;
			else if (cam_change_noise_filter_request)
				cam_change_noise_filter_request = false;
		}
		/*else if (cam_change_fps_request)
		{
			cam_change_fps_request = false;
			log_num = Log_log_save(cam_capture_thread_string , "Cam_set_capture_fps()...", 1234567890, false);
			result = Cam_set_capture_fps(cam_request_capture_fps_mode);
			Log_log_add(log_num, result.string, result.code, false);

			if (result.code != 0)
			{
				cam_request_capture_fps_mode = cam_current_capture_fps_mode;
				Err_set_error_message(result.string, result.error_description, cam_capture_thread_string , result.code);
				Err_set_error_show_flag(true);
			}
			else
				cam_current_capture_fps_mode = cam_request_capture_fps_mode;
		}
		else if (cam_change_contrast_request)
		{
			cam_change_contrast_request = false;
			log_num = Log_log_save(cam_capture_thread_string , "CAMU_SetContrast()...", 1234567890, false);
			result = Cam_set_capture_contrast(cam_request_capture_contrast_mode);
			Log_log_add(log_num, result.string, result.code, false);

			if (result.code != 0)
			{
				cam_request_capture_contrast_mode = cam_current_capture_contrast_mode;
				Err_set_error_message(result.string, result.error_description, cam_capture_thread_string , result.code);
				Err_set_error_show_flag(true);
			}
			else
				cam_current_capture_contrast_mode = cam_request_capture_contrast_mode;
		}
		else if (cam_change_white_balance_request)
		{
			cam_change_white_balance_request = false;
			log_num = Log_log_save(cam_capture_thread_string , "Cam_set_capture_white_balance()...", 1234567890, false);
			result = Cam_set_capture_white_balance(cam_request_capture_white_balance_mode);
			Log_log_add(log_num, result.string, result.code, false);

			if (result.code != 0)
			{
				cam_request_capture_white_balance_mode = cam_current_capture_white_balance_mode;
				Err_set_error_message(result.string, result.error_description, cam_capture_thread_string , result.code);
				Err_set_error_show_flag(true);
			}
			else
				cam_current_capture_white_balance_mode = cam_request_capture_white_balance_mode;
		}
		else if (cam_change_lens_correction_request)
		{
			cam_change_lens_correction_request = false;
			log_num = Log_log_save(cam_capture_thread_string , "Cam_set_capture_lens_correction()...", 1234567890, false);
			result = Cam_set_capture_lens_correction(cam_request_capture_lens_correction_mode);
			Log_log_add(log_num, result.string, result.code, false);

			if (result.code != 0)
			{
				cam_request_capture_lens_correction_mode = cam_current_capture_lens_correction_mode;
				Err_set_error_message(result.string, result.error_description, cam_capture_thread_string , result.code);
				Err_set_error_show_flag(true);
			}
			else
				cam_current_capture_lens_correction_mode = cam_request_capture_lens_correction_mode;
		}
		else if (cam_change_camera_request)
		{
			cam_change_camera_request = false;
			log_num = Log_log_save(cam_capture_thread_string , "Cam_set_capture_camera()...", 1234567890, false);
			result = Cam_set_capture_camera(cam_request_capture_camera_mode, Cam_convert_to_resolution(cam_current_capture_resolution_mode, true), Cam_convert_to_resolution(cam_current_capture_resolution_mode, false), &cam_buffer_size);
			Log_log_add(log_num, result.string, result.code, false);

			if (result.code != 0)
			{
				cam_request_capture_camera_mode = cam_current_capture_camera_mode;
				Err_set_error_message(result.string, result.error_description, cam_capture_thread_string , result.code);
				Err_set_error_show_flag(true);
			}
			else
				cam_current_capture_camera_mode = cam_request_capture_camera_mode;
		}
		else if (cam_change_exposure_request)
		{
			cam_change_exposure_request = false;
			log_num = Log_log_save(cam_capture_thread_string , "Cam_set_capture_exposure()...", 1234567890, false);
			result = Cam_set_capture_exposure(cam_request_capture_exposure_mode);
			Log_log_add(log_num, result.string, result.code, false);

			if (result.code != 0)
			{
				cam_request_capture_exposure_mode = cam_current_capture_exposure_mode;
				Err_set_error_message(result.string, result.error_description, cam_capture_thread_string , result.code);
				Err_set_error_show_flag(true);
			}
			else
				cam_current_capture_exposure_mode = cam_request_capture_exposure_mode;
		}
		else if (cam_change_noise_filter_request)
		{
			cam_change_noise_filter_request = false;
			log_num = Log_log_save(cam_capture_thread_string , "Cam_set_capture_noise_filter()...", 1234567890, false);
			result = Cam_set_capture_noise_filter(cam_request_capture_noise_filter_mode);
			Log_log_add(log_num, result.string, result.code, false);

			if (result.code != 0)
			{
				cam_request_capture_noise_filter_mode = cam_current_capture_noise_filter_mode;
				Err_set_error_message(result.string, result.error_description, cam_capture_thread_string , result.code);
				Err_set_error_show_flag(true);
			}
			else
				cam_current_capture_noise_filter_mode = cam_request_capture_noise_filter_mode;
		}*/
		else
		{
			if (cam_request_capture_camera_mode == 1)
				out_left = true;
			else
				out_left = false;

			result = Cam_take_picture(cam_capture_frame_buffer[3], (Cam_convert_to_resolution(cam_current_capture_resolution_mode, true) * Cam_convert_to_resolution(cam_current_capture_resolution_mode, false) * 2), out_left, false);

			if (result.code == 0)
			{
				cam_writing = 0;
				while (cam_reading == 0)
					usleep(5000);

				memcpy(cam_capture_frame_buffer[0], cam_capture_frame_buffer[3], (Cam_convert_to_resolution(cam_current_capture_resolution_mode, true) * Cam_convert_to_resolution(cam_current_capture_resolution_mode, false) * 2));
			}
			cam_writing = -1;

			if (result.code == 0)
			{
				if (cam_current_img_num == 0)
					cam_current_img_num = 1;
				else
					cam_current_img_num = 0;

				cam_fps++;
				cam_parse_request = true;
			}
			else
			{
				Log_log_save(cam_capture_thread_string , result.string, result.code, false);
				usleep(50000);
			}
		}
	}

	result.code = CAMU_Activate(SELECT_NONE);

	Log_log_save(cam_capture_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Cam_parse_thread(void* arg)
{
	Log_log_save(cam_parse_thread_string, "Thread started.", 1234567890, false);
	int previous_num= 0;
	int processing_num[2] = { 1, 2 };
	int tex_size = 512;
	C3D_Tex* c3d_cache_tex[4];
	Tex3DS_SubTexture* c3d_cache_subtex[4];
	Result_with_string result;

	for (int i = 0; i < 4; i++)
	{
		c3d_cache_tex[i] = (C3D_Tex*)linearAlloc(sizeof(C3D_Tex));
		c3d_cache_subtex[i] = (Tex3DS_SubTexture*)linearAlloc(sizeof(Tex3DS_SubTexture));
	}

	while (cam_parse_thread_run)
	{
		while (cam_thread_suspend)
			usleep(250000);

		if (cam_parse_request)
		{
			cam_parse_request = false;

			if (previous_num!= cam_current_img_num)
			{
				previous_num= cam_current_img_num;

				if (cam_current_img_num == 0)
				{
					cam_reading = 0;
					while (cam_writing == 0)
						usleep(5000);

					memcpy(cam_capture_frame_buffer[1], cam_capture_frame_buffer[0], (Cam_convert_to_resolution(cam_current_capture_resolution_mode, true) * Cam_convert_to_resolution(cam_current_capture_resolution_mode, false) * 2));
				}
				else
				{
					cam_reading = 0;
					while (cam_writing == 0)
						usleep(5000);

					memcpy(cam_capture_frame_buffer[2], cam_capture_frame_buffer[0], (Cam_convert_to_resolution(cam_current_capture_resolution_mode, true) * Cam_convert_to_resolution(cam_current_capture_resolution_mode, false) * 2));
				}
				cam_reading = -1;

				if (cam_current_display_img_num == 0)
				{
					processing_num[1] = 1;
					processing_num[2] = 2;
				}
				else
				{
					processing_num[1] = 0;
					processing_num[2] = 0;
				}

				for (int i = 0; i < 2; i++)
				{
					C3D_TexDelete(c3d_cache_tex[processing_num[2] + i]);
					cam_capture_image[processing_num[2] + i].tex = c3d_cache_tex[processing_num[2] + i];
					cam_capture_image[processing_num[2] + i].subtex = c3d_cache_subtex[processing_num[2] + i];
				}

				Draw_rgb565_to_abgr888_rgb888(cam_capture_frame_buffer[0], cam_rgba8888_frame_buffer[processing_num[1]], Cam_convert_to_resolution(cam_current_capture_resolution_mode, true), Cam_convert_to_resolution(cam_current_capture_resolution_mode, false), false);

				if(cam_current_capture_resolution_mode >= 0 && cam_current_capture_resolution_mode <= 5)
					tex_size = 512;
				else if(cam_current_capture_resolution_mode >= 6)
					tex_size = 256;

				result = Draw_c3dtex_to_c2dimage(c3d_cache_tex[processing_num[2]], c3d_cache_subtex[processing_num[2]], cam_rgba8888_frame_buffer[processing_num[1]], (Cam_convert_to_resolution(cam_current_capture_resolution_mode, true) * Cam_convert_to_resolution(cam_current_capture_resolution_mode, false) * 4), Cam_convert_to_resolution(cam_current_capture_resolution_mode, true), Cam_convert_to_resolution(cam_current_capture_resolution_mode, false), 0, 0, tex_size, tex_size, GPU_RGBA8);
				if(cam_current_capture_resolution_mode == 0)
					result = Draw_c3dtex_to_c2dimage(c3d_cache_tex[processing_num[2] + 1], c3d_cache_subtex[processing_num[2] + 1], cam_rgba8888_frame_buffer[processing_num[1]], (Cam_convert_to_resolution(cam_current_capture_resolution_mode, true) * Cam_convert_to_resolution(cam_current_capture_resolution_mode, false) * 4), Cam_convert_to_resolution(cam_current_capture_resolution_mode, true), Cam_convert_to_resolution(cam_current_capture_resolution_mode, false), 512, 0, 128, 512, GPU_RGBA8);

				if (result.code == 0)
				{
					if (processing_num[1] == 1)
						cam_current_display_img_num = 1;
					else
						cam_current_display_img_num = 0;
				}
			}
		}
		else
			usleep(10000);
	}

	for (int i = 0; i < 4; i++)
	{
		C3D_TexDelete(c3d_cache_tex[i]);
		linearFree(c3d_cache_tex[i]);
		c3d_cache_tex[i] = NULL;
		linearFree(c3d_cache_subtex[i]);
		c3d_cache_subtex[i] = NULL;
	}

	Log_log_save(cam_parse_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Cam_exit(void)
{
	Log_log_save(cam_exit_string , "Exiting...", 1234567890, s_debug_slow);
	u64 time_out = 10000000000;
	int log_num;
	bool failed = false;
	Result_with_string result;

	Draw_progress("[Cam] Exiting...");
	cam_already_init = false;
	cam_thread_suspend = false;
	cam_capture_thread_run = false;
	cam_encode_thread_run = false;
	cam_parse_thread_run = false;

	for (int i = 0; i < 5; i++)
	{
		log_num = Log_log_save(cam_exit_string, "Exiting thread(" + std::to_string(i) + "/4)...", 1234567890, s_debug_slow);

		if (i >= 0 && i <= 2)
			result.code = threadJoin(cam_encode_thread[i], time_out);
		else if (i == 3)
			result.code = threadJoin(cam_parse_thread, time_out);
		else if (i == 4)
			result.code = threadJoin(cam_capture_thread, time_out);

		if (result.code == 0)
			Log_log_add(log_num, s_success, result.code, s_debug_slow);
		else
		{
			failed = true;
			Log_log_add(log_num, s_error, result.code, s_debug_slow);
		}
	}

	for (int i = 0; i < 3; i++)
		threadFree(cam_encode_thread[i]);
	threadFree(cam_parse_thread);
	threadFree(cam_capture_thread);

	camExit();

	for (int i = 0; i < 2; i++)
	{
		free(cam_rgba8888_frame_buffer[i]);
		cam_rgba8888_frame_buffer[i] = NULL;
	}
	for (int i = 0; i < 4; i++)
	{
		free(cam_capture_frame_buffer[i]);
		cam_capture_frame_buffer[i] = NULL;
	}

	if (failed)
		Log_log_save(cam_exit_string , "[Warn] Some function returned error.", 1234567890, s_debug_slow);

}

Result_with_string Cam_cam_init(void)
{
	Result_with_string result;
	bool failed = false;
	result.code = 0;
	result.string = s_success;

	result = Cam_set_capture_size(400, 240, &cam_buffer_size);
	if (result.code != 0)
		failed = true;

	if (!failed)
	{
		result.code = CAMU_SetOutputFormat(SELECT_ALL, OUTPUT_RGB_565, CONTEXT_BOTH);
		if (result.code != 0)
			failed = true;
	}

	if (!failed)
	{
		result.code = CAMU_SetNoiseFilter(SELECT_ALL, false);
		if (result.code != 0)
			failed = true;
	}

	if (!failed)
	{
		result.code = CAMU_SetAutoExposure(SELECT_ALL, false);
		if (result.code != 0)
			failed = true;
	}

	if (!failed)
	{
		result = Cam_set_capture_white_balance(0);
		if (result.code != 0)
			failed = true;
	}

	if (!failed)
	{
		result.code = CAMU_SetPhotoMode(SELECT_ALL, PHOTO_MODE_NORMAL);
		if (result.code != 0)
			failed = true;
	}

	if (!failed)
	{
		result.code = CAMU_SetTrimming(PORT_BOTH, false);
		if (result.code != 0)
			failed = true;
	}

	if (!failed)
	{
		result = Cam_set_capture_fps(8);
		if (result.code != 0)
			failed = true;
	}

	if (!failed)
	{
		result = Cam_set_capture_contrast(5);
		if (result.code != 0)
			failed = true;
	}

	return result;
}

void Cam_init(void)
{
	Log_log_save(cam_init_string , "Initializing...", 1234567890, s_debug_slow);
	bool failed = false;
	int log_num;
	Result_with_string result;

	Draw_progress("0/2 [Cam] Initializing service...");
	log_num = Log_log_save(cam_init_string , "camInit...", 1234567890, s_debug_slow);
	result.code = camInit();
	if (result.code == 0)
		Log_log_add(log_num, s_success, result.code, s_debug_slow);
	else
	{
		failed = true;
		Err_set_error_message("camInit failed. ", "", cam_init_string , result.code);
		Err_set_error_show_flag(true);
		Log_log_add(log_num,s_error, result.code, s_debug_slow);
	}

	Draw_progress("1/2 [Cam] Initializing camera...");
	if (!failed)
	{
		log_num = Log_log_save(cam_init_string , "Initializing camera...", 1234567890, s_debug_slow);
		result = Cam_cam_init();
		Log_log_add(log_num, result.string, result.code, s_debug_slow);
		if (result.code != 0)
		{
			Err_set_error_message(result.string, result.error_description, cam_init_string , result.code);
			Err_set_error_show_flag(true);
		}

		log_num = Log_log_save(cam_init_string , "APT_SetAppCpuTimeLimit_80...", 1234567890, s_debug_slow);
		result.code = APT_SetAppCpuTimeLimit(80);
		if (result.code == 0)
			Log_log_add(log_num, s_success, result.code, s_debug_slow);
		else
			Log_log_add(log_num,s_error, result.code, s_debug_slow);
	}

	if (!failed)
	{
		for (int i = 0; i < 2; i++)
		{
			cam_rgba8888_frame_buffer[i] = (u8*)malloc(640 * 480 * 4);
			if (cam_rgba8888_frame_buffer[i] == NULL)
			{
				Err_set_error_message("Out of memory.", "Couldn't allocate 'cam_rgba8888_frame_buffer[" + std::to_string(i) + "]'\n(" + std::to_string(640 * 480 * 4 / 1024) + "KB).", cam_init_string , OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				failed = true;
				break;
			}
		}
	}
	if (!failed)
	{
		for (int i = 0; i < 4; i++)
		{
			cam_capture_frame_buffer[i] = (u8*)malloc(640 * 480 * 2);
			if (cam_capture_frame_buffer[i] == NULL)
			{
				Err_set_error_message("Out of memory.", "Couldn't allocate 'cam_capture_frame_buffer[" + std::to_string(i) + "]'\n(" + std::to_string(640 * 480 * 2 / 1024) + "KB).", cam_init_string , OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				failed = true;
				break;
			}
		}
	}

	Draw_progress("2/2 [Cam] Starting threads...");
	if (!failed)
	{
		cam_capture_thread_run = true;
		cam_encode_thread_run = true;
		cam_parse_thread_run = true;
		cam_capture_thread = threadCreate(Cam_capture_thread, (void*)(""), STACKSIZE, 0x23, -1, false);
		cam_encode_thread[0] = threadCreate(Cam_encode_thread, (void*)("0"), STACKSIZE, 0x23, 0, false);
		//cam_encode_thread[1] = threadCreate(Cam_encode_thread, (void*)("1"), STACKSIZE, 0x23, 1, false);
		//cam_encode_thread[2] = threadCreate(Cam_encode_thread, (void*)("2"), STACKSIZE, 0x23, 2, false);
		cam_parse_thread = threadCreate(Cam_parse_thread, (void*)(""), STACKSIZE, 0x24, -1, false);
	}

	cam_current_display_img_num = -1;
	cam_current_capture_resolution_mode = 2;

	Cam_resume();
	cam_already_init = true;
	Log_log_save(cam_init_string , "Initialized", 1234567890, s_debug_slow);
}

void Cam_main(void)
{
	int image_num = 0;
	int log_y = Log_query_y();
	double log_x = Log_query_x();
	float text_red;
	float text_green;
	float text_blue;
	float text_alpha;
	double draw_x = 0.0;
	double draw_y = 0.0;

	if (Sem_query_settings(SEM_NIGHT_MODE))
	{
		text_red = 1.0f;
		text_green = 1.0f;
		text_blue = 1.0f;
		text_alpha = 0.75f;
		white_or_black_tint = white_tint;
	}
	else
	{
		text_red = 0.0f;
		text_green = 0.0f;
		text_blue = 0.0f;
		text_alpha = 1.0f;
		white_or_black_tint = black_tint;
	}

	Draw_set_draw_mode(Sem_query_settings(SEM_VSYNC_MODE));
	if (Sem_query_settings(SEM_NIGHT_MODE))
		Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);
	
	if (cam_current_display_img_num != -1)
	{
		if (cam_current_display_img_num == 1)
			image_num = 2;
		else
			image_num = 0;

		if (Cam_convert_to_resolution(cam_current_capture_resolution_mode, true) == 400 && Cam_convert_to_resolution(cam_current_capture_resolution_mode, false) == 240)
			Draw_texture(cam_capture_image, dammy_tint, image_num, 0.0, 0.0, 400.0, 240.0);
		else if (cam_current_capture_resolution_mode == 0)
		{
			Draw_texture(cam_capture_image, dammy_tint, image_num, 40.0, 0.0, 256.0, 240.0);
			Draw_texture(cam_capture_image, dammy_tint, image_num + 1, 296.0, 0.0, 64.0, 240.0);
		}
		else
			Draw_texture(cam_capture_image, dammy_tint, image_num, 40.0, 0.0, 320.0, 240.0);
	}

	Draw_top_ui();
	if (Sem_query_settings(SEM_DEBUG_MODE))
		Draw_debug_info();
	if (Log_query_log_show_flag())
	{
		for (int i = 0; i < 23; i++)
			Draw(Log_query_log(log_y + i), 0, log_x, 10.0f + (i * 10), 0.4, 0.4, 0.0, 0.5, 1.0, 1.0);
	}

	if (Sem_query_settings(SEM_NIGHT_MODE))
		Draw_screen_ready_to_draw(1, true, 2, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(1, true, 2, 1.0, 1.0, 1.0);

	Draw(s_cam_ver, 0, 0.0, 0.0, 0.4, 0.4, 0.0, 1.0, 0.0, 1.0);
	Draw(std::to_string(Cam_convert_to_resolution(cam_current_capture_resolution_mode, true)) + cam_msg[0] + std::to_string(Cam_convert_to_resolution(cam_current_capture_resolution_mode, false)) + cam_msg[1] + cam_framelate_list[cam_current_capture_fps_mode] + cam_msg[2], 0, 20.0, 20.0, 0.6, 0.6, text_red, text_green, text_blue, text_alpha);
	Draw(cam_msg[3] + cam_contrast_list[cam_current_capture_contrast_mode] + cam_msg[4] + cam_white_balance_list[cam_current_capture_white_balance_mode], 0, 20.0, 35.0, 0.55, 0.55, text_red, text_green, text_blue, text_alpha);
	Draw(cam_msg[5] + cam_lens_correction_list[cam_current_capture_lens_correction_mode] + cam_msg[6] + cam_exposure_list[cam_current_capture_exposure_mode], 0, 20.0, 50.0, 0.55, 0.55, text_red, text_green, text_blue, text_alpha);	
	Draw(cam_msg[30] + cam_format_name_list[cam_encode_format_mode] + cam_msg[31], 0, 20.0, 60.0, 0.5, 0.5, 1.0, 0.0, 0.0, 1.0);

	Draw_texture(Square_image, weak_yellow_tint, 0, 40.0, 110.0, 60.0, 10.0);
	Draw_texture(Square_image, weak_blue_tint, 0, 100.0, 110.0, 60.0, 10.0);
	Draw_texture(Square_image, weak_aqua_tint, 0, 160.0, 110.0, 60.0, 10.0);
	Draw_texture(Square_image, weak_red_tint, 0, 220.0, 110.0, 60.0, 10.0);
	Draw_texture(Square_image, weak_aqua_tint, 0, 40.0, 120.0, 240.0, 80.0);

	for (int i = 0; i < 3; i++)
	{
		Draw_texture(Square_image, weak_aqua_tint, 0, 40.0 + (i * 85.0), 85.0, 70.0, 20.0);
		Draw(cam_msg[27 + i], 0, 40.0 + (i * 85.0), 85.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	}

	if (cam_selected_menu_mode == CAM_MENU_RESOLUTION)
	{
		Draw_texture(Square_image, yellow_tint, 0, 40.0, 110.0, 60.0, 10.0);
		draw_x = 40.0;
		draw_y = 120.0;
		for (int i = 0; i < 9; i++)
		{
			Draw_texture(Square_image, weak_aqua_tint, 0, draw_x, draw_y, 60.0, 20.0);
			if(i == cam_current_capture_resolution_mode)
				Draw(cam_resolution_list[i], 0, (draw_x + 2.5), draw_y, 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
			else
				Draw(cam_resolution_list[i], 0, (draw_x + 2.5), draw_y, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);

			if (draw_y + 30.0 > 180.0)
			{
				draw_x += 90.0;
				draw_y = 120.0;
			}
			else
				draw_y += 30.0;
		}
	}
	else if (cam_selected_menu_mode == CAM_MENU_FPS)
	{
		Draw_texture(Square_image, weak_blue_tint, 0, 100.0, 110.0, 60.0, 10.0);
		draw_x = 40.0;
		draw_y = 120.0;
		for (int i = 0; i < 15; i++)
		{
			Draw_texture(Square_image, weak_aqua_tint, 0, draw_x, draw_y, 30.0, 20.0);
			if(i == cam_current_capture_fps_mode)
				Draw(cam_framelate_list[i], 0, (draw_x + 2.5), draw_y, 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
			else
				Draw(cam_framelate_list[i], 0, (draw_x + 2.5), draw_y, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);

			if (draw_y + 30.0 > 180.0)
			{
				draw_x += 50.0;
				draw_y = 120.0;
			}
			else
				draw_y += 30.0;
		}
	}
	else if (cam_selected_menu_mode == CAM_MENU_ADVANCED_0)
	{
		Draw_texture(Square_image, weak_aqua_tint, 0, 160.0, 110.0, 60.0, 10.0);
		
		Draw(cam_msg[7], 0, 42.5, 120.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw_texture(Square_image, weak_red_tint, 0, 40.0, 137.5, 100.0, 5.0);
		Draw_texture(Square_image, white_or_black_tint, 0, (40.0 + 9.5 * cam_request_capture_contrast_mode), 130.0, 5.0, 20.0);

		Draw(cam_msg[8], 0, 42.5, 150.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw_texture(Square_image, weak_red_tint, 0, 40.0, 167.5, 100.0, 5.0);
		Draw_texture(Square_image, white_or_black_tint, 0, (40.0 + 19.0 * cam_request_capture_white_balance_mode), 160.0, 5.0, 20.0);

		Draw(cam_msg[9], 0, 182.5, 120.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw_texture(Square_image, weak_red_tint, 0, 180.0, 137.5, 100.0, 5.0);
		Draw_texture(Square_image, white_or_black_tint, 0, (180.0 + 47.5 * cam_request_capture_lens_correction_mode), 130.0, 5.0, 20.0);

		Draw(cam_msg[10], 0, 182.5, 150.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw_texture(Square_image, weak_red_tint, 0, 180.0, 167.5, 100.0, 5.0);
		Draw_texture(Square_image, white_or_black_tint, 0, (180.0 + 19.0 * cam_request_capture_exposure_mode), 160.0, 5.0, 20.0);
	}
	else if (cam_selected_menu_mode == CAM_MENU_ADVANCED_1)
	{
		Draw_texture(Square_image, red_tint, 0, 220.0, 110.0, 60.0, 10.0);

		Draw(cam_msg[11], 0, 42.5, 120.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw_texture(Square_image, weak_aqua_tint, 0, 40.0, 130.0, 40.0, 20.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 100.0, 130.0, 40.0, 20.0);
		if (cam_encode_format_mode == 0)
		{
			Draw(cam_msg[12], 0, 42.5, 130.0, 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
			Draw(cam_msg[13], 0, 102.5, 130.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		}
		else if(cam_encode_format_mode == 1)
		{
			Draw(cam_msg[12], 0, 42.5, 130.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw(cam_msg[13], 0, 102.5, 130.0, 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
			Draw(cam_msg[14] + std::to_string(cam_selected_jpg_quality), 0, 42.5, 150.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw_texture(Square_image, weak_red_tint, 0, 41.0, 167.5, 100.0, 5.0);
			Draw_texture(Square_image, white_or_black_tint, 0, (40.0 + 1.0 * cam_selected_jpg_quality), 160.0, 5.0, 20.0);
		}

		Draw(cam_msg[15], 0, 182.5, 120.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw_texture(Square_image, weak_aqua_tint, 0, 180.0, 130.0, 40.0, 20.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 240.0, 130.0, 40.0, 20.0);
		if (cam_request_capture_noise_filter_mode == 1)
		{
			Draw(cam_msg[16], 0, 182.5, 130.0, 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
			Draw(cam_msg[17], 0, 242.5, 130.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		}
		else
		{
			Draw(cam_msg[16], 0, 182.5, 130.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw(cam_msg[17], 0, 242.5, 130.0, 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
		}

		Draw(cam_msg[18], 0, 182.5, 150.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw_texture(Square_image, weak_aqua_tint, 0, 180.0, 160.0, 40.0, 20.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 240.0, 160.0, 40.0, 20.0);
		if (cam_shutter_sound_mode == 1)
		{
			Draw(cam_msg[16], 0, 182.5, 160.0, 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
			Draw(cam_msg[17], 0, 242.5, 160.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		}
		else
		{
			Draw(cam_msg[16], 0, 182.5, 160.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw(cam_msg[17], 0, 242.5, 160.0, 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
		}

		//Draw(cam_msg[19] + std::to_string(cam_selected_num_of_encode_threads), 0, 20.0, 200.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
	}
	for (int i = 0; i < 4; i++)
		Draw(cam_msg[i + 23], 0, 40.0 + (i * 60.0), 110.0, 0.35, 0.35, text_red, text_green, text_blue, text_alpha);

	if (cam_encode_request[0] || cam_encode_request[1] || cam_encode_request[2])
	{
		Draw_texture(Square_image, aqua_tint, 0, 40.0, 80.0, 240.0, 110.0);

		if(s_setting[0] == "en")
			Draw(cam_msg[20] + cam_format_name_list[cam_encode_format_mode] + cam_msg[21], 0, 52.5, 80.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		else if (s_setting[0] == "jp")
			Draw(cam_format_name_list[cam_encode_format_mode] + cam_msg[20] + cam_msg[21], 0, 52.5, 80.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);


//		for (int i = 0; i < 3; i++)
		for (int i = 0; i < 1; i++)
		{
			Draw(cam_msg[22] + std::to_string(i), 0, 52.5, 90.0 + (i * 30.0), 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw_texture(Square_image, white_tint, 0, 50.0, 100.0 + (i * 30.0), 220.0, 20.0);
			if(cam_encode_thread_num_of_jobs[i] == 0)
				Draw_texture(Square_image, red_tint, 0, 50.0, 100.0 + (i * 30.0), 0.0, 20.0);
			else
				Draw_texture(Square_image, red_tint, 0, 50.0, 100.0 + (i * 30.0), 220.0 * ((double)cam_encode_thread_progress[i] / (double)cam_encode_thread_num_of_jobs[i]), 20.0);
		}
	}

	if (Err_query_error_show_flag())
		Draw_error();

	Draw_bot_ui();
	if (Hid_query_key_held_state(KEY_H_TOUCH))
		Draw(s_circle_string, 0, Hid_query_touch_pos(true), Hid_query_touch_pos(false), 0.20f, 0.20f, 1.0f, 0.0f, 0.0f, 1.0f);
	s_fps += 1;

	Draw_apply_draw();
	osTickCounterUpdate(&s_tcount_frame_time);
	s_frame_time = osTickCounterRead(&s_tcount_frame_time);
}
