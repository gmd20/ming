#include "ming/channel/channel.h"

namespace ming {

class Channel
{
public:
  std::string name;          // name of this channel
  unsigned int id;           // id = hash(name)
  unsigned int thread_id;    // the thread that this channel belongs to

  virtual void Init(std::string setting);

  // other channel use this function to send a message to this channel
  virtual void Write(Message *msg);

private:
  // this channel's service function. If new message was found, this function
  // will be invoked and it is responsible for processing the message and
  // dispatch it to other channel if needed
  virtual void DispatchMessage(Message *msg);

  std::string upstream_channel;
  std::string downstream_channel;

  // once ta channel was created, it will be maintained by the ChannelManager.
  // all threads even the oowner should not destroyed this channel,  unused
  // channels whichref_counter is zero wiil be reclaimed by the ChannelManager
  // at a proper time.
  uint32_t  ref_counter_;
};


class TimerChannel {
public:
};

class FlowControlChannel {
public:
};

class DelayControlChannel {
public:
};

class InprocChannel: public Channel
{
public:
  // Inproc  Channel's Write() is thread-safe and can be invoked from
  // any thread.
  virtual void Write(Message *msg);

private:
  RingBuffer  ring_buffers_;   // to read message from peer thread
};

class IpcChannel: public Channel
{
public:
  virtual void Write(Message *msg);
};

class TcpChannel: public Channel
{
public:
  virtual void Write(Message *msg);
};

class ChannelManager
{
public:
  bool NewChannel(Channel * channel);

private:
  SpinLock                                  lock_;
  std::vector <Channel *>                   channels_;
  std::vector <ThreadLocalChannelManager *> threads_;
};

class ThreadLocalChannelManager
{
public:
  std::string thread_name;
  int thread_id;

  // remote thread informs this thread that it had created a inproc_channel
  // which will be used to write message to local thread
  bool BindInProcChannel(Channel * in_inproc_channel);

private:
  std::vector <Channel *>     in_inproc_channels_;    // inproc channel write message to another thread
  std::vector <Channel *>     out_inproc_channels_;   // inproc channel read message from another thread

  std::vector <Channel *>     local_channels_;
};

} // namespace ming
