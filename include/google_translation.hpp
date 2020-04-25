#pragma once

#define GTR_TYPE_TEXT_REQUEST 0

bool Gtr_query_init_flag(void);

bool Gtr_query_running_flag(void);

int Gtr_query_selected_num(void);

double Gtr_query_text_pos_x(void);

std::string Gtr_query_tr_history(int num);

void Gtr_set_selected_num(int num);

void Gtr_set_operation_flag(int operation_num, bool flag);

void Gtr_set_text_pos_x(double x);

void Gtr_suspend(void);

void Gtr_resume(void);

std::string Gtr_get_lang_name(std::string short_name);

void Gtr_init(void);

void Gtr_exit(void);

void Gtr_main(void);

void Gtr_tr_thread(void* arg);
