#include "../job/job.hpp"
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <atomic>
#include <mutex>
#include <thread>

class DAG {
private:
  std::unordered_map<std::string, Job> jobs;
  std::unordered_map<std::string, int> semaphores;
  std::unordered_map<std::string, int> in_degree;

  std::mutex jobs_mutex;

public:
  DAG(const std::vector<Job> &, const std::unordered_map<std::string, int> &);

  bool has_cycle() const;

  void execute(int);

  void print() {
    for (const auto &[name, job] : jobs) {
      std::cout << name << ": ";
      for (const auto &depend : job.depends_on) {
        std::cout << depend << " ";
      }
      std::cout << std::endl;
    }
  }
};