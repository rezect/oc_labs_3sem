#include "../include/control/control.cpp"

int main() {
  // Подписываемся на завершение дочерних процессов
  struct sigaction sa;
  sa.sa_handler = sigchld_handler;
  sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  sigaction(SIGCHLD, &sa, NULL);

  Control control;
  CommandData command;
  std::string request;
  zmq::message_t reply;

  while (true) {
    request = "";

    fmt::print("Enter command: ");
    std::getline(std::cin, request);

    command = parse_command_control(request);

    if (command.command == "create") {
      if (command.params.size() == 0) {
        fmt::print(fg(fmt::color::red),
                   "Usage: create <id> <parent>\n<parent> is optional\n");
      } else if (command.params.size() == 1) {
        try {
          std::stoi(command.params[0].c_str());
        } catch (...) {
          fmt::print(fg(fmt::color::red),
                     "Usage: create <id> <parent>\nid is int\n");
          continue;
        }
        control.create_process(std::stoi(command.params[0].c_str()));
      } else {
        try {
          std::stoi(command.params[0].c_str());
          std::stoi(command.params[1].c_str());
        } catch (...) {
          fmt::print(fg(fmt::color::red),
                     "Usage: create <id> <parent>\nid is int\nparent is int\n");
          continue;
        }
        control.create_process(std::stoi(command.params[0].c_str()),
                               std::stoi(command.params[1].c_str()));
      }
    } else if (command.command == "exec") {
      if (command.params.size() != 2) {
        fmt::print(fg(fmt::color::red), "Usage: exec <id> <command>\n");
        continue;
      }

      try {
        std::stoi(command.params[0].c_str());
      } catch (...) {
        fmt::print(fg(fmt::color::red),
                   "Usage: exec <id> <command>\nid should be int\n");
      }

      std::string req_mess = command.params[0] + " " + command.params[1];

      send_message(control.publisher, req_mess);

      auto result = control.socket_reply.recv(reply, zmq::recv_flags::none);

      if (!result) {
        fmt::print(fg(fmt::color::red), "Failed to receive message\n");
      }

      fmt::print(fg(fmt::color::green), "Ok: {}\n", reply.to_string());
    } else if (command.command == "ping") {
      try {
        std::stoi(command.params[0].c_str());
      } catch (...) {
        fmt::print(fg(fmt::color::red), "Usage: ping <id>\nid is int\n");
        continue;
      }
      if (control.ping_process(std::stoi(command.params[0].c_str()))) {
        fmt::print(fg(fmt::color::green), "Ok: 1\n");
      } else {
        fmt::print(fg(fmt::color::red), "Ok: 0\n");
      }
    } else {
      fmt::print(fg(fmt::color::red), "Unknown command\n");
      fmt::print(INSTRUCTION);
      continue;
    }
  }

  int result = 0;
  wait(&result);

  return 0;
}
