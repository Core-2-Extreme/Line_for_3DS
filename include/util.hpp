#pragma once

#define UTIL_AUDIO_ENCODER_0 0
#define UTIL_AUDIO_ENCODER_1 1

extern "C" {
#include <libavcodec/avcodec.h>
}

Result_with_string Util_init_audio_encoder(AVCodecID id, int samplerate, int bitrate, int session);

Result_with_string Util_encode_audio(int size, u8* raw_data, int* encoded_size, u8* encoded_data, int session);

void Util_exit_audio_encoder(int session);
