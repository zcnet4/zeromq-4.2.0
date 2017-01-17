/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\likely.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-12-15 9:44
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef LIKELY_H_
#define LIKELY_H_

// -------------------------------------------------------------------------
#undef LIKELY
#undef UNLIKELY

#if defined(__GNUC__) && __GNUC__ >= 4
#define LIKELY(x)   (__builtin_expect((x), 1))
#define UNLIKELY(x) (__builtin_expect((x), 0))
#else
#define LIKELY(x)   (x)
#define UNLIKELY(x) (x)
#endif

// -------------------------------------------------------------------------
#endif /* LIKELY_H_ */
