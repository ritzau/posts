---
layout: post
title: Introduction to Concurrent Code
date: 2024-02-25 11:11:11 +0100
author: Tobias Ritzau
categories: draft note cs concurrency introduction
---

Before we delve into concurrency, this initial note aims to establish the foundational knowledge necessary for beginning with concurrent programming. It starts with an overview of what concurrency is and identifies scenarios where it can enhance our coding projects.

Following this introduction, we'll examine practical examples of concurrent code implemented in C++ and Kotlin. Along the way, attention will be given to common pitfalls and effective strategies for navigating these challenges.

The provided examples are designed to be concise and easily readable on any device. While they effectively illustrate key concepts, they're not intended for direct use in production environments—a distinction that might be explored in a future discussion. For those interested in a deeper dive, all examples are available [here](https://github.com/ritzau/posts), with comprehensive instructions for building and running them. This code has been adjusted for optimal viewing on a wide range of screens.

## Introducing Concurrency

When we start coding, we usually begin with _sequential_ programs, where tasks follow one another in order. This straightforward approach lays the groundwork for understanding how programs execute. However, as we grow more comfortable with sequential execution, diving into _concurrent_ programming opens up new possibilities for enhancing our skills.

Modern computing inherently multitasks, running multiple programs simultaneously. Launching a program creates a _process_, essentially a container for the running code. These processes can spawn additional ones, running side by side, yet usually operate independently unless explicitly designed to interact. While concurrency encompasses managing multiple processes, this note focuses on concurrency within a single process—a common scenario for many programming tasks, though the principles discussed often extend to multiple processes.

At the heart of process-based concurrency are _threads_. Think of a thread as a pathway of execution within a program. Each program starts with a single thread, the main thread, which can initiate more threads as needed. Threads share the same memory space, making them lightweight and efficient but also necessitating careful management to avoid conflicts. The question then arises: why use multiple threads?

- UI applications benefit from concurrency by keeping the interface responsive while performing heavy tasks in the background.
- In gaming, separate threads can enhance the experience by rendering graphics and processing game logic concurrently.
- Web servers improve scalability and responsiveness by handling each request with its own thread.

With the advent of multi-core processors, concurrency has become even more critical. Each core can execute a thread independently, allowing for true parallel processing. However, the benefits of concurrency aren't limited to multi-core environments. Even on single-core systems, efficiently managing CPU time can significantly improve performance, much like multitasking household chores while waiting for a TV show to continue.

However, it's crucial to recognize that not all tasks benefit from concurrency. Some processes, like baking a cake or, more pointedly, pregnancy, cannot be expedited by adding more resources. This analogy underscores the inherent limitations of concurrency and parallel processing.

Engaging with concurrency, despite its complexities and limitations, offers significant advantages. Properly implemented, it can make our programs more efficient, responsive, and capable, fully leveraging the capabilities of modern hardware. As we delve deeper into concurrency, we'll explore how to harness these benefits effectively.

## Benefits of concurrency

Embracing concurrency in programming unlocks several key advantages. These benefits not only enhance the performance of applications but also contribute to more efficient and maintainable code. Let's explore the three primary benefits:

### 1. Efficiency

The evolution of processors has shifted from increasing clock speeds to adding more cores. This change underscores the growing importance of concurrency in modern computing. Think of a busy restaurant kitchen: having several chefs (cores) working on different dishes (threads) simultaneously maximizes kitchen throughput. However, the addition of more chefs is only effective if there are enough tasks to keep them busy. Similarly, to fully utilize multi-core processors' capabilities, applications need to run multiple threads, allowing each core to contribute to the overall processing power.

### 2. Responsiveness

System responsiveness is crucial in various contexts. For example, in a web server scenario, handling HTTP requests sequentially is akin to a single waiter managing and cooking all orders in a restaurant, leading to delays and unhappy guests. By contrast, applying concurrency to a web server can be compared to a team of waiters swiftly taking orders and passing them to the kitchen, significantly improving response times.

This principle applies to UI applications as well, where a main thread dedicated to rendering should not be burdened with time-consuming tasks. Offloading such tasks to separate threads or processes keeps the main thread available for rendering and user interactions, similar to waiters efficiently managing orders while the kitchen handles the cooking.

