#ifndef MING_ATOMIC_H_
#define MING_ATOMIC_H_


// NOTE:
// Use the atomic_thread_fence and std::atomic<T> in c++ 11 instead of the
// following atomic_* functions if possiable.
//
// Acquire semantics means that the result of the operation are made visible
// to all threads and processors before any subsequent memory reads anf
// writes. Acquire is useful when entering a critical section. Release
// semantics means that all memory reads and writes are forced to be made
// visible to all threads and processors before the result of the operation
// is made visible itself. Release is useful when leaving a critical section.
//
// All the atomic_inc* atomic_dec*  atomic_add* atomic_cas* functions
// behave as full read-write memory barriers. That is, no memory operand will
// be moved across the operation, either forward or backward. Further,
// instructions will be issued as necessary to prevent the processor from
// speculating loads across the operation and from queuing stores after
// the operation.
//
// The variable pointed to by the lpAddend parameter must be aligned on a 32-bit
// boundary; otherwise, this function fails on multiprocessor x86 systems and
// any non-x86 systems.


#if _MSC_VER < 1600
typedef int int32_t;
typedef long long int64_t;
#else
#include <stdint.h>
#endif

#if defined(_MSC_VER)
  // Microsoft Visual C++

  #if (_MSC_VER >= 1700)
  // use c ++ 11 std::atomic if possible
  #include <atomic>
  #endif

  // To declare an interlocked function for use as an intrinsic,
  // include intrin.h and put the function in a #pragma intrinsic
  // statement.
  #include <intrin.h>
  #pragma intrinsic (_InterlockedIncrement)
  #pragma intrinsic (_InterlockedDecrement)
  #pragma intrinsic (_InterlockedAdd)
  #pragma intrinsic (_InterlockedCompareExchange)
  #pragma intrinsic (_InterlockedIncrement64)
  #pragma intrinsic (_InterlockedDecrement64)
  #pragma intrinsic (_InterlockedAdd64)
  #pragma intrinsic (_InterlockedCompareExchange64)

namespace ming {
  // The return value is the resulting incremented value.
  inline int32_t atomic_inc32(int32_t volatile * addend) {
    return _InterlockedIncrement((volatile long*) addend);
  }
  // The return value is the resulting incremented value.
  inline int32_t atomic_dec32(int32_t volatile * addend) {
    return _InterlockedDecrement((volatile long*)addend);
  }
  // The return value is the result of the addition.
  inline int32_t atomic_add32(int32_t volatile * addend, int32_t value) {
    return _InterlockedAdd((volatile long*)addend, value);
  }
  // The return value is the initial value of the Destination pointer.
  inline int32_t atomic_cas32(int32_t volatile *destination, int32_t exchange, int32_t comparand) {
    return _InterlockedCompareExchange((volatile long*)destination,exchange,comparand);
  }
  // The return value is the resulting incremented value.
  inline int64_t atomic_inc64(int64_t volatile * addend) {
    return _InterlockedIncrement64(addend);
  }
  // The return value is the resulting incremented value.
  inline int64_t atomic_dec64(int64_t volatile * addend) {
    return _InterlockedDecrement64(addend);
  }
  // The return value is the result of the addition.
  inline int64_t atomic_add64(int64_t volatile * addend, int64_t value) {
    return _InterlockedAdd64(addend, value);
  }
  // The return value is the initial value of the Destination pointer.
  inline int64_t atomic_cas32(int64_t volatile *destination, int64_t exchange, int64_t comparand) {
    return _InterlockedCompareExchange64(destination,exchange,comparand);
  }
} // namespace ming

#elif defined(__GNUC__)
  // GNU C++

  #if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7))
  // use c ++ 11 std::atomic if possible
  #include <atomic>
  #endif

namespace ming {
  // The return value is the resulting incremented value.
  inline int32_t atomic_inc32(int32_t volatile * addend) {
    return __sync_add_and_fetch(addend, 1);
  }
  // The return value is the resulting incremented value.
  inline int32_t atomic_dec32(int32_t volatile * addend) {
    return __sync_sub_and_fetch(addend, 1);
  }
  // The return value is the result of the addition.
  inline int32_t atomic_add32(int32_t volatile * addend, int32_t value) {
    return __sync_add_and_fetch(addend, value);
  }
  // The return value is the initial value of the Destination pointer.
  inline int32_t atomic_cas32(int32_t volatile * destination, int32_t exchange, int32_t comparand) {
    return __sync_val_compare_and_swap(destination,exchange,comparand);
  }
  // The return value is the resulting incremented value.
  inline int64_t atomic_inc64(int64_t volatile * addend) {
    return __sync_add_and_fetch(addend, 1);
  }
  // The return value is the resulting incremented value.
  inline int64_t atomic_dec64(int64_t volatile * addend) {
    return __sync_sub_and_fetch(addend, 1);
  }
  // The return value is the result of the addition.
  inline int64_t atomic_add64(int64_t volatile * addend, int64_t value) {
    return __sync_add_and_fetch(addend, value);
  }
  // The return value is the initial value of the Destination pointer.
  inline int64_t atomic_cas32(int64_t volatile * destination, int64_t exchange, int64_t comparand) {
    return __sync_val_compare_and_swap(destination,exchange,comparand);
  }
} // namespace ming
#else
  #error "Support Windows and Linux platform Only!"
#endif

#endif // MING_ATOMIC_H_
