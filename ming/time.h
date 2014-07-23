#ifndef MING_TIME_H_
#define MING_TIME_H_

#if _MSC_VER < 1600
typedef unsigned long long uint64_t;
#else
#include <stdint.h>
#endif

const uint64_t NSEC_PER_SEC     = 1000000000;
const uint64_t NSEC_PER_MSEC    = 1000000;
const uint64_t NSEC_PER_USEC    = 1000;
const uint64_t MSEC_PER_SEC     = 1000;
const uint64_t USEC_PER_SEC     = 1000000;
const uint64_t EPOCH            = 0x19DB1DED53E8000i64;
const uint64_t NSEC100_PER_SEC  = (NSEC_PER_SEC / 100);
const uint64_t NSEC100_PER_MSEC = (NSEC_PER_MSEC / 100);

//======================windows====================================
#if defined(_MSC_VER)
#include <windows.h>
namespace ming {
  inline uint64_t get_time_ticks(void)
  {/* get system time in 100-nanosecond intervals since the epoch */
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return ((((uint64_t)ft.dwHighDateTime) << 32)
        + (uint64_t)ft.dwLowDateTime - EPOCH);
  }

  inline void gettimeofday(struct timeval *tp)
  {
    uint64_t  now = ming::get_time_ticks();
    tp->tv_sec = (long)(now / NSEC100_PER_SEC);
    tp->tv_usec = (long)((now % NSEC100_PER_SEC) / 10);
  }
} // namespace ming

#if (_MSC_VER >= 1700)
// c++ 11
#include <ratio>
#include <chrono>
namespace ming {
  inline uint64_t get_time_usec(void)
  { /* get system time in microseconds intervals since the epoch */
    using namespace std::chrono;
    high_resolution_clock::time_point tp1 = high_resolution_clock::now();
    microseconds usec = duration_cast<microseconds>(tp1.time_since_epoch());
    return usec.count();
  }
} // namespace ming
#else
namespace ming {
  inline uint64_t get_time_usec(void)
  { /* get system time in microseconds intervals since the epoch */
    uint64_t  now = ming::get_time_ticks();
    return now/10;
  }
} // namespace ming
#endif

// ======================linux==================================
#elif defined(__GNUC__)
// GNU C++
#include <sys/time.h>
namespace ming {
  inline void gettimeofday(struct timeval *tp)
  {
    ::gettimeofday(tp,NULL);
  }
} // namespace ming

#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7))
// c++ 11
#include <ratio>
#include <chrono>
namespace ming {
  inline uint64_t get_time_usec(void)
  { /* get system time in microseconds intervals since the epoch */
    using namespace std::chrono;
    high_resolution_clock::time_point tp1 = high_resolution_clock::now();
    microseconds usec = duration_cast<microseconds>(tp1.time_since_epoch());
    return usec.count();
  }
} // namespace ming
#else
namespace ming {
  inline uint64_t get_time_usec(void)
  { /* get system time in microseconds intervals since the epoch */
    struct timeval tv;
    ming::gettimeofday(&tv);
    return ((uint64_t)tv.tv_sec) * USEC_PER_SEC + (uint64_t)tv.tv_usec;
  }
} // namespace ming

//  /* nanoseconds resolution on linux*/
// #include <time.h>
// struct timespec ts;
// clock_gettime(CLOCK_REALTIME, &ts);
//
#endif

#else
  #error "Support Windows and Linux platform Only!"
#endif

#endif  // MING_TIME_H_
