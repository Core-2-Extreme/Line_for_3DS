#pragma once

#define STACKSIZE (64 * 1024)
#define INACTIW_THREAD_SLEEP_TIME 100000
#define ACTIW_THREAD_SLEEP_TIME 50000
 //0x18~0x3F
#define PRIORITY_IDLE 0x36
#define PRIORITY_LOW 0x25
#define PRIORITY_NORMAL 0x24
#define PRIORITY_HIGHT 0x23
#define PRIORITY_REALTIME 0x18
#define DEBUG false

struct Result_with_string
{
	std::string string = "[Success] ";
	std::string error_description = "";
	Result code = 0;
};
