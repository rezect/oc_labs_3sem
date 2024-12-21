#include "src/parser.cpp"

int main() {
  std::string str = read_file_to_string();

  DAG dag = from_json(str);

  dag.print();

  std::cout << "Starting DAG execution..." << std::endl;

  dag.execute(4);

  std::cout << "DAG execution completed successfully." << std::endl;

  return 0;
}
