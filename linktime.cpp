#include <iostream>

float Square(float A, float B);
char *translation(long x);

int main() {
  int command;
  std::cout << "Enter command (1 for area, 2 for number translation): ";
  std::cin >> command;

  if (command == 1) {
    float A, B;
    std::cout << "Enter sides A and B: ";
    std::cin >> A >> B;
    std::cout << "Area: " << Square(A, B) << std::endl;
  } else if (command == 2) {
    long x;
    std::cout << "Enter number to convert: ";
    std::cin >> x;
    std::cout << "Converted: " << translation(x) << std::endl;
  } else {
    std::cout << "Invalid command!" << std::endl;
  }

  return 0;
}