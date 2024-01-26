use crossbeam::thread;

fn main() {
    let mut var = vec![1, 2, 3];
    var[0] = 2;

    thread::scope(|s| {
        var[0] = 3;

        s.spawn(|_| {
            println!("A child thread borrowing `var`: {:?}", var);
        });

        s.spawn(|_| {
            println!("A child thread borrowing `var`: {:?}", var);
        });
    }).unwrap();
}
