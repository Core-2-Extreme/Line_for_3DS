#pragma once

#define MUP_NUM_OF_MSG 19

#define MUP_HTTP_PORT0 8

bool Mup_query_init_flag(void);

bool Mup_query_running_flag(void);

void Mup_resume(void);

void Mup_suspend(void);

void Mup_cancel_select_file(void);

void Mup_set_msg(int msg_num, std::string msg);

void Mup_set_allow_sleep(bool flag);

void Mup_set_url(std::string url);

void Mup_set_load_file(std::string file_name, std::string dir_name);

Result_with_string Mup_play_sound(u8* sound_buffer, int buffer_size, int sample_rate, int num_of_ch, int play_ch);

void Mup_timer_thread(void* arg);

void Mup_play_thread(void* arg);

void Mup_exit(void);

void Mup_init(void);

void Mup_main(void);
