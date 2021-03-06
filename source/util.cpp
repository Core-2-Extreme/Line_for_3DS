#include <string>
#include <unistd.h>
#include <3ds.h>

#include "types.hpp"
#include "error.hpp"
#include "util.hpp"
#include "log.hpp"

extern "C" {
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavformat/avformat.h>
}

extern "C" void memcpy_asm(u8*, u8*, int);

//Encoder
int util_audio_pos[3] = { 0, 0, 0, };
int util_audio_increase_pts[3] = { 0, 0, 0, };
AVPacket* util_audio_encoder_packet[3] = { NULL, NULL, NULL, };
AVFrame* util_audio_encoder_raw_data[3] = { NULL, NULL, NULL, };
AVCodecContext* util_audio_encoder_context[3] = { NULL, NULL, NULL, };
AVCodec* util_audio_encoder_codec[3] = { NULL, NULL, NULL, };
SwrContext* util_audio_encoder_swr_context[3] = { NULL, NULL, NULL, };
AVStream* util_audio_encoder_stream[3] = { NULL, NULL, NULL, };

int util_video_pos[3] = { 0, 0, 0, };
int util_video_increase_pts[3] = { 0, 0, 0, };
AVPacket* util_video_encoder_packet[3] = { NULL, NULL, NULL, };
AVFrame* util_video_encoder_raw_data[3] = { NULL, NULL, NULL, };
AVCodecContext* util_video_encoder_context[3] = { NULL, NULL, NULL, };
AVCodec* util_video_encoder_codec[3] = { NULL, NULL, NULL, };
SwsContext* util_video_encoder_sws_context[3] = { NULL, NULL, NULL, };
AVStream* util_video_encoder_stream[3] = { NULL, NULL, NULL, };

AVFormatContext* util_encoder_format_context[3] = { NULL, NULL, NULL, };

//Decoder
int util_audio_decoder_stream_num[2] = { -1, -1, };
AVPacket* util_audio_decoder_packet[2] = { NULL, NULL, };
AVPacket* util_audio_decoder_cache_packet[2] = { NULL, NULL, };
AVFrame* util_audio_decoder_raw_data[2] = { NULL, NULL, };
AVCodecContext* util_audio_decoder_context[2] = { NULL, NULL, };
AVCodec* util_audio_decoder_codec[2] = { NULL, NULL, };
SwrContext* util_audio_decoder_swr_context[2] = { NULL, NULL, };

bool util_video_decoder_lock[2][3] = { { false, false, false, }, { false, false, false, } };
int util_video_decoder_buffer_num[2] = { 0, 0, };
int util_video_decoder_ready_buffer_num[2] = { 0, 0, };
int util_video_decoder_stream_num[2] = { -1, -1, };
AVPacket* util_video_decoder_packet[2] = { NULL, NULL, };
AVPacket* util_video_decoder_cache_packet[2] = { NULL, NULL, };
AVFrame* util_video_decoder_raw_data[2][3] = { { NULL, NULL, NULL, }, { NULL, NULL, NULL, } };
AVCodecContext* util_video_decoder_context[2] = { NULL, NULL, };
AVCodec* util_video_decoder_codec[2] = { NULL, NULL, };

AVFormatContext* util_decoder_format_context[2] = { NULL, NULL, };

//Muxer
int util_audio_muxer_stream_num[2] = { -1, -1, };
AVPacket* util_audio_muxer_packet[2] = { NULL, NULL, };
AVFormatContext* util_audio_muxer_format_context[2] = { NULL, NULL, };
AVCodecContext* util_audio_muxer_context[2] = { NULL, NULL, };
AVCodec* util_audio_muxer_codec[2] = { NULL, NULL, };
AVStream* util_audio_muxer_format_stream[2] = { NULL, NULL, };

int util_video_muxer_stream_num[2] = { -1, -1, };
AVPacket* util_video_muxer_packet[2] = { NULL, NULL, };
AVFormatContext* util_video_muxer_format_context[2] = { NULL, NULL, };
AVCodecContext* util_video_muxer_context[2] = { NULL, NULL, };
AVCodec* util_video_muxer_codec[2] = { NULL, NULL, };
AVStream* util_video_muxer_format_stream[2] = { NULL, NULL, };

AVFormatContext* util_muxer_format_context[2] = { NULL, NULL, };

//Converter
int util_yuv422_rgb565_width[2] = { 0, 0, };
int util_yuv422_rgb565_height[2] = { 0, 0, };
SwsContext* util_yuv422_rgb565_converter_sws_context[2] = { NULL, NULL, };

Result_with_string Util_create_output_file(std::string file_name, int session)
{
	Result_with_string result;
	int ffmpeg_result = 0;

	util_encoder_format_context[session] = avformat_alloc_context();
	if(!util_encoder_format_context[session])
	{
		result.error_description = "avformat_alloc_context() failed";
		goto fail;
	}

	util_encoder_format_context[session]->oformat = av_guess_format(NULL, file_name.c_str(), NULL);
	if(!util_encoder_format_context[session]->oformat)
	{
		result.error_description = "av_guess_format() failed";
		goto fail;
	}

	ffmpeg_result = avio_open(&util_encoder_format_context[session]->pb, file_name.c_str(), AVIO_FLAG_READ_WRITE);
	if(ffmpeg_result != 0)
	{
		result.error_description = "avio_open() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	return result;

	fail:

	result.code = FFMPEG_RETURNED_NOT_SUCCESS;
	result.string = Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS);
	avio_close(util_encoder_format_context[session]->pb);
	avformat_free_context(util_encoder_format_context[session]);
	return result;
}

