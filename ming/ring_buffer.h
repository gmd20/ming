#ifndef MING_RING_BUFFER_H_
#define MING_RING_BUFFER_H_

#include <atomic>

#ifdef __GNUC__
#include <unistd.h>
namespace ming {
inline void sched_yield(int k) {
  if (k < 256) {
    for (int i = 0; i < k; i++) {
      asm volatile("pause");
      // __asm__ (".byte 0xf3, 0x90")
    }
  } else if (k < 1024) {
    usleep(0);
  } else {
    usleep(500);
  }
}
}  // namespace ming
#elif defined(_MSC_VER)
#include <windows.h>
namespace ming {
inline void sched_yield(int k) {
  if (k < 256) {
    for (int i = 0; i < k; i++) {
      ::YieldProcessor();
      // _mm_pause();
      // __asm  _emit 0xf3;
      // __asm  _emit 0x90;
    }
  } else if (k < 1024) {
    Sleep(0);
  } else {
    Sleep(1);
  }
}
}  // namespace ming
#endif

#include "ming/likely.h"
#include "ming/noncopyable.h"

namespace ming {

// a single producer and single consumer lock-free queue
// T should has a default constructor
// Size should be defined as a power of two (2^n)
template <typename T, unsigned int Size>
class RingBuffer : private noncopyable {
 public:
  RingBuffer() : tail_(0), head_(0) {
    for (int i = 0; i < Size; i++) {
      array_[i].~T();
    }
  }
  virtual ~RingBuffer() {}

  template <class... Args>
  bool Push(Args&&... args) {
    auto const tail = tail_.load(std::memory_order_relaxed);
    auto next_tail = tail + 1;
    if (next_tail == Size) {
      next_tail = 0;
    }
    if (LIKELY(next_tail != head_.load(std::memory_order_acquire))) {
      new (&array_[tail]) T(std::forward<Args>(args)...);
      tail_.store(next_tail, std::memory_order_release);
      return true;
    }
    // queue is full
    return false;
  }

  template <class... Args>
  void BlockPush(Args&&... args) {
    int k = 1;
    while (UNLIKELY(Push(std::forward<Args>(args)...) == false)) {
      k <<= 1;
      sched_yield(k);  // Exponential backoff
    }
  }

  bool Pop(T& item) {
    auto const head = head_.load(std::memory_order_relaxed);
    if (LIKELY(head != tail_.load(std::memory_order_acquire))) {
      auto next_head = head + 1;
      if (next_head == Size) {
        next_head = 0;
      }
      item = std::move(array_[head]);
      array_[head].~T();
      head_.store(next_head, std::memory_order_release);
      return true;
    }
    // queue is empty
    return false;
  }

  void BlockPop(T& item) {
    int k = 1;
    while (UNLIKELY(Pop(item) == false)) {
      k <<= 1;
      sched_yield(k);  // Exponential backoff
    }
  }

  // pointer to the value at the front of the queue (for use in-place) or
  // nullptr if empty.
  T* FrontPtr() {
    auto const head = head_.load(std::memory_order_relaxed);
    if (LIKELY(head != tail_.load(std::memory_order_acquire))) {
      return &array_[head];
    }
    // queue is empty
    return nullptr;
  }

  // queue must not be empty
  void PopFront() {
    auto const head = head_.load(std::memory_order_relaxed);
    // assert(head != tail_.load(std::memory_order_acquire));

    auto next_head = next_head + 1;
    if (next_head == Size) {
      next_head = 0;
    }
    array_[head].~T();
    head_.store(next_head, std::memory_order_release);
  }

 private:
// to avoid false sharing problem,  padding should be added to
// make sure each fields are in different CACHE_LINE.
// const int CACHE_LINE_SIZE = 64;
// char padding[CACHE_LINE_SIZE - sizeof(T)];;
// so the following data is aligned.
#ifdef __GNUC__
#define CACHE_LINE_ALIGN(x) x __attribute__((aligned(64)));
#elif defined(_MSC_VER)
#define CACHE_LINE_ALIGN(x) __declspec(align(64)) x;
#endif
  CACHE_LINE_ALIGN(std::atomic<int> tail_);
  CACHE_LINE_ALIGN(std::atomic<int> head_);
  CACHE_LINE_ALIGN(T array_[Size]);
#undef CACHE_LINE_ALIGN
};

}  // namespace ming

#endif  // MING_RING_BUFFER_H_
