#include "../include/computing/computing.cpp"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: create <id> <parent>\n<parent> is optional"
              << std::endl;
    return 1;
  }
  Computing main_process = Computing(std::stoi(argv[1]));
  zmq::message_t request(1024);
  std::optional<ChildCommadData> command_opt;
  ChildCommadData command;

  while (true) {
    auto result = main_process.subscriber.recv(request, zmq::recv_flags::none);

    if (!result) {
      fmt::print(fg(fmt::color::red), "Failed to receive message\n");
    }

    std::string input = request.to_string();

    command_opt = parse_command_computing(input);

    if (!command_opt.has_value()) {
      main_process.socket_request.send(request, zmq::send_flags::none);
      fmt::print(fg(fmt::color::red),
                 "Unknown command\nUsage: exec <id> [stop|start|time]\n");
      continue;
    } else {
      command = command_opt.value();
    }

    if (command.req_id != main_process.get_id()) {
      continue;
    }

    switch (command.command) {
    case CompCommands::Start:
      main_process.start_timer();
      break;
    case CompCommands::Stop:
      main_process.stop_timer();
      break;
    case CompCommands::Time:
      main_process.show_time();
      break;
    default:
      main_process.socket_request.send(request, zmq::send_flags::none);
      fmt::print(fg(fmt::color::red), "Unknown command\n");
      fmt::print(INSTRUCTION);
      break;
    }
  }
  return 0;
}