Result_with_string Util_init_audio_encoder(AVCodecID id, int original_samplerate, int encode_samplerate, int bitrate, int session)
{
	int ffmpeg_result = 0;
	Result_with_string result;

	util_audio_pos[session] = 0;
	util_audio_encoder_codec[session] = avcodec_find_encoder(id);
	if(!util_audio_encoder_codec[session])
	{
		result.error_description = "avcodec_find_encoder() failed";
		goto fail;
	}

	util_audio_encoder_context[session] = avcodec_alloc_context3(util_audio_encoder_codec[session]);
	if(!util_audio_encoder_context[session])
	{
		result.error_description = "avcodec_alloc_context3() failed";
		goto fail;
	}

	if(id == AV_CODEC_ID_MP2)
		util_audio_encoder_context[session]->sample_fmt = AV_SAMPLE_FMT_S16;
	else if(id == AV_CODEC_ID_ALAC || id == AV_CODEC_ID_FLAC)
		util_audio_encoder_context[session]->sample_fmt = AV_SAMPLE_FMT_S16P;
	else
		util_audio_encoder_context[session]->sample_fmt = AV_SAMPLE_FMT_FLT;
	
	util_audio_encoder_context[session]->bit_rate = bitrate;
	util_audio_encoder_context[session]->sample_rate = encode_samplerate;
	util_audio_encoder_context[session]->channel_layout = AV_CH_LAYOUT_MONO;
	util_audio_encoder_context[session]->channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_MONO);
	util_audio_encoder_context[session]->codec_type = AVMEDIA_TYPE_AUDIO;
	util_audio_encoder_context[session]->time_base = (AVRational){ 1, encode_samplerate };
	if(id == AV_CODEC_ID_AAC)
		util_audio_encoder_context[session]->profile = FF_PROFILE_AAC_LOW;

	ffmpeg_result = avcodec_open2(util_audio_encoder_context[session], util_audio_encoder_codec[session], NULL);
	if(ffmpeg_result != 0)
	{
		result.error_description = "avcodec_open2() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}
	
	util_audio_increase_pts[session] = util_audio_encoder_context[session]->frame_size;
	util_audio_encoder_packet[session] = av_packet_alloc();
	util_audio_encoder_raw_data[session] = av_frame_alloc();
	if(!util_audio_encoder_raw_data[session] || !util_audio_encoder_packet)
	{
		result.error_description = "av_packet_alloc() / av_frame_alloc() failed";
		goto fail;
	}
	
	util_audio_encoder_raw_data[session]->nb_samples = util_audio_encoder_context[session]->frame_size;
	util_audio_encoder_raw_data[session]->format = util_audio_encoder_context[session]->sample_fmt;
	util_audio_encoder_raw_data[session]->channel_layout = util_audio_encoder_context[session]->channel_layout;

	ffmpeg_result = av_frame_get_buffer(util_audio_encoder_raw_data[session], 0);
	if(ffmpeg_result != 0)
	{
		result.error_description = "av_frame_get_buffer() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	av_frame_make_writable(util_audio_encoder_raw_data[session]);

	util_audio_encoder_swr_context[session] = swr_alloc();
	swr_alloc_set_opts(util_audio_encoder_swr_context[session], av_get_default_channel_layout(util_audio_encoder_context[session]->channels), util_audio_encoder_context[session]->sample_fmt, util_audio_encoder_context[session]->sample_rate,
	av_get_default_channel_layout(util_audio_encoder_context[session]->channels), AV_SAMPLE_FMT_S16, original_samplerate, 0, NULL);
	if(!util_audio_encoder_swr_context[session])
	{
		result.error_description = "swr_alloc_set_opts() failed";
		goto fail;
	}

	ffmpeg_result = swr_init(util_audio_encoder_swr_context[session]);
	if(ffmpeg_result != 0)
	{
		result.error_description = "swr_init() failed";
		goto fail;
	}

	util_audio_encoder_stream[session] = avformat_new_stream(util_encoder_format_context[session], util_audio_encoder_codec[session]);
	if(!util_audio_encoder_stream[session])
	{
		result.error_description = "avformat_new_stream() failed";
		goto fail;
	}

	if (util_encoder_format_context[session]->oformat->flags & AVFMT_GLOBALHEADER)
		util_encoder_format_context[session]->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	ffmpeg_result = avcodec_parameters_from_context(util_audio_encoder_stream[session]->codecpar, util_audio_encoder_context[session]);
	if(ffmpeg_result != 0)
	{
		result.error_description = "avcodec_parameters_from_context() failed";
		goto fail;
	}

	return result;

	fail:
	Util_exit_audio_encoder(session);
	result.code = FFMPEG_RETURNED_NOT_SUCCESS;
	result.string = Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS);
	return result;
}

