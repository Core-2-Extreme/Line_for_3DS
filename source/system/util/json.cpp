#include "definitions.hpp"
#include "system/types.hpp"

#include "system/util/log.hpp"

//Include myself.
#include "system/util/json.hpp"

extern "C"
{
#include "jsmn/jsmn.h"
}


typedef enum
{
	JSON_INTERNAL_STATE_INVALID = -1,

	JSON_INTERNAL_STATE_SEARCHING_KEY,
	JSON_INTERNAL_STATE_SEARCHING_ARRAY_INDEX,
	JSON_INTERNAL_STATE_FOUND,

	JSON_INTERNAL_STATE_MAX,
} Json_internal_search_state;

typedef enum
{
	JSON_INTERNAL_UNESCAPE_STATE_INVALID = -1,

	JSON_INTERNAL_UNESCAPE_STATE_SEARCHING_BACKSLASH,
	JSON_INTERNAL_UNESCAPE_STATE_SEARCHING_ESCAPED_CHAR,
	JSON_INTERNAL_UNESCAPE_STATE_SEARCHING_4_HEX_DIGITS,

	JSON_INTERNAL_UNESCAPE_STATE_MAX,
} Json_internal_unescaping_search_state;

typedef struct 
{
	char escaped_chars;
	char unescaped_chars;
} Json_internal_escape_char_list;

typedef struct
{
	bool is_object = false;
	u32 size = 0;
	u32 current_index = 0;
} Json_internal_link_list_data;

typedef struct
{
	u32 capacity = 0;
	u32 used = 0;
	Json_internal_link_list_data* data;
} Json_internal_link_list;

typedef struct
{
	u32 capacity = 0;
	u32 raw_json_size = 0;
	char* raw_json = NULL;
	jsmn_parser parser = { .pos = 0, .toknext = 0, .toksuper = 0, };
	jsmntok_t* tokens = NULL;
} Json_internal_data;

typedef struct
{
	bool is_array = false;
	char* key = NULL;
	u32 array_index = 0;
} Json_internal_requested_key_data;

typedef struct
{
	u32 capacity = 0;
	u32 used = 0;
	u32 key_size = 0;
	u32 current_offset = 0;
	u32 current_index = 0;
	Json_internal_search_state state = JSON_INTERNAL_STATE_SEARCHING_KEY;
	Json_internal_requested_key_data* data = NULL;
} Json_internal_requested_key;

Json_internal_escape_char_list json_escape_char_list[] =
{
	{ .escaped_chars = '\"', .unescaped_chars = '\"', },
	{ .escaped_chars = '\\', .unescaped_chars = '\\', },
	{ .escaped_chars = '/', .unescaped_chars = '/',  },
	{ .escaped_chars = 'b', .unescaped_chars = '\b', },
	{ .escaped_chars = 'f', .unescaped_chars = '\f', },
	{ .escaped_chars = 'n', .unescaped_chars = '\n', },
	{ .escaped_chars = 'r', .unescaped_chars = '\r', },
	{ .escaped_chars = 't', .unescaped_chars = '\t', },
	//"u" is special, so it's not included here.
};


static std::string Util_json_unescape(char* escaped_string);


