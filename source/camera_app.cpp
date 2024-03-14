#include "definitions.hpp"
#include "system/types.hpp"

#include "system/menu.hpp"
#include "system/variables.hpp"

#include "system/draw/draw.hpp"

#include "system/util/camera.hpp"
#include "system/util/converter.hpp"
#include "system/util/encoder.hpp"
#include "system/util/error.hpp"
#include "system/util/file.hpp"
#include "system/util/hid.hpp"
#include "system/util/log.hpp"
#include "system/util/queue.hpp"
#include "system/util/util.hpp"

extern "C"
{
	#include "system/util/qr.h"
}

//Include myself.
#include "camera_app.hpp"


typedef enum
{
	CAM_COMMAND_INVALID = -1,

	CAM_COMMAND_MAIN_THREAD_SCAN_QR_SUCCESS_INDICATION,	//Indication that indicates QR code scan was successful.
	CAM_COMMAND_SCAN_QR_THREAD_SCAN_QR_CODE_REQUEST,	//Request to scan and decode a QR code.

	CAM_COMMAND_MAX,
	CAM_COMMAND_FORCE_32BIT = INT32_MAX,
} Cam_command;

typedef struct
{
	char* decoded_data;	//Decoded data.
} Cam_scan_qr_success_indication_data;

typedef struct
{
	uint8_t* buffer;	//Raw picture (PIXEL_FORMAT_RGB565LE) buffer.
	uint16_t width;		//Picture Width in px.
	uint16_t height;	//Picture height in px.
} Cam_scan_qr_code_request_data;


extern "C" void memcpy_asm(u8*, u8*, int);


bool cam_main_run = false;
bool cam_already_init = false;
bool cam_thread_run = false;
bool cam_thread_suspend = false;
bool cam_take_request = false;
bool cam_encode_request = false;
bool cam_change_resolution_request = false;
bool cam_change_fps_request = false;
bool cam_change_contrast_request = false;
bool cam_change_white_balance_request = false;
bool cam_change_lens_correction_request = false;
bool cam_change_camera_request = false;
bool cam_change_exposure_request = false;
bool cam_change_noise_filter_request = false;
bool cam_scan_qr_mode = false;
u8* cam_encode_buffer = NULL;
int cam_selected_menu_mode = 0;
int cam_selected_jpg_quality = 95;
int cam_display_img_num = -1;
Camera_resolution cam_resolution_mode = CAM_RES_400x240;
Camera_resolution cam_request_resolution_mode = CAM_RES_400x240;
Camera_exposure cam_exposure_mode = CAM_EXPOSURE_0;
Camera_exposure cam_request_exposure_mode = CAM_EXPOSURE_0;
int cam_noise_filter_mode = 1;
int cam_request_noise_filter_mode = 1;
Camera_framerate cam_fps_mode = CAM_FPS_30;
Camera_framerate cam_request_fps_mode = CAM_FPS_30;
Camera_contrast cam_contrast_mode = CAM_CONTRAST_06;
Camera_contrast cam_request_contrast_mode = CAM_CONTRAST_06;
Camera_white_balance cam_white_balance_mode = CAM_WHITE_BALANCE_AUTO;
Camera_white_balance cam_request_white_balance_mode = CAM_WHITE_BALANCE_AUTO;
Camera_lens_correction cam_lens_correction_mode = CAM_LENS_CORRECTION_OFF;
Camera_lens_correction cam_request_lens_correction_mode = CAM_LENS_CORRECTION_OFF;
Camera_port cam_camera_mode = CAM_PORT_OUT_LEFT;
Camera_port cam_request_camera_mode = CAM_PORT_OUT_LEFT;
int cam_shutter_sound_mode = 0;
int cam_encode_format_mode = 1;
int cam_width_list[8] = { 640, 512, 400, 352, 320, 256, 176, 160, };
int cam_height_list[8] = { 480, 384, 240, 288, 240, 192, 144, 120, };
Camera_framerate cam_fps_list[13] = { CAM_FPS_15, CAM_FPS_15_TO_5, CAM_FPS_15_TO_2,
CAM_FPS_10, CAM_FPS_8_5, CAM_FPS_5, CAM_FPS_20, CAM_FPS_20_TO_5,
CAM_FPS_30, CAM_FPS_30_TO_5, CAM_FPS_15_TO_10, CAM_FPS_20_TO_10, CAM_FPS_30_TO_10, };
Camera_port cam_camera_list[3] = { CAM_PORT_OUT_LEFT, CAM_PORT_OUT_RIGHT, CAM_PORT_IN, };
void (*cam_qr_callback)(char*) = NULL;
std::string cam_status = "";
std::string cam_msg[DEF_CAM_NUM_OF_MSG];
std::string cam_resolution_list[9];
std::string cam_exposure_list[6];
std::string cam_lens_correction_list[3];
std::string cam_white_balance_list[6];
std::string cam_contrast_list[11];
std::string cam_framelate_list[15];
Thread cam_init_thread, cam_exit_thread, cam_capture_thread, cam_encode_thread, cam_scan_qr_thread;
Image_data cam_capture_image[2], cam_menu_button[4], cam_resolution_button[9], cam_camera_button[3], cam_fps_button[15], cam_contrast_bar,
cam_white_balance_bar, cam_lens_correction_bar, cam_exposure_bar, cam_jpg_quality_bar, cam_png_button, cam_jpg_button, cam_noise_filter_on_button,
cam_noise_filter_off_button, cam_shutter_sound_on_button, cam_shutter_sound_off_button;
Queue cam_scan_qr_thread_queue = { 0, };
Queue cam_main_thread_queue = { 0, };


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
	var_need_reflesh = true;
	Menu_suspend();
}

void Cam_suspend(void)
{
	cam_thread_suspend = true;
	cam_main_run = false;
	var_need_reflesh = true;
	Menu_resume();
}

