#include <algorithm>
#include <atomic>
#include <future>
#include <iostream>
#include <numeric>
#include <ranges>
#include <thread>
#include <vector>

using namespace std;

void data_race() {
  const auto n = 1'000'000;
  auto counter = 0;

  auto f1 = [&] {
    for (auto i = 0; i < n; ++i)
      ++counter;
  };
  auto f2 = [&] {
    for (auto i = 0; i < n; ++i)
      --counter;
  };

  auto t1 = thread(f1);
  auto t2 = thread(f2);

  t1.join();
  t2.join();

  cout << "Counter: " << counter << endl;
}

void thread_safe() {
  const auto n = 1'000'000;
  auto counter = 0;

  auto f1 = [local_counter = counter]() mutable {
    for (auto i = 0; i < n; ++i)
      ++local_counter;

    return local_counter;
  };

  auto f2 = [local_counter = counter]() mutable {
    for (auto i = 0; i < n; ++i)
      --local_counter;

    return local_counter;
  };

  auto t1 = async(f1);
  auto t2 = async(f2);

  auto result = t1.get() + t2.get();
  cout << "Counter: " << result << endl;
}

int main() {
  thread_safe();

  return 0;
}
