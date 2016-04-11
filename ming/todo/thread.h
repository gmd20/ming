#ifndef MING_THREAD_H_
#define MING_THREAD_H_

#include <thread>

namespace ming {

class Thread {
 public:
  template <class Fn, class... Args>
  explicit Thread(Fn&& fn, Args&&... args);
  ~Thread();
  void Exit();

 public:
  boost::asio::io_service io_service_;

 private:
  std::thread thread_;
  std::string name_;
};

}  // namespace ming

#endif  // MING_THREAD_H_

