#include <iostream>
#include <functional>


class SomeClass final {

    int a;
    int b;
    std::function<int()> op;

public:

    SomeClass() = delete;
    SomeClass(int x, int y): a(x), b(y) {
        this->setOp(a);
    }

    void setOp(int k) {
        switch (k) {
            case (0):
                op = [this](){ return this->a - this->b; };
                break;
            case (1):
                op = [this](){ return this->a + this->b; };
                break;
            default:
                op = [this](){ return this->a * this->b; };
        };
    }

    int someMethod() const {
        return this->op();
    }
};

int someFunction(int a, const int b) {
    return a + b;
}

int main(int argc, char* argv[], char* env[]) {

    /// [init_capture]
    int x = 2, y = 3;
    auto l1 = [z = x + y](){ std::cout << z << std::endl; };
    l1(); // 5
    x = 4;
    y = 5;
    l1(); // 5
    /// [init_capture]

    SomeClass sc(0, 2);
    std::cout << sc.someMethod() << std::endl; // -2
    sc.setOp(1);
    std::cout << sc.someMethod() << std::endl; // 2

    return 0;
}
