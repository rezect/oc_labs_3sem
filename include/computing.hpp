#include <chrono>
#include <zmq.hpp>
#include <fmt/color.h>
#include <fmt/core.h>

enum TimerStats { Active, Inactive };

struct CommandData {
  std::string command = "";
  std::vector<std::string> params;
};

struct ChildCommadData {
  int req_id;
  std::string command;
};

class Computing {
private:
  int id;
  long long milliseconds;
  std::chrono::steady_clock::time_point start_time;
  TimerStats timer_stat;

public:
  zmq::context_t context;
  zmq::socket_t socet_request;
  zmq::socket_t subscriber;

public:
  Computing(int id);
  
  void start_timer();

  void stop_timer();

  int get_id();

  void show_time();

  ~Computing();
};
