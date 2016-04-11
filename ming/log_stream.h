#ifndef MING_LOGSTREAM_H_
#define MING_LOGSTREAM_H_

#include <string.h>
#include <string>

namespace ming {

template <int SIZE>
class FixedBuffer {
 public:
  FixedBuffer() : overflow_(false), cur_(data_) {}
  ~FixedBuffer() {}

  void append(const char* buf, int len) {
    if (avail() > len) {
      memcpy(cur_, buf, len);
      cur_ += len;
      return;
    }
    overflow_ = true;
  }

  const char* c_str() const {
    *cur_ = '\0';
    return data_;
  }
  const char* data() const { return data_; }
  int length() const { return static_cast<int>(cur_ - data_); }
  char* current() { return cur_; }
  int avail() const { return static_cast<int>(end() - cur_); }
  char* reserve(int len) {
    if (avail() > len) {
      return cur_;
    } else {
      overflow_ = true;
      return 0;
    }
  }
  void commit(int len) { cur_ += len; }
  void reset() {
    cur_ = data_;
    overflow_ = false;
  }
  bool overflow() { return overflow_; }

 private:
  const char* end() const { return data_ + sizeof(data_); }

 private:
  bool overflow_;
  char* cur_;
  char data_[SIZE];
};

template <typename T>
int format_int(char buf[], T value);
int format_pointer_hex(char buf[], void* value);
int format_double(char buf[], double value);

const int kMaxNumericSize = 32;
const int kLogStreamDefaultBufferSize = 1024 * 4;
template <int SIZE = kLogStreamDefaultBufferSize>
class LogStream {
 public:
  typedef FixedBuffer<SIZE> Buffer;
  LogStream() {}

  const char* c_str() const { return buffer_.c_str(); }
  const char* data() const { return buffer_.data(); }
  int length() const { return buffer_.length(); }
  char* reserve(int len) { return buffer_.reserve(len); }
  void commit(int len) { buffer_.commit(len); }
  void append(const char* data, int len) { buffer_.append(data, len); }
  Buffer& buffer() { return buffer_; }
  void reset() { buffer_.reset(); }
  bool overflow() { return buffer_.overflow(); }

  LogStream& operator<<(bool v) {
    if (v) {
      buffer_.append("true", 4);
    } else {
      buffer_.append("false", 5);
    }
    return *this;
  }

  LogStream& operator<<(char v) {
    buffer_.append(&v, 1);
    return *this;
  }
  LogStream& operator<<(const char* v) {
    buffer_.append(v, strlen(v));
    return *this;
  }
  LogStream& operator<<(const std::string& str) {
    buffer_.append(str.data(), str.length());
    return *this;
  }

  LogStream& operator<<(short v) {
    *this << static_cast<int>(v);
    return *this;
  }

  LogStream& operator<<(unsigned short v) {
    *this << static_cast<unsigned int>(v);
    return *this;
  }

  LogStream& operator<<(int v) {
    format_integer(v);
    return *this;
  }

  LogStream& operator<<(unsigned int v) {
    format_integer(v);
    return *this;
  }

  LogStream& operator<<(long v) {
    format_integer(v);
    return *this;
  }

  LogStream& operator<<(unsigned long v) {
    format_integer(v);
    return *this;
  }

  LogStream& operator<<(long long v) {
    format_integer(v);
    return *this;
  }

  LogStream& operator<<(unsigned long long v) {
    format_integer(v);
    return *this;
  }

  LogStream& operator<<(const void* p) {
    char* buf = buffer_.reserve(kMaxNumericSize);
    if (buf == 0) {
      return *this;
    }

    buf[0] = '0';
    buf[1] = 'x';
    int len = format_pointer_hex(buf + 2, p);
    buffer_.commit(len + 2);
    return *this;
  }

  LogStream& operator<<(double v) {
    char* buf = buffer_.reserve(kMaxNumericSize);
    if (buf == 0) {
      return;
    }

    int len = format_double(buf, v);
    buffer_.commit(len);
    return *this;
  }

  LogStream& operator<<(float v) {
    *this << static_cast<double>(v);
    return *this;
  }

 private:
  template <typename T>
  void format_integer(T v) {
    char* buf = buffer_.reserve(kMaxNumericSize);
    if (buf == 0) {
      return;
    }

    int len = format_int(buf, v);
    buffer_.commit(len);
  }

 private:
  Buffer buffer_;
};

#define LOGSTREAM_APPEND_CONST_STRING(stream, const_str) \
  {                                                      \
    const char* warn_if_not_a_const_str = const_str "";  \
    warn_if_not_a_const_str = warn_if_not_a_const_str;   \
    stream.append((const_str), sizeof(const_str) - 1);   \
  }

}  // namespace ming

#endif  // MING_LOGSTREAM_H_
