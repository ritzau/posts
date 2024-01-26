use std::sync::Arc;
use std::thread;

fn main() {
    let numbers = Arc::new(vec![1.0, 2.0, 3.0]);

    let handles: Vec<_> = (0..3).map(|i| {
        let numbers = Arc::clone(&numbers);
        thread::spawn(move || {
            println!("{:?}", numbers[i]);
        })
    }).collect();

    for handle in handles {
        handle.join().unwrap();
    }
}
