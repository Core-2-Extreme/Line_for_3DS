#pragma once

#define SPT_HTTPC_BUFFER 0

bool Spt_query_init_flag(void);

bool Spt_query_running_flag(void);

int Spt_query_buffer_size(int buffer_num);

void Spt_set_buffer_size(int buffer_num, int size);

void Spt_set_spt_data_size(int size);

void Spt_start_request(void);

void Spt_resume(void);

void Spt_suspend(void);

void Spt_init(void);

void Spt_exit(void);

void Spt_main(void);

void Spt_spt_thread(void* arg);
