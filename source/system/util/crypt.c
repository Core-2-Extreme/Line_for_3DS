#include "definitions.hpp"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbedtls/aes.h"
#include "mbedtls/des.h"
#include "mbedtls/md5.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"

#include "system/system_definitions.hpp"

//Include myself.
#include "system/util/crypt.h"


#define DEF_CRYPT_SHA_224_256_LIB_HASH_SIZE	32	//Hash size of SHA 224 and SHA 256 required by library.
#define DEF_CRYPT_SHA_384_512_LIB_HASH_SIZE	64	//Hash size of SHA 384 and SHA 512 required by library.


static uint32_t Util_crypt_hash_md5(uint8_t* input_buffer, uint32_t input_size, uint8_t* output_buffer);
static uint32_t Util_crypt_hash_sha1(uint8_t* input_buffer, uint32_t input_size, uint8_t* output_buffer);
static uint32_t Util_crypt_hash_sha224_256(uint8_t* input_buffer, uint32_t input_size, uint8_t* output_buffer, bool is_224);
static uint32_t Util_crypt_hash_sha384_512(uint8_t* input_buffer, uint32_t input_size, uint8_t* output_buffer, bool is_384);
static uint32_t Util_crypt_des_ecb(uint8_t* input_buffer, uint32_t buffer_size, uint8_t* output_buffer, uint8_t private_key[8], bool is_encrypt);
static uint32_t Util_crypt_des_cbc(uint8_t* input_buffer, uint32_t buffer_size, uint8_t* output_buffer, uint8_t private_key[8], uint8_t initialization_vector[8], bool is_encrypt);
static uint32_t Util_crypt_aes_ecb(uint8_t* input_buffer, uint32_t buffer_size, uint8_t* output_buffer, uint8_t* private_key, uint8_t key_size, bool is_encrypt);
static uint32_t Util_crypt_aes_cbc(uint8_t* input_buffer, uint32_t buffer_size, uint8_t* output_buffer, uint8_t* private_key, uint8_t initialization_vector[16], uint8_t key_size, bool is_encrypt);