### 3. Separation of concerns

Concurrency also facilitates the division of a program into logical components, streamlining both development and maintenance. Back to the restaurant analogy: it’s more efficient to provide a chef with three separate recipes for different courses than one convoluted set of instructions for all three courses. This separation allows the chef to manage tasks effectively, switching between them based on progress and priority. Similarly, when programming tasks are divided into distinct concurrent components, they become less complex and more adaptable, enhancing the overall code structure and flexibility.

## Writing concurrent code

Concurrent programming offers a wealth of opportunities to enhance both the performance and responsiveness of applications. Let's delve into some commonly used methods for writing concurrent code, showcasing examples in C++ and Kotlin.

### Threads

Threads form the backbone of concurrency, each representing a unique path of execution within a process. Initially viewed as lightweight alternatives to running multiple processes, threads are now acknowledged for their inherent complexity and resource demands. We'll start with an exploration of threads before progressing to more advanced and efficient concepts.

When you create a thread, you provide it with a function to execute. The life cycle of the thread is closely tied to this function - when the function's execution ends, so does the thread.

#### Example: C++ Thread

Creating a new thread in C++ is relatively simple. You define a function or a lambda expression and pass it to the `std::thread` constructor. This action causes the thread to start executing concurrently with the main thread. It's important to note that you must invoke `std::join` on the thread before its instance is destroyed (which generally happens when it goes out of scope). Neglecting to do so results in the program terminating unexpectedly. To mitigate this risk, C++20 introduced `std::jthread`, which automatically handles the joining of the thread upon its destruction, provided it hasn't been joined already.

{% include replit.md href="" %}

```cpp
// Define a lambda function
// to say hello
auto sayHello = []{
    cout << "Hello, "
         << this_thread::get_id()
         << endl;
};

// Create and start a new thread
thread t(sayHello);

// Call the function in the
// main thread
sayHello();

// Wait for the new thread
// to finish
t.join();
```

#### Example: Kotlin Thread

Kotlin supports multiple runtimes. In this guide, "Kotlin" refers to Kotlin running on the JVM. To create a thread in Kotlin, you define an instance of the `Runnable` interface, either by implementing it or using a lambda expression. This instance is then passed to the `Thread` constructor. Unlike some languages, the thread in Kotlin does not start with the constructor; instead, you must start it manually using the `start` method. After starting, you can join the thread with the `join` method. The program waits for the thread to complete before exiting, unless you have explicitly specified otherwise.

{% include replit.md href="" %}

```kotlin
// Define a lambda function to
// say hello
val sayHello = {
  val tid = Thread.currentThread().id
  println("Hello, $tid")
};

// Create and start a new thread
val t = Thread(sayHello)
t.start()

// Call the function on the
// main thread
sayHello();

// Wait for the new thread
// to finish
t.join();
```

These examples illustrate the basic usage patterns of threads in both C++ and Kotlin. As you delve deeper into concurrent programming, you'll encounter more advanced scenarios and techniques for managing threads.

Threading in both C++ and Kotlin enables the execution of multiple operation sequences concurrently, a crucial concept for leveraging the power of modern multi-core processors and optimizing application performance.

Understanding the specifics of each language is less important than grasping the overall concepts, which remain consistent across different languages.

## Pitfalls

Understanding the nuances of concurrency and its implementation is only the beginning. Awareness of potential issues arising from concurrent execution is crucial. These problems can be elusive, manifesting as rare "once in a million" bugs, and are notoriously difficult to identify and fix. Code that is safe to execute concurrently by multiple threads is said to be _thread safe_.

### Data races and race conditions

Two major categories of issues commonly arise in concurrent code:

- _Race Conditions_: These occur when the timing of uncontrollable events affects the outcome of a computation. While sometimes harmless or intentional, they often lead to unexpected and erroneous results. Consider a restaurant where the sequence of dish preparation varies. Normally, this is fine, but if desserts are served before the main courses, it indicates a problem.

