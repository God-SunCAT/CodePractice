#include <iostream>
#include <vector>
#include <thread>
#include <cassert>
#include "PoolAllocator.h" // 确保文件名正确

struct ComplexData {
    int id;
    double values[3];
    char tag[8];
};

void test_basic_allocation() {
    std::cout << "Starting basic allocation test..." << std::endl;
    PoolAllocator pool;

    // 1. 测试不同规格的分配
    int* p1 = pool.allocate<int>(1);          // 匹配 8字节桶
    *p1 = 12345;

    ComplexData* p2 = pool.allocate<ComplexData>(1); // 40字节，匹配 64字节桶
    p2->id = 99;
    p2->values[0] = 3.14;

    // 2. 验证数据
    assert(*p1 == 12345);
    assert(p2->id == 99);

    // 3. 释放
    pool.deallocate(p1, 1);
    pool.deallocate(p2, 1);
    std::cout << "Basic test passed!" << std::endl;
}

void test_expansion() {
    std::cout << "Starting expansion test..." << std::endl;
    PoolAllocator pool;
    std::vector<int*> ptrs;

    // 初始 8字节桶只有 64个块，我们申请 100个，强行触发扩容
    try {
        for (int i = 0; i < 100; ++i) {
            int* p = pool.allocate<int>(1);
            *p = i;
            ptrs.push_back(p);
        }

        for (int i = 0; i < 100; ++i) {
            assert(*ptrs[i] == i);
            pool.deallocate(ptrs[i], 1);
        }
    } catch (const std::exception& e) {
        std::cerr << "Expansion failed: " << e.what() << std::endl;
    }
    std::cout << "Expansion test finished!" << std::endl;
}

void multi_threaded_test() {
    std::cout << "Starting multi-threaded stress test..." << std::endl;
    PoolAllocator pool;
    const int thread_count = 4;
    const int allocs_per_thread = 50;
    std::vector<std::thread> threads;

    for (int t = 0; t < thread_count; ++t) {
        threads.emplace_back([&pool, t]() {
            std::vector<double*> data;
            for (int i = 0; i < allocs_per_thread; ++i) {
                double* d = pool.allocate<double>(1);
                *d = static_cast<double>(t * 1000 + i);
                data.push_back(d);
            }
            for (auto d : data) {
                pool.deallocate(d, 1);
            }
        });
    }

    for (auto& t : threads) t.join();
    std::cout << "Multi-threaded test finished!" << std::endl;
}

int main() {
    try {
        test_basic_allocation();
        std::cout << "---------------------------" << std::endl;
        test_expansion();
        std::cout << "---------------------------" << std::endl;
        multi_threaded_test();

        std::cout << "\nAll tests completed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}