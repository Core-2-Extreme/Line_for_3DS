#include <3ds.h>
#include <string>
#include <cstring>
#include <stdlib.h>
#include "share_function.hpp"

char read_data[0x800000];

Result_with_string Share_save_to_file(std::string file_name, std::string data, std::string dir_path, bool over_write, Handle fs_handle, FS_Archive fs_archive)
{
	u32 written_size = 0;
	u32 read_size = 0;
	bool function_fail = false;
	std::string file_path = dir_path + file_name;
	memset(read_data, 0x0, strlen(read_data));
	Result_with_string save_file_result;
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
			save_file_result.code = FSFILE_Read(fs_handle, &read_size, 0, read_data, 0x800000);
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
		save_file_result.code = FSFILE_Write(fs_handle, &written_size, read_size, data.c_str(), data.length(), FS_WRITE_FLUSH);
		if (save_file_result.code != 0)
		{
			save_file_result.string = "[Error] FSFILE_Write failed. ";
			function_fail = true;
		}
	}
	
	FSFILE_Close(fs_handle);

	return save_file_result;
}

Result_with_string Share_load_from_file(std::string file_name, std::string dir_path, Handle fs_handle, FS_Archive fs_archive)
{
	u32 read_size = 0;
	bool function_fail = false;
	std::string file_path = dir_path + file_name;
	Result_with_string load_file_result;
	load_file_result.code = 0;
	load_file_result.string = "";
	memset(read_data, 0x0, strlen(read_data));

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
		load_file_result.code = FSFILE_Read(fs_handle, &read_size, 0, read_data, 0x800000);
		if (load_file_result.code != 0)
		{
			load_file_result.string = "[Error] FSFILE_Read failed. ";
			function_fail = true;
		}
		else
			load_file_result.string = read_data;
	}

	FSFILE_Close(fs_handle);
	return load_file_result;
}
