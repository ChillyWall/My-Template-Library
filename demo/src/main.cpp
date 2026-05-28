import std;
import mtl;
import ts_mtl.stack;

int main() {
    // mtl::vector — single-threaded container
    mtl::vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);

    std::println("mtl::vector contains {} elements:", v.size());
    for (auto x : v) {
        std::print("{} ", x);
    }
    std::println("");

    // mtl::ts_stack — thread-safe stack
    mtl::ts_stack<int> s;
    s.push(10);
    s.push(20);
    s.push(30);

    std::println("ts_stack has {} elements", s.size());

    int val;
    s.pop(val);
    std::println("popped: {}", val);
    s.pop(val);
    std::println("popped: {}", val);
    s.pop(val);
    std::println("popped: {}", val);
    std::println("ts_stack empty: {}", s.empty());

    return 0;
}
