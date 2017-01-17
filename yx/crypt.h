/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\crypt.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-12-2 17:04
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef CRYPT_H_
#define CRYPT_H_
#include <stdint.h>
#include "yx_export.h"
// -------------------------------------------------------------------------
namespace yx {
namespace crypt {
/*
@func			: randomkey
@brief		: 
*/
YX_EXPORT void randomkey(uint8_t key[8]);
/*
@func			: dhexchange
@brief		:
*/
YX_EXPORT bool dhexchange(const uint8_t in[8], uint8_t out[8]);
/*
@func			: dhsecret
@brief		:
*/
YX_EXPORT bool dhsecret(const uint8_t x[8], const uint8_t y[8], uint8_t out[8]);
/*
@func			: hmac64
@brief		:
*/
YX_EXPORT void hmac64(const uint8_t x[8], const uint8_t y[8], uint8_t out[8]);
/*
@func			: desencode
@brief		:
*/
YX_EXPORT uint8_t* desencode(const uint8_t key[8], const uint8_t* buf, uint16_t buf_size);
/*
@func			: desdecode
@brief		:
*/
YX_EXPORT uint8_t* desdecode(const uint8_t key[8], const uint8_t* buf, uint16_t buf_size, uint16_t* size);
/*
@func			: xor_string
@brief		:
*/
YX_EXPORT void xor_string(uint8_t* str, uint16_t str_len, const uint8_t* key, uint16_t key_len);

}; // namespace crypt
}; // namespace yx

// -------------------------------------------------------------------------
#endif /* CRYPT_H_ */
