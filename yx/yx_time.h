/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\yx_time.h
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-12-22 14:29
//	Description	:	
//
// -----------------------------------------------------------------------*/
#ifndef YX_TIME_H_
#define YX_TIME_H_
#include <stdint.h>
#include <time.h>
#include "yx_export.h"
#include "build/compiler_specific.h"

// -------------------------------------------------------------------------
namespace yx {
//////////////////////////////////////////////////////////////////////////
// TimeTicks
class YX_EXPORT TimeTicks
{
public:
  TimeTicks():ticks_(0){}
  /*
  @func			: Now
  @brief		: 
  */
  static TimeTicks Now();
private:
  explicit TimeTicks(int64_t ticks):ticks_(ticks){}
  // Tick count in microseconds.
  int64_t ticks_;
};
//////////////////////////////////////////////////////////////////////////
// Time
class YX_EXPORT Time
{
  public:
  static const int64_t kMillisecondsPerSecond = 1000;
  static const int64_t kMicrosecondsPerMillisecond = 1000;
  static const int64_t kMicrosecondsPerSecond = kMicrosecondsPerMillisecond *
                                              kMillisecondsPerSecond;
  static const int64_t kMicrosecondsPerMinute = kMicrosecondsPerSecond * 60;
  static const int64_t kMicrosecondsPerHour = kMicrosecondsPerMinute * 60;
  static const int64_t kMicrosecondsPerDay = kMicrosecondsPerHour * 24;
  static const int64_t kMicrosecondsPerWeek = kMicrosecondsPerDay * 7;
  static const int64_t kNanosecondsPerMicrosecond = 1000;
  static const int64_t kNanosecondsPerSecond = kNanosecondsPerMicrosecond *
                                             kMicrosecondsPerSecond;

#if !defined(OS_WIN)
  // On Mac & Linux, this value is the delta from the Windows epoch of 1601 to
  // the Posix delta of 1970. This is used for migrating between the old
  // 1970-based epochs to the new 1601-based ones. It should be removed from
  // this global header and put in the platform-specific ones when we remove the
  // migration code.
  static const int64_t kWindowsEpochDeltaMicroseconds;
#endif
public:
  Time() :us_(0){}
  /*
  @func			: Now
  @brief		: 
  */
  static Time Now();
public:
  // Converts to/from time_t in UTC and a Time class.
  // TODO(brettw) this should be removed once everybody starts using the |Time|
  // class.
  static Time FromTimeT(time_t tt);
  time_t ToTimeT() const;
  uint64_t ToMillisecond() const;
private:
  explicit Time(int64_t us) : us_(us) {}
  // The representation of Jan 1, 1970 UTC in microseconds since the
  // platform-dependent epoch.
  static const int64_t kTimeTToMicrosecondsOffset;
  // Time in microseconds in UTC.
  int64_t us_;
};

}; // namespace yx


// -------------------------------------------------------------------------
#endif /* YX_TIME_H_ */
