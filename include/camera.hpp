#pragma once

#define CAM_NUM_OF_MSG 29
#define CAM_NUM_OF_OPTION_MSG 52

#define CAM_TAKE_PICTURES_REQUEST 0
#define CAM_ENCODE_PICTURES_REQUEST 1

#define CAM_SELECTED_MENU_MODE 0
#define CAM_SELECTED_FORMAT_MODE 1
#define CAM_SELECTED_JPG_QUALITY 2

#define CAM_MENU_RESOLUTION 0
#define CAM_MENU_FPS 1
#define CAM_MENU_ADVANCED_0 2
#define CAM_MENU_ADVANCED_1 3

#define CAM_RESOLUTION 0
#define CAM_FPS 1
#define CAM_CONTRAST 2
#define CAM_WHITE_BALANCE 3
#define CAM_LENS_CORRECTION 4
#define CAM_CAMERA 5
#define CAM_EXPOSURE 6
#define CAM_SHUTTER_SOUND 7
#define CAM_NOISE_FILTER 8

#define CAM_MSG 0
#define CAM_OPTION_MSG 1

void Cam_main(void);

void Cam_init(void);

void Cam_exit(void);

void Cam_encode_thread(void* arg);

void Cam_capture_thread(void* arg);

void Cam_parse_thread(void* arg);

bool Cam_query_init_flag(void);

bool Cam_query_running_flag(void);

void Cam_resume(void);

void Cam_suspend(void);

int Cam_convert_to_resolution(int resolution_mode, bool width);

int Cam_query_cam_settings(int setting_num);

int Cam_query_framerate(void);

bool Cam_query_operation_flag(int operation_num);

int Cam_query_selected_num(int item_num);

void Cam_reset_framerate(void);

void Cam_set_cam_settings(int setting_num, int num);

void Cam_set_msg(int msg_num, int msg_type, std::string msg);

void Cam_set_operation_flag(int operation_num, bool flag);

void Cam_set_selected_num(int item_num, int value);

Result_with_string Cam_set_capture_camera(int camera_num, int width, int height, u32* out_buffer_size);

Result_with_string Cam_set_capture_contrast(int contrast_num);

Result_with_string Cam_set_capture_exposure(int exposure_num);

Result_with_string Cam_set_capture_fps(int fps_num);

Result_with_string Cam_set_capture_lens_correction(int lens_correction_num);

Result_with_string Cam_set_capture_noise_filter(int noise_filter_num);

Result_with_string Cam_set_capture_size(int width, int height, u32* out_buffer_size);

Result_with_string Cam_set_capture_white_balance(int white_balance_num);

Result_with_string Cam_take_picture(u8* capture_buffer, int size, bool out_cam_1, bool play_shutter_sound);

Result_with_string Cam_cam_init(int camera_moide, int resolution_mode, int noise_filter_mode, int exposure_mode,
	int white_balance_mode, int fps_mode, int contrast_mode, int lens_correction_mode, u32* buffer_size);