void Cam_hid(Hid_info key)
{
	int random_num = 0;
	int selected_num = 0;

	if(Util_err_query_error_show_flag())
		Util_err_main(key);
	else
	{
		if(Util_hid_is_pressed(key, *Draw_get_bot_ui_button()))
			Draw_get_bot_ui_button()->selected = true;
		else if (key.p_start || (Util_hid_is_released(key, *Draw_get_bot_ui_button()) && Draw_get_bot_ui_button()->selected))
			Cam_suspend();
		else if (!cam_encode_request)
		{
			if (key.p_a)
				cam_take_request = true;
			for (int i = 0; i < 3; i++)
			{
				if(Util_hid_is_pressed(key, cam_camera_button[i]))
					cam_camera_button[i].selected = true;
				else if(Util_hid_is_released(key, cam_camera_button[i]) && cam_camera_button[i].selected)
				{
					if(cam_request_camera_mode != cam_camera_list[i])
					{
						cam_request_camera_mode = cam_camera_list[i];
						cam_change_camera_request = true;
					}
					break;
				}
			}
			for (int i = 0; i < 4; i++)
			{
				if(Util_hid_is_pressed(key, cam_menu_button[i]))
					cam_menu_button[i].selected = true;
				else if(Util_hid_is_released(key, cam_menu_button[i]) && cam_menu_button[i].selected)
				{
					cam_selected_menu_mode = i;
					break;
				}
			}

			if (cam_selected_menu_mode == DEF_CAM_MENU_RESOLUTION)
			{
				for (int i = 0; i < 9; i++)
				{
					if(Util_hid_is_pressed(key, cam_resolution_button[i]))
						cam_resolution_button[i].selected = true;
					else if(Util_hid_is_released(key, cam_resolution_button[i]) && cam_resolution_button[i].selected && i != cam_request_resolution_mode)
					{
						//Random.
						if (i == 8)
						{
							srand(time(NULL));
							i = rand() % 8;
						}

						cam_request_resolution_mode = (Camera_resolution)i;
						cam_change_resolution_request = true;
						break;
					}
				}
			}
			else if (cam_selected_menu_mode == DEF_CAM_MENU_FPS)
			{
				for (int i = 0; i < 15; i++)
				{
					if(Util_hid_is_pressed(key, cam_fps_button[i]))
						cam_fps_button[i].selected = true;
					else if(Util_hid_is_released(key, cam_fps_button[i]) && cam_fps_button[i].selected && i != cam_request_fps_mode)
					{
						if (i == 13 || i == 14)
						{
							srand(time(NULL));
							if (i == 13)
							{
								random_num = 13;
								while (random_num == 13 || random_num == 0 || random_num == 3 || random_num == 4 || random_num == 5
									|| random_num == 6 || random_num == 8)
								{
									random_num = rand() % 13;
									Util_sleep(10000);
								}
							}
							else if(i == 14)
							{
								random_num = 14;
								while (random_num == 14 || random_num == 1 || random_num == 2 || random_num == 7 || random_num == 9
									|| random_num == 10 || random_num == 11 || random_num == 12)
								{
									random_num = rand() % 13;
									Util_sleep(10000);
								}
							}
							i = random_num;
						}
						cam_request_fps_mode = cam_fps_list[i];
						cam_change_fps_request = true;
						break;
					}
				}
			}
			else if (cam_selected_menu_mode == DEF_CAM_MENU_ADVANCED_0)
			{
				if (Util_hid_is_pressed(key, cam_contrast_bar))
					cam_contrast_bar.selected = true;
				else if(cam_contrast_bar.selected && key.h_touch)
				{
					if(key.touch_x < 40)
						selected_num = 0;
					else if(key.touch_x > 140)
						selected_num = 10;
					else
						selected_num = (key.touch_x - 40) / 9.1;

					if(selected_num != cam_request_contrast_mode)
					{
						cam_request_contrast_mode = (Camera_contrast)selected_num;
						cam_change_contrast_request = true;
					}
				}
				else if (Util_hid_is_pressed(key, cam_white_balance_bar))
					cam_white_balance_bar.selected = true;
				else if(cam_white_balance_bar.selected && key.h_touch)
				{
					if(key.touch_x < 40)
						selected_num = 0;
					else if(key.touch_x > 140)
						selected_num = 5;
					else
						selected_num = (key.touch_x - 40) / 16.6;

					if(selected_num != cam_request_white_balance_mode)
					{
						cam_request_white_balance_mode = (Camera_white_balance)selected_num;
						cam_change_white_balance_request = true;
					}
				}
				else if (Util_hid_is_pressed(key, cam_lens_correction_bar))
					cam_lens_correction_bar.selected = true;
				else if(cam_lens_correction_bar.selected && key.h_touch)
				{
					if(key.touch_x < 180)
						selected_num = 0;
					else if(key.touch_x > 280)
						selected_num = 2;
					else
						selected_num = (key.touch_x - 180) / 33.3;

					if(selected_num != cam_request_lens_correction_mode)
					{
						cam_request_lens_correction_mode = (Camera_lens_correction)selected_num;
						cam_change_lens_correction_request = true;
					}
				}
				else if (Util_hid_is_pressed(key, cam_exposure_bar))
					cam_exposure_bar.selected = true;
				else if(cam_exposure_bar.selected && key.h_touch)
				{
					if(key.touch_x < 180)
						selected_num = 0;
					else if(key.touch_x > 280)
						selected_num = 5;
					else
						selected_num = (key.touch_x - 180) / 16.6;

					if(selected_num != cam_request_exposure_mode)
					{
						cam_request_exposure_mode = (Camera_exposure)selected_num;
						cam_change_exposure_request = true;
					}
				}
			}
			else if (cam_selected_menu_mode == DEF_CAM_MENU_ADVANCED_1)
			{
				if(Util_hid_is_pressed(key, cam_png_button))
					cam_png_button.selected = true;
				else if(Util_hid_is_released(key, cam_png_button) && cam_png_button.selected)
					cam_encode_format_mode = 0;
				else if(Util_hid_is_pressed(key, cam_jpg_button))
					cam_jpg_button.selected = true;
				else if(Util_hid_is_released(key, cam_jpg_button) && cam_jpg_button.selected)
					cam_encode_format_mode = 1;
				else if(Util_hid_is_pressed(key, cam_noise_filter_on_button))
					cam_noise_filter_on_button.selected = true;
				else if(Util_hid_is_released(key, cam_noise_filter_on_button) && cam_noise_filter_on_button.selected)
				{
					cam_request_noise_filter_mode = 1;
					cam_change_noise_filter_request = true;
				}
				else if(Util_hid_is_pressed(key, cam_noise_filter_off_button))
					cam_noise_filter_off_button.selected = true;
				else if(Util_hid_is_released(key, cam_noise_filter_off_button) && cam_noise_filter_off_button.selected)
				{
					cam_request_noise_filter_mode = 0;
					cam_change_noise_filter_request = true;
				}
				else if(Util_hid_is_pressed(key, cam_shutter_sound_on_button))
					cam_shutter_sound_on_button.selected = true;
				else if(Util_hid_is_released(key, cam_shutter_sound_on_button) && cam_shutter_sound_on_button.selected)
					cam_shutter_sound_mode = 1;
				else if(Util_hid_is_pressed(key, cam_shutter_sound_off_button))
					cam_shutter_sound_off_button.selected = true;
				else if(Util_hid_is_released(key, cam_shutter_sound_off_button) && cam_shutter_sound_off_button.selected)
					cam_shutter_sound_mode = 0;

				if (cam_encode_format_mode == 1)
				{
					if (Util_hid_is_pressed(key, cam_jpg_quality_bar))
						cam_jpg_quality_bar.selected = true;
					else if(cam_jpg_quality_bar.selected && key.h_touch)
					{
						if(key.touch_x < 40)
							selected_num = 1;
						else if(key.touch_x > 140)
							selected_num = 100;
						else
							selected_num = (key.touch_x - 40) + 1;

						cam_selected_jpg_quality = selected_num;
					}
				}
			}
		}

	}

	if(!key.p_touch && !key.h_touch)
	{
		Draw_get_bot_ui_button()->selected = false;
		cam_contrast_bar.selected = false;
		cam_white_balance_bar.selected = false;
		cam_lens_correction_bar.selected = false;
		cam_exposure_bar.selected = false;
		cam_jpg_quality_bar.selected = false;
		cam_png_button.selected = false;
		cam_jpg_button.selected = false;
		cam_noise_filter_on_button.selected = false;
		cam_noise_filter_off_button.selected = false;
		cam_shutter_sound_on_button.selected = false;
		cam_shutter_sound_off_button.selected = false;
		for (int i = 0; i < 4; i++)
			cam_menu_button[i].selected = false;
		for (int i = 0; i < 9; i++)
			cam_resolution_button[i].selected = false;
		for (int i = 0; i < 3; i++)
			cam_camera_button[i].selected = false;
		for (int i = 0; i < 15; i++)
			cam_fps_button[i].selected = false;
	}

	if(Util_log_query_log_show_flag())
		Util_log_main(key);
}

Result_with_string Cam_load_msg(std::string lang)
{
	return Util_load_msg("cam_" + lang + ".txt", cam_msg, DEF_CAM_NUM_OF_MSG);
}

void Cam_set_qr_scan_mode(bool enable)
{
	if(!cam_already_init)
		return;

	cam_scan_qr_mode = enable;
}

void Cam_set_qr_scan_callback(void (*callback)(char*))
{
	if(!cam_already_init)
		return;

	cam_qr_callback = callback;
}

