#ifndef MING_MUTEX_H_
#define MING_MUTEX_H_

#if defined(_MSC_VER)
// Microsoft Visual C++
#include "ming/win/win_mutex.h"

#if (_MSC_VER >= 1700)
// c++ 11 mutex
#include <mutex>
#endif
#elif defined(__GNUC__)
// GNU C++
#include "ming/linux/linux_mutex.h"

#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7))
// c++ 11 mutex
#include <mutex>
#endif
#else
#error "Support Windows and Linux platform Only!"
#endif

#endif  // MING_MUTEX_H_

