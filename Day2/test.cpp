#include <iostream>
#include <cassert>
#include "UniquePtr.h"

struct Counter {
    static int alive;
    int value;

    explicit Counter(int v) : value(v) {
        ++alive;
    }
    ~Counter() {
        --alive;
    }
};

int Counter::alive = 0;

int main() {
    std::cout << "Running UniquePtr tests...\n";

    // Test 1: 默认构造
    {
        UniquePtr<int> p;
        assert(!p);
        assert(p.get() == nullptr);
    }

    // Test 2: 接管裸指针 + 解引用
    {
        UniquePtr<int> p(new int(42));
        assert(p);
        assert(*p == 42);
        assert(p.get() != nullptr);
    }

    // Test 3: operator->()
    {
        UniquePtr<Counter> p(new Counter(7));
        assert(p->value == 7);
        assert(Counter::alive == 1);
    }
    assert(Counter::alive == 0);

    // Test 4: 移动构造
    {
        UniquePtr<int> p1(new int(10));
        UniquePtr<int> p2(std::move(p1));

        assert(!p1);
        assert(p2);
        assert(*p2 == 10);
    }

    // Test 5: 移动赋值
    {
        UniquePtr<int> p1(new int(20));
        UniquePtr<int> p2;

        p2 = std::move(p1);
        assert(!p1);
        assert(p2);
        assert(*p2 == 20);
    }

    // Test 6: reset()
    {
        UniquePtr<Counter> p(new Counter(1));
        assert(Counter::alive == 1);

        p.reset(new Counter(2));
        assert(Counter::alive == 1);
        assert(p->value == 2);

        p.reset();
        assert(!p);
        assert(Counter::alive == 0);
    }

    // Test 7: release()
    {
        UniquePtr<int> p(new int(99));
        int* raw = p.release();

        assert(!p);
        assert(raw != nullptr);
        assert(*raw == 99);

        delete raw;
    }

    // Test 8: bool 语义（仅布尔上下文）
    {
        UniquePtr<int> p(new int(5));
        if (p) {
            assert(true);
        } else {
            assert(false);
        }
    }

    std::cout << "[OK] All UniquePtr tests passed\n";
    return 0;
}
