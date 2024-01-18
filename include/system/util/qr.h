#ifndef _QR_H
#define _QR_H
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
	bool is_identified;		//(out) Whether QR code exists in the picture.
	uint8_t* input;			//(in) Input raw picture data (must be 8bit grayscaled image).
	uint32_t width;			//(in) Picture width in px.
	uint32_t height;		//(in) Picture height in px.
	uint32_t decoded_size;	//(out) Decoded data size in bytes.
	char* decoded;			//(out) Decoded data, this buffer will be allocated inside of function.
	char error_message[32];	//(out) Additional error message if available.
} Util_qr_decode_parameters;

/**
 * @brief Initialize Util_qr_decode_parameters.
 * @param paraeters (in/out) Pointer for parameters to initialize.
 * @note Thread safe.
*/
void Util_qr_init_decode_parameters(Util_qr_decode_parameters* parameters);

/**
 * @brief Decode QR code.
 * @param paraeters (in/out) Pointer for parameters. (See struct definition for explanation).
 * @return On success DEF_SUCCESS, on failure DEF_ERR_*.
 * @note Thread safe.
*/
uint32_t Util_qr_decode(Util_qr_decode_parameters* parameters);

#endif //_QR_H
