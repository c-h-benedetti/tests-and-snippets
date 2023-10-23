#include <iostream>
#include <utility>
#include <stdexcept>
#include <vector>
#include <memory>

class unsupported_operation : public std::logic_error {
public:
    unsupported_operation(const std::string& message)
        : std::logic_error(message) {}
};

struct Bucket {
    int index = 0;

    Bucket() = default;
    Bucket(int i) : index(i) {}
};

std::ostream& operator<<(std::ostream& o, const Bucket& b) {
    o << b.index;
    return o;
}

class Delegator;

class A {
public:
    virtual int run(Delegator* o, Bucket b) = 0;
    virtual int dtype() = 0;
};


class B : public A {
public:
    int run(Delegator* o, Bucket b) override;
    int dtype() override { return 1; }
};


class C : public A {
public:
    int run(Delegator* o, Bucket b) override;
    int dtype() override { return 2; }
};


struct Delegator {

    virtual int run(A* a, Bucket k) { throw unsupported_operation("Not implemented for class A."); }
    virtual int run(B* b, Bucket k) { throw unsupported_operation("Not implemented for class B."); }
    virtual int run(C* c, Bucket k) { throw unsupported_operation("Not implemented for class C."); }
    
    virtual int execute(Bucket b) = 0; // return target->run(this);
};


class ThreadsPool {

public:

    Delegator* task;
    std::vector<Bucket> buckets;
    static std::unique_ptr<ThreadsPool> instance;

    static ThreadsPool& threadspool() { return *(ThreadsPool::instance.get()); }

    void execute_task() {
        for (Bucket& b : buckets) {
            this->task->execute(b);
        }
    }

public:

    void delegate(Delegator* d, std::vector<Bucket> b) {
        this->task = d;
        this->buckets = b;
        this->execute_task();
    }
};

std::unique_ptr<ThreadsPool> ThreadsPool::instance = std::unique_ptr<ThreadsPool>(new ThreadsPool);

int B::run(Delegator* o, Bucket b) {
    return o->run(this, b);
}

int C::run(Delegator* o, Bucket b) {
    return o->run(this, b);
}



class Threshold {

public:

    A* target;
    std::pair<float, float> bounds;

    struct ApplyThreshold : public Delegator {
        float t;
        A* target;

        int run(B* b, Bucket k) override {
            t = 3.0f;
            return static_cast<int>(b->dtype());
        }

        int run(C* c, Bucket k) override {
            t = 4.0f;
            return static_cast<int>(c->dtype());
        }

        int execute(Bucket b) override { return target->run(this, b); }
    };

    struct ProcessBounds : public Delegator {
        std::pair<float, float> bounds;
        A* target;

        int run(A* a, Bucket k) override {
            bounds = {0, 1};
            return static_cast<int>(a->dtype());
        }

        int run(B* b, Bucket k) override {
            bounds = {2, 3};
            return static_cast<int>(b->dtype());
        }

        int run(C* c, Bucket k) override {
            bounds = {4, 5};
            return static_cast<int>(c->dtype());
        }

        int execute(Bucket b) override { 
            std::cout << b << std::endl;
            return target->run(this, b); 
        }
    };

public:

    Threshold(A* a): target(a), bounds({0.0f, 0.0f}) {}

    std::pair<float, float> process_bounds() {
        ProcessBounds pb;
        pb.target = target;
        std::vector<Bucket> buckets = {Bucket(1), Bucket(2), Bucket(3), Bucket(4), Bucket(5)};
        ThreadsPool::threadspool().delegate(&pb, buckets);
        bounds = pb.bounds;
        return bounds;
    }

    float apply_threshold() {
        ApplyThreshold at;
        at.target = target;
        std::vector<Bucket> buckets = {Bucket(6), Bucket(7), Bucket(8), Bucket(9), Bucket(10)};
        ThreadsPool::threadspool().delegate(&at, buckets);
        return at.t;
    }
};



int main(int argc, char* argv[], char* env[]) {

    B b;
    C c;

    Threshold t1(static_cast<A*>(&b));
    Threshold t2(static_cast<A*>(&c));

    std::pair<float, float> bounds1 = t1.process_bounds();
    std::pair<float, float> bounds2 = t2.process_bounds();

    std::cout << '{' << bounds1.first << ", " << bounds1.second << '}' << std::endl;
    std::cout << '{' << bounds2.first << ", " << bounds2.second << '}' << std::endl;

    std::cout << t1.apply_threshold() << std::endl;
    std::cout << t2.apply_threshold() << std::endl;

    return 0;
}
