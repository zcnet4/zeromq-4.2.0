/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\yx_util.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-14 11:19
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "yx_util.h"
#include <fstream>
#include "yx_stl_util.h"
#include "uv.h"
#include <thread>
#include <chrono>
#include <string.h>
#include "build/compiler_specific.h"
#ifndef OS_WIN
#include <unistd.h>
#endif //
// -------------------------------------------------------------------------

namespace yx {
namespace util {
/*
@func			: GetCurrentDirectory
@brief		:
*/
bool GetCurrentDirectory(std::string* dir) {
#ifdef OS_WIN
  char system_buffer[MAX_PATH];
  system_buffer[0] = 0;
  DWORD len = ::GetCurrentDirectoryA(MAX_PATH, system_buffer);
  if (len == 0 || len > MAX_PATH)
    return false;
  dir->assign(system_buffer);
  return true;
#else
  char system_buffer[PATH_MAX] = "";
  if (!getcwd(system_buffer, sizeof(system_buffer))) {
    return false;
  }
  dir->assign(system_buffer);
  return true;
#endif
}

#ifndef OS_WIN
static char kSeparators = '/';
#else
static char kSeparators = '\\';
#endif

/*
@func			: PathSplit
@brief		:
*/
void PathSplit(const char* path, std::string* dir, std::string* name, std::string* ext) {
  if (dir) dir->clear();
  if (name) dir->clear();
  if (ext) dir->clear();
  //
  if (NULL == path || '/0' == *path) {
     // nothing 
  } else {
    char ch = path[strlen(path)-1];
    if (kSeparators == ch || '.' == ch) {
      if (dir) dir->assign(path);
    } else {
      const char* pname = strrchr(const_cast<char*>(path), kSeparators);
      if (pname) {
        pname++;
        if (dir) dir->assign(path, pname - path);
      } else {
        pname = path;
      }
      //
      std::string whole_name(pname);
      size_t pos = whole_name.find_last_of(".");
      if (std::string::npos != pos) {
        if (ext) ext->assign(whole_name.substr(pos + 1));
        whole_name.erase(pos);
      }
      if (name) name->swap(whole_name);
    }
  }
}

/*
@func			: ReadFileToBuffer
@brief		:
*/
bool ReadFileToBuffer(const char* file, std::vector<uint8_t>* buf) {
  bool success = false;
  //
  std::fstream	f;
  f.open(file, std::ios_base::in | std::ios_base::binary);
  if (f.is_open()) {
    f.seekg(0, std::ios::end);
    int file_size = static_cast<int>(f.tellg());
    f.seekg(0);
    //
    buf->resize(file_size);
    f.read((char*)vector_as_array(buf), std::streamsize(file_size));
    f.close();

    success = true;
  }
  //
  return success;
}

/*
@func			: TrimString
@brief		:
*/
void TrimString(std::string& str, const char* trim) {
  size_t right_found = str.find_last_not_of(trim);
  str.erase(right_found + 1);
  //
  size_t left_found = str.find_first_not_of(trim);
  str.erase(0, left_found);
}

/*
@func			: multwait_new
@brief		:
*/
void* multwait_new(unsigned int count) {
  uv_barrier_t* mw = new uv_barrier_t;
  uv_barrier_init(mw, count);
  return mw;
}
/*
@func			: multwait_wait
@brief		:
*/
bool multwait_wait(void* mw) {
  uv_barrier_t* b = reinterpret_cast<uv_barrier_t*>(mw);
  if (uv_barrier_wait(b) > 0) {
    // 最后个wait要等下，才能进行销毁。
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    uv_barrier_destroy(b);
    delete b;
    return true;
  }
  return false;
}

}; // namespace util
}; // namespace yx

// -------------------------------------------------------------------------
