#include <3ds.h>
#include <string>
#include <unistd.h>
#include "citro2d.h"

#include "hid.hpp"
#include "share_function.hpp"
#include "file.hpp"
//#include "setting_menu.hpp"
#include "error.hpp"
#include "explorer.hpp"

bool expl_read_dir_thread_run = false;
bool expl_read_dir_request = false;
int expl_num_of_file = 0;
double expl_view_offset_y = 0.0;
double expl_selected_file_num = 0.0;
std::string expl_current_patch = "/";
std::string expl_files[256];
std::string expl_type[256];
Thread expl_read_dir_thread;

std::string Expl_query_current_patch(void)
{
 	return expl_current_patch;
}

std::string Expl_query_file_name(int file_num)
{
	if (file_num >= 0 && file_num <= 255)
		return expl_files[file_num];
	else
		return "";
}

double Expl_query_num_of_file(void)
{
	return expl_num_of_file;
}

bool Expl_query_operation_flag(int operation_num)
{
	if (operation_num == EXPL_READ_DIR_REQUEST)
		return expl_read_dir_request;
	else
		return false;
}

double Expl_query_selected_num(int item_num)
{
	if (item_num == EXPL_SELECTED_FILE_NUM)
		return expl_selected_file_num;
	else
		return -1.0;
}

std::string Expl_query_type(int file_num)
{
	if (file_num >= 0 && file_num <= 255)
		return expl_type[file_num];
	else
		return "";
}

double Expl_query_view_offset_y(void)
{
	return expl_view_offset_y;
}

void Expl_set_current_patch(std::string patch)
{
	expl_current_patch = patch;
}

void Expl_set_operation_flag(int operation_num, bool flag)
{
	if (operation_num == EXPL_READ_DIR_REQUEST)
		expl_read_dir_request = flag;
}

void Expl_set_selected_num(int item_num, double value)
{
	if (item_num == EXPL_SELECTED_FILE_NUM)
		expl_selected_file_num = value;
}

void Expl_set_view_offset_y(double value)
{
	expl_view_offset_y = value;
}

void Expl_init(void)
{
	expl_read_dir_thread_run = true;
	expl_read_dir_thread = threadCreate(Expl_read_dir_thread, (void*)(""), STACKSIZE, 0x28, -1, true);
}

void Expl_exit(void)
{
	expl_read_dir_thread_run = false;
	threadJoin(expl_read_dir_thread, 10000000000);
}

void Expl_read_dir_thread(void* arg)
{
	S_log_save("Share/Read dir thread", "Thread started.", 1234567890, false);
	int read_dir_lou_num_return;
	int num_of_hidden;
	int num_of_dir;
	int num_of_file;
	int num_of_read_only;
	int num_of_unknown;
	int num_offset;
	std::string name_of_hidden[256];
	std::string name_of_dir[256];
	std::string name_of_file[256];
	std::string name_of_read_only[256];
	std::string name_of_unknown[256];
	std::string sort_cache[256];
	std::string name_sample = "!#$%&'()+,-.0123456789;=@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{}~";
	Result_with_string read_dir_result;

	for (int i = 0; i < 256; i++)
	{
		expl_files[i] = "";
		expl_type[i] = "";
	}

	while (expl_read_dir_thread_run)
	{
		if (expl_read_dir_request)
		{
			for (int i = 0; i < 256; i++)
			{
				expl_files[i] = "";
				expl_type[i] = "";
			}

			read_dir_lou_num_return = S_log_save("Share/Read dir thread", "Share_read_dir(" + expl_current_patch + ")...", 1234567890, false);
			read_dir_result = Share_read_dir(&expl_num_of_file, expl_files, 256, expl_type, 256, expl_current_patch);
			S_log_add(read_dir_lou_num_return, read_dir_result.string, read_dir_result.code, false);

			if (read_dir_result.code == 0)
			{
				num_of_hidden = 0;
				num_of_dir = 0;
				num_of_file = 0;
				num_of_read_only = 0;
				num_of_unknown = 0;
				for (int i = 0; i < 256; i++)
				{
					name_of_hidden[i] = "";
					name_of_dir[i] = "";
					name_of_file[i] = "";
					name_of_read_only[i] = "";
					name_of_unknown[i] = "";
					sort_cache[i] = "";
				}

				for (int i = 0; i < expl_num_of_file; i++)
				{
					if (expl_type[i] == "hidden")
					{
						name_of_hidden[num_of_hidden] = expl_files[i];
						num_of_hidden++;
					}
					else if (expl_type[i] == "dir")
					{
						name_of_dir[num_of_dir] = expl_files[i];
						num_of_dir++;
					}
					else if (expl_type[i] == "file")
					{
						name_of_file[num_of_file] = expl_files[i];
						num_of_file++;
					}
					else if (expl_type[i] == "read only")
					{
						name_of_read_only[num_of_read_only] = expl_files[i];
						num_of_read_only++;
					}
					else if (expl_type[i] == "unknown")
					{
						name_of_unknown[num_of_unknown] = expl_files[i];
						num_of_unknown++;
					}
				}

				for (int i = 0; i < 256; i++)
				{
					expl_files[i] = "";
					expl_type[i] = "";
				}

				if (!(expl_current_patch == "/"))
				{
					num_offset = 1;
					expl_num_of_file += 1;
					if (s_setting[1] == "en")
						expl_files[0] = "Move to parent directory";
					else if (s_setting[1] == "jp")
						expl_files[0] = "親ディレクトリへ移動";
				}
				else
					num_offset = 0;

				for (int i = 0; i < num_of_hidden; i++)
				{
					expl_type[i + num_offset] = "hidden";
					expl_files[i + num_offset] = name_of_hidden[i];
				}
				for (int i = 0; i < num_of_dir; i++)
				{
					expl_type[i + num_of_hidden + num_offset] = "dir";
					expl_files[i + num_of_hidden + num_offset] = name_of_dir[i];
				}
				for (int i = 0; i < num_of_file; i++)
				{
					expl_type[i + num_of_hidden + num_of_dir + num_offset] = "file";
					expl_files[i + num_of_hidden + num_of_dir + num_offset] = name_of_file[i];
				}
				for (int i = 0; i < num_of_read_only; i++)
				{
					expl_type[i + num_of_hidden + num_of_dir + num_of_file + num_offset] = "read only";
					expl_files[i + num_of_hidden + num_of_dir + num_of_file + num_offset] = name_of_read_only[i];
				}
				for (int i = 0; i < num_of_unknown; i++)
				{
					expl_type[i + num_of_hidden + num_of_dir + num_of_file + num_of_read_only + num_offset] = "unknown";
					expl_files[i + num_of_hidden + num_of_dir + num_of_file + num_of_read_only + num_offset] = name_of_unknown[i];
				}
			}

			expl_read_dir_request = false;
		}
		usleep(50000);
	}
	S_log_save("Share/Read dir thread", "Thread exit.", 1234567890, false);
}


