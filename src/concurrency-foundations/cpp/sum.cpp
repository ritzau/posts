#include <algorithm>
#include <atomic>
#include <future>
#include <iostream>
#include <numeric>
#include <ranges>
#include <thread>
#include <vector>

constexpr int n = 1000000;

int future_sum(const std::vector<int> &numbers) {
  auto start = numbers.cbegin();
  auto split = numbers.cbegin() + numbers.size() / 2;
  auto end = numbers.cend();

  auto f1 = std::async([=] { return std::reduce(start, split); });
  auto f2 = std::async([=] { return std::reduce(split, end); });

  return f1.get() + f2.get();
}

int main() {
  auto numbers = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto start = numbers.cbegin();
  auto split = numbers.cbegin() + numbers.size() / 2;
  auto end = numbers.cend();

  auto f1 = std::async([=] { return std::reduce(start, split); });
  auto f2 = std::async([=] { return std::reduce(split, end); });

  std::cout << "Sum: " << f1.get() + f2.get() << std::endl;

  return 0;
}
