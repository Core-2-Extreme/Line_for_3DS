#pragma once

#include "share_function.hpp"

int Httpc_query_dl_progress(void);

Result_with_string Httpc_dl_data(std::string url, u8* data_buffer, int buffer_size, u32* downloaded_data_size, u32* status_code, bool follow_redirect);

