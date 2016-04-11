#ifndef ASIO_BUFFER_ARRAY_H_
#define ASIO_BUFFER_ARRAY_H_

#include <vector>
#include <boost/asio.hpp>

namespace ming {

// a wrapper class of std::vector to avoid memory copy in asio handler
class AsioBufferArray {
 public:
  typedef boost::asio::const_buffer value_type;
  typedef std::vector<boost::asio::const_buffer>::const_iterator const_iterator;

  AsioBufferArray(std::vector<boost::asio::const_buffer> &a) : array_(a) {}
  AsioBufferArray(const AsioBufferArray &a) : array_(a.array_) {}
  ~AsioBufferArray() {
    // the lifecycle of array_ was maintained by our owner.
    // nothing to do here
  }

  const_iterator begin() const { return array_.begin(); }

  const_iterator end() const { return array_.end(); }

 private:
  std::vector<boost::asio::const_buffer> &array_;
};

}  // namespace ming

#endif  // ASIO_BUFFER_ARRAY_H_
