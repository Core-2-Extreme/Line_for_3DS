#ifndef _CRYPT_H
#define _CRYPT_H
#include <stdbool.h>
#include <stdint.h>

#define DEF_CRYPT_MD5_HASH_SIZE				16	//Hash size of MD5.
#define DEF_CRYPT_SHA_1_HASH_SIZE			20	//Hash size of SHA 1.
#define DEF_CRYPT_SHA_224_HASH_SIZE			28	//Hash size of SHA 224.
#define DEF_CRYPT_SHA_256_HASH_SIZE			32	//Hash size of SHA 256.
#define DEF_CRYPT_SHA_384_HASH_SIZE			48	//Hash size of SHA 384.
#define DEF_CRYPT_SHA_512_HASH_SIZE			64	//Hash size of SHA 512.

#define DEF_CRYPT_DES_KEY_SIZE				8	//Key size of DES.
#define DEF_CRYPT_DES_IV_SIZE				8	//Initialization vector size of DES.
#define DEF_CRYPT_DES_BLOCK_SIZE			8	//Block size of DES.
#define DEF_CRYPT_AES_128_KEY_SIZE			16	//Key size of AES 128.
#define DEF_CRYPT_AES_192_KEY_SIZE			24	//Key size of AES 192.
#define DEF_CRYPT_AES_256_KEY_SIZE			32	//Key size of AES 256.
#define DEF_CRYPT_AES_IV_SIZE				16	//Initialization vector size of AES.
#define DEF_CRYPT_AES_BLOCK_SIZE			16	//Block size of AES.

typedef enum
{
	CRYPT_HASH_TYPE_INVALID = -1,

	CRYPT_HASH_TYPE_MD5,		//MD5 message digest hashing algorithm.
	CRYPT_HASH_TYPE_SHA_1,		//Secure hash algorithm 1.
	CRYPT_HASH_TYPE_SHA_224,	//Secure hash algorithm 2 (224 bit).
	CRYPT_HASH_TYPE_SHA_256,	//Secure hash algorithm 2 (256 bit).
	CRYPT_HASH_TYPE_SHA_384,	//Secure hash algorithm 2 (384 bit).
	CRYPT_HASH_TYPE_SHA_512,	//Secure hash algorithm 2 (512 bit).

	CRYPT_HASH_TYPE_MAX,
} Util_crypt_hash_type;

typedef enum
{
	CRYPT_ENCRYPTION_TYPE_INVALID = -1,

	CRYPT_ENCRYPTION_TYPE_DES_ECB,		//Data encryption standard ECB mode.
	CRYPT_ENCRYPTION_TYPE_DES_CBC,		//Data encryption standard CBC mode.
	CRYPT_ENCRYPTION_TYPE_AES_128_ECB,	//Advanced encryption standard ECB mode (128 bit).
	CRYPT_ENCRYPTION_TYPE_AES_128_CBC,	//Advanced encryption standard CBC mode (128 bit).
	CRYPT_ENCRYPTION_TYPE_AES_192_ECB,	//Advanced encryption standard ECB mode (192 bit).
	CRYPT_ENCRYPTION_TYPE_AES_192_CBC,	//Advanced encryption standard CBC mode (192 bit).
	CRYPT_ENCRYPTION_TYPE_AES_256_ECB,	//Advanced encryption standard ECB mode (256 bit).
	CRYPT_ENCRYPTION_TYPE_AES_256_CBC,	//Advanced encryption standard CBC mode (256 bit).

	CRYPT_ENCRYPTION_TYPE_MAX,
} Util_crypt_encryption_type;

