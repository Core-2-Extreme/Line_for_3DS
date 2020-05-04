#include <3ds.h>
#include <cstring>
#include <string>

#include "draw.hpp"

bool log_show_logs = false;
int log_current_log_num = 0; 
int log_y = 0;
double log_x = 0.0;
double log_up_time_ms = 0.0;
double log_spend_time[512];
std::string log_logs[512];
TickCounter log_up_time_timer;

void Log_start_up_time_timer(void)
{
	osTickCounterStart(&log_up_time_timer);
}

double Log_query_x(void)
{
	return log_x;
}

int Log_query_y(void)
{
	return log_y;
}

bool Log_query_log_show_flag(void)
{
	return log_show_logs;
}

std::string Log_query_log(int log_num)
{
	if (log_num >= 0 && log_num <= 511)
		return log_logs[log_num];
	else
		return "";
}

void Log_set_x(double value)
{
	log_x = value;
}

void Log_set_y(int value)
{
	log_y = value;
}

void Log_set_log_show_flag(bool flag)
{
	log_show_logs = flag;
}

int Log_log_save(std::string type, std::string text, Result result, bool draw)
{
	double time_cache;
	char app_log_cache[4096];
	osTickCounterUpdate(&log_up_time_timer);
	time_cache = osTickCounterRead(&log_up_time_timer);
	log_up_time_ms = log_up_time_ms + time_cache;
	log_spend_time[log_current_log_num] = log_up_time_ms;
	time_cache = log_up_time_ms / 1000;
	memset(app_log_cache, 0x0, 4096);

	if (result == 1234567890)
		sprintf(app_log_cache, "[%.5f][%s] %s", time_cache, type.c_str(), text.c_str());
	else
		sprintf(app_log_cache, "[%.5f][%s] %s0x%lx", time_cache, type.c_str(), text.c_str(), result);

	log_logs[log_current_log_num] = app_log_cache;
	log_current_log_num++;
	if (log_current_log_num >= 512)
		log_current_log_num = 0;

	if (log_current_log_num < 23)
		log_y = 0;
	else
		log_y = log_current_log_num - 23;

	if (draw)
		Draw_log();

	return (log_current_log_num - 1);
}

void Log_log_add(int add_log_num, std::string add_text, Result result, bool draw)
{
	double time_cache;
	char app_log_add_cache[4096];
	osTickCounterUpdate(&log_up_time_timer);
	time_cache = osTickCounterRead(&log_up_time_timer);
	log_up_time_ms = log_up_time_ms + time_cache;
	time_cache = log_up_time_ms - log_spend_time[add_log_num];
	memset(app_log_add_cache, 0x0, 4096);

	if (result != 1234567890)
		sprintf(app_log_add_cache, "%s0x%lx (%.2fms)", add_text.c_str(), result, time_cache);
	else
		sprintf(app_log_add_cache, "%s (%.2fms)", add_text.c_str(), time_cache);

	if (draw)
		Draw_log();

	log_logs[add_log_num] += app_log_add_cache;
}