Result_with_string Util_json_parse(char* raw_json, Json_data* parsed_json_data)
{
	u32 raw_json_size = 0;
	int elements = 0;
	Json_internal_data* data = NULL;
	Result_with_string result;

	if(!raw_json || !parsed_json_data)
		goto invalid_arg;

	raw_json_size = strlen(raw_json);

	if(raw_json_size <= 0)
		goto invalid_arg;

	Util_json_free(parsed_json_data);
	parsed_json_data->internal_data = (Json_internal_data*)malloc(sizeof(Json_internal_data));
	if(!parsed_json_data->internal_data)
		goto out_of_memory;

	data = (Json_internal_data*)(parsed_json_data->internal_data);
	data->capacity = 0;
	data->parser.pos = 0;
	data->parser.toknext = 0;
	data->parser.toksuper = 0;
	data->raw_json = NULL;
	data->raw_json_size = 0;
	data->tokens = NULL;

	data->raw_json = (char*)malloc(raw_json_size + 1);
	if(!data->raw_json)
		goto out_of_memory;

	data->raw_json_size = raw_json_size;
	memcpy(data->raw_json, raw_json, data->raw_json_size);
	data->raw_json[data->raw_json_size] = 0x00;//Add a NULL terminator.

	//Check how many tokens we need to allocate.
	jsmn_init(&data->parser);
	elements = jsmn_parse(&data->parser, data->raw_json, data->raw_json_size, NULL, 0);
	if(elements < 0)
		goto jsmn_api_failed;

	data->tokens = (jsmntok_t*)malloc(sizeof(jsmntok_t) * elements);
	if(!data->tokens)
		goto out_of_memory;

	data->capacity = elements;

	//Parse json data.
	jsmn_init(&data->parser);
	elements = jsmn_parse(&data->parser, data->raw_json, data->raw_json_size, data->tokens, elements);
	if(elements < 0)
		goto jsmn_api_failed;

	return result;

	invalid_arg:
	result.code = DEF_ERR_INVALID_ARG;
	result.string = DEF_ERR_INVALID_ARG_STR;
	return result;

	out_of_memory:
	Util_json_free(parsed_json_data);
	result.code = DEF_ERR_OUT_OF_MEMORY;
	result.string = DEF_ERR_OUT_OF_MEMORY_STR;
	return result;

	jsmn_api_failed:
	Util_json_free(parsed_json_data);
	result.code = DEF_ERR_JSMN_RETURNED_NOT_SUCCESS;
	result.string = DEF_ERR_JSMN_RETURNED_NOT_SUCCESS_STR;
	return result;
}

