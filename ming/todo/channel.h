#ifndef MING_CHANNEL_H_
#define MING_CHANNEL_H_

namespace ming {

struct Message {     // 可以改用 FlatBuffers 类型
  uint32_t size;
  uint32_t dest_channel_id;
  uint32_t src_channel_id;
  uint32_t id;
  uint64_t seq_no;
};

typedef void (*)(Message *msg) DispatchMessageFunc;




bool NewInprocChannel(std::string name, DispatchMessageFunc func);
bool NewIpcClientChannel(std::string name, DispatchMessageFunc func);
bool NewIpcServerChannel(std::string name, DispatchMessageFunc func);
bool NewTcpClientChannel(std::string name, DispatchMessageFunc func);
bool NewTcpServerChannel(std::string name, DispatchMessageFunc func);
bool NewCustomChannel(std::string name, DispatchMessageFunc func, Channel *custom);

// if a Inproc channel's DispatchMessageFunc receive a message which
// dest_channel_id is not current channel, it should invoke this function
// to forward the message to the target channel(expected to be in the same thread)
bool ForwardMessage(Message *msg);

bool SendMessage(std::string to_channel, Message* msg) {}


// the thread and channel should be dynamicly created and adjusted in runtime.

} // namespace ming

#endif // MING_CHANNEL_H_
