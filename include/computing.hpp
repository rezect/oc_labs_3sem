#include <chrono>
#include <zmq.hpp>

enum TimerStats { Active, Inactive };

class Computing {
private:
  int id;
  int parent;
  long long milliseconds;
  std::chrono::steady_clock::time_point start_time;
  TimerStats timer_stat;

public:
  zmq::context_t context;
  zmq::socket_t socket;

public:
  Computing(int id, int parent);
  
  void start_timer();

  void stop_timer();

  void show_time();

  ~Computing();
};
