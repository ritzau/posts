package se.ritzau.posts.cs.concurrency.foundations;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.Executors;

class Threads {
    public static void main(String[] args) throws InterruptedException, ExecutionException {
        Runnable f = () -> System.out.println("Hello, " + Thread.currentThread().getId());

        var executor = Executors.newFixedThreadPool(10);
        var future = executor.submit(f);
        f.run();
        future.get();
        executor.shutdown();
    }
}
