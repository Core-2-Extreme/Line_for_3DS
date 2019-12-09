#pragma once
int Share_save_to_file(std::string file_name, std::string data, std::string dir_path, bool delete_old_file, Handle fs_handle, FS_Archive fs_archive);

std::string Share_load_from_file(std::string file_name, std::string dir_path, Handle fs_handle, FS_Archive fs_archive);
