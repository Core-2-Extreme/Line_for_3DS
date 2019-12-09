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
	std::string file_path = dir_path + file_name;
	memset(read_data, 0x0, strlen(read_data));

	FSUSER_OpenArchive(&fs_archive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
	FSUSER_CreateDirectory(fs_archive, fsMakePath(PATH_ASCII, dir_path.c_str()), FS_ATTRIBUTE_DIRECTORY);
	if (over_write)
		FSUSER_DeleteFile(fs_archive, fsMakePath(PATH_ASCII, file_path.c_str()));
	
	FSUSER_CreateFile(fs_archive, fsMakePath(PATH_ASCII, file_path.c_str()), FS_ATTRIBUTE_ARCHIVE, 0);
	FSUSER_OpenFile(&fs_handle, fs_archive, fsMakePath(PATH_ASCII, file_path.c_str()), FS_OPEN_WRITE, FS_ATTRIBUTE_ARCHIVE);
	if (!over_write)
	{
		function_result = FSFILE_Read(fs_handle, &read_size, 0, read_data, 0x800000);
		if (function_result != 0)
			read_size = 0;
	}

	function_result = FSFILE_Write(fs_handle, &written_size, read_size, data.c_str(), data.length(), FS_WRITE_FLUSH);
	FSFILE_Close(fs_handle);

	return function_result;
}

std::string Share_load_from_file(std::string file_name, std::string dir_path, Handle fs_handle, FS_Archive fs_archive)
{
	Result function_result;
	u32 read_size = 0;
	std::string file_path = dir_path + file_name;
	memset(read_data, 0x0, strlen(read_data));

	FSUSER_OpenArchive(&fs_archive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
	FSUSER_OpenFile(&fs_handle, fs_archive, fsMakePath(PATH_ASCII, file_path.c_str()), FS_OPEN_WRITE, FS_ATTRIBUTE_ARCHIVE);
	function_result = FSFILE_Read(fs_handle, &read_size, 0, read_data, 0x800000);
	FSFILE_Close(fs_handle);

	if (function_result == 0)
		return read_data;
	else
		return "file read error";
}
