#include <dlfcn.h>
#include <iostream>

typedef float (*SquareFunc)(float, float);
typedef char *(*TranslationFunc)(long);

int main() {
  void *handle = dlopen("libs/libgeometry_a.so", RTLD_LAZY);
  if (!handle) {
    std::cerr << "Failed to load library: " << dlerror() << std::endl;
    return 1;
  }

  SquareFunc Square = (SquareFunc)dlsym(handle, "Square");
  TranslationFunc translation = (TranslationFunc)dlsym(handle, "translation");
  if (!Square || !translation) {
    std::cerr << "Failed to find functions: " << dlerror() << std::endl;
    dlclose(handle);
    return 1;
  }

  int command = 0;
  bool a = true;
  std::string lib_path = "libs/libgeometry_a.so";

  while (true) {
    std::cout << "Enter command (0 to switch lib, 1 for area, 2 for number "
                 "translation): ";
    std::cin >> command;

    if (command == 0) {
      if (a) {
        lib_path = "libs/libgeometry_b.so";
        a = false;
      } else {
        lib_path = "libs/libgeometry_a.so";
        a = true;
      }
      dlclose(handle);
      handle = dlopen(lib_path.c_str(), RTLD_LAZY);
      if (!handle) {
        std::cerr << "Failed to load library: " << dlerror() << std::endl;
        return 1;
      }
      Square = (SquareFunc)dlsym(handle, "Square");
      translation = (TranslationFunc)dlsym(handle, "translation");
    } else if (command == 1) {
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
  }

  dlclose(handle);
  return 0;
}
