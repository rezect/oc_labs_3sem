#include "../include/computing.hpp"
#include <zmq.hpp>
#include<unistd.h>
#include <map>

struct ProcessData {
public:
  pid_t pid;
  int parent;

public:
  ProcessData(pid_t pid, int parent);
};

class Control {
private:
  std::map<int, ProcessData> computings_map;
  
public:
  zmq::context_t context;
  zmq::socket_t socket;

public:
  Control();

  void create_process(int id, int parent = 0);

  int get_pid(int id) {
    return computings_map.size();
  }

  void exec_command(int id, char *argv[]);

  ~Control();
};
