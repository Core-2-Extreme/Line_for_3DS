#include <3ds.h>
#include <cstring>
#include <string>

#include "error.hpp"
#include "unicodetochar/unicodetochar.h"
#include "types.hpp"
#include "file.hpp"
#include "log.hpp"

Result_with_string File_save_to_file(std::string file_name, u8* write_data, int size, std::string dir_path, bool delete_old_file, Handle fs_handle, FS_Archive fs_archive)
{
	u32 written_size = 0;
	u64 file_size = 0;
	bool failed = false;
	std::string file_path = dir_path + file_name;
	TickCounter write_time;
	Result_with_string save_file_result;

	save_file_result.code = FSUSER_OpenArchive(&fs_archive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
	if(save_file_result.code != 0)
	{
		save_file_result.string = "[Error] FSUSER_OpenArchive failed. ";
		failed = true;
	}

	if (!failed)
	{
		save_file_result.code = FSUSER_CreateDirectory(fs_archive, fsMakePath(PATH_ASCII, dir_path.c_str()), FS_ATTRIBUTE_DIRECTORY);
		if (save_file_result.code != 0 && save_file_result.code != (s32)0xC82044BE)//#0xC82044BE directory already exist
		{
			save_file_result.string = "[Error] FSUSER_CreateDirectory failed. ";
			failed = true;
		}
	}

	if (!failed)
	{
		if (delete_old_file)
			FSUSER_DeleteFile(fs_archive, fsMakePath(PATH_ASCII, file_path.c_str()));
	}

	if (!failed)
	{
		save_file_result.code = FSUSER_CreateFile(fs_archive, fsMakePath(PATH_ASCII, file_path.c_str()), FS_ATTRIBUTE_ARCHIVE, 0);
		if (save_file_result.code != 0 && save_file_result.code != (s32)0xC82044BE)//#0xC82044BE file already exist
		{
			save_file_result.string = "[Error] FSUSER_CreateFile failed. ";
			failed = true;
		}
	}

	if (!failed)
	{
		save_file_result.code = FSUSER_OpenFile(&fs_handle, fs_archive, fsMakePath(PATH_ASCII, file_path.c_str()), FS_OPEN_WRITE, FS_ATTRIBUTE_ARCHIVE);
		if (save_file_result.code != 0)
		{
			save_file_result.string = "[Error] FSUSER_OpenFile failed. ";
			failed = true;
		}
	}

	if (!failed)
	{
		if (!(delete_old_file))
		{
			save_file_result.code = FSFILE_GetSize(fs_handle, &file_size);
			if (save_file_result.code != 0)
			{
				save_file_result.string = "[Error] FSFILE_GetSize failed. ";
				failed = true;
			}
		}
	}

	if (!failed)
	{
		osTickCounterStart(&write_time);
		save_file_result.code = FSFILE_Write(fs_handle, &written_size, file_size, write_data, size, FS_WRITE_FLUSH);
		osTickCounterUpdate(&write_time);
		if (save_file_result.code == 0)
			save_file_result.string += std::to_string(written_size / 1024) + "KB " + std::to_string(((double)written_size / (osTickCounterRead(&write_time) / 1000.0)) / 1024.0 / 1024.0) + "MB/s ";
		else
		{
			save_file_result.string = "[Error] FSFILE_Write failed. ";
			failed = true;
		}
	}

	FSFILE_Close(fs_handle);
	FSUSER_CloseArchive(fs_archive);
	if(failed)
		save_file_result.error_description = "sdmc:" + file_path;

	return save_file_result;
}

Result_with_string File_load_from_file(std::string file_name, u8* read_data, int max_size, u32* read_size, std::string dir_path, Handle fs_handle, FS_Archive fs_archive)
{
	return File_load_from_file_with_range(file_name, read_data, max_size, 0, read_size, dir_path, fs_handle, fs_archive);
}

Result_with_string File_load_from_rom(std::string file_name, u8* read_data, int max_size, u32* read_size, std::string dir_path)
{
	bool failed = false;
	size_t read_size_;
	u64 file_size;
	std::string file_path = dir_path + file_name;
	Result_with_string result;

	FILE* f = fopen(file_path.c_str(), "rb");
	if (f == NULL)
	{
		result.string = "[Error] fopen failed. ";
		failed = true;
	}

	if (!failed)
	{
		fseek(f, 0, SEEK_END);
		file_size = ftell(f);
		rewind(f);

		if((int)file_size > max_size)
		{
			result.code = BUFFER_SIZE_IS_TOO_SMALL;
			result.string = Err_query_template_summary(BUFFER_SIZE_IS_TOO_SMALL) + std::to_string(file_size);
			result.error_description = Err_query_template_detail(BUFFER_SIZE_IS_TOO_SMALL);
			failed = true;
		}
	}

	if (!failed)
	{
		read_size_ = fread(read_data, 1, file_size, f);
		*read_size = read_size_;
		if (read_size_ != file_size)
		{
			result.string = "[Error] fread failed. ";
			failed = true;
		}
	}
	fclose(f);

	if(failed)
		result.error_description = "romfs:" + file_path;

	return result;
}

Result_with_string File_load_from_file_with_range(std::string file_name, u8* read_data, int read_length, u64 read_offset, u32* read_size, std::string dir_path, Handle fs_handle, FS_Archive fs_archive)
{
	bool failed = false;
	u32 read_size_calc;
	std::string file_path = dir_path + file_name;
	TickCounter read_time;
	Result_with_string result;

	result.code = FSUSER_OpenArchive(&fs_archive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
	if (result.code != 0)
	{
		result.string = "[Error] FSUSER_OpenArchive failed. ";
		failed = true;
	}

	if (!failed)
	{
		result.code = FSUSER_OpenFile(&fs_handle, fs_archive, fsMakePath(PATH_ASCII, file_path.c_str()), FS_OPEN_READ, FS_ATTRIBUTE_ARCHIVE);
		if (result.code != 0)
		{
			result.string = "[Error] FSUSER_OpenFile failed. ";
			failed = true;
		}
	}

	if (!failed)
	{
		osTickCounterStart(&read_time);
		result.code = FSFILE_Read(fs_handle, &read_size_calc, read_offset, read_data, read_length);
		osTickCounterUpdate(&read_time);
		*read_size = read_size_calc;
		if (result.code == 0)
			result.string += std::to_string(read_size_calc / 1024) + "KB " + std::to_string(((double)read_size_calc / (osTickCounterRead(&read_time) / 1000.0)) / 1024.0 / 1024.0) + "MB/s ";
		if (result.code != 0)
		{
			result.string = "[Error] FSFILE_Read failed. ";
			failed = true;
		}
	}

	FSFILE_Close(fs_handle);
	FSUSER_CloseArchive(fs_archive);
	if(failed)
		result.error_description = "sdmc:" + file_path;

	return result;
}

Result_with_string File_delete_file(std::string file_name, std::string dir_path, FS_Archive fs_archive)
{
	bool failed = false;
	std::string file_path = dir_path + file_name;
	Result_with_string result;

	result.code = FSUSER_OpenArchive(&fs_archive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
	if (result.code != 0)
	{
		result.string = "[Error] FSUSER_OpenArchive failed. ";
		failed = true;
	}

	if (!failed)
	{
		result.code = FSUSER_DeleteFile(fs_archive, fsMakePath(PATH_ASCII, file_path.c_str()));
		if (result.code != 0)
		{
			result.string = "[Error] FSUSER_DeleteFile failed. ";
			failed = true;
		}
	}

	FSUSER_CloseArchive(fs_archive);
	if(failed)
		result.error_description = "sdmc:" + file_path;

	return result;
}

Result_with_string File_check_file_size(std::string file_name, std::string dir_path, u64* file_size, Handle fs_handle, FS_Archive fs_archive)
{
	bool failed = false;
	std::string file_path = dir_path + file_name;
	Result_with_string result;

	result.code = FSUSER_OpenArchive(&fs_archive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
	if (result.code != 0)
	{
		result.string = "[Error] FSUSER_OpenArchive failed. ";
		failed = true;
	}

	if (!failed)
	{
		result.code = FSUSER_OpenFile(&fs_handle, fs_archive, fsMakePath(PATH_ASCII, file_path.c_str()), FS_OPEN_READ, FS_ATTRIBUTE_ARCHIVE);
		if (result.code != 0)
		{
			result.string = "[Error] FSUSER_OpenFile failed. ";
			failed = true;
		}
	}

	if (!failed)
	{
		result.code = FSFILE_GetSize(fs_handle, file_size);
		if (result.code != 0)
		{
			result.string = "[Error] FSFILE_GetSize failed. ";
			failed = true;
		}
	}

	FSFILE_Close(fs_handle);
	FSUSER_CloseArchive(fs_archive);
	if(failed)
		result.error_description = "sdmc:" + file_path;

	return result;
}

Result_with_string File_check_file_exist(std::string file_name, std::string dir_path, Handle fs_handle, FS_Archive fs_archive)
{
	bool failed = false;
	std::string file_path = dir_path + file_name;
	Result_with_string result;

	result.code = FSUSER_OpenArchive(&fs_archive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
	if (result.code != 0)
	{
		result.string = "[Error] FSUSER_OpenArchive failed. ";
		failed = true;
	}

	if (!failed)
	{
		result.code = FSUSER_OpenFile(&fs_handle, fs_archive, fsMakePath(PATH_ASCII, file_path.c_str()), FS_OPEN_READ, FS_ATTRIBUTE_ARCHIVE);
		if (result.code != 0)
		{
			result.string = "[Error] FSUSER_OpenFile failed. ";
			failed = true;
		}
	}

	FSFILE_Close(fs_handle);
	FSUSER_CloseArchive(fs_archive);
	if(failed)
		result.error_description = "sdmc:" + file_path;

	return result;
}

Result_with_string File_read_dir(int* num_of_detected, std::string file_dir_name[], int name_num_of_array, std::string type[], int type_num_of_array, std::string dir_path)
{
	int count = 0;
	u32 read_entry = 0;
	u32 read_entry_count = 1;
	bool failed = false;
	char* cache;
	FS_DirectoryEntry fs_entry;
	Handle fs_handle;
	FS_Archive fs_archive;
	Result_with_string result;
	cache = (char*)malloc(0x500);

	result.code = FSUSER_OpenArchive(&fs_archive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
	if (result.code != 0)
	{
		result.string = "[Error] FSUSER_OpenArchive failed. ";
		failed = true;
	}

	if (!failed)
	{
		result.code = FSUSER_OpenDirectory(&fs_handle, fs_archive, fsMakePath(PATH_ASCII, dir_path.c_str()));
		if (result.code != 0)
		{
			result.string = "[Error] FSUSER_OpenDirectory failed. ";
			failed = true;
		}
	}

	if (!failed)
	{
		while (true)
		{
			if (count >= name_num_of_array || count >= type_num_of_array)
			{
				result.string = "[Error] array size is too small. ";
				break;
			}
			result.code = FSDIR_Read(fs_handle, &read_entry, read_entry_count, (FS_DirectoryEntry*)&fs_entry);
			if (read_entry == 0)
				break;

			unicodeToChar(cache, fs_entry.name, 512);
			file_dir_name[count] = cache;

			if (fs_entry.attributes == FS_ATTRIBUTE_HIDDEN)
				type[count] = "hidden";
			else if (fs_entry.attributes == FS_ATTRIBUTE_DIRECTORY)
				type[count] = "dir";
			else if (fs_entry.attributes == FS_ATTRIBUTE_ARCHIVE)
				type[count] = "file";
			else if (fs_entry.attributes == FS_ATTRIBUTE_READ_ONLY)
				type[count] = "read only";
			else
				type[count] = "unknown";

			memset(cache, 0x0, 0x500);
			count++;
		}
		*num_of_detected = count;
	}

	FSDIR_Close(fs_handle);
	FSUSER_CloseArchive(fs_archive);
	free(cache);
	return result;
}
