/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\allocator.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-12-20 17:15
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "allocator.h"
#include <stdlib.h>
#include "build/compiler_specific.h"
#ifdef OS_WIN
//#undef USE_TCMALLOC
// windows版本jemalloc需要vs2015编译。暂时不提供。
#undef USE_JEMALLOC
// Debug
//#include "../3rd/vld/src/vld.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
/*
int f = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
f |= _CRTDBG_CHECK_ALWAYS_DF;
f |= _CRTDBG_DELAY_FREE_MEM_DF;
_CrtSetDbgFlag(f);
*/
#else
#ifndef USE_TCMALLOC
#define USE_TCMALLOC
#endif
#endif // OS_WIN
// -------------------------------------------------------------------------
#ifdef USE_TCMALLOC
#include "gperftools/tcmalloc.h"
void* yx_malloc(size_t sz) {
  return tc_malloc(sz);
}

void* yx_calloc(size_t nmemb, size_t size) {
  return tc_calloc(nmemb, size);
}

void* yx_realloc(void *ptr, size_t size) {
  return tc_realloc(ptr, size);
}

void yx_free(void *ptr) {
  tc_free(ptr); 
}

#ifdef OS_WIN
#include "../gperftools/malloc_extension.h"
#else
#include "gperftools/malloc_extension.h"
#endif // OS_WIN
size_t yx_heap_size() {
  size_t heap_size = 0;
  MallocExtension* ext = MallocExtension::instance();
  ext->GetNumericProperty("generic.heap_size",
    &heap_size);
  return heap_size;
}
size_t yx_current_allocated_bytes() {
  size_t allocated_bytes = 0;
  MallocExtension* ext = MallocExtension::instance();
  ext->GetNumericProperty("generic.current_allocated_bytes",
    &allocated_bytes);
  return allocated_bytes;
}
#elif defined(USE_JEMALLOC)
#include "skynet/3rd/jemalloc/include/jemalloc/jemalloc.h"

void* yx_malloc(size_t sz) {
  void* ptr = je_malloc(sz);
  return ptr;
}

void* yx_calloc(size_t nmemb, size_t size) {
  void* ptr = je_calloc(nmemb, size);
  return ptr;
}

void* yx_realloc(void* ptr, size_t size) {
  void* newptr = je_realloc(ptr, size);
  return newptr;
}

void yx_free(void *ptr) {
  je_free(ptr);
}

size_t yx_heap_size() {
  return 0;
}
size_t yx_current_allocated_bytes() {
  return 0;
}
#else
void* yx_malloc(size_t sz) {
  return malloc(sz);
}

void* yx_calloc(size_t nmemb, size_t size) {
  return calloc(nmemb, size);
}

void* yx_realloc(void *ptr, size_t size) {
  return realloc(ptr, size);
}

void yx_free(void *ptr) {
  free(ptr); 
}

size_t yx_heap_size() {
  return 0;
}
size_t yx_current_allocated_bytes() {
  return 0;
}
#endif

// -------------------------------------------------------------------------
