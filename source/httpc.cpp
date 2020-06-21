#include <3ds.h>
#include <string>

#include "error.hpp"
#include "types.hpp"
#include "share_function.hpp"

int dl_progress = 0;
int post_and_dl_progress = 0;

int Httpc_query_dl_progress(void)
{
	return dl_progress;
}

int Httpc_query_post_and_dl_progress(void)
{
	return post_and_dl_progress;
}

Result_with_string Httpc_dl_data(std::string url, u8* data_buffer, int buffer_size, u32* downloaded_data_size, u32* status_code, bool follow_redirect, std::string* last_url, bool do_not_dl, int max_redirect)
{
	int redirected = 0;
	bool redirect = false;
	bool function_fail = false;
	char* moved_url;
	std::string moved_url_string;
	std::string dl_string;
	httpcContext dl_httpc;
	Result_with_string result;
	result.code = 0;
	result.string = s_success;
	*last_url = url;

	moved_url = (char*)malloc(0x1000);
	if (moved_url == NULL)
	{
		result.error_description = "Couldn't malloc to 'moved_url'(" + std::to_string(0x1000 / 1024) + "KB).";
		result.code = OUT_OF_MEMORY;
		result.string = "[Error] Out of memory. ";
		return result;
	}

	while (true)
	{
		dl_progress = 0;
		redirect = false;

		if (!function_fail)
		{
			result.code = httpcOpenContext(&dl_httpc, HTTPC_METHOD_GET, last_url->c_str(), 0);
			if (result.code != 0)
			{
				result.error_description = "This'll occur in the case the wrong URL was specified.\nPlease check the URL.";
				result.string = "[Error] httpcOpenContext failed. ";
				function_fail = true;
			}
		}
		dl_progress++;

		if (!function_fail)
		{
			result.code = httpcSetSSLOpt(&dl_httpc, SSLCOPT_DisableVerify);
			if (result.code != 0)
			{
				result.error_description = "N/A";
				result.string = "[Error] httpcSetSSLOpt failed. ";
				function_fail = true;
			}
		}
		dl_progress++;

		if (!function_fail)
		{
			result.code = httpcSetKeepAlive(&dl_httpc, HTTPC_KEEPALIVE_ENABLED);
			if (result.code != 0)
			{
				result.error_description = "N/A";
				result.string = "[Error] httpcSetKeepAlive failed. ";
				function_fail = true;
			}
		}
		dl_progress++;

		if (!function_fail)
		{
			httpcAddRequestHeaderField(&dl_httpc, "Connection", "Keep-Alive");
			httpcAddRequestHeaderField(&dl_httpc, "User-Agent", s_httpc_user_agent.c_str());
			result.code = httpcBeginRequest(&dl_httpc);
			if (result.code != 0)
			{
				result.error_description = "N/A";
				result.string = "[Error] httpcBeginRequest failed. ";
				function_fail = true;
			}
		}
		dl_progress++;

		if (!function_fail)
			httpcGetResponseStatusCode(&dl_httpc, status_code);

		dl_progress++;

		if (!function_fail && follow_redirect && max_redirect > redirected)
		{
			result.code = httpcGetResponseHeader(&dl_httpc, "location", moved_url, 0x1000);
			if (result.code == 0)
			{
				moved_url_string = moved_url;
				*last_url = moved_url_string;
				redirect = true;
			}
			else
			{
				result.code = httpcGetResponseHeader(&dl_httpc, "Location", moved_url, 0x1000);
				if (result.code == 0)
				{
					moved_url_string = moved_url;
					*last_url = moved_url_string;
					redirect = true;
				}
			}
			result.code = 0;
		}
		dl_progress++;

		if (!function_fail && !redirect && !do_not_dl)
		{
			result.code = httpcDownloadData(&dl_httpc, data_buffer, buffer_size, downloaded_data_size);
			if (result.code != 0)
			{
				if(result.code == (s32)0xD840A02B)
					result.error_description = "In the case that the buffer size is too small, this'll occur.\nPlease increase buffer size from settings.";
				else
					result.error_description = "It may occur in case of wrong internet connection.\nPlease check internet connection.";
				
				result.string = "[Error] httpcDownloadData failed. ";
				function_fail = true;
			}
			else if(follow_redirect && max_redirect > redirected)
			{
				dl_string = (char*)data_buffer;
				if (dl_string.substr(0, 4) == "http")
				{
					*last_url = dl_string;
					redirect = true;
				}
			}
		}
		dl_progress++;

		httpcCloseContext(&dl_httpc);
		dl_progress++;

		if (function_fail || !redirect)
			break;
		else
			redirected++;
	}
	free(moved_url);
	return result;
}

