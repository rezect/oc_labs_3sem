#include "../include/DAG/dag.cpp"
#include "nlohmann/json.hpp"
#include <fstream>

const std::string JOBS_JSON = "data/jobs.json";

using json = nlohmann::json;

static DAG from_json(const std::string &json_string) {
  auto parsed_json = json::parse(json_string);

  std::vector<Job> jobs_list;
  for (const auto &job_json : parsed_json["jobs"]) {
    Job job{job_json["name"].get<std::string>(),
            job_json.value("depends_on", std::vector<std::string>()),
            job_json["sleep_time"].get<int16_t>(),
            job_json.value("semaphore", "")};
    jobs_list.push_back(job);
  }

  std::unordered_map<std::string, int> semaphores_list;
  if (parsed_json.contains("semaphores")) {
    for (const auto &[key, value] : parsed_json["semaphores"].items()) {
      semaphores_list[key] = value.get<int>();
    }
  }

  return DAG(jobs_list, semaphores_list);
}

std::string read_file_to_string(const std::string &file_path = JOBS_JSON) {
  std::ifstream file(file_path);
  if (!file.is_open()) {
    throw std::runtime_error("Не удалось открыть файл: " + file_path);
  }

  std::stringstream buffer;
  buffer << file.rdbuf();

  return buffer.str();
}