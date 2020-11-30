#pragma once

#define GTR_NUM_OF_MSG 10
#define GTR_NUM_OF_LANG_LIST_MSG 105
#define GTR_NUM_OF_LANG_SHORT_LIST_MSG 105

#define GTR_SHORT_LANG_LIST 0
#define GTR_LANG_LIST 1
#define GTR_MSG 2

#define GTR_HTTP_POST_PORT0 2

bool Gtr_query_init_flag(void);

bool Gtr_query_running_flag(void);

void Gtr_set_msg(int msg_num, int num_of_msg, std::string msg);

void Gtr_suspend(void);

void Gtr_resume(void);

std::string Gtr_get_lang_name(std::string short_name);

void Gtr_init(void);

void Gtr_exit(void);

void Gtr_main(void);

void Gtr_tr_thread(void* arg);
