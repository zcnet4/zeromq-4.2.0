/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\yx_timer.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2017-1-9 16:11
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "yx_timer.h"
#include "allocator.h"
#include "uv.h"
#include "loop.h"
#include "loop_impl.h"
// -------------------------------------------------------------------------
namespace yx {
//////////////////////////////////////////////////////////////////////////
// Timer
Timer::Timer(yx::Loop* loop)
{
  uv_timer_init(loop->loop_impl()->uv_loop(), reinterpret_cast<uv_timer_t*>(this + 1));
  reinterpret_cast<uv_timer_t*>(this + 1)->data = this;
}


Timer::~Timer()
{

}

/*
@func			: New
@brief		:
*/
Timer* Timer::New(yx::Loop* loop) {
  return new Timer(loop);
}

void* Timer::operator new(size_t){
  return yx_malloc(sizeof(Timer) + sizeof(uv_timer_t));
}

void Timer::operator delete(void* ptr) {
  yx_free(ptr);
}

void Timer::timer_cb(uv_timer_t* handle) {
  Timer* timer = reinterpret_cast<Timer*>(handle->data);
  if (timer->cb_)
    timer->cb_(timer);
}


/*
@func			: Start
@brief		:
*/
void Timer::Start(uint64_t timeout) {
  uv_timer_start(reinterpret_cast<uv_timer_t*>(this + 1), &Timer::timer_cb, timeout, 0);
}

/*
@func			: Stop
@brief		:
*/
void Timer::Stop() {
  uv_timer_stop(reinterpret_cast<uv_timer_t*>(this + 1));
}


void Timer::close_cb(uv_handle_t* handle) {
  Timer* timer = reinterpret_cast<Timer*>(handle->data);
  delete timer;
}

/*
@func			: Close
@brief		:
*/
void Timer::Close() {
  uv_close(reinterpret_cast<uv_handle_t*>(this + 1), &Timer::close_cb);
}

}; // namespace yx

// -------------------------------------------------------------------------
