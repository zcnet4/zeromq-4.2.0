/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\fw\atomic.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-2 17:13
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef ATOMIC_H_
#define ATOMIC_H_

// -------------------------------------------------------------------------
#define ATOM_CAS(ptr, oval, nval) __sync_bool_compare_and_swap(ptr, oval, nval)
#define ATOM_CAS_POINTER(ptr, oval, nval) __sync_bool_compare_and_swap(ptr, oval, nval)
#define ATOM_INC(ptr) __sync_add_and_fetch(ptr, 1)
#define ATOM_FINC(ptr) __sync_fetch_and_add(ptr, 1)
#define ATOM_DEC(ptr) __sync_sub_and_fetch(ptr, 1)
#define ATOM_FDEC(ptr) __sync_fetch_and_sub(ptr, 1)
#define ATOM_ADD(ptr,n) __sync_add_and_fetch(ptr, n)
#define ATOM_SUB(ptr,n) __sync_sub_and_fetch(ptr, n)
#define ATOM_AND(ptr,n) __sync_and_and_fetch(ptr, n)

#ifdef _MSC_VER
#include "yx_export.h"
int YX_EXPORT __sync_fetch_and_sub(int *p, int n);
int YX_EXPORT __sync_fetch_and_add(int *p, int n);
int YX_EXPORT __sync_add_and_fetch(int *p, int n);
int YX_EXPORT __sync_sub_and_fetch(int *p, int n);
int YX_EXPORT __sync_lock_test_and_set(int *p, int n);
void YX_EXPORT __sync_lock_release(int *p);
void YX_EXPORT __sync_synchronize();
char YX_EXPORT __sync_bool_compare_and_swap(unsigned int *p, int value, int compare);
int YX_EXPORT __sync_and_and_fetch(int *p, int n);
#endif

// -------------------------------------------------------------------------
#endif /* ATOMIC_H_ */
