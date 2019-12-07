#include <3ds.h>
#include <string>
#include <cstring>
#include <stdlib.h>

char read_data[0x800000];

int Share_save_to_file(std::string file_name, std::string data, std::string dir_path, bool over_write, Handle fs_handle, FS_Archive fs_archive)
{
	Result function_result;
	u32 written_size = 0;
	u32 read_size = 0;
	char dir_path_char[256];
	sprintf(dir_path_char, "%s", dir_path.c_str());

	memset(read_data, 0x0, strlen(read_data));

	FS_Path fs_path_dir = fsMakePath(PATH_ASCII, dir_path_char);
	FS_Path fs_path_empty = fsMakePath(PATH_EMPTY, "");
	FSUSER_OpenArchive(&fs_archive, ARCHIVE_SDMC, fs_path_empty);
	FSUSER_CreateDirectory(fs_archive, fs_path_dir, FS_ATTRIBUTE_DIRECTORY);

	char* file_path = strcat(dir_path_char, file_name.c_str());

	FS_Path path_file = fsMakePath(PATH_ASCII, file_path);
	if (over_write)
	{
		FSUSER_DeleteFile(fs_archive, path_file);
	}
	
	FSUSER_CreateFile(fs_archive, path_file, FS_ATTRIBUTE_ARCHIVE, 0);
	FSUSER_OpenFile(&fs_handle, fs_archive, path_file, FS_OPEN_WRITE, FS_ATTRIBUTE_ARCHIVE);
	if (!over_write)
	{
		function_result = FSFILE_Read(fs_handle, &read_size, 0, read_data, 0x800000);
	}
	function_result = FSFILE_Write(fs_handle, &written_size, read_size, data.c_str(), data.length(), FS_WRITE_FLUSH);

	FSFILE_Close(fs_handle);

	if (function_result == 0)
	{
		return 0;
	}
	else
	{
		return function_result;
	}
}

std::string Share_load_from_file(std::string file_name, std::string dir_path, Handle fs_handle, FS_Archive fs_archive)
{
	Result function_result;
	std::string read_data_return;
	u32 read_size = 0;
	char dir_path_char[256];
	sprintf(dir_path_char, "%s", dir_path.c_str());

	memset(read_data, 0x0, strlen(read_data));

	FS_Path fs_path_empty = fsMakePath(PATH_EMPTY, "");
	FSUSER_OpenArchive(&fs_archive, ARCHIVE_SDMC, fs_path_empty);

	char* file_path = strcat(dir_path_char, file_name.c_str());
	FS_Path fs_path_dir = fsMakePath(PATH_ASCII, file_path);

	FSUSER_OpenFile(&fs_handle, fs_archive, fs_path_dir, FS_OPEN_WRITE, FS_ATTRIBUTE_ARCHIVE);
	function_result = FSFILE_Read(fs_handle, &read_size, 0, read_data, 0x800000);
	FSFILE_Close(fs_handle);

	if (function_result == 0)
	{
		return read_data;
	}
	else
	{
		return "file read error";
	}
}
