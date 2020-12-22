#include <3ds.h>
#include <unistd.h>
#include <string>
#include <malloc.h>
#include "citro2d.h"

#include "hid.hpp"
#include "httpc.hpp"
#include "draw.hpp"
#include "file.hpp"
#include "log.hpp"
#include "error.hpp"
#include "menu.hpp"
#include "types.hpp"
#include "setting_menu.hpp"
#include "video_player.hpp"
#include "explorer.hpp"
#include "music_player.hpp"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
}

/*For draw*/
bool vid_need_reflesh = false;
int vid_pre_dl_progress = 0;
int vid_pre_dled_size = 0;
/*---------------------------------------------*/

bool vid_main_run = false;
bool vid_decord_video_thread_run = false;
bool vid_convert_thread_run = false;
bool vid_worker_thread_run = false;
bool vid_timer_thread_run = false;
bool vid_already_init = false;
bool vid_thread_suspend = true;
bool vid_decode_request = false;
bool vid_stop_request = false;
bool vid_decode_video_request = false;
bool vid_pause_request = false;
bool vid_change_video_request = false;
bool vid_convert_texture_request = false;
bool vid_select_file_request = false;
bool vid_seek_request = false;
bool vid_get_info_request = false;
bool vid_sws_init_request = false;
bool vid_wait = false;
bool vid_decoding = false;
bool vid_converting = false;
bool vid_down_sampling = false;
bool vid_strong_down_sampling = false;
bool vid_allow_skip_frame = false;
bool vid_scroll_mode = false;
bool vid_dl_and_decode_request = false;
bool vid_count_request = false;
bool vid_count_reset_request = false;
bool vid_enable[16];
bool vid_button_selected[2] = { false, false, };
u64 vid_offset = 0;
int vid_buffer_num = 0;
int vid_dl_progress = 0;
int vid_dled_size = 0;
int vid_pic_num = 0;
int vid_video_width = 0;
int vid_video_height = 0;
int vid_video_stream_num = -1;
int vid_audio_stream_num = -1;
int vid_read_fps = 0;
int vid_decode_fps = 0;
int vid_convert_fps = 0;
int vid_temp_read_fps = 0;
int vid_temp_decode_fps = 0;
int vid_temp_convert_fps = 0;
int vid_frames = 0;
int vid_decoded_frames = 0;
int vid_menu_mode = 0;
double vid_bit_rate = 0.0;
double vid_video_length = 0.0;
double vid_bar_pos = 0.0;
double vid_touch_x_move_left = 0.0;
double vid_touch_y_move_left = 0.0;
double vid_framerate = 0.0;
double vid_zoom = 0.0;
double vid_x = 0.0;
double vid_y = 0.0;
std::string vid_url = "";
std::string vid_current_video_format = "none";
std::string vid_current_audio_format = "none";
std::string vid_load_file_name = "video.mp4";
std::string vid_load_dir_name = "/Line/";
std::string vid_msg[VID_NUM_OF_MSG];
std::string vid_ver = "v1.0.1";
std::string vid_timer_thread_string = "Vid/Timer thread";
std::string vid_decord_video_thread_string = "Vid/Decode video thread";
std::string vid_convert_thread_string = "Vid/Convert thread";
std::string vid_worker_thread_string = "Vid/Worker thread";
std::string vid_init_string = "Vid/Init";
std::string vid_exit_string = "Vid/Exit";
Thread vid_decode_video_thread, vid_convert_thread, vid_worker_thread, vid_timer_thread;

C2D_Image vid_c2d_image[32];
u8* vid_bgr_data[2] = { NULL, NULL, };
u8* yuv_data[2] = { NULL, NULL, };
AVCodecContext *context[2] = { NULL, NULL, };
AVCodec *codec[2] = { NULL, NULL, };
AVPacket *vid_packet = NULL;
AVFrame *vid_raw_data[2] = { NULL, NULL, };

bool Vid_query_init_flag(void)
{
	return vid_already_init;
}

bool Vid_query_running_flag(void)
{
	return vid_main_run;
}

void Vid_set_msg(int msg_num, std::string msg)
{
	if (msg_num >= 0 && msg_num <= VID_NUM_OF_MSG)
		vid_msg[msg_num] = msg;
}

void Vid_resume(void)
{
	if(!vid_dl_and_decode_request)
		APT_SetAppCpuTimeLimit(80);

	vid_thread_suspend = false;
	vid_main_run = true;
	vid_need_reflesh = true;
	Menu_suspend();
}

void Vid_suspend(void)
{
	if(!vid_decode_request)
		APT_SetAppCpuTimeLimit(30);

	vid_thread_suspend = true;
	vid_main_run = false;
	Menu_resume();
}

void Vid_set_url(std::string url)
{
	vid_url = url;
	vid_dl_and_decode_request = true;
}

void Vid_set_load_file(std::string file_name, std::string dir_name)
{
	vid_load_file_name = file_name;
	vid_load_dir_name = dir_name;
	vid_select_file_request = false;
	vid_change_video_request = true;
	vid_decode_request = true;
}

void Vid_reset_fps(void)
{
	vid_read_fps = vid_temp_read_fps;
	vid_decode_fps = vid_temp_decode_fps;
	vid_convert_fps = vid_temp_convert_fps;
	vid_temp_read_fps = 0;
	vid_temp_decode_fps = 0;
	vid_temp_convert_fps = 0;
}

