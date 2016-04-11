#ifndef MING_TIME_H_
#define MING_TIME_H_

#if _MSC_VER < 1600
typedef unsigned long long uint64_t;
#else
#include <stdint.h>
#endif

namespace ming {
const uint64_t NSEC_PER_SEC = 1000000000;
const uint64_t NSEC_PER_MSEC = 1000000;
const uint64_t NSEC_PER_USEC = 1000;
const uint64_t MSEC_PER_SEC = 1000;
const uint64_t USEC_PER_SEC = 1000000;
const uint64_t EPOCH = 0x19DB1DED53E8000;
const uint64_t NSEC100_PER_SEC = (NSEC_PER_SEC / 100);
const uint64_t NSEC100_PER_MSEC = (NSEC_PER_MSEC / 100);
}  // namespace ming

#if defined(_MSC_VER)
#if (_MSC_VER >= 1700)
#define MING_CPLUSPLUS_11_CHRONO 1
#else

#include <windows.h>
namespace ming {
/* get system time in 100-nanosecond intervals since the epoch */
inline uint64_t get_time_ticks(void) {
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  return ((((uint64_t)ft.dwHighDateTime) << 32) + (uint64_t)ft.dwLowDateTime -
          EPOCH);
}

inline void gettimeofday(struct timeval *tp) {
  uint64_t now = ming::get_time_ticks();
  tp->tv_sec = (long)(now / NSEC100_PER_SEC);
  tp->tv_usec = (long)((now % NSEC100_PER_SEC) / 10);
}

}  // namespace ming

#endif

#elif defined(__GNUC__)

#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7))
#define MING_CPLUSPLUS_11_CHRONO 1

#else
// GNU C++
#include <sys/time.h>
namespace ming {
inline void gettimeofday(struct timeval *tp) { ::gettimeofday(tp, NULL); }

// nanoseconds resolution on linux
// #include <time.h>
// struct timespec ts;
// clock_gettime(CLOCK_REALTIME, &ts);

}  // namespace ming
#endif

#else
#error "Support Windows and Linux platform Only!"
#endif

#if defined(MING_CPLUSPLUS_11_CHRONO)
#include <ratio>
#include <chrono>

namespace ming {

/* get system time in microseconds intervals since the epoch */
inline uint64_t microseconds_since_epoch(void) {
  using namespace std::chrono;
  high_resolution_clock::time_point tp = high_resolution_clock::now();
  microseconds usec = duration_cast<microseconds>(tp.time_since_epoch());
  return usec.count();
}

/* get system time in milliseconds intervals since the epoch */
inline uint64_t milliseconds_since_epoch(void) {
  using namespace std::chrono;
  high_resolution_clock::time_point tp = high_resolution_clock::now();
  milliseconds msec = duration_cast<milliseconds>(tp.time_since_epoch());
  return msec.count();
}

/* get system time in seconds intervals since the epoch */
inline uint32_t seconds_since_epoch(void) {
  using namespace std::chrono;
  high_resolution_clock::time_point tp = high_resolution_clock::now();
  seconds sec = duration_cast<seconds>(tp.time_since_epoch());
  return sec.count();
}

}  // namespace ming

#else

namespace ming {
/* get system time in microseconds intervals since the epoch */
inline uint64_t microseconds_since_epoch(void) {
  struct timeval tv;
  ming::gettimeofday(&tv);
  return ((uint64_t)tv.tv_sec) * USEC_PER_SEC + (uint64_t)tv.tv_usec;
}

/* get system time in milliseconds intervals since the epoch */
inline uint64_t milliseconds_since_epoch(void) {
  struct timeval tv;
  ming::gettimeofday(&tv);
  return ((uint64_t)tv.tv_sec) * MSEC_PER_SEC +
         (uint64_t)tv.tv_usec / USEC_PER_MSEC;
}

/* get system time in seconds intervals since the epoch */
inline uint32_t seconds_since_epoch(void) {
  struct timeval tv;
  ming::gettimeofday(&tv);
  return tv.tv_sec;
}

}  // namespace ming

#endif

#endif  // MING_TIME_H_
