#pragma once

#define SPT_NUM_OF_MSG 12

#define SPT_HTTPC_BUFFER 0

#define SPT_HTTP_PORT0 5

bool Spt_query_init_flag(void);

bool Spt_query_running_flag(void);

int Spt_query_buffer_size(int buffer_num);

void Spt_set_buffer_size(int buffer_num, int size);

void Spt_set_msg(int msg_num, std::string msg);

void Spt_reset_data(void);

void Spt_reset_draw_data(void);

void Spt_resume(void);

void Spt_suspend(void);

void Spt_init(void);

void Spt_exit(void);

void Spt_main(void);

void Spt_timer_thread(void* arg);

void Spt_spt_thread(void* arg);
