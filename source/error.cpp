#include <string>
#include <3ds.h>
#include "draw.hpp"
#include "share_function.hpp"
#include "error.hpp"

bool err_error_display;
std::string err_error_summary = "N/A";
std::string err_error_description = "N/A";
std::string err_error_place = "N/A";
std::string err_error_code = "0x0";

bool Err_query_error_show_flag(void)
{
	return err_error_display;
}

std::string Err_query_error_data(int error_num)
{
	if (error_num == ERR_SUMMARY)
		return err_error_summary;
	else if (error_num == ERR_DESCRIPTION)
		return err_error_description;
	else if (error_num == ERR_PLACE)
		return err_error_place;
	else if (error_num == ERR_CODE)
		return err_error_code;
	else
		return "";
}

void Err_set_error_code(long error_code)
{
	if (error_code == 1234567890)
		err_error_code = "N/A";
	else
		err_error_code = Err_dec_to_hex_string(error_code);
}

void Err_set_error_data(int error_num, std::string error_data)
{
	if (error_num == ERR_SUMMARY)
		err_error_summary = error_data;
	else if (error_num == ERR_DESCRIPTION)
		err_error_description = error_data;
	else if (error_num == ERR_PLACE)
		err_error_place = error_data;
}

void Err_set_error_message(std::string summary, std::string description, std::string place, long error_code)
{
	Err_clear_error_message();
	err_error_summary = summary;
	err_error_description = description;
	err_error_place = place;
	if (error_code == 1234567890)
		err_error_code = "N/A";
	else
		err_error_code = Err_dec_to_hex_string(error_code);
}

void Err_set_error_show_flag(bool flag)
{
	err_error_display = flag;
}

void Err_clear_error_message(void)
{
	err_error_summary = "N/A";
	err_error_description = "N/A";
	err_error_place = "N/A";
	err_error_code = Err_dec_to_hex_string(0);
}

std::string Err_dec_to_hex_string(long dec)
{
	char hex_string[128];
	sprintf(hex_string, "0x%lx", dec);
	return hex_string;
}
