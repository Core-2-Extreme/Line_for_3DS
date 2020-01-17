#include <3ds.h>
#include <string>
#include <cstring>
#include <stdlib.h>
#include "share_function.hpp"

Result_with_string Share_save_to_file(std::string file_name, u8* write_data, int size, std::string dir_path, bool over_write, Handle fs_handle, FS_Archive fs_archive)
{
	u8* read_data;
	u32 written_size = 0;
	u32 read_size = 0;
	bool function_fail = false;
	std::string file_path = dir_path + file_name;
	Result_with_string save_file_result;

	if (!over_write)
	{
		read_data = (u8*)malloc(0x300000);
		memset(read_data, 0x0, 0x300000);
	}
	else
		read_data = NULL;

	save_file_result.code = 0;
	save_file_result.string = "[Success] ";

	save_file_result.code = FSUSER_OpenArchive(&fs_archive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
	if(save_file_result.code != 0)
	{
		save_file_result.string = "[Error] FSUSER_OpenArchive failed. ";
		function_fail = true;
	}

	if (!function_fail)
	{
		save_file_result.code = FSUSER_CreateDirectory(fs_archive, fsMakePath(PATH_ASCII, dir_path.c_str()), FS_ATTRIBUTE_DIRECTORY);
		if (save_file_result.code != 0 && save_file_result.code != (s32)0xC82044BE)//#0xC82044BE directory already exist
		{
			save_file_result.string = "[Error] FSUSER_CreateDirectory failed. ";
			function_fail = true;
		}
	}

	if (!function_fail)
	{
		if (over_write)
			FSUSER_DeleteFile(fs_archive, fsMakePath(PATH_ASCII, file_path.c_str()));
	}

	if (!function_fail)
	{
		save_file_result.code = FSUSER_CreateFile(fs_archive, fsMakePath(PATH_ASCII, file_path.c_str()), FS_ATTRIBUTE_ARCHIVE, 0);
		if (save_file_result.code != 0) 
		{
			save_file_result.string = "[Error] FSUSER_CreateFile failed. ";
			function_fail = true;
		}
	}

	if (!function_fail)
	{
		save_file_result.code = FSUSER_OpenFile(&fs_handle, fs_archive, fsMakePath(PATH_ASCII, file_path.c_str()), FS_OPEN_WRITE, FS_ATTRIBUTE_ARCHIVE);
		if (save_file_result.code != 0)
		{
			save_file_result.string = "[Error] FSUSER_OpenFile failed. ";
			function_fail = true;
		}
	}
	
	if (!function_fail)
	{
		if (!over_write)
		{
			save_file_result.code = FSFILE_Read(fs_handle, &read_size, 0, read_data, 0x300000);
			if (save_file_result.code != 0)
			{
				read_size = 0;
				save_file_result.string = "[Error] FSFILE_Read failed. ";
				function_fail = true;
			}
		}
	}

	if (!function_fail)
	{
		save_file_result.code = FSFILE_Write(fs_handle, &written_size, read_size, write_data, size, FS_WRITE_FLUSH);
		if (save_file_result.code != 0)
		{
			save_file_result.string = "[Error] FSFILE_Write failed. ";
			function_fail = true;
		}
	}
	
	FSFILE_Close(fs_handle);
	free(read_data);
	read_data = NULL;

	return save_file_result;
}

Result_with_string Share_load_from_file(std::string file_name, u8* read_data, int max_size, u32* read_size, std::string dir_path, Handle fs_handle, FS_Archive fs_archive)
{
	bool function_fail = false;
	std::string file_path = dir_path + file_name;
	Result_with_string load_file_result;
	load_file_result.code = 0;
	load_file_result.string = "";

	load_file_result.code = FSUSER_OpenArchive(&fs_archive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
	if (load_file_result.code != 0)
	{
		load_file_result.string = "[Error] FSUSER_OpenArchive failed. ";
		function_fail = true;
	}

	if (!function_fail)
	{
		load_file_result.code = FSUSER_OpenFile(&fs_handle, fs_archive, fsMakePath(PATH_ASCII, file_path.c_str()), FS_OPEN_WRITE, FS_ATTRIBUTE_ARCHIVE);
		if (load_file_result.code != 0)
		{
			load_file_result.string = "[Error] FSUSER_OpenFile failed. ";
			function_fail = true;
		}
	}
	
	if (!function_fail)
	{
		load_file_result.code = FSFILE_Read(fs_handle, read_size, 0, read_data, max_size);
		if (load_file_result.code != 0)
		{
			load_file_result.string = "[Error] FSFILE_Read failed. ";
			function_fail = true;
		}
	}

	FSFILE_Close(fs_handle);
	return load_file_result;
}