void Cam_encode_thread(void* arg)
{
	Util_log_save(DEF_CAM_ENCODE_THREAD_STR, "Thread started.");
	int log_num = 0;
	u8 dummy = 0;
	char date[128];
	std::string file_name = "";
	std::string extension = "";
	std::string dir_path = "";
	std::string cache_dir_path = "";
	TickCounter stopwatch;
	Result_with_string result;
	osTickCounterStart(&stopwatch);

	//create directory
	Util_file_save_to_file(".", DEF_MAIN_DIR + "dcim/", &dummy, 1, true);
	Util_file_save_to_file(".", DEF_MAIN_DIR + "dcim/jpg/", &dummy, 1, true);
	Util_file_save_to_file(".", DEF_MAIN_DIR + "dcim/png/", &dummy, 1, true);

	while (cam_thread_run)
	{
		if (cam_encode_request)
		{
			Color_converter_parameters converter_parameters;

			memset(date, 0, 128);
			sprintf(date, "%02d_%02d_%02d_", var_hours, var_minutes, var_seconds);
			file_name = date;

			memset(date, 0, 128);
			sprintf(date, "%04d_%02d_%02d", var_years, var_months, var_days);
			if(cam_encode_format_mode == 0)
			{
				extension = ".png";
				dir_path = DEF_MAIN_DIR + (std::string)"dcim/png/" + date + "/";
			}
			else if(cam_encode_format_mode == 1)
			{
				extension = ".jpg";
				dir_path = DEF_MAIN_DIR + (std::string)"dcim/jpg/" + date + "/";
			}
			//create directory
			Util_file_save_to_file(".", dir_path, &dummy, 1, true);

			for(int i = 0; i < 10; i++)
			{
				result = Util_file_check_file_exist(file_name + std::to_string(i) + extension, dir_path);
				if(result.code != 0)
				{
					file_name = file_name + std::to_string(i) + extension;
					break;
				}
			}

			converter_parameters.source = cam_encode_buffer;
			converter_parameters.in_color_format = PIXEL_FORMAT_RGB565LE;
			converter_parameters.in_width = cam_width_list[cam_resolution_mode];
			converter_parameters.in_height = cam_height_list[cam_resolution_mode];
			converter_parameters.converted = NULL;
			converter_parameters.out_color_format = PIXEL_FORMAT_RGB888;
			converter_parameters.out_width = converter_parameters.in_width;
			converter_parameters.out_height = converter_parameters.in_height;

			log_num = Util_log_save(DEF_CAM_ENCODE_THREAD_STR, "Util_converter_convert_color()...");
			result = Util_converter_convert_color(&converter_parameters);
			Util_log_add(log_num, result.string + result.error_description, result.code);
			if(result.code == 0)
			{
				log_num = Util_log_save(DEF_CAM_ENCODE_THREAD_STR, "Util_image_encoder_encode()...");
				result = Util_image_encoder_encode(dir_path + file_name, converter_parameters.converted, cam_width_list[cam_resolution_mode], cam_height_list[cam_resolution_mode],
				cam_encode_format_mode == 0 ? IMAGE_CODEC_PNG : IMAGE_CODEC_JPG, cam_encode_format_mode == 0 ? 0 : cam_selected_jpg_quality);
				Util_log_add(log_num, result.string + result.error_description, result.code);
			}

			free(converter_parameters.converted);
			converter_parameters.converted = NULL;

			if(result.code != 0)
			{
				Util_err_set_error_message(result.string, result.error_description, DEF_CAM_ENCODE_THREAD_STR, result.code);
				Util_err_set_error_show_flag(true);
			}

			cam_encode_request = false;
		}
		else
			Util_sleep(DEF_ACTIVE_THREAD_SLEEP_TIME);

		while (cam_thread_suspend)
			Util_sleep(DEF_INACTIVE_THREAD_SLEEP_TIME);
	}
	Util_log_save(DEF_CAM_ENCODE_THREAD_STR, "Thread exit.");
	threadExit(0);
}

