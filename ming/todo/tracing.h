#ifndef TRACING_H_
#define TRACING_H_

#include "walltime.h"
#include <vector>

void tracing_init(const char *metric_name_prefix);
void tracing_config(bool stat_enabled, bool status_report_enabled,
                    bool span_enabled);

//------------------------------------------------------------------------------
// internal functions
unsigned int tracing_metric_id(const char *metric);
void tracing_counter_inc(unsigned int metric_id);
void tracing_counter_dec(unsigned int metric_id);
uint64_t tracing_timer_start();
void tracing_timer_stop(unsigned int metric_id, uint64_t start_time);

inline bool tracing_is_stat_enabled();
inline bool tracing_is_status_report_enabled();
inline bool tracing_is_span_enabled();

//------------------------------------------------------------------------------
// statistics macros
#define TRACING_COUNTER_INC(metric)                            \
  {                                                            \
    static unsigned int metric_id = tracing_metric_id(metric); \
    tracing_counter_inc(metric_id);                            \
  }

#define TRACING_COUNTER_DEC(metric)                            \
  {                                                            \
    static unsigned int metric_id = tracing_metric_id(metric); \
    tracing_counter_dec(metric_id);                            \
  }

// return current time
#define TRACING_TIMER_START(start_time) \
  { start_time = tracing_timer_start(); }

#define TRACING_TIMER_STOP(metric, start_time)                 \
  {                                                            \
    static unsigned int metric_id = tracing_metric_id(metric); \
    tracing_timer_stop(metric_id, start_time);                 \
  }

//------------------------------------------------------------------------------
// status report macro
#define TRACING_STATUS_REPORT(name, value)

//------------------------------------------------------------------------------
// span macro

union Span {
  struct {
    uint32_t
        span_name;  // span name index in the span_name_list. -1 for SPAN_STOP
    uint32_t
        timestamp;  // Event-triggered time, offset to Spans's create_timestamp
  } base;
  struct {
    uint32_t span_id;
    uint32_t parent_id;
  } id;  // following id record if it is SPAN_START
};

struct Spans {
  bool sampling;  // enable statistic on this spans if sampling is true
  uint64_t create_timestamp;
  uint32_t num_items;
  std::vector<Span> span;
};

#define TRACING_SPANS_ALLOC(spans)
#define TRACING_SPAN_START(spans, span_name, span_id, parent_id)
#define TRACING_SPAN_STOP(spans, span_name)
#define TRACING_SPANS_FREE(spans, discard)

#endif  // TRACING_H_
