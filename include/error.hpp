#pragma once

#define ERR_SUMMARY 0
#define ERR_DESCRIPTION 1
#define ERR_PLACE 2
#define ERR_CODE 3

bool Err_query_error_show_flag(void);

std::string Err_query_error_data(int error_num);

void Err_set_error_code(long error_code);

void Err_set_error_data(int error_num, std::string error_data);

void Err_set_error_message(std::string summary, std::string description, std::string place, long error_code);

void Err_set_error_show_flag(bool flag);

void Err_clear_error_message(void);

std::string Err_dec_to_hex_string(long dec);
