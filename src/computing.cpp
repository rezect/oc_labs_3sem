#include "../include/computing.hpp"
#include <iostream>

Computing::Computing(int id, int parent) : milliseconds(0), id(id), parent(parent), timer_stat(TimerStats::Inactive) {
  context = zmq::context_t(1);
  socket = zmq::socket_t(context, ZMQ_REP);
  socket.bind("tcp://localhost:52000");
}

Computing::~Computing() {
  socket.close();
  context.close();
}

void Computing::start_timer() {
  std::string response = "";
  if (timer_stat == TimerStats::Active) {
    response = "Error: Timer is already running";
    return;
  }

  timer_stat = TimerStats::Active;
  start_time = std::chrono::steady_clock::now();

  response = "Timer started";

  zmq::message_t message(response.c_str(), response.length());
  socket.send(message, zmq::send_flags::none);
}

void Computing::stop_timer() {
  std::string response = "";
  if (timer_stat == TimerStats::Inactive) {
    response = "Error: Timer is already stopped";
    return;
  }

  timer_stat = TimerStats::Inactive;
  milliseconds += std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now() - start_time)
                      .count();

  response = "Timer stopped";

  zmq::message_t message(response.c_str(), response.length());
  socket.send(message, zmq::send_flags::none);
}

void Computing::show_time() {
  std::string response = "Seconds: " + std::to_string(milliseconds / 1000) + ":" + std::to_string(milliseconds % 1000);
  zmq::message_t message(response.c_str(), response.length());
  socket.send(message, zmq::send_flags::none);
}

int main() {
  std::cout << "Computing process started" << std::endl;
  Computing* main_process = new Computing(0, 0);
  while (true) {
    zmq::message_t request;
    main_process->socket.recv(request, zmq::recv_flags::none);

    if (request.to_string() == "start") {
      main_process->start_timer();
    } else if (request.to_string() == "stop") {
      main_process->stop_timer();
    } else if (request.to_string() == "time") {
      main_process->show_time();
    } else {
      std::cout << "Unknown command" << std::endl;
    }
  }
  return 0;
}
