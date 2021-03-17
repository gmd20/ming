#ifndef MING_EXEC_H_
#define MING_EXEC_H_

#include <memory>
#include <string>
#include <array>

namespace ming {
inline int exec(const char* cmd, std::string &result) {
  result.resize(0);
  std::array<char, 512> buffer;

  FILE *pipe = popen(cmd, "r");
  if (!pipe) {
    return -1;
  }
  while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
    result += buffer.data();
  }

  return pclose(pipe);
}

inline int exec_parse_line(const char* cmd, void (*handler)(const char*, void *), void *data) {
  char buffer[512];

  FILE *pipe = popen(cmd, "r");
  if (!pipe) {
    return -1;
  }
  while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
    handler(buffer, data);
  }

  return pclose(pipe);
}

} // namespace ming
#endif
