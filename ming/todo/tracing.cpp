#include "tracing.h"

#include <string.h>

#include <string>

struct Statistic {
  unsigned int counter;
  std::vector<unsigned int> timer;
};

enum { kDefaultFlushIntervals = 10 };

std::vector<std::string> metrics_name;
std::vector<Statistic> statistics;

unsigned int tracing_metric_id(const char *metric) {
  int len = metrics_name.size();
  for (int i = 0; i < len; i++) {
    if (0 == strcmp(metrics_name[i].c_str(), metric)) {
      return i;
    }
  }
  metrics_name.push_back(metric);
  return metrics_name.size();
}

void tracing_counter_inc(unsigned int metric_id) {}

void tracing_counter_dec(unsigned int metric_id) {}

uint64_t tracing_timer_start() { return walltime::microseconds_since_epoch(); }

void tracing_timer_stop(unsigned int metric_id, uint64_t start_time) {}

//============================================================================

//============================================================================

bool tracing_thread_start() { return true; }

bool tracing_thread_stop() { return true; }

void tracing_init(const char *metric_name_prefix) {}

void tracing_config(bool stat_enabled, bool status_report_enabled,
                    bool span_enabled) {}
