#pragma once

#define ERR_SUMMARY 0
#define ERR_DESCRIPTION 1
#define ERR_PLACE 2
#define ERR_CODE 3

#define OUT_OF_MEMORY -1
#define OUT_OF_LINEAR_MEMORY -2
#define GAS_RETURNED_NOT_SUCCESS -3
#define STB_IMG_RETURNED_NOT_SUCCESS -4
#define BUFFER_SIZE_IS_TOO_SMALL -5
#define FILE_SIZE_IS_TOO_BIG -6
#define WRONG_PARSING_POS -7
#define TOO_MANY_MESSAGES -8
#define INVALID_ARG -9
#define BAD_ICON_INFO -10
#define NO_ICON_INFO -11

bool Err_query_error_show_flag(void);

std::string Err_query_error_data(int error_num);

void Err_set_error_code(long error_code);

void Err_set_error_data(int error_num, std::string error_data);

std::string Err_query_general_error_string(void);

std::string Err_query_general_success_string(void);

void Err_set_error_message(std::string summary, std::string description, std::string place, long error_code);

void Err_set_error_show_flag(bool flag);

void Err_clear_error_message(void);

std::string Err_dec_to_hex_string(long dec);
