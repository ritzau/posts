---
layout: post
title: "Concurrency: Synchronization"
date: 2024-01-27 14:24:26 +0100
author: Tobias Ritzau
categories: wip post blog cs concurrency synchronization deadlocks
---

Welcome to the next step in our journey through concurrent programming. Following up on the [Introduction to Concurrency]() post, which laid the groundwork by discussing threads, task queues, thread safety, and the happens-before principle, we're now set to dive deeper.

This guide will focus on scenarios where fine-grained control is essential. In the introduction, we learned that a data race might occur if multiple threads access the same resource, with at least one thread modifying it, all without proper synchronization. We addressed this by sharing immutable data or allowing each thread to work on its own data copies. This time we will make our code thread-safe by addressing synchronization.

Recall the happens-before rules:

- Operations before a thread's initiation are visible within the thread.
- Actions within a thread occur before the thread has been successfully joined.

We also touched on futures, a means for asynchronously and thread-safely passing data between threads.

Now, we shift our focus to synchronizing threads and their access to shared data. This approach enables us to read and modify data from different threads but introduces the risk of deadlocks. I'll explain what deadlocks are, how they occur, and strategies to avoid them.

A quick heads-up: the examples are concise and optimized for readability across all devices. The level of brevity here is greater than what I'd recommend for production code. Strive for readability in production code. All code in this guide is also available [online](https://github.com/ritzau/posts), with instructions for building and running it. The repository code is better formatted but primarily serves to explain the examples, not for production use.

## When to synchronize

## Mutex

### Foo

**Synchronization When Avoidance Isn’t Possible**: If avoiding race conditions isn't feasible, then the next step is to synchronize access to shared resources. The key here is isolation – keep the synchronized code as simple, brief, and isolated as possible, ideally in a separate file. This makes it easier to understand and reduces the risk of errors.

**Mutexes for Synchronization**: One common tool for synchronization is the Mutex (or lock). A mutex ensures that only one owner (be it a thread or a process) can access a resource at any given time. However, be cautious with recursive mutexes, as they can lead to a thread deadlocking itself if not handled correctly.

**Scoped Mutexes**: Some systems provide scoped mutexes which automatically unlock at the end of a scope. This is helpful as it prevents forgetting to release the lock, but be aware, it doesn't eliminate the risk of deadlocks.

### Bar

So what is left? These techniques mentioned above all avoids the data race and those will take you far, but you may still end up in situations where they are not enough. They are enough to handle data races, but you can end up with data races when multiple resources needs to be updated atomically. Other examples include needing to conditionally wake up other threads but that is a topic for another note.

Consider a bank transaction where funds are transfered from one account to another. You will need to remove funds from one account and add to another. This needs to be atomically otherwise someone may see a state where the total funds does not add up. In this case atomics are not sufficient, you could do the operation on a single thread using a queue, but you may also use a mutex. The Java Virtual Machine (JVM) is interesting in many ways, one being that all object instances has a mutex. The following Kotlin example will show you how a bank transfer can be implemted.

It is **very** important to note that **all** access to involved variables needs to be synchronized using the same mutex. That means that, for example, read access also needs to by synchronized to be thread safe.

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

We mentioned above that using the atomic reference counted references in Rust, the data access is by default immutable. Using a mutex will allow us to mutate the data. The code is similar to before, but now we keep a `Mutex` in the Arc. To access the data within, we need to lock the mutex which ensures that there is no data race. Note that the lock is held during the lifetime of the returned reference.

```rust
let counter = Arc::new(Mutex::new(0));

let t = thread::spawn({
    let counter_alias = counter.clone();
    move || {
        let mut c = counter_alias.lock().unwrap();
        *c += 42;
    }
});

t.join().unwrap();
let result = counter.lock().unwrap();
println!("counter {} from the main thread!", result);
```

## Deadlock

> When two trains approach each other at a crossing, both shall come to a full stop and neither shall start up again until the other has gone.

**Deadlocks**: These are more complex. When a deadlock occurs, the involved threads freeze. Consider the Dining Philosophers Problem: a group of philosophers sit around a table, with a chopstick between each of them. A philosopher needs two sticks to eat but can only pick one at a time. If every philosopher picks up one stick and waits for the other, none can eat, leading to a deadlock. In coding, deadlocks occur when multiple processes wait on each other to release resources, creating a standstill.

## Deadlock prevension

**Avoiding Deadlocks**: Deadlocks occur when a process holding a resource is waiting for another resource, which is also being held. To simplify, try to never hold a lock while attempting to acquire another. If you must hold multiple locks, lock them all at once. And most crucially, avoid holding a lock while calling external code, such as callbacks, as this significantly increases the risk of a deadlock.

## Key takeaways

## Next steps

## Terminology
