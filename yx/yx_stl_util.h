/* -------------------------------------------------------------------------
//	FileName		：	base/stl_util_yx.h
//	Creator			：	(zc) <zcnet4@gmail.com>
//	CreateTime	：	2014-3-27 15:32
//	Description	：
//
// -----------------------------------------------------------------------*/
#ifndef BASE_STL_UTIL_YX_H_
#define BASE_STL_UTIL_YX_H_
#include <string>
#include <vector>
#include "build/compiler_specific.h"
#include "string_util.h"
//#include "base/strings/string_util.h"
//#include "base/strings/string_piece.h"
// -------------------------------------------------------------------------
//binary_function is deprecated in C++11 and removed in C++17. 
template<class T>
struct less_i
{	// functor for operator<
  bool operator()(const T& left, const T& right) const
  {	// apply operator< to operands
    return (left < right);
  }
};

MSVC_PUSH_DISABLE_WARNING(4996)
template<>
struct less_i<std::string>
{	// functor for operator<
  bool operator()(const std::string& left, const std::string& right) const
  {	// apply operator< to operands
    return yx::strcasecmp(left.c_str(), right.c_str()) < 0;
  }
};

template<>
struct less_i<std::wstring>
{	// functor for operator<
  bool operator()(const std::wstring& left, const std::wstring& right) const
  {	// apply operator< to operands
    //return (base::strcasecmp16(left.c_str(), right.c_str()) < 0);
    return yx::strcasecmp16(left.c_str(), right.c_str()) < 0;
  }
};

MSVC_POP_WARNING()

/*
template<>
struct less_i<base::StringPiece>
: public std::binary_function<base::StringPiece, base::StringPiece, bool>
{	// functor for operator<
bool operator()(const first_argument_type& left, const second_argument_type& right) const
{	// apply operator< to operands
return (base::strcasecmp_p(left, right) < 0);
}
};
*/

// To treat a possibly-empty vector as an array, use these functions.
// If you know the array will never be empty, you can use &*v.begin()
// directly, but that is undefined behaviour if |v| is empty.
template<typename T>
inline T* vector_as_array(std::vector<T>* v) {
  return v->empty() ? NULL : &*v->begin();
}

template<typename T>
inline const T* vector_as_array(const std::vector<T>* v) {
  return v->empty() ? NULL : &*v->begin();
}

// Return a mutable char* pointing to a string's internal buffer,
// which may not be null-terminated. Writing through this pointer will
// modify the string.
//
// string_as_array(&str)[i] is valid for 0 <= i < str.size() until the
// next call to a string method that invalidates iterators.
//
// As of 2006-04, there is no standard-blessed way of getting a
// mutable reference to a string's internal buffer. However, issue 530
// (http://www.open-std.org/JTC1/SC22/WG21/docs/lwg-active.html#530)
// proposes this as the method. According to Matt Austern, this should
// already work on all current implementations.
inline char* string_as_array(std::string* str) {
  // DO NOT USE const_cast<char*>(str->data())
  return str->empty() ? NULL : &*str->begin();
}
// -------------------------------------------------------------------------
#endif /* BASE_STL_UTIL_YX_H_ */