uint32_t Util_crypt_hash(Util_crypt_hash_parameters* parameters)
{
	uint32_t result = DEF_ERR_OTHER;

	if(!parameters || parameters->type <= CRYPT_HASH_TYPE_INVALID || parameters->type >= CRYPT_HASH_TYPE_MAX
	|| !parameters->input || parameters->input_size == 0)
		goto invalid_arg;

	free(parameters->hash);
	parameters->hash = NULL;
	parameters->hash_size = 0;

	switch (parameters->type)
	{
		case CRYPT_HASH_TYPE_MD5:
		{
			parameters->hash = (uint8_t*)malloc(DEF_CRYPT_MD5_HASH_SIZE);
			if(!parameters->hash)
				goto out_of_memory;

			//Clear output buffer then calculate the hash.
			memset(parameters->hash, 0x00, DEF_CRYPT_MD5_HASH_SIZE);

			result = Util_crypt_hash_md5(parameters->input, parameters->input_size, parameters->hash);
			if(result != DEF_SUCCESS)
				goto error_other;

			parameters->hash_size = DEF_CRYPT_MD5_HASH_SIZE;

			break;
		}

		case CRYPT_HASH_TYPE_SHA_1:
		{
			parameters->hash = (uint8_t*)malloc(DEF_CRYPT_SHA_1_HASH_SIZE);
			if(!parameters->hash)
				goto out_of_memory;

			//Clear output buffer then calculate the hash.
			memset(parameters->hash, 0x00, DEF_CRYPT_SHA_1_HASH_SIZE);

			result = Util_crypt_hash_sha1(parameters->input, parameters->input_size, parameters->hash);
			if(result != DEF_SUCCESS)
				goto error_other;

			parameters->hash_size = DEF_CRYPT_SHA_1_HASH_SIZE;

			break;
		}

		case CRYPT_HASH_TYPE_SHA_224:
		case CRYPT_HASH_TYPE_SHA_256:
		{
			bool is_224 = (parameters->type == CRYPT_HASH_TYPE_SHA_224);

			//We need to allocate 32 bytes even we use sha 224 because library require 32 bytes buffer.
			parameters->hash = (uint8_t*)malloc(DEF_CRYPT_SHA_224_256_LIB_HASH_SIZE);
			if(!parameters->hash)
				goto out_of_memory;

			//Clear output buffer then calculate the hash.
			memset(parameters->hash, 0x00, DEF_CRYPT_SHA_224_256_LIB_HASH_SIZE);

			result = Util_crypt_hash_sha224_256(parameters->input, parameters->input_size, parameters->hash, is_224);
			if(result != DEF_SUCCESS)
				goto error_other;

			//Return actual hash size.
			if(is_224)
				parameters->hash_size = DEF_CRYPT_SHA_224_HASH_SIZE;
			else
				parameters->hash_size = DEF_CRYPT_SHA_256_HASH_SIZE;

			break;
		}

		case CRYPT_HASH_TYPE_SHA_384:
		case CRYPT_HASH_TYPE_SHA_512:
		{
			bool is_384 = (parameters->type == CRYPT_HASH_TYPE_SHA_384);

			//We need to allocate 64 bytes even we use sha 384 because library require 64 bytes buffer.
			parameters->hash = (uint8_t*)malloc(DEF_CRYPT_SHA_384_512_LIB_HASH_SIZE);
			if(!parameters->hash)
				goto out_of_memory;

			//Clear output buffer then calculate the hash.
			memset(parameters->hash, 0x00, DEF_CRYPT_SHA_384_512_LIB_HASH_SIZE);

			result = Util_crypt_hash_sha384_512(parameters->input, parameters->input_size, parameters->hash, is_384);
			if(result != DEF_SUCCESS)
				goto error_other;

			//Return actual hash size.
			if(is_384)
				parameters->hash_size = DEF_CRYPT_SHA_384_HASH_SIZE;
			else
				parameters->hash_size = DEF_CRYPT_SHA_512_HASH_SIZE;

			break;
		}

		default:
			break;
	}

	return result;

	invalid_arg:
	return DEF_ERR_INVALID_ARG;

	out_of_memory:
	return DEF_ERR_OUT_OF_MEMORY;

	error_other:
	free(parameters->hash);
	parameters->hash = NULL;
	return result;
}

