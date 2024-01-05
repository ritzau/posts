use std::thread;

fn main() {
    const N: i32 = 1_000_000;
    let mut counter = Box::new(0);

    let t1 = thread::spawn({
        let icounter = unsafe { &mut *counter };
        move || {
            for _ in 0..N {
                *icounter += 1; // This will create a race condition
            }
        }
    });

    let t2 = thread::spawn({
        let icounter = unsafe { &mut *counter };
        move || {
            for _ in 0..N {
                *icounter -= 1; // This will create a race condition
            }
        }
    });

    t1.join().unwrap();
    t2.join().unwrap();

    // Unsafe: accessing the boxed i32 without synchronization
    let result = unsafe { *counter };

    println!("counter {} from the main thread!", result);
}
