/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\fw\loop.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-3 17:55
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "loop.h"
#include "uv.h"
#include "loop_impl.h"
#include "logging.h"
// -------------------------------------------------------------------------

namespace yx {
namespace {
class GuardLoopThreadLocal {
public:
  GuardLoopThreadLocal() {
    uv_key_create(&key_);
  }
  ~GuardLoopThreadLocal() {
    uv_key_delete(&key_);
  }
  Loop* Get() {
    return (Loop*)uv_key_get(&key_);
  }
  void Set(Loop* wrap) {
    uv_key_set(&key_, wrap);
  }
private:
  uv_key_t key_;
};
static GuardLoopThreadLocal s_guard_loop_thread_local;
};
//////////////////////////////////////////////////////////////////////////
// Loop
Loop::Loop()
	: impl_(new LoopImpl(this))
{
  // 为每个线程设置相关的LoopWrap。by Zc 2016-11-1 18:29.
  s_guard_loop_thread_local.Set(this);
}

Loop::~Loop()
{
  s_guard_loop_thread_local.Set(nullptr);
	impl_.reset();
}

Loop* Loop::current() {
  return s_guard_loop_thread_local.Get();
}

/*
@func			: Run
@brief		:
*/
int Loop::Run() {
	return impl_->Run();
}

/*
@func			: Quit
@brief		:
*/
void Loop::Quit() {
  impl_->PostTask(std::bind(&LoopImpl::Quit, impl_.get()));
}

/*
@func			: PostTask
@brief		:
*/
void Loop::PostTask(const Closure& task) {
	impl_->PostTask(task);
}

/*
@func			: Watch
@brief		:
*/
void Loop::Watch(const Closure& task) {
  impl_->Watch(task);
}

/*
@func			: task_runner
@brief		:
*/
std::shared_ptr<TaskRunner> Loop::task_runner() {
	return impl_->task_runner();
}


/*
@func			: loop_impl
@brief		:
*/
LoopImpl* Loop::loop_impl() {
  return impl_.get();
}

/*
@func			: loop_id
@brief		:
*/
uint8_t Loop::loop_id() const {
  return impl_->loop_id();
}

/*
@func			: uv_loop
@brief		:
*/
uv_loop_t* Loop::uv_loop() {
  return impl_->uv_loop();
}

/*
@func			: socket_manager
@brief		:
*/
SocketManager* Loop::socket_manager() {
  return impl_->socket_manager();
}

/*
@func			: ticks_now
@brief		:
*/
uint64_t Loop::ticks_now() const {
  return uv_now(impl_->uv_loop());
}

/*
@func			: time_now
@brief		:
*/
uint64_t Loop::time_now() const {
  return impl_->time_now();
}

static uint8_t g_machine_id = 0;
/*
@func			: SetMachineId
@brief		:
*/
void SetMachineId(uint8_t machine_id) {
  CHECK(g_machine_id == 0);
  g_machine_id = machine_id;
}

/*
@func			: GetMachineId
@brief		:
*/
uint8_t GetMachineId() {
  return g_machine_id;
}

}; // namespace yx

// -------------------------------------------------------------------------
