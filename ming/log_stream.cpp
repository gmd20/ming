#include "ming/log_stream.h"

#include <algorithm>
#include <limits>
#include <assert.h>
#include <stdio.h>

namespace ming {

static const int kMaxNumericSize = 32;
static const char digits[] = "9876543210123456789";
static const char* zero = digits + 9;
static const char digits_hex[] = "0123456789ABCDEF";

// Efficient Integer to String Conversions, by Matthew Wilson.
  template<typename T>
size_t convert(char buf[], T value)
{
  T i = value;
  char* p = buf;

  do {
    int lsd = static_cast<int>(i % 10);
    i /= 10;
    *p++ = zero[lsd];
  } while (i != 0);

  if (value < 0) {
    *p++ = '-';
  }
  *p = '\0';
  std::reverse(buf, p);

  return p - buf;
}

size_t convert_hex(char buf[], uintptr_t value)
{
  uintptr_t i = value;
  char* p = buf;

  do {
    int lsd = i % 16;
    i /= 16;
    *p++ = digits_hex[lsd];
  } while (i != 0);

  *p = '\0';
  std::reverse(buf, p);

  return p - buf;
}
template<int SIZE>
template<typename T>
void LogStream<SIZE>::format_integer(T v)
{
  char *buf = buffer_.reserve(kMaxNumericSize);
  if (buf == 0) {return;}

  size_t len = convert(buf, v);
  buffer_.commit(len);
}

template<int SIZE>
LogStream<SIZE>& LogStream<SIZE>::operator<<(short v)
{
  *this << static_cast<int>(v);
  return *this;
}

template<int SIZE>
LogStream<SIZE>& LogStream<SIZE>::operator<<(unsigned short v)
{
  *this << static_cast<unsigned int>(v);
  return *this;
}

template<int SIZE>
LogStream<SIZE>& LogStream<SIZE>::operator<<(int v)
{
  format_integer(v);
  return *this;
}

template<int SIZE>
LogStream<SIZE>& LogStream<SIZE>::operator<<(unsigned int v)
{
  format_integer(v);
  return *this;
}

template<int SIZE>
LogStream<SIZE>& LogStream<SIZE>::operator<<(long v)
{
  format_integer(v);
  return *this;
}

template<int SIZE>
LogStream<SIZE>& LogStream<SIZE>::operator<<(unsigned long v)
{
  format_integer(v);
  return *this;
}

template<int SIZE>
LogStream<SIZE>& LogStream<SIZE>::operator<<(long long v)
{
  format_integer(v);
  return *this;
}

template<int SIZE>
LogStream<SIZE>& LogStream<SIZE>::operator<<(unsigned long long v)
{
  format_integer(v);
  return *this;
}

template<int SIZE>
LogStream<SIZE>& LogStream<SIZE>::operator<<(const void* p)
{
  uintptr_t v = reinterpret_cast<uintptr_t>(p);
  char *buf = buffer_.reserve(kMaxNumericSize);
  if (buf == 0) {return *this;}

  buf[0] = '0';
  buf[1] = 'x';
  size_t len = convert_hex(buf+2, v);
  buffer_.commit(len+2);
  return *this;
}

// FIXME: replace this with Grisu3 by Florian Loitsch.
template<int SIZE>
LogStream<SIZE>& LogStream<SIZE>::operator<<(double v)
{
  if (buffer_.avail() >= kMaxNumericSize) {
    int len = _snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
    buffer_.add(len);
  }
  return *this;
}





} // namespace ming
