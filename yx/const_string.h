// (C)2007 S2 Games
// k2_stl_strings.h
//
//=============================================================================
#ifndef __K2_STL_STRINGS_H__
#define __K2_STL_STRINGS_H__

//=============================================================================
// Headers
//=============================================================================
//#include "k2_stl_allocator.h"

#include <string>
#include <sstream>
//=============================================================================

//=============================================================================
// Declarations and definitions
//=============================================================================
#if defined(__GLIBCXX__)
// gcc's sso versa string outperforms their standard basic_string
#include <ext/vstring.h>
template <class charT, class Traits = std::char_traits<charT>, class Alloc = std::allocator<charT>>
using basic_string = __gnu_cxx::__versa_string<charT, Traits, Alloc>;
#else
using std::basic_string;
#endif

namespace K2
{
	typedef basic_string<char, std::char_traits<char>, std::allocator<char>>			string;
	typedef basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>>	wstring;
}

using K2::wstring;
using K2::string;
using std::fstream;
//=============================================================================

//=============================================================================
// const_string
//=============================================================================
#ifdef _MSC_VER

template<class _Elem, class _Traits, class _Ax>
class const_string : public std::basic_string<_Elem, _Traits, _Ax>, public NoCopy
{
public:
	~const_string()
	{
		_Bx._Ptr = nullptr;
	}

	const_string(const _Elem* const _Ptr) :
	basic_string()
	{
		_Bx._Ptr = (_Elem *)_Ptr;
		_Mysize = _Traits::length(_Ptr);
		_Myres = (size_type)-1;
	}

	const_string(const _Elem* const _Ptr, size_type _Count) :
	basic_string()
	{
		_Bx._Ptr = (_Elem*)_Ptr;
		_Mysize = _Count;
		_Myres = (size_type)-1;
	}
};

#elif defined(__GLIBCXX__)
// the following is specific to gcc-4.7's standard library
template <class _CharT, class _Traits, class _Alloc>
class sso_base_accessor : protected __gnu_cxx::__vstring_utility<_CharT, _Traits, _Alloc>
{
public:
	typedef _Traits							traits_type;
	typedef typename _Traits::char_type		value_type;

	typedef __gnu_cxx::__vstring_utility<_CharT, _Traits, _Alloc>	_Util_Base;
	typedef typename _Util_Base::_CharT_alloc_type					_CharT_alloc_type;
	typedef typename _CharT_alloc_type::size_type					size_type;

	// Data Members:
	typename _Util_Base::template _Alloc_hider<_CharT_alloc_type>	_M_dataplus;
	size_type														_M_string_length;

	enum { _S_local_capacity = 15 };

	union
	{
		_CharT		_M_local_data[_S_local_capacity + 1];
		size_type	_M_allocated_capacity;
	};
};

template <class _CharT, class _Traits = std::char_traits<_CharT>, class _Alloc = std::allocator<_CharT>>
class const_string : public __gnu_cxx::__versa_string<_CharT, _Traits, _Alloc>, public NoCopy
{
public:
	typedef typename __gnu_cxx::__versa_string<_CharT, _Traits, _Alloc> base_type;
	typedef typename base_type::size_type size_type;

	~const_string()
	{
		sso_base_accessor<_CharT, _Traits, _Alloc>* sso_base = reinterpret_cast<sso_base_accessor<_CharT, _Traits, _Alloc>*>(this);
		sso_base->_M_dataplus._M_p = sso_base->_M_local_data;
		sso_base->_M_string_length = 0;
	}

	const_string(const _CharT* const ptr)
	: base_type()
	{
		sso_base_accessor<_CharT, _Traits, _Alloc>* sso_base = reinterpret_cast<sso_base_accessor<_CharT, _Traits, _Alloc>*>(this);
		sso_base->_M_dataplus._M_p = const_cast<_CharT*>(ptr);
		sso_base->_M_string_length = _Traits::length(ptr);
	}

	const_string(const _CharT* const ptr, size_type count)
	{
		sso_base_accessor<_CharT, _Traits, _Alloc>* sso_base = reinterpret_cast<sso_base_accessor<_CharT, _Traits, _Alloc>*>(this);
		sso_base->_M_dataplus._M_p = const_cast<_CharT*>(ptr);
		sso_base->_M_string_length = count;
	}
};
#elif defined(_LIBCPP_VERSION)
// the following is specific to apple's libc++
template <class _CharT, class _Traits, class _Alloc>
class basic_string_accessor
{
public:
    enum {__long_mask  = 0x1ul};

	typedef typename std::basic_string<_CharT, _Traits, _Alloc>::size_type	size_type;
	typedef typename std::basic_string<_CharT, _Traits, _Alloc>::pointer	pointer;
	size_type __cap_;
	size_type __size_;
 	pointer   __data_;
};

template <class _CharT, class _Traits = std::char_traits<_CharT>, class _Alloc = std::allocator<_CharT>>
class const_string : public std::basic_string<_CharT, _Traits, _Alloc>, public NoCopy
{
public:
	typedef typename std::basic_string<_CharT, _Traits, _Alloc> base_type;
	typedef typename base_type::size_type size_type;

	~const_string()
	{
		basic_string_accessor<_CharT, _Traits, _Alloc>* string = reinterpret_cast<basic_string_accessor<_CharT, _Traits, _Alloc>*>(this);
		string->__size_ = 0;
		string->__cap_ = 0;
		string->__data_ = 0;
	}

	const_string(const _CharT* const ptr)
	: base_type()
	{
		basic_string_accessor<_CharT, _Traits, _Alloc>* string = reinterpret_cast<basic_string_accessor<_CharT, _Traits, _Alloc>*>(this);
		string->__size_ = _Traits::length(ptr);
		string->__cap_ = string->__size_ | basic_string_accessor<_CharT, _Traits, _Alloc>::__long_mask;
		string->__data_ = const_cast<_CharT*>(ptr);
	}

	const_string(const _CharT* const ptr, size_type count)
	{
		basic_string_accessor<_CharT, _Traits, _Alloc>* string = reinterpret_cast<basic_string_accessor<_CharT, _Traits, _Alloc>*>(this);
		string->__size_ = count;
		string->__cap_ = count | basic_string_accessor<_CharT, _Traits, _Alloc>::__long_mask;
		string->__data_ = const_cast<_CharT*>(ptr);
	}
};


#endif
//=============================================================================

#if defined(_MSC_VER) || defined(__GLIBCXX__) || defined(_LIBCPP_VERSION)
typedef const const_string<char, std::char_traits<char>, std::allocator<char> > cstring;
typedef const const_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> > cwstring;
#else
typedef const basic_string<char, std::char_traits<char>, K2_STRINGS_allocator<char> > cstring;
typedef const basic_string<wchar_t, std::char_traits<wchar_t>, K2_STRINGS_allocator<wchar_t> > cwstring;
#endif

#define _CS(s) ((const string&)(cstring(s, sizeof(s) / sizeof(char) - 1)))
#define _CWS(s) ((const wstring&)(cwstring(L##s, sizeof(L##s) / sizeof(wchar_t) - 1)))

#define _S(s) string(s, sizeof(s) / sizeof(char) - 1)
#define _WS(s) wstring((L##s), sizeof(L##s) / sizeof(wchar_t) - 1)

#define CONST_STRING(name, sz) const cwstring name(sz, sizeof(sz) / sizeof(wchar_t) - 1)

//#undef K2_STRINGS_allocator
//=============================================================================

#endif //__K2_STL_STRINGS_H__
