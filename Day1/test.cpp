//
// Created by Ayr on 2025/12/19.
//

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstring>
#include <cassert>
#include <stdexcept>

using namespace std;
using namespace std::chrono;

// ===== 你的实现 =====
#include "BufferPool.h"
// ===== 原始题目测试用例 =====

void test_case_1() {
    cout << "Test 1: 基本功能 - ";
    try {
        BufferPool pool(1024, 10);
        {
            auto buf1 = pool.acquire();
            auto buf2 = pool.acquire();
            // buf1 和 buf2 应该是不同的缓冲区
            assert(buf1.data() != buf2.data() && "buf1 and buf2 should be different");
        } // 缓冲区应该自动归还

        cout << "✅ PASSED" << endl;
    } catch (const exception& e) {
        cout << "❌ FAILED: " << e.what() << endl;
    }
}

void test_case_2() {
    cout << "Test 2: 移动语义 - ";
    try {
        BufferPool pool(1024, 10);
        auto buf = pool.acquire();
        char* original_ptr = buf.data();

        auto buf2 = std::move(buf);  // buf 应该失效

        assert(buf.data() == nullptr && "buf should be invalid after move");
        assert(buf2.data() == original_ptr && "buf2 should own the buffer");

        cout << "✅ PASSED" << endl;
    } catch (const exception& e) {
        cout << "❌ FAILED: " << e.what() << endl;
    }
}

void test_case_3() {
    cout << "Test 3: 耗尽场景 - ";
    try {
        BufferPool pool(1024, 10);
        std::vector<Buffer> buffers;

        for(int i = 0; i < 10; ++i) {
            buffers.push_back(pool.acquire());
        }

        // 第11次 acquire 应该阻塞或抛出异常
        bool exception_thrown = false;
        try {
            auto buf = pool.acquire();
        } catch (const runtime_error&) {
            exception_thrown = true;
        }

        assert(exception_thrown && "Should throw exception when exhausted");
        cout << "✅ PASSED" << endl;
    } catch (const exception& e) {
        cout << "❌ FAILED: " << e.what() << endl;
    }
}

void test_case_4() {
    cout << "Test 4: 性能测试 - ";
    try {
        BufferPool pool(1024, 100);

        auto start = high_resolution_clock::now();
        for(int i = 0; i < 100000; ++i) {
            auto buf = pool.acquire();
            // 使用 buf
            memset(buf.data(), 0, 1024);
        }
        auto end = high_resolution_clock::now();
        auto pool_duration = duration_cast<microseconds>(end - start).count();

        // 对比 new/delete
        start = high_resolution_clock::now();
        for(int i = 0; i < 100000; ++i) {
            char* buf = new char[1024];
            memset(buf, 0, 1024);
            delete[] buf;
        }
        end = high_resolution_clock::now();
        auto new_duration = duration_cast<microseconds>(end - start).count();

        cout << endl;
        cout << "  BufferPool: " << pool_duration << " μs" << endl;
        cout << "  new/delete: " << new_duration << " μs" << endl;
        cout << "  Speedup: " << (double)new_duration / pool_duration << "x" << endl;

        if (pool_duration < new_duration) {
            cout << "  ✅ PASSED (BufferPool is faster)" << endl;
        } else {
            cout << "  ⚠️  WARNING: BufferPool is slower" << endl;
        }
    } catch (const exception& e) {
        cout << "❌ FAILED: " << e.what() << endl;
    }
}

void test_case_5() {
    cout << "Test 5: 自动归还机制 - ";
    try {
        BufferPool pool(512, 5);

        // 第一轮：占满所有缓冲区
        {
            vector<Buffer> buffers;
            for (int i = 0; i < 5; ++i) {
                buffers.push_back(pool.acquire());
            }
            // 此时应该无法再获取
            bool exhausted = false;
            try {
                auto buf = pool.acquire();
            } catch (...) {
                exhausted = true;
            }
            assert(exhausted && "Pool should be exhausted");
        } // 所有 Buffer 析构，自动归还

        // 第二轮：应该能再次获取所有缓冲区
        vector<Buffer> buffers;
        for (int i = 0; i < 5; ++i) {
            buffers.push_back(pool.acquire());
        }

        cout << "✅ PASSED" << endl;
    } catch (const exception& e) {
        cout << "❌ FAILED: " << e.what() << endl;
    }
}

