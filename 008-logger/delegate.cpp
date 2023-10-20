#include <iostream>
#include <utility>
#include <stdexcept>

class unsupported_operation : public std::logic_error {
public:
    unsupported_operation(const std::string& message)
        : std::logic_error(message) {}
};

class Delegator;

class A {
public:
    virtual int run(Delegator* o) = 0;
    virtual int dtype() = 0;
};


class B : public A {
public:
    int run(Delegator* o) override;
    int dtype() override { return 1; }
};


class C : public A {
public:
    int run(Delegator* o) override;
    int dtype() override { return 2; }
};


struct Delegator {
    virtual int run(A* a) { throw unsupported_operation("Not implemented for class A."); }
    virtual int run(B* b) { throw unsupported_operation("Not implemented for class B."); }
    virtual int run(C* c) { throw unsupported_operation("Not implemented for class C."); }
    
    int operator()(A* t) { 
        return t->run(this);
    }
};


int B::run(Delegator* o) {
    return o->run(this);
}

int C::run(Delegator* o) {
    return o->run(this);
}



class Threshold {

    A* target;
    std::pair<float, float> bounds;

private:

    struct ApplyThreshold : public Delegator {
        float t;

        int run(B* b) override {
            t = 3.0f;
            return static_cast<int>(b->dtype());
        }

        int run(C* c) override {
            t = 4.0f;
            return static_cast<int>(c->dtype());
        }
    };

    struct ProcessBounds : public Delegator {
        std::pair<float, float> bounds;

        int run(A* a) override {
            bounds = {0, 1};
            return static_cast<int>(a->dtype());
        }

        int run(B* b) override {
            bounds = {2, 3};
            return static_cast<int>(b->dtype());
        }

        int run(C* c) override {
            bounds = {4, 5};
            return static_cast<int>(c->dtype());
        }
    };

public:

    Threshold(A* a): target(a), bounds({0.0f, 0.0f}) {}

    std::pair<float, float> process_bounds() {
        ProcessBounds pb;
        pb(target);
        bounds = pb.bounds;
        return bounds;
    }

    float apply_threshold() {
        ApplyThreshold at;
        at(target);
        return at.t;
    }
};



int main(int argc, char* argv[], char* env[]) {

    B b;
    C c;

    Threshold t1(static_cast<A*>(&b));
    Threshold t2(static_cast<A*>(&c));

    std::cout << '{' << t1.process_bounds().first << ", " << t1.process_bounds().second << '}' << std::endl;
    std::cout << '{' << t2.process_bounds().first << ", " << t2.process_bounds().second << '}' << std::endl;

    std::cout << t1.apply_threshold() << std::endl;
    std::cout << t2.apply_threshold() << std::endl;

    return 0;
}
