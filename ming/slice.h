#ifndef SLICE_H_
#define SLICE_H_

#include <stdint.h>
namespace ming {

// A wrapper to raw bytes array
class Slice {
 public:
  enum { SLICE_EOF = -1 };

  char *begin;
  char *cur;
  char *end;

 public:
  explicit Slice(char *data, int len)
      : begin(data), cur(data), end(data + len) {}
  explicit Slice(char *_begin, char *_end)
      : begin(_begin), cur(_begin), end(_end) {}
  explicit Slice(const Slice &s) : begin(s.begin), cur(s.cur), end(s.end) {}
  // <data, len> is a block of parent slice
  explicit Slice(const Slice &parent, char *data, int len)
      : begin(data), cur(data), end(data + len) {
    if (end > parent.end) end = parent.end;
  }
  int Len() const { return end - begin; }

  int Position() const { return cur - begin; }

  int FreeSpace() const { return end - cur; }

  char *Reserve(int n) {
    if (cur + n <= end) {
      char *pre = cur;
      cur += n;
      return pre;
    } else {
      return NULL;
    }
  }

  char *Consume(int n) {
    if (cur + n <= end) {
      cur += n;
      return cur;
    } else {
      return NULL;
    }
  }

  bool Eof() const { return FreeSpace() <= 0; }

  int PeekChar() const {
    if (!Eof()) {
      return cur[0];
    } else {
      return SLICE_EOF;
    }
  }

  int ReadChar() {
    char *c = Reserve(1);
    if (c != NULL) {
      return *c;
    } else {
      return SLICE_EOF;
    }
  }

  uint8_t ReadUint8(bool &eof_error) {
    char *c = Reserve(sizeof(uint8_t));
    if (c != NULL) {
      return *(uint8_t *)(c);
    } else {
      eof_error = true;
      return 0;
    }
  }

  uint16_t ReadUint16(bool &eof_error) {
    char *c = Reserve(sizeof(uint16_t));
    if (c != NULL) {
      return *(uint16_t *)(c);
    } else {
      eof_error = true;
      return 0;
    }
  }

  uint32_t ReadUint32(bool &eof_error) {
    char *c = Reserve(sizeof(uint32_t));
    if (c != NULL) {
      return *(uint32_t *)(c);
    } else {
      eof_error = true;
      return 0;
    }
  }

  bool WriteUint32(uint32_t v) {
    uint32_t *i = (uint32_t *)cur;
    cur += sizeof(uint32_t);
    if (cur <= end) {
      *i = v;
      return true;
    } else {
      return false;
    }
  }

  bool WriteBytes(char *s, int len) {
    char *b = cur;
    cur += len;
    if (cur <= end) {
      memcpy(b, s, len);
      return true;
    } else {
      return false;
    }
  }
};

}  // namespace ming
#endif  // SLICE_H_