void Vid_timer_thread(void* arg)
{
	Log_log_save(vid_timer_thread_string, "Thread started.", 1234567890, false);
	TickCounter elapsed_time;
	osTickCounterStart(&elapsed_time);

	while (vid_timer_thread_run)
	{
		if (vid_count_reset_request)
		{
			vid_count_reset_request = false;
			osTickCounterUpdate(&elapsed_time);
			vid_bar_pos = 0.0;
		}

		if (vid_count_request)
		{
			osTickCounterUpdate(&elapsed_time);
			while (vid_count_request)
			{
				if (vid_count_reset_request)
					break;

				usleep(100000);
				osTickCounterUpdate(&elapsed_time);
				vid_bar_pos += osTickCounterRead(&elapsed_time);
				if(vid_bar_pos / 1000 > vid_video_length)
					vid_bar_pos = vid_video_length * 1000;
			}
		}
		else
			usleep(ACTIVE_THREAD_SLEEP_TIME);

		while (vid_thread_suspend && !vid_count_request && !vid_count_reset_request)
			usleep(INACTIVE_THREAD_SLEEP_TIME);
	}

	Log_log_save(vid_timer_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Vid_decode_video_thread(void* arg)
{
	Log_log_save(vid_decord_video_thread_string, "Thread started.", 1234567890, false);
	int ffmpeg_result = 0;
	int buffer_num = 0;
	int size = 0;
	Result_with_string result;
	AVPacket *packet = NULL;

	while (vid_decord_video_thread_run)
	{
		while(vid_decode_request)
		{
			if(vid_decode_video_request)
			{
				vid_decode_video_request = false;
				vid_decoding = true;
				vid_decoded_frames++;
				packet = av_packet_alloc();
				vid_raw_data[buffer_num] = av_frame_alloc();

				if(!packet || !vid_raw_data[buffer_num])
					Log_log_save(vid_decord_video_thread_string, "alloc failed ", -1, false);
				else
				{
					av_init_packet(packet);
					while (vid_wait)
						usleep(100);

					av_packet_ref(packet, vid_packet);
					ffmpeg_result = avcodec_send_packet(context[0], packet);
					if(ffmpeg_result != 0)
						Log_log_save(vid_worker_thread_string, "avcodec_send_packet()...[Error] ", ffmpeg_result, false);

					ffmpeg_result = avcodec_receive_frame(context[0], vid_raw_data[buffer_num]);
					if(ffmpeg_result != 0)
						Log_log_save(vid_worker_thread_string, "avcodec_receive_frame()...[Error] ", ffmpeg_result, false);
					else
					{
						if(vid_get_info_request)
						{
							vid_framerate = (double)context[0]->framerate.num / (double)context[0]->framerate.den;
							vid_video_width = vid_raw_data[buffer_num]->width;
							vid_video_height = vid_raw_data[buffer_num]->height;
							size = (vid_video_width * vid_video_height) + (vid_video_width * vid_video_height / 2);
							size += 32;
							for(int i = 0; i < 2; i++)
							{
								free(yuv_data[i]);
								yuv_data[i] = NULL;
								yuv_data[i] = (u8*)malloc(size);
							}

							vid_zoom = 1.0;
							while(vid_video_width * vid_zoom > 400.0 || vid_video_height * vid_zoom > 220.0)
								vid_zoom -= 0.01;

							vid_x = 0;
							vid_y = 15;
							vid_get_info_request = false;
						}

						vid_convert_texture_request = true;
					}
				}
				vid_buffer_num = buffer_num;
				if(buffer_num == 0)
					buffer_num = 1;
				else
					buffer_num = 0;

				av_packet_free(&packet);
				av_frame_free(&vid_raw_data[buffer_num]);
				vid_temp_decode_fps++;
				vid_decoding = false;
			}
			else
				usleep(100);
		}
		usleep(ACTIVE_THREAD_SLEEP_TIME);

		while (vid_thread_suspend)
			usleep(INACTIVE_THREAD_SLEEP_TIME);
	}

	for(int i = 0; i < 2; i++)
	{
		av_frame_free(&vid_raw_data[i]);
		free(yuv_data[i]);
		yuv_data[i] = NULL;
	}
	Log_log_save(vid_decord_video_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Vid_worker_thread(void* arg)
{
	Log_log_save(vid_worker_thread_string, "Thread started.", 1234567890, false);
	bool init_swr = true;
	size_t cut_pos = 0;
	int log_num = 0;
	int ffmpeg_result = 0;
	int audio_size = 0;
	int buffer_num = 0;
	int count[2] = { 0, 0, };
	int num_of_buffers = 50;
	u8* sound_buffer[num_of_buffers];
	u8* cache = NULL;
	u8* httpc_buffer = NULL;
	u32 dl_size = 0;
	u32 status_code = 0;
	float frametime = 0.0;
	std::string last_url = "";
	std::string cache_string = "";
	TickCounter timer;
	ndspWaveBuf ndsp_buffer[num_of_buffers];
	AVPacket *packet = NULL;
	AVFrame *raw_data = NULL;
	AVFormatContext* format_context = NULL;
	SwrContext* swr_context = NULL;
	Result_with_string result;

	for(int i = 0; i < num_of_buffers; i++)
		sound_buffer[i] = (u8*)linearAlloc(0x2000);

	osTickCounterStart(&timer);

	while (vid_worker_thread_run)
	{
		if(vid_dl_and_decode_request)
		{
			APT_SetAppCpuTimeLimit(30);
			cut_pos = vid_url.find("&id=");
			if (!(cut_pos == std::string::npos))
			{
				cache_string = vid_url.substr(cut_pos + 4);

				if (cache_string.length() > 33)
					cache_string = cache_string.substr(0, 33);

				cache_string += ".mp4";
			}

			httpc_buffer = (u8*)malloc(0x20000);
			log_num = Log_log_save(vid_worker_thread_string, "Httpc_dl_data()...", 1234567890, false);
			result = Httpc_dl_data(vid_url, httpc_buffer, 0x20000, &dl_size, &status_code, true, &last_url, false, 10, VID_HTTP_PORT0, "/Line/videos/", cache_string);
			Log_log_add(log_num, result.string, result.code, false);
			free(httpc_buffer);
			httpc_buffer = NULL;

			if(result.code == 0)
			{
				vid_load_file_name = cache_string;
				vid_load_dir_name = "/Line/videos/";
				vid_change_video_request = true;
				vid_decode_request = true;
			}
			APT_SetAppCpuTimeLimit(80);
			vid_dl_and_decode_request = false;
		}
		else if(vid_decode_request)
		{
			APT_SetAppCpuTimeLimit(80);
			osTickCounterUpdate(&timer);
			for(int i = 0; i < 16; i++)
				vid_enable[i] = false;

			vid_stop_request = false;
			vid_pause_request = false;
			vid_change_video_request = false;

			frametime = 0.0;
			init_swr = true;
			vid_offset = 0;
			vid_frames = 0;
			vid_decoded_frames = 0;
			vid_count_reset_request = true;
			vid_get_info_request = true;
			vid_sws_init_request = true;
			vid_video_width = 0;
			vid_video_height = 0;
			vid_video_stream_num = -1;
			vid_audio_stream_num = -1;
			vid_framerate = 0.0;
			vid_current_video_format = "none";
			vid_current_audio_format = "none";
			for(int i = 0; i < num_of_buffers; i++)
				memset(sound_buffer[i], 0x0, 0x2000);

			format_context = avformat_alloc_context();
			ffmpeg_result = avformat_open_input(&format_context, (vid_load_dir_name + vid_load_file_name).c_str(), NULL, NULL);
			if(ffmpeg_result != 0)
			{
				Err_set_error_message(Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS), "avformat_open_input() failed", vid_worker_thread_string, ffmpeg_result);
				Err_set_error_show_flag(true);
				Log_log_save(vid_worker_thread_string, "avformat_open_input()...[Error] ", ffmpeg_result, false);
			}
			else
			{
				Log_log_save(vid_worker_thread_string, "avformat_open_input()...[Success] ", ffmpeg_result, false);

				ffmpeg_result = avformat_find_stream_info(format_context, NULL);
				if(format_context != NULL)
				{
					Log_log_save(vid_worker_thread_string, "avformat_find_stream_info()...[Success] ", ffmpeg_result, false);
					for(int i = 0; i < (int)format_context->nb_streams; i++)
					{
						if(format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
							vid_video_stream_num = i;
						else if(format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
							vid_audio_stream_num = i;
					}

					if(vid_video_stream_num != -1)
					{
						codec[0] = avcodec_find_decoder(format_context->streams[vid_video_stream_num]->codecpar->codec_id);
						if(!codec[0])
							Log_log_save(vid_worker_thread_string, "avcodec_find_decoder() failed ", -1, false);

						context[0] = avcodec_alloc_context3(codec[0]);
						if(!context[0])
							Log_log_save(vid_worker_thread_string, "alloc failed ", -1, false);

						ffmpeg_result = avcodec_parameters_to_context(context[0], format_context->streams[vid_video_stream_num]->codecpar);
						if(ffmpeg_result != 0)
							Log_log_save(vid_worker_thread_string, "avcodec_parameters_to_context() failed ", ffmpeg_result, false);

						ffmpeg_result = avcodec_open2(context[0], codec[0], NULL);
						if(ffmpeg_result != 0)
							Log_log_save(vid_worker_thread_string, "avcodec_open2() failed ", ffmpeg_result, false);
					}
					if(vid_audio_stream_num != -1)
					{
						codec[1] = avcodec_find_decoder(format_context->streams[vid_audio_stream_num]->codecpar->codec_id);
						if(!codec[1])
							Log_log_save(vid_worker_thread_string, "avcodec_find_decoder() failed ", -1, false);

						context[1] = avcodec_alloc_context3(codec[1]);
						if(!context[1])
							Log_log_save(vid_worker_thread_string, "alloc failed ", -1, false);

						ffmpeg_result = avcodec_parameters_to_context(context[1], format_context->streams[vid_audio_stream_num]->codecpar);
						if(ffmpeg_result != 0)
							Log_log_save(vid_worker_thread_string, "avcodec_parameters_to_context() failed ", ffmpeg_result, false);

						ffmpeg_result = avcodec_open2(context[1], codec[1], NULL);
						if(ffmpeg_result != 0)
							Log_log_save(vid_worker_thread_string, "avcodec_open2() failed ", ffmpeg_result, false);
					}
					if(context[0])
					{
						const AVCodecDescriptor* codec_info = NULL;
						codec_info = avcodec_descriptor_get(format_context->streams[vid_video_stream_num]->codecpar->codec_id);
						vid_current_video_format = codec_info->long_name;
					}
					if(context[1])
					{
						const AVCodecDescriptor* codec_info = NULL;
						codec_info = avcodec_descriptor_get(format_context->streams[vid_audio_stream_num]->codecpar->codec_id);
						vid_current_audio_format = codec_info->long_name;
					}
					/*if(context[0]->codec->max_lowres >= 2 && vid_strong_down_sampling)
						context[0]->lowres = 2;
					else if(context[0]->codec->max_lowres >= 1 && vid_down_sampling)
						context[0]->lowres = 1;*/

					vid_video_length = (double)format_context->duration / AV_TIME_BASE;
					vid_bit_rate = format_context->bit_rate;

					vid_count_request = true;
					while (true)
					{
						Menu_reset_afk_time();
						vid_count_request = false;
						while (vid_pause_request && !vid_stop_request && !vid_seek_request && !vid_change_video_request)
							usleep(50000);

						vid_count_request = true;

						if(vid_stop_request || vid_change_video_request || vid_seek_request)
						{
							ndspChnWaveBufClear(21);
							if(vid_seek_request)
							{
								log_num = Log_log_save(vid_worker_thread_string, "avformat_seek_file()... ", 1234567890, false);
								//Use us(microsecond) to specify time
								ffmpeg_result = avformat_seek_file(format_context, -1, vid_offset, vid_offset, vid_offset, 0);
								if(ffmpeg_result >= 0)
								{
									vid_bar_pos = vid_offset / 1000;
									for(int i = 0; i < num_of_buffers; i++)
										memset(sound_buffer[i], 0x0, 0x2000);

									Log_log_add(log_num, Err_query_template_summary(0), ffmpeg_result, false);
								}
								else
									Log_log_add(log_num, Err_query_template_summary(1024), ffmpeg_result, false);

								vid_seek_request = false;
							}
							else
								break;
						}

						packet = av_packet_alloc();
						if(!packet)
						{
							Log_log_save(vid_worker_thread_string, "av_packet_alloc()...[Error] ", -1, false);
							break;
						}

						ffmpeg_result = av_read_frame(format_context, packet);
						if(packet->size == 0)
						{
							Log_log_save(vid_worker_thread_string, "av_read_frame()...[Error] ", ffmpeg_result, false);
							if(ffmpeg_result <= 0)
								break;
						}
						else
						{
							if(packet->stream_index == vid_video_stream_num)
							{
								vid_frames++;
								vid_wait = true;
								av_packet_free(&vid_packet);
								vid_packet = av_packet_alloc();
								av_packet_ref(vid_packet, packet);
								vid_wait = false;

								vid_decode_video_request = true;

								osTickCounterUpdate(&timer);
								frametime = osTickCounterRead(&timer);
								if(vid_framerate == 0.0)
								{
									if(50.0 - frametime > 0)
										usleep((50.0 - frametime) * 1000);
								}
								else
								{
									if((1000.0 / vid_framerate) - frametime > 0)
										usleep(((1000.0 / vid_framerate) - frametime) * 1000);
								}

								osTickCounterUpdate(&timer);

								if(!vid_allow_skip_frame || packet->flags & AV_PKT_FLAG_KEY)
								{
									while(vid_decode_video_request)
										usleep(500);
								}

								vid_temp_read_fps++;
							}
							else if(packet->stream_index == vid_audio_stream_num)
							{
								raw_data = av_frame_alloc();
								if(!raw_data)
									Log_log_save(vid_worker_thread_string, "av_frame_alloc()...[Error] ", -1, false);
								else
								{
									ffmpeg_result = avcodec_send_packet(context[1], packet);
									if(ffmpeg_result != 0)
										Log_log_save(vid_worker_thread_string, "avcodec_send_packet()...[Error] ", ffmpeg_result, false);

									count[0] = 0;
									while(true)
									{
										ffmpeg_result = avcodec_receive_frame(context[1], raw_data);
										if(ffmpeg_result != 0)
										{
											if(count[0] <= 0)
												Log_log_save(vid_worker_thread_string, "avcodec_receive_frame()...[Error] ", ffmpeg_result, false);

											break;
										}
										else
										{
											count[0]++;
											if(init_swr)
											{
												swr_context = swr_alloc();
												swr_alloc_set_opts(swr_context, av_get_default_channel_layout(context[1]->channels), AV_SAMPLE_FMT_S16, raw_data->sample_rate,
												av_get_default_channel_layout(context[1]->channels), context[1]->sample_fmt, raw_data->sample_rate, 0, NULL);
												if(!swr_context)
													Log_log_save(vid_worker_thread_string, "swr_alloc_set_opts()...[Error] ", 1234567890, false);

												swr_init(swr_context);
												ndspChnReset(21);
												ndspChnWaveBufClear(21);
												float mix[12];
												memset(mix, 0, sizeof(mix));
												mix[0] = 1.0;
												mix[1] = 1.0;
												ndspChnSetMix(21, mix);
												memset(ndsp_buffer, 0, sizeof(ndsp_buffer));
												if(raw_data->channels == 2)
												{
													ndspChnSetFormat(21, NDSP_FORMAT_STEREO_PCM16);
													ndspSetOutputMode(NDSP_OUTPUT_STEREO);
												}
												else
												{
													ndspChnSetFormat(21, NDSP_FORMAT_MONO_PCM16);
													ndspSetOutputMode(NDSP_OUTPUT_MONO);
												}

												ndspChnSetInterp(21, NDSP_INTERP_LINEAR);
												ndspChnSetRate(21, raw_data->sample_rate);
												init_swr = false;
											}

											av_samples_alloc(&cache, NULL, context[1]->channels, raw_data->nb_samples, AV_SAMPLE_FMT_S16, 0);
											swr_convert(swr_context, &cache, raw_data->nb_samples, (const uint8_t**)raw_data->data, raw_data->nb_samples);
											audio_size = av_samples_get_buffer_size(NULL, context[1]->channels, raw_data->nb_samples, AV_SAMPLE_FMT_S16, 1);

											memcpy(sound_buffer[buffer_num], cache, audio_size);
											av_freep(&cache);

											ndsp_buffer[buffer_num].data_vaddr = sound_buffer[buffer_num];
											ndsp_buffer[buffer_num].nsamples = raw_data->nb_samples;
											ndspChnWaveBufAdd(21, &ndsp_buffer[buffer_num]);

											if(buffer_num >= 0 && buffer_num <= (num_of_buffers - 2))
												buffer_num++;
											else
												buffer_num = 0;

											count[1]++;
											if(count[1] > 30)
											{
												vid_bar_pos = (double)raw_data->pkt_pos * 8 / vid_bit_rate * 1000;
												count[1] = 0;
											}

											while((ndsp_buffer[buffer_num].status == NDSP_WBUF_PLAYING || ndsp_buffer[buffer_num].status == NDSP_WBUF_QUEUED)
											&& !vid_stop_request && !vid_change_video_request && !vid_seek_request)
												usleep(1000);

											memset(sound_buffer[buffer_num], 0x0, audio_size);
										}
									}
								}
								av_frame_free(&raw_data);
							}
						}
						av_packet_free(&packet);
					}
				}
				else
				{
					Err_set_error_message(Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS), "avformat_find_stream_info() failed", vid_worker_thread_string, ffmpeg_result);
					Err_set_error_show_flag(true);
					Log_log_save(vid_worker_thread_string, "avformat_find_stream_info()...[Error] ", ffmpeg_result, false);
				}

				if(context[1])
				{
					ffmpeg_result = avcodec_send_packet(context[1], NULL);
					for(int i = 0; i < 100; i++)
					{
						raw_data = av_frame_alloc();
						ffmpeg_result = avcodec_receive_frame(context[1], raw_data);
						av_frame_free(&raw_data);
						if(ffmpeg_result == AVERROR_EOF)
							break;
					}
				}
			}

			while(ndsp_buffer[0].status == NDSP_WBUF_PLAYING || ndsp_buffer[0].status == NDSP_WBUF_QUEUED
			|| ndsp_buffer[1].status == NDSP_WBUF_PLAYING || ndsp_buffer[1].status == NDSP_WBUF_QUEUED
			|| ndsp_buffer[2].status == NDSP_WBUF_PLAYING || ndsp_buffer[2].status == NDSP_WBUF_QUEUED
			|| ndsp_buffer[3].status == NDSP_WBUF_PLAYING || ndsp_buffer[3].status == NDSP_WBUF_QUEUED
			|| ndsp_buffer[4].status == NDSP_WBUF_PLAYING || ndsp_buffer[4].status == NDSP_WBUF_QUEUED)
			{
				if(vid_stop_request || vid_change_video_request)
					break;

				usleep(25000);
			}

			while(vid_decoding || vid_converting)
				usleep(10000);

			APT_SetAppCpuTimeLimit(30);
			for(int i = 0; i < 2; i++)
			{
				free(vid_bgr_data[i]);
				vid_bgr_data[i] = NULL;
				avcodec_free_context(&context[i]);
			}
			avformat_close_input(&format_context);
			av_packet_free(&packet);
			swr_free(&swr_context);
			vid_count_request = false;
			if(!vid_change_video_request)
				vid_decode_request = false;
		}
		usleep(ACTIVE_THREAD_SLEEP_TIME);

		while (vid_thread_suspend)
			usleep(INACTIVE_THREAD_SLEEP_TIME);
	}

	for(int i = 0; i < num_of_buffers; i++)
	{
		linearFree(sound_buffer[i]);
		sound_buffer[i] = NULL;
	}

	Log_log_save(vid_worker_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

extern "C" void memcpy_asm(u8*, u8*, int);
void Vid_convert_thread(void* arg)
{
	Log_log_save(vid_convert_thread_string, "Thread started.", 1234567890, false);
	int process_pic_num = 0;
	int log_num = 0;
	int width = 0;
	int height = 0;
	int parse_x = 0;
	int parse_y = 0;
	Result_with_string result;

	while (vid_convert_thread_run)
	{
		while(vid_decode_request)
		{
			if(vid_convert_texture_request)
			{
				vid_converting = true;
				vid_convert_texture_request = false;
				parse_x = 0;
				parse_y = 0;
				if(vid_pic_num == 0)
					process_pic_num = 1;
				else
					process_pic_num = 0;

				if(vid_raw_data[vid_buffer_num]->data[0] != NULL)
				{
					//log_num = Log_log_save(vid_convert_thread_string, "memcpy_asm()...", 1234567890, false);
					memcpy_asm(yuv_data[vid_buffer_num], vid_raw_data[vid_buffer_num]->data[0], vid_video_width * vid_video_height);
					memcpy_asm(yuv_data[vid_buffer_num] + (vid_video_width * vid_video_height), vid_raw_data[vid_buffer_num]->data[1], vid_video_width * vid_video_height / 4);
					memcpy_asm(yuv_data[vid_buffer_num] + ((vid_video_width * vid_video_height) + (vid_video_width * vid_video_height / 4)), vid_raw_data[vid_buffer_num]->data[2], vid_video_width * vid_video_height / 4);
					//Log_log_add(log_num, "", 1234567890, false);
	//				memcpy(yuv_data[1], yuv_data[0], (vid_video_width * vid_video_height) + (vid_video_width * vid_video_height / 2));

					free(vid_bgr_data[process_pic_num]);
					vid_bgr_data[process_pic_num] = NULL;
					vid_bgr_data[process_pic_num] = (u8*)malloc(vid_video_width * vid_video_height * 2);
					if(!vid_bgr_data[process_pic_num])
					{

					}
					else
					{
						for(int i = 0; i < 16; i++)
						{
							linearFree(vid_c2d_image[(process_pic_num * 16) + i].tex->data);
							vid_c2d_image[(process_pic_num * 16) + i].tex->data = NULL;
						}

						//log_num = Log_log_save(vid_convert_thread_string, "Draw_yuv420p_to_rgb565()..." + result.string, result.code, false);
						Draw_yuv420p_to_rgb565((unsigned char*)yuv_data[vid_buffer_num], (unsigned char*)yuv_data[vid_buffer_num] + (vid_video_width * vid_video_height), (unsigned char*)yuv_data[vid_buffer_num] + (vid_video_width * vid_video_height) + (vid_video_width * vid_video_height / 4), (unsigned char*)vid_bgr_data[process_pic_num], vid_video_width, vid_video_height);
						//memcpy_asm(vid_bgr_data[process_pic_num], yuv_data[vid_buffer_num], vid_video_width * vid_video_height * 1.5);
						//Log_log_add(log_num, "", 1234567890, false);

						width = vid_video_width;
						height = vid_video_height;

						for(int i = 0; i < 16; i++)
						{
							if(parse_x <= vid_video_width && parse_y <= vid_video_height)
							{
								//log_num = Log_log_save(vid_convert_thread_string, "Draw_create_texture()..." + result.string, result.code, false);
								result = Draw_create_texture(vid_c2d_image[(process_pic_num * 16) + i].tex, (Tex3DS_SubTexture*)vid_c2d_image[(process_pic_num * 16) + i].subtex, vid_bgr_data[process_pic_num], (u32)(width  * height * 2), width, height, 2, parse_x, parse_y, 512, 512, GPU_RGB565);
								//Log_log_add(log_num, "", 1234567890, false);
								if(result.code != 0)
									Log_log_save(vid_convert_thread_string, "Draw_create_texture()..." + result.string, result.code, false);

								vid_enable[i] = true;
							}

							if(parse_x + 512 >= 2048)
							{
								parse_x = 0;
								parse_y += 512;
							}
							else
								parse_x += 512;
						}
					}
					if(vid_pic_num == 0)
						vid_pic_num = 1;
					else
						vid_pic_num = 0;

					vid_temp_convert_fps++;
					vid_need_reflesh = true;
				}
				vid_converting = false;
			}
			else
				usleep(100);
		}
		usleep(ACTIVE_THREAD_SLEEP_TIME);

		while (vid_thread_suspend)
			usleep(INACTIVE_THREAD_SLEEP_TIME);
	}
	Log_log_save(vid_worker_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Vid_cancel_select_file(void)
{
	vid_select_file_request = false;
}

void Vid_exit(void)
{
	Log_log_save(vid_exit_string, "Exiting...", 1234567890, FORCE_DEBUG);
	u64 time_out = 10000000000;
	int log_num;
	bool failed = false;
	Result_with_string result;

	vid_already_init = false;
	vid_thread_suspend = false;
	vid_stop_request = true;
	vid_decord_video_thread_run = false;
	vid_worker_thread_run = false;
	vid_convert_thread_run = false;
	vid_timer_thread_run = false;

	Draw_progress("[Vid] Exiting...");
	log_num = Log_log_save(vid_exit_string, "threadJoin()0/3...", 1234567890, FORCE_DEBUG);
	result.code = threadJoin(vid_worker_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, Err_query_template_summary(0), result.code, FORCE_DEBUG);
	else
	{
		failed = true;
		Log_log_add(log_num, Err_query_template_summary(-1024), result.code, FORCE_DEBUG);
	}

	log_num = Log_log_save(vid_exit_string, "threadJoin()1/3...", 1234567890, FORCE_DEBUG);
	result.code = threadJoin(vid_convert_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, Err_query_template_summary(0), result.code, FORCE_DEBUG);
	else
	{
		failed = true;
		Log_log_add(log_num, Err_query_template_summary(-1024), result.code, FORCE_DEBUG);
	}

	log_num = Log_log_save(vid_exit_string, "threadJoin()2/3...", 1234567890, FORCE_DEBUG);
	result.code = threadJoin(vid_decode_video_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, Err_query_template_summary(0), result.code, FORCE_DEBUG);
	else
	{
		failed = true;
		Log_log_add(log_num, Err_query_template_summary(-1024), result.code, FORCE_DEBUG);
	}

	log_num = Log_log_save(vid_exit_string, "threadJoin()3/3...", 1234567890, FORCE_DEBUG);
	result.code = threadJoin(vid_timer_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, Err_query_template_summary(0), result.code, FORCE_DEBUG);
	else
	{
		failed = true;
		Log_log_add(log_num, Err_query_template_summary(-1024), result.code, FORCE_DEBUG);
	}

	threadFree(vid_worker_thread);
	threadFree(vid_convert_thread);
	threadFree(vid_decode_video_thread);

	for(int i = 0; i < 32; i++)
	{
		linearFree(vid_c2d_image[i].tex->data);
		linearFree(vid_c2d_image[i].tex);
		linearFree((void*)vid_c2d_image[i].subtex);
		vid_c2d_image[i].tex->data = NULL;
		vid_c2d_image[i].tex = NULL;
		vid_c2d_image[i].subtex = NULL;
	}

	if (failed)
		Log_log_save(vid_exit_string, "[Warn] Some function returned error.", 1234567890, FORCE_DEBUG);

	APT_SetAppCpuTimeLimit(30);
}

void Vid_init(void)
{
	Log_log_save(vid_init_string, "Initializing...", 1234567890, FORCE_DEBUG);
	bool failed = false;
	bool new_3ds = false;
	Result_with_string result;

	Draw_progress("0/0 [Vid] Starting threads...");
	if (!failed)
	{
		vid_worker_thread_run = true;
		vid_convert_thread_run = true;
		vid_decord_video_thread_run = true;
		vid_timer_thread_run = true;
		APT_CheckNew3DS(&new_3ds);
		vid_worker_thread = threadCreate(Vid_worker_thread, (void*)(""), STACKSIZE, PRIORITY_REALTIME, 1, false);
		vid_timer_thread = threadCreate(Vid_timer_thread, (void*)(""), STACKSIZE, PRIORITY_REALTIME, 0, false);
		if(new_3ds)
		{
			vid_convert_thread = threadCreate(Vid_convert_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, 1, false);
			vid_decode_video_thread = threadCreate(Vid_decode_video_thread, (void*)(""), STACKSIZE, PRIORITY_REALTIME, 2, false);
		}
		else
		{
			vid_convert_thread = threadCreate(Vid_convert_thread, (void*)(""), STACKSIZE, PRIORITY_HIGH, 1, false);
			vid_decode_video_thread = threadCreate(Vid_decode_video_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, 0, false);
		}
	}

	for(int i = 0; i < 32; i++)
	{
		vid_c2d_image[i].tex = (C3D_Tex*)linearAlloc(sizeof(C3D_Tex));
		vid_c2d_image[i].subtex = (Tex3DS_SubTexture*)linearAlloc(sizeof(Tex3DS_SubTexture));
	}

	for(int i = 0; i < 16; i++)
		vid_enable[i] = false;

	vid_stop_request = false;
	Vid_resume();
	vid_already_init = true;
	Log_log_save(vid_init_string, "Initialized", 1234567890, FORCE_DEBUG);
}

void Test_thread(void* args)
{
	std::string file = "/test.mp4";
	int ffmpeg_result = 0;
	int video_stream_num = 0;
	AVCodecContext *context = NULL;
	AVFrame *raw_image = NULL;
	AVPacket *packet = NULL;
	AVCodec *codec = NULL;
	AVFormatContext* format_context = NULL;

	//alloc memory
	//Log_log_save() is my logging api you can use printf() insted.
	format_context = avformat_alloc_context();
	packet = av_packet_alloc();
	raw_image = av_frame_alloc();
	if(!format_context || !packet || !raw_image)
			Log_log_save("debug", "alloc failed ", -1, false);
	else
			Log_log_save("debug", "alloc ok ", 0, false);

	//open file
	ffmpeg_result = avformat_open_input(&format_context, file.c_str(), NULL, NULL);
	if(ffmpeg_result != 0)
			Log_log_save("debug", "avformat_open_input() failed ", -1, false);
	else
			Log_log_save("debug", "avformat_open_input() ok ", 0, false);

	//search video stream
	for(int i = 0; i < (int)format_context->nb_streams; i++)
	{
			if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
			{
					video_stream_num = i;
					break;
			}
	}

	Log_log_save("debug", "video stream " + std::to_string(video_stream_num) + " ", 0, false);

	//find decoder
	codec = avcodec_find_decoder(format_context->streams[video_stream_num]->codecpar->codec_id);
	if(!codec)
			Log_log_save("debug", "avcodec_find_decoder() failed ", -1, false);
	else
			Log_log_save("debug", "avcodec_find_decoder() ok ", 0, false);

	//alloc memory
	context = avcodec_alloc_context3(codec);
	if(!context)
			Log_log_save("debug", "alloc failed ", -1, false);
	else
			Log_log_save("debug", "alloc ok ", 0, false);

	ffmpeg_result = avcodec_parameters_to_context(context, format_context->streams[video_stream_num]->codecpar);
	if(ffmpeg_result == 0)
		Log_log_save("debug", "avcodec_parameters_to_context() ok ", ffmpeg_result, false);
	else
		Log_log_save("debug", "avcodec_parameters_to_context() failed ", ffmpeg_result, false);

	//open codec
	ffmpeg_result = avcodec_open2(context, codec, NULL);
	if(ffmpeg_result != 0)
			Log_log_save("debug", "avcodec_open2() failed ", ffmpeg_result, false);
	else
			Log_log_save("debug", "avcodec_open2() ok ", ffmpeg_result, false);

	while(true)
	{
			//read 1 frame
			ffmpeg_result = av_read_frame(format_context, packet);
			if(ffmpeg_result != 0)
					Log_log_save("debug", "av_read_frame() failed ", ffmpeg_result, false);
			else
					Log_log_save("debug", "av_read_frame() ok ", ffmpeg_result, false);

			Log_log_save("debug", "frame size " + std::to_string(packet->size) + " ", 0, false);

			if(packet->stream_index == format_context->streams[video_stream_num]->index) //if frame was video data
			{
					Log_log_save("debug", "video frame ", 0, false);

					ffmpeg_result = avcodec_send_packet(context, packet);
					if(ffmpeg_result != 0)
							Log_log_save("debug", "avcodec_send_packet() failed ", ffmpeg_result, false);
					else
							Log_log_save("debug", "avcodec_send_packet() ok ", ffmpeg_result, false);

					ffmpeg_result = avcodec_receive_frame(context, raw_image);
					if(ffmpeg_result != 0)
							Log_log_save("debug", "avcodec_receive_frame() failed ", ffmpeg_result, false);
					else
							Log_log_save("debug", "avcodec_receive_frame() ok ", ffmpeg_result, false);

		//      Log_log_save("debug", "w " + std::to_string(raw_image->width) + " h " + std::to_string(raw_image->height) + " format " + av_get_pix_fmt_name((AVPixelFormat)raw_image->format), 1234567890, false);

					break;
			}
			else
					Log_log_save("debug", "other frame retrying... ", 0, false);
	}
}

void Vid_main(void)
{
	int msg_num = 0;
	float text_red;
	float text_green;
	float text_blue;
	float text_alpha;
	double scroll_speed = 0.5;
	double draw_x = 0.0;
	double draw_y = 0.0;
	double tex_size_x = 0.0;
	double tex_size_y = 0.0;
	std::string msg = "";
 	Hid_info key;

	if(vid_dl_and_decode_request)
	{
		vid_dl_progress = Httpc_query_dl_progress(VID_HTTP_PORT0);
		vid_dled_size = Httpc_query_dled_size(VID_HTTP_PORT0);
	}

	if(vid_dl_and_decode_request && vid_dled_size == 0)
		vid_dled_size = 2;

	if (Sem_query_settings(SEM_NIGHT_MODE))
	{
		text_red = 1.0;
		text_green = 1.0;
		text_blue = 1.0;
		text_alpha = 0.75;
		white_or_black_tint = white_tint;
	}
	else
	{
		text_red = 0.0;
		text_green = 0.0;
		text_blue = 0.0;
		text_alpha = 1.0;
		white_or_black_tint = black_tint;
	}
	if(vid_video_width > 512)
		tex_size_x = 512 * vid_zoom;
	else
		tex_size_x = vid_video_width * vid_zoom;

	if(vid_video_height > 512)
		tex_size_y = 512 * vid_zoom;
	else
		tex_size_y = vid_video_height * vid_zoom;

	if((vid_video_width > 400 && vid_video_height > 240 && tex_size_x >= 400 && tex_size_y >= 240) || (vid_video_width < 400 || vid_video_height < 240))
	{
		//Log_log_save("", "N", 1234567890, false);
		for(int i = 0; i < 16; i++)
		{
			if(vid_enable[i])
				C3D_TexSetFilter(vid_c2d_image[(16 * vid_pic_num) + i].tex, GPU_NEAREST, GPU_NEAREST);
		}
	}
	else
	{
	//	Log_log_save("", "L", 1234567890, false);
		for(int i = 0; i < 16; i++)
		{
			if(vid_enable[i])
				C3D_TexSetFilter(vid_c2d_image[(16 * vid_pic_num) + i].tex, GPU_LINEAR, GPU_LINEAR);
		}
	}

	if(vid_pre_dl_progress != vid_dl_progress || vid_pre_dled_size != vid_dled_size)
	{
		vid_pre_dl_progress = vid_dl_progress;
		vid_pre_dled_size = vid_dled_size;
		vid_need_reflesh = true;
	}

	scroll_speed = Sem_query_settings_d(SEM_SCROLL_SPEED);
	Hid_query_key_state(&key);
	scroll_speed *= key.count;
	Log_main();
	if(Draw_query_need_reflesh() || !Sem_query_settings(SEM_ECO_MODE) || Expl_query_need_reflesh())
		vid_need_reflesh = true;

	if(!vid_select_file_request)
		Hid_key_flag_reset();

	if(vid_need_reflesh)
	{
		Draw_frame_ready();
		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);

		for(int i = 0; i < 16; i++)
		{
			if(vid_enable[i])
				Draw_texture(vid_c2d_image, dammy_tint, (16 * vid_pic_num) + i, (vid_x + draw_x), (vid_y + draw_y), tex_size_x, tex_size_y);

			if(draw_x + tex_size_x > tex_size_x * 3)
			{
				draw_x = 0.0;
				draw_y += tex_size_y;
			}
			else
				draw_x += tex_size_x;
		}
		if(vid_dl_and_decode_request)
		{
			Draw_texture(Square_image, aqua_tint, 0, 0.0, 15.0, 50.0 * vid_dl_progress, 3.0);
			Draw(vid_msg[0] + std::to_string(vid_dled_size / 1024.0 / 1024.0).substr(0, 4) + "MB", 0, 10.0, 20.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		}

		Draw_top_ui();

		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(1, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(1, true, 2, 1.0, 1.0, 1.0);

		draw_x = 0.0;
		draw_y = 0.0;
		for(int i = 0; i < 16; i++)
		{
			if(vid_enable[i])
				Draw_texture(vid_c2d_image, dammy_tint, (16 * vid_pic_num) + i, (vid_x + draw_x - 40.0), (vid_y + draw_y - 240.0), tex_size_x, tex_size_y);

			if(draw_x + tex_size_x > tex_size_x * 3)
			{
				draw_x = 0.0;
				draw_y += tex_size_y;
			}
			else
				draw_x += tex_size_x;
		}

		Draw_texture(Square_image, weak_yellow_tint, 0, 5.0, 155.0, 310.0, 70.0);
		Draw(vid_ver, 0, 280.0, 160.0, 0.4, 0.4, 0.0, 1.0, 0.0, 1.0);
		Draw(Sem_convert_seconds_to_time(vid_bar_pos / 1000) + " / " + Sem_convert_seconds_to_time(vid_video_length), 0, 12.5, 160.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);

		Draw_texture(Square_image, weak_aqua_tint, 0, 140.0, 160.0, 60.0, 13.0);
		Draw_texture(Square_image, weak_aqua_tint, 0, 210.0, 160.0, 60.0, 13.0);
		if(vid_decode_request && !vid_pause_request)
			msg_num = 11;
		else
			msg_num = 1;

		Draw(vid_msg[msg_num], 0, 142.5, 160.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(vid_msg[2], 0, 212.5, 160.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);

		Draw_texture(Square_image, blue_tint, 0, 10.0, 175.0, 300.0, 8.0);
		if(vid_video_length != 0.0)
			Draw_texture(Square_image, yellow_tint, 0, 10.0, 175.0, 300.0 * ((vid_bar_pos / 1000) / vid_video_length), 8.0);

		Draw_texture(Square_image, weak_aqua_tint, 0, 10.0, 185.0, 75.0, 10.0);
		Draw_texture(Square_image, weak_yellow_tint, 0, 85.0, 185.0, 75.0, 10.0);
		if(vid_menu_mode == 0)
		{
			Draw_texture(Square_image, aqua_tint, 0, 10.0, 185.0, 75.0, 10.0);
			Draw_texture(Square_image, weak_aqua_tint, 0, 10.0, 205.0, 70.0, 15.0);
			if(vid_allow_skip_frame)
				msg = vid_msg[9];
			else
				msg = vid_msg[10];

			Draw(vid_msg[5], 0, 10.0, 195.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			Draw(msg, 0, 10.0, 207.5, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);

			/*Draw_texture(Square_image, weak_aqua_tint, 0, 20.0, 210.0, 70.0, 15.0);
			Draw("Down sampling", 0, 20.0, 200.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
			if(vid_strong_down_sampling)
				msg = "ON(1/4)";
			else if(vid_down_sampling)
				msg = "ON(1/2)";
			else
				msg = "OFF";*/

			Draw_texture(Square_image, weak_aqua_tint, 0, 90.0, 205.0, 70.0, 15.0);
			Draw(vid_msg[6], 0, 92.5, 205.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);

			Draw_texture(Square_image, weak_aqua_tint, 0, 170.0, 185.0, 140.0, 15.0);
			Draw(vid_msg[7], 0, 172.5, 185.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);

			Draw_texture(Square_image, weak_aqua_tint, 0, 170.0, 205.0, 140.0, 15.0);
			Draw(vid_msg[8], 0, 172.5, 205.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		}
		else if(vid_menu_mode == 1)
		{
			Draw_texture(Square_image, yellow_tint, 0, 85.0, 185.0, 75.0, 10.0);
			Draw(vid_current_video_format, 0, 10.0, 195.0, 0.35, 0.35, 1.0, 0.0, 0.0, 1.0);
			Draw(vid_current_audio_format, 0, 10.0, 202.5, 0.35, 0.35, 1.0, 0.0, 0.0, 1.0);
			Draw(std::to_string(vid_video_width) + " x " + std::to_string(vid_video_height) + "@" + std::to_string(vid_framerate).substr(0, 5) + "fps " + std::to_string(vid_read_fps) + " / " + std::to_string(vid_decode_fps) + " / " + std::to_string(vid_convert_fps) + "fps  Skipped " + std::to_string(vid_frames - vid_decoded_frames) + " / " + std::to_string(vid_frames), 0, 10.0, 210.0, 0.375, 0.375, text_red, text_green, text_blue, text_alpha);
		}
		Draw(vid_msg[3], 0, 12.5, 185.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(vid_msg[4], 0, 87.5, 185.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);

		if(vid_select_file_request)
			Expl_draw("");

		Draw_bot_ui();
		Draw_touch_pos();

		Draw_apply_draw();
		vid_need_reflesh = false;
	}
	else
		gspWaitForVBlank();

	if (Err_query_error_show_flag())
	{
		if (key.p_touch && key.touch_x >= 150 && key.touch_x <= 170 && key.touch_y >= 150 && key.touch_y < 170)
			Err_set_error_show_flag(false);
	}
	else if(vid_select_file_request)
		Expl_main();
	else
	{
		if (key.p_touch || key.h_touch)
		{
			vid_touch_x_move_left = 0;
			vid_touch_y_move_left = 0;

			if (vid_scroll_mode)
			{
				vid_touch_x_move_left = key.touch_x_move;
				vid_touch_y_move_left = key.touch_y_move;
			}
			else if (key.p_touch && key.touch_y <= 154)
				vid_scroll_mode = true;
		}
		else
		{
			vid_scroll_mode = false;
			vid_button_selected[0] = false;
			vid_button_selected[1] = false;
			vid_touch_x_move_left -= (vid_touch_x_move_left * 0.025) * key.count;
			vid_touch_y_move_left -= (vid_touch_y_move_left * 0.025) * key.count;
			if (vid_touch_x_move_left < 0.5 && vid_touch_x_move_left > -0.5)
				vid_touch_x_move_left = 0;
			if (vid_touch_y_move_left < 0.5 && vid_touch_y_move_left > -0.5)
				vid_touch_y_move_left = 0;
		}

		if (key.p_start || (key.p_touch && key.touch_x >= 110 && key.touch_x <= 230 && key.touch_y >= 220 && key.touch_y <= 240))
			Vid_suspend();
		else if(key.p_a || (key.p_touch && key.touch_x >= 140 && key.touch_x <= 199 && key.touch_y >= 160 && key.touch_y <= 172))
		{
			if(!vid_decode_request)
				vid_decode_request = true;
			else if(!vid_pause_request)
				vid_pause_request = true;
			else
				vid_pause_request = false;

			vid_need_reflesh = true;
		}
		else if(key.p_b || (key.p_touch && key.touch_x >= 220 && key.touch_x <= 269 && key.touch_y >= 160 && key.touch_y <= 172))
			vid_stop_request = true;
		else if(key.p_touch && key.touch_x >= 10 && key.touch_x <= 310 && key.touch_y >= 175 && key.touch_y <= 182)
		{
			vid_offset = (key.touch_x - 10) * (vid_video_length / 300);
			vid_offset *= 1000000;
			vid_seek_request = true;
		}
		else if(key.p_touch && key.touch_x >= 10 && key.touch_x <= 84 && key.touch_y >= 185 && key.touch_y <= 194)
			vid_menu_mode = 0;
		else if(key.p_touch && key.touch_x >= 85 && key.touch_x <= 159 && key.touch_y >= 185 && key.touch_y <= 194)
			vid_menu_mode = 1;
		else if(vid_menu_mode == 0 && key.p_touch && key.touch_x >= 10 && key.touch_x <= 79 && key.touch_y >= 205 && key.touch_y <= 219)
			vid_allow_skip_frame = !vid_allow_skip_frame;
		else if(key.p_x || (vid_menu_mode == 0 && key.p_touch && key.touch_x >= 90 && key.touch_x <= 159 && key.touch_y >= 205 && key.touch_y <= 219))
		{
			void (*callback)(std::string, std::string);
			void (*cancel_callback)(void);
			callback = Vid_set_load_file;
			cancel_callback = Vid_cancel_select_file;
			Expl_set_callback(callback);
			Expl_set_cancel_callback(cancel_callback);
			Expl_set_operation_flag(EXPL_READ_DIR_REQUEST, true);
			vid_select_file_request = true;
		}
		/*else if(vid_menu_mode == 0 && key.p_touch && key.touch_x > 20 && key.touch_x < 90 && key.touch_y > 210 && key.touch_y < 225)
		{
			if(vid_strong_down_sampling)
			{
				vid_strong_down_sampling = false;
			}
			else if(vid_down_sampling)
			{
				vid_strong_down_sampling = true;
				vid_down_sampling = false;
			}
			else
				vid_down_sampling = true;
		}*/
		else if(vid_menu_mode == 0 && key.p_touch && key.touch_x >= 170 && key.touch_x <= 309 && key.touch_y >= 185 && key.touch_y <= 199)
			vid_button_selected[0] = true;
		else if(vid_menu_mode == 0 && key.p_touch && key.touch_x >= 170 && key.touch_x <= 309 && key.touch_y >= 205 && key.touch_y <= 219)
			vid_button_selected[1] = true;
		else if(vid_menu_mode == 0 && vid_button_selected[0] && key.h_touch && key.touch_x >= 170 && key.touch_x <= 309 && key.touch_y >= 185 && key.touch_y <= 199 && vid_zoom - 0.01 > 0.04)
		{
			vid_zoom -= 0.01;
			vid_need_reflesh = true;
		}
		else if(vid_menu_mode == 0 && vid_button_selected[1] && key.h_touch && key.touch_x >= 170 && key.touch_x <= 309 && key.touch_y >= 205 && key.touch_y <= 219)
		{
			vid_zoom += 0.01;
			vid_need_reflesh = true;
		}

		if(key.h_c_down || key.h_c_up || key.h_c_left || key.h_c_right || key.h_r || key.h_l || vid_touch_x_move_left != 0.0 || vid_touch_y_move_left != 0.0)
		{
			if(key.h_c_up || key.h_c_down)
			{
				if(key.held_time > 240)
					vid_y += (double)key.cpad_y * scroll_speed * 0.125;
				else
					vid_y += (double)key.cpad_y * scroll_speed * 0.0625;
			}
			if(key.h_c_right || key.h_c_left)
			{
				if(key.held_time > 240)
					vid_x -= (double)key.cpad_x * scroll_speed * 0.125;
				else
					vid_x -= (double)key.cpad_x * scroll_speed * 0.0625;
			}
			if(key.h_l && vid_zoom - 0.01 > 0.04)
				vid_zoom -= 0.01;
			else if(key.h_r)
				vid_zoom += 0.01;

			vid_x -= (vid_touch_x_move_left * scroll_speed);
			vid_y -= (vid_touch_y_move_left * scroll_speed);
			vid_need_reflesh = true;
		}
	}
}
