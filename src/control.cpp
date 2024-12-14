#include "../include/control.hpp"
#include <zmq.hpp>
#include <sys/wait.h>
#include <iostream>

ProcessData::ProcessData(pid_t pid, int parent) : pid(pid), parent(parent) {};

Control::Control() {
  context = zmq::context_t(1);
  socket = zmq::socket_t(context, ZMQ_REQ);
  socket.connect("tcp://localhost:52000");

  computings_map = std::map<int, ProcessData>();
}

Control::~Control() {
  socket.close();
  context.close();
}

void Control::create_process(int id, int parent) {
  pid_t pid = fork();

  if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    execl("src/computing", "computing", NULL);
    perror("execl failed");
    exit(1);
  }
  computings_map.insert(std::make_pair(id, ProcessData(pid, parent)));

  std::string message = "Ok: " + std::to_string(pid);
}

int main() {
  Control control;
  std::string request = "";
  int id = 0;
  while (true) {
    request = "";
    std::cin >> request;
    if (request.substr(0, 6) == "create") {
      std::cin >> id;
      control.create_process(id);
    } else if (request.substr(0, 4) == "exec") {
      std::cin >> request;
      zmq::message_t message(request.c_str(), request.length());
      control.socket.send(message, zmq::send_flags::none);
      zmq::message_t reply;
      control.socket.recv(reply, zmq::recv_flags::none);
      std::cout << reply.to_string() << std::endl;
    }
  }
  int result = 0;
  wait(&result);
  return 0;
}