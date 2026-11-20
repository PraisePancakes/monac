export module Math;
import module Test;
import module Another;
@define X 4

func sealed(mut x: i32, y: i32) {
    x += 2;
    return x + y;
}

export func add(x: i32, y: i32) -> i32 {
    return sealed(X, y);
};