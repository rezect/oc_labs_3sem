#include <cmath>
#include <cstring>
#include <string>
#include "../include/libgeometry.h"

// Подсчет площади прямоугольника
extern "C" float Square(float A, float B) { return A * B; }

// Перевод числа x в двоичную систему
extern "C" char *translation(long x) {
  static char buffer[64];
  std::string result;
  while (x > 0) {
    result = std::to_string(x % 2) + result;
    x /= 2;
  }
  strncpy(buffer, result.c_str(), sizeof(buffer));
  return buffer;
}
