#pragma once

#include "share_function.hpp"

Result_with_string Draw_C3DTexToC2DImage(C3D_Tex* c3d_tex, Tex3DS_SubTexture* c3d_subtex, u8* buf, u32 size, u32 width, u32 height, int parse_start_width, int parse_start_height, GPU_TEXCOLOR format);

void Line_init(void);

void Line_main(void);

void Line_log_download_thread(void* arg);

void Line_update_thread(void* arg);

void Line_send_message_thread(void* arg);

void Line_pic_parse_thread(void* arg);

Result_with_string Line_save_new_id(std::string id, std::string dir_path);

Result_with_string Line_read_id(std::string dir_path);

Result_with_string Line_load_log_from_sd(std::string file_name);

void Line_log_parse_thread(void* arg);

void Line_log_load_thread(void* arg);

void Line_exit(void);