uint32_t Util_crypt_encrypt(Util_crypt_encrypt_parameters* parameters)
{
	uint32_t result = DEF_ERR_OTHER;

	//Some format doesn't need initialization vector.
	if(!parameters || parameters->type <= CRYPT_ENCRYPTION_TYPE_INVALID || parameters->type >= CRYPT_ENCRYPTION_TYPE_MAX
	|| !parameters->input || parameters->input_size == 0 || !parameters->private_key || parameters->key_size == 0)
		goto invalid_arg;

	free(parameters->encrypted);
	parameters->encrypted = NULL;
	parameters->encrypted_size = 0;

	switch (parameters->type)
	{
		case CRYPT_ENCRYPTION_TYPE_DES_ECB:
		{
			uint32_t out_size = 0;

			//Input buffer size must be multiple of 8 bytes and key size must be 8 bytes.
			if((parameters->input_size % DEF_CRYPT_DES_BLOCK_SIZE) != 0 || parameters->key_size != DEF_CRYPT_DES_KEY_SIZE)
				goto invalid_arg;

			//Output size and input size are the same.
			out_size = parameters->input_size;
			parameters->encrypted = (uint8_t*)malloc(out_size);
			if(!parameters->encrypted)
				goto out_of_memory;

			//Clear output buffer then encrypt.
			memset(parameters->encrypted, 0x00, out_size);

			result = Util_crypt_des_ecb(parameters->input, parameters->input_size, parameters->encrypted, parameters->private_key, true);
			if(result != DEF_SUCCESS)
				goto error_other;

			parameters->encrypted_size = out_size;

			break;
		}

		case CRYPT_ENCRYPTION_TYPE_DES_CBC:
		{
			uint32_t out_size = 0;

			//Input buffer size must be multiple of 8 bytes, key size must be 8 bytes
			//and initialization vector size must be 8 bytes.
			if((parameters->input_size % DEF_CRYPT_DES_BLOCK_SIZE) != 0 || parameters->key_size != DEF_CRYPT_DES_KEY_SIZE
			|| !parameters->initialization_vector || parameters->iv_size != DEF_CRYPT_DES_IV_SIZE)
				goto invalid_arg;

			//Output size and input size are the same.
			out_size = parameters->input_size;
			parameters->encrypted = (uint8_t*)malloc(out_size);
			if(!parameters->encrypted)
				goto out_of_memory;

			//Clear output buffer then encrypt.
			memset(parameters->encrypted, 0x00, out_size);

			result = Util_crypt_des_cbc(parameters->input, parameters->input_size, parameters->encrypted,
			parameters->private_key, parameters->initialization_vector, true);
			if(result != DEF_SUCCESS)
				goto error_other;

			parameters->encrypted_size = out_size;

			break;
		}

		case CRYPT_ENCRYPTION_TYPE_AES_128_ECB:
		case CRYPT_ENCRYPTION_TYPE_AES_192_ECB:
		case CRYPT_ENCRYPTION_TYPE_AES_256_ECB:
		{
			uint32_t out_size = 0;
			uint8_t key_size = 0;

			if(parameters->type == CRYPT_ENCRYPTION_TYPE_AES_128_ECB)
				key_size = DEF_CRYPT_AES_128_KEY_SIZE;
			else if(parameters->type == CRYPT_ENCRYPTION_TYPE_AES_192_ECB)
				key_size = DEF_CRYPT_AES_192_KEY_SIZE;
			else if(parameters->type == CRYPT_ENCRYPTION_TYPE_AES_256_ECB)
				key_size = DEF_CRYPT_AES_256_KEY_SIZE;

			//Input buffer size must be multiple of 16 bytes and key size must be either 16, 24 or 32 bytes.
			if((parameters->input_size % DEF_CRYPT_AES_BLOCK_SIZE) != 0 || parameters->key_size != key_size)
				goto invalid_arg;

			//Output size and input size are the same.
			out_size = parameters->input_size;
			parameters->encrypted = (uint8_t*)malloc(out_size);
			if(!parameters->encrypted)
				goto out_of_memory;

			//Clear output buffer then encrypt.
			memset(parameters->encrypted, 0x00, out_size);

			result = Util_crypt_aes_ecb(parameters->input, parameters->input_size, parameters->encrypted, parameters->private_key, key_size, true);
			if(result != DEF_SUCCESS)
				goto error_other;

			parameters->encrypted_size = out_size;

			break;
		}

		case CRYPT_ENCRYPTION_TYPE_AES_128_CBC:
		case CRYPT_ENCRYPTION_TYPE_AES_192_CBC:
		case CRYPT_ENCRYPTION_TYPE_AES_256_CBC:
		{
			uint32_t out_size = 0;
			uint8_t key_size = 0;

			if(parameters->type == CRYPT_ENCRYPTION_TYPE_AES_128_CBC)
				key_size = DEF_CRYPT_AES_128_KEY_SIZE;
			else if(parameters->type == CRYPT_ENCRYPTION_TYPE_AES_192_CBC)
				key_size = DEF_CRYPT_AES_192_KEY_SIZE;
			else if(parameters->type == CRYPT_ENCRYPTION_TYPE_AES_256_CBC)
				key_size = DEF_CRYPT_AES_256_KEY_SIZE;

			//Input buffer size must be multiple of 16 bytes, key size must be either 16, 24 or 32 bytes
			//and initialization vector size must be 16 bytes.
			if((parameters->input_size % DEF_CRYPT_AES_BLOCK_SIZE) != 0 || parameters->key_size != key_size
			|| !parameters->initialization_vector || parameters->iv_size != DEF_CRYPT_AES_IV_SIZE)
				goto invalid_arg;

			//Output size and input size are the same.
			out_size = parameters->input_size;
			parameters->encrypted = (uint8_t*)malloc(out_size);
			if(!parameters->encrypted)
				goto out_of_memory;

			//Clear output buffer then encrypt.
			memset(parameters->encrypted, 0x00, out_size);

			result = Util_crypt_aes_cbc(parameters->input, parameters->input_size, parameters->encrypted,
			parameters->private_key, parameters->initialization_vector, key_size, true);
			if(result != DEF_SUCCESS)
				goto error_other;

			parameters->encrypted_size = out_size;

			break;
		}

		default:
			break;
	}

	return result;

	invalid_arg:
	return DEF_ERR_INVALID_ARG;

	out_of_memory:
	return DEF_ERR_OUT_OF_MEMORY;

	error_other:
	free(parameters->encrypted);
	parameters->encrypted = NULL;
	return result;
}

