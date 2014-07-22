#ifndef MING_HASH_H_
#define MING_HASH_H_

namespace ming {

class noncopyable
{
protected:
  noncopyable() {}
  ~noncopyable() {}
private:
  noncopyable(const noncopyable&);
  const noncopyable& operator=(const noncopyable&);
};

} // namespace ming

using ming::noncopyable;

#endif // MING_HASH_H_
