#include <string>
#include <vector>

struct Job {
  std::string name;
  std::vector<std::string> depends_on;
  int16_t sleep_time;
  std::string semaphore;
};
