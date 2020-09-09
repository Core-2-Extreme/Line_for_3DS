﻿#include <3ds.h>
#include <string>
#include <unistd.h>
#include <algorithm>

#include "setting_menu.hpp"
#include "file.hpp"
#include "explorer.hpp"
#include "log.hpp"

/*For draw*/
int expl_pre_num_of_file = 0;
int expl_pre_size[256];
double expl_pre_view_offset_y = 0.0;
double expl_pre_selected_file_num = 0.0;
std::string expl_pre_current_patch = "/";
std::string expl_pre_files[256];
/*---------------------------------------------*/

bool expl_read_dir_thread_run = false;
bool expl_read_dir_request = false;
int expl_num_of_file = 0;
int expl_size[256];
double expl_view_offset_y = 0.0;
double expl_selected_file_num = 0.0;
std::string expl_current_patch = "/";
std::string expl_files[256];
std::string expl_type[256];
std::string expl_read_dir_thread_string = "Expl/Read dir thread thread";
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

bool Expl_query_need_reflesh(void)
{
  bool need = false;

  for(int i = 0; i < 256; i++)
  {
    if(expl_pre_size[i] != expl_size[i] || expl_pre_files[i] != expl_files[i])
    {
      need = true;
      break;
    }
  }

  if(need || expl_pre_num_of_file != expl_num_of_file || expl_pre_view_offset_y != expl_view_offset_y
    || expl_pre_selected_file_num != expl_selected_file_num || expl_pre_current_patch != expl_current_patch)
  {
    for(int i = 0; i < 256; i++)
    {
      expl_pre_files[i] = expl_files[i];
      expl_pre_size[i] = expl_size[i];
    }
    expl_pre_num_of_file = expl_num_of_file;
    expl_pre_view_offset_y = expl_view_offset_y;
    expl_pre_selected_file_num = expl_selected_file_num;
    expl_pre_current_patch = expl_current_patch;
    return true;
  }
  else
    return false;
}

int Expl_query_num_of_file(void)
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

int Expl_query_size(int file_num)
{
	if (file_num >= 0 && file_num <= 255)
		return expl_size[file_num];
	else
		return -1;
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
	expl_read_dir_thread = threadCreate(Expl_read_dir_thread, (void*)(""), STACKSIZE, PRIORITY_HIGHT, -1, false);
}

void Expl_exit(void)
{
	expl_read_dir_thread_run = false;
	threadJoin(expl_read_dir_thread, 10000000000);
	threadFree(expl_read_dir_thread);
}

void Expl_read_dir_thread(void* arg)
{
	Log_log_save(expl_read_dir_thread_string, "Thread started.", 1234567890, false);
	int read_dir_lou_num_return;
	int num_of_hidden = 0;
	int num_of_dir = 0;
	int num_of_file = 0;
	int num_of_read_only = 0;
	int num_of_unknown = 0;
	int num_offset = 0;
  int index = 0;
  u64 file_size;
	std::string name_of_hidden[256];
	std::string name_of_dir[256];
	std::string name_of_file[256];
	std::string name_of_read_only[256];
	std::string name_of_unknown[256];
	std::string sort_cache[256];
  FS_Archive fs_archive = 0;
	Handle fs_handle = 0;
  Result_with_string read_dir_result;

	for (int i = 0; i < 256; i++)
	{
		expl_files[i] = "";
		expl_type[i] = "";
    expl_size[i] = 0;
	}

	while (expl_read_dir_thread_run)
	{
		if (expl_read_dir_request)
		{
			for (int i = 0; i < 256; i++)
			{
				expl_files[i] = "";
				expl_type[i] = "";
        expl_size[i] = 0;
			}

			read_dir_lou_num_return = Log_log_save(expl_read_dir_thread_string, "File_read_dir()...", 1234567890, false);
			read_dir_result = File_read_dir(&expl_num_of_file, expl_files, 256, expl_type, 256, expl_current_patch);
			Log_log_add(read_dir_lou_num_return, read_dir_result.string, read_dir_result.code, false);

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

        std::sort(begin(name_of_hidden), begin(name_of_hidden) + num_of_hidden);
        std::sort(begin(name_of_dir), begin(name_of_dir) + num_of_dir);
        std::sort(begin(name_of_file), begin(name_of_file) + num_of_file);
        std::sort(begin(name_of_read_only), begin(name_of_read_only) + num_of_read_only);
        std::sort(begin(name_of_unknown), begin(name_of_unknown) + num_of_unknown);

				for (int i = 0; i < 256; i++)
				{
					expl_files[i] = "";
					expl_type[i] = "";
				}
        index = 0;

				if (!(expl_current_patch == "/"))
				{
					num_offset = 1;
					expl_num_of_file += 1;
          if (Sem_query_lang() == "jp")
						expl_files[0] = "親ディレクトリへ移動";
  				else
						expl_files[0] = "Move to parent directory";
					}
				else
					num_offset = 0;

				for (int i = 0; i < num_of_hidden; i++)
				{
          index = i + num_offset;
					expl_type[index] = "hidden";
					expl_files[index] = name_of_hidden[i];
				}
				for (int i = 0; i < num_of_dir; i++)
				{
          index = i + num_of_hidden + num_offset;
					expl_type[index] = "dir";
					expl_files[index] = name_of_dir[i];
				}
				for (int i = 0; i < num_of_file; i++)
				{
          index = i + num_of_hidden + num_of_dir + num_offset;
					expl_type[index] = "file";
					expl_files[index] = name_of_file[i];
				}
				for (int i = 0; i < num_of_read_only; i++)
				{
          index = i + num_of_hidden + num_of_dir + num_of_file + num_offset;
					expl_type[index] = "read only";
					expl_files[index] = name_of_read_only[i];
				}
				for (int i = 0; i < num_of_unknown; i++)
				{
          index = i + num_of_hidden + num_of_dir + num_of_file + num_of_read_only + num_offset;
					expl_type[index] = "unknown";
					expl_files[index] = name_of_unknown[i];
				}
			}
			expl_read_dir_request = false;

      for(int i = 0; i <= index; i++)
      {
        if(expl_read_dir_request)
          break;

        read_dir_result = File_check_file_size(expl_files[i], expl_current_patch, &file_size, fs_handle, fs_archive);
        if(read_dir_result.code == 0)
          expl_size[i] = (int)file_size;
      }
		}
    else
			usleep(ACTIW_THREAD_SLEEP_TIME);
	}
	Log_log_save(expl_read_dir_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}
