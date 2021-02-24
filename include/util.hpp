#pragma once

#define UTIL_ENCODER_0 0
#define UTIL_ENCODER_1 1
#define UTIL_ENCODER_2 2

#define UTIL_DECODER_0 0
#define UTIL_DECODER_1 1

#define UTIL_MUXER_0 0
#define UTIL_MUXER_1 1

#define UTIL_CONVERTER_0 0
#define UTIL_CONVERTER_1 1

extern "C" {
#include <libavcodec/avcodec.h>
}

Result_with_string Util_create_output_file(std::string file_name, int session);

Result_with_string Util_init_audio_encoder(AVCodecID id, int original_samplerate, int encode_samplerate, int bitrate, int session);

Result_with_string Util_init_video_encoder(AVCodecID id, int width, int height, int fps, int session);

Result_with_string Util_write_header(int session);

Result_with_string Util_encode_audio(int size, u8* raw_data, int session);

Result_with_string Util_encode_video(u8* raw_data, int session);

void Util_close_output_file(int session);

void Util_exit_audio_encoder(int session);

void Util_exit_video_encoder(int session);

Result_with_string Util_open_file(std::string file_path, bool* has_audio, bool* has_video, int session);

Result_with_string Util_init_audio_decoder(int session);

Result_with_string Util_init_video_decoder(int low_resolution, int session);

void Util_get_audio_info(int* bitrate, int* samplerate, int* ch, std::string* format_name, double* duration, int session);

void Util_get_video_info(int* width, int* height, double* framerate, std::string* format_name, double* duration, int session);

Result_with_string Util_read_packet(AVMediaType* type, int session);

Result_with_string Util_ready_audio_packet(int session);

Result_with_string Util_ready_video_packet(int session);

void Util_skip_audio_packet(int session);

void Util_skip_video_packet(int session);

Result_with_string Util_decode_audio(int* size, u8** raw_data, int session);

Result_with_string Util_decode_video(int* width, int* height, bool* key_frame, double* current_pos, int session);

void Util_get_video_image(u8* raw_data, int width, int height, int session);

Result_with_string Util_seek(u64 seek_pos, int flag, int session);

void Util_close_file(int session);

void Util_exit_audio_decoder(int session);

void Util_exit_video_decoder(int session);

Result_with_string Util_mux_file(std::string file_name, int session);

Result_with_string Util_open_muxer_audio_file(std::string file_path, int session);

Result_with_string Util_open_muxer_video_file(std::string file_path, int session);

void Util_close_muxer_audio_file(int session);

void Util_close_muxer_video_file(int session);

Result_with_string Util_init_yuv422_rgb565_converter(int width, int height, int session);

void Util_convert_yuv422_rgb565(u8* yuv422_raw, u8* rgb565_raw, int session);

void Util_exit_yuv422_rgb565_converter(int session);
