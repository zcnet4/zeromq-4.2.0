/* -------------------------------------------------------------------------
//	FileName		:	D:\yx_code\yx\yx\yx_time.cpp
//	Creator			:	(zc) <zcnet4@gmail.com>
//	CreateTime	:	2016-12-22 14:23
//	Description	:	
//
// -----------------------------------------------------------------------*/
#include "yx_time.h"
#include "uv.h"
#include "logging.h"
#ifdef OS_WIN
  #include <windows.h>
#else
  #include <sys/time.h>
#endif
// -------------------------------------------------------------------------
template <class Dest, class Source>
inline Dest bit_cast(const Source& source) {
  static_assert(sizeof(Dest) == sizeof(Source), "VerifySizesAreEqual");
  Dest dest;
  memcpy(&dest, &source, sizeof(dest));
  return dest;
};

namespace yx {
//////////////////////////////////////////////////////////////////////////
// TimeTicks
/*
@func			: Now
@brief		:
*/
TimeTicks TimeTicks::Now() {
  return TimeTicks(uv_hrtime());
}
//////////////////////////////////////////////////////////////////////////
// Time
#ifdef OS_WIN
// The internal representation of Time uses FILETIME, whose epoch is 1601-01-01
// 00:00:00 UTC.  ((1970-1601)*365+89)*24*60*60*1000*1000, where 89 is the
// number of leap year days between 1601 and 1970: (1970-1601)/4 excluding
// 1700, 1800, and 1900.
// static
const int64_t Time::kTimeTToMicrosecondsOffset = 11644473600000000I64;
// From MSDN, FILETIME "Contains a 64-bit value representing the number of
// 100-nanosecond intervals since January 1, 1601 (UTC)."
int64_t FileTimeToMicroseconds(const FILETIME& ft) {
  // Need to bit_cast to fix alignment, then divide by 10 to convert
  // 100-nanoseconds to milliseconds. This only works on little-endian
  // machines.
  return bit_cast<int64_t, FILETIME>(ft) / 10;
}

void MicrosecondsToFileTime(int64_t us, FILETIME* ft) {
  DCHECK_GE(us, 0LL) << "Time is less than 0, negative values are not "
    "representable in FILETIME";

  // Multiply by 10 to convert milliseconds to 100-nanoseconds. Bit_cast will
  // handle alignment problems. This only works on little-endian machines.
  *ft = bit_cast<FILETIME, int64_t>(us * 10);
}

int64_t SystemNow() {
  FILETIME ft;
  ::GetSystemTimeAsFileTime(&ft);
  return FileTimeToMicroseconds(ft);
}
#else
// Windows uses a Gregorian epoch of 1601.  We need to match this internally
// so that our time representations match across all platforms.  See bug 14734.
//   irb(main):010:0> Time.at(0).getutc()
//   => Thu Jan 01 00:00:00 UTC 1970
//   irb(main):011:0> Time.at(-11644473600).getutc()
//   => Mon Jan 01 00:00:00 UTC 1601
static const int64_t kWindowsEpochDeltaSeconds = 11644473600LL;
// static
const int64_t Time::kWindowsEpochDeltaMicroseconds =
kWindowsEpochDeltaSeconds * Time::kMicrosecondsPerSecond;

// Some functions in time.cc use time_t directly, so we provide an offset
// to convert from time_t (Unix epoch) and internal (Windows epoch).
// static
const int64_t Time::kTimeTToMicrosecondsOffset = kWindowsEpochDeltaMicroseconds;
int64_t SystemNow() {
  struct timeval tv;
  struct timezone tz = { 0, 0 };  // UTC
  if (gettimeofday(&tv, &tz) != 0) {
    DCHECK(0) << "Could not determine time of day";
    LOG(ERROR) << "Call to gettimeofday failed.";
    // Return null instead of uninitialized |tv| value, which contains random
    // garbage data. This may result in the crash seen in crbug.com/147570.
    return 0;
  }
  // Combine seconds and microseconds in a 64-bit field containing microseconds
  // since the epoch.  That's enough for nearly 600 centuries.  Adjust from
  // Unix (1970) to Windows (1601) epoch.
  return (tv.tv_sec * Time::kMicrosecondsPerSecond + tv.tv_usec) + Time::kWindowsEpochDeltaMicroseconds;
}
#endif // OS_WIN

/*
@func			: Now
@brief		:
*/
Time Time::Now() {
  return Time(SystemNow());
}


Time Time::FromTimeT(time_t tt) {
  return Time((tt * kMicrosecondsPerSecond) + kTimeTToMicrosecondsOffset);
}

time_t Time::ToTimeT() const {
  return (us_ - kTimeTToMicrosecondsOffset) / kMicrosecondsPerSecond;
}

uint64_t Time::ToMillisecond() const {
  return (us_ - kTimeTToMicrosecondsOffset) / kMicrosecondsPerMillisecond;
}

}; // namespace yx

// -------------------------------------------------------------------------
