use std::sync::{Arc, Mutex};
use std::thread;

#[allow(dead_code)]
pub fn thread_example() {
    let f = || { println!("Hello, {:?}", thread::current().id()); };
    let t = thread::spawn(f);
    f();
    t.join().unwrap();
}

#[allow(dead_code)]
pub fn mutex_example() {
    const N: i32 = 1_000_000;
    let counter = Arc::new(Mutex::new(0));

    let t1 = thread::spawn({
        let icounter = Arc::clone(&counter);
        move || {
            for _ in 0..N {
                let mut foo = icounter.lock().unwrap();
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

#[allow(dead_code)]
fn data_race_attempt() {
    // const N: i32 = 1_000_000;
    // let mut counter = Box::new(0);

    // let t1 = thread::spawn({
    //     let icounter = unsafe { &mut *counter };
    //     move || {
    //         for _ in 0..N {
    //             *icounter += 1; // This will create a race condition
    //         }
    //     }
    // });

    // let t2 = thread::spawn({
    //     let icounter = unsafe { &mut *counter };
    //     move || {
    //         for _ in 0..N {
    //             *icounter -= 1; // This will create a race condition
    //         }
    //     }
    // });

    // t1.join().unwrap();
    // t2.join().unwrap();

    // // Unsafe: accessing the boxed i32 without synchronization
    // let result = unsafe { *counter };

    // println!("counter {} from the main thread!", result);
}
