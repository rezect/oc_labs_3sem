#include <chrono>
#include <cmath>
#include <fstream>
#include <future>
#include <iostream>
#include <iterator>
#include <vector>

std::atomic<int> TOTAL_THREADS(1);

template <typename T> int parrtition(std::vector<T> &data, int start, int end) {
  int pivot = data[end];
  int i = start;

  for (int j = start; j <= end; ++j) {
    if (data[j] <= pivot) {
      std::swap(data[i], data[j]);
      ++i;
    }
  }

  return i - 1;
}

template <typename T>
void quick_sort(std::vector<T> &data, int start, int end, int max_stage,
                int stage = 0) {
  if (start < end) {
    int p = parrtition(data, start, end);

    if (end - start > 1) {
      if (stage < max_stage) {
        auto async_sort = [&data, max_stage](int start, int end, int stage) {
          quick_sort(data, start, end, max_stage, stage);
        };

        auto rightFuture =
            std::async(std::launch::async, async_sort, p + 1, end, stage + 1);

        std::cout << "New thread created!" << '\n';
        ++TOTAL_THREADS;

        quick_sort(data, start, p - 1, max_stage, stage + 1);

        rightFuture.get();
      } else {
        quick_sort(data, start, p - 1, max_stage, stage + 1);
        quick_sort(data, p + 1, end, max_stage, stage + 1);
      }
    }
  }
}

int main(int argc, char *argv[]) {
  int max_threads = std::atoi(argv[1]);
  int max_stage = (int)log2(max_threads);

  std::ifstream file("helper.txt");
  std::vector<int> vec;
  std::copy(std::istream_iterator<int>(file), std::istream_iterator<int>(),
            std::back_inserter(vec));

  auto start_time = std::chrono::high_resolution_clock::now();
  quick_sort(vec, 0, vec.size() - 1, max_stage);
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      end_time - start_time);

  std::cout << "\nTotal threads used: " << TOTAL_THREADS << '\n';
  std::cout << '\n' << "Runtime: " << duration.count() << " мс" << std::endl;
  return 0;
}