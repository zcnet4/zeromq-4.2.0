﻿/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\crypt.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-12-2 17:04
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "crypt.h"
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "build/compiler_specific.h"
#ifdef OS_WIN
#define random rand
#define srandom srand
#else
#include <sys/time.h>
#endif // OS_WIN
// -------------------------------------------------------------------------

namespace yx {
namespace crypt {
namespace {
  bool init_radomkey() {
    srandom((unsigned int)time(NULL));
    return true;
  }
};
/*
@func			: randomkey
@brief		:
*/
void randomkey(uint8_t key[8]) {
  static bool _s_init = init_radomkey();
  //
  uint8_t x = 0;
  for (int i = 0; i < 8; i++) {
    key[i] = random() & 0xff;
    x ^= key[i];
  }
  if (x == 0) {
    key[0] |= 1;	// avoid 0
  }
}

namespace {
// powmodp64 for DH-key exchange
// The biggest 64bit prime
#define P 0xffffffffffffffc5ull

static inline uint64_t
  mul_mod_p(uint64_t a, uint64_t b) {
  uint64_t m = 0;
  while (b) {
    if (b & 1) {
      uint64_t t = P - a;
      if (m >= t) {
        m -= t;
      } else {
        m += a;
      }
    }
    if (a >= P - a) {
      a = a * 2 - P;
    } else {
      a = a * 2;
    }
    b >>= 1;
  }
  return m;
}

static inline uint64_t
  pow_mod_p(uint64_t a, uint64_t b) {
  if (b == 1) {
    return a;
  }
  uint64_t t = pow_mod_p(a, b >> 1);
  t = mul_mod_p(t, t);
  if (b % 2) {
    t = mul_mod_p(t, a);
  }
  return t;
}

// calc a^b % p
static uint64_t
  powmodp(uint64_t a, uint64_t b) {
  if (a > P)
    a %= P;
  return pow_mod_p(a, b);
}

void i64toint8(uint64_t r, uint8_t out[8]) {
  out[0] = r & 0xff;
  out[1] = (r >> 8) & 0xff;
  out[2] = (r >> 16) & 0xff;
  out[3] = (r >> 24) & 0xff;
  out[4] = (r >> 32) & 0xff;
  out[5] = (r >> 40) & 0xff;
  out[6] = (r >> 48) & 0xff;
  out[7] = (r >> 56) & 0xff;
}

void i32toint8(uint32_t r[2], uint8_t out[8]) {
  out[0] = r[0] & 0xff;
  out[1] = (r[0] >> 8) & 0xff;
  out[2] = (r[0] >> 16) & 0xff;
  out[3] = (r[0] >> 24) & 0xff;
  out[4] = (r[1] >> 0) & 0xff;
  out[5] = (r[1] >> 8) & 0xff;
  out[6] = (r[1] >> 16) & 0xff;
  out[7] = (r[1] >> 24) & 0xff;
}

void int8toi32(const uint8_t in[8], uint32_t r[2]) {
  r[0] = in[0] | in[1] << 8 | in[2] << 16 | in[3] << 24;
  r[1] = in[4] | in[5] << 8 | in[6] << 16 | in[7] << 24;
}

void int8toi64(const uint8_t in[8], uint64_t* r) {
  uint32_t l = in[0] | in[1] << 8 | in[2] << 16 | in[3] << 24;
  uint32_t h = in[4] | in[5] << 8 | in[6] << 16 | in[7] << 24;
  *r = h;
  *r <<= 32;
  *r |= l;
}

}

/*
@func			: dhexchange
@brief		: 
*/
#define G 5
bool dhexchange(const uint8_t in[8], uint8_t out[8]) {
  uint32_t xx[2];
  xx[0] = in[0] | in[1] << 8 | in[2] << 16 | in[3] << 24;
  xx[1] = in[4] | in[5] << 8 | in[6] << 16 | in[7] << 24;

  uint64_t x64 = (uint64_t)xx[0] | (uint64_t)xx[1] << 32;
  if (x64 == 0)
    return false;

  uint64_t r = powmodp(G, x64);
  i64toint8(r, out);
  //
  return true;
}

/*
@func			: dhsecret 
@brief		: 
*/
bool dhsecret(const uint8_t x[8], const uint8_t y[8], uint8_t out[8]) {
  uint64_t xx = 0, yy = 0;
  int8toi64(x, &xx);
  int8toi64(y, &yy);
  if (xx == 0 || yy == 0)
    return false;

  uint64_t r = powmodp(xx, yy);

  i64toint8(r, out);
  return true;
}

namespace {
  // Constants are the integer part of the sines of integers (in radians) * 2^32.
const uint32_t k[64] = {
0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee ,
0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501 ,
0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be ,
0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 ,
0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa ,
0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8 ,
0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed ,
0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a ,
0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c ,
0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70 ,
0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05 ,
0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 ,
0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039 ,
0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1 ,
0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1 ,
0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };
 
// r specifies the per-round shift amounts
const uint32_t r[] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
					  5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
					  4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
					  6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};
 
// leftrotate function definition
#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))
 
static void
hmac(uint32_t x[2], uint32_t y[2], uint32_t result[2]) {
	uint32_t w[16];
	uint32_t a, b, c, d, f, g, temp;
	int i;
 
	a = 0x67452301u;
	b = 0xefcdab89u;
	c = 0x98badcfeu;
	d = 0x10325476u;

	for (i=0;i<16;i+=4) {
		w[i] = x[1];
		w[i+1] = x[0];
		w[i+2] = y[1];
		w[i+3] = y[0];
	}

	for(i = 0; i<64; i++) {
		if (i < 16) {
			f = (b & c) | ((~b) & d);
			g = i;
		} else if (i < 32) {
			f = (d & b) | ((~d) & c);
			g = (5*i + 1) % 16;
		} else if (i < 48) {
			f = b ^ c ^ d;
			g = (3*i + 5) % 16; 
		} else {
			f = c ^ (b | (~d));
			g = (7*i) % 16;
		}

		temp = d;
		d = c;
		c = b;
		b = b + LEFTROTATE((a + f + k[i] + w[g]), r[i]);
		a = temp;

	}

	result[0] = c^d;
	result[1] = a^b;
}
}


/*
@func			: hmac64
@brief		:
*/
void hmac64(const uint8_t x[8], const uint8_t y[8], uint8_t out[8]) {
  uint32_t xx[2], yy[2], result[2];
  int8toi32(x, xx);
  int8toi32(y, yy);
  hmac(xx, yy, result);
  i32toint8(result, out);
}

namespace {
#define SMALL_CHUNK 256

/* the eight DES S-boxes */

static uint32_t SB1[64] = {
  0x01010400, 0x00000000, 0x00010000, 0x01010404,
  0x01010004, 0x00010404, 0x00000004, 0x00010000,
  0x00000400, 0x01010400, 0x01010404, 0x00000400,
  0x01000404, 0x01010004, 0x01000000, 0x00000004,
  0x00000404, 0x01000400, 0x01000400, 0x00010400,
  0x00010400, 0x01010000, 0x01010000, 0x01000404,
  0x00010004, 0x01000004, 0x01000004, 0x00010004,
  0x00000000, 0x00000404, 0x00010404, 0x01000000,
  0x00010000, 0x01010404, 0x00000004, 0x01010000,
  0x01010400, 0x01000000, 0x01000000, 0x00000400,
  0x01010004, 0x00010000, 0x00010400, 0x01000004,
  0x00000400, 0x00000004, 0x01000404, 0x00010404,
  0x01010404, 0x00010004, 0x01010000, 0x01000404,
  0x01000004, 0x00000404, 0x00010404, 0x01010400,
  0x00000404, 0x01000400, 0x01000400, 0x00000000,
  0x00010004, 0x00010400, 0x00000000, 0x01010004
};

static uint32_t SB2[64] = {
  0x80108020, 0x80008000, 0x00008000, 0x00108020,
  0x00100000, 0x00000020, 0x80100020, 0x80008020,
  0x80000020, 0x80108020, 0x80108000, 0x80000000,
  0x80008000, 0x00100000, 0x00000020, 0x80100020,
  0x00108000, 0x00100020, 0x80008020, 0x00000000,
  0x80000000, 0x00008000, 0x00108020, 0x80100000,
  0x00100020, 0x80000020, 0x00000000, 0x00108000,
  0x00008020, 0x80108000, 0x80100000, 0x00008020,
  0x00000000, 0x00108020, 0x80100020, 0x00100000,
  0x80008020, 0x80100000, 0x80108000, 0x00008000,
  0x80100000, 0x80008000, 0x00000020, 0x80108020,
  0x00108020, 0x00000020, 0x00008000, 0x80000000,
  0x00008020, 0x80108000, 0x00100000, 0x80000020,
  0x00100020, 0x80008020, 0x80000020, 0x00100020,
  0x00108000, 0x00000000, 0x80008000, 0x00008020,
  0x80000000, 0x80100020, 0x80108020, 0x00108000
};

static uint32_t SB3[64] = {
  0x00000208, 0x08020200, 0x00000000, 0x08020008,
  0x08000200, 0x00000000, 0x00020208, 0x08000200,
  0x00020008, 0x08000008, 0x08000008, 0x00020000,
  0x08020208, 0x00020008, 0x08020000, 0x00000208,
  0x08000000, 0x00000008, 0x08020200, 0x00000200,
  0x00020200, 0x08020000, 0x08020008, 0x00020208,
  0x08000208, 0x00020200, 0x00020000, 0x08000208,
  0x00000008, 0x08020208, 0x00000200, 0x08000000,
  0x08020200, 0x08000000, 0x00020008, 0x00000208,
  0x00020000, 0x08020200, 0x08000200, 0x00000000,
  0x00000200, 0x00020008, 0x08020208, 0x08000200,
  0x08000008, 0x00000200, 0x00000000, 0x08020008,
  0x08000208, 0x00020000, 0x08000000, 0x08020208,
  0x00000008, 0x00020208, 0x00020200, 0x08000008,
  0x08020000, 0x08000208, 0x00000208, 0x08020000,
  0x00020208, 0x00000008, 0x08020008, 0x00020200
};

static uint32_t SB4[64] = {
  0x00802001, 0x00002081, 0x00002081, 0x00000080,
  0x00802080, 0x00800081, 0x00800001, 0x00002001,
  0x00000000, 0x00802000, 0x00802000, 0x00802081,
  0x00000081, 0x00000000, 0x00800080, 0x00800001,
  0x00000001, 0x00002000, 0x00800000, 0x00802001,
  0x00000080, 0x00800000, 0x00002001, 0x00002080,
  0x00800081, 0x00000001, 0x00002080, 0x00800080,
  0x00002000, 0x00802080, 0x00802081, 0x00000081,
  0x00800080, 0x00800001, 0x00802000, 0x00802081,
  0x00000081, 0x00000000, 0x00000000, 0x00802000,
  0x00002080, 0x00800080, 0x00800081, 0x00000001,
  0x00802001, 0x00002081, 0x00002081, 0x00000080,
  0x00802081, 0x00000081, 0x00000001, 0x00002000,
  0x00800001, 0x00002001, 0x00802080, 0x00800081,
  0x00002001, 0x00002080, 0x00800000, 0x00802001,
  0x00000080, 0x00800000, 0x00002000, 0x00802080
};

static uint32_t SB5[64] = {
  0x00000100, 0x02080100, 0x02080000, 0x42000100,
  0x00080000, 0x00000100, 0x40000000, 0x02080000,
  0x40080100, 0x00080000, 0x02000100, 0x40080100,
  0x42000100, 0x42080000, 0x00080100, 0x40000000,
  0x02000000, 0x40080000, 0x40080000, 0x00000000,
  0x40000100, 0x42080100, 0x42080100, 0x02000100,
  0x42080000, 0x40000100, 0x00000000, 0x42000000,
  0x02080100, 0x02000000, 0x42000000, 0x00080100,
  0x00080000, 0x42000100, 0x00000100, 0x02000000,
  0x40000000, 0x02080000, 0x42000100, 0x40080100,
  0x02000100, 0x40000000, 0x42080000, 0x02080100,
  0x40080100, 0x00000100, 0x02000000, 0x42080000,
  0x42080100, 0x00080100, 0x42000000, 0x42080100,
  0x02080000, 0x00000000, 0x40080000, 0x42000000,
  0x00080100, 0x02000100, 0x40000100, 0x00080000,
  0x00000000, 0x40080000, 0x02080100, 0x40000100
};

static uint32_t SB6[64] = {
  0x20000010, 0x20400000, 0x00004000, 0x20404010,
  0x20400000, 0x00000010, 0x20404010, 0x00400000,
  0x20004000, 0x00404010, 0x00400000, 0x20000010,
  0x00400010, 0x20004000, 0x20000000, 0x00004010,
  0x00000000, 0x00400010, 0x20004010, 0x00004000,
  0x00404000, 0x20004010, 0x00000010, 0x20400010,
  0x20400010, 0x00000000, 0x00404010, 0x20404000,
  0x00004010, 0x00404000, 0x20404000, 0x20000000,
  0x20004000, 0x00000010, 0x20400010, 0x00404000,
  0x20404010, 0x00400000, 0x00004010, 0x20000010,
  0x00400000, 0x20004000, 0x20000000, 0x00004010,
  0x20000010, 0x20404010, 0x00404000, 0x20400000,
  0x00404010, 0x20404000, 0x00000000, 0x20400010,
  0x00000010, 0x00004000, 0x20400000, 0x00404010,
  0x00004000, 0x00400010, 0x20004010, 0x00000000,
  0x20404000, 0x20000000, 0x00400010, 0x20004010
};

static uint32_t SB7[64] = {
  0x00200000, 0x04200002, 0x04000802, 0x00000000,
  0x00000800, 0x04000802, 0x00200802, 0x04200800,
  0x04200802, 0x00200000, 0x00000000, 0x04000002,
  0x00000002, 0x04000000, 0x04200002, 0x00000802,
  0x04000800, 0x00200802, 0x00200002, 0x04000800,
  0x04000002, 0x04200000, 0x04200800, 0x00200002,
  0x04200000, 0x00000800, 0x00000802, 0x04200802,
  0x00200800, 0x00000002, 0x04000000, 0x00200800,
  0x04000000, 0x00200800, 0x00200000, 0x04000802,
  0x04000802, 0x04200002, 0x04200002, 0x00000002,
  0x00200002, 0x04000000, 0x04000800, 0x00200000,
  0x04200800, 0x00000802, 0x00200802, 0x04200800,
  0x00000802, 0x04000002, 0x04200802, 0x04200000,
  0x00200800, 0x00000000, 0x00000002, 0x04200802,
  0x00000000, 0x00200802, 0x04200000, 0x00000800,
  0x04000002, 0x04000800, 0x00000800, 0x00200002
};

static uint32_t SB8[64] = {
  0x10001040, 0x00001000, 0x00040000, 0x10041040,
  0x10000000, 0x10001040, 0x00000040, 0x10000000,
  0x00040040, 0x10040000, 0x10041040, 0x00041000,
  0x10041000, 0x00041040, 0x00001000, 0x00000040,
  0x10040000, 0x10000040, 0x10001000, 0x00001040,
  0x00041000, 0x00040040, 0x10040040, 0x10041000,
  0x00001040, 0x00000000, 0x00000000, 0x10040040,
  0x10000040, 0x10001000, 0x00041040, 0x00040000,
  0x00041040, 0x00040000, 0x10041000, 0x00001000,
  0x00000040, 0x10040040, 0x00001000, 0x00041040,
  0x10001000, 0x00000040, 0x10000040, 0x10040000,
  0x10040040, 0x10000000, 0x00040000, 0x10001040,
  0x00000000, 0x10041040, 0x00040040, 0x10000040,
  0x10040000, 0x10001000, 0x10001040, 0x00000000,
  0x10041040, 0x00041000, 0x00041000, 0x00001040,
  0x00001040, 0x00040040, 0x10000000, 0x10041000
};

/* PC1: left and right halves bit-swap */

static uint32_t LHs[16] = {
  0x00000000, 0x00000001, 0x00000100, 0x00000101,
  0x00010000, 0x00010001, 0x00010100, 0x00010101,
  0x01000000, 0x01000001, 0x01000100, 0x01000101,
  0x01010000, 0x01010001, 0x01010100, 0x01010101
};

static uint32_t RHs[16] = {
  0x00000000, 0x01000000, 0x00010000, 0x01010000,
  0x00000100, 0x01000100, 0x00010100, 0x01010100,
  0x00000001, 0x01000001, 0x00010001, 0x01010001,
  0x00000101, 0x01000101, 0x00010101, 0x01010101,
};

/* platform-independant 32-bit integer manipulation macros */

#define GET_UINT32(n,b,i)					   \
{											   \
(n) = ( (uint32_t) (b)[(i)	] << 24 )	   \
	| ( (uint32_t) (b)[(i) + 1] << 16 )	   \
	| ( (uint32_t) (b)[(i) + 2] <<  8 )	   \
	| ( (uint32_t) (b)[(i) + 3]	   );	  \
}

#define PUT_UINT32(n,b,i)					   \
  {											   \
(b)[(i)	] = (uint8_t) ( (n) >> 24 );	   \
(b)[(i) + 1] = (uint8_t) ( (n) >> 16 );	   \
(b)[(i) + 2] = (uint8_t) ( (n) >>  8 );	   \
(b)[(i) + 3] = (uint8_t) ( (n)	   );	   \
  }

/* Initial Permutation macro */

#define DES_IP(X,Y)											 \
  {															   \
T = ((X >>  4) ^ Y) & 0x0F0F0F0F; Y ^= T; X ^= (T <<  4);   \
T = ((X >> 16) ^ Y) & 0x0000FFFF; Y ^= T; X ^= (T << 16);   \
T = ((Y >>  2) ^ X) & 0x33333333; X ^= T; Y ^= (T <<  2);   \
T = ((Y >>  8) ^ X) & 0x00FF00FF; X ^= T; Y ^= (T <<  8);   \
Y = ((Y << 1) | (Y >> 31)) & 0xFFFFFFFF;					\
T = (X ^ Y) & 0xAAAAAAAA; Y ^= T; X ^= T;				   \
X = ((X << 1) | (X >> 31)) & 0xFFFFFFFF;					\
  }

/* Final Permutation macro */

#define DES_FP(X,Y)											 \
  {															   \
X = ((X << 31) | (X >> 1)) & 0xFFFFFFFF;					\
T = (X ^ Y) & 0xAAAAAAAA; X ^= T; Y ^= T;				   \
Y = ((Y << 31) | (Y >> 1)) & 0xFFFFFFFF;					\
T = ((Y >>  8) ^ X) & 0x00FF00FF; X ^= T; Y ^= (T <<  8);   \
T = ((Y >>  2) ^ X) & 0x33333333; X ^= T; Y ^= (T <<  2);   \
T = ((X >> 16) ^ Y) & 0x0000FFFF; Y ^= T; X ^= (T << 16);   \
T = ((X >>  4) ^ Y) & 0x0F0F0F0F; Y ^= T; X ^= (T <<  4);   \
  }

/* DES round macro */

#define DES_ROUND(X,Y)						  \
  {											   \
T = *SK++ ^ X;							  \
Y ^= SB8[ (T	  ) & 0x3F ] ^			  \
		SB6[ (T >>  8) & 0x3F ] ^			  \
		SB4[ (T >> 16) & 0x3F ] ^			  \
		SB2[ (T >> 24) & 0x3F ];			   \
											\
T = *SK++ ^ ((X << 28) | (X >> 4));		 \
Y ^= SB7[ (T	  ) & 0x3F ] ^			  \
		SB5[ (T >>  8) & 0x3F ] ^			  \
		SB3[ (T >> 16) & 0x3F ] ^			  \
		SB1[ (T >> 24) & 0x3F ];			   \
  }

/* DES key schedule */

static void
  des_main_ks(uint32_t SK[32], const uint8_t key[8]) {
  int i;
  uint32_t X, Y, T;

  GET_UINT32(X, key, 0);
  GET_UINT32(Y, key, 4);

  /* Permuted Choice 1 */

  T = ((Y >> 4) ^ X) & 0x0F0F0F0F;  X ^= T; Y ^= (T << 4);
  T = ((Y) ^ X) & 0x10101010;  X ^= T; Y ^= (T);

  X = (LHs[(X)& 0xF] << 3) | (LHs[(X >> 8) & 0xF] << 2)
    | (LHs[(X >> 16) & 0xF] << 1) | (LHs[(X >> 24) & 0xF])
    | (LHs[(X >> 5) & 0xF] << 7) | (LHs[(X >> 13) & 0xF] << 6)
    | (LHs[(X >> 21) & 0xF] << 5) | (LHs[(X >> 29) & 0xF] << 4);

  Y = (RHs[(Y >> 1) & 0xF] << 3) | (RHs[(Y >> 9) & 0xF] << 2)
    | (RHs[(Y >> 17) & 0xF] << 1) | (RHs[(Y >> 25) & 0xF])
    | (RHs[(Y >> 4) & 0xF] << 7) | (RHs[(Y >> 12) & 0xF] << 6)
    | (RHs[(Y >> 20) & 0xF] << 5) | (RHs[(Y >> 28) & 0xF] << 4);

  X &= 0x0FFFFFFF;
  Y &= 0x0FFFFFFF;

  /* calculate subkeys */

  for (i = 0; i < 16; i++) {
    if (i < 2 || i == 8 || i == 15) {
      X = ((X << 1) | (X >> 27)) & 0x0FFFFFFF;
      Y = ((Y << 1) | (Y >> 27)) & 0x0FFFFFFF;
    } else {
      X = ((X << 2) | (X >> 26)) & 0x0FFFFFFF;
      Y = ((Y << 2) | (Y >> 26)) & 0x0FFFFFFF;
    }

    *SK++ = ((X << 4) & 0x24000000) | ((X << 28) & 0x10000000)
      | ((X << 14) & 0x08000000) | ((X << 18) & 0x02080000)
      | ((X << 6) & 0x01000000) | ((X << 9) & 0x00200000)
      | ((X >> 1) & 0x00100000) | ((X << 10) & 0x00040000)
      | ((X << 2) & 0x00020000) | ((X >> 10) & 0x00010000)
      | ((Y >> 13) & 0x00002000) | ((Y >> 4) & 0x00001000)
      | ((Y << 6) & 0x00000800) | ((Y >> 1) & 0x00000400)
      | ((Y >> 14) & 0x00000200) | ((Y)& 0x00000100)
      | ((Y >> 5) & 0x00000020) | ((Y >> 10) & 0x00000010)
      | ((Y >> 3) & 0x00000008) | ((Y >> 18) & 0x00000004)
      | ((Y >> 26) & 0x00000002) | ((Y >> 24) & 0x00000001);

    *SK++ = ((X << 15) & 0x20000000) | ((X << 17) & 0x10000000)
      | ((X << 10) & 0x08000000) | ((X << 22) & 0x04000000)
      | ((X >> 2) & 0x02000000) | ((X << 1) & 0x01000000)
      | ((X << 16) & 0x00200000) | ((X << 11) & 0x00100000)
      | ((X << 3) & 0x00080000) | ((X >> 6) & 0x00040000)
      | ((X << 15) & 0x00020000) | ((X >> 4) & 0x00010000)
      | ((Y >> 2) & 0x00002000) | ((Y << 8) & 0x00001000)
      | ((Y >> 14) & 0x00000808) | ((Y >> 9) & 0x00000400)
      | ((Y)& 0x00000200) | ((Y << 7) & 0x00000100)
      | ((Y >> 7) & 0x00000020) | ((Y >> 3) & 0x00000011)
      | ((Y << 2) & 0x00000004) | ((Y >> 21) & 0x00000002);
  }
}

/* DES 64-bit block encryption/decryption */

static void
  des_crypt(const uint32_t SK[32], const uint8_t input[8], uint8_t output[8]) {
  uint32_t X, Y, T;

  GET_UINT32(X, input, 0);
  GET_UINT32(Y, input, 4);

  DES_IP(X, Y);

  DES_ROUND(Y, X);  DES_ROUND(X, Y);
  DES_ROUND(Y, X);  DES_ROUND(X, Y);
  DES_ROUND(Y, X);  DES_ROUND(X, Y);
  DES_ROUND(Y, X);  DES_ROUND(X, Y);
  DES_ROUND(Y, X);  DES_ROUND(X, Y);
  DES_ROUND(Y, X);  DES_ROUND(X, Y);
  DES_ROUND(Y, X);  DES_ROUND(X, Y);
  DES_ROUND(Y, X);  DES_ROUND(X, Y);

  DES_FP(Y, X);

  PUT_UINT32(Y, output, 0);
  PUT_UINT32(X, output, 4);
}
}

/*
@func			: desencode
@brief		:
*/
uint8_t* desencode(const uint8_t key[8], const uint8_t* buf, uint16_t buf_size) {
  uint32_t SK[32];
  des_main_ks(SK, key);
  //
  size_t crypt_buf_size = (buf_size + 8) & ~7;
  uint8_t* crypt_buf = new uint8_t[crypt_buf_size];
  int i = 0;
  for (i = 0; i < (int)buf_size - 7; i += 8) {
    des_crypt(SK, buf + i, crypt_buf + i);
  }
  int bytes = buf_size - i;
  uint8_t tail[8];
  int j;
  for (j = 0; j < 8; j++) {
    if (j < bytes) {
      tail[j] = buf[i + j];
    } else if (j == bytes) {
      tail[j] = 0x80;
    } else {
      tail[j] = 0;
    }
  }
  des_crypt(SK, tail, crypt_buf + i);
  //
  return crypt_buf;
}

/*
@func			: desdecode
@brief		:
*/
uint8_t* desdecode(const uint8_t key[8], const uint8_t* buf, uint16_t buf_size, uint16_t* size) {
  uint32_t ESK[32];
  des_main_ks(ESK, key);
  uint32_t SK[32];
   for (size_t i = 0; i < 32; i += 2) {
    SK[i] = ESK[30 - i];
    SK[i + 1] = ESK[31 - i];
  }
  //
   uint8_t* buffer = new uint8_t[buf_size];
   for (size_t i = 0; i < buf_size; i += 8) {
     des_crypt(SK, buf + i, buffer + i);
   }
   int padding = 1;
   for (int i = buf_size - 1; i >= buf_size - 8; i--) {
     if (buffer[i] == 0) {
       padding++;
     } else if (buffer[i] == 0x80) {
       break;
     } else {
       delete[] buffer;
       return nullptr;
     }
   }
   if (padding > 8) {
     delete[] buffer;
     return nullptr;
   }
   *size = buf_size - padding;
   return buffer;
}

/*
@func			: xor_string
@brief		:
*/
void xor_string(uint8_t* str, uint16_t str_len, const uint8_t* key, uint16_t key_len) {
  for (int i = 0; i < str_len; i++) {
    str[i] ^= key[i % key_len];
  }
}

}; // namespace crypt
}; // namespace yx

// -------------------------------------------------------------------------
