#include <string>
#include <3ds.h>

#include "types.hpp"
#include "error.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

extern "C" void memcpy_asm(u8*, u8*, int);


AVPacket* util_packet[2] = { NULL, NULL, };
AVFrame* util_raw_data[2] = { NULL, NULL, };
AVCodecContext* util_context[2] = { NULL, NULL, };
AVCodec* util_codec[2] = { NULL, NULL, };
SwrContext* util_swr_context[2] = { NULL, NULL, };

Result_with_string Util_init_audio_encoder(AVCodecID id, int samplerate, int bitrate, int session)
{
	int ffmpeg_result = 0;
	int original_samplerate = samplerate;
	Result_with_string result;

	util_codec[session] = avcodec_find_encoder(id);
	if(!util_codec[session])
	{
		result.code = FFMPEG_RETURNED_NOT_SUCCESS;
		result.string = "avcodec_find_encoder() failed";
		return result;
	}

	util_context[session] = avcodec_alloc_context3(util_codec[session]);
	if(!util_codec[session])
	{
		result.code = FFMPEG_RETURNED_NOT_SUCCESS;
		result.string = "avcodec_alloc_context3() failed";
		return result;
	}

	for(int i = 0; i < 30; i++)//select supported samplerate
	{
		if(!util_codec[session]->supported_samplerates[i])
			break;
		else
		{
			if(util_codec[session]->supported_samplerates[i] - samplerate <= 0)
			{
				samplerate += util_codec[session]->supported_samplerates[i] - samplerate;
				//Log_log_save("", std::to_string(samplerate), 1234567890, false);
				break;
			}
		}
	}
	util_context[session]->bit_rate = bitrate;
	util_context[session]->sample_fmt = AV_SAMPLE_FMT_S16;
	util_context[session]->sample_rate = samplerate;
	util_context[session]->channel_layout = AV_CH_LAYOUT_MONO;
	util_context[session]->channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_MONO);
	//context->profile = FF_PROFILE_AAC_MAIN;
	util_context[session]->codec_type = AVMEDIA_TYPE_AUDIO;

	ffmpeg_result = avcodec_open2(util_context[session], util_codec[session], NULL);
	if(ffmpeg_result != 0)
	{
		result.code = FFMPEG_RETURNED_NOT_SUCCESS;
		result.string = "avcodec_open2() failed";
		result.error_description = "avcodec error code : " + std::to_string(ffmpeg_result);
		return result;
	}
	
	util_packet[session] = av_packet_alloc();
	util_raw_data[session] = av_frame_alloc();
	if(!util_raw_data[session] || !util_packet)
	{
		result.code = FFMPEG_RETURNED_NOT_SUCCESS;
		result.string = "av_packet_alloc() / av_frame_alloc() failed";
		return result;	
	}

	util_raw_data[session]->nb_samples = util_context[session]->frame_size;
	util_raw_data[session]->format = util_context[session]->sample_fmt;
	util_raw_data[session]->channel_layout = util_context[session]->channel_layout;

	ffmpeg_result = av_frame_get_buffer(util_raw_data[session], 0);
	if(ffmpeg_result != 0)
	{
		result.code = FFMPEG_RETURNED_NOT_SUCCESS;
		result.string = "av_frame_get_buffer() failed";
		result.error_description = "avcodec error code : " + std::to_string(ffmpeg_result);
		return result;
	}

	av_frame_make_writable(util_raw_data[session]);

	util_swr_context[session] = swr_alloc();
	swr_alloc_set_opts(util_swr_context[session], av_get_default_channel_layout(util_context[session]->channels), util_context[session]->sample_fmt, util_context[session]->sample_rate,
	av_get_default_channel_layout(util_context[session]->channels), AV_SAMPLE_FMT_S16, original_samplerate, 0, NULL);
	if(!util_swr_context[session])
	{
		result.code = FFMPEG_RETURNED_NOT_SUCCESS;
		result.string = "swr_alloc_set_opts() failed";
		return result;
	}
	swr_init(util_swr_context[session]);

	return result;
}

Result_with_string Util_encode_audio(int size, u8* raw_data, int* encoded_size, u8* encoded_data, int session)
{
	int encode_offset = 0;
	int encoded_offset = 0;
	int ffmpeg_result = 0;
	int one_frame_size = av_samples_get_buffer_size(NULL, util_context[session]->channels, util_context[session]->frame_size, util_context[session]->sample_fmt, 0);
	int out_samples = 0;
	u8* swr_in_cache[1] = { NULL, };
	u8* swr_out_cache = NULL;
	Result_with_string result;

	*encoded_size = 0;
	swr_in_cache[0] = (u8*)malloc(size);
	swr_out_cache = (u8*)malloc(size);
	if(swr_in_cache[0] == NULL || swr_out_cache == NULL)
	{
		result.code = OUT_OF_MEMORY;
		result.string = Err_query_template_summary(OUT_OF_MEMORY);
		result.error_description = Err_query_template_detail(OUT_OF_MEMORY);
		free(swr_in_cache[0]);
		free(swr_out_cache);
		swr_in_cache[0] = NULL;
		swr_out_cache = NULL;
		return result;
	}

	memcpy(swr_in_cache[0] , raw_data, size);
	out_samples = swr_convert(util_swr_context[session], &swr_out_cache, size / 2, (const uint8_t**)swr_in_cache, size / 2);
	free(swr_in_cache[0]);
	swr_in_cache[0] = NULL;

	for(int i = 0; i < 100000; i++)
	{
		util_raw_data[session]->data[0] = swr_out_cache  + encode_offset;

		ffmpeg_result = avcodec_send_frame(util_context[session], util_raw_data[session]);
		if(ffmpeg_result != 0)
		{
			result.code = FFMPEG_RETURNED_NOT_SUCCESS;
			result.string = "avcodec_send_frame() failed";
			result.error_description = "avcodec error code : " + std::to_string(ffmpeg_result);
			break;
		}

		ffmpeg_result = avcodec_receive_packet(util_context[session], util_packet[session]);
		if(ffmpeg_result == 0)
		{
			memcpy_asm(encoded_data + encoded_offset, util_packet[session]->data, util_packet[session]->size);
			encoded_offset += util_packet[session]->size;
			av_packet_unref(util_packet[session]);
		}

		if(encode_offset + one_frame_size*2 > (int)out_samples * 2)
			break;
		else
			encode_offset += one_frame_size;
	}
	*encoded_size = encoded_offset;
	free(swr_out_cache);
	swr_out_cache = NULL;

	return result;
}

void Util_exit_audio_encoder(int session)
{
	avcodec_free_context(&util_context[session]);
	av_packet_free(&util_packet[session]);
	av_frame_free(&util_raw_data[session]);
	swr_free(&util_swr_context[session]);	
}