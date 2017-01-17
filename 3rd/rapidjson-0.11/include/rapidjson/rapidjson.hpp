/* -------------------------------------------------------------------------
//    FileName		:	D:\yx_code\yx\3rd\rapidjson-0.11\include\rapidjson\rapidjson.hpp
//    Creator		  : (zc) <zcnet4@gmail.com>
//    CreateTime	:	2016-12-29 23:32
//    Description	:    
//
// -----------------------------------------------------------------------*/
#ifndef RAPIDJSON_H_
#define RAPIDJSON_H_

// -------------------------------------------------------------------------
#include "rapidjson.h"
#include "document.h"
#include "internal/stack.h"
#include "filestream.h"
#include "reader.h"
#include "writer.h"

class JsonBuffer
{
public:
  JsonBuffer()
    : allocator_(buffer_, sizeof(buffer_))
    , stack_(&allocator_, kDefaultCapacity)
  {
    
  }
  typedef rapidjson::UTF8<>::Ch Ch;
public:
  operator rapidjson::MemoryPoolAllocator<>*() {
    return &allocator_;
  }
  void Put(Ch c) { *stack_.template Push<Ch>() = c; }

  const char* GetString() const {
    // Push and pop a null terminator. This is safe.
    *stack_.Push<Ch>() = '\0';
    stack_.Pop<Ch>(1);

    return stack_.Bottom<Ch>();
  }
  size_t Size() const {
    return stack_.GetSize();
  }

  void Parse(const char* str, int str_len, rapidjson::Document& json_doc) {
    Ch* src = stack_.Push<Ch>(str_len);
    memcpy(src, str, str_len);
    // Push and pop a null terminator. This is safe.
    *stack_.template Push<Ch>() = '\0';
    json_doc.ParseInsitu<0>(src);
  }
private:
  char buffer_[8 * 1024];
  rapidjson::MemoryPoolAllocator<> allocator_;
  static const size_t kDefaultCapacity = 256;
  mutable rapidjson::internal::Stack<rapidjson::MemoryPoolAllocator<>> stack_;
};

// -------------------------------------------------------------------------
#endif /* RAPIDJSON_H_ */