typedef enum
{
	CRYPT_DECRYPTION_TYPE_INVALID = -1,

	CRYPT_DECRYPTION_TYPE_DES_ECB,		//Data encryption standard ECB mode.
	CRYPT_DECRYPTION_TYPE_DES_CBC,		//Data encryption standard CBC mode.
	CRYPT_DECRYPTION_TYPE_AES_128_ECB,	//Advanced encryption standard ECB mode (128 bit).
	CRYPT_DECRYPTION_TYPE_AES_128_CBC,	//Advanced encryption standard CBC mode (128 bit).
	CRYPT_DECRYPTION_TYPE_AES_192_ECB,	//Advanced encryption standard ECB mode (192 bit).
	CRYPT_DECRYPTION_TYPE_AES_192_CBC,	//Advanced encryption standard CBC mode (192 bit).
	CRYPT_DECRYPTION_TYPE_AES_256_ECB,	//Advanced encryption standard ECB mode (256 bit).
	CRYPT_DECRYPTION_TYPE_AES_256_CBC,	//Advanced encryption standard CBC mode (256 bit).

	CRYPT_DECRYPTION_TYPE_MAX,
} Util_crypt_decryption_type;

typedef struct
{
	uint8_t* input;				//(in) Input buffer (plain data).
	uint32_t input_size;		//(in) Input size.
	uint8_t* hash;				//(out) Calculated hash, this buffer will be allocated inside of function.
	uint32_t hash_size;			//(out) Hash size.
	Util_crypt_hash_type type;	//(in) Hash type.
} Util_crypt_hash_parameters;

typedef struct
{
	uint8_t* input;						//(in) Input buffer (plain data, must be multiple of encryption type's block size).
	uint32_t input_size;				//(in) Input size.
	uint8_t* private_key;				//(in) Encryption key.
	uint16_t key_size;					//(in) Encryption key size (size requirement depends on encryption type to use).
	uint8_t* initialization_vector;		//(in/out) Initialization vector (requirement depends on encryption type to use).
	uint16_t iv_size;					//(in) Initialization vector size (size requirement depends on encryption type to use).
	uint8_t* encrypted;					//(out) Encrypted data, this buffer will be allocated inside of function.
	uint32_t encrypted_size;			//(out) Encrypted data size.
	Util_crypt_encryption_type type;	//(in) Encryption type.
} Util_crypt_encrypt_parameters;

typedef struct
{
	uint8_t* input;						//(in) Input buffer (encrypted data, must be multiple of encryption type's block size).
	uint32_t input_size;				//(in) Input size.
	uint8_t* private_key;				//(in) Encryption key.
	uint16_t key_size;					//(in) Encryption key size (size requirement depends on encryption type to use).
	uint8_t* initialization_vector;		//(in/out) Initialization vector (requirement depends on encryption type to use).
	uint16_t iv_size;					//(in) Initialization vector size (size requirement depends on encryption type to use).
	uint8_t* decrypted;					//(out) Decrypted data, this buffer will be allocated inside of function.
	uint32_t decrypted_size;			//(out) Decrypted data size.
	Util_crypt_decryption_type type;	//(in) Decryption type.
} Util_crypt_decrypt_parameters;

/**
 * @brief Calculate hash.
 * @param paraeters (in/out) Pointer for parameters. (See struct definitions for detail).
 * @return On success DEF_SUCCESS, on failure DEF_ERR_*.
 * @note Thread safe.
*/
uint32_t Util_crypt_hash(Util_crypt_hash_parameters* parameters);

/**
 * @brief Encrypt data.
 * @param paraeters (in/out) Pointer for parameters. (See struct definitions for detail).
 * @return On success DEF_SUCCESS, on failure DEF_ERR_*.
 * @note Thread safe.
*/
uint32_t Util_crypt_encrypt(Util_crypt_encrypt_parameters* parameters);

/**
 * @brief Decrypt data.
 * @param paraeters (in/out) Pointer for parameters. (See struct definitions for detail).
 * @return On success DEF_SUCCESS, on failure DEF_ERR_*.
 * @note Thread safe.
*/
uint32_t Util_crypt_decrypt(Util_crypt_decrypt_parameters* parameters);

#endif //_CRYPT_H
