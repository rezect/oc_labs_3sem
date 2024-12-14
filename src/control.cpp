#include "../include/control.hpp"
#include <iostream>
#include <signal.h>
#include <sys/wait.h>
#include <zmq.hpp>

CommandData parse_command(const std::string &input) {
  std::istringstream stream(input);
  std::string command = "";
  std::vector<std::string> params;
  std::string parametr = "";

  if (!(stream >> command)) {
    return CommandData{"", {}};
  }

  while (stream >> parametr) {
    params.push_back(parametr);
  }

  return CommandData{command, params};
}

Control::Control() {
  context = zmq::context_t(1);
  socket_reply = zmq::socket_t(context, ZMQ_PULL);
  socket_reply.bind("tcp://localhost:52001");
  publisher = zmq::socket_t(context, ZMQ_PUB);
  publisher.bind("tcp://localhost:52000");
  computings_map = std::map<int, ProcessData>();
}

Control::~Control() {
  publisher.close();
  socket_reply.close();
  context.close();
}

void Control::create_process(int id, int parent) {
  pid_t pid = fork();

  if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    execl("src/computing", "computing", std::to_string(id).c_str(), NULL);
    perror("execl failed");
    exit(1);
  }

  if (computings_map.find(id) != computings_map.end()) {
    fmt::print(fg(fmt::color::red), "Error: Already exists\n");
    return;
  }
  if (parent != 0) {
    if (computings_map.find(parent) == computings_map.end()) {
      fmt::print(fg(fmt::color::red), "Error: Parent not found\n");
    } else if (ping_process(parent) == false) {
      fmt::print(fg(fmt::color::red), "Error: Parent is unavailable\n");
    }
  }

  computings_map.insert(std::make_pair(id, ProcessData(pid, parent)));

  fmt::print(fg(fmt::color::green), "Ok: {}\n", id);
}

bool Control::ping_process(int id) {
  pid_t pid = computings_map.at(id).pid;
  fmt::print(fg(fmt::color::green), "Ping: {}\n", pid);
  if (kill(pid, 0) == 0) {
    return true;
  } else if (errno == ESRCH) {
    return false;
  } else {
    perror("Error checking process");
    return false;
  }
}

void sigchld_handler(int signum) {
  int status;
  pid_t pid;

  // Обрабатываем все завершённые дочерние процессы
  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    if (WIFEXITED(status)) {
      std::cout << "Process " << pid << " exited with status "
                << WEXITSTATUS(status) << std::endl;
    } else if (WIFSIGNALED(status)) {
      std::cout << "Process " << pid << " was terminated by signal "
                << WTERMSIG(status) << std::endl;
    }
  }
}

int main() {
  struct sigaction sa;
  sa.sa_handler = sigchld_handler;
  sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  sigaction(SIGCHLD, &sa, NULL);

  Control control;
  CommandData command;
  std::string request = "";
  zmq::message_t reply;
  while (true) {
    request = "";
    fmt::print("Enter command: ");
    std::getline(std::cin, request);
    command = parse_command(request);
    fmt::print(fg(fmt::color::green), "Command: {}\n", request);
    if (command.command == "create") {
      if (command.params.size() == 0) {
        fmt::print(fg(fmt::color::red),
                   "Usage: create <id> <parent>\n<parent> is optional\n");
      } else if (command.params.size() == 1) {
        control.create_process(std::stoi(command.params[0].c_str()), 0);
      } else {
        control.create_process(std::stoi(command.params[0].c_str()),
                               std::stoi(command.params[1].c_str()));
      }
    } else if (command.command == "exec") {
      if (command.params.size() != 2) {
        fmt::print(fg(fmt::color::red), "Usage: exec <id> <command>\n");
        continue;
      }
      std::string req_mess = command.params[0] + " " + command.params[1];
      zmq::message_t message(req_mess.begin(), req_mess.end());
      control.publisher.send(message, zmq::send_flags::none);
      control.socket_reply.recv(reply, zmq::recv_flags::none);
      fmt::print(fg(fmt::color::green), "Receiving: {}\n", reply.to_string());
    } else if (command.command == "ping") {
      if (control.ping_process(std::stoi(command.params[0].c_str()))) {
        fmt::print(fg(fmt::color::green), "Ok: {}\n", command.params[0]);
      } else {
        fmt::print(fg(fmt::color::red), "Error: {}\n", command.params[0]);
      }
    }
  }
  int result = 0;
  wait(&result);
  return 0;
}
