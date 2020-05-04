#pragma once

#define GTR_TYPE_TEXT_REQUEST 0
#define GTR_SELECT_SORCE_LANG_REQUEST 1
#define GTR_SELECT_TARGET_LANG_REQUEST 2

#define GTR_SELECTED_HISTORY_NUM 0

#define GTR_SELECTED_SORCE_LANG_NUM_D 0
#define GTR_SELECTED_TARGET_LANG_NUM_D 1

#define GTR_SORCE_LANG_OFFSET 0
#define GTR_TARGET_LANG_OFFSET 1

bool Gtr_query_init_flag(void);

double Gtr_query_offset(int item_num);

bool Gtr_query_operation_flag(int operation_num);

bool Gtr_query_running_flag(void);

int Gtr_query_selected_num(int item_num);

double Gtr_query_selected_num_d(int item_num);

double Gtr_query_text_pos_x(void);

std::string Gtr_query_tr_history(int num);

void Gtr_set_offset(int item_num, double value);

void Gtr_set_operation_flag(int operation_num, bool flag);

void Gtr_set_selected_num(int item_num, int value);

void Gtr_set_selected_num_d(int item_num, double value);

void Gtr_set_sorce_lang(int lang_num);

void Gtr_set_target_lang(int lang_num);

void Gtr_set_text_pos_x(double x);

void Gtr_suspend(void);

void Gtr_resume(void);

std::string Gtr_get_lang_name(std::string short_name);

void Gtr_init(void);

void Gtr_exit(void);

void Gtr_main(void);

void Gtr_tr_thread(void* arg);
