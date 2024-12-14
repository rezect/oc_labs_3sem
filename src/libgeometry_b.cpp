#include <cmath>
#include <cstring>
#include <string>
#include "../include/libgeometry.h"


// Подсчет площади прямоугольного треугольника
extern "C" float Square(float A, float B) { return (A * B) / 2.0; }

// Перевод числа x в троичную систему
extern "C" char *translation(long x) {
  static char buffer[64];
  std::string result;
  while (x > 0) {
    result = std::to_string(x % 3) + result;
    x /= 3;
  }
  strncpy(buffer, result.c_str(), sizeof(buffer));
  return buffer;
}
