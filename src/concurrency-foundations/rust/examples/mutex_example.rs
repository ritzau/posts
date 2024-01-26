use std::sync::{Arc, Mutex};
use std::thread;

fn main() {
    const N: i32 = 1_000_000;
    let counter: Arc<Mutex<i32>> = Arc::new(Mutex::new(0));

    let t1 = thread::spawn({
        let icounter = Arc::clone(&counter);
        move || {
            for _ in 0..N {
                let mut foo: std::sync::MutexGuard<'_, i32> = icounter.lock().unwrap();
                *foo += 1;
            }
        }
    });

    let t2 = thread::spawn({
        let icounter = Arc::clone(&counter);
        move || {
            for _ in 0..N {
                let mut foo = icounter.lock().unwrap();
                *foo -= 1;
            }
        }
    });

    t1.join().unwrap();
    t2.join().unwrap();

    let result = counter.lock().unwrap();
    println!("counter {} from the main thread!", *result);

}
