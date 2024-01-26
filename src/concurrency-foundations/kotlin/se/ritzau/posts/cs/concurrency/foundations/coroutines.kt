package se.ritzau.posts.cs.concurrency.foundations

import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking

fun main() =
    runBlocking(Dispatchers.Default) {
        val f = { println("Hello, ${Thread.currentThread().id}") }
        launch { f() }
        f()
    }
