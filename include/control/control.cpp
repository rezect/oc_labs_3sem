#include "control.hpp"

Control::Control() {
  context = zmq::context_t(1);

  socket_reply = zmq::socket_t(context, ZMQ_PULL);
  socket_reply.bind(PARENT_READ_CHILD_WRITE);

  publisher = zmq::socket_t(context, ZMQ_PUB);
  publisher.bind(PARENT_WRITE_CHILD_READ);

  computings_map = std::map<int, ProcessData>();
}

Control::~Control() { context.close(); }

bool Control::is_data_correct(int id, int parent) {

  if (computings_map.find(id) != computings_map.end()) {
    fmt::print(fg(fmt::color::red), "Error: Already exists\n");
    return false;
  } else if (parent != 0) {
    if (computings_map.find(parent) == computings_map.end()) {
      fmt::print(fg(fmt::color::red), "Error: Parent not found\n");
      return false;
    } else if (ping_process(parent) == false) {
      fmt::print(fg(fmt::color::red), "Error: Parent is unavailable\n");
      return false;
    }
  }

  return true;
}

void Control::fork_handler(int id, pid_t pid) {
  if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    execl("obj/computing_process", "computing_process",
          std::to_string(id).c_str(), NULL);
    perror("execl failed");
    exit(1);
  }
}

void Control::create_process(int id, int parent) {
  pid_t pid = fork();

  fork_handler(id, pid);

  if (is_data_correct(id, parent) == false) {
    return;
  }

  computings_map.insert(std::make_pair(id, ProcessData(pid, parent)));

  fmt::print(fg(fmt::color::green), "Ok: {}\n", id);
}

bool is_process_exists(pid_t pid) {

  if (kill(pid, 0) == 0) {
    return true;
  } else if (errno == ESRCH) {
    return false;
  }

  perror("Error checking process");

  return false;
}

bool Control::ping_process(int id) {

  if (computings_map.find(id) == computings_map.end()) {
    return false;
  }

  pid_t pid = computings_map.at(id).pid;

  fmt::print(fg(fmt::color::green), "Ping: {}\n", pid);

  return is_process_exists(pid);
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
