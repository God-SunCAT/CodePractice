#ifndef DAY2_UNIQUEPTR_H
#define DAY2_UNIQUEPTR_H
// 模板类必须全部写在头文件中。

template<typename T>
class UniquePtr {
    T* ptr;

public:
    // 默认构造
    UniquePtr() noexcept
        : ptr(nullptr) {}

    // 接管裸指针
    explicit UniquePtr(T* ptr) noexcept
        : ptr(ptr) {}

    // 禁止拷贝
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // 移动构造
    // 移动构造不可能 self-move 故无需自检
    UniquePtr(UniquePtr&& other) noexcept
        : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    // 移动赋值
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    // 析构
    ~UniquePtr() noexcept {
        delete ptr;
    }

    // 访问接口
    T* get() const noexcept {
        return ptr;
    }

    T& operator*() const {
        return *ptr;
    }

    T* operator->() const noexcept {
        // up->foo() → up.operator->()->foo()
        return ptr;
    }

    explicit operator bool() const noexcept {
        return ptr != nullptr;
    }

    // 资源管理
    T* release() noexcept {
        T* tmp = ptr;
        ptr = nullptr;
        return tmp;
    }

    void reset(T* new_ptr = nullptr) noexcept {
        if (ptr != new_ptr) {
            delete ptr;
            ptr = new_ptr;
        }
    }
};

#endif // DAY2_UNIQUEPTR_H