Result_with_string Util_init_video_encoder(AVCodecID id, int width, int height, int fps, int session)
{
	int ffmpeg_result = 0;
	Result_with_string result;

	util_video_pos[session] = 0;
	util_video_encoder_codec[session] = avcodec_find_encoder(id);
	if(!util_video_encoder_codec[session])
	{
		result.error_description = "avcodec_find_encoder() failed";
		goto fail;
	}

	util_video_encoder_context[session] = avcodec_alloc_context3(util_video_encoder_codec[session]);
	if(!util_video_encoder_context[session])
	{
		result.error_description = "avcodec_alloc_context3() failed";
		goto fail;
	}
	
	util_video_encoder_context[session]->bit_rate = 100000;
	util_video_encoder_context[session]->width = width;
	util_video_encoder_context[session]->height = height;
	util_video_encoder_context[session]->time_base = (AVRational){ 1, fps };
	util_video_encoder_context[session]->pix_fmt = AV_PIX_FMT_YUV420P;
	util_video_encoder_context[session]->gop_size = 30;
	//util_video_encoder_context[session]->flags2 = AV_CODEC_FLAG2_FAST;
	/*util_video_encoder_context[session]->flags |= AV_CODEC_FLAG_QSCALE;
	util_video_encoder_context[session]->global_quality = 31;
	util_video_encoder_context[session]->qmin = 1;
	util_video_encoder_context[session]->qmax = 1;
	util_video_encoder_context[session]->i_quant_factor = 0.1;
	util_video_encoder_context[session]->qcompress = 0.1;
	*/
	
	ffmpeg_result = avcodec_open2(util_video_encoder_context[session], util_video_encoder_codec[session], NULL);
	if(ffmpeg_result != 0)
	{
		result.error_description = "avcodec_open2() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}
	
	util_video_increase_pts[session] = 90000 / fps;

	util_video_encoder_packet[session] = av_packet_alloc();
	util_video_encoder_raw_data[session] = av_frame_alloc();
	if(!util_video_encoder_raw_data[session] || !util_video_encoder_packet[session])
	{
		result.error_description = "av_packet_alloc() / av_frame_alloc() failed";
		goto fail;
	}
	
	util_video_encoder_raw_data[session]->format = util_video_encoder_context[session]->pix_fmt;
	util_video_encoder_raw_data[session]->width = util_video_encoder_context[session]->width;
	util_video_encoder_raw_data[session]->height = util_video_encoder_context[session]->height;
	ffmpeg_result = av_frame_get_buffer(util_video_encoder_raw_data[session], 0);
	if(ffmpeg_result < 0)
	{
		result.error_description = "av_image_alloc() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	av_frame_make_writable(util_video_encoder_raw_data[session]);

	util_video_encoder_stream[session] = avformat_new_stream(util_encoder_format_context[session], util_video_encoder_codec[session]);
	if(!util_video_encoder_stream[session])
	{
		result.error_description = "avformat_new_stream() failed";
		goto fail;
	}
	
	if (util_encoder_format_context[session]->oformat->flags & AVFMT_GLOBALHEADER)
		util_encoder_format_context[session]->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	ffmpeg_result = avcodec_parameters_from_context(util_video_encoder_stream[session]->codecpar, util_video_encoder_context[session]);
	if(ffmpeg_result != 0)
	{
		result.error_description = "avcodec_parameters_from_context() failed";
		goto fail;
	}

	util_video_encoder_sws_context[session] = sws_getContext(util_video_encoder_context[session]->width, util_video_encoder_context[session]->height, AV_PIX_FMT_YUYV422,
	util_video_encoder_context[session]->width, util_video_encoder_context[session]->height, util_video_encoder_context[session]->pix_fmt, 0, 0, 0, 0);
	if(!util_video_encoder_sws_context[session])
	{
		result.error_description = "sws_getContext() failed";
		goto fail;
	}

	return result;

	fail:
	Util_exit_video_encoder(session);
	result.code = FFMPEG_RETURNED_NOT_SUCCESS;
	result.string = Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS);
	return result;
}

Result_with_string Util_write_header(int session)
{
	int ffmpeg_result = 0;
	Result_with_string result;

	ffmpeg_result = avformat_write_header(util_encoder_format_context[session], NULL);
	if(ffmpeg_result != 0)
	{
		result.code = FFMPEG_RETURNED_NOT_SUCCESS;
		result.string = Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS);
		result.error_description = "avformat_write_header() failed";
	}

	return result;
}

Result_with_string Util_encode_audio(int size, u8* raw_data, int session)
{
	int encode_offset = 0;
	int ffmpeg_result = 0;
	int one_frame_size = av_samples_get_buffer_size(NULL, util_audio_encoder_context[session]->channels, util_audio_encoder_context[session]->frame_size, util_audio_encoder_context[session]->sample_fmt, 0);
	int out_samples = 0;
	int bytes_per_sample = av_get_bytes_per_sample(util_audio_encoder_context[session]->sample_fmt);
	u8* swr_in_cache[1] = { NULL, };
	u8* swr_out_cache[1] = { NULL, };
	Result_with_string result;

	swr_in_cache[0] = raw_data;
	swr_out_cache[0] = (u8*)malloc(size * bytes_per_sample);
	if(swr_out_cache[0] == NULL)
		goto fail_;

	out_samples = swr_convert(util_audio_encoder_swr_context[session], (uint8_t**)swr_out_cache, size, (const uint8_t**)swr_in_cache, size);
	out_samples = out_samples / 2;
	out_samples *= bytes_per_sample;

	while(true)
	{
		memcpy(util_audio_encoder_raw_data[session]->data[0], swr_out_cache[0] + encode_offset, one_frame_size);
		//set pts
		util_audio_encoder_raw_data[session]->pts = util_audio_pos[session];
		util_audio_pos[session] += util_audio_increase_pts[session];

		ffmpeg_result = avcodec_send_frame(util_audio_encoder_context[session], util_audio_encoder_raw_data[session]);
		if(ffmpeg_result != 0)
		{
			result.error_description = "avcodec_send_frame() failed " + std::to_string(ffmpeg_result);
			goto fail;
		}

		ffmpeg_result = avcodec_receive_packet(util_audio_encoder_context[session], util_audio_encoder_packet[session]);
		if(ffmpeg_result == 0)
		{
			util_audio_encoder_packet[session]->stream_index = 0;
			ffmpeg_result = av_interleaved_write_frame(util_encoder_format_context[session], util_audio_encoder_packet[session]);
			av_packet_unref(util_audio_encoder_packet[session]);
			if(ffmpeg_result != 0)
			{
				result.error_description = "av_interleaved_write_frame() failed " + std::to_string(ffmpeg_result);
				goto fail;
			}
		}
		else
			av_packet_unref(util_audio_encoder_packet[session]);

		out_samples -= one_frame_size;
		encode_offset += one_frame_size;
		if(one_frame_size > out_samples)
			break;
	}
	free(swr_out_cache[0]);
	swr_out_cache[0] = NULL;

	return result;

	fail:

	result.code = FFMPEG_RETURNED_NOT_SUCCESS;
	result.string = Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS);
	return result;

	fail_:

	free(swr_out_cache[0]);
	swr_out_cache[0] = NULL;
	result.code = OUT_OF_MEMORY;
	result.string = Err_query_template_summary(OUT_OF_MEMORY);
	result.error_description = Err_query_template_detail(OUT_OF_MEMORY);
	return result;
}