- _Data Races_: These happen when two or more threads access the same resource without proper synchronization, and at least one thread modifies it. Often involving shared variables, data races can cause threads to work with outdated or incorrect states of the resource, leading to complications. For instance, imagine a chef mistakenly adding salt instead of sugar to a dessert due to confusion in a busy kitchen.

_Data races_ can also lead to _tearing_, where only part of a resource is updated, potentially resulting in corrupted data and unpredictable behavior. For example, if one thread partially updates a 64-bit value while another reads it, this could cause program crashes or other serious issues on a 32 bit architecture.

#### Example: C++ data race

Here is an example demonstrating a data race, where two threads modify a `counter` variable – one incrementing and the other decrementing. Due to the nature of concurrent execution, the final value of `counter` might not be as expected.

{% include replit.md href="" %}

```cpp
const auto n = 1'000'000;

// Shared counter
auto counter = 0;

// The '&' allows the lambda to
// reference any in-scope variable
auto increase = [&counter] {
  for (auto i = 0; i < n; ++i) {
    // Update shared variable
    ++counter;
}
};
auto decrease = [&counter] {
  for (auto i = 0; i < n; ++i) {
    // Update shared variable
    --counter;
}
};

auto t1 = thread(increase);
auto t2 = thread(decrease);

t1.join();
t2.join();

// Result is undefined due to
// the data race
cout << "Counter: "
  << counter
  << endl;
```

The challenges posed by data races in programming boil down to three aspects:

1. **Sequentiality**&emsp; Operations within a thread are sequential, but the ordering of operations across different threads is unknown.
2. **Visibility**&emsp; The various caching levels between executing instructions and main memory make it hard to predict when an instruction's result becomes visible to another thread.
3. **Optimization**&emsp; To optimize performance, compilers and processors often reorder instructions, further complicating the matter.

To manage these challenges, it's necessary to establish rules determining which operations logically **happen before** others.

## Thread-safe code

Now let's explore how to structure concurrent code to prevent data races and manage race conditions. Key to this is isolating code that accesses a shared state and understanding the _happened-before_ relationship.

By keeping the shared state and the code that accesses it isolated, you significantly simplify understanding the code, even when returning to it later. This isolation makes it easier to modify the code and verify that it remains thread-safe.

Compilers and processors adhere to specific rules to ensure which operations **happen before** others, regardless of which thread they execute on. These rules constitute the memory consistency model, defined at the language/runtime level. Therefore, these rules vary between languages/runtimes, and it's essential to understand the rules for the setup you are using. Here's a subset of these rules that hold true for both Kotlin and C++, ensuring thread safety – the ability to safely execute code concurrently by multiple threads:

1. **In-Thread Sequence**&emsp; Each operation in a thread follows the one before it, establishing a clear order within the thread itself.
2. **Thread Start**&emsp; Operations executed in a thread before starting a new thread "happen before" all operations in the new thread. This is also applicable to Kotlin coroutines and tasks in task queues.
3. **Thread Join**&emsp; Operations executed in a thread "happen before" those executed after joining the thread.

A _future_ is a powerful concept that enables asynchronous access to the outcome of a computation and is a thread-safe way to pass values in concurrent code. A future represents a value that may not yet be available but will be in the future. In some contexts, these are called _Promises_. Futures and promises can be completed either with a value or an error.

#### Example: Thread-safe Kotlin

Consider this example using `async()` in Kotlin and C++, where the result of concurrent code execution is available as a future:

{% include replit.md href="" %}

```kotlin
// Operations here are guaranteed
// to execute before those in
// the coroutine
val foo = 42
val future = async {
  // Safe access to foo, but
  // no ordering guarantees
  // for modifications by other
  // threads after this
  // coroutine starts
  foo + 1337
}
// Exercise caution until the
// future's result is awaited
val result = future.await()
```

For concurrency safety, making data accessed within concurrent code immutable is recommended, as it eliminates race condition risks. If immutability isn't feasible, copying or moving the data into the thread ensures exclusive access.

#### Example: Thread-safe C++

Here's an example using futures and local data copies to avoid data races:

{% include replit.md href="" %}

