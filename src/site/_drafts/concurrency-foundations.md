---
layout: post
title: "Concurrency Foundations"
date: 2024-01-01 14:24:26 +0100
author: Tobias Ritzau
categories: wip post blog cs concurrency essentials foundations
---

```
Totally missed talking about UI frameworks and techniques used in iOS and Android
```

This note aims to support developers who wants to add concurrency to their toolbelt. While concurrency can be challenging and is often tackled with some experience, enthusiastic junior engineers will also find it valuable, especially if you target mobile.

We will start by looking at what concurrency is and why you want to use it in your code. Then we will dive in and look at how to write concurrent code using a few different languages. After that we will take a closer look at the pitfalls of concurrency and how to avoid them using synchronization, only to see that now we opened up for potential deadlocks. We will end the note with some guides on how to avoid locking up your program. This knowledge is all you need to take advantage of concurrent code.

Note that the examples are written to fit all screens, meaning that the brevity may be beyond what I would write for production. For example, the C++ examples assumes that we are `using std`, which I would never recommend in general.

## Introducing Concurrency

A non-concurrent program executes operations in a single sequence, while a _concurrent_ program executes multiple sequences of operations concurrently. Sequences are typically started and stopped dynamically during the execution of the program. Example use include:

- Fetching data while rendering a user interface.
- Computing the next state of a game while rendering the current state.
- A web server that processes the requests of multiple clients at the same time.
- A GPU that runs shaders to compute color the pixels of a scene.

There are three main reasons to use concurrency:

- Efficient use of the hardware
- To simplify the division of logical components of a program
- To avoid blocking the UI thread.

When it comes to efficiency, the clock frequency of processors have not increased much the last ~20 years, while the number of processor cores is steadily increasing. To put the cores to use, the code needs to be concurrent. For a physical world comparison, consider a restaurant. A restaurant can be operated by a single person, but the orders will be taken faster and the food will be delivered faster with a staff of waiters; likewise the food will be cooked faster with a staff of chefs. Modern processors, even in mobile phones have multiple cores.

An example of the logical split is to implement a producer of data and the consumer of the data as two different components without the need to handle the scheduling of these components. In the restaurant case, it is far easier for a chef to follow separate recepies for the courses they cook, than to try combine all recepies into one set of instructions. The chef will schedule their work by switching between the recepies, and all the chefs works mostly independenly on the courses they prepare. Much like CPU cores execute sequences of operations.

Mobile and Web all use a main thread which is responsible for rendering the UI. That thread must not be blocked, or you will see glitches in responsiveness, or even have your program crash. Back to the restaurant, if the waiter and the chef is the same person, then guests would have a hard time getting response from the waiter while they are working in the kitchen. Instead, the waiters hand off to the chefs and can get back to the guests of the restaurant while the chefs continue their work.

It's worth noting that concurrency benefits aren't limited to multi-core processors. Even on single-core systems, cores often idle while waiting for I/O operations, like reading from storage, network communication, or awaiting user input. In these scenarios, concurrency enables the execution of other tasks during these idle periods, optimizing resource utilization. Just like a single chef can prepare several dishes at the same time.

It’s also worth noting that concurrency is not a tool that can be used to increase performance in all situations, as in the case of pregnancy: a woman can't get a baby in a month with the help of 9 men. Some things are inherently sequential, more cores will not speed those operations up, algorithms and better use of a single core is the way to optimize such code.

## Writing Concurrent Code

There are countless ways to write concurrent code. This section provides some examples of common approaches uses in popular runtimes.

### Threads

A straight forward approach available in most runtimes is the direct use of threads. A _thread_ simply represent a separate sequence of execution whithin a process. Threads within a processes share most things except CPU registers and the stack. Switching between threads happens by pausing the thread, swap the registers and the stack, and then resume the new thread.

When introduced, threads were considered very light weight, but the view on that has changed. The typical way to start a thread is to specify a function that should be invoked by the thread. When the function exits, the thread has run to completion. Note that a single thread, often called the 'main thread', is always available and executes the main function of your program. In the example below a new thread is created and launched concurrently with the main thread. Both threads then output their thread ID, before the main thread waits for the created thread to come to an end, at which point the threads can _join_.

{% include replit.md title="Example: C++ threads" href="concurrency-foundations-threads-cpp#main.cpp" %}

```cpp
auto sayHello = []
    { cout << "Hello, " << this_thread::get_id() << endl; };

auto t = thread(sayHello);
sayHello();

t.join();
```

### Executors

