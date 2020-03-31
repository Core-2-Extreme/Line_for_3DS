#include <3ds.h>
#include <string>
#include <cstring>
#include <stdlib.h>
#include "share_function.hpp"

Result_with_string Share_save_to_file(std::string file_name, u8* write_data, int size, std::string dir_path, bool delete_old_file, Handle fs_handle, FS_Archive fs_archive)
{
	u32 written_size = 0;
	u64 file_size = 0;
	bool function_fail = false;
	std::string file_path = dir_path + file_name;
	Result_with_string save_file_result;

	save_file_result.code = 0;
	save_file_result.string = "[Success] ";

	save_file_result.code = FSUSER_OpenArchive(&fs_archive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
	if(save_file_result.code != 0)
	{
		save_file_result.string = "[Error] FSUSER_OpenArchive failed. ";
		save_file_result.error_description = "N/A ";
		function_fail = true;
	}

	if (!function_fail)
	{
		save_file_result.code = FSUSER_CreateDirectory(fs_archive, fsMakePath(PATH_ASCII, dir_path.c_str()), FS_ATTRIBUTE_DIRECTORY);
		if (save_file_result.code != 0 && save_file_result.code != (s32)0xC82044BE)//#0xC82044BE directory already exist
		{
			save_file_result.string = "[Error] FSUSER_CreateDirectory failed. ";
			save_file_result.error_description = "N/A ";
			function_fail = true;
		}
	}

	if (!function_fail)
	{
		if (delete_old_file)
			FSUSER_DeleteFile(fs_archive, fsMakePath(PATH_ASCII, file_path.c_str()));
	}

	if (!function_fail)
	{
		save_file_result.code = FSUSER_CreateFile(fs_archive, fsMakePath(PATH_ASCII, file_path.c_str()), FS_ATTRIBUTE_ARCHIVE, 0);
		if (save_file_result.code != 0) 
		{
			save_file_result.string = "[Error] FSUSER_CreateFile failed. ";
			save_file_result.error_description = "N/A ";
			function_fail = true;
		}
	}

	if (!function_fail)
	{
		save_file_result.code = FSUSER_OpenFile(&fs_handle, fs_archive, fsMakePath(PATH_ASCII, file_path.c_str()), FS_OPEN_WRITE, FS_ATTRIBUTE_ARCHIVE);
		if (save_file_result.code != 0)
		{
			save_file_result.string = "[Error] FSUSER_OpenFile failed. ";
			save_file_result.error_description = "N/A ";
			function_fail = true;
		}
	}
	
	if (!function_fail)
	{
		if (!delete_old_file)
		{
			save_file_result.code = FSFILE_GetSize(fs_handle, &file_size);
			if (save_file_result.code != 0)
			{
				save_file_result.string = "[Error] FSFILE_GetSize failed. ";
				save_file_result.error_description = "N/A ";
				function_fail = true;
			}
		}
	}

	if (!function_fail)
	{
		save_file_result.code = FSFILE_Write(fs_handle, &written_size, file_size, write_data, size, FS_WRITE_FLUSH);
		if (save_file_result.code != 0)
		{
			save_file_result.string = "[Error] FSFILE_Write failed. ";
			save_file_result.error_description = "N/A ";
			function_fail = true;
		}
	}
	
	FSFILE_Close(fs_handle);
	FSUSER_CloseArchive(fs_archive);
	return save_file_result;
}

Result_with_string Share_load_from_file(std::string file_name, u8* read_data, int max_size, u32* read_size, std::string dir_path, Handle fs_handle, FS_Archive fs_archive)
{
	bool function_fail = false;
	u64 file_size;
	std::string file_path = dir_path + file_name;
	Result_with_string check_exist_result;
	check_exist_result.code = 0;
	check_exist_result.string = "[Success] ";

	check_exist_result.code = FSUSER_OpenArchive(&fs_archive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
	if (check_exist_result.code != 0)
	{
		check_exist_result.string = "[Error] FSUSER_OpenArchive failed. ";
		check_exist_result.error_description = "N/A ";
		function_fail = true;
	}

	if (!function_fail)
	{
		check_exist_result.code = FSUSER_OpenFile(&fs_handle, fs_archive, fsMakePath(PATH_ASCII, file_path.c_str()), FS_OPEN_READ, FS_ATTRIBUTE_ARCHIVE);
		if (check_exist_result.code != 0)
		{
			check_exist_result.string = "[Error] FSUSER_OpenFile failed. ";
			check_exist_result.error_description = "N/A ";
			function_fail = true;
		}
	}
	
	if (!function_fail)
	{
		check_exist_result.code = FSFILE_GetSize(fs_handle, &file_size);
		if (check_exist_result.code != 0)
		{
			check_exist_result.string = "[Error] FSFILE_GetSize failed. ";
			check_exist_result.error_description = "N/A ";
			function_fail = true;
		}
	}

	if (!function_fail)
	{
		if ((int)file_size > max_size)
		{
			check_exist_result.code = BUFFER_SIZE_IS_TOO_SMALL;
			check_exist_result.string = "[Error] Buffer size is too small. ";
			check_exist_result.error_description = "In the case that the buffer size is too small, this'll occur.\nPlease increase buffer size from settings.";
			function_fail = true;
		}
	}

	if (!function_fail)
	{
		check_exist_result.code = FSFILE_Read(fs_handle, read_size, 0, read_data, max_size);
		if (check_exist_result.code != 0)
		{
			check_exist_result.string = "[Error] FSFILE_Read failed. ";
			check_exist_result.error_description = "N/A ";
			function_fail = true;
		}
	}

	FSFILE_Close(fs_handle);
	FSUSER_CloseArchive(fs_archive);
	return check_exist_result;
}

Result_with_string Share_check_file_exist(std::string file_name, std::string dir_path, Handle fs_handle, FS_Archive fs_archive)
{
	bool function_fail = false;
	std::string file_path = dir_path + file_name;
	Result_with_string check_exist_result;
	check_exist_result.code = 0;
	check_exist_result.string = "[Success] ";

	check_exist_result.code = FSUSER_OpenArchive(&fs_archive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
	if (check_exist_result.code != 0)
	{
		check_exist_result.string = "[Error] FSUSER_OpenArchive failed. ";
		check_exist_result.error_description = "N/A ";
		function_fail = true;
	}

	if (!function_fail)
	{
		check_exist_result.code = FSUSER_OpenFile(&fs_handle, fs_archive, fsMakePath(PATH_ASCII, file_path.c_str()), FS_OPEN_READ, FS_ATTRIBUTE_ARCHIVE);
		if (check_exist_result.code != 0)
		{
			check_exist_result.string = "[Error] FSUSER_OpenFile failed. ";
			check_exist_result.error_description = "N/A ";
			function_fail = true;
		}
	}

	FSFILE_Close(fs_handle);
	FSUSER_CloseArchive(fs_archive);
	return check_exist_result;
}