```cpp
const auto n = 1'000'000;
auto counter = 0;

// Copying data into the
// lambda ensures no
// race condition
auto f1 =
  [local_counter = counter]() mutable {
    for (auto i = 0; i < n; ++i) {
      // Update local copy
      ++local_counter;
    }
    return local_counter;
};

auto f2 =
  [local_counter = counter]() mutable {
    for (auto i = 0; i < n; ++i) {
      // Update another local copy
      --local_counter;
    }
    return local_counter;
};

// Concurrently execute
// without risk of
// data race
auto t1 = async(f1);
auto t2 = async(f2);

// Await results and combine
auto result = t1.get() + t2.get();

// Consistent and predictable
// outcome
cout << "Counter: "
     << result
     << endl;
```

## Key Takeaways

- **Concurrent code** helps you improve **separation of concerns**, **efficiency**, and **responsiveness**.
- **Threads** are the foundational abstraction for executing concurrent code.
- **Task queues**, aka. **Executors**, increase the efficiency and control of concurrent code.
- Either ensure that your code is only executed by a single thread (e.g., on a serial executor), or that it is **thread-safe**.
- A **race condition** makes the result of your code dependent on external events out of your control. This may be harmless and even intentional, but it often results in bugs.
- A **data race** happens when multiple threads access the same resource without proper synchronization, and at least one thread modifies the resource. Data races cause issues with **sequentiality**, **visibility**, and **optimizations**.
- The **Happened-before** relationship and the **Memory consistency model** provide guarantees to help you write thread-safe concurrent code.
  - Code before a thread start happens before the code in the thread.
  - Code in the thread happens before code after the join.
- A **future** represents a thread-safe future outcome of a computation.
- Most importantly, to make it easier to reason about your concurrent code, you should **isolate code that concurrently accesses a shared state**.

Equipped with a thorough understanding of concurrency's principles and advanced techniques, you're now better prepared to leverage its power in your programming endeavors.

## Next Steps

As you apply these principles to your projects, more complex scenarios and challenges may arise. Our next guide will delve into synchronization mechanisms and explore their potential pitfalls. Stay tuned for a deeper dive into the advanced realms of concurrency mastery!

## Terminology

**[concurrent code](https://en.wikipedia.org/wiki/Concurrent_computing)**&emsp; Code that utilize multiple threads to concurrently perform a computation.

**[data race](https://en.wikipedia.org/wiki/Race_condition#Data_race)**&emsp; Situations in concurrent programming where two or more threads access a shared resource simultaneously, and at least one modifies it, leading to unpredictable results.

**[future](https://en.wikipedia.org/wiki/Futures_and_promises)**&emsp; An object representing a result that is initially unknown but will become available at some point, typically used in asynchronous computations.

**[happened-before](https://en.wikipedia.org/wiki/Happened-before)**&emsp; A relationship in concurrent programming that guarantees memory visibility and ordering of operations across threads.

**[main thread](https://developer.android.com/guide/components/processes-and-threads)**&emsp; The primary thread in a program where execution starts and where UI interactions typically occur.

**[process](<https://en.wikipedia.org/wiki/Process_(computing)>)**&emsp; An instance of a computer program that is being executed, containing its code and its own memory space.

**[promise](https://en.wikipedia.org/wiki/Futures_and_promises)**&emsp; Objects similar to futures, often used in asynchronous programming to represent the eventual completion (or failure) of an operation.

**[race condition](https://en.wikipedia.org/wiki/Race_condition)**&emsp; Situations in concurrent programming where the system's behavior depends on the sequence or timing of uncontrollable events.

**sequential**&emsp; A programming model where operations are executed one after the other, in a predictable order.

**tearing**&emsp; A phenomenon in concurrent programming where a data race causes a variable to be partially updated, leading to inconsistent or corrupted data.

**[thread](<https://en.wikipedia.org/wiki/Thread_(computing)>)**&emsp; A path of execution within a process, allowing for parallel operation within a program.

**[thread safe](https://en.wikipedia.org/wiki/Thread_safety)**&emsp; Code that is safely executed or accessed in a multi-threaded environment without causing any problems like data corruption or unexpected behavior.