Threads are cheaper than processes but they don't scale well. Instead of mapping sequences of instructions to separate threads, the code can be split into executables that can be executed on existing threads in a pool. The abstraction is often called something along the lines of _Executors_ or _Thread queues_. A piece of code can be submitted or enqueued to be executed by an executor.

There are to main scenarios: either the code is split into chunks that doesn't block, meaning that the threads of the pool will never block if there is work to do; or a thread pool is used to limit the concurrent use of resources such as network connections. A special case occurs when a single thread is made available to a queue, this is called a _serial queue_ and is a powerful tool used to battle concurrency issues as described below. In advanced scenarios you can allow executors for different scenarios to share an underlying thread pool.

An example of that is presented in the Java example below.

{% include replit.md title="Example: Java executors" href="" %}

```java
Runnable sayHello = () ->
	System.out.println("Hello, " + Thread.currentThread().getId());

var executor = Executors.newFixedThreadPool(10);
var future = executor.submit(sayHello);
sayHello.run();

future.get();
executor.shutdown();
```

C++ introduced an abstraction for this in C++11 where a function can be asked to execute asynchronously. The execution policy determines how the code is executed, passing `std::launch::async` specifies that the function will be executed on a different thread.

{% include replit.md title="Example: C++ <kbd>async()</kbd>" href="" %}

```cpp
auto sayHello = []
    { cout << "Hello, " << this_thread::get_id() << endl; };

auto future = async(launch::async, sayHello);
sayHello();

future.get();
```

### Coroutines

Some languages take this a step further and splits the code up into non-blocking chunks automagically. It's really a best effort since a developer still can block threads, but if the developer sticks to asynchrnous operations, threads never need to block. In some case the thread pools are also abstracted away. The concept of _coroutines_ in Kotlin is an example of this. Coroutines have different interpretations in different languages, at the core a coroutine is a function that can yield control while executing, and resume it from where it left off.

Kotlin coroutines are designed to yield control instead of blocking. In theory, this makes it easy to write code that never blocks a thread and thus if you have a pool with the same number of threads as the number of cores, then you can use the CPU very efficiently. Note that Kotlin coroutines execute in a context. The context controls the execution and the default is a serial queue. By specifying the default dispatcher when launching a coroutine, you ensure that it can execute concurrently with other coroutines. In most cases you don't specify the dispatcher like this though. You have a context and only modify that when needed.

{% include replit.md title="Example: Kotlin coroutines" href="" %}

```kotlin
launch(Dispatchers.DEFAULT) {
	println("Hello, from ${Thread.currentThread.id}")
}

println("Hello, from ${Thread.currentThread.id}")
```

### Inter Process Concurrency

It should be noted that concurrency doesn’t only exist within a process. The code may very well run in different processes, or even on different hosts. Even if the best practices presented here still apply, we stick to talk about concurrency within a process in this post.

## Data Races and Race Conditions

When performing work concurrenctly we can end up with _race conditions_. A race condition occurs when the timing that is out of the control of the developer affects the outcome of a computation. That typically means that you will see different results when you run the code, that may be all fine, but it may also be a bug. Bugs caused by race conditions may be of the "one in a billion" kind and may be very hard to identify.

A _data race_ may happens when two or more threads access the same resource without synchronization and at least one is modifying it. The resource is typically a memory location. Data races can result in some threads not seeing the current state of the resource and may also overwrite it, but it in some situations it can also lead to _tearing_, meaning that the resource is only partially updated. A partially updated resource typically leads to corrupt data and as such, undefined behavior.

Imagine a race condition as a literal race on a freeway, where multiple cars (threads) vie for the same lane (resource). If no one enters or leaves the lane (writes to it) there is no race condition, but as soon as someone enters or leaves there we must prevent a race condition. All could be fine, but we may also end up in a crash. In software terms, a race condition occurs when multiple threads access the same resource, and at least one of them is writing to it. This can lead to inconsistency in the threads view of the data. Threads can even see teared data with data being partially overwritten. Detecting potential race conditions is crucial in both the coding and review phases.

The following code is an example of a data race. Depending on your setup, you may need to adjust the number of iterations to get the (un)desired outcome. Here we have two threads that read and update the `counter` variable. Given the nature of modern hardware architectures you should see that `counter` is not equal to `2 * n` given a large enough n.

```cpp
const auto n = 1000000;
auto counter = 0;

auto f = [] { for (auto i = 0; i < n; ++i) ++counter; };
auto t1 = thread(f);
auto t2 = thread(f);

t1.join();
t2.join();

cout << "Counter: " << counter << endl;
```

## Avoiding Data Racces