void test_case_6() {
    cout << "Test 6: 缓冲区独立性 - ";
    try {
        BufferPool pool(256, 5);

        auto buf1 = pool.acquire();
        auto buf2 = pool.acquire();

        // 写入不同数据
        memset(buf1.data(), 'A', 256);
        memset(buf2.data(), 'B', 256);

        // 验证独立性
        assert(buf1.data()[0] == 'A' && "buf1 should contain 'A'");
        assert(buf2.data()[0] == 'B' && "buf2 should contain 'B'");
        assert(buf1.data() != buf2.data() && "Buffers should be independent");

        cout << "✅ PASSED" << endl;
    } catch (const exception& e) {
        cout << "❌ FAILED: " << e.what() << endl;
    }
}

void test_case_7() {
    cout << "Test 7: 移动赋值 - ";
    try {
        BufferPool pool(1024, 10);

        auto buf1 = pool.acquire();
        auto buf2 = pool.acquire();

        char* ptr1 = buf1.data();
        char* ptr2 = buf2.data();

        buf1 = std::move(buf2); // buf1 释放原缓冲区，接管 buf2 的缓冲区

        assert(buf2.data() == nullptr && "buf2 should be invalid");
        assert(buf1.data() == ptr2 && "buf1 should now own buf2's buffer");

        cout << "✅ PASSED" << endl;
    } catch (const exception& e) {
        cout << "❌ FAILED: " << e.what() << endl;
    }
}

void test_case_8() {
    cout << "Test 8: 异常安全性 - ";
    try {
        BufferPool pool(1024, 5);

        try {
            auto buf = pool.acquire();
            // 模拟异常
            throw runtime_error("Simulated error");
        } catch (...) {
            // 捕获异常
        }

        // 缓冲区应该已经自动归还，可以再次获取
        vector<Buffer> buffers;
        for (int i = 0; i < 5; ++i) {
            buffers.push_back(pool.acquire());
        }

        cout << "✅ PASSED" << endl;
    } catch (const exception& e) {
        cout << "❌ FAILED: " << e.what() << endl;
    }
}

void test_case_9() {
    cout << "Test 9: 多次循环获取释放 - ";
    try {
        BufferPool pool(1024, 3);

        for (int cycle = 0; cycle < 10; ++cycle) {
            vector<Buffer> buffers;
            for (int i = 0; i < 3; ++i) {
                buffers.push_back(pool.acquire());
            }
            // 循环结束，自动释放
        }

        // 最后一次验证还能获取
        auto buf = pool.acquire();
        assert(buf.data() != nullptr && "Should still be able to acquire");

        cout << "✅ PASSED" << endl;
    } catch (const exception& e) {
        cout << "❌ FAILED: " << e.what() << endl;
    }
}

void test_case_10() {
    cout << "Test 10: 不同大小的缓冲区 - ";
    try {
        BufferPool small_pool(64, 20);
        BufferPool large_pool(8192, 2);

        auto small = small_pool.acquire();
        auto large = large_pool.acquire();

        // 写入测试
        memset(small.data(), 'S', 64);
        memset(large.data(), 'L', 8192);

        assert(small.data()[0] == 'S' && "Small buffer write successful");
        assert(large.data()[0] == 'L' && "Large buffer write successful");

        cout << "✅ PASSED" << endl;
    } catch (const exception& e) {
        cout << "❌ FAILED: " << e.what() << endl;
    }
}

int main() {
    cout << "\n╔═══════════════════════════════════════════════╗" << endl;
    cout << "║  BufferPool 测试套件 (原始题目)              ║" << endl;
    cout << "╚═══════════════════════════════════════════════╝\n" << endl;

    test_case_1();
    test_case_2();
    test_case_3();
    test_case_4();
    test_case_5();
    test_case_6();
    test_case_7();
    test_case_8();
    test_case_9();
    test_case_10();

    cout << "\n═══════════════════════════════════════════════" << endl;
    cout << "测试完成！" << endl;
    cout << "═══════════════════════════════════════════════\n" << endl;

    return 0;
}