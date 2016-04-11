#ifndef FLOW_CONTRL_
#define FLOW_CONTRL_
#include "ming/time.h"

namespace ming {

class FlowControl {
 public:
  FlowControl()
      : limit_per_sec_(0),
        counter_per_sec_(0),
        last_sec_(0),
        limit_per_min_(0),
        counter_per_min_(0),
        last_min_(0),
        limit_per_hour_(0),
        counter_per_hour_(0),
        last_hour_(0),
        limit_per_day_(0),
        counter_per_day_(0),
        last_day_(0) {}
  void Init(unsigned int limit_per_sec, unsigned int limit_per_min,
            unsigned int limit_per_hour, unsigned int limit_per_day) {
    limit_per_sec_ = limit_per_sec;
    limit_per_min_ = limit_per_min;
    limit_per_hour_ = limit_per_hour;
    limit_per_day_ = limit_per_day;
  }

  // return 0 if a packet was allowed
  int Allow() {
    unsigned int seconds_since_epoch = ming::seconds_since_epoch();
    // sec
    if (limit_per_sec_ != 0) {  // no limit
      if (seconds_since_epoch > last_sec_) {
        last_sec_ = seconds_since_epoch;
        counter_per_sec_ = 0;
      }
      if (++counter_per_sec_ > limit_per_sec_) {
        return 1;
      }
    }

    // min
    if (limit_per_min_ != 0) {  // no limit
      if (seconds_since_epoch > (last_min_ + kSecsPerMin)) {
        last_min_ = seconds_since_epoch;
        counter_per_min_ = 0;
      }
      if (++counter_per_min_ > limit_per_min_) {
        return 2;
      }
    }

    // hour
    if (limit_per_hour_ != 0) {  // no limit
      if (seconds_since_epoch > (last_hour_ + kSecsPerHour)) {
        last_hour_ = seconds_since_epoch;
        counter_per_hour_ = 0;
      }
      if (++counter_per_hour_ > limit_per_hour_) {
        return 3;
      }
    }

    // day
    if (limit_per_day_ != 0) {  // no limit
      if (seconds_since_epoch > (last_day_ + kSecsPerDay)) {
        last_day_ = seconds_since_epoch;
        counter_per_day_ = 0;
      }
      if (++counter_per_day_ > limit_per_day_) {
        return 4;
      }
    }

    return 0;
  }

 private:
  enum { kSecsPerMin = 60, kSecsPerHour = 60 * 60, kSecsPerDay = 3600 * 24 };
  unsigned int limit_per_sec_;
  unsigned int counter_per_sec_;
  unsigned int last_sec_;

  unsigned int limit_per_min_;
  unsigned int counter_per_min_;
  unsigned int last_min_;

  unsigned int limit_per_hour_;
  unsigned int counter_per_hour_;
  unsigned int last_hour_;

  unsigned int limit_per_day_;
  unsigned int counter_per_day_;
  unsigned int last_day_;
};

}  // namespace ming

#endif  // FLOW_CONTRL_