uint32_t Util_crypt_decrypt(Util_crypt_decrypt_parameters* parameters)
{
	uint32_t result = DEF_ERR_OTHER;

	//Some format doesn't need initialization vector.
	if(!parameters || parameters->type <= CRYPT_DECRYPTION_TYPE_INVALID || parameters->type >= CRYPT_DECRYPTION_TYPE_MAX
	|| !parameters->input || parameters->input_size == 0 || !parameters->private_key || parameters->key_size == 0)
		goto invalid_arg;

	free(parameters->decrypted);
	parameters->decrypted = NULL;
	parameters->decrypted_size = 0;

	switch (parameters->type)
	{
		case CRYPT_DECRYPTION_TYPE_DES_ECB:
		{
			uint32_t out_size = 0;

			//Input buffer size must be multiple of 8 bytes and key size must be 8 bytes.
			if((parameters->input_size % DEF_CRYPT_DES_BLOCK_SIZE) != 0 || parameters->key_size != DEF_CRYPT_DES_KEY_SIZE)
				goto invalid_arg;

			//Output size and input size are the same.
			out_size = parameters->input_size;
			parameters->decrypted = (uint8_t*)malloc(out_size);
			if(!parameters->decrypted)
				goto out_of_memory;

			//Clear output buffer then decrypt.
			memset(parameters->decrypted, 0x00, out_size);

			result = Util_crypt_des_ecb(parameters->input, parameters->input_size, parameters->decrypted, parameters->private_key, false);
			if(result != DEF_SUCCESS)
				goto error_other;

			parameters->decrypted_size = out_size;

			break;
		}

		case CRYPT_DECRYPTION_TYPE_DES_CBC:
		{
			uint32_t out_size = 0;

			//Input buffer size must be multiple of 8 bytes, key size must be 8 bytes
			//and initialization vector size must be 8 bytes.
			if((parameters->input_size % DEF_CRYPT_DES_BLOCK_SIZE) != 0 || parameters->key_size != DEF_CRYPT_DES_KEY_SIZE
			|| !parameters->initialization_vector || parameters->iv_size != DEF_CRYPT_DES_IV_SIZE)
				goto invalid_arg;

			//Output size and input size are the same.
			out_size = parameters->input_size;
			parameters->decrypted = (uint8_t*)malloc(out_size);
			if(!parameters->decrypted)
				goto out_of_memory;

			//Clear output buffer then decrypt.
			memset(parameters->decrypted, 0x00, out_size);

			result = Util_crypt_des_cbc(parameters->input, parameters->input_size, parameters->decrypted,
			parameters->private_key, parameters->initialization_vector, false);
			if(result != DEF_SUCCESS)
				goto error_other;

			parameters->decrypted_size = out_size;

			break;
		}

		case CRYPT_DECRYPTION_TYPE_AES_128_ECB:
		case CRYPT_DECRYPTION_TYPE_AES_192_ECB:
		case CRYPT_DECRYPTION_TYPE_AES_256_ECB:
		{
			uint32_t out_size = 0;
			uint8_t key_size = 0;

			if(parameters->type == CRYPT_DECRYPTION_TYPE_AES_128_ECB)
				key_size = DEF_CRYPT_AES_128_KEY_SIZE;
			else if(parameters->type == CRYPT_DECRYPTION_TYPE_AES_192_ECB)
				key_size = DEF_CRYPT_AES_192_KEY_SIZE;
			else if(parameters->type == CRYPT_DECRYPTION_TYPE_AES_256_ECB)
				key_size = DEF_CRYPT_AES_256_KEY_SIZE;

			//Input buffer size must be multiple of 16 bytes and key size must be either 16, 24 or 32 bytes.
			if((parameters->input_size % DEF_CRYPT_AES_BLOCK_SIZE) != 0 || parameters->key_size != key_size)
				goto invalid_arg;

			//Output size and input size are the same.
			out_size = parameters->input_size;
			parameters->decrypted = (uint8_t*)malloc(out_size);
			if(!parameters->decrypted)
				goto out_of_memory;

			//Clear output buffer then decrypt.
			memset(parameters->decrypted, 0x00, out_size);

			result = Util_crypt_aes_ecb(parameters->input, parameters->input_size, parameters->decrypted, parameters->private_key, key_size, false);
			if(result != DEF_SUCCESS)
				goto error_other;

			parameters->decrypted_size = out_size;

			break;
		}

		case CRYPT_DECRYPTION_TYPE_AES_128_CBC:
		case CRYPT_DECRYPTION_TYPE_AES_192_CBC:
		case CRYPT_DECRYPTION_TYPE_AES_256_CBC:
		{
			uint32_t out_size = 0;
			uint8_t key_size = 0;

			if(parameters->type == CRYPT_DECRYPTION_TYPE_AES_128_CBC)
				key_size = DEF_CRYPT_AES_128_KEY_SIZE;
			else if(parameters->type == CRYPT_DECRYPTION_TYPE_AES_192_CBC)
				key_size = DEF_CRYPT_AES_192_KEY_SIZE;
			else if(parameters->type == CRYPT_DECRYPTION_TYPE_AES_256_CBC)
				key_size = DEF_CRYPT_AES_256_KEY_SIZE;

			//Input buffer size must be multiple of 16 bytes, key size must be either 16, 24 or 32 bytes
			//and initialization vector size must be 16 bytes.
			if((parameters->input_size % DEF_CRYPT_AES_BLOCK_SIZE) != 0 || parameters->key_size != key_size
			|| !parameters->initialization_vector || parameters->iv_size != DEF_CRYPT_AES_IV_SIZE)
				goto invalid_arg;

			//Output size and input size are the same.
			out_size = parameters->input_size;
			parameters->decrypted = (uint8_t*)malloc(out_size);
			if(!parameters->decrypted)
				goto out_of_memory;

			//Clear output buffer then decrypt.
			memset(parameters->decrypted, 0x00, out_size);

			result = Util_crypt_aes_cbc(parameters->input, parameters->input_size, parameters->decrypted,
			parameters->private_key, parameters->initialization_vector, key_size, false);
			if(result != DEF_SUCCESS)
				goto error_other;

			parameters->decrypted_size = out_size;

			break;
		}

		default:
			break;
	}

	return result;

	invalid_arg:
	return DEF_ERR_INVALID_ARG;

	out_of_memory:
	return DEF_ERR_OUT_OF_MEMORY;

	error_other:
	free(parameters->decrypted);
	parameters->decrypted = NULL;
	return result;
}