Result_with_string Util_json_get_data(std::string key, Json_data* json_data, Json_extracted_data* extracted_data)
{
	bool is_searching_key = true;
	std::size_t dot_pos = std::string::npos;
	std::size_t start_of_array_pos = std::string::npos;
	std::size_t end_of_array_pos = std::string::npos;
	Json_internal_data* data = NULL;
	Json_internal_requested_key* key_info = NULL;
	Json_internal_link_list* link_list = NULL;
	Result_with_string result;

	if(key == "" || !json_data || !extracted_data || !json_data->internal_data)
		goto invalid_arg_0;

	data = (Json_internal_data*)json_data->internal_data;
	if(!data->raw_json || data->raw_json_size <= 0 || !data->tokens || data->capacity <= 0)
		goto invalid_arg_0;

	free(extracted_data->value);
	extracted_data->size = 0;
	extracted_data->value = NULL;
	extracted_data->type = JSON_TYPE_INVALID;

	key_info = (Json_internal_requested_key*)malloc(sizeof(Json_internal_requested_key));
	if(!key_info)
		goto out_of_memory;

	key_info->capacity = 0;
	key_info->used = 0;
	key_info->key_size = key.length();
	key_info->current_offset = 0;
	key_info->current_index = 0;
	key_info->state = JSON_INTERNAL_STATE_SEARCHING_KEY;

	key_info->data = (Json_internal_requested_key_data*)malloc(sizeof(Json_internal_requested_key_data) * 64);
	if(!key_info->data)
		goto out_of_memory;

	key_info->capacity = 64;
	for(u32 i = 0; i <key_info->capacity; i++)
	{
		key_info->data[i].is_array = false;
		key_info->data[i].key = NULL;
		key_info->data[i].array_index = 0;
	}

	do
	{
		dot_pos = key.find(".", key_info->current_offset);
		start_of_array_pos = key.find("[", key_info->current_offset);
		end_of_array_pos = key.find("]", key_info->current_offset);

		//If there are no ".", "[" nor "]", we reached EOF.
		if(dot_pos == std::string::npos && start_of_array_pos == std::string::npos && end_of_array_pos == std::string::npos)
		{
			u32 size = (key_info->key_size - key_info->current_offset);

			if(size <= 0)
				goto invalid_arg_1;

			key_info->data[key_info->used].key = (char*)malloc(size + 1);
			if(!key_info->data[key_info->used].key)
				goto out_of_memory;

			memcpy(key_info->data[key_info->used].key, (key.c_str() + key_info->current_offset), size);
			key_info->data[key_info->used].key[size] = 0x00;//Add null terminator.
			key_info->used++;

			break;
		}
		else
		{
			bool is_array_detected = false;
			bool is_dot_detected = false;

			//If only "[" or "]" exists, it's illegal.
			//e.g object.array[0.data
			//                ^
			//object.array0].data
			//             ^
			if((start_of_array_pos != std::string::npos && end_of_array_pos == std::string::npos)
			|| (start_of_array_pos == std::string::npos && end_of_array_pos != std::string::npos))
			{
				// Util_log_save("debug", "invalid arg 0, dot:" + std::to_string(dot_pos) + " array start:"  + std::to_string(start_of_array_pos) + " array end:"  + std::to_string(end_of_array_pos));
				goto invalid_arg_1;
			}

			if(start_of_array_pos != std::string::npos && end_of_array_pos != std::string::npos)
				is_array_detected = true;
			if(dot_pos != std::string::npos)
				is_dot_detected = true;

			if(is_array_detected)
			{
				//It's also illegal if (start_pos + 1) is greater than or equal to end_pos.
				//e.g object.array[].data
				//                ^^
				if((start_of_array_pos + 1) >= end_of_array_pos)
				{
					// Util_log_save("debug", "invalid arg 1, dot:" + std::to_string(dot_pos) + " array start:"  + std::to_string(start_of_array_pos) + " array end:"  + std::to_string(end_of_array_pos));
					goto invalid_arg_1;
				}

				if(is_dot_detected)
				{
					//Finally, "." can't exist between "[" and "]".
					//e.g object.array[.3].data
					//                 ^
					if(dot_pos < start_of_array_pos && dot_pos > end_of_array_pos)
					{
						// Util_log_save("debug", "invalid arg 2, dot:" + std::to_string(dot_pos) + " array start:"  + std::to_string(start_of_array_pos) + " array end:"  + std::to_string(end_of_array_pos));
						goto invalid_arg_1;
					}
				}
			}

			//If only dot exists or brackets comes later, process dot first.
			//e.g. object.data
			//           ^
			//object.data[2]
			//      ^    ^ ^
			if((is_dot_detected && !is_array_detected) || (is_dot_detected && is_array_detected && dot_pos < start_of_array_pos))
			{
				u32 size = (dot_pos - key_info->current_offset);

				if(size <= 0)
				{
					// Util_log_save("debug", "invalid arg 3, dot:" + std::to_string(dot_pos) + " array start:"  + std::to_string(start_of_array_pos) + " array end:"  + std::to_string(end_of_array_pos));
					goto invalid_arg_1;
				}

				key_info->data[key_info->used].key = (char*)malloc(size + 1);
				if(!key_info->data[key_info->used].key)
					goto out_of_memory;

				memcpy(key_info->data[key_info->used].key, (key.c_str() + key_info->current_offset), size);
				key_info->data[key_info->used].key[size] = 0x00;//Add null terminator.
				key_info->used++;

				//Increment index by key_size + dot_size.
				key_info->current_offset += size + 1;
			}
			//If only brackets exist or dot comes later, process brackets first.
			//e.g. data[2]
			//         ^ ^
			//object[1].data
			//      ^ ^^
			else if((!is_dot_detected && is_array_detected) || (is_dot_detected && is_array_detected && start_of_array_pos < dot_pos))
			{
				u32 size = (start_of_array_pos - key_info->current_offset);
				char* array_index_temp = NULL;

				//If dot exists, it must be just after "]", if not it's illegal.
				//e.g. object[1]z.data
				//              ^
				if(is_dot_detected && (end_of_array_pos + 1) != dot_pos)
				{
					// Util_log_save("debug", "invalid arg 4, dot:" + std::to_string(dot_pos) + " array start:"  + std::to_string(start_of_array_pos) + " array end:"  + std::to_string(end_of_array_pos));
					goto invalid_arg_1;
				}

				if(size <= 0)
				{
					// Util_log_save("debug", "invalid arg 5, dot:" + std::to_string(dot_pos) + " array start:"  + std::to_string(start_of_array_pos) + " array end:"  + std::to_string(end_of_array_pos));
					goto invalid_arg_1;
				}

				key_info->data[key_info->used].key = (char*)malloc(size + 1);
				if(!key_info->data[key_info->used].key)
					goto out_of_memory;

				memcpy(key_info->data[key_info->used].key, (key.c_str() + key_info->current_offset), size);
				key_info->data[key_info->used].key[size] = 0x00;//Add null terminator.

				//Increment index by key_size + bracket_size.
				key_info->current_offset += size + 1;

				size = (end_of_array_pos - key_info->current_offset);

				array_index_temp = (char*)malloc(size + 1);
				if(!array_index_temp)
					goto out_of_memory;

				memcpy(array_index_temp, (key.c_str() + key_info->current_offset), size);
				array_index_temp[size] = 0x00;//Add null terminator.

				//Check if it's a valid integer.
				for(u32 i = 0; i < size; i++)
				{
					if(array_index_temp[i] < '0' || array_index_temp[i] > '9')
					{
						free(array_index_temp);
						array_index_temp = NULL;
						// Util_log_save("debug", "invalid arg 6, dot:" + std::to_string(dot_pos) + " array start:"  + std::to_string(start_of_array_pos) + " array end:"  + std::to_string(end_of_array_pos));
						goto invalid_arg_1;
					}
				}

				key_info->data[key_info->used].array_index = atoi(array_index_temp);
				key_info->data[key_info->used].is_array = true;
				key_info->used++;

				free(array_index_temp);
				array_index_temp = NULL;

				//Increment index by index_size + bracket_size.
				key_info->current_offset += size + 1;
				if(is_dot_detected)//If dot exists, + dot_size.
					key_info->current_offset++;
			}
		}
	}
	while(key_info->current_offset < key_info->key_size);

	link_list = (Json_internal_link_list*)malloc(sizeof(Json_internal_link_list));
	if(!link_list)
		goto out_of_memory;

	link_list->data = (Json_internal_link_list_data*)malloc(sizeof(Json_internal_link_list_data) * 64);
	if(!link_list->data)
		goto out_of_memory;

	link_list->capacity = 64;
	link_list->used = 0;

	for(u32 i = 0; i < link_list->capacity; i++)
	{
		link_list->data[i].is_object = false;
		link_list->data[i].size = 0;
		link_list->data[i].current_index = 0;
	}

	//Search for the value.
	for(u32 i = 1; i < data->capacity; i++)
	{
		if(data->tokens[i].type == JSMN_ARRAY || data->tokens[i].type == JSMN_OBJECT)
		{
			if(link_list->used + 1 >= 64)
			{
				//todo realloc
				goto out_of_memory;
			}

			link_list->used++;
			link_list->data[link_list->used].is_object = (data->tokens[i].type == JSMN_OBJECT);
			link_list->data[link_list->used].size = data->tokens[i].size;
			link_list->data[link_list->used].current_index = 0;

			if(data->tokens[i].type == JSMN_OBJECT)
				is_searching_key = true;
		}
		else if(is_searching_key)
		{
			if(data->tokens[i].type == JSMN_STRING)
			{
				u32 key_size = data->tokens[i].end - data->tokens[i].start;

				//Check if key matches.
				if(key_size == strlen(key_info->data[key_info->current_index].key)
				&& memcmp((data->raw_json + data->tokens[i].start), key_info->data[key_info->current_index].key, key_size) == 0)
				{
					if((key_info->current_index + 1) == key_info->used)
					{
						if(!key_info->data[key_info->current_index].is_array)
							key_info->state = JSON_INTERNAL_STATE_FOUND;//We found a key.
						else
							key_info->state = JSON_INTERNAL_STATE_SEARCHING_ARRAY_INDEX;//We found a key and need to search for array index.
					}

					if(!key_info->data[key_info->current_index].is_array)
						key_info->current_index++;
				}

				is_searching_key = false;
			}
		}
		else
		{
			if(key_info->data[key_info->current_index].array_index == link_list->data[link_list->used].current_index
			&& key_info->state == JSON_INTERNAL_STATE_SEARCHING_ARRAY_INDEX && !link_list->data[link_list->used].is_object
			&& link_list->used > 0)
			{
				//We found a key.
				key_info->state = JSON_INTERNAL_STATE_FOUND;
				key_info->current_index++;
			}

			if(key_info->state == JSON_INTERNAL_STATE_FOUND && key_info->current_index >= key_info->used)
				goto found;

			if(link_list->used <= 0)
				is_searching_key = true;
			else
			{
				link_list->data[link_list->used].current_index++;

				if(link_list->data[link_list->used].current_index >= link_list->data[link_list->used].size)
				{
					link_list->used--;

					if(key_info->current_index > link_list->used)
					{
						//We've finished the nest and the key that is the same name can't exist,
						//so there will be no hope finding a requested key.
						goto not_found;
					}

					is_searching_key = true;
				}

				if(link_list->data[link_list->used].is_object)
					is_searching_key = true;
			}
		}

		continue;

		found:
		//We found the all keys, get the value.
		if(data->tokens[i].type == JSMN_STRING)//Data type : string.
		{
			u32 escaped_size = data->tokens[i].end - data->tokens[i].start;
			char* escaped_text = (char*)malloc(escaped_size + 1);
			std::string unescaped_text = "";

			if(!escaped_text)
				goto out_of_memory;

			//Get the actual value.
			memcpy(escaped_text, (data->raw_json + data->tokens[i].start), escaped_size);
			escaped_text[escaped_size] = 0x00;//Add a null terminator.

			//Unescape it.
			unescaped_text = Util_json_unescape(escaped_text);
			free(escaped_text);
			escaped_text = NULL;

			extracted_data->value = (void*)malloc(unescaped_text.length() + 1);
			if(!extracted_data->value)
				goto out_of_memory;

			extracted_data->type = JSON_TYPE_STRING;
			extracted_data->size = unescaped_text.length();
			memcpy(extracted_data->value, unescaped_text.c_str(), extracted_data->size);
			((char*)extracted_data->value)[extracted_data->size] = 0x00;//Add a null terminator.
		}
		else if(data->tokens[i].type == JSMN_PRIMITIVE)
		{
			char type = *((char*)data->raw_json + data->tokens[i].start);

			if(type == 't' || type == 'f')//Data type : bool.
			{
				extracted_data->type = JSON_TYPE_BOOL;
				extracted_data->size = sizeof(bool);
			}
			else if(type == 'n')//Data type : null.
			{
				extracted_data->type = JSON_TYPE_NULL;
				extracted_data->size = 0;
			}
			else//Data type : number.
			{
				extracted_data->type = JSON_TYPE_NUMBER;
				extracted_data->size = sizeof(double);
			}

			if(extracted_data->type == JSON_TYPE_BOOL || extracted_data->type == JSON_TYPE_NUMBER)
			{
				extracted_data->value = (void*)malloc(extracted_data->size);
				if(!extracted_data->value)
					goto out_of_memory;

				//Get the actual value.
				if(extracted_data->type == JSON_TYPE_BOOL)
				{
					if(type == 't')
						*((bool*)extracted_data->value) = true;
					else
						*((bool*)extracted_data->value) = false;
				}
				else if(extracted_data->type == JSON_TYPE_NUMBER)
				{
					char* value_cache = NULL;
					u32 value_size = (data->tokens[i].end - data->tokens[i].start);

					value_cache = (char*)malloc(value_size + 1);
					if(!value_cache)
						goto out_of_memory;

					memcpy(value_cache, (data->raw_json + data->tokens[i].start), value_size);
					value_cache[value_size] = 0x00;//Null terminator.

					*((double*)extracted_data->value) = atof(value_cache);

					free(value_cache);
					value_cache = NULL;
				}
			}
		}

		break;
	}

	not_found:

	if(key_info)
	{
		if(key_info->data)
		{
			for(u32 i = 0; i < key_info->capacity; i++)
			{
				free(key_info->data[i].key);
				key_info->data[i].key = NULL;
			}
			free(key_info->data);
			key_info->data = NULL;
		}
		free(key_info);
		key_info = NULL;
	}

	if(link_list)
	{
		free(link_list->data);
		link_list->data = NULL;
		free(link_list);
		link_list = NULL;
	}

	return result;

	invalid_arg_0:
	result.code = DEF_ERR_INVALID_ARG;
	result.string = DEF_ERR_INVALID_ARG_STR;
	return result;

	invalid_arg_1:
	result.code = DEF_ERR_INVALID_ARG;
	result.string = DEF_ERR_INVALID_ARG_STR;
	goto free_ram;

	out_of_memory:
	result.code = DEF_ERR_OUT_OF_MEMORY;
	result.string = DEF_ERR_OUT_OF_MEMORY_STR;
	goto free_ram;

	free_ram:
	if(extracted_data)
	{
		free(extracted_data->value);
		extracted_data->size = 0;
		extracted_data->value = NULL;
		extracted_data->type = JSON_TYPE_INVALID;
	}

	if(key_info)
	{
		if(key_info->data)
		{
			for(u32 i = 0; i < key_info->capacity; i++)
			{
				free(key_info->data[i].key);
				key_info->data[i].key = NULL;
			}
			free(key_info->data);
			key_info->data = NULL;
		}
		free(key_info);
		key_info = NULL;
	}

	if(link_list)
	{
		free(link_list->data);
		link_list->data = NULL;
		free(link_list);
		link_list = NULL;
	}

	return result;
}

