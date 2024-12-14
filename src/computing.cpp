#include "../include/computing.hpp"
#include <iostream>

ChildCommadData parse_command(const std::string &input) {
  std::istringstream stream(input);
  int req_id = -1;
  std::string command = "";

  if (!(stream >> req_id) || !(stream >> command)) {
    return ChildCommadData{-1, ""};
  }

  return ChildCommadData{req_id, command};
}

Computing::Computing(int id)
    : milliseconds(0), id(id), timer_stat(TimerStats::Inactive) {
  context = zmq::context_t(1);
  subscriber = zmq::socket_t(context, ZMQ_SUB);
  subscriber.connect("tcp://localhost:52000");
  // Для отправки результатов выполнения программы без получения ответа
  socet_request = zmq::socket_t(context, ZMQ_PUSH);
  socet_request.connect("tcp://localhost:52001");
  subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
}

Computing::~Computing() {
  subscriber.close();
  socet_request.close();
  context.close();
}

int Computing::get_id() { return id; }

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
  socet_request.send(message, zmq::send_flags::none);
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
  socet_request.send(message, zmq::send_flags::none);
}

void Computing::show_time() {
  std::string response = "Seconds: " + std::to_string(milliseconds / 1000) +
                         ":" + std::to_string(milliseconds % 1000);
  zmq::message_t message(response.c_str(), response.length());
  socet_request.send(message, zmq::send_flags::none);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: create <id> <parent>\n<parent> is optional"
              << std::endl;
    return 1;
  }
  Computing main_process = Computing(std::stoi(argv[1]));
  zmq::message_t request(1024);
  ChildCommadData command;

  while (true) {
    main_process.subscriber.recv(request, zmq::recv_flags::none);

    std::string input = request.to_string();
    command = parse_command(input);

    if (command.req_id == -1 || command.command == "") {
      main_process.socet_request.send(request, zmq::send_flags::none);
      fmt::print(fg(fmt::color::red),
                 "Unknown command\nUsage: exec <id> [stop|start|time]\n");
      continue;
    }

    if (command.req_id != main_process.get_id()) {
      continue;
    }

    if (command.command == "start") {
      main_process.start_timer();
    } else if (command.command == "stop") {
      main_process.stop_timer();
    } else if (command.command == "time") {
      main_process.show_time();
    } else if (command.command == "ping") {
      main_process.socet_request.send(request, zmq::send_flags::none);
    } else {
      main_process.socet_request.send(request, zmq::send_flags::none);
      fmt::print(fg(fmt::color::red), "Unknown command: {}\n", command.command);
    }
  }
  return 0;
}
