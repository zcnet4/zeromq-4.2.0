/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\yx_timer.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2017-1-9 16:11
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef YX_TIMER_H_
#define YX_TIMER_H_
#include "yx_export.h"
#include <stdint.h>
#include <functional>
// -------------------------------------------------------------------------
struct uv_timer_s;
typedef uv_timer_s uv_timer_t;
struct uv_handle_s;
typedef uv_handle_s uv_handle_t;
namespace yx {
class Loop;
//////////////////////////////////////////////////////////////////////////
//
class YX_EXPORT Timer
{
public:
  /*
  @func			: New
  @brief		: 
  */
  static Timer* New(yx::Loop* loop);
  /*
  @func			: Start
  @brief		: 
  */
  void Start(uint64_t timeout);
  /*
  @func			: 
  @brief		: 
  */
  void Stop();
  /*
  @func			: Close
  @brief		: 
  */
  void Close();
  /*
  @func			: set_cb
  @brief		: 
  */
  void set_cb(const std::function<void(Timer*)>& cb) {
    cb_ = cb;
  }
protected:
  static void timer_cb(uv_timer_t* handle);
  static void close_cb(uv_handle_t* handle);
private:
  Timer(yx::Loop* loop);
  Timer(const Timer&) = delete;
  Timer& operator=(const Timer&) = delete;
  ~Timer();
  //
  void* operator new(size_t);
  void* operator new(size_t, void* ptr){ return ptr; }
  void operator delete(void*);
  std::function<void(Timer*)> cb_;
};


}; // namespace yx


// -------------------------------------------------------------------------
#endif /* YX_TIMER_H_ */
