#ifndef SLICE_H_
#define SLICE_H_

#include <stdint.h>
namespace ming {

// A wrapper to raw bytes array
class Slice
{
public:
  enum {
    SLICE_EOF = -1
  };

  char *begin;
  char *cur;
  char *end;

public:
  explicit Slice(char *data, int len):begin(data), cur(data),end(data+len) { }

  int Len() const
  {
    return end - begin;
  }

  int Position() const
  {
    return cur - begin;
  }

  int FreeSpace() const
  {
    return end - cur;
  }

  char * Reserve(int n)
  {
    if (cur + n <= end) {
      char * pre = cur;
      cur += n;
      return pre;
    } else {
      return NULL;
    }
  }

  char * Consume(int n)
  {
    if (cur + n <= end) {
      cur += n;
      return cur;
    } else {
      return NULL;
    }
  }

  bool Eof() const
  {
    return FreeSpace() <= 0;
  }

  int PeekChar() const
  {
    if (!Eof()) {
      return cur[0];
    } else {
      return SLICE_EOF;
    }
  }

  int ReadChar()
  {
    char *c = Reserve(1);
    if (c != NULL) {
      return *c;
    } else {
      return SLICE_EOF;
    }
  }

  uint8_t ReadUint8(bool &eof_error)
  {
    char *c = Reserve(sizeof(uint8_t));
    if (c != NULL) {
      return *(uint8_t *)(c);
    } else {
      eof_error = true;
      return 0;
    }
  }

  uint16_t ReadUint16(bool &eof_error)
  {
    char *c = Reserve(sizeof(uint16_t));
    if (c != NULL) {
      return *(uint16_t *)(c);
    } else {
      eof_error = true;
      return 0;
    }
  }

  uint32_t ReadUint32(bool &eof_error)
  {
    char *c = Reserve(sizeof(uint32_t));
    if (c != NULL) {
      return *(uint32_t *)(c);
    } else {
      eof_error = true;
      return 0;
    }
  }

};

} // namespace ming
#endif // SLICE_H_