In the words of the great Mr. Miyagi, the best defense is not to be there. In many cases, it is possibly to avoid data races by breaking the invariant of a data race. A data race requires multiple threads to access the same resource without synchronization, while at least one is modifying the resource. Thus we can avoid the race by:

- Accessing the resource from a single thread
- Copying the data so that all threads have their own copy
- Ensure that only a single thread has access to the resource (move semantics)
- Let the threads work with immutable data
- Use thread safe data structures and/or atomics

Accessing resources from a single thread is common when it comes to UI frameworks. For example both Android, iOS, and Web only allow access to the UI hierarchy from the main thread. In other cases this may of course limit the possibilities of making use of concurrency, but often only the restriction on a single thread can be for short operations. A serial queue is commonly used to implement this.

For the other cases we focus on ensuring that the data can only be modified by a single thread. Copying the data may be enough for some cases, but you often want to communicate some result back, and then some thread is responsible to combine the outcomes of the other threads, possibly hierarchically. In the data race example above, each thread would increment their own counter and then communicate the result back, and the main thread would then sum up the results giving the expected answer.

Moving data is common in C++, Rust, and with Web Workers. This does not allow concurrent access, but rather ensures that only one thread **can** access a resource at any point in time.

Immutability is a powerful tool in runtimes that lacks value semantics. Value semantics means that when you store a value, you will get a copy, the same holds when you pass values to a function. If you use reference semantics you will instead pass a reference to the same data. This is called _aliasing_, one value may have multiple aliases. Though this is not a problem if no one modifies the value. You can often avoid concurrency issues by making data that should not change immutable.

Thread safe data structures and atomics is a topic for another note. For now, let's just say that an atomic type allows you to read and update variables atomically and it ensures that no data race can occur. The data race in the counter example can be fixed by simply making the counter atomic.

```cpp
const auto n = 1000000;
auto counter = atomic_int(0);

auto f = [] { for (auto i = 0; i < n; ++i) ++counter; };
auto t1 = thread(f);
auto t2 = thread(f);

t1.join();
t2.join();

cout << "Counter: " << counter << endl;
```

> A note on why this is not possible in Rust

So what is left? These techniques will take you far, but you may still end up in situations where they are not enough. They are enough to handle data races, but you can end up with data races when multiple resources needs to be updated atomically. Other examples include needing to conditionally wake up other threads but that is a topic for another note.

## Synchronization using Mutexes

Consider a bank transaction where funds are transfered from one account to another. You will need to remove funds from one account and add to another. This needs to be atomically otherwise someone may see a state where the total funds does not add up. In this case atomics are not sufficient, you could do the operation on a single thread using a queue, but you may also use a mutex. The Java Virtual Machine (JVM) is interesting in many ways, one being that all object instances has a mutex. The following Kotlin example will show you how a bank transfer can be implemted.

> Note that all access to the involved variables needs to be protected by the mutex

```kotlin
class Bank(var accountA: Int, var accountB: Int) {
  private val lock = object {}

  fun transfer(amount: Int) = synchronized(lock) {
    accountA -= amount
    accountB += amount
  }

  fun totalFunds(): Int = synchronized(lock) {
    return accountA + accountB
  }
}
```

> Example using Rust mutexes

## Deadlocks

> When two trains approach each other at a crossing, both shall come to a full stop and neither shall start up again until the other has gone.

**Deadlocks**: These are more complex. When a deadlock occurs, the involved threads freeze. Consider the Dining Philosophers Problem: a group of philosophers sit around a table, with a chopstick between each of them. A philosopher needs two sticks to eat but can only pick one at a time. If every philosopher picks up one stick and waits for the other, none can eat, leading to a deadlock. In coding, deadlocks occur when multiple processes wait on each other to release resources, creating a standstill.

## What Can We Do About It?

So, you need the efficiency, how can you use concurrency safely? As Mr. Miyagi wisely advised in karate, “The best defense is no be there.” This philosophy is just as applicable in managing race conditions.

**Avoiding Race Conditions**: The ideal approach is to design your system to avoid race conditions altogether. This can be done in two ways: by ensuring data is accessed by only one thread (common in UI frameworks like iOS, Android, and web applications) or by avoiding shared data altogether. In environments like Rust and with Web Workers, race conditions are circumvented by either making each thread work with its own copy of the data, eliminating races, or by transferring data ownership between threads, thus avoiding concurrent access. Fixme: Add section in thread safe data structures.

**Synchronization When Avoidance Isn’t Possible**: If avoiding race conditions isn't feasible, then the next step is to synchronize access to shared resources. The key here is isolation – keep the synchronized code as simple, brief, and isolated as possible, ideally in a separate file. This makes it easier to understand and reduces the risk of errors.

