#include "../job/job.hpp"
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <iostream>

class DAG {
private:
  std::unordered_map<std::string, Job> jobs;
  std::unordered_map<std::string, int> semaphores;
  std::unordered_map<std::string, int> in_degree;

public:
  DAG(const std::vector<Job>& jobs_list, const std::unordered_map<std::string, int>& semaphores_list);

  bool has_cycle() const;

  void execute(int16_t max_threads) const;

  void print() {
    for (const auto& [name, job] : jobs) {
      std::cout << name << ": ";
      for (const auto& depend : job.depends_on) {
        std::cout << depend << " ";
      }
      std::cout << std::endl;
    }
  }
};