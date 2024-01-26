#include <algorithm>
#include <atomic>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

constexpr int n = 1'000'000;

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
  using namespace std;

  // Define a function to say hello
  auto sayHello = [] { cout << "Hello, " << this_thread::get_id() << endl; };

  // Create and spin up a new thread to say hello
  auto t = thread(sayHello);
  // Say hello from the main thread
  sayHello();

  // Wait for the thread to complete
  t.join();
}

int main() {
  // asyncTest();
  threadTest();
  return 0;
}
