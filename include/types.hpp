#pragma once

#define STACKSIZE (64 * 1024)

struct Result_with_string
{
	std::string string = "[Success] ";
	std::string error_description = "";
	Result code = 0;
};
