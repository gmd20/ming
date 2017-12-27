// strftime alternative
void string_append_timestamp(std::string &s, uint64_t epoch_millisecond)
{
  static uint64_t last_time = 0;
  static char timestamp[32]; // "2017-12-25 12:01:02.003"

  uint32_t sec = (uint32_t)(epoch_millisecond / MSEC_PER_SEC);
  uint32_t millisecond = (uint32_t)(epoch_millisecond % MSEC_PER_SEC);
  int i;

  if (last_time != sec) {
    last_time = sec;
    struct tm * timeinfo;
    time_t t = (time_t)sec;
    timeinfo = localtime(&t);

    i =  timeinfo->tm_year + 1900;
    timestamp[0] = '0' + i / 1000;
    i = i % 1000;
    timestamp[1] = '0' + i / 100;
    i = i % 100;
    timestamp[2] = '0' + i / 10;
    timestamp[3] = '0' + i % 10;
    timestamp[4] = '-';
    i = timeinfo->tm_mon + 1;
    timestamp[5] = '0' + i / 10;
    timestamp[6] = '0' + i % 10;
    timestamp[7] = '-';
    i = timeinfo->tm_mday;
    timestamp[8] = '0' + i / 10;
    timestamp[9] = '0' + i % 10;
    timestamp[10] = ' ';
    i = timeinfo->tm_hour;
    timestamp[11] = '0' + i / 10;
    timestamp[12] = '0' + i % 10;
    timestamp[13] = ':';
    i = timeinfo->tm_min;
    timestamp[14] = '0' + i / 10;
    timestamp[15] = '0' + i % 10;
    timestamp[16] = ':';
    i = timeinfo->tm_sec;
    timestamp[17] = '0' + i / 10;
    timestamp[18] = '0' + i % 10;
    timestamp[19] = '.';
  }
  i = (int) millisecond;
  timestamp[20] = '0' + i / 100;
  i = i % 100;
  timestamp[21] = '0' + i / 10;
  timestamp[22] = '0' + i % 10;
  s.append(timestamp, 23);
}
