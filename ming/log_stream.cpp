#include  "ming/log_stream.h"

#include <limits>
#include <stdio.h>
#include <stdint.h>
#include <algorithm>


namespace ming {

static const char digits[] = "9876543210123456789";
static const char* zero = digits + 9;
static const char digits_hex[] = "0123456789ABCDEF";

template<typename T>
int format_int(char buf[], T value)
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

// Explicit Instantiation
template int format_int(char buf[], char value);
template int format_int(char buf[], unsigned char value);
template int format_int(char buf[], short value);
template int format_int(char buf[], unsigned short value);
template int format_int(char buf[], int value);
template int format_int(char buf[], unsigned int value);
template int format_int(char buf[], long value);
template int format_int(char buf[], unsigned long value);
template int format_int(char buf[], long long  value);
template int format_int(char buf[], unsigned long long value);


int format_pointer_hex(char buf[], void * value)
{
  uintptr_t i = reinterpret_cast<uintptr_t>(value);
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

int format_double(char buf[], double value)
{
  return sprintf(buf, "%.12g", value);
}


} // namespace ming
