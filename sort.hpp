#include <chrono>
#include <cmath>
#include <fstream>
#include <future>
#include <iostream>
#include <iterator>
#include <vector>

template <typename T> class QuickSort {
public:
  QuickSort(std::vector<T> &v);

  void quick_sort(int start, int end, int stage = 0, int max_stage = 0);

  int size();

  void print();

  ~QuickSort();

private:
  int parrtition(int start, int end);

private:
  std::vector<T> &data_;
};