static uint32_t Util_crypt_hash_md5(uint8_t* input_buffer, uint32_t input_size, uint8_t* output_buffer)
{
	mbedtls_md5_context md5_context = { 0, };

	mbedtls_md5_init(&md5_context);

	if(mbedtls_md5_starts_ret(&md5_context) != 0)
		goto mbed_tls_returned_not_success;

	//Calculate it.
	if(mbedtls_md5_update_ret(&md5_context, input_buffer, input_size) != 0)
		goto mbed_tls_returned_not_success;

	if(mbedtls_md5_finish_ret(&md5_context, output_buffer) != 0)
		goto mbed_tls_returned_not_success;

	mbedtls_md5_free(&md5_context);

	return DEF_SUCCESS;

	mbed_tls_returned_not_success:
	mbedtls_md5_free(&md5_context);
	return DEF_ERR_MBEDTLS_RETURNED_NOT_SUCCESS;
}

static uint32_t Util_crypt_hash_sha1(uint8_t* input_buffer, uint32_t input_size, uint8_t* output_buffer)
{
	mbedtls_sha1_context sha1_context = { 0, };

	mbedtls_sha1_init(&sha1_context);

	if(mbedtls_sha1_starts_ret(&sha1_context) != 0)
		goto mbed_tls_returned_not_success;

	//Calculate it.
	if(mbedtls_sha1_update_ret(&sha1_context, input_buffer, input_size) != 0)
		goto mbed_tls_returned_not_success;

	if(mbedtls_sha1_finish_ret(&sha1_context, output_buffer) != 0)
		goto mbed_tls_returned_not_success;

	mbedtls_sha1_free(&sha1_context);

	return DEF_SUCCESS;

	mbed_tls_returned_not_success:
	mbedtls_sha1_free(&sha1_context);
	return DEF_ERR_MBEDTLS_RETURNED_NOT_SUCCESS;
}

