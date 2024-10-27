#include "sort.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <cmath>

int TOTAL_THREADS = 1;

template <typename T> QuickSort<T>::QuickSort(std::vector<T> &v) : data_(v) {}

template <typename T> void QuickSort<T>::quick_sort(int start, int end, int stage, int max_stage) {
  if (start < end) {
    int p = this->parrtition(start, end);

    if (end - start > 1) {
      if (stage < max_stage) {
        auto async_sort = [this](int start, int end, int stage, int max_stage) {
          this->quick_sort(start, end, stage, max_stage);
        };

        auto rightFuture = std::async(std::launch::async, async_sort, p + 1, end, stage + 1, max_stage);

        ++TOTAL_THREADS;

        this->quick_sort(start, p - 1, stage + 1, max_stage);

        rightFuture.get();
      } else {
        this->quick_sort(start, p - 1, stage + 1, max_stage);
        this->quick_sort(p + 1, end, stage + 1, max_stage);
      }
    }
  }
}

template <typename T> int QuickSort<T>::size() { return data_.size(); }

template <typename T> void QuickSort<T>::print() {
  for (auto el : data_) {
    std::cout << el << ' ';
  }
}

template <typename T> int QuickSort<T>::parrtition(int start, int end) {
  int pivot = data_[end];
  int i = start;

  for (int j = start; j <= end; ++j) {
    if (data_[j] <= pivot) {
      std::swap(data_[i], data_[j]);
      ++i;
    }
  }

  return i - 1;
}

template <typename T> QuickSort<T>::~QuickSort() {}

int main(int argc, char* argv[]) {
  int max_threads = std::atoi(argv[1]);
  int max_stage = (int)log2(max_threads);

  std::ifstream file("helper.txt");
  std::vector<int> v;
  std::copy(std::istream_iterator<int>(file), std::istream_iterator<int>(),
            std::back_inserter(v));

  QuickSort<int> vec(v);

  auto start_time = std::chrono::high_resolution_clock::now();
  vec.quick_sort(0, vec.size() - 1, 0, max_stage);
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      end_time - start_time);

  std::cout << "Total threads used: " << TOTAL_THREADS << '\n';
  std::cout << '\n'
            << "Runtime: " << duration.count() << " мс"
            << std::endl;
  return 0;
}