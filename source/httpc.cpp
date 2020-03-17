#include <3ds.h>
#include <string>
#include <unistd.h>
#include "share_function.hpp"

int download_progress = 0;

int Httpc_query_dl_progress(void)
{
	return download_progress;
}

Result_with_string Httpc_dl_data(std::string url, u8* data_buffer, int buffer_size, u32* downloaded_data_size, u32* status_code, bool follow_redirect)
{
	bool redirect = false;
	bool function_fail = false;
	char* moved_url;
	std::string downloaded_data_string;
	httpcContext dl_httpc;
	Result_with_string httpc_dl_data_result;
	httpc_dl_data_result.code = 0;
	httpc_dl_data_result.string = "[Success] ";

	moved_url = (char*)malloc(0x1000);
	if (moved_url == NULL)
	{
		httpc_dl_data_result.error_description = "Couldn't malloc to 'moved_url'(" + std::to_string(0x1000 / 1024) + "KB).";
		httpc_dl_data_result.code = OUT_OF_MEMORY;
		httpc_dl_data_result.string = "[Error] Out of memory. ";
		return httpc_dl_data_result;
	}

	while (true)
	{
		download_progress = 0;
		redirect = false;
		usleep(25000);

		if (!function_fail)
		{
			httpc_dl_data_result.code = httpcOpenContext(&dl_httpc, HTTPC_METHOD_GET, url.c_str(), 0);
			if (httpc_dl_data_result.code != 0)
			{
				httpc_dl_data_result.error_description = "This'll occur in the case the wrong URL was specified.\nPlease check the URL.";
				httpc_dl_data_result.string = "[Error] httpcOpenContext failed. ";
				function_fail = true;
			}
		}
		download_progress++;

		if (!function_fail)
		{
			httpc_dl_data_result.code = httpcSetSSLOpt(&dl_httpc, SSLCOPT_DisableVerify);
			if (httpc_dl_data_result.code != 0)
			{
				httpc_dl_data_result.error_description = "N/A";
				httpc_dl_data_result.string = "[Error] httpcSetSSLOpt failed. ";
				function_fail = true;
			}
		}
		download_progress++;

		if (!function_fail)
		{
			httpc_dl_data_result.code = httpcSetKeepAlive(&dl_httpc, HTTPC_KEEPALIVE_ENABLED);
			if (httpc_dl_data_result.code != 0)
			{
				httpc_dl_data_result.error_description = "N/A";
				httpc_dl_data_result.string = "[Error] httpcSetKeepAlive failed. ";
				function_fail = true;
			}
		}
		download_progress++;

		if (!function_fail)
		{
			httpcAddRequestHeaderField(&dl_httpc, "Connection", "Keep-Alive");
			httpcAddRequestHeaderField(&dl_httpc, "User-Agent", s_httpc_user_agent.c_str());
			httpc_dl_data_result.code = httpcBeginRequest(&dl_httpc);
			if (httpc_dl_data_result.code != 0)
			{
				httpc_dl_data_result.error_description = "N/A";
				httpc_dl_data_result.string = "[Error] httpcBeginRequest failed. ";
				function_fail = true;
			}
		}
		download_progress++;

		if (!function_fail)
		{
			httpc_dl_data_result.code = httpcGetResponseStatusCode(&dl_httpc, status_code);
			if (httpc_dl_data_result.code != 0)
			{
				httpc_dl_data_result.error_description = "N/A";
				httpc_dl_data_result.string = "[Error] httpcGetResponseStatusCode failed. ";
				function_fail = true;
			}
		}
		download_progress++;

		if (!function_fail && follow_redirect)
		{
			httpc_dl_data_result.code = httpcGetResponseHeader(&dl_httpc, "Location", moved_url, 0x1000);
			if (httpc_dl_data_result.code == 0)
			{
				url = moved_url;
				redirect = true;
			}
		}
		download_progress++;

		if (!function_fail && !redirect)
		{
			httpc_dl_data_result.code = httpcDownloadData(&dl_httpc, data_buffer, buffer_size, downloaded_data_size);
			if (httpc_dl_data_result.code != 0)
			{
				if(httpc_dl_data_result.code == (s32)0xD840A02B)
					httpc_dl_data_result.error_description = "In the case that the buffer size is too small, this'll occur.\nPlease increase buffer size from settings.";
				else
					httpc_dl_data_result.error_description = "It may occur in case of wrong internet connection.\nPlease check internet connection.";
				
				httpc_dl_data_result.string = "[Error] httpcDownloadData failed. ";
				function_fail = true;
			}
			else if(follow_redirect)
			{
				downloaded_data_string = (char*)data_buffer;
				if (downloaded_data_string.substr(0, 4) == "http")
				{
					url = downloaded_data_string;
					redirect = true;
				}
			}
		}
		download_progress++;

		httpcCloseContext(&dl_httpc);
		download_progress++;

		if (function_fail || !redirect)
			break;
	}
	free(moved_url);
	return httpc_dl_data_result;
}