static uint32_t Util_crypt_hash_sha224_256(uint8_t* input_buffer, uint32_t input_size, uint8_t* output_buffer, bool is_224)
{
	mbedtls_sha256_context sha256_context = { 0, };

	mbedtls_sha256_init(&sha256_context);

	if(mbedtls_sha256_starts_ret(&sha256_context, is_224) != 0)
		goto mbed_tls_returned_not_success;

	//Calculate it.
	if(mbedtls_sha256_update_ret(&sha256_context, input_buffer, input_size) != 0)
		goto mbed_tls_returned_not_success;

	if(mbedtls_sha256_finish_ret(&sha256_context, output_buffer) != 0)
		goto mbed_tls_returned_not_success;

	mbedtls_sha256_free(&sha256_context);

	return DEF_SUCCESS;

	mbed_tls_returned_not_success:
	mbedtls_sha256_free(&sha256_context);
	return DEF_ERR_MBEDTLS_RETURNED_NOT_SUCCESS;
}

static uint32_t Util_crypt_hash_sha384_512(uint8_t* input_buffer, uint32_t input_size, uint8_t* output_buffer, bool is_384)
{
	mbedtls_sha512_context sha512_context = { 0, };

	mbedtls_sha512_init(&sha512_context);

	if(mbedtls_sha512_starts_ret(&sha512_context, is_384) != 0)
		goto mbed_tls_returned_not_success;

	//Calculate it.
	if(mbedtls_sha512_update_ret(&sha512_context, input_buffer, input_size) != 0)
		goto mbed_tls_returned_not_success;

	if(mbedtls_sha512_finish_ret(&sha512_context, output_buffer) != 0)
		goto mbed_tls_returned_not_success;

	mbedtls_sha512_free(&sha512_context);

	return DEF_SUCCESS;

	mbed_tls_returned_not_success:
	mbedtls_sha512_free(&sha512_context);
	return DEF_ERR_MBEDTLS_RETURNED_NOT_SUCCESS;
}

static uint32_t Util_crypt_des_ecb(uint8_t* input_buffer, uint32_t buffer_size, uint8_t* output_buffer, uint8_t private_key[8], bool is_encrypt)
{
	mbedtls_des_context des_context = { 0, };

	mbedtls_des_init(&des_context);

	if(is_encrypt)
	{
		if(mbedtls_des_setkey_enc(&des_context, private_key) != 0)
			goto mbed_tls_returned_not_success;
	}
	else
	{
		if(mbedtls_des_setkey_dec(&des_context, private_key) != 0)
			goto mbed_tls_returned_not_success;
	}

	//Encryption and decryption use the same function in ECB mode.
	for(uint32_t offset = 0; offset < buffer_size; offset += 8)
	{
		if(mbedtls_des_crypt_ecb(&des_context, (input_buffer + offset), (output_buffer + offset)) != 0)
			goto mbed_tls_returned_not_success;
	}

	mbedtls_des_free(&des_context);

	return DEF_SUCCESS;

	mbed_tls_returned_not_success:
	mbedtls_des_free(&des_context);
	return DEF_ERR_MBEDTLS_RETURNED_NOT_SUCCESS;
}

