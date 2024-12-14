#include "../data/data.hpp"

struct ProcessData {
public:
  pid_t pid;
  int parent;

public:
  ProcessData(pid_t pid, int parent) : pid(pid), parent(parent) {};
};

class Control {
private:
  std::map<int, ProcessData> computings_map;

public:
  zmq::context_t context;
  zmq::socket_t socket_reply;
  zmq::socket_t publisher;

public:
  Control();

  void create_process(int id, int parent = 0);

  bool ping_process(int id);

  void exec_command(int id, char *argv[]);

  ~Control();

  bool is_data_correct(int id, int parent);

private:
  void fork_handler(int id, pid_t pid);
};
