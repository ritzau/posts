use std::thread;

fn main() {
    let f = || { println!("Hello, {:?}", thread::current().id()); };
    let t = thread::spawn(f);
    f();
    t.join().unwrap();
}