static uint32_t Util_crypt_des_cbc(uint8_t* input_buffer, uint32_t buffer_size, uint8_t* output_buffer, uint8_t private_key[8], uint8_t initialization_vector[8], bool is_encrypt)
{
	mbedtls_des_context des_context = { 0, };

	mbedtls_des_init(&des_context);

	if(is_encrypt)
	{
		if(mbedtls_des_setkey_enc(&des_context, private_key) != 0)
			goto mbed_tls_returned_not_success;

		//Encrypt it.
		if(mbedtls_des_crypt_cbc(&des_context, MBEDTLS_DES_ENCRYPT, buffer_size, initialization_vector, input_buffer, output_buffer) != 0)
			goto mbed_tls_returned_not_success;
	}
	else
	{
		if(mbedtls_des_setkey_dec(&des_context, private_key) != 0)
			goto mbed_tls_returned_not_success;

		//Decrypt it.
		if(mbedtls_des_crypt_cbc(&des_context, MBEDTLS_DES_DECRYPT, buffer_size, initialization_vector, input_buffer, output_buffer) != 0)
			goto mbed_tls_returned_not_success;
	}

	mbedtls_des_free(&des_context);

	return DEF_SUCCESS;

	mbed_tls_returned_not_success:
	mbedtls_des_free(&des_context);
	return DEF_ERR_MBEDTLS_RETURNED_NOT_SUCCESS;
}

static uint32_t Util_crypt_aes_ecb(uint8_t* input_buffer, uint32_t buffer_size, uint8_t* output_buffer, uint8_t* private_key, uint8_t key_size, bool is_encrypt)
{
	mbedtls_aes_context aes_context = { 0, };

	mbedtls_aes_init(&aes_context);

	if(is_encrypt)
	{
		if(mbedtls_aes_setkey_enc(&aes_context, private_key, (key_size * 8)) != 0)
			goto mbed_tls_returned_not_success;

		//Encrypt it.
		for(uint32_t offset = 0; offset < buffer_size; offset += 16)
		{
			if(mbedtls_aes_crypt_ecb(&aes_context, MBEDTLS_AES_ENCRYPT, (input_buffer + offset), (output_buffer + offset)) != 0)
				goto mbed_tls_returned_not_success;
		}
	}
	else
	{
		if(mbedtls_aes_setkey_dec(&aes_context, private_key, (key_size * 8)) != 0)
			goto mbed_tls_returned_not_success;

		//Decrypt it.
		for(uint32_t offset = 0; offset < buffer_size; offset += 16)
		{
			if(mbedtls_aes_crypt_ecb(&aes_context, MBEDTLS_AES_DECRYPT, (input_buffer + offset), (output_buffer + offset)) != 0)
				goto mbed_tls_returned_not_success;
		}
	}

	mbedtls_aes_free(&aes_context);

	return DEF_SUCCESS;

	mbed_tls_returned_not_success:
	mbedtls_aes_free(&aes_context);
	return DEF_ERR_MBEDTLS_RETURNED_NOT_SUCCESS;
}

static uint32_t Util_crypt_aes_cbc(uint8_t* input_buffer, uint32_t buffer_size, uint8_t* output_buffer, uint8_t* private_key, uint8_t initialization_vector[16], uint8_t key_size, bool is_encrypt)
{
	mbedtls_aes_context aes_context = { 0, };

	mbedtls_aes_init(&aes_context);

	if(is_encrypt)
	{
		if(mbedtls_aes_setkey_enc(&aes_context, private_key, (key_size * 8)) != 0)
			goto mbed_tls_returned_not_success;

		//Encrypt it.
		if(mbedtls_aes_crypt_cbc(&aes_context, MBEDTLS_AES_ENCRYPT, buffer_size, initialization_vector, input_buffer, output_buffer) != 0)
			goto mbed_tls_returned_not_success;
	}
	else
	{
		if(mbedtls_aes_setkey_dec(&aes_context, private_key, (key_size * 8)) != 0)
			goto mbed_tls_returned_not_success;

		//Decrypt it.
		if(mbedtls_aes_crypt_cbc(&aes_context, MBEDTLS_AES_DECRYPT, buffer_size, initialization_vector, input_buffer, output_buffer) != 0)
			goto mbed_tls_returned_not_success;
	}

	mbedtls_aes_free(&aes_context);

	return DEF_SUCCESS;

	mbed_tls_returned_not_success:
	mbedtls_aes_free(&aes_context);
	return DEF_ERR_MBEDTLS_RETURNED_NOT_SUCCESS;
}