static std::string Util_json_unescape(char* escaped_string)
{
	int string_index = 0;
	int escaped_string_length = strlen(escaped_string);
	std::string unescaped_string = "";
	Json_internal_unescaping_search_state state = JSON_INTERNAL_UNESCAPE_STATE_SEARCHING_BACKSLASH;

	while ((escaped_string_length - string_index) > 0)
	{
		int char_length = mblen(&(escaped_string[string_index]), (escaped_string_length - string_index));
		if(char_length == 1)
		{
			switch (state)
			{
				case JSON_INTERNAL_UNESCAPE_STATE_SEARCHING_BACKSLASH:
				{
					if(escaped_string[string_index] == '\\')
					{
						//We found a start of escaping character.
						//e.g. "data 0 \n data 1"
						//             ^
						//"data 0 \t data 1"
						//        ^
						//"We love \u2622 so much."
						//         ^
						state = JSON_INTERNAL_UNESCAPE_STATE_SEARCHING_ESCAPED_CHAR;
					}
					else
						unescaped_string += escaped_string[string_index];//Normal character.

					break;
				}

				case JSON_INTERNAL_UNESCAPE_STATE_SEARCHING_ESCAPED_CHAR:
				{
					if(escaped_string[string_index] == 'u')
					{
						//We found a unicode escaping character.
						//"We love \u2622 so much."
						//          ^
						state = JSON_INTERNAL_UNESCAPE_STATE_SEARCHING_4_HEX_DIGITS;
						break;
					}

					for(unsigned int i = 0; i < (sizeof(json_escape_char_list) / sizeof(Json_internal_escape_char_list)); i++)
					{
						if(escaped_string[string_index] == json_escape_char_list[i].escaped_chars)
						{
							//We found a escaping character.
							//e.g. "data 0 \n data 1"
							//              ^
							//"data 0 \t data 1"
							//         ^
							unescaped_string += json_escape_char_list[i].unescaped_chars;
							break;
						}
					}

					state = JSON_INTERNAL_UNESCAPE_STATE_SEARCHING_BACKSLASH;

					break;
				}

				case JSON_INTERNAL_UNESCAPE_STATE_SEARCHING_4_HEX_DIGITS:
				{
					if((escaped_string_length - string_index) >= 4)
					{
						char escaped_unicode_temp[5] = { 0, 0, 0, 0, 0, };
						char out[5] = { 0, 0, 0, 0, 0, };
						uint32_t in = 0;

						memcpy(escaped_unicode_temp, &(escaped_string[string_index]), 4);
						sscanf(escaped_unicode_temp, "%lX", &in);

						//Convert escaped unicode char to actual unicode char.
						if(encode_utf8((uint8_t*)out, in) > 0)
							unescaped_string += out;
					}

					string_index += 3;
					state = JSON_INTERNAL_UNESCAPE_STATE_SEARCHING_BACKSLASH;

					break;
				}

				default:
					break;
			}

			string_index += char_length;
		}
		else if(char_length > 0)
		{
			char temp_char[char_length + 1];

			memcpy(temp_char, &(escaped_string[string_index]), char_length);
			temp_char[char_length] = 0x00;//Add a null terminator.
			unescaped_string += temp_char;
			string_index += char_length;
		}
		else
			string_index++;
	}

	return unescaped_string;
}

void Util_json_free(Json_data* parsed_json_data)
{
	Json_internal_data* data = NULL;

	if(!parsed_json_data)
		return;

	data = (Json_internal_data*)(parsed_json_data->internal_data);
	if(data)
	{
		free(data->raw_json);
		free(data->tokens);
		data->raw_json = NULL;
		data->tokens = NULL;
	}
	free(parsed_json_data->internal_data);
	parsed_json_data->internal_data = NULL;
}
