use std::sync::{Arc, Mutex};
use std::thread;

fn main() {
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
}
