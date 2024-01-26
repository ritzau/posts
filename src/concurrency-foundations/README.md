---
layout: post
title:  "Concurrency Foundations"
date:   2024-01-01 14:24:26 +0100
categories: wip post blog cs concurrency essentials foundations
---

**tl;dr**: Concurrency, the ability to run multiple sequences of operations in parallel, is a great tool to improve the efficiency of your code. But with great power comes great responsibility; concurrency introduces its own set of challenges. In this post you will learn all you need to write efficient and hassle free concurrent code.

To get started we take a look at some basic techniques to write concurrent code using **Kotlin and C++**,  after which we take a look at data races and race conditions which both represents the primary issues that can be caused by concurrency. A data race is an unpredictability caused by the timing of events that are out of control of the developer, and they can easily cause race conditions which may in turn cause concurrent code to have inconsistent view of the state of the program. For a race condition to occur, concurrent code needs to access the same data, and at least one thread needs to update the data.

The easiest way to avoid a race condition is to ensure that these conditions are not met. This can typically be done by only allow data to be accessed from a single thread, to use thread safe data structures, or to disallow write access to the data.

However, sometimes you need to squeeze more efficiency out of the code and in some cases you can do this by using synchronization mechanisms that allow concurrent code to read and update the state of your program. In this post we will be looking at a mechanism called Mutex. It is the most common synchronization mechanism and it will cover your needs for the vast majority of cases.

But synchronization comes at a cost. Most importantly, synchronization may cause deadlocks, and you can’t use synchronization without understanding how deadlocks arise. You will learn a strategy that guarantees that your code will be deadlock free, simply by avoiding to hold a Mutex while waiting for another.

By learning the best practices presented here, you will have everything you need to write concurrent code.

## The good bits of Concurrency
Consider a freeway with multiple lanes: each lane allows additional cars to travel in parallel, increasing the road's overall throughput. Similarly, a grocery store with several cashiers or a restaurant kitchen with multiple chefs efficiently handles more customers simultaneously. These everyday examples mirror the concept of concurrency in software development.
In the realm of coding, concurrency is a powerful tool for enhancing efficiency. Modern processors have transitioned from higher clock speeds to boasting more cores. To fully harness the power of these multi-core processors, your code needs to be structured to execute tasks concurrently. For instance, a web service can be designed to handle multiple requests in parallel, or a game engine might calculate the next state of the game while rendering the current scene.
It's worth noting that concurrency benefits aren't limited to multi-core processors. Even on single-core systems, cores often idle while waiting for I/O operations, like reading from storage, network communication, or awaiting user input. In these scenarios, concurrency enables the execution of other tasks during these idle periods, optimizing resource utilization. When it comes to multi-core processors, this advantage is even more pronounced, as they cannot be fully utilized without concurrent task execution.
It’s also worth noting that concurrency is not a tool that can be used to increase performance in all situations, as in the case of getting 9 women pregnant won’t get you a baby in a month. Some operations needs to happen in sequence and in those situations more cores doesn’t increase performance.

## How to Write Concurrent Code

There are countless ways to write concurrent code. This section provides some examples of common approaches uses in popular runtimes.

### Threads

{% include replit.md href="concurrency-foundations-threads-cpp#main.cpp" %}
```cpp
auto f = []
    { cout << "Hello, " << this_thread::get_id() << endl; };

auto t = thread(f);
f();
t.join();
```

### Executors/queues
```java
Runnable f = () ->
	System.out.println("Hello, " + Thread.currentThread().getId());

var executor = Executors.newFixedThreadPool(10);
var future = executor.submit(f);
f.run();
future.get();
executor.shutdown();
```
* Serial

```cpp
auto f = []
    { cout << "Hello, " << this_thread::get_id() << endl; };

auto future = async(f);
f();
future.get();
```

### Coroutines
```kotlin
launch {
	println("Hello, from ${Thread.currentThread.id}")
}

println("Hello, from ${Thread.currentThread.id}")
```

### Inter Process Concurrency
It should be noted that concurrency doesn’t only exist within a process. The code may very well run in separate processes, or even on different hosts. Even if the best practices presented here still apply, we stick to talk about concurrency within a process in this post.
## Race Conditions
Concurrency, while a powerful tool for efficiency, comes with its pitfalls. The two primary issues to watch out for are race conditions and deadlocks.
**Race Conditions**: Imagine a race condition as a literal race on a freeway, where multiple cars (threads) vie for the same lane (resource). If no one enters or leaves the lane (writes to it) there is no race condition, but as soon as someone enters or leaves there we must prevent a race condition. All could be fine, but we may also end up in a crash. In software terms, a race condition occurs when multiple threads access the same resource, and at least one of them is writing to it. This can lead to inconsistency in the threads view of the data. Threads can even see teared data with data being partially overwritten. Detecting potential race conditions is crucial in both the coding and review phases.
## Avoiding Race Conditions
* Access from a single thread
  * All access from a single thread
  * Copy data (also mention thread locals)
  * Move data
