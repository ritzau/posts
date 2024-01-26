package se.ritzau.posts.cs.concurrency.foundations

import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.coroutineScope
import kotlinx.coroutines.launch
import kotlinx.coroutines.newSingleThreadContext
import kotlinx.coroutines.runBlocking
import kotlinx.coroutines.withContext
import kotlin.system.measureTimeMillis

fun main() = runBlocking {
    val serialDispatcher = Dispatchers.Default.limitedParallelism(1)
    var counter = 0

    withContext(Dispatchers.Default) {
        repeat(10) {
            launch {
                repeat(100_000) {
                    withContext(serialDispatcher) {
                        counter++
                    }
                }
            }
        }
    }

    println("Counter = $counter")
}
