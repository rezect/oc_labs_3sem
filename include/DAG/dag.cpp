#include "dag.hpp"
#include <queue>

DAG::DAG(const std::vector<Job> &jobs_list,
         const std::unordered_map<std::string, int> &semaphores_list) {
  for (const auto &job : jobs_list) {
    jobs.insert(std::make_pair(job.name, job));
    in_degree[job.name] = job.depends_on.size();
  }

  semaphores = semaphores_list;
}

bool DAG::has_cycle() const {
  std::queue<std::string> ready;
  std::unordered_map<std::string, int> local_in_degree = in_degree;

  for (const auto& [name, degree] : local_in_degree) {
    if (degree == 0) {
      ready.push(name);
    }
  }

  size_t jobs_counter = 0;

  while (!ready.empty()) {
    std::string current = ready.front();
    ready.pop();

    ++jobs_counter;

    for (const auto& dependency : jobs.at(current).depends_on) {
      --local_in_degree[dependency];
      if (local_in_degree[dependency] == 0) {
        ready.push(dependency);
      }
    }
  }

  return jobs_counter != jobs.size();
}

void DAG::execute(int16_t max_threads) const {
  
}
