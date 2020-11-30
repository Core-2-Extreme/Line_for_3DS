#pragma once

#include "types.hpp"

Result_with_string File_save_to_file(std::string file_name, u8* write_data, int size, std::string dir_path, bool delete_old_file, Handle fs_handle, FS_Archive fs_archive);

Result_with_string File_load_from_file(std::string file_name, u8* read_data, int max_size, u32* read_size, std::string dir_path, Handle fs_handle, FS_Archive fs_archive);

Result_with_string File_load_from_rom(std::string file_name, u8* read_data, int max_size, u32* read_size, std::string dir_path);

Result_with_string File_load_from_file_with_range(std::string file_name, u8* read_data, int read_length, u64 read_offset, u32* read_size, std::string dir_path, Handle fs_handle, FS_Archive fs_archive);

Result_with_string File_delete_file(std::string file_name, std::string dir_path, FS_Archive fs_archive);

Result_with_string File_check_file_size(std::string file_name, std::string dir_path, u64* file_size, Handle fs_handle, FS_Archive fs_archive);

Result_with_string File_check_file_exist(std::string file_name, std::string dir_path, Handle fs_handle, FS_Archive fs_archive);

Result_with_string File_read_dir(int* num_of_detected, std::string file_and_dir_name[], int name_num_of_array, std::string type[], int type_num_of_array, std::string dir_path);
