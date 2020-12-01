#include <3ds.h>
#include <unistd.h>
#include <string>

#include "hid.hpp"
#include "draw.hpp"
#include "file.hpp"
#include "log.hpp"
#include "error.hpp"
#include "menu.hpp"
#include "types.hpp"
#include "setting_menu.hpp"
#include "music_player.hpp"
#include "explorer.hpp"
#include "httpc.hpp"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
}

/*For draw*/
bool mup_need_reflesh = false;
bool mup_pre_loop_request = false;
bool mup_pre_select_file_request = false;
bool mup_pre_shuffle_request = false;
bool mup_pre_allow_sleep = false;
double mup_pre_music_length = 0.0;
float mup_pre_bar_pos = 0.0;
int mup_pre_music_sample_rate = 0;
int mup_pre_num_of_music_ch = 0;
int mup_pre_music_bit_rate = 0;
int mup_pre_dled_size = 0;
int mup_pre_dl_progress = 0;
std::string mup_pre_file_type = "unknown";
/*---------------------------------------------*/
int mup_debug = 0;
bool mup_main_run = false;
bool mup_play_thread_run = false;
bool mup_timer_thread_run = false;
bool mup_worker_thread_run = false;
bool mup_already_init = false;
bool mup_thread_suspend = true;
bool mup_play_request = false;
bool mup_stop_request = false;
bool mup_loop_request = false;
bool mup_count_request = false;
bool mup_count_reset_request = false;
bool mup_select_file_request = false;
bool mup_shuffle_request = false;
bool mup_allow_sleep = false;
bool mup_seek_request = false;
bool mup_change_music_request = false;
bool mup_dl_and_play_request = false;
bool mup_pause_request = false;
double mup_music_length = 0.0;
float mup_bar_pos = 0.0;
int mup_music_sample_rate = 0;
int mup_num_of_music_ch = 0;
int mup_music_bit_rate = 0;
int mup_offset = 0;
int mup_file_size = 0;
int mup_dled_size = 0;
int mup_dl_progress = 0;
std::string mup_url = "";
std::string mup_load_file_name = "";
std::string mup_load_dir_name = "/";
std::string mup_file_type = "unknown";
std::string mup_msg[MUP_NUM_OF_MSG];
std::string mup_worker_thread_string = "Mup/Worker thread";
std::string mup_play_thread_string = "Mup/Play thread";
std::string mup_timer_thread_string = "Mup/Timer thread";
std::string mup_init_string = "Mup/Init";
std::string mup_exit_string = "Mup/Exit";
std::string mup_ver = "v1.1.0";
Thread mup_play_thread, mup_timer_thread, mup_worker_thread;

bool Mup_query_init_flag(void)
{
	return mup_already_init;
}

bool Mup_query_running_flag(void)
{
	return mup_main_run;
}

void Mup_resume(void)
{
	mup_thread_suspend = false;
	mup_main_run = true;
	mup_need_reflesh = true;
	Menu_suspend();
}

void Mup_suspend(void)
{
	mup_thread_suspend = true;
	mup_main_run = false;
	Menu_resume();
}

void Mup_cancel_select_file(void)
{
	mup_select_file_request = false;
}

void Mup_set_msg(int msg_num, std::string msg)
{
	if (msg_num >= 0 && msg_num < MUP_NUM_OF_MSG)
		mup_msg[msg_num] = msg;
}

void Mup_set_allow_sleep(bool flag)
{
	mup_allow_sleep = flag;
	aptSetSleepAllowed(mup_allow_sleep);
}

void Mup_set_url(std::string url)
{
	mup_url = url;
	mup_dl_and_play_request = true;
}

void Mup_set_load_file(std::string file_name, std::string dir_name)
{
	mup_load_file_name = file_name;
	mup_load_dir_name = dir_name;
	mup_play_request = true;
	mup_change_music_request = true;
	mup_select_file_request = false;
}

Result_with_string Mup_play_sound(u8* sound_buffer, int buffer_size, int sample_rate, int num_of_ch, int play_ch)
{
	Result_with_string result;

	result.code = csndPlaySound(play_ch, SOUND_ONE_SHOT | SOUND_FORMAT_16BIT, (sample_rate * num_of_ch), 1.0, 0.0, (u32*)sound_buffer, NULL, buffer_size);
	if (result.code != 0)
		result.string = "csndPlaySound failed. ";

	return result;
}