void Cam_capture_thread(void* arg)
{
	Util_log_save(DEF_CAM_CAPTURE_THREAD_STR, "Thread started.");
	u8* capture_buffer = NULL;
	int width = 0;
	int height = 0;
	int log_num = 0;
	int buffer_num = 0;
	Result_with_string result;

	while (cam_thread_run)
	{
		if (cam_take_request && !cam_encode_request)
		{
			log_num = Util_log_save(DEF_CAM_CAPTURE_THREAD_STR , "Util_cam_take_a_picture()...");
			result = Util_cam_take_a_picture(&capture_buffer, &width, &height, cam_shutter_sound_mode);
			Util_log_add(log_num, result.string, result.code);

			if(result.code == 0)
			{
				memcpy_asm(cam_encode_buffer, capture_buffer, width * height * 2);
				cam_encode_request = true;
			}
			else
			{
				Util_err_set_error_message(result.string, result.error_description, DEF_CAM_CAPTURE_THREAD_STR, result.code);
				Util_err_set_error_show_flag(true);
			}
			free(capture_buffer);
			capture_buffer = NULL;

			cam_take_request = false;
		}
		else if (cam_change_resolution_request || cam_change_fps_request || cam_change_contrast_request || cam_change_white_balance_request
			|| cam_change_lens_correction_request || cam_change_camera_request || cam_change_exposure_request || cam_change_noise_filter_request)
		{
			if (cam_change_resolution_request)
			{
				log_num = Util_log_save(DEF_CAM_CAPTURE_THREAD_STR, "Util_cam_set_resolution()...");
				result = Util_cam_set_resolution(cam_request_resolution_mode);
			}
			else if (cam_change_fps_request)
			{
				log_num = Util_log_save(DEF_CAM_CAPTURE_THREAD_STR, "Util_cam_set_fps()...");
				result = Util_cam_set_fps(cam_request_fps_mode);
			}
			else if (cam_change_contrast_request)
			{
				log_num = Util_log_save(DEF_CAM_CAPTURE_THREAD_STR, "Util_cam_set_contrast()...");
				result = Util_cam_set_contrast(cam_request_contrast_mode);
			}
			else if (cam_change_white_balance_request)
			{
				log_num = Util_log_save(DEF_CAM_CAPTURE_THREAD_STR, "Util_cam_set_white_balance()...");
				result = Util_cam_set_white_balance(cam_request_white_balance_mode);
			}
			else if (cam_change_lens_correction_request)
			{
				log_num = Util_log_save(DEF_CAM_CAPTURE_THREAD_STR, "Util_cam_set_lens_correction()...");
				result = Util_cam_set_lens_correction(cam_request_lens_correction_mode);
			}
			else if (cam_change_camera_request)
			{
				log_num = Util_log_save(DEF_CAM_CAPTURE_THREAD_STR, "Util_cam_set_camera()...");
				result = Util_cam_set_camera(cam_request_camera_mode);
			}
			else if (cam_change_exposure_request)
			{
				log_num = Util_log_save(DEF_CAM_CAPTURE_THREAD_STR, "Util_cam_set_exposure()...");
				result = Util_cam_set_exposure(cam_request_exposure_mode);
			}
			else if (cam_change_noise_filter_request)
			{
				log_num = Util_log_save(DEF_CAM_CAPTURE_THREAD_STR, "Util_cam_set_noise_filter()...");
				result = Util_cam_set_noise_filter(cam_request_noise_filter_mode);
			}
			else
				log_num = 0;

			Util_log_add(log_num, result.string, result.code);

			if (result.code != 0)
			{
				cam_request_resolution_mode = cam_resolution_mode;
				Util_err_set_error_message(result.string, result.error_description, DEF_CAM_CAPTURE_THREAD_STR, result.code);
				Util_err_set_error_show_flag(true);
			}
			else
			{
				if (cam_change_resolution_request)
				{
					cam_display_img_num = -1;
					cam_resolution_mode = cam_request_resolution_mode;
				}
				else if (cam_change_fps_request)
					cam_fps_mode = cam_request_fps_mode;
				else if (cam_change_contrast_request)
					cam_contrast_mode = cam_request_contrast_mode;
				else if (cam_change_white_balance_request)
					cam_white_balance_mode = cam_request_white_balance_mode;
				else if (cam_change_lens_correction_request)
					cam_lens_correction_mode = cam_request_lens_correction_mode;
				else if (cam_change_camera_request)
					cam_camera_mode = cam_request_camera_mode;
				else if (cam_change_exposure_request)
					cam_exposure_mode = cam_request_exposure_mode;
				else if (cam_change_noise_filter_request)
					cam_noise_filter_mode = cam_request_noise_filter_mode;
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
		else
		{
			result = Util_cam_take_a_picture(&capture_buffer, &width, &height, false);
			if (result.code == 0)
			{
				if (cam_display_img_num == 0)
					buffer_num = 1;
				else
					buffer_num = 0;

				result = Draw_set_texture_data(&cam_capture_image[buffer_num], capture_buffer, width, height);

				if (result.code == 0)
				{
					cam_display_img_num = buffer_num;
					var_need_reflesh = true;
				}
				else
				{
					Util_err_set_error_message(result.string, result.error_description, DEF_CAM_CAPTURE_THREAD_STR, result.code);
					Util_err_set_error_show_flag(true);
					Util_sleep(1000000);
				}
			}
			else
			{
				Util_log_save(DEF_CAM_CAPTURE_THREAD_STR, result.string, result.code);
				Util_sleep(1000);
			}

			if (result.code == DEF_SUCCESS && cam_scan_qr_mode
			&& !Util_queue_check_event_exist(&cam_scan_qr_thread_queue, CAM_COMMAND_SCAN_QR_THREAD_SCAN_QR_CODE_REQUEST))
			{
				Cam_scan_qr_code_request_data* data = (Cam_scan_qr_code_request_data*)malloc(sizeof(Cam_scan_qr_code_request_data));

				if(data)
				{
					data->buffer = capture_buffer;
					data->width = width;
					data->height = height;

					Util_queue_add(&cam_scan_qr_thread_queue, CAM_COMMAND_SCAN_QR_THREAD_SCAN_QR_CODE_REQUEST, data, 0, QUEUE_OPTION_NONE);

					//capture_buffer will be freed in Cam_scan_qr_thread().
					capture_buffer = NULL;
				}
				else
				{
					free(capture_buffer);
					capture_buffer = NULL;
				}
			}
			else
			{
				free(capture_buffer);
				capture_buffer = NULL;
			}
		}

		while (cam_thread_suspend)
			Util_sleep(DEF_INACTIVE_THREAD_SLEEP_TIME);
	}

	result.code = CAMU_Activate(SELECT_NONE);

	Util_log_save(DEF_CAM_CAPTURE_THREAD_STR, "Thread exit.");
	threadExit(0);
}

void Cam_scan_qr_thread(void* arg)
{
	Util_log_save(DEF_CAM_SCAN_QR_THREAD_STR, "Thread started.");
	Result_with_string result;

	while (cam_thread_run)
	{
		void* command_data = NULL;
		Cam_command command_id = CAM_COMMAND_INVALID;

		result = Util_queue_get(&cam_scan_qr_thread_queue, (u32*)&command_id, &command_data, DEF_ACTIVE_THREAD_SLEEP_TIME);
		if(result.code == DEF_SUCCESS)
		{
			switch (command_id)
			{
				case CAM_COMMAND_SCAN_QR_THREAD_SCAN_QR_CODE_REQUEST:
				{
					Color_converter_parameters parameters;
					Cam_scan_qr_code_request_data* data = (Cam_scan_qr_code_request_data*)command_data;

					if(!command_data)
						break;

					if(!cam_scan_qr_mode)
					{
						free(data->buffer);
						data->buffer = NULL;
						break;
					}

					parameters.source = data->buffer;
					parameters.converted = NULL;
					parameters.in_width = data->width;
					parameters.in_height = data->height;
					parameters.out_width = data->width;
					parameters.out_height = data->height;
					parameters.in_color_format = PIXEL_FORMAT_RGB565LE;
					parameters.out_color_format = PIXEL_FORMAT_GRAY8;

					result = Util_converter_convert_color(&parameters);
					free(data->buffer);
					data->buffer = NULL;

					if(result.code == DEF_SUCCESS)
					{
						Util_qr_decode_parameters qr_parameters = { 0, };

						Util_qr_init_decode_parameters(&qr_parameters);

						qr_parameters.input = parameters.converted;
						qr_parameters.width = parameters.out_width;
						qr_parameters.height = parameters.out_height;

						result.code = Util_qr_decode(&qr_parameters);
						if(result.code == DEF_SUCCESS)
						{
							Cam_scan_qr_success_indication_data* indication_data = (Cam_scan_qr_success_indication_data*)malloc(sizeof(Cam_scan_qr_success_indication_data));

							Util_log_save(DEF_CAM_SCAN_QR_THREAD_STR, qr_parameters.decoded);

							//If QR code has been decoded successfully, stop QR code scan.
							cam_scan_qr_mode = false;

							if(indication_data)
							{
								indication_data->decoded_data = qr_parameters.decoded;

								Util_queue_add(&cam_main_thread_queue, CAM_COMMAND_MAIN_THREAD_SCAN_QR_SUCCESS_INDICATION, indication_data, 0, QUEUE_OPTION_NONE);

								//decoded will be freed in Cam_main().
								qr_parameters.decoded = NULL;
							}
							else
							{
								free(qr_parameters.decoded);
								qr_parameters.decoded = NULL;
							}
						}
						else
						{
							free(qr_parameters.decoded);
							qr_parameters.decoded = NULL;

							if(result.code != DEF_ERR_QUIRC_RETURNED_NOT_SUCCESS)
								Util_log_save(DEF_CAM_SCAN_QR_THREAD_STR, (std::string)"Util_qr_decode()..." + qr_parameters.error_message, result.code);
						}
					}
					else
						Util_log_save(DEF_CAM_SCAN_QR_THREAD_STR, "Util_converter_convert_color()..." + result.string + result.error_description, result.code);

					free(parameters.converted);
					parameters.converted = NULL;

					break;
				}

				default:
				{
					//Invalid command was received.
					char msg[64];
					snprintf(msg, sizeof(msg), "Invalid command (%08X) was received!!!!!", command_id);
					Util_log_save(DEF_CAM_SCAN_QR_THREAD_STR, msg);
					break;
				}
			}
		}

		free(command_data);
		command_data = NULL;

		while (cam_thread_suspend)
			Util_sleep(DEF_INACTIVE_THREAD_SLEEP_TIME);
	}

	Util_log_save(DEF_CAM_SCAN_QR_THREAD_STR, "Thread exit.");
	threadExit(0);
}

void Cam_init_thread(void* arg)
{
	Util_log_save(DEF_CAM_INIT_STR, "Thread started.");
	int log_num = 0;
	u8* fs_buffer = NULL;
	std::string data[11];
	std::string option_cache[DEF_CAM_NUM_OF_OPTION_MSG];
	Result_with_string result;

	cam_status = "Loading settings...";
	log_num = Util_log_save(DEF_CAM_INIT_STR, "Util_file_load_from_file()...");
	result = Util_file_load_from_file("Cam_setting.txt", DEF_MAIN_DIR, &fs_buffer, 0x2000);
	Util_log_add(log_num, result.string, result.code);
	if (result.code == 0)
	{
		result = Util_parse_file((char*)fs_buffer, 11, data);
		if(result.code == 0)
		{
			cam_camera_mode = (Camera_port)atoi(data[0].c_str());
			cam_resolution_mode = (Camera_resolution)atoi(data[1].c_str());
			cam_fps_mode = (Camera_framerate)atoi(data[2].c_str());
			cam_contrast_mode = (Camera_contrast)atoi(data[3].c_str());
			cam_white_balance_mode = (Camera_white_balance)atoi(data[4].c_str());
			cam_lens_correction_mode = (Camera_lens_correction)atoi(data[5].c_str());
			cam_exposure_mode = (Camera_exposure)atoi(data[6].c_str());
			cam_encode_format_mode= atoi(data[7].c_str());
			cam_noise_filter_mode = atoi(data[8].c_str());
			cam_selected_jpg_quality = atoi(data[9].c_str());
			cam_shutter_sound_mode = atoi(data[10].c_str());
		}
	}
	free(fs_buffer);
	fs_buffer = NULL;

	//todo
	if(cam_camera_mode <= CAM_PORT_INVALID || cam_camera_mode >= CAM_PORT_MAX)
		cam_camera_mode = CAM_PORT_OUT_LEFT;
	if(cam_resolution_mode <= CAM_RES_INVALID || cam_resolution_mode >= CAM_RES_MAX)
		cam_resolution_mode = CAM_RES_400x240;
	if(cam_fps_mode <= CAM_FPS_INVALID || cam_fps_mode >= CAM_FPS_MAX)
		cam_fps_mode = CAM_FPS_30;
	if(cam_contrast_mode <= CAM_CONTRAST_INVALID || cam_contrast_mode >= CAM_CONTRAST_MAX)
		cam_contrast_mode = CAM_CONTRAST_06;
	if(cam_white_balance_mode <= CAM_WHITE_BALANCE_INVALID || cam_white_balance_mode >= CAM_WHITE_BALANCE_MAX)
		cam_white_balance_mode = CAM_WHITE_BALANCE_AUTO;
	if(cam_lens_correction_mode <= CAM_LENS_CORRECTION_INVALID || cam_lens_correction_mode >= CAM_LENS_CORRECTION_MAX)
		cam_lens_correction_mode = CAM_LENS_CORRECTION_OFF;
	if(cam_exposure_mode <= CAM_EXPOSURE_INVALID || cam_exposure_mode >= CAM_EXPOSURE_MAX)
		cam_exposure_mode = CAM_EXPOSURE_0;
	if(!(cam_encode_format_mode >= 0 && cam_encode_format_mode <= 1))
		cam_encode_format_mode = 1;
	if(!(cam_noise_filter_mode >= 0 && cam_noise_filter_mode <= 1))
		cam_noise_filter_mode = 1;
	if(!(cam_selected_jpg_quality >= 1 && cam_selected_jpg_quality <= 100))
		cam_selected_jpg_quality = 95;
	if(!(cam_shutter_sound_mode >= 0 && cam_shutter_sound_mode <= 1))
		cam_shutter_sound_mode = 0;

	cam_request_camera_mode = cam_camera_mode;
	cam_request_resolution_mode = cam_resolution_mode;
	cam_request_fps_mode = cam_fps_mode;
	cam_request_contrast_mode = cam_contrast_mode;
	cam_request_white_balance_mode = cam_white_balance_mode;
	cam_request_lens_correction_mode = cam_lens_correction_mode;
	cam_request_exposure_mode = cam_exposure_mode;

	Util_load_msg("cam_options.txt", option_cache, DEF_CAM_NUM_OF_OPTION_MSG);
	for(int i = 0; i < DEF_CAM_NUM_OF_OPTION_MSG; i++)
	{
		if (i >= 0 && i <= 8)
			cam_resolution_list[i] = option_cache[i];
		else if (i >= 9 && i <= 14)
			cam_exposure_list[i - 9] = option_cache[i];
		else if (i >= 15 && i <= 17)
			cam_lens_correction_list[i - 15] = option_cache[i];
		else if (i >= 18 && i <= 23)
			cam_white_balance_list[i - 18] = option_cache[i];
		else if (i >= 24 && i <= 34)
			cam_contrast_list[i - 24] = option_cache[i];
		else if (i >= 35 && i <= 49)
			cam_framelate_list[i - 35] = option_cache[i];
	}

	cam_status += "\nInitializing camera...";
	log_num = Util_log_save(DEF_CAM_INIT_STR , "Util_cam_init()...");
	result = Util_cam_init(PIXEL_FORMAT_RGB565LE);
	Util_log_add(log_num, result.string + result.error_description, result.code);
	if(result.code != 0)
	{
		Util_err_set_error_message(result.string, result.error_description, DEF_CAM_INIT_STR , result.code);
		Util_err_set_error_show_flag(true);
	}

	cam_status += ".";
	log_num = Util_log_save(DEF_CAM_INIT_STR, "Util_cam_set_resolution()...");
	result = Util_cam_set_resolution(cam_request_resolution_mode);
	Util_log_add(log_num, result.string, result.code);

	cam_status += ".";
	log_num = Util_log_save(DEF_CAM_INIT_STR, "Util_cam_set_fps()...");
	result = Util_cam_set_fps(cam_request_fps_mode);
	Util_log_add(log_num, result.string, result.code);

	cam_status += ".";
	log_num = Util_log_save(DEF_CAM_INIT_STR, "Util_cam_set_contrast()...");
	result = Util_cam_set_contrast(cam_request_contrast_mode);
	Util_log_add(log_num, result.string, result.code);

	cam_status += ".";
	log_num = Util_log_save(DEF_CAM_INIT_STR, "Util_cam_set_white_balance()...");
	result = Util_cam_set_white_balance(cam_request_white_balance_mode);
	Util_log_add(log_num, result.string, result.code);

	cam_status += ".";
	log_num = Util_log_save(DEF_CAM_INIT_STR, "Util_cam_set_lens_correction()...");
	result = Util_cam_set_lens_correction(cam_request_lens_correction_mode);
	Util_log_add(log_num, result.string, result.code);

	cam_status += ".";
	log_num = Util_log_save(DEF_CAM_INIT_STR, "Util_cam_set_camera()...");
	result = Util_cam_set_camera(cam_request_camera_mode);
	Util_log_add(log_num, result.string, result.code);

	cam_status += ".";
	log_num = Util_log_save(DEF_CAM_INIT_STR, "Util_cam_set_exposure()...");
	result = Util_cam_set_exposure(cam_request_exposure_mode);
	Util_log_add(log_num, result.string, result.code);

	cam_status += ".";
	log_num = Util_log_save(DEF_CAM_INIT_STR, "Util_cam_set_noise_filter()...");
	result = Util_cam_set_noise_filter(cam_request_noise_filter_mode);
	Util_log_add(log_num, result.string, result.code);

	cam_status += "\nInitializing variables...";

	result = Util_queue_create(&cam_scan_qr_thread_queue, 25);
	Util_log_save(DEF_CAM_INIT_STR, "Util_queue_create()..." + result.string + result.error_description, result.code);

	result = Util_queue_create(&cam_main_thread_queue, 10);
	Util_log_save(DEF_CAM_INIT_STR, "Util_queue_create()..." + result.string + result.error_description, result.code);

	for (int i = 0; i < 2; i++)
	{
		result = Draw_texture_init(&cam_capture_image[i], 1024, 512, PIXEL_FORMAT_RGB565LE);
		if(result.code != 0)
		{
			Util_err_set_error_message(result.string, result.error_description, DEF_CAM_INIT_STR, result.code);
			Util_err_set_error_show_flag(true);
		}
	}

	cam_encode_buffer = (u8*)Util_safe_linear_alloc(640 * 480 * 2);
	if(!cam_encode_buffer)
	{
		Util_err_set_error_message(DEF_ERR_OUT_OF_LINEAR_MEMORY_STR, "", DEF_CAM_INIT_STR, DEF_ERR_OUT_OF_LINEAR_MEMORY);
		Util_err_set_error_show_flag(true);
	}

	cam_scan_qr_mode = false;
	cam_qr_callback = NULL;
	cam_contrast_bar.c2d = var_square_image[0];
	cam_white_balance_bar.c2d = var_square_image[0];
	cam_lens_correction_bar.c2d = var_square_image[0];
	cam_exposure_bar.c2d = var_square_image[0];
	cam_jpg_quality_bar.c2d = var_square_image[0];
	cam_png_button.c2d = var_square_image[0];
	cam_jpg_button.c2d = var_square_image[0];
	cam_noise_filter_on_button.c2d = var_square_image[0];
	cam_noise_filter_off_button.c2d = var_square_image[0];
	cam_shutter_sound_on_button.c2d = var_square_image[0];
	cam_shutter_sound_off_button.c2d = var_square_image[0];
	for (int i = 0; i < 4; i++)
		cam_menu_button[i].c2d = var_square_image[0];
	for (int i = 0; i < 9; i++)
		cam_resolution_button[i].c2d = var_square_image[0];
	for (int i = 0; i < 3; i++)
		cam_camera_button[i].c2d = var_square_image[0];
	for (int i = 0; i < 15; i++)
		cam_fps_button[i].c2d = var_square_image[0];

	Util_add_watch(&cam_contrast_bar.selected);
	Util_add_watch(&cam_white_balance_bar.selected);
	Util_add_watch(&cam_lens_correction_bar.selected);
	Util_add_watch(&cam_exposure_bar.selected);
	Util_add_watch(&cam_jpg_quality_bar.selected);
	Util_add_watch(&cam_png_button.selected);
	Util_add_watch(&cam_jpg_button.selected);
	Util_add_watch(&cam_noise_filter_on_button.selected);
	Util_add_watch(&cam_noise_filter_off_button.selected);
	Util_add_watch(&cam_shutter_sound_on_button.selected);
	Util_add_watch(&cam_shutter_sound_off_button.selected);
	for (int i = 0; i < 4; i++)
		Util_add_watch(&cam_menu_button[i].selected);
	for (int i = 0; i < 9; i++)
		Util_add_watch(&cam_resolution_button[i].selected);
	for (int i = 0; i < 3; i++)
		Util_add_watch(&cam_camera_button[i].selected);
	for (int i = 0; i < 15; i++)
		Util_add_watch(&cam_fps_button[i].selected);

	cam_status += "\nStarting threads...";
	cam_thread_run = true;
	cam_capture_thread = threadCreate(Cam_capture_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_REALTIME, 0, false);

	if(var_model == CFG_MODEL_N2DSXL || var_model == CFG_MODEL_N3DS || var_model == CFG_MODEL_N3DSXL)
	{
		cam_scan_qr_thread = threadCreate(Cam_scan_qr_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_IDLE, 2, false);
		cam_encode_thread = threadCreate(Cam_encode_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_HIGH, 2, false);
	}
	else
	{
		cam_scan_qr_thread = threadCreate(Cam_scan_qr_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_IDLE, 0, false);
		cam_encode_thread = threadCreate(Cam_encode_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 0, false);
	}

	cam_display_img_num = -1;
	cam_already_init = true;

	Util_log_save(DEF_CAM_INIT_STR, "Thread exit.");
	threadExit(0);
}

void Cam_exit_thread(void* arg)
{
	Util_log_save(DEF_CAM_EXIT_STR, "Thread started.");
	int log_num;
	std::string data = "<0>" + std::to_string(cam_camera_mode) + "</0><1>" + std::to_string(cam_resolution_mode) + "</1><2>" + std::to_string(cam_fps_mode) + "</2>"
	+ "<3>" + std::to_string(cam_contrast_mode) + "</3><4>" + std::to_string(cam_white_balance_mode) + "</4><5>" + std::to_string(cam_lens_correction_mode) + "</5>"
	+ "<6>" + std::to_string(cam_exposure_mode) + "</6><7>" + std::to_string(cam_encode_format_mode) + "</7><8>" + std::to_string(cam_noise_filter_mode) + "</8>"
	+ "<9>" + std::to_string(cam_selected_jpg_quality) + "</9><10>" + std::to_string(cam_shutter_sound_mode) + "</10>";
	Result_with_string result;

	cam_thread_suspend = false;
	cam_thread_run = false;

	cam_status = "Saving settings...";
	log_num = Util_log_save(DEF_CAM_EXIT_STR, "Util_file_save_to_file()...");
	result = Util_file_save_to_file("Cam_setting.txt", DEF_MAIN_DIR, (u8*)data.c_str(), data.length(), true);
	Util_log_add(log_num, result.string, result.code);

	cam_status += "\nExiting threads...";
	Util_log_save(DEF_CAM_EXIT_STR, "threadJoin()...", threadJoin(cam_encode_thread, DEF_THREAD_WAIT_TIME));

	cam_status += ".";
	Util_log_save(DEF_CAM_EXIT_STR, "threadJoin()...", threadJoin(cam_capture_thread, DEF_THREAD_WAIT_TIME));

	cam_status += ".";
	Util_log_save(DEF_CAM_EXIT_STR, "threadJoin()...", threadJoin(cam_scan_qr_thread, DEF_THREAD_WAIT_TIME));

	cam_status += "\nExiting camera...";
	Util_cam_exit();

	cam_status += "\nCleaning up...";
	threadFree(cam_encode_thread);
	threadFree(cam_capture_thread);
	Util_queue_delete(&cam_scan_qr_thread_queue);
	Util_queue_delete(&cam_main_thread_queue);

	for (int i = 0; i < 2; i++)
		Draw_texture_free(&cam_capture_image[i]);

	Util_safe_linear_free(cam_encode_buffer);
	cam_encode_buffer = NULL;

	Util_remove_watch(&cam_contrast_bar.selected);
	Util_remove_watch(&cam_white_balance_bar.selected);
	Util_remove_watch(&cam_lens_correction_bar.selected);
	Util_remove_watch(&cam_exposure_bar.selected);
	Util_remove_watch(&cam_jpg_quality_bar.selected);
	Util_remove_watch(&cam_png_button.selected);
	Util_remove_watch(&cam_jpg_button.selected);
	Util_remove_watch(&cam_noise_filter_on_button.selected);
	Util_remove_watch(&cam_noise_filter_off_button.selected);
	Util_remove_watch(&cam_shutter_sound_on_button.selected);
	Util_remove_watch(&cam_shutter_sound_off_button.selected);
	for (int i = 0; i < 4; i++)
		Util_remove_watch(&cam_menu_button[i].selected);
	for (int i = 0; i < 9; i++)
		Util_remove_watch(&cam_resolution_button[i].selected);
	for (int i = 0; i < 3; i++)
		Util_remove_watch(&cam_camera_button[i].selected);
	for (int i = 0; i < 15; i++)
		Util_remove_watch(&cam_fps_button[i].selected);

	cam_already_init = false;

	Util_log_save(DEF_CAM_EXIT_STR, "Thread exit.");
	threadExit(0);
}

void Cam_init(bool draw)
{
	Util_log_save(DEF_CAM_INIT_STR, "Initializing...");
	int color = DEF_DRAW_BLACK;
	int back_color = DEF_DRAW_WHITE;

	Util_add_watch(&cam_status);
	cam_status = "";

	if((var_model == CFG_MODEL_N2DSXL || var_model == CFG_MODEL_N3DSXL || var_model == CFG_MODEL_N3DS) && var_core_2_available)
		cam_init_thread = threadCreate(Cam_init_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 2, false);
	else
	{
		APT_SetAppCpuTimeLimit(80);
		cam_init_thread = threadCreate(Cam_init_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 1, false);
	}

	while(!cam_already_init)
	{
		if(draw)
		{
			if (var_night_mode)
			{
				color = DEF_DRAW_WHITE;
				back_color = DEF_DRAW_BLACK;
			}

			if(Util_is_watch_changed() || var_need_reflesh || !var_eco_mode)
			{
				var_need_reflesh = false;
				Draw_frame_ready();
				Draw_screen_ready(SCREEN_TOP_LEFT, back_color);
				Draw_top_ui();
				if(var_monitor_cpu_usage)
					Draw_cpu_usage_info();

				Draw(cam_status, 0, 20, 0.65, 0.65, color);

				Draw_apply_draw();
			}
			else
				gspWaitForVBlank();
		}
		else
			Util_sleep(20000);
	}

	if(!(var_model == CFG_MODEL_N2DSXL || var_model == CFG_MODEL_N3DSXL || var_model == CFG_MODEL_N3DS) || !var_core_2_available)
		APT_SetAppCpuTimeLimit(10);

	Util_log_save(DEF_CAM_EXIT_STR, "threadJoin()...", threadJoin(cam_init_thread, DEF_THREAD_WAIT_TIME));
	threadFree(cam_init_thread);
	Cam_resume();

	Util_log_save(DEF_CAM_INIT_STR, "Initialized.");
}

void Cam_exit(bool draw)
{
	Util_log_save(DEF_CAM_EXIT_STR, "Exiting...");

	int color = DEF_DRAW_BLACK;
	int back_color = DEF_DRAW_WHITE;

	cam_status = "";
	cam_exit_thread = threadCreate(Cam_exit_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 1, false);

	while(cam_already_init)
	{
		if(draw)
		{
			if (var_night_mode)
			{
				color = DEF_DRAW_WHITE;
				back_color = DEF_DRAW_BLACK;
			}

			if(Util_is_watch_changed() || var_need_reflesh || !var_eco_mode)
			{
				var_need_reflesh = false;
				Draw_frame_ready();
				Draw_screen_ready(SCREEN_TOP_LEFT, back_color);
				Draw_top_ui();
				if(var_monitor_cpu_usage)
					Draw_cpu_usage_info();

				Draw(cam_status, 0, 20, 0.65, 0.65, color);

				Draw_apply_draw();
			}
			else
				gspWaitForVBlank();
		}
		else
			Util_sleep(20000);
	}

	Util_log_save(DEF_CAM_EXIT_STR, "threadJoin()...", threadJoin(cam_exit_thread, DEF_THREAD_WAIT_TIME));
	threadFree(cam_exit_thread);
	Util_remove_watch(&cam_status);
	var_need_reflesh = true;

	Util_log_save(DEF_CAM_EXIT_STR, "Exited.");
}

void Cam_main(void)
{
	int color = DEF_DRAW_BLACK;
	int weak_color = DEF_DRAW_WEAK_BLACK;
	int back_color = DEF_DRAW_WHITE;
	int image_num = 0;
	double pos_x = 0.0;
	double pos_y = 0.0;
	double bar_x[4] = { 0, 0, 0, 0, };
	void* command_data = NULL;
	Result_with_string result;
	Cam_command command_id = CAM_COMMAND_INVALID;

	if (var_night_mode)
	{
		color = DEF_DRAW_WHITE;
		weak_color = DEF_DRAW_WEAK_WHITE;
		back_color = DEF_DRAW_BLACK;
	}

	if(Util_is_watch_changed() || var_need_reflesh || !var_eco_mode)
	{
		var_need_reflesh = false;
		Draw_frame_ready();

		if(var_turn_on_top_lcd)
		{
			Draw_screen_ready(SCREEN_TOP_LEFT, back_color);

			Draw_top_ui();
			if (cam_display_img_num != -1)
			{
				//Camera image
				image_num = cam_display_img_num;
				if (cam_resolution_mode == 2)
					Draw_texture(&cam_capture_image[image_num], 0, 0, 400, 240);
				else
					Draw_texture(&cam_capture_image[image_num], 40, 0, 320, 240);
			}

			if(cam_scan_qr_mode)
			{
				//Display scan information.
				Draw(cam_msg[32], 0, 25, 0.45, 0.45, DEF_DRAW_WHITE, X_ALIGN_CENTER, Y_ALIGN_TOP,
				400, 30, BACKGROUND_UNDER_TEXT, var_square_image[0], 0xA0000000);
			}

			if(Util_log_query_log_show_flag())
				Util_log_draw();

			if(var_monitor_cpu_usage)
				Draw_cpu_usage_info();

			if(Draw_is_3d_mode())
			{
				Draw_screen_ready(SCREEN_TOP_RIGHT, back_color);

				Draw_top_ui();
				if (cam_display_img_num != -1)
				{
					//Camera image
					image_num = cam_display_img_num;
					if (cam_resolution_mode == 2)
						Draw_texture(&cam_capture_image[image_num], 0, 0, 400, 240);
					else
						Draw_texture(&cam_capture_image[image_num], 40, 0, 320, 240);
				}

				if(Util_log_query_log_show_flag())
					Util_log_draw();

				if(var_monitor_cpu_usage)
					Draw_cpu_usage_info();
			}
		}

		if(var_turn_on_bottom_lcd)
		{
			Draw_screen_ready(SCREEN_BOTTOM, back_color);

			Draw(DEF_CAM_VER, 0, 0, 0.4, 0.4, DEF_DRAW_GREEN);

			Draw(std::to_string(cam_width_list[cam_resolution_mode]) + cam_msg[0] + std::to_string(cam_height_list[cam_resolution_mode]) + cam_msg[1] + cam_framelate_list[cam_fps_mode] + cam_msg[2], 20, 15, 0.6, 0.6, color);
			Draw(cam_msg[3] + cam_contrast_list[cam_contrast_mode] + cam_msg[4] + cam_white_balance_list[cam_white_balance_mode], 20, 30, 0.55, 0.55, color);
			Draw(cam_msg[5] + cam_lens_correction_list[cam_lens_correction_mode] + cam_msg[6] + cam_exposure_list[cam_exposure_mode], 20, 45, 0.55, 0.55, color);
			//Draw(cam_msg[28] + cam_format_name_list[cam_encode_format_mode] + "/" + Menu_query_time(1) + "/", 20, 60, 0.4, 0.4, DEF_DRAW_RED);

			//Menu buttons
			Draw_texture(&cam_menu_button[0], (cam_menu_button[0].selected || cam_selected_menu_mode == DEF_CAM_MENU_RESOLUTION) ? DEF_DRAW_YELLOW : DEF_DRAW_WEAK_YELLOW, 40, 110, 60, 10);
			Draw_texture(&cam_menu_button[1], (cam_menu_button[1].selected || cam_selected_menu_mode == DEF_CAM_MENU_FPS) ? DEF_DRAW_GREEN : DEF_DRAW_WEAK_GREEN, 100, 110, 60, 10);
			Draw_texture(&cam_menu_button[2], (cam_menu_button[2].selected || cam_selected_menu_mode == DEF_CAM_MENU_ADVANCED_0) ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA, 160, 110, 60, 10);
			Draw_texture(&cam_menu_button[3], (cam_menu_button[3].selected || cam_selected_menu_mode == DEF_CAM_MENU_ADVANCED_1) ? DEF_DRAW_RED : DEF_DRAW_WEAK_RED, 220, 110, 60, 10);
			Draw_texture(var_square_image[0], DEF_DRAW_WEAK_AQUA, 40, 120, 240, 80);

			for (int i = 0; i < 3; i++)
			{
				//Camera button
				Draw(cam_msg[25 + i], 40 + (i * 85), 75, 0.4, 0.4, i == cam_camera_mode ? DEF_DRAW_RED : color, X_ALIGN_CENTER, Y_ALIGN_CENTER,
				70, 30, BACKGROUND_ENTIRE_BOX, &cam_camera_button[i], cam_camera_button[i].selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
			}

			if (cam_selected_menu_mode == DEF_CAM_MENU_RESOLUTION)
			{
				pos_x = 40;
				pos_y = 120;
				for (int i = 0; i < 9; i++)
				{
					//Resolution button
					Draw(cam_resolution_list[i], pos_x, pos_y, 0.4, 0.4, i == cam_resolution_mode ? DEF_DRAW_RED : color, X_ALIGN_CENTER, Y_ALIGN_CENTER,
					60, 20, BACKGROUND_ENTIRE_BOX, &cam_resolution_button[i], cam_resolution_button[i].selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
					if (pos_y + 30 > 180)
					{
						pos_x += 90;
						pos_y = 120;
					}
					else
						pos_y += 30;
				}
			}
			else if (cam_selected_menu_mode == DEF_CAM_MENU_FPS)
			{
				pos_x = 40;
				pos_y = 120;
				for (int i = 0; i < 15; i++)
				{
					//Framerate button
					Draw(cam_framelate_list[i], pos_x, pos_y, 0.4, 0.4, i == cam_fps_mode ? DEF_DRAW_RED : color, X_ALIGN_CENTER, Y_ALIGN_CENTER,
					30, 20, BACKGROUND_ENTIRE_BOX, &cam_fps_button[i], cam_fps_button[i].selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
					if (pos_y + 30 > 180)
					{
						pos_x += 50;
						pos_y = 120;
					}
					else
						pos_y += 30;
				}
			}
			else if (cam_selected_menu_mode == DEF_CAM_MENU_ADVANCED_0)
			{
				pos_x = 40;
				pos_y = 120;
				bar_x[0] = 40 + 9.5 * cam_contrast_mode;
				bar_x[1] = 40 + 19 * cam_white_balance_mode;
				bar_x[2] = 180 + 47.5 * cam_lens_correction_mode;
				bar_x[3] = 180 + 19 * cam_exposure_mode;

				//Contrast bar
				Draw(cam_msg[7], pos_x, pos_y, 0.4, 0.4, color);
				Draw_texture(var_square_image[0], DEF_DRAW_WEAK_RED, pos_x, (pos_y + 17.5), 100, 5);
				Draw_texture(&cam_contrast_bar, cam_contrast_bar.selected ? color : weak_color, bar_x[0], (pos_y + 10), 5, 20);

				//White balance bar
				pos_y += 30;
				Draw(cam_msg[8], pos_x, pos_y, 0.4, 0.4, color);
				Draw_texture(var_square_image[0], DEF_DRAW_WEAK_RED, pos_x, (pos_y + 17.5), 100, 5);
				Draw_texture(&cam_white_balance_bar, cam_white_balance_bar.selected ? color : weak_color, bar_x[1], (pos_y + 10), 5, 20);

				//Lens correction bar
				pos_x = 180;
				pos_y = 120;
				Draw(cam_msg[9], pos_x, pos_y, 0.4, 0.4, color);
				Draw_texture(var_square_image[0], DEF_DRAW_WEAK_RED, pos_x, (pos_y + 17.5), 100, 5);
				Draw_texture(&cam_lens_correction_bar, cam_lens_correction_bar.selected ? color : weak_color, bar_x[2], (pos_y + 10), 5, 20);

				//Exposure bar
				pos_y += 30;
				Draw(cam_msg[10], pos_x, pos_y, 0.4, 0.4, color);
				Draw_texture(var_square_image[0], DEF_DRAW_WEAK_RED, pos_x, (pos_y + 17.5), 100, 5);
				Draw_texture(&cam_exposure_bar, cam_exposure_bar.selected ? color : weak_color, bar_x[3], (pos_y + 10), 5, 20);
			}
			else if (cam_selected_menu_mode == DEF_CAM_MENU_ADVANCED_1)
			{
				Draw(cam_msg[11], 42.5, 120, 0.4, 0.4, color);

				//PNG button
				Draw(cam_msg[12], 40, 130, 0.4, 0.4, cam_encode_format_mode == 0 ? DEF_DRAW_RED : color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 40, 20,
				BACKGROUND_ENTIRE_BOX, &cam_png_button, cam_png_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

				//JPG button
				Draw(cam_msg[13], 100, 130, 0.4, 0.4, cam_encode_format_mode == 1 ? DEF_DRAW_RED : color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 40, 20,
				BACKGROUND_ENTIRE_BOX, &cam_jpg_button, cam_jpg_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

				if(cam_encode_format_mode == 1)
				{
					//JPG quality bar
					Draw(cam_msg[14] + std::to_string(cam_selected_jpg_quality), 42.5, 150, 0.4, 0.4, color);
					Draw_texture(var_square_image[0], DEF_DRAW_WEAK_RED, 41, 167.5, 100, 5);
					Draw_texture(&cam_jpg_quality_bar, cam_jpg_quality_bar.selected ? color : weak_color, (40 + 1 * cam_selected_jpg_quality), 160, 5, 20);
				}

				//Noise filter
				Draw(cam_msg[15], 182.5, 120, 0.4, 0.4, color);

				//ON
				Draw(cam_msg[16], 180, 130, 0.4, 0.4, cam_noise_filter_mode ? DEF_DRAW_RED : color, X_ALIGN_CENTER, Y_ALIGN_CENTER,
				40, 20, BACKGROUND_ENTIRE_BOX, &cam_noise_filter_on_button, cam_noise_filter_on_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

				//OFF
				Draw(cam_msg[17], 240, 130, 0.4, 0.4, !cam_noise_filter_mode ? DEF_DRAW_RED : color, X_ALIGN_CENTER, Y_ALIGN_CENTER,
				40, 20, BACKGROUND_ENTIRE_BOX, &cam_noise_filter_off_button, cam_noise_filter_off_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

				//Shutter sound
				Draw(cam_msg[18], 182.5, 150, 0.4, 0.4, color);

				//ON
				Draw(cam_msg[16], 180, 160, 0.4, 0.4, cam_shutter_sound_mode ? DEF_DRAW_RED : color, X_ALIGN_CENTER, Y_ALIGN_CENTER,
				40, 20, BACKGROUND_ENTIRE_BOX, &cam_shutter_sound_on_button, cam_shutter_sound_on_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

				//OFF
				Draw(cam_msg[17], 240, 160, 0.4, 0.4, !cam_shutter_sound_mode ? DEF_DRAW_RED : color, X_ALIGN_CENTER, Y_ALIGN_CENTER,
				40, 20, BACKGROUND_ENTIRE_BOX, &cam_shutter_sound_off_button, cam_shutter_sound_off_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
			}
			for (int i = 0; i < 4; i++)
				Draw(cam_msg[i + 21], 40 + (i * 60), 110, 0.35, 0.35, color);

			if (cam_encode_request)
			{
				Draw_texture(var_square_image[0], DEF_DRAW_AQUA, 40, 80, 240, 50);

				if(cam_encode_request)
					Draw(cam_msg[19], 52.5, 100, 0.5, 0.5, color);
			}
			//Controls
			Draw(cam_msg[31], 0, 200, 0.5, 0.5, color, X_ALIGN_CENTER, Y_ALIGN_CENTER, 320, 20);

			if(Util_err_query_error_show_flag())
				Util_err_draw();

			Draw_bot_ui();
		}

		Draw_apply_draw();
	}
	else
		gspWaitForVBlank();

	result = Util_queue_get(&cam_main_thread_queue, (u32*)&command_id, &command_data, 0);
	if(result.code == DEF_SUCCESS)
	{
		switch (command_id)
		{
			case CAM_COMMAND_MAIN_THREAD_SCAN_QR_SUCCESS_INDICATION:
			{
				Cam_scan_qr_success_indication_data* data = (Cam_scan_qr_success_indication_data*)command_data;

				//Data is mandatory.
				if(!command_data)
				{
					cam_qr_callback = NULL;
					break;
				}

				//Call callback function, then free decoded_data.
				if(cam_qr_callback)
					cam_qr_callback(data->decoded_data);

				free(data->decoded_data);
				data->decoded_data = NULL;
				cam_qr_callback = NULL;

				break;
			}

			default:
			{
				//Invalid command was received.
				char msg[64];
				snprintf(msg, sizeof(msg), "Invalid command (%08X) was received!!!!!", command_id);
				Util_log_save(DEF_CAM_MAIN_STR, msg);
				break;
			}
		}
	}

	free(command_data);
	command_data = NULL;
}
