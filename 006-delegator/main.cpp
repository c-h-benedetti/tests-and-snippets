#include <iostream>
#include <exception>

/**
 * Dans l'implémentation suivante:
 *   - La classe A serait la classe Data, dont tous les types héritent.
 *   - Les classes B, C et D seraient des types de données dérivés de A. Ils représentent des images concrètes et instanciables.
 *   - La classe E suit une hiérarchie tout à fait indépendante. Cette classe pourrait symboliser un autre composant de VG qui est malgré tout soumis aux actions.
 *   - La classe Task représente une action à effectuer sur les données. Elle est paramétrée par le type de données sur lequel elle doit s'appliquer. Tous les types de données ne sont pas acceptés. Toutes les tâches devront hériter de cette classe.
*/

struct Task;

class A {

protected:

    char identity;

    A(char d) : identity(d) {
        std::cout << "A()" << std::endl;
    }

    virtual ~A() {
        std::cout << "~A()" << std::endl;
    }

};


struct Task {
    virtual int run(A* data) = 0;
};


class B : public A {

public:

    B() : A('B') {
        std::cout << "B()" << std::endl;
    }

    ~B() override {
        std::cout << "~B()" << std::endl;
    }
};

class C : public A {

public:

    C() : A('C') {
        std::cout << "C()" << std::endl;
    }

    ~C() override {
        std::cout << "~C()" << std::endl;
    }
};

class D : public A {

public:

    D() : A('D') {
        std::cout << "D()" << std::endl;
    }

    ~D() override {
        std::cout << "~D()" << std::endl;
    }
};

// Independant hierarchy
class E {

public:

    E() {
        std::cout << "E()" << std::endl;
    }

    virtual ~E() {
        std::cout << "~E()" << std::endl;
    }
};


struct TrueTask : public Task {
    template <typename T>
    int run();

    int run(A* data) override {
        return this->run_impl(this, data);
    }
private:
    int run_impl(TrueTask* task, B* data) {
        return task->run<B>();
    }
    int run_impl(TrueTask* task, C* data) {
        return task->run<C>();
    }
    int run_impl(TrueTask* task, D* data) {
        return task->run<D>();
    }
};

template <>
int TrueTask::run<B>() {
    std::cout << "TrueTask::run<B>()" << std::endl;
    return 0;
}

template <>
int TrueTask::run<C>() {
    std::cout << "TrueTask::run<C>()" << std::endl;
    return 0;
}

template <>
int TrueTask::run<D>() {
    std::cout << "TrueTask::run<D>()" << std::endl;
    return 0;
}


int main(int argc, char* argv[], char* env[]) {
    
    B b;
    TrueTask t;
    b.run(&t);

    return 0;
}

// Le worker 1 doit être capable d'accepter tous les types, sauf E. Le worker 2 fait l'inverse.