void Mup_worker_thread(void* arg)
{
	Log_log_save(mup_worker_thread_string, "Thread started.", 1234567890, false);
	size_t cut_pos = std::string::npos;
	int log_num = 0;
	u8* httpc_buffer = NULL;
	u32 dl_size = 0;
	u32 status_code = 0;
	std::string cache_string = "";
	std::string last_url = "";
	Result_with_string result;

	while (mup_worker_thread_run)
	{
		if(mup_dl_and_play_request)
		{
			cut_pos = mup_url.find("&id=");
			if (!(cut_pos == std::string::npos))
			{
				cache_string = mup_url.substr(cut_pos + 4);

				if (cache_string.length() > 33)
					cache_string = cache_string.substr(0, 33);

				cache_string += ".m4a";
			}

			httpc_buffer = (u8*)malloc(0x20000);
			Log_log_save(mup_worker_thread_string, mup_url, 1234567890, false);
			log_num = Log_log_save(mup_worker_thread_string, "Httpc_dl_data()...", 1234567890, false);
			result = Httpc_dl_data(mup_url, httpc_buffer, 0x20000, &dl_size, &status_code, true, &last_url, false, 10, MUP_HTTP_PORT0, "/Line/audio/", cache_string);
			Log_log_add(log_num, result.string, result.code, false);
			free(httpc_buffer);
			httpc_buffer = NULL;

			if(result.code == 0)
			{
				mup_load_file_name = cache_string;
				mup_load_dir_name = "/Line/audio/";
				mup_play_request = true;
				mup_change_music_request = true;
			}
			mup_dl_and_play_request = false;
		}
		else
			usleep(ACTIW_THREAD_SLEEP_TIME);

		while (mup_thread_suspend)
			usleep(INACTIW_THREAD_SLEEP_TIME);
	}

	Log_log_save(mup_worker_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Mup_timer_thread(void* arg)
{
	Log_log_save(mup_timer_thread_string, "Thread started.", 1234567890, false);
	TickCounter elapsed_time;
	osTickCounterStart(&elapsed_time);

	while (mup_timer_thread_run)
	{
		if (mup_count_reset_request)
		{
			mup_count_reset_request = false;
			osTickCounterUpdate(&elapsed_time);
			mup_bar_pos = 0.0;
		}

		if (mup_count_request)
		{
			osTickCounterUpdate(&elapsed_time);
			while (mup_count_request)
			{
				if (mup_count_reset_request)
					break;

				usleep(100000);
				osTickCounterUpdate(&elapsed_time);
				mup_bar_pos += osTickCounterRead(&elapsed_time);
				if(mup_bar_pos / 1000 > mup_music_length)
					mup_bar_pos = mup_music_length * 1000;
			}
		}
		else
			usleep(ACTIW_THREAD_SLEEP_TIME);

		while (mup_thread_suspend && !mup_count_request && !mup_count_reset_request)
			usleep(INACTIW_THREAD_SLEEP_TIME);
	}

	Log_log_save(mup_timer_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Mup_play_thread(void* arg)
{
	Log_log_save(mup_play_thread_string, "Thread started.", 1234567890, false);
	bool init_swr = true;
	int ffmpeg_result = 0;
	int stream_num = 0;
	int pre_buffer_num = 0;
	int buffer_num = 0;
	int buffer_offset = 0;
	int random_num = 0;
	int count = 0;
	int audio_size = 0;
	int log_num = 0;
	int samples = 0;
	double current_pos[2] = { 0.0, 0.0, };
	u8* sound_buffer[2] = { NULL, NULL, };
	u8* cache = NULL;
	u8 status;
	u64 file_size = 0;
	std::string file_name = "";
	std::string dir_name = "/";
	Handle fs_handle = 0;
	FS_Archive fs_archive = 0;
	ndspWaveBuf ndsp_buffer[2];
	AVPacket *packet = NULL;
	AVFrame *raw_data = NULL;
	AVFormatContext* format_context = NULL;
	const AVCodecDescriptor* codec_info = NULL;
	SwrContext* swr_context = NULL;
	AVCodecContext *context = NULL;
	AVCodec *codec = NULL;
	Result_with_string result;

	sound_buffer[0] = (u8*)linearAlloc(0x20000);
	sound_buffer[1] = (u8*)linearAlloc(0x20000);
	if(sound_buffer[0] == NULL || sound_buffer[1] == NULL)
	{
		Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), mup_play_thread_string, OUT_OF_MEMORY);
		Err_set_error_show_flag(true);
		Log_log_save(mup_play_thread_string, Err_query_template_summary(OUT_OF_MEMORY), OUT_OF_MEMORY, false);
		mup_play_thread_run = false;
	}

	while (mup_play_thread_run)
	{
		if (mup_play_request)
		{
			if(mup_change_music_request)
				mup_change_music_request = false;

			mup_stop_request = false;
			mup_seek_request = false;
			mup_pause_request = false;
			init_swr = true;
			mup_count_reset_request = true;
			file_name = mup_load_file_name;
			dir_name = mup_load_dir_name;
			file_size = 0;
			stream_num = -1;
			samples = 0;
			mup_file_size = 0;
			mup_offset = 0;
			current_pos[0] = 0.0;
			current_pos[1] = 0.0;			
			memset(sound_buffer[0], 0x0, 0x20000);
			memset(sound_buffer[1], 0x0, 0x20000);
			Expl_set_current_patch(dir_name);
			Expl_set_operation_flag(EXPL_READ_DIR_REQUEST, true);
			while (Expl_query_operation_flag(EXPL_READ_DIR_REQUEST))
				usleep(50000);

			if(mup_shuffle_request)
			{
				count++;
				srand((unsigned) time(NULL));
				while (true)
				{
					random_num = rand() % 2;
					if(!(count < Expl_query_num_of_file()))
						count = 0;
					else if(random_num == 1 && Expl_query_type(count) == "file")
						break;
					else
						count++;
				}

				file_name = Expl_query_file_name(count);
			}

			format_context = avformat_alloc_context();
			ffmpeg_result = avformat_open_input(&format_context, (dir_name + file_name).c_str(), NULL, NULL);
			if(ffmpeg_result != 0)
			{
				Err_set_error_message(Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS), "avformat_open_input() failed", mup_play_thread_string, ffmpeg_result);
				Err_set_error_show_flag(true);
				Log_log_save(mup_play_thread_string, "avformat_open_input()...[Error] ", ffmpeg_result, false);
			}
			else
			{
				Log_log_save(mup_play_thread_string, "avformat_open_input()...[Success] ", ffmpeg_result, false);
				File_check_file_size(file_name, dir_name, &file_size, fs_handle, fs_archive);
				mup_file_size = file_size;

				for(int i = 0; i < (int)format_context->nb_streams; i++)
				{
					if(format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
						stream_num = i;
				}

				ffmpeg_result = avformat_find_stream_info(format_context, NULL);
				if(format_context != NULL)
				{
					Log_log_save(mup_play_thread_string, "avformat_find_stream_info()...[Success] ", ffmpeg_result, false);
					codec = avcodec_find_decoder(format_context->streams[stream_num]->codecpar->codec_id);
					if(!codec)
						Log_log_save(mup_play_thread_string, "avcodec_find_decoder() failed ", -1, false);

					context = avcodec_alloc_context3(codec);
					if(!context)
						Log_log_save(mup_play_thread_string, "alloc failed ", -1, false);

					ffmpeg_result = avcodec_parameters_to_context(context, format_context->streams[stream_num]->codecpar);
					if(ffmpeg_result != 0)
						Log_log_save(mup_play_thread_string, "avcodec_parameters_to_context() failed ", ffmpeg_result, false);

					ffmpeg_result = avcodec_open2(context, codec, NULL);
					if(ffmpeg_result != 0)
						Log_log_save(mup_play_thread_string, "avcodec_open2() failed ", ffmpeg_result, false);
					if(context)
					{
						codec_info = avcodec_descriptor_get(format_context->streams[stream_num]->codecpar->codec_id);
						mup_file_type = codec_info->long_name;
					}
			
					while(true)
					{
						mup_count_request = false;
						while(mup_pause_request && !mup_stop_request && !mup_change_music_request && !mup_seek_request)
							usleep(50000);

						mup_count_request = true;
						if(mup_stop_request || mup_change_music_request || mup_seek_request)
						{
							ndspChnWaveBufClear(8);
							if(mup_seek_request)
							{
								log_num = Log_log_save(mup_play_thread_string, "avformat_seek_file()... ", 1234567890, false);
								//Use us(microsecond) to specify time
								Log_log_save(mup_play_thread_string, std::to_string(mup_offset), 1234567890, false);
								ffmpeg_result = avformat_seek_file(format_context, -1, mup_offset, mup_offset, mup_offset, AVSEEK_FLAG_ANY);
								if(ffmpeg_result >= 0)
								{
									mup_bar_pos = mup_offset / 1000;
									current_pos[0] = mup_bar_pos;
									current_pos[1] = mup_bar_pos;
									memset(sound_buffer[0], 0x0, 0x20000);
									memset(sound_buffer[1], 0x0, 0x20000);
									buffer_offset = 0;
									Log_log_add(log_num, Err_query_template_summary(0), ffmpeg_result, false);
								}
								else
									Log_log_add(log_num, Err_query_template_summary(1024), ffmpeg_result, false);

								mup_seek_request = false;
							}
							else
								break;
						}

						packet = av_packet_alloc();
						if(!packet)
						{
							Log_log_save(mup_play_thread_string, "av_packet_alloc()...[Error] ", -1, false);
							break;
						}
						ffmpeg_result = av_read_frame(format_context, packet);
						if(packet->size == 0)
						{
							Log_log_save(mup_play_thread_string, "av_read_frame()...[Error] ", ffmpeg_result, false);
							if(ffmpeg_result <= 0)
								break;
						}
						else
						{
							if(packet->stream_index == stream_num)
							{
								raw_data = av_frame_alloc();
								if(!raw_data)
									Log_log_save(mup_play_thread_string, "av_frame_alloc()...[Error] ", -1, false);
								else
								{
									ffmpeg_result = avcodec_send_packet(context, packet);
									if(ffmpeg_result != 0)
										Log_log_save(mup_play_thread_string, "avcodec_send_packet()...[Error] ", ffmpeg_result, false);

									ffmpeg_result = avcodec_receive_frame(context, raw_data);
									if(ffmpeg_result != 0)
										Log_log_save(mup_play_thread_string, "avcodec_receive_frame()...[Error] ", ffmpeg_result, false);
									else
									{
										if(init_swr)
										{
											swr_context = swr_alloc();
											swr_alloc_set_opts(swr_context, av_get_default_channel_layout(raw_data->channels), AV_SAMPLE_FMT_S16, raw_data->sample_rate,
												av_get_default_channel_layout(raw_data->channels), context->sample_fmt, raw_data->sample_rate, 0, NULL);
											if(!swr_context)
												Log_log_save(mup_play_thread_string, "swr_alloc_set_opts()...[Error] ", 1234567890, false);

											swr_init(swr_context);
											mup_music_sample_rate = raw_data->sample_rate;
											mup_num_of_music_ch = raw_data->channels;
											mup_music_bit_rate = format_context->bit_rate;
											mup_music_length = (double)format_context->duration / AV_TIME_BASE;
											ndspChnReset(8);
											ndspChnWaveBufClear(8);
											float mix[12];
												memset(mix, 0, sizeof(mix));
												mix[0] = 1.0;
												mix[1] = 1.0;
												ndspChnSetMix(8, mix);
											memset(ndsp_buffer, 0, sizeof(ndsp_buffer));
											ndspSetOutputMode(NDSP_OUTPUT_STEREO);
											ndspChnSetInterp(8, NDSP_INTERP_LINEAR);
											ndspChnSetRate(8, raw_data->sample_rate);
											ndspChnSetFormat(8, NDSP_FORMAT_STEREO_PCM16);
											init_swr = false;
										}

										av_samples_alloc(&cache, NULL, raw_data->channels, raw_data->nb_samples, AV_SAMPLE_FMT_S16, 0);
										swr_convert(swr_context, &cache, raw_data->nb_samples, (const uint8_t**)raw_data->data, raw_data->nb_samples);
										audio_size = av_samples_get_buffer_size(NULL, raw_data->channels, raw_data->nb_samples, AV_SAMPLE_FMT_S16, 1);
										samples += raw_data->nb_samples;
										memcpy(sound_buffer[buffer_num] + buffer_offset, cache, audio_size);
										buffer_offset += audio_size;
										av_freep(&cache);

										if(buffer_offset + audio_size > 0x20000)
										{
											current_pos[buffer_num] = (double)raw_data->pkt_pos * 8 / mup_music_bit_rate * 1000;
											ndsp_buffer[buffer_num].data_vaddr = sound_buffer[buffer_num];
											ndsp_buffer[buffer_num].nsamples = samples;
											ndspChnWaveBufAdd(8, &ndsp_buffer[buffer_num]);
											DSP_FlushDataCache(sound_buffer[buffer_num], 0x20000);

											while(ndsp_buffer[pre_buffer_num].status == NDSP_WBUF_PLAYING || ndsp_buffer[pre_buffer_num].status == NDSP_WBUF_QUEUED
											&& !mup_stop_request && !mup_change_music_request && !mup_seek_request)
												usleep(25000);

											mup_bar_pos = current_pos[pre_buffer_num];
											pre_buffer_num = buffer_num;
											if(buffer_num == 0)
												buffer_num = 1;
											else
												buffer_num = 0;

											memset(sound_buffer[buffer_num], 0x0, 0x20000);
											samples = 0;
											buffer_offset = 0;
										}
									}
								}
								av_frame_free(&raw_data);
							}
						}
						av_packet_free(&packet);
					}

					ffmpeg_result = avcodec_send_packet(context, NULL);
					for(int i = 0; i < 100; i++)
					{
						raw_data = av_frame_alloc();
						ffmpeg_result = avcodec_receive_frame(context, raw_data);
						av_frame_free(&raw_data);
						if(ffmpeg_result == AVERROR_EOF)
							break;
					}

				}
				else
				{
					Err_set_error_message(Err_query_template_summary(FFMPEG_RETURNED_NOT_SUCCESS), "avformat_find_stream_info() failed", mup_play_thread_string, ffmpeg_result);
					Err_set_error_show_flag(true);
					Log_log_save(mup_play_thread_string, "avformat_find_stream_info()...[Error] ", ffmpeg_result, false);
				}
			}
			while(ndsp_buffer[0].status == NDSP_WBUF_PLAYING || ndsp_buffer[0].status == NDSP_WBUF_QUEUED 
			|| ndsp_buffer[1].status == NDSP_WBUF_PLAYING || ndsp_buffer[1].status == NDSP_WBUF_QUEUED)
			{
				if(mup_stop_request || mup_change_music_request)
					break;

				usleep(25000);
			}


			avformat_close_input(&format_context);
			avcodec_free_context(&context);
			av_packet_free(&packet);
			swr_free(&swr_context);
			mup_count_request = false;
			if (!mup_loop_request && !mup_change_music_request)
				mup_play_request = false;
		}
		else
			usleep(ACTIW_THREAD_SLEEP_TIME);

		while (mup_thread_suspend && !mup_loop_request && !mup_change_music_request)
			usleep(INACTIW_THREAD_SLEEP_TIME);
	}

	for (int i = 0; i < 2; i++)
	{
		linearFree(sound_buffer[i]);
		sound_buffer[i] = NULL;
	}

	Log_log_save(mup_play_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Mup_exit(void)
{
	Log_log_save(mup_exit_string, "Exiting...", 1234567890, FORCE_DEBUG);
	u64 time_out = 10000000000;
	int log_num;
	bool failed = false;
	Result_with_string result;

	if(mup_play_request)
		mup_stop_request = true;

	mup_already_init = false;
	mup_thread_suspend = false;
	mup_play_thread_run = false;
	mup_timer_thread_run = false;
	mup_worker_thread_run = false;

	aptSetSleepAllowed(true);
	Draw_progress("[Mup] Exiting...");

	for (int i = 0; i < 3; i++)
	{
		log_num = Log_log_save(mup_exit_string, "threadJoin()" + std::to_string(i) + "/1...", 1234567890, FORCE_DEBUG);

		if(i == 0)
			result.code = threadJoin(mup_play_thread, time_out);
		else if(i == 1)
			result.code = threadJoin(mup_timer_thread, time_out);
		else if(i == 2)
			result.code = threadJoin(mup_worker_thread, time_out);

		if (result.code == 0)
			Log_log_add(log_num, Err_query_template_summary(0), result.code, FORCE_DEBUG);
		else
		{
			failed = true;
			Log_log_add(log_num, Err_query_template_summary(-1024), result.code, FORCE_DEBUG);
		}
	}

	threadFree(mup_play_thread);
	threadFree(mup_timer_thread);
	threadFree(mup_worker_thread);

	if (failed)
		Log_log_save(mup_exit_string, "[Warn] Some function returned error.", 1234567890, FORCE_DEBUG);
}

void Mup_init(void)
{
	Log_log_save(mup_init_string, "Initializing...", 1234567890, FORCE_DEBUG);
	bool failed = false;
	int log_num;
	Result_with_string result;

	Draw_progress("0/0 [Mup] Starting threads...");
	if (!failed)
	{
		mup_play_thread_run = true;
		mup_timer_thread_run = true;
		mup_worker_thread_run = true;
		mup_play_thread = threadCreate(Mup_play_thread, (void*)(""), STACKSIZE, PRIORITY_LOW, 0, false);
		mup_timer_thread = threadCreate(Mup_timer_thread, (void*)(""), STACKSIZE, PRIORITY_REALTIME, 1, false);
		mup_worker_thread = threadCreate(Mup_worker_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, 0, false);
	}

	aptSetSleepAllowed(false);
	mup_music_length = 0.0;
	mup_bar_pos = 0.0;
	mup_music_sample_rate = 0;
	mup_num_of_music_ch = 0;
	mup_music_bit_rate = 0;
	mup_file_type = "unknown";

	Mup_resume();
	mup_already_init = true;
	Log_log_save(mup_init_string, "Initialized", 1234567890, FORCE_DEBUG);
}

void Mup_main(void)
{
	int msg_num = 0;
	float text_red, text_green, text_blue, text_alpha;
	float red[2], green[2], blue[2], alpha[2];
	Hid_info key;

	if(mup_dl_and_play_request)
	{
		mup_dled_size = Httpc_query_dled_size(MUP_HTTP_PORT0);
		mup_dl_progress = Httpc_query_dl_progress(MUP_HTTP_PORT0);
	}

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

	for (int i = 0; i < 2; i++)
	{
		red[i] = text_red;
		green[i] = text_green;
		blue[i] = text_blue;
		alpha[i] = text_alpha;
	}

	if(mup_pre_music_bit_rate != mup_music_bit_rate || mup_pre_music_sample_rate != mup_music_sample_rate || mup_pre_num_of_music_ch != mup_num_of_music_ch
		|| mup_pre_music_length != mup_music_length || mup_pre_file_type != mup_file_type || mup_pre_bar_pos != mup_bar_pos || mup_pre_loop_request != mup_loop_request
		|| mup_pre_select_file_request != mup_select_file_request || mup_pre_shuffle_request != mup_shuffle_request || mup_pre_allow_sleep != mup_allow_sleep
		|| mup_dled_size != mup_pre_dled_size || mup_dl_progress != mup_pre_dl_progress)
	{
		mup_pre_music_bit_rate = mup_music_bit_rate;
		mup_pre_music_sample_rate = mup_music_sample_rate;
		mup_pre_num_of_music_ch = mup_num_of_music_ch;
		mup_pre_music_length = mup_music_length;
		mup_pre_file_type = mup_file_type;
		mup_pre_bar_pos = mup_bar_pos;
		mup_pre_loop_request = mup_loop_request;
		mup_pre_select_file_request = mup_select_file_request;
		mup_pre_shuffle_request = mup_shuffle_request;
		mup_pre_allow_sleep = mup_allow_sleep;
		mup_pre_dled_size = mup_dled_size;
		mup_pre_dl_progress = mup_dl_progress;
		mup_need_reflesh = true;
	}

	Hid_query_key_state(&key);
	Log_main();
	if(Draw_query_need_reflesh() || !Sem_query_settings(SEM_ECO_MODE) || Expl_query_need_reflesh())
		mup_need_reflesh = true;

	if(!mup_select_file_request)
		Hid_key_flag_reset();

	if(mup_need_reflesh)
	{
		Draw_frame_ready();
		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);

		if(mup_dl_and_play_request)
		{
			Draw_texture(Square_image, aqua_tint, 0, 0.0, 15.0, 50.0 * mup_dl_progress, 3.0);
			Draw(mup_msg[20] + std::to_string(mup_dled_size / 1024.0 / 1024.0).substr(0, 4) + "MB", 0, 0.0, 20.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		}
		Draw(mup_msg[0] + std::to_string(mup_music_bit_rate / 1000) + mup_msg[1], 0, 0.0, 35.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(mup_msg[2] + std::to_string((double)mup_music_sample_rate / 1000.0).substr(0, std::to_string((double)mup_music_sample_rate / 1000.0).length() - 4) + "Kbps", 0, 0.0, 50.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(mup_msg[3] + std::to_string(mup_num_of_music_ch) + mup_msg[4], 0, 0.0, 65.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(mup_msg[5] + Sem_convert_seconds_to_time(mup_music_length), 0, 0.0, 80.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(mup_msg[7] + mup_file_type, 0, 0.0, 95.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw_top_ui();

		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(1, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(1, true, 2, 1.0, 1.0, 1.0);

		for (int i = 0; i < 2; i++)
			Draw_texture(Square_image, weak_aqua_tint, 0, 105.0 + (i * 60.0), 60.0, 50.0, 50.0);

		if(mup_play_request && !mup_pause_request)
			msg_num = 19;
		else
			msg_num = 16;
		
		Draw(mup_msg[msg_num], 0, 107.5, 80.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(mup_msg[17], 0, 167.5, 80.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);

		Draw(mup_ver, 0, 0.0, 0.0, 0.4, 0.4, 0.0, 1.0, 0.0, 1.0);
		Draw(Sem_convert_seconds_to_time(mup_bar_pos / 1000) + " / " + Sem_convert_seconds_to_time(mup_music_length), 0, 12.5, 105.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw_texture(Square_image, aqua_tint, 0, 10.0, 120.0, 300.0, 8.0);
		if(mup_music_length != 0.0)
			Draw_texture(Square_image, red_tint, 0, 10.0, 120.0, 300.0 * ((mup_bar_pos / 1000) / mup_music_length), 8.0);

		red[!mup_loop_request] = 1.0;
		green[!mup_loop_request] = 0.0;
		blue[!mup_loop_request] = 0.0;
		alpha[!mup_loop_request] = 1.0;

		Draw(mup_msg[9], 0, 12.5, 165.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		for (int i = 0; i < 2; i++)
		{
			Draw_texture(Square_image, weak_aqua_tint, 0, 10.0 + (i * 50.0), 180.0, 40.0, 20.0);
			Draw(mup_msg[10 + i], 0, 12.5 + (i * 50.0), 180.0, 0.5, 0.5, red[i], green[i], blue[i], alpha[i]);
		}

		for (int i = 0; i < 2; i++)
		{
			red[i] = text_red;
			green[i] = text_green;
			blue[i] = text_blue;
			alpha[i] = text_alpha;
		}
		red[!mup_allow_sleep] = 1.0;
		green[!mup_allow_sleep] = 0.0;
		blue[!mup_allow_sleep] = 0.0;
		alpha[!mup_allow_sleep] = 1.0;

		Draw(mup_msg[12], 0, 122.5, 165.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		for (int i = 0; i < 2; i++)
		{
			Draw_texture(Square_image, weak_aqua_tint, 0, 120.0 + (i * 50.0), 180.0, 40.0, 20.0);
			Draw(mup_msg[13 + i], 0, 122.5 + (i * 50.0), 180.0, 0.4, 0.4, red[i], green[i], blue[i], alpha[i]);
		}

		Draw_texture(Square_image, weak_aqua_tint, 0, 230.0, 180.0, 80.0, 20.0);
		Draw(mup_msg[15], 0, 232.5, 180.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);

		for (int i = 0; i < 2; i++)
		{
			red[i] = text_red;
			green[i] = text_green;
			blue[i] = text_blue;
			alpha[i] = text_alpha;
		}
		red[!mup_shuffle_request] = 1.0;
		green[!mup_shuffle_request] = 0.0;
		blue[!mup_shuffle_request] = 0.0;
		alpha[!mup_shuffle_request] = 1.0;

		Draw(mup_msg[18], 0, 12.5, 135.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		for (int i = 0; i < 2; i++)
		{
			Draw_texture(Square_image, weak_aqua_tint, 0, 10.0 + (i * 50.0), 150.0, 40.0, 20.0);
			Draw(mup_msg[10 + i], 0, 12.5 + (i * 50.0), 150.0, 0.5, 0.5, red[i], green[i], blue[i], alpha[i]);
		}

		if (mup_select_file_request)
			Expl_draw(mup_msg[8]);

		Draw_bot_ui();
		Draw_touch_pos();

		Draw_apply_draw();
		mup_need_reflesh = false;
	}
	else
		gspWaitForVBlank();

	if (Err_query_error_show_flag())
	{
		if (key.p_touch && key.touch_x >= 150 && key.touch_x <= 170 && key.touch_y >= 150 && key.touch_y <= 169)
			Err_set_error_show_flag(false);
	}
	else if (mup_select_file_request)
		Expl_main();
	else
	{
		if (key.p_start || (key.p_touch && key.touch_x >= 110 && key.touch_x <= 230 && key.touch_y >= 220 && key.touch_y <= 240))
			Mup_suspend();
		else if(key.p_touch && key.touch_x >= 10 && key.touch_x <= 310 && key.touch_y >= 120 && key.touch_y <= 127)
		{
			mup_offset = (key.touch_x - 10) * (mup_music_length / 300);
			mup_offset *= 1000000;
			mup_seek_request = true;
		}
		if (key.p_a || (key.p_touch && key.touch_x >= 105 && key.touch_x <= 154 && key.touch_y >= 60 && key.touch_y <= 109))
		{
			if(!mup_play_request)
				mup_play_request = true;
			else if(!mup_pause_request)
				mup_pause_request = true;
			else
				mup_pause_request = false;
		}
		else if (key.p_b || (key.p_touch && key.touch_x >= 165 && key.touch_x <= 214 && key.touch_y >= 60 && key.touch_y <= 109))
			mup_stop_request = true;
		else if (key.p_touch && key.touch_x >= 10 && key.touch_x <= 49 && key.touch_y >= 150 && key.touch_y <= 169)
			mup_shuffle_request = true;
		else if (key.p_touch && key.touch_x >= 60 && key.touch_x <= 99 && key.touch_y >= 150 && key.touch_y <= 169)
			mup_shuffle_request = false;
		else if (key.p_touch && key.touch_x >= 10 && key.touch_x <= 49 && key.touch_y >= 180 && key.touch_y <= 199)
			mup_loop_request = true;
		else if (key.p_touch && key.touch_x >= 60 && key.touch_x <= 99 && key.touch_y >= 180 && key.touch_y <= 199)
			mup_loop_request = false;
		else if (key.p_touch && key.touch_x >= 110 && key.touch_x <= 149 && key.touch_y >= 180 && key.touch_y <= 199)
			Mup_set_allow_sleep(true);
		else if (key.p_touch && key.touch_x >= 160 && key.touch_x <= 199 && key.touch_y >= 180 && key.touch_y <= 199)
			Mup_set_allow_sleep(false);
		else if (key.p_x || (key.p_touch && key.touch_x >= 230 && key.touch_x <= 309 && key.touch_y >= 180 && key.touch_y <= 199))
		{
			void (*callback)(std::string, std::string);
			void (*cancel_callback)(void);
			callback = Mup_set_load_file;
			cancel_callback = Mup_cancel_select_file;
			Expl_set_callback(callback);
			Expl_set_cancel_callback(cancel_callback);
			mup_select_file_request = true;
			Expl_set_operation_flag(EXPL_READ_DIR_REQUEST, true);
		}
	}
}
