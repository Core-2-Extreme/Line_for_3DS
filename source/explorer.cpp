#include <3ds.h>
#include <string>
#include <unistd.h>

#include "share_function.hpp"
#include "file.hpp"
#include "explorer.hpp"
#include "log.hpp"

bool expl_read_dir_thread_run = false;
bool expl_read_dir_request = false;
int expl_num_of_file = 0;
int expl_size[256];
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
	expl_read_dir_thread = threadCreate(Expl_read_dir_thread, (void*)(""), STACKSIZE, 0x24, -1, false);
}

void Expl_exit(void)
{
	expl_read_dir_thread_run = false;
	threadJoin(expl_read_dir_thread, 10000000000);
	threadFree(expl_read_dir_thread);
}

void Expl_read_dir_thread(void* arg)
{
	Log_log_save("Expl/Read dir thread", "Thread started.", 1234567890, false);
	int read_dir_lou_num_return;
	int num_of_hidden;
	int num_of_dir;
	int num_of_file;
	int num_of_read_only;
	int num_of_unknown;
	int num_offset;
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

			read_dir_lou_num_return = Log_log_save("Expl/Read dir thread", "File_read_dir(" + expl_current_patch + ")...", 1234567890, false);
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

				for (int i = 0; i < 256; i++)
				{
					expl_files[i] = "";
					expl_type[i] = "";
				}

				if (!(expl_current_patch == "/"))
				{
					num_offset = 1;
					expl_num_of_file += 1;
					if (s_setting[0] == "en")
						expl_files[0] = "Move to parent directory";
					else if (s_setting[0] == "jp")
						expl_files[0] = "親ディレクトリへ移動";
				}
				else
					num_offset = 0;

				for (int i = 0; i < num_of_hidden; i++)
				{
					expl_type[i + num_offset] = "hidden";
					expl_files[i + num_offset] = name_of_hidden[i];
          read_dir_result = File_check_file_size(expl_files[i + num_offset], expl_current_patch, &file_size, fs_handle, fs_archive);
          if(read_dir_result.code == 0)
            expl_size[i + num_offset] = (int)file_size;
				}
				for (int i = 0; i < num_of_dir; i++)
				{
					expl_type[i + num_of_hidden + num_offset] = "dir";
					expl_files[i + num_of_hidden + num_offset] = name_of_dir[i];
          read_dir_result = File_check_file_size(expl_files[i + num_of_hidden + num_offset], expl_current_patch, &file_size, fs_handle, fs_archive);
          if(read_dir_result.code == 0)
            expl_size[i + num_of_hidden + num_offset] = (int)file_size;
				}
				for (int i = 0; i < num_of_file; i++)
				{
					expl_type[i + num_of_hidden + num_of_dir + num_offset] = "file";
					expl_files[i + num_of_hidden + num_of_dir + num_offset] = name_of_file[i];
          read_dir_result = File_check_file_size(expl_files[i + num_of_hidden + num_of_dir + num_offset], expl_current_patch, &file_size, fs_handle, fs_archive);
          if(read_dir_result.code == 0)
            expl_size[i + num_of_hidden + num_of_dir + num_offset] = (int)file_size;
				}
				for (int i = 0; i < num_of_read_only; i++)
				{
					expl_type[i + num_of_hidden + num_of_dir + num_of_file + num_offset] = "read only";
					expl_files[i + num_of_hidden + num_of_dir + num_of_file + num_offset] = name_of_read_only[i];
          read_dir_result = File_check_file_size(expl_files[i + num_of_hidden + num_of_dir + num_of_file + num_offset], expl_current_patch, &file_size, fs_handle, fs_archive);
          if(read_dir_result.code == 0)
            expl_size[i + num_of_hidden + num_of_dir + num_of_file + num_offset] = (int)file_size;
				}
				for (int i = 0; i < num_of_unknown; i++)
				{
					expl_type[i + num_of_hidden + num_of_dir + num_of_file + num_of_read_only + num_offset] = "unknown";
					expl_files[i + num_of_hidden + num_of_dir + num_of_file + num_of_read_only + num_offset] = name_of_unknown[i];
          read_dir_result = File_check_file_size(expl_files[i + num_of_hidden + num_of_dir + num_of_file + num_of_read_only + num_offset], expl_current_patch, &file_size, fs_handle, fs_archive);
          if(read_dir_result.code == 0)
            expl_size[i + num_of_hidden + num_of_dir + num_of_file + num_of_read_only + num_offset] = (int)file_size;
				}
			}

			expl_read_dir_request = false;
		}
		usleep(50000);
	}
	Log_log_save("Expl/Read dir thread", "Thread exit.", 1234567890, false);
	threadExit(0);
}
