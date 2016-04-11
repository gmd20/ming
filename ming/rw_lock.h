#ifndef MING_RW_LOCK_H_
#define MING_RW_LOCK_H_

#if defined(_MSC_VER)
// Microsoft Visual C++
#if (_MSC_VER >= 1700)
// c++ 11 RWSpinLock from folly
#include "ming/folly/RWSpinLock.h"
#endif
#elif defined(__GNUC__)
// GNU C++
#include "ming/linux/linux_rw_lock.h"
#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7))
// c++ 11 RWSpinLock from folly
#include "ming/folly/RWSpinLock.h"
#endif
#else
#error "Support Windows and Linux platform Only!"
#endif

#endif  // MING_RW_LOCK_H_