Result_with_string Httpc_post_and_dl_data(std::string url, char* post_data_buffer, int post_buffer_size, u8* dl_data_buffer, int dl_buffer_size, u32* downloaded_data_size, u32* status_code, bool follow_redirect)
{
	bool redirect = false;
	bool function_fail = false;
	bool post = true;
	char* moved_url;
	std::string dl_string;
	httpcContext post_and_dl_httpc;
	Result_with_string result;
	result.code = 0;
	result.string = s_success;

	moved_url = (char*)malloc(0x1000);
	if (moved_url == NULL)
	{
		result.error_description = "Couldn't malloc to 'moved_url'(" + std::to_string(0x1000 / 1024) + "KB).";
		result.code = OUT_OF_MEMORY;
		result.string = "[Error] Out of memory. ";
		return result;
	}

	while (true)
	{
		post_and_dl_progress = 0;
		redirect = false;

		if (!function_fail)
		{
			if (post)
				result.code = httpcOpenContext(&post_and_dl_httpc, HTTPC_METHOD_POST, url.c_str(), 0);
			else 
				result.code = httpcOpenContext(&post_and_dl_httpc, HTTPC_METHOD_GET, url.c_str(), 0);

			if (result.code != 0)
			{
				result.error_description = "This'll occur in the case the wrong URL was specified.\nPlease check the URL.";
				result.string = "[Error] httpcOpenContext failed. ";
				function_fail = true;
			}
		}
		post_and_dl_progress++;

		if (!function_fail)
		{
			result.code = httpcSetSSLOpt(&post_and_dl_httpc, SSLCOPT_DisableVerify);
			if (result.code != 0)
			{
				result.error_description = "N/A";
				result.string = "[Error] httpcSetSSLOpt failed. ";
				function_fail = true;
			}
		}
		post_and_dl_progress++;

		if (!function_fail)
		{
			result.code = httpcSetKeepAlive(&post_and_dl_httpc, HTTPC_KEEPALIVE_ENABLED);
			if (result.code != 0)
			{
				result.error_description = "N/A";
				result.string = "[Error] httpcSetKeepAlive failed. ";
				function_fail = true;
			}
		}
		post_and_dl_progress++;

		if (!function_fail)
		{
			httpcAddRequestHeaderField(&post_and_dl_httpc, "Connection", "Keep-Alive");
			httpcAddRequestHeaderField(&post_and_dl_httpc, "User-Agent", s_httpc_user_agent.c_str());
			if (post)
			{
				httpcAddPostDataRaw(&post_and_dl_httpc, (u32*)post_data_buffer, post_buffer_size);
				post = false;
			}
			result.code = httpcBeginRequest(&post_and_dl_httpc);
			if (result.code != 0)
			{
				result.error_description = "N/A";
				result.string = "[Error] httpcBeginRequest failed. ";
				function_fail = true;
			}
		}
		post_and_dl_progress++;

		if (!function_fail)
			result.code = httpcGetResponseStatusCode(&post_and_dl_httpc, status_code);

		post_and_dl_progress++;

		if (!function_fail && follow_redirect)
		{
			result.code = httpcGetResponseHeader(&post_and_dl_httpc, "Location", moved_url, 0x1000);
			if (result.code == 0)
			{
				url = moved_url;
				redirect = true;
			}
		}
		post_and_dl_progress++;

		if (!function_fail && !redirect)
		{
			result.code = httpcDownloadData(&post_and_dl_httpc, dl_data_buffer, dl_buffer_size, downloaded_data_size);
			if (result.code != 0)
			{
				if (result.code == (s32)0xD840A02B)
					result.error_description = "In the case that the buffer size is too small, this'll occur.\nPlease increase buffer size from settings.";
				else
					result.error_description = "It may occur in case of wrong internet connection.\nPlease check internet connection.";

				result.string = "[Error] httpcDownloadData failed. ";
				function_fail = true;
			}
			else if (follow_redirect)
			{
				dl_string = (char*)dl_data_buffer;
				if (dl_string.substr(0, 4) == "http")
				{
					url = dl_string;
					redirect = true;
				}
			}
		}
		post_and_dl_progress++;

		httpcCloseContext(&post_and_dl_httpc);
		post_and_dl_progress++;

		if (function_fail || !redirect)
			break;
	}
	free(moved_url);
	return result;
}
