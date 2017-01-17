/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\gateway\main.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-11-1 13:37
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include <stdlib.h>
#include <string>
#include "gateway.h"
#include "gateway_config.h"
#include "yx/logging.h"
#include "yx/glogger_async.h"
#include "build/compiler_specific.h"
#ifdef OS_WIN
#include <direct.h>
#else
#include <sys/stat.h>
#define USE_PROFILER
#endif

#ifdef USE_PROFILER 
#include "gperftools/profiler.h"
#endif
// -------------------------------------------------------------------------

class GuardGLog
{
public:
  GuardGLog(const char* argv0)
  {
    google::InitGoogleLogging(argv0);
    google::SetStderrLogging(google::GLOG_WARNING);
    FLAGS_colorlogtostderr = true;                                  //设置输出到屏幕的日志显示相应颜色
    FLAGS_logbufsecs = 0;                                           //缓冲日志输出，默认为30秒，此处改为立即输出
    FLAGS_max_log_size = 100;                                       // 单位MB
    FLAGS_stop_logging_if_full_disk = true;                         //当磁盘被写满时，停止日志输出
    config_log_dir(argv0);
#ifndef _WIN32
    google::InstallFailureSignalHandler();                          //捕捉 core dumped
    //google::InstallFailureWriter(&SignalHandle);
#endif
    //async_glogger_set(google::GLOG_INFO, google::base::GetLogger(google::GLOG_INFO));
    //google::base::SetLogger(google::GLOG_INFO, (google::base::Logger*)async_glogger_get(google::GLOG_INFO));
  }
  ~GuardGLog()
  {
    google::ShutdownGoogleLogging();
  }
private:
  void config_log_dir(const char* argv0) {
    std::string log_dir(argv0);
#ifdef _WIN32
    const char kSeparators = '\\';
#else
    const char kSeparators = '/';
#endif
    size_t last_backslash = log_dir.rfind(kSeparators, log_dir.size());
    if (last_backslash != std::string::npos) {
      log_dir.erase(last_backslash + 1);
      log_dir.append("gateway_log");
    } else {
      log_dir.append("/gateway_log");
    }
#ifdef _WIN32
    _mkdir(log_dir.c_str());
#else
    mkdir(log_dir.c_str(), S_IRWXU);
#endif    
    google::SetLogDestination(google::GLOG_INFO, (log_dir + "/INFO_").c_str());
    google::SetLogDestination(google::GLOG_WARNING, (log_dir + "/WARNING_").c_str());
    google::SetLogDestination(google::GLOG_ERROR, (log_dir + "/ERROR_").c_str());
    google::SetLogFilenameExtension("gateway.");
  }
};

int main(int argc, const char * argv[]) {
#ifdef USE_PROFILER
  ProfilerStart("gateway-profiler.log");
#endif
  // 日志
  GuardGLog gurad_glog(argv[0]);
  if (argv[1] && *argv[1])
    Config::kConfigFileName = argv[1];
  // 初始化配置文件并优化设置machine_id。by ZC. 2016-12-22 12:56.
  Config& cfg = Config::Instance();
  uint32_t machine_id = cfg.getMachineId();
  if (machine_id == 0 || machine_id >= 256) {
    LOG(ERROR) << "The machine_id must be greater than 0 and less than 256";
    LOG(FATAL) << "Gateway startup fatal";
    return 1;
  }
  yx::SetMachineId(machine_id);
  //
  Gateway gate;
  if (gate.Start()) {
    gate.Run();
  }
  gate.Stop();
  //
#ifdef USE_PROFILER 
  ProfilerStop();
#endif
  return 0;
}

// -------------------------------------------------------------------------
