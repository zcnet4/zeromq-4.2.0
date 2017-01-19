/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\fw\loop_impl.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-10-26 15:38
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "loop_impl.h"
#include "task_runner_impl.h"
#include "recv_cache.h"
#include <assert.h>
#include "logging.h"
#include "atomic.h"
#include "allocator.h"
#include "yx_time.h"
// -------------------------------------------------------------------------
namespace yx {
extern void udp_init();

namespace {
#pragma pack(push)
#pragma pack(1)
  struct uv_req_block_s {
    uint64_t req_pool;
    union {
      char padding_u[512 - sizeof(uint64_t)];
      uv_req_t req_u;                       // size:60
      uv_getaddrinfo_t getaddrinfo_u;       // size:112
      //uv_getnameinfo_t getnameinfo_u;       // size:1288
      uv_shutdown_t shutdown_u;             // size:68
      uv_write_t write_u;                   // size:92
      uv_connect_t connect_u;               // size:68
      uv_udp_send_t udp_u;                  // size:68
      uv_fs_t fs_u;                         // size:312
      uv_work_t work_u;                     // size:92
    } u;
  };
#pragma pack(pop)
  //
  static int s_loop_id_next = 0;
  static uv_once_t g_loop_init_guard = UV_ONCE_INIT;
  void loop_init() {
    uv_replace_allocator(yx_malloc, yx_realloc, yx_calloc, yx_free);
    udp_init();
  }
}

/*
@func			: GetMachineId
@brief		:
*/
extern uint8_t GetMachineId();

LoopImpl::LoopImpl(Loop* loop)
  : loop_(loop)
  , recv_cache_pool_("RecvCache", RecvCache::GetBlockSize())
  , uv_req_pool_("UVReqs", sizeof(uv_req_block_s))
  , socket_manager_(loop)
  , time_now_(0)
  , ticks_point_(0)
{
  uv_once(&g_loop_init_guard, loop_init);
  CHECK(s_loop_id_next < 32);
  fd_allocator_.set_worker_id(ATOM_INC(&s_loop_id_next));
  fd_allocator_.set_machine_id(GetMachineId());
  //
  int err = uv_loop_init(&uv_loop_);
  if (0 != err) {
    LOG(FATAL) << "uv loop init failed!!!";
  }
  uv_loop_.data = this;
  // 如果采用uv_idle_t，每帧都执行task_cb。故用uv_prepare_t加uv_async_t实现。by ZC.
  uv_prepare_init(&uv_loop_, &task_watcher_);
  task_watcher_.data = this;
  uv_prepare_start(&task_watcher_, &LoopImpl::task_cb);
  // uv_idle_t使用不当会导致程序高负载，但uv_idle_cb中wait又担心不能及时响应网络消息，故禁止使用uv_idle_t！！！ by ZC.
  uv_prepare_init(&uv_loop_, &queue_watcher_);
  queue_watcher_.data = this;
  // 接下来初始化时间。
  time_refresh();
	//
	task_runner_.reset(new TaskRunnerImpl(this));
  //
  LOG(INFO) << "loop[" << (uint32_t)loop_id() << "] " << recv_cache_pool_.name() << " Pool slice_size:" << recv_cache_pool_.slice_size();
  LOG(INFO) << "loop[" << (uint32_t)loop_id() << "] " << uv_req_pool_.name() << " Pool slice_size:" << uv_req_pool_.slice_size();
  //int s0 = sizeof(uv_req_block_s);
  //int s1 = sizeof(uv_req_t);
  //int s2 = sizeof(uv_getaddrinfo_t);
  //int s3 = sizeof(uv_getnameinfo_t);
  //int s4 = sizeof(uv_shutdown_t);
  //int s5 = sizeof(uv_write_t);
  //int s6 = sizeof(uv_connect_t);
  //int s7 = sizeof(uv_udp_send_t);
  //int s8 = sizeof(uv_fs_t);
  //int s9 = sizeof(uv_work_t);
}

LoopImpl::~LoopImpl()
{
  task_runner_->WillDestroy();
  socket_manager_.WillDestroy();
  // 关闭prepare监视器。
  uv_close((uv_handle_t*)&queue_watcher_, nullptr);
  uv_close((uv_handle_t*)&task_watcher_, nullptr);
  uv_run(&uv_loop_, UV_RUN_NOWAIT);
  //
  int err = uv_loop_close(&uv_loop_);
  if (0 != err) {
    LOG(ERROR) << "faild to close uv_loop. loop_id:" << (uint32_t)loop_id();
  }
  uv_loop_.data = NULL;
}

/*
@func			: Run
@brief		:
*/
int LoopImpl::Run() {
  int err = uv_run(&uv_loop_, UV_RUN_DEFAULT);
  // uv_loop里还有请求或事件需要处理。
  if (0 != err) {
    err = uv_run(&uv_loop_, UV_RUN_NOWAIT);
  }
  //
  return err;
}

/*
@func			: Quit
@brief		:
*/
void LoopImpl::Quit() {
  // 通知所有socket对象退出。
  socket_manager_.Shutdown();
  uv_prepare_stop(&queue_watcher_);
  uv_prepare_stop(&task_watcher_);
  // 通过uv_loop退出。
  uv_stop(&uv_loop_);
}


/*
@func			: Wakeup
@brief		:
*/
void LoopImpl::Wakeup() {
	task_runner_->Wakeup();
}

void LoopImpl::task_cb(uv_prepare_t* handle) {
  LoopImpl* impl = (LoopImpl*)handle->data;

	impl->task_runner_->RunTasks();
}

/*
@func			: PostTask
@brief		:
*/
void LoopImpl::PostTask(const Closure& task) {
	task_runner_->PostTask(task);
}

/*
@func			: task_runner
@brief		:
*/
std::shared_ptr<TaskRunner> LoopImpl::task_runner() {
	return task_runner_;
}

/*
@func			: time_now
@brief		:
*/
uint64_t LoopImpl::time_now() {
  uint64_t ticks_now = uv_now(&uv_loop_);
  if (ticks_now < ticks_point_) {
    uv_update_time(&uv_loop_);
    ticks_point_ = uv_now(&uv_loop_);
    time_now_ = yx::Time::Now().ToMillisecond();
  } else if (ticks_now != ticks_point_) {
    uint32_t diff = (uint32_t)(ticks_now - ticks_point_);
    ticks_point_ = ticks_now;
    time_now_ += diff;
  }
  //
  return time_now_;
}

/*
@func			: time_refresh
@brief		:
*/
void LoopImpl::time_refresh() {
  // 接下来初始化时间。
  uv_update_time(&uv_loop_);
  ticks_point_ = uv_now(&uv_loop_);
  time_now_ = yx::Time::Now().ToMillisecond();
}


void LoopImpl::queue_cb(uv_prepare_t* handle) {
  LoopImpl* impl = (LoopImpl*)handle->data;
  //
  if (impl->queue_task_){
    impl->queue_task_();
  }
}


/*
@func			: Watch
@brief		:
*/
void LoopImpl::Watch(const Closure& task) {
  if (task) {
    queue_task_ = task;
    uv_prepare_start(&queue_watcher_, &LoopImpl::queue_cb);
  } else {
    uv_prepare_stop(&queue_watcher_);
  }
}

/*
@func			: Register
@brief		:
*/
uint64_t LoopImpl::Register(Tcp* tcp) {
  return socket_manager_.SockNew(tcp, true);
}

uint64_t LoopImpl::Register(Udp* udp) {
  return socket_manager_.SockNew(udp, false);
}

/*
@func			: Unregister
@brief		:
*/
void LoopImpl::Unregister(uint64_t id) {
  socket_manager_.SockFree(id);
}

/*
@func			: alloc_fd
@brief		: 分配文件描述符。
*/
uint64_t LoopImpl::alloc_fd() {
  return fd_allocator_.generate(time_now());
}

/*
@func			: alloc_write_req
@brief		:
*/
void* LoopImpl::alloc_uv_req() {
  uint64_t* req = static_cast<uint64_t*>(uv_req_pool_.allocate());
  memset(req, 0, uv_req_pool_.slice_size());
  *req = reinterpret_cast<uint64_t>(&uv_req_pool_);
  return reinterpret_cast<void*>(req + 1);
}

/*
@func			: free_write_req
@brief		:
*/
void LoopImpl::free_uv_req(void* p) {
  uint64_t* req = reinterpret_cast<uint64_t*>(p);
  MemoryPool* pool = reinterpret_cast<MemoryPool*>(*(req - 1));
  pool->deallocate(req - 1);
}

}; // namespace yx

// -------------------------------------------------------------------------
