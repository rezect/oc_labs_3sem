#include "dag.hpp"
#include <algorithm>
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

  // Добавляем все задачи с нулевой степенью входа в очередь
  for (const auto &[name, degree] : local_in_degree) {
    if (degree == 0) {
      ready.push(name);
    }
  }

  size_t processed_jobs = 0;

  while (!ready.empty()) {
    std::string current = ready.front();
    ready.pop();

    ++processed_jobs;

    // Для каждой задачи, зависящей от текущей, уменьшаем in_degree
    for (const auto &[name, job] : jobs) {
      if (std::find(job.depends_on.begin(), job.depends_on.end(), current) !=
          job.depends_on.end()) {
        --local_in_degree[name];
        if (local_in_degree[name] == 0) {
          ready.push(name);
        }
      }
    }
  }

  // Если количество обработанных задач не равно общему числу задач, есть цикл
  return processed_jobs != jobs.size();
}

void DAG::execute(int max_threads) {
  if (this->has_cycle()) {
    throw std::logic_error("DAG has cycle");
  }

  std::queue<std::string> ready;
  std::unordered_map<std::string, int> local_in_degree = in_degree;
  std::mutex semaphores_mutex;
  std::atomic<bool> execution_failed = false;

  // Заполняем очередь стартовыми задачами
  for (const auto &[name, degree] : local_in_degree) {
    if (degree == 0) {
      ready.push(name);
    }
  }

  auto worker = [&](const std::string &job_name) {
    try {
      // Эмуляция выполнения задачи с учётом времени сна
      std::this_thread::sleep_for(
          std::chrono::milliseconds(jobs[job_name].sleep_time));
      std::cout << "Executing job: " << job_name << std::endl;

      // Уменьшаем счетчики зависимостей у всех связанных задач
      {
        std::lock_guard<std::mutex> lock(jobs_mutex);
        for (const auto &dependency : jobs[job_name].depends_on) {
          --local_in_degree[dependency];
          if (local_in_degree[dependency] == 0) {
            ready.push(dependency);
          }
        }
      }

    } catch (const std::exception &e) {
      execution_failed = true;
      std::cerr << "Execution of job " << job_name << " failed: " << e.what()
                << std::endl;
    }
  };

  // Основной цикл выполнения DAG
  std::vector<std::thread> active_threads;

  while (!ready.empty() || !active_threads.empty()) {
    while (!ready.empty() && active_threads.size() < max_threads &&
           !execution_failed) {
      std::string job_name;
      {
        std::lock_guard<std::mutex> lock(jobs_mutex);
        job_name = ready.front();
        ready.pop();
      }

      // Проверяем семафоры перед запуском задачи
      if (!jobs[job_name].semaphore.empty()) {
        std::lock_guard<std::mutex> lock(semaphores_mutex);
        if (semaphores[jobs[job_name].semaphore] <= 0) {
          ready.push(job_name); // Возвращаем задачу в очередь
          continue; // Ждем освобождения семафора
        }
        --semaphores[jobs[job_name].semaphore];
      }

      // Запускаем задачу в отдельном потоке
      active_threads.emplace_back([&, job_name]() {
        worker(job_name);

        // Освобождаем семафор после выполнения задачи
        if (!jobs[job_name].semaphore.empty()) {
          std::lock_guard<std::mutex> lock(semaphores_mutex);
          ++semaphores[jobs[job_name].semaphore];
        }
      });
    }

    // Удаляем завершенные потоки
    active_threads.erase(std::remove_if(active_threads.begin(),
                                        active_threads.end(),
                                        [](std::thread &t) {
                                          if (t.joinable()) {
                                            t.join();
                                            return true;
                                          }
                                          return false;
                                        }),
                         active_threads.end());

    if (execution_failed) {
      for (auto &t : active_threads) {
        if (t.joinable()) {
          t.join();
        }
      }
      throw std::runtime_error("Execution failed due to job error");
    }
  }
}
