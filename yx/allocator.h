/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\allocator.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-12-20 17:15
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_
#include "yx_export.h"
#include <stddef.h>
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
//
YX_EXPORT void* yx_malloc(size_t sz);

YX_EXPORT void* yx_calloc(size_t nmemb, size_t size);

YX_EXPORT void* yx_realloc(void *ptr, size_t size);

YX_EXPORT void yx_free(void *ptr);

YX_EXPORT size_t yx_heap_size();
YX_EXPORT size_t yx_current_allocated_bytes();
//////////////////////////////////////////////////////////////////////////
// Anything that has done a #define new <blah> will screw operator new definitions up
// so undefine
#ifdef new
#  undef new
#endif
#ifdef delete
#  undef delete
#endif
template <int N = 0>
class YXAllocObject
{
public:
  explicit YXAllocObject(){ }
  ~YXAllocObject(){ }
  /// operator new, with debug line info
  void* operator new(size_t sz, const char* file, int line, const char* func) {
    return yx_malloc(sz);
  }
  void* operator new(size_t sz) {
    return yx_malloc(sz);
  }
  // placement operator new
  void* operator new(size_t sz, void* ptr) {
    (void)sz;
    return ptr;
  }
  // array operator new, with debug line info
  void* operator new[](size_t sz, const char* file, int line, const char* func) {
    return yx_malloc(sz);
  }
  void* operator new[](size_t sz) {
    return yx_malloc(sz);
  }
  void operator delete(void* ptr) {
    yx_free(ptr);
  }
  // Corresponding operator for placement delete (second param same as the first)
  void operator delete(void* ptr, void*) {
    yx_free(ptr);
  }
  // only called if there is an exception in corresponding 'new'
  void operator delete(void* ptr, const char*, int, const char*) {
    yx_free(ptr);
  }
  void operator delete[](void* ptr) {
    yx_free(ptr);
  }
  void operator delete[](void* ptr, const char*, int, const char*) {
    yx_free(ptr);
  }
};

//////////////////////////////////////////////////////////////////////////
// YXAllocator
template <typename T>
class YXAllocator
{
public:
  typedef size_t     size_type;
  typedef ptrdiff_t  difference_type;
  typedef T*         pointer;
  typedef const T*   const_pointer;
  typedef T&         reference;
  typedef const T&   const_reference;
  typedef T          value_type;

  template <class T1> struct rebind {
    typedef YXAllocator<T1> other;
  };

  YXAllocator() { }
  YXAllocator(const YXAllocator&) { }
  template <class T1> YXAllocator(const YXAllocator<T1>&) { }
  ~YXAllocator() { }

  pointer address(reference x) const { return &x; }
  const_pointer address(const_reference x) const { return &x; }

  pointer allocate(size_type n, const void* = 0) {
    //RAW_DCHECK((n * sizeof(T)) / sizeof(T) == n, "n is too big to allocate");
    return static_cast<T*>(yx_malloc(n * sizeof(T)));
  }
  void deallocate(pointer p, size_type n) { 
    yx_free(p); 
  }

  size_type max_size() const { return size_t(-1) / sizeof(T); }

  void construct(pointer p, const T& val) { ::new(p) T(val); }
  void construct(pointer p) { ::new(p) T(); }
  void destroy(pointer p) { p->~T(); }

  // There's no state, so these allocators are always equal
  bool operator==(const YXAllocator&) const { return true; }
};
// -------------------------------------------------------------------------
#endif /* ALLOCATOR_H_ */
