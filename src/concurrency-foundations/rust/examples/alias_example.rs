use std::thread;
use std::sync::Arc;

fn main() {
    let counter = Arc::new(0);

    let t = thread::spawn({
        let counter_alias = counter.clone();
        move || { *counter_alias + 42 }
    });

    let result = t.join().unwrap();
    println!("counter {} from the main thread!", result);
}
