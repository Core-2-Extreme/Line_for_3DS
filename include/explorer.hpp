#pragma once

#define EXPL_READ_DIR_REQUEST 0

#define EXPL_SELECTED_FILE_NUM 0

std::string Expl_query_current_patch(void);

std::string Expl_query_file_name(int file_num);

double Expl_query_num_of_file(void);

bool Expl_query_operation_flag(int operation_num);

double Expl_query_selected_num(int item_num);

std::string Expl_query_type(int file_num);

double Expl_query_view_offset_y(void);

void Expl_set_current_patch(std::string patch);

void Expl_set_operation_flag(int operation_num, bool flag);

void Expl_set_selected_num(int item_num, double value);

void Expl_set_view_offset_y(double value);

void Expl_init(void);

void Expl_exit(void);

void Expl_read_dir_thread(void* arg);