**Mutexes for Synchronization**: One common tool for synchronization is the Mutex (or lock). A mutex ensures that only one owner (be it a thread or a process) can access a resource at any given time. However, be cautious with recursive mutexes, as they can lead to a thread deadlocking itself if not handled correctly.

**Scoped Mutexes**: Some systems provide scoped mutexes which automatically unlock at the end of a scope. This is helpful as it prevents forgetting to release the lock, but be aware, it doesn't eliminate the risk of deadlocks.

**Avoiding Deadlocks**: Deadlocks occur when a process holding a resource is waiting for another resource, which is also being held. To simplify, try to never hold a lock while attempting to acquire another. If you must hold multiple locks, lock them all at once. And most crucially, avoid holding a lock while calling external code, such as callbacks, as this significantly increases the risk of a deadlock.

## Key Takeaways

- **Leveraging Concurrency for Efficiency**: Concurrency is a powerful tool to enhance the performance of your code, particularly on modern multi-core processors, allowing for parallel task execution.
- **Essential for UI Responsiveness**: In applications like mobile apps, concurrency is vital to prevent blocking the UI thread, ensuring a smooth and responsive user interface.
- **Diverse Techniques for Implementation**: Implementing concurrency can be achieved through various methods, including threads, executors/queues, and coroutines, each suitable for different scenarios and needs.
- **Navigating Race Conditions**: One of the primary challenges in concurrent programming is avoiding race conditions. These occur when multiple threads access the same data simultaneously, with at least one modifying it, potentially leading to inconsistent results.
- **Strategies to Prevent Race Conditions**:
  - Ensure exclusive data access by a single thread.
  - Use thread-safe data structures and atomic operations.
  - Employ data copying or handover strategies for thread exclusivity.
  - Embrace immutability where feasible.
- **The Role of Synchronization**: When necessary, synchronization, primarily through mutexes, can manage data access across threads but must be used judiciously to balance performance.
- **Deadlock Prevention**: A critical aspect of using synchronization is avoiding deadlocks, which can be achieved by avoiding holding one mutex while waiting for another and being cautious with callback invocations within locked contexts.

These insights offer a comprehensive overview of concurrency, its benefits, challenges, and strategies for safe and efficient implementation. Equipped with this knowledge, you're prepared to tackle concurrency in your coding projects with confidence.

vs.

- Concurrency can be used to increase the efficiency of your code.
- Sometimes concurrency is required to avoid blocking the UI thread.
- There are several techniques to write concurrent code: threads,
  executors/queues, and coroutines.
- Race conditions may occur if multiple threads access the same data, and at least one is writing.
- A simple strategy to avoid race conditions is to ensure that these conditions
  are not met.
  - Only allow access from a single thread
  - Copy data to give each thread its own copy
  - Hand over data so that only one thread can access it at any given time
  - Use immutable data.
  - Use thread safe data structures (including atomics).
- Sometimes performance requires synchronization
- Mutexes can be used to synchronize data access
- Synchronization can cause deadlocks
- Deadlocks can be avoided if you never hold a Mutex while waiting for another.
- Most importantly, never invoke a callback with a locked mutex.

## Where to go from here?

As we've explored in "Concurrency 101," the efficient implementation of concurrency is crucial in modern software development, offering significant performance enhancements. However, it's equally important to be mindful of the pitfalls that come with it, namely race conditions and deadlocks. Here are the key takeaways:

- **Concurrency for Efficiency**: Just like multiple lanes on a freeway, concurrency allows your software to perform multiple operations in parallel, maximizing the use of modern multi-core processors.
- **Avoid Race Conditions**: The best strategy to handle race conditions is to avoid them altogether, either by limiting data access to a single thread or by ensuring no shared data is accessed concurrently.
- **Effective Synchronization**: When avoidance isn’t possible, careful synchronization is key. Isolate synchronized code, keep it simple, and use tools like mutexes wisely.
- **Beware of Deadlocks**: Always be cautious of deadlocks, especially when using locks. Avoid holding multiple locks if possible, and never hold a lock while calling external code.

As you begin to integrate these principles into your projects, you'll likely encounter more complex scenarios and challenges. In the next section, "Advanced Concurrency Concepts," we'll dive deeper into sophisticated synchronization mechanisms, explore language-specific features, and discuss how to navigate the intricacies of concurrent programming in more complex environments. Stay tuned as we unlock the next level of concurrency mastery!

## Terminology

- Concurrency
- Data race
- Race condtition
- Thread
