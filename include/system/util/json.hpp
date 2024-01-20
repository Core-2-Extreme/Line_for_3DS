#ifndef JSON_HPP
#define JSON_HPP

#if DEF_ENABLE_JSON_API
#include "system/types.hpp"

/**
 * @brief Parse json data for later use.
 * @param raw_json Pointer for NULL terminated json string.
 * @param parsed_json_data Pointer for parsed JSON data, the user
 * MUST free it by Util_json_free() when it's no longer needed.
 * @return On success DEF_SUCCESS,
 * on failure DEF_ERR_* or Nintendo API's error.
 * @note Thread safe
*/
Result_with_string Util_json_parse(char* raw_json, Json_data* parsed_json_data);

/**
 * @brief Extract data with a key.
 * @param key Key name.
 * @param parsed_json_data Pointer for parsed JSON data from Util_json_parse().
 * @param extracted_data Pointer for extracted value.
 * @return On success DEF_SUCCESS,
 * on failure DEF_ERR_* or Nintendo API's error.
 * @note Thread safe
*/
Result_with_string Util_json_get_data(std::string key, Json_data* parsed_json_data, Json_extracted_data* extracted_data);

/**
 * @brief Free parsed json data.
 * @param parsed_json_data Pointer for parsed JSON data.
 * @note Thread safe
*/
void Util_json_free(Json_data* parsed_json_data);

#else

#define Util_json_parse(...) Util_return_result_with_string(var_disabled_result)
#define Util_json_get_data(...) Util_return_result_with_string(var_disabled_result)
#define Util_json_free(...)

#endif

#endif
