
int format_small_int_padding(char *s, uint32_t i)
{
  char *begin = s;
  if (i >= 1000) {
    *s++ = '0' + i / 1000;
    i = i % 1000;
  }

  *s++ = '0' + i / 100;
  i = i % 100;
  *s++ = '0' +  i / 10;
  i = i % 10;
  *s++ = '0' + i;

  *s = '\0';
  return s - begin;
}

void string_append_timestamp(std::string &s, uint64_t epoch_millisecond)
{
  uint64_t last_time;
  static char timestamp[64]; // "2017-12-25 12:01:02.003"

  uint32_t sec = (uint32_t)(epoch_millisecond / MSEC_PER_SEC);
  uint32_t millisecond = (uint32_t)(epoch_millisecond % MSEC_PER_SEC);

  struct tm * timeinfo;
  timeinfo = localtime(&sec);

  if (last_time != t) {
    last_t = t;
    format_small_int_padding(timestamp, timeinfo->tm_year + 1900);
    timestamp[4] = '-';
    format_small_int_padding(&timestamp[5], timeinfo->tm_mon + 1);
    timestamp[7] = '-';
    format_small_int_padding(&timestamp[8], timeinfo->tm_mday);
    timestamp[10] = ' ';
    format_small_int_padding(&timestamp[11], timeinfo->tm_hour);
    timestamp[13] = ':';
    format_small_int_padding(&timestamp[14], timeinfo->tm_min);
    timestamp[16] = ':';
    format_small_int_padding(&timestamp[17], timeinfo->tm_sec);
    timestamp[19] = '.';
  }
  format_small_int_padding(&timestamp[20], millisecond);
  s.append(timestamp, 23);
}
