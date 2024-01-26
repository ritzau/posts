package se.ritzau.posts.cs.concurrency.foundations

fun main() {
    val sayHello = { println("Hello, ${Thread.currentThread().id}") }

    val t = Thread(sayHello).also { it.start() }
    sayHello()

    t.join()
}
