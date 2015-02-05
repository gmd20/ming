#ifndef MING_LOGSTREAM_H_
#define MING_LOGSTREAM_H_

#include <string.h>

namespace ming {

template<int SIZE>
class FixedBuffer
{
public:
  FixedBuffer():overflow_(false),cur_(data_)
  {
  }
  ~FixedBuffer()
  {
  }

  void append(const char* buf, size_t len)
  {
    if (static_cast<size_t>(avail()) > len) {
      memcpy(cur_, buf, len);
      cur_ += len;
      return;
    }
    overflow_ = true;
  }

  const char* data() const { return data_; }
  int length() const { return static_cast<int>(cur_ - data_); }
  char*current() { return cur_; }
  int avail() const { return static_cast<int>(end() - cur_); }
  char* reserve(int size) const {
    if (avail() >= size) {
      return cur_;
    } else {
      overflow_ = true;
      return 0;
    }
  }
  void commit(size_t len) { cur_ += len; }
  void reset() { cur_ = data_; overflow_ = false; }
  bool overflow() {return overflow_; }

private:
  const char* end() const { return data_ + sizeof(data_); }

private:
  bool overflow_;
  char* cur_;
  char data_[SIZE];
};

const int kLogStreamDefaultBufferSize = 1024 * 4;
template<int SIZE = kLogStreamDefaultBufferSize>
class LogStream
{
public:
  typedef FixedBuffer<SIZE> Buffer;
  LogStream() { }

  void append(const char* data, int len) { buffer_.append(data, len); }
  Buffer& buffer() { return buffer_; }
  void reset() { buffer_.reset();}
  bool overflow() { return buffer_.overflow(); }

  LogStream & operator<< (bool v)
  {
    if (v){
      buffer_.append("true" ,4);
    } else {
      buffer_.append("false",5);
    }
    return *this;
  }

  LogStream & operator<< (char v)
  {
    buffer_.append(&v, 1);
    return *this;
  }
  LogStream & operator<< (const char* v)
  {
    buffer_.append(v, strlen(v));
    return *this;
  }

  LogStream & operator<< (short);
  LogStream & operator<< (unsigned short);
  LogStream & operator<< (int);
  LogStream & operator<< (unsigned int);
  LogStream & operator<< (long);
  LogStream & operator<< (unsigned long);
  LogStream & operator<< (long long);
  LogStream & operator<< (unsigned long long);
  LogStream & operator<< (const void*);

  LogStream & operator<< (float v)
  {
    *this << static_cast<double>(v);
    return *this;
  }
  LogStream & operator<< (double);

private:
  template<typename T>
    void format_integer(T);

private:
  Buffer buffer_;
};

}  // namcespace ming
#endif   // MING_LOGSTREAM_H_
