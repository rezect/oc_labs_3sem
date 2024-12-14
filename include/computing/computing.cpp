#include "computing.hpp"

Computing::Computing(int id)
    : milliseconds(0), id(id), timer_stat(TimerStats::Inactive) {
  context = zmq::context_t(1);
  subscriber = zmq::socket_t(context, ZMQ_SUB);
  subscriber.connect(PARENT_WRITE_CHILD_READ);

  // Для отправки результатов выполнения программы без получения ответа
  socket_request = zmq::socket_t(context, ZMQ_PUSH);
  socket_request.connect(PARENT_READ_CHILD_WRITE);
  subscriber.set(zmq::sockopt::subscribe, "");
}

Computing::~Computing() { context.close(); }

int Computing::get_id() { return id; }

void Computing::start_timer() {
  std::string response = "";
  if (timer_stat == TimerStats::Active) {
    response = "Error: Timer is already running";
  } else {
    timer_stat = TimerStats::Active;
    start_time = std::chrono::steady_clock::now();

    response = "Timer started";
  }

  send_message(socket_request, response);
}

void Computing::stop_timer() {
  std::string response = "";
  if (timer_stat == TimerStats::Inactive) {
    response = "Error: Timer is already stopped";
  } else {
    timer_stat = TimerStats::Inactive;
    milliseconds += std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - start_time)
                        .count();

    response = "Timer stopped";
  }

  send_message(socket_request, response);
}

void Computing::show_time() {
  std::string response = "";

  if (timer_stat == TimerStats::Inactive) {
    response = "Timer: " + std::to_string(milliseconds / 1000) + ":" +
               std::to_string(milliseconds % 1000);
  } else {
    auto milliseconds_now =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start_time)
            .count();
    response = "Timer: " + std::to_string(milliseconds_now / 60000) + ":" +
               std::to_string((milliseconds_now % 60000) / 1000) + ":" +
               std::to_string(milliseconds_now % 1000);
  }

  send_message(socket_request, response);
}