Result_with_string Util_encode_video(u8* raw_data, int session)
{
	int ffmpeg_result = 0;
	int width = util_video_encoder_raw_data[session]->width;
	int height = util_video_encoder_raw_data[session]->height;
	int input_line_size[4] = { 0, 0, 0, 0, };
	u8* input[4] = { NULL, NULL, NULL, NULL, };
	Result_with_string result;

	input[0] = raw_data;
	input_line_size[0] = width*2;
	sws_scale(util_video_encoder_sws_context[session], input, input_line_size, 0, height, util_video_encoder_raw_data[session]->data, util_video_encoder_raw_data[session]->linesize);

	//set pts
	util_video_encoder_raw_data[session]->pts = util_video_pos[session];
	util_video_pos[session] += util_video_increase_pts[session];
	
	ffmpeg_result = avcodec_send_frame(util_video_encoder_context[session], util_video_encoder_raw_data[session]);
	if(ffmpeg_result != 0)
	{
		result.error_description = "avcodec_send_frame() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	ffmpeg_result = avcodec_receive_packet(util_video_encoder_context[session], util_video_encoder_packet[session]);
	if(ffmpeg_result == 0)
	{
		util_video_encoder_packet[session]->stream_index = 0;
		ffmpeg_result = av_interleaved_write_frame(util_encoder_format_context[session], util_video_encoder_packet[session]);
		av_packet_unref(util_video_encoder_packet[session]);
		if(ffmpeg_result != 0)
		{
			result.error_description = "av_interleaved_write_frame() failed " + std::to_string(ffmpeg_result);
			goto fail;
		}
	}
	else
		av_packet_unref(util_video_encoder_packet[session]);

	return result;

	fail:

	result.code = FFMPEG_RETURNED_NOT_SUCCESS;
	result.string = Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS);
	return result;
}

void Util_close_output_file(int session)
{
	av_write_trailer(util_encoder_format_context[session]);
	avio_close(util_encoder_format_context[session]->pb);
	avformat_free_context(util_encoder_format_context[session]);
}

void Util_exit_audio_encoder(int session)
{
	avcodec_free_context(&util_audio_encoder_context[session]);
	av_packet_free(&util_audio_encoder_packet[session]);
	av_frame_free(&util_audio_encoder_raw_data[session]);
	swr_free(&util_audio_encoder_swr_context[session]);	
}

void Util_exit_video_encoder(int session)
{
	avcodec_free_context(&util_video_encoder_context[session]);
	av_packet_free(&util_video_encoder_packet[session]);
	av_frame_free(&util_video_encoder_raw_data[session]);
	sws_freeContext(util_video_encoder_sws_context[session]);
}

