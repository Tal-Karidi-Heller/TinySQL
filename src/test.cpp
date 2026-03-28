//
// Created by Tal Karidi on 28/03/2026.
//

#include <memory>
#include <iostream>

class A {
public:
    virtual ~A() = default;

    virtual bool is_0() = 0;
    virtual bool is_10() = 0;
    virtual void add_1() = 0;
    virtual void print_r() = 0;
};

class A1 : public A {
    int r;
public:
    A1(): r(0) {}
    bool is_0() override {
        return r == 0;
    }
    bool is_10() override {
        return r == 10;
    }
    void add_1() override {
        r += 1;
    }
    void print_r() override {
        std::cout << r << std::endl;
    }
};

std::shared_ptr<A> func(std::shared_ptr<A> obj) {
    std::cout << "was called" << std::endl;
    if (!obj->is_10()) {
        std::cout << "adding 1" << std::endl;
        obj->add_1();
        func(obj);
    }

    return obj;
}

int main() {
    auto a1 = std::make_shared<A1>(A1());
    func(a1);


    A1 &a1_ = *a1;
    a1_.add_1();
    std::cout << "shared a1: ";
    a1->print_r();
    std::cout << "a1_: ";
    a1_.print_r();
    return 0;
}