#include "src/parser.cpp"

int main() {
  std::string str = read_file_to_string();
  DAG dag = from_json(str);
  dag.print();
}