* Immutability
* Thread safe data structures and atomics
## Synchronization using Mutexes
## Deadlocks
**Deadlocks**: These are more complex. When a deadlock occurs, the involved threads freeze. Consider the Dining Philosophers Problem: a group of philosophers sit around a table, with a chopstick between each of them. A philosopher needs two sticks to eat but can only pick one at a time. If every philosopher picks up one stick and waits for the other, none can eat, leading to a deadlock. In coding, deadlocks occur when multiple processes wait on each other to release resources, creating a standstill.
## What Can We Do About It?
So, you need the efficiency, how can you use concurrency safely? As Mr. Miyagi wisely advised in karate, “The best defense is no be there.” This philosophy is just as applicable in managing race conditions.
**Avoiding Race Conditions**: The ideal approach is to design your system to avoid race conditions altogether. This can be done in two ways: by ensuring data is accessed by only one thread (common in UI frameworks like iOS, Android, and web applications) or by avoiding shared data altogether. In environments like Rust and with Web Workers, race conditions are circumvented by either making each thread work with its own copy of the data, eliminating races, or by transferring data ownership between threads, thus avoiding concurrent access.
Fixme: Add section in thread safe data structures.
**Synchronization When Avoidance Isn’t Possible**: If avoiding race conditions isn't feasible, then the next step is to synchronize access to shared resources. The key here is isolation – keep the synchronized code as simple, brief, and isolated as possible, ideally in a separate file. This makes it easier to understand and reduces the risk of errors.
**Mutexes for Synchronization**: One common tool for synchronization is the Mutex (or lock). A mutex ensures that only one owner (be it a thread or a process) can access a resource at any given time. However, be cautious with recursive mutexes, as they can lead to a thread deadlocking itself if not handled correctly.
**Scoped Mutexes**: Some systems provide scoped mutexes which automatically unlock at the end of a scope. This is helpful as it prevents forgetting to release the lock, but be aware, it doesn't eliminate the risk of deadlocks.
**Avoiding Deadlocks**: Deadlocks occur when a process holding a resource is waiting for another resource, which is also being held. To simplify, try to never hold a lock while attempting to acquire another. If you must hold multiple locks, lock them all at once. And most crucially, avoid holding a lock while calling external code, such as callbacks, as this significantly increases the risk of a deadlock.
## Conclusions
* Concurrency can be used to increase the efficiency of your code.
* Sometimes concurrency is required to avoid blocking the UI thread.
* There are several techniques to write concurrent code: threads, executors/queues, and coroutines.
* Race conditions may occur if multiple threads access the same data, and at least one is writing.
* A simple strategy to avoid race conditions is to ensure that these conditions are not met.
  * Only allow access from a single thread
  * Copy data to give each thread its own copy
  * Hand over data so that only one thread can access it at any given time
  * Use immutable data.
  * Use thread safe data structures (including atomics).
* Sometimes performance requires synchronization
* Mutexes can be used to synchronize data access
* Synchronization can cause deadlocks
* Deadlocks can be avoided if you never hold a Mutex while waiting for another.
* Most importantly, never invoke a callback with a locked mutex.
## Where to go from here?
As we've explored in "Concurrency 101," the efficient implementation of concurrency is crucial in modern software development, offering significant performance enhancements. However, it's equally important to be mindful of the pitfalls that come with it, namely race conditions and deadlocks. Here are the key takeaways:
* **Concurrency for Efficiency**: Just like multiple lanes on a freeway, concurrency allows your software to perform multiple operations in parallel, maximizing the use of modern multi-core processors.
* **Avoid Race Conditions**: The best strategy to handle race conditions is to avoid them altogether, either by limiting data access to a single thread or by ensuring no shared data is accessed concurrently.
* **Effective Synchronization**: When avoidance isn’t possible, careful synchronization is key. Isolate synchronized code, keep it simple, and use tools like mutexes wisely.
* **Beware of Deadlocks**: Always be cautious of deadlocks, especially when using locks. Avoid holding multiple locks if possible, and never hold a lock while calling external code.

As you begin to integrate these principles into your projects, you'll likely encounter more complex scenarios and challenges. In the next section, "Advanced Concurrency Concepts," we'll dive deeper into sophisticated synchronization mechanisms, explore language-specific features, and discuss how to navigate the intricacies of concurrent programming in more complex environments.
Stay tuned as we unlock the next level of concurrency mastery!
