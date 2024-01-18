#include "definitions.hpp"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "quirc/include/quirc.h"

#include "system/system_definitions.hpp"

//Include myself.
#include "system/util/qr.h"


void Util_qr_init_decode_parameters(Util_qr_decode_parameters* parameters)
{
	if(!parameters)
		return;

	parameters->is_identified = false;
	parameters->input = NULL;
	parameters->width = 0;
	parameters->height = 0;
	parameters->decoded_size = 0;
	parameters->decoded = NULL;
	memset(parameters->error_message, 0x00, sizeof(parameters->error_message));
}

uint32_t Util_qr_decode(Util_qr_decode_parameters* parameters)
{
	int32_t num_of_qr = 0;
	quirc_decode_error_t decode_result = QUIRC_ERROR_INVALID_VERSION;
	struct quirc* quirc_handle = NULL;

	if(!parameters || parameters->height == 0 || parameters->width == 0 || !parameters->input)
		goto invalid_arg;

	quirc_handle = quirc_new();
	if(!quirc_handle)
		goto out_of_memory;

	//This will allocate raw picture buffer.
	if(quirc_resize(quirc_handle, parameters->width, parameters->height) != 0)
		goto out_of_memory;

	//Copy raw image data.
	memcpy(quirc_begin(quirc_handle, NULL, NULL), parameters->input, (parameters->width * parameters->height));

	//Identify if there is a QR code.
	quirc_end(quirc_handle);
	num_of_qr = quirc_count(quirc_handle);
	parameters->is_identified = (num_of_qr > 0);

	//Try to decode it if QR code is identified.
	for(int32_t i = 0; i < num_of_qr; i++)
	{
		struct quirc_code code;//Library will initialize it internally.
		struct quirc_data data;//Library will initialize it internally.

		quirc_extract(quirc_handle, i, &code);

		decode_result = quirc_decode(&code, &data);
		if(decode_result == QUIRC_ERROR_DATA_ECC)
		{
			//Retry.
			quirc_flip(&code);
			decode_result = quirc_decode(&code, &data);
		}

		if(decode_result == QUIRC_SUCCESS)
		{
			parameters->decoded = (char*)malloc((data.payload_len + 1));
			if(!parameters->decoded)
				goto out_of_memory;

			memcpy(parameters->decoded, data.payload, data.payload_len);
			parameters->decoded[data.payload_len] = 0x00;//NULL terminator.
			parameters->decoded_size = data.payload_len;
			break;
		}
	}

	if(decode_result != QUIRC_SUCCESS && num_of_qr > 0)
		snprintf(parameters->error_message, sizeof(parameters->error_message), "%s", quirc_strerror(decode_result));

	if(quirc_handle)
	{
		quirc_destroy(quirc_handle);
		quirc_handle = NULL;
	}

	return ((decode_result == QUIRC_SUCCESS) ? DEF_SUCCESS : DEF_ERR_QUIRC_RETURNED_NOT_SUCCESS);

	invalid_arg:
	return DEF_ERR_INVALID_ARG;

	out_of_memory:
	if(quirc_handle)
	{
		quirc_destroy(quirc_handle);
		quirc_handle = NULL;
	}

	return DEF_ERR_OUT_OF_MEMORY;
}
