#pragma once

#include <chrono>
#include <fmt/color.h>
#include <fmt/core.h>
#include <iostream>
#include <map>
#include <signal.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <zmq.hpp>

const std::string PARENT_WRITE_CHILD_READ = "tcp://localhost:52000";
const std::string PARENT_READ_CHILD_WRITE = "tcp://localhost:52001";
std::string INSTRUCTION = R"(Availible commands:

1. exec <id> [stop|start|time]
2. create <id> <parent>
3. ping <id>

\(^*^)/ ...miu...miu...miu
)";

enum CompCommands { Start, Stop, Time, Noname };

struct CommandData {
  std::string command = "";
  std::vector<std::string> params;
};

struct ChildCommadData {
  int req_id;
  CompCommands command;
};

void send_message(zmq::socket_t &socket, const std::string &message) {
  zmq::message_t msg(message.c_str(), message.length());
  socket.send(msg, zmq::send_flags::none);
}

CommandData parse_command_control(const std::string &input) {
  std::istringstream stream(input);
  std::string command = "";
  std::string parametr = "";
  std::vector<std::string> params;

  if (!(stream >> command)) {
    return CommandData{"", {}};
  }

  while (stream >> parametr) {
    params.push_back(parametr);
  }

  return CommandData{command, params};
}

std::optional<ChildCommadData>
parse_command_computing(const std::string &input) {
  std::istringstream stream(input);
  int req_id = -1;
  std::string command = "";
  CompCommands command_enum = CompCommands::Noname;

  if (!(stream >> req_id) || !(stream >> command)) {
    return std::nullopt;
  }

  if (command == "start") {
    command_enum = CompCommands::Start;
  } else if (command == "stop") {
    command_enum = CompCommands::Stop;
  } else if (command == "time") {
    command_enum = CompCommands::Time;
  }

  return std::optional<ChildCommadData>(ChildCommadData{req_id, command_enum});
}
