#include <algorithm>
#include <atomic>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

constexpr int n = 1000000;

int main2() {
  auto f = [] { std::cout << "foo" << std::endl; };

  auto t = std::thread{f};

  auto foo = std::vector<std::thread>{};
  foo.push_back(std::move(t));

  auto counter = std::atomic_int(0);

  auto threads = std::vector<std::thread>();
  threads.emplace_back([&counter] {
    for (auto i = 0; i < n; ++i) {
      ++counter;
    }
  });
  threads.emplace_back([&counter] {
    for (auto i = 0; i < n; ++i) {
      --counter;
    }
  });

  for (auto &t : threads)
    t.join();

  for (std::thread &t : foo)
    t.join();

  std::cout << "Hello, World! " << counter << std::endl;

  return 0;
}

void asyncTest() {
  auto f = [] {
    std::cout << "Hello, " << std::this_thread::get_id() << std::endl;
  };

  auto future = std::async(std::launch::async, f);

  // f();

  future.get();
}

void threadTest() {
  auto f = [] {
    std::cout << "Hello, " << std::this_thread::get_id() << std::endl;
  };

  // f();

  auto t = std::thread(f);

  // f();

  t.join();
}

int main() {
  asyncTest();
  threadTest();
  return 0;
}