Result_with_string Util_open_file(std::string file_path, bool* has_audio, bool* has_video, int session)
{
	int ffmpeg_result = 0;
	Result_with_string result;
	*has_audio = false;
	*has_video = false;

	util_decoder_format_context[session] = avformat_alloc_context();
	ffmpeg_result = avformat_open_input(&util_decoder_format_context[session], file_path.c_str(), NULL, NULL);
	if(ffmpeg_result != 0)
	{
		result.error_description = "avformat_open_input() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	ffmpeg_result = avformat_find_stream_info(util_decoder_format_context[session], NULL);
	if(util_decoder_format_context[session] == NULL)
	{
		result.error_description = "avformat_find_stream_info() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	util_audio_decoder_stream_num[session] = -1;
	for(int i = 0; i < (int)util_decoder_format_context[session]->nb_streams; i++)
	{
		if(util_decoder_format_context[session]->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			*has_audio = true;
			util_audio_decoder_stream_num[session] = i;
		}
		else if(util_decoder_format_context[session]->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			*has_video = true;
			util_video_decoder_stream_num[session] = i;
		}
	}

	if(util_audio_decoder_stream_num[session] == -1 && util_video_decoder_stream_num[session] == -1)
	{
		result.error_description = "No audio and video data";
		goto fail;
	}
	return result;

	fail:

	Util_exit_audio_decoder(session);
	result.code = FFMPEG_RETURNED_NOT_SUCCESS;
	result.string = Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS);
	return result;
}

Result_with_string Util_init_audio_decoder(int session)
{
	int ffmpeg_result = 0;
	Result_with_string result;

	util_audio_decoder_codec[session] = avcodec_find_decoder(util_decoder_format_context[session]->streams[util_audio_decoder_stream_num[session]]->codecpar->codec_id);
	if(!util_audio_decoder_codec[session])
	{
		result.error_description = "avcodec_find_decoder() failed";
		goto fail;
	}

	util_audio_decoder_context[session] = avcodec_alloc_context3(util_audio_decoder_codec[session]);
	if(!util_audio_decoder_context[session])
	{
		result.error_description = "avcodec_alloc_context3() failed";
		goto fail;
	}

	ffmpeg_result = avcodec_parameters_to_context(util_audio_decoder_context[session], util_decoder_format_context[session]->streams[util_audio_decoder_stream_num[session]]->codecpar);
	if(ffmpeg_result != 0)
	{
		result.error_description = "avcodec_parameters_to_context() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	ffmpeg_result = avcodec_open2(util_audio_decoder_context[session], util_audio_decoder_codec[session], NULL);
	if(ffmpeg_result != 0)
	{
		result.error_description = "avcodec_open2() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	util_audio_decoder_swr_context[session] = swr_alloc();
	swr_alloc_set_opts(util_audio_decoder_swr_context[session], av_get_default_channel_layout(util_audio_decoder_context[session]->channels), AV_SAMPLE_FMT_S16, util_audio_decoder_context[session]->sample_rate,
		av_get_default_channel_layout(util_audio_decoder_context[session]->channels), util_audio_decoder_context[session]->sample_fmt, util_audio_decoder_context[session]->sample_rate, 0, NULL);
	if(!util_audio_decoder_swr_context[session])
	{
		result.error_description = "swr_alloc_set_opts() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	ffmpeg_result = swr_init(util_audio_decoder_swr_context[session]);
	if(ffmpeg_result != 0)
	{
		result.error_description = "swr_init() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	return result;

	fail:

	Util_exit_audio_decoder(session);
	result.code = FFMPEG_RETURNED_NOT_SUCCESS;
	result.string = Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS);
	return result;
}

Result_with_string Util_init_video_decoder(int low_resolution, int session)
{
	int ffmpeg_result = 0;
	Result_with_string result;

	util_video_decoder_codec[session] = avcodec_find_decoder(util_decoder_format_context[session]->streams[util_video_decoder_stream_num[session]]->codecpar->codec_id);
	if(!util_video_decoder_codec[session])
	{
		result.error_description = "avcodec_find_decoder() failed";
		goto fail;
	}

	util_video_decoder_context[session] = avcodec_alloc_context3(util_video_decoder_codec[session]);
	if(!util_video_decoder_context[session])
	{
		result.error_description = "avcodec_alloc_context3() failed";
		goto fail;
	}

	ffmpeg_result = avcodec_parameters_to_context(util_video_decoder_context[session], util_decoder_format_context[session]->streams[util_video_decoder_stream_num[session]]->codecpar);
	if(ffmpeg_result != 0)
	{
		result.error_description = "avcodec_parameters_to_context() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	util_video_decoder_context[session]->lowres = low_resolution;
	ffmpeg_result = avcodec_open2(util_video_decoder_context[session], util_video_decoder_codec[session], NULL);
	if(ffmpeg_result != 0)
	{
		result.error_description = "avcodec_open2() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	return result;

	fail:

	Util_exit_video_decoder(session);
	result.code = FFMPEG_RETURNED_NOT_SUCCESS;
	result.string = Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS);
	return result;
}

void Util_get_audio_info(int* bitrate, int* samplerate, int* ch, std::string* format_name, double* duration, int session)
{
	*bitrate = util_audio_decoder_context[session]->bit_rate;
	*samplerate = util_audio_decoder_context[session]->sample_rate;
	*ch = util_audio_decoder_context[session]->channels;
	*format_name = util_audio_decoder_codec[session]->name;
	*duration = (double)util_decoder_format_context[session]->duration / AV_TIME_BASE;
}

void Util_get_video_info(int* width, int* height, double* framerate, std::string* format_name, double* duration, int session)
{
	*width = util_video_decoder_context[session]->width;
	*height = util_video_decoder_context[session]->height;
	*framerate = (double)util_decoder_format_context[session]->streams[util_video_decoder_stream_num[session]]->avg_frame_rate.num / util_decoder_format_context[session]->streams[util_video_decoder_stream_num[session]]->avg_frame_rate.den;
	*format_name = util_video_decoder_codec[session]->name;
	*duration = (double)util_decoder_format_context[session]->duration / AV_TIME_BASE;
}

Result_with_string Util_read_packet(AVMediaType* type, int session)
{
	Result_with_string result;
	int ffmpeg_result = 0;
	AVPacket* cache_packet = NULL;
	*type = AVMEDIA_TYPE_UNKNOWN;

	cache_packet = av_packet_alloc();
	if(!cache_packet)
	{
		result.error_description = "av_packet_alloc() failed";
		goto fail;
	}

	ffmpeg_result = av_read_frame(util_decoder_format_context[session], cache_packet);
	if(ffmpeg_result != 0)
	{
		result.error_description = "av_read_frame() failed";
		goto fail;
	}

	if(cache_packet->stream_index == util_audio_decoder_stream_num[session])//audio packet
	{
		util_audio_decoder_cache_packet[session] = av_packet_alloc();
		if(!util_audio_decoder_cache_packet[session])
		{
			result.error_description = "av_packet_alloc() failed" + std::to_string(ffmpeg_result);
			goto fail;
		}

		av_packet_unref(util_audio_decoder_cache_packet[session]);
		ffmpeg_result = av_packet_ref(util_audio_decoder_cache_packet[session], cache_packet);
		if(ffmpeg_result != 0)
		{
			result.error_description = "av_packet_ref() failed" + std::to_string(ffmpeg_result);
			goto fail;
		}
		*type = AVMEDIA_TYPE_AUDIO;
	}
	else if(cache_packet->stream_index == util_video_decoder_stream_num[session])//video packet
	{
		util_video_decoder_cache_packet[session] = av_packet_alloc();
		if(!util_video_decoder_cache_packet[session])
		{
			result.error_description = "av_packet_alloc() failed";
			goto fail;
		}

		av_packet_unref(util_video_decoder_cache_packet[session]);
		ffmpeg_result = av_packet_ref(util_video_decoder_cache_packet[session], cache_packet);
		if(ffmpeg_result != 0)
		{
			result.error_description = "av_packet_ref() failed" + std::to_string(ffmpeg_result);
			goto fail;
		}
		*type = AVMEDIA_TYPE_VIDEO;
	}

	av_packet_free(&cache_packet);
	return result;

	fail:

	av_packet_free(&util_audio_decoder_cache_packet[session]);
	av_packet_free(&util_video_decoder_cache_packet[session]);
	av_packet_free(&cache_packet);
	result.code = FFMPEG_RETURNED_NOT_SUCCESS;
	result.string = Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS);
	return result;
}

Result_with_string Util_ready_audio_packet(int session)
{
	Result_with_string result;
	int ffmpeg_result = 0;

	av_packet_free(&util_audio_decoder_packet[session]);
	util_audio_decoder_packet[session] = av_packet_alloc();
	if(!util_audio_decoder_packet[session])
	{
		result.error_description = "av_packet_alloc() failed";
		goto fail;
	}

	av_packet_unref(util_audio_decoder_packet[session]);
	ffmpeg_result = av_packet_ref(util_audio_decoder_packet[session], util_audio_decoder_cache_packet[session]);
	if(ffmpeg_result != 0)
	{
		result.error_description = "av_packet_ref() failed" + std::to_string(ffmpeg_result);
		goto fail;
	}

	av_packet_free(&util_audio_decoder_cache_packet[session]);
	return result;

	fail:

	av_packet_free(&util_audio_decoder_packet[session]);
	result.code = FFMPEG_RETURNED_NOT_SUCCESS;
	result.string = Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS);
	return result;
}

Result_with_string Util_ready_video_packet(int session)
{
	Result_with_string result;
	int ffmpeg_result = 0;

	av_packet_free(&util_video_decoder_packet[session]);
	util_video_decoder_packet[session] = av_packet_alloc();
	if(!util_video_decoder_packet[session])
	{
		result.error_description = "av_packet_alloc() failed";
		goto fail;
	}

	av_packet_unref(util_video_decoder_packet[session]);
	ffmpeg_result = av_packet_ref(util_video_decoder_packet[session], util_video_decoder_cache_packet[session]);
	if(ffmpeg_result != 0)
	{
		result.error_description = "av_packet_ref() failed" + std::to_string(ffmpeg_result);
		goto fail;
	}

	av_packet_free(&util_video_decoder_cache_packet[session]);
	return result;

	fail:

	av_packet_free(&util_video_decoder_packet[session]);
	result.code = FFMPEG_RETURNED_NOT_SUCCESS;
	result.string = Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS);
	return result;
}

void Util_skip_audio_packet(int session)
{
	av_packet_free(&util_audio_decoder_cache_packet[session]);
}

void Util_skip_video_packet(int session)
{
	av_packet_free(&util_video_decoder_cache_packet[session]);
}

Result_with_string Util_decode_audio(int* size, u8** raw_data, int session)
{
	int ffmpeg_result = 0;
	Result_with_string result;
	*size = 0;

	util_audio_decoder_raw_data[session] = av_frame_alloc();
	if(!util_audio_decoder_raw_data[session])
	{
		result.error_description = "av_frame_alloc() failed";
		goto fail;
	}
	
	ffmpeg_result = avcodec_send_packet(util_audio_decoder_context[session], util_audio_decoder_packet[session]);
	if(ffmpeg_result == 0)
	{
		ffmpeg_result = avcodec_receive_frame(util_audio_decoder_context[session], util_audio_decoder_raw_data[session]);
		if(ffmpeg_result == 0)
		{
			*raw_data = (u8*)malloc(util_audio_decoder_raw_data[session]->nb_samples * 2 * util_audio_decoder_context[session]->channels);
			*size = swr_convert(util_audio_decoder_swr_context[session], raw_data, util_audio_decoder_raw_data[session]->nb_samples, (const uint8_t**)util_audio_decoder_raw_data[session]->data, util_audio_decoder_raw_data[session]->nb_samples);
			*size *= 2;
		}
		else
		{
			result.error_description = "avcodec_receive_frame() failed " + std::to_string(ffmpeg_result);
			goto fail;
		}
	}
	else
	{
		result.error_description = "avcodec_send_packet() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	av_packet_free(&util_audio_decoder_packet[session]);
	av_frame_free(&util_audio_decoder_raw_data[session]);
	return result;

	fail:

	av_packet_free(&util_audio_decoder_packet[session]);
	av_frame_free(&util_audio_decoder_raw_data[session]);
	result.code = FFMPEG_RETURNED_NOT_SUCCESS;
	result.string = Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS);
	return result;
}

Result_with_string Util_decode_video(int* width, int* height, bool* key_frame, double* current_pos, int session)
{
	int ffmpeg_result = 0;
	int count = 0;
	double framerate = (double)util_decoder_format_context[session]->streams[util_video_decoder_stream_num[session]]->avg_frame_rate.num / util_decoder_format_context[session]->streams[util_video_decoder_stream_num[session]]->avg_frame_rate.den;
	double current_frame = (double)util_video_decoder_packet[session]->dts / util_video_decoder_packet[session]->duration;
	Result_with_string result;
	*width = 0;
	*height = 0;
	*current_pos = 0;
	if(framerate != 0.0)
		*current_pos = current_frame * (1000 / framerate);//calc frame pos
	//Log_log_save("", std::to_string(framerate) + " " + std::to_string(current_frame), 1234567890, false);

	if(util_video_decoder_packet[session]->flags == 1)
		*key_frame = true;
	else
		*key_frame = false;
	
	util_video_decoder_raw_data[session][util_video_decoder_buffer_num[session]] = av_frame_alloc();
	if(!util_video_decoder_raw_data[session][util_video_decoder_buffer_num[session]])
	{
		result.error_description = "av_frame_alloc() failed";
		goto fail;
	}

	ffmpeg_result = avcodec_send_packet(util_video_decoder_context[session], util_video_decoder_packet[session]);
	if(ffmpeg_result == 0)
	{
		ffmpeg_result = avcodec_receive_frame(util_video_decoder_context[session], util_video_decoder_raw_data[session][util_video_decoder_buffer_num[session]]);
		if(ffmpeg_result == 0)
		{
			*width = util_video_decoder_raw_data[session][util_video_decoder_buffer_num[session]]->width;
			*height = util_video_decoder_raw_data[session][util_video_decoder_buffer_num[session]]->height;
		}
		else
		{
			result.error_description = "avcodec_receive_frame() failed " + std::to_string(ffmpeg_result);
			goto fail;
		}
	}
	else
	{
		result.error_description = "avcodec_send_packet() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	if(util_video_decoder_buffer_num[session] == 0)
		util_video_decoder_buffer_num[session] = 1;
	else if(util_video_decoder_buffer_num[session] == 1)
		util_video_decoder_buffer_num[session] = 2;
	else
		util_video_decoder_buffer_num[session] = 0;
	
	while(util_video_decoder_lock[session][util_video_decoder_buffer_num[session]])
	{
		count++;
		if(count > 40000)//time out 4000ms
			break;
	
		usleep(100);
	}

	if(util_video_decoder_buffer_num[session] == 0)
		util_video_decoder_ready_buffer_num[session] = 2;
	else if(util_video_decoder_buffer_num[session] == 1)
		util_video_decoder_ready_buffer_num[session] = 0;
	else
		util_video_decoder_ready_buffer_num[session] = 1;

	av_packet_free(&util_video_decoder_packet[session]);
	av_frame_free(&util_video_decoder_raw_data[session][util_video_decoder_buffer_num[session]]);
	return result;

	fail:

	av_packet_free(&util_video_decoder_packet[session]);
	av_frame_free(&util_video_decoder_raw_data[session][util_video_decoder_buffer_num[session]]);
	result.code = FFMPEG_RETURNED_NOT_SUCCESS;
	result.string = Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS);
	return result;
}

void Util_get_video_image(u8* raw_data, int width, int height, int session)
{
	int cpy_size[2] = { 0, 0, };
	int buffer_num = util_video_decoder_ready_buffer_num[session];
	util_video_decoder_lock[session][buffer_num] = true;//lock

	cpy_size[0] = (width * height);
	cpy_size[1] = cpy_size[0] / 4;
	cpy_size[0] -= cpy_size[0] % 32;
	cpy_size[1] -= cpy_size[1] % 32;
	memcpy_asm(raw_data, util_video_decoder_raw_data[session][buffer_num]->data[0], cpy_size[0]);
	memcpy_asm(raw_data + (width * height), util_video_decoder_raw_data[session][buffer_num]->data[1], cpy_size[1]);
	memcpy_asm(raw_data + (width * height) + (width * height / 4), util_video_decoder_raw_data[session][buffer_num]->data[2], cpy_size[1]);

	util_video_decoder_lock[session][buffer_num] = false;//unlock
}

Result_with_string Util_seek(u64 seek_pos, int flag, int session)
{
	int ffmpeg_result;
	Result_with_string result;

	ffmpeg_result = avformat_seek_file(util_decoder_format_context[session], -1, seek_pos, seek_pos, seek_pos, flag);//AVSEEK_FLAG_FRAME 8 AVSEEK_FLAG_ANY 4  AVSEEK_FLAG_BACKWORD 1
	if(ffmpeg_result < 0)
	{
		result.code = FFMPEG_RETURNED_NOT_SUCCESS;
		result.string = Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS);
		result.error_description = "avformat_seek_file() failed " + std::to_string(ffmpeg_result);
	}

	return result;
}

void Util_close_file(int session)
{
	avformat_close_input(&util_decoder_format_context[session]);
}

void Util_exit_audio_decoder(int session)
{
	avcodec_free_context(&util_audio_decoder_context[session]);
	av_packet_free(&util_audio_decoder_packet[session]);
	av_packet_free(&util_audio_decoder_cache_packet[session]);
	av_frame_free(&util_audio_decoder_raw_data[session]);
	swr_free(&util_audio_decoder_swr_context[session]);
}

void Util_exit_video_decoder(int session)
{
	avcodec_free_context(&util_video_decoder_context[session]);
	av_packet_free(&util_video_decoder_packet[session]);
	av_packet_free(&util_video_decoder_cache_packet[session]);
	av_frame_free(&util_video_decoder_raw_data[session][0]);
	av_frame_free(&util_video_decoder_raw_data[session][1]);
	av_frame_free(&util_video_decoder_raw_data[session][2]);
}

Result_with_string Util_mux_file(std::string file_name, int session)
{
	Result_with_string result;
	int ffmpeg_result = 0;

	util_muxer_format_context[session] = avformat_alloc_context();
	if(!util_muxer_format_context[session])
	{
		result.error_description = "avformat_alloc_context() failed";
		goto fail;
	}

	util_muxer_format_context[session]->oformat = av_guess_format(NULL, file_name.c_str(), NULL);
	if(!util_muxer_format_context[session]->oformat)
	{
		result.error_description = "av_guess_format() failed";
		goto fail;
	}

	ffmpeg_result = avio_open(&util_muxer_format_context[session]->pb, file_name.c_str(), AVIO_FLAG_READ_WRITE);
	if(ffmpeg_result != 0)
	{
		result.error_description = "avio_open() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	//setup for audio
	util_audio_muxer_codec[session] = avcodec_find_encoder(util_audio_muxer_format_context[session]->streams[util_audio_muxer_stream_num[session]]->codecpar->codec_id);
	if(!util_audio_muxer_codec[session])
	{
		result.error_description = "avcodec_find_encoder() failed";
		goto fail;
	}
	
	util_audio_muxer_context[session] = avcodec_alloc_context3(util_audio_muxer_codec[session]);
	if(!util_audio_muxer_context[session])
	{
		result.error_description = "avcodec_alloc_context3() failed";
		goto fail;
	}

	ffmpeg_result = avcodec_parameters_to_context(util_audio_muxer_context[session], util_audio_muxer_format_context[session]->streams[util_audio_muxer_stream_num[session]]->codecpar);
	if(ffmpeg_result != 0)
	{
		result.error_description = "avcodec_parameters_to_context() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	util_audio_muxer_format_stream[session] = avformat_new_stream(util_muxer_format_context[session], NULL);
	if(!util_audio_muxer_format_stream[session])
	{
		result.error_description = "avformat_new_stream() failed";
		goto fail;
	}

	ffmpeg_result = avcodec_parameters_from_context(util_audio_muxer_format_stream[session]->codecpar, util_audio_muxer_context[session]);
	if(ffmpeg_result != 0)
	{
		result.error_description = "avcodec_parameters_from_context() failed";
		goto fail;
	}

	//setup for video
	util_video_muxer_codec[session] = avcodec_find_encoder(util_video_muxer_format_context[session]->streams[util_video_muxer_stream_num[session]]->codecpar->codec_id);
	if(!util_video_muxer_codec[session])
	{
		result.error_description = "avcodec_find_encoder() failed";
		goto fail;
	}

	util_video_muxer_context[session] = avcodec_alloc_context3(util_video_muxer_codec[session]);
	if(!util_video_muxer_context[session])
	{
		result.error_description = "avcodec_alloc_context3() failed";
		goto fail;
	}

	ffmpeg_result = avcodec_parameters_to_context(util_video_muxer_context[session], util_video_muxer_format_context[session]->streams[util_video_muxer_stream_num[session]]->codecpar);
	if(ffmpeg_result != 0)
	{
		result.error_description = "avcodec_parameters_to_context() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	util_video_muxer_format_stream[session] = avformat_new_stream(util_muxer_format_context[session], NULL);
	if(!util_video_muxer_format_stream[session])
	{
		result.error_description = "avformat_new_stream() failed";
		goto fail;
	}

	ffmpeg_result = avcodec_parameters_from_context(util_video_muxer_format_stream[session]->codecpar, util_video_muxer_context[session]);
	if(ffmpeg_result != 0)
	{
		result.error_description = "avcodec_parameters_from_context() failed";
		goto fail;
	}

	if (util_muxer_format_context[session]->oformat->flags & AVFMT_GLOBALHEADER)
		util_muxer_format_context[session]->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	ffmpeg_result = avformat_write_header(util_muxer_format_context[session], NULL);
	if(ffmpeg_result != 0)
	{
		result.error_description = "avformat_write_header() failed";
		goto fail;
	}
	
	util_audio_muxer_packet[session] = av_packet_alloc();
	if(!util_audio_muxer_packet[session])
	{
		result.error_description = "av_packet_alloc() failed";
		goto fail;
	}

	while(true)//mux audio
	{
		ffmpeg_result = av_read_frame(util_audio_muxer_format_context[session], util_audio_muxer_packet[session]);
		if(ffmpeg_result != 0)
			break;
		
		if(util_audio_muxer_stream_num[session] == util_audio_muxer_packet[session]->stream_index)
		{
			util_audio_muxer_packet[session]->stream_index = 0;
			ffmpeg_result = av_interleaved_write_frame(util_muxer_format_context[session], util_audio_muxer_packet[session]);
			if(ffmpeg_result != 0)
			{
				result.error_description = "av_interleaved_write_frame() failed " + std::to_string(ffmpeg_result);
				goto fail;
			}
		}
	}
	av_packet_free(&util_audio_muxer_packet[session]);

	util_video_muxer_packet[session] = av_packet_alloc();
	if(!util_video_muxer_packet[session])
	{
		result.error_description = "av_packet_alloc() failed";
		goto fail;
	}

	while(true)//mux video
	{
		ffmpeg_result = av_read_frame(util_video_muxer_format_context[session], util_video_muxer_packet[session]);
		if(ffmpeg_result != 0)
			break;
		
		if(util_video_muxer_stream_num[session] == util_video_muxer_packet[session]->stream_index)
		{
			util_video_muxer_packet[session]->stream_index = 1;
			ffmpeg_result = av_interleaved_write_frame(util_muxer_format_context[session], util_video_muxer_packet[session]);
			if(ffmpeg_result != 0)
			{
				result.error_description = "av_interleaved_write_frame() failed " + std::to_string(ffmpeg_result);
				goto fail;
			}
		}
	}
	av_packet_free(&util_video_muxer_packet[session]);

	av_write_trailer(util_muxer_format_context[session]);
	avio_close(util_muxer_format_context[session]->pb);
	avformat_free_context(util_muxer_format_context[session]);

	return result;

	fail:

	result.code = FFMPEG_RETURNED_NOT_SUCCESS;
	result.string = Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS);
	avio_close(util_muxer_format_context[session]->pb);
	avformat_free_context(util_muxer_format_context[session]);
	return result;
}

Result_with_string Util_open_muxer_audio_file(std::string file_path, int session)
{
	int ffmpeg_result = 0;
	Result_with_string result;

	util_audio_muxer_format_context[session] = avformat_alloc_context();
	ffmpeg_result = avformat_open_input(&util_audio_muxer_format_context[session], file_path.c_str(), NULL, NULL);
	if(ffmpeg_result != 0)
	{
		result.error_description = "avformat_open_input() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	ffmpeg_result = avformat_find_stream_info(util_audio_muxer_format_context[session], NULL);
	if(util_audio_muxer_format_context[session] == NULL)
	{
		result.error_description = "avformat_find_stream_info() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	util_audio_muxer_stream_num[session] = -1;
	for(int i = 0; i < (int)util_audio_muxer_format_context[session]->nb_streams; i++)
	{
		if(util_audio_muxer_format_context[session]->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
			util_audio_muxer_stream_num[session] = i;
	}

	if(util_audio_muxer_stream_num[session] == -1)
	{
		result.error_description = "No audio data";
		goto fail;
	}
	return result;

	fail:

	Util_close_muxer_audio_file(session);
	result.code = FFMPEG_RETURNED_NOT_SUCCESS;
	result.string = Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS);
	return result;
}

Result_with_string Util_open_muxer_video_file(std::string file_path, int session)
{
	int ffmpeg_result = 0;
	Result_with_string result;

	util_video_muxer_format_context[session] = avformat_alloc_context();
	ffmpeg_result = avformat_open_input(&util_video_muxer_format_context[session], file_path.c_str(), NULL, NULL);
	if(ffmpeg_result != 0)
	{
		result.error_description = "avformat_open_input() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	ffmpeg_result = avformat_find_stream_info(util_video_muxer_format_context[session], NULL);
	if(util_video_muxer_format_context[session] == NULL)
	{
		result.error_description = "avformat_find_stream_info() failed " + std::to_string(ffmpeg_result);
		goto fail;
	}

	util_video_muxer_stream_num[session] = -1;
	for(int i = 0; i < (int)util_video_muxer_format_context[session]->nb_streams; i++)
	{
		if(util_video_muxer_format_context[session]->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
			util_video_muxer_stream_num[session] = i;
	}

	if(util_video_muxer_stream_num[session] == -1)
	{
		result.error_description = "No video data";
		goto fail;
	}
	return result;

	fail:

	Util_close_muxer_video_file(session);
	result.code = FFMPEG_RETURNED_NOT_SUCCESS;
	result.string = Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS);
	return result;
}

void Util_close_muxer_audio_file(int session)
{
	avformat_close_input(&util_audio_muxer_format_context[session]);
}

void Util_close_muxer_video_file(int session)
{
	avformat_close_input(&util_video_muxer_format_context[session]);
}

Result_with_string Util_init_yuv422_rgb565_converter(int width, int height, int session)
{
	Result_with_string result;

	util_yuv422_rgb565_width[session] = width;
	util_yuv422_rgb565_height[session] = height;
	util_yuv422_rgb565_converter_sws_context[session] = sws_getContext(width, height, AV_PIX_FMT_YUYV422,
	width, height, AV_PIX_FMT_RGB565LE, 0, 0, 0, 0);
	if(!util_yuv422_rgb565_converter_sws_context[session])
	{
		result.error_description = "sws_getContext() failed";
		goto fail;
	}

	return result;

	fail:
	Util_exit_yuv422_rgb565_converter(session);
	result.code = FFMPEG_RETURNED_NOT_SUCCESS;
	result.string = Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS);
	return result;
}

void Util_convert_yuv422_rgb565(u8* yuv422_raw, u8* rgb565_raw, int session)
{
	int src_line_size[4] = { 0, 0, 0, 0, };
	int dst_line_size[4] = { 0, 0, 0, 0, };
	u8* src_data[4] = { NULL, NULL, NULL, NULL, };
	u8* dst_data[4] = { NULL, NULL, NULL, NULL, };

	src_data[0] = yuv422_raw;
	src_line_size[0] = util_yuv422_rgb565_width[session] * 2;
	dst_data[0] = rgb565_raw;
	dst_line_size[0] = util_yuv422_rgb565_width[session] * 2;
	
	sws_scale(util_yuv422_rgb565_converter_sws_context[session], src_data, src_line_size, 0, 
	util_yuv422_rgb565_height[session], dst_data, dst_line_size);
}

void Util_exit_yuv422_rgb565_converter(int session)
{
	sws_freeContext(util_yuv422_rgb565_converter_sws_context[session]);
}
