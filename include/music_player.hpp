#pragma once

#define MUP_NUM_OF_MSG 19

#define MUP_PLAY_MUSIC_REQUEST 0
#define MUP_STOP_MUSIC_REQUEST 1
#define MUP_SELECT_FILE_REQUEST 2
#define MUP_LOOP_REQUEST 3
#define MUP_SHUFFLE_REQUEST 4
#define MUP_CHANGE_MUSIC_REQUEST 5

#define MUP_FS_OUT_BUFFER 0
#define MUP_FS_IN_BUFFER 1

bool Mup_query_init_flag(void);

bool Mup_query_running_flag(void);

void Mup_resume(void);

void Mup_suspend(void);

int Mup_query_buffer_size(int buffer_num);

bool Mup_query_operation_flag(int operation_num);

void Mup_set_buffer_size(int buffer_num, int size);

void Mup_set_msg(int msg_num, std::string msg);

void Mup_set_offset(int offset);

void Mup_set_operation_flag(int operation_num, bool flag);

void Mup_set_allow_sleep(bool flag);

void Mup_set_load_file_name(std::string file_name);

void Mup_set_load_dir_name(std::string dir_name);

Result_with_string Mup_play_sound(u8* sound_buffer, int buffer_size, int sample_rate, int num_of_ch);

void Mup_timer_thread(void* arg);

void Mup_play_thread(void* arg);

void Mup_exit(void);

void Mup_init(void);

void Mup_main(void);
