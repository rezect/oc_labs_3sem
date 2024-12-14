#include "../data/data.hpp"

enum TimerStats { Active, Inactive };

class Computing {
private:
  int id;
  long long milliseconds;
  std::chrono::steady_clock::time_point start_time;
  TimerStats timer_stat;

public:
  zmq::context_t context;
  zmq::socket_t socket_request;
  zmq::socket_t subscriber;

public:
  Computing(int id);

  void start_timer();

  void stop_timer();

  int get_id();

  void show_time();

  ~Computing();
};
