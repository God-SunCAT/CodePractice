#include <stdexcept>
// 题目1：智能缓冲区管理器

// Buffer Class 用于提供Buffer使用权
class Buffer {
    size_t buffer_id;
    char *buffer;
    void(*callback)(size_t, void*);
    void *p;
public:
    Buffer(size_t buffer_id, char *buffer, void(*callback)(size_t, void*), void* p)
        :buffer_id{buffer_id}, buffer{buffer}, callback{callback}, p{p} {}

    ~Buffer() noexcept {
        callback(buffer_id, p);
    }

    Buffer(const Buffer &) = delete;
    Buffer(Buffer && other) noexcept {
        buffer_id = other.buffer_id;
        buffer = other.buffer;
        callback = other.callback;
        p = other.p;

        other.buffer_id = 0;
        other.buffer = nullptr;
        other.callback = nullptr;
        other.p = nullptr;
    }

    Buffer &operator=(const Buffer &) = delete;
    Buffer &operator=(Buffer && other) noexcept {
        buffer_id = other.buffer_id;
        buffer = other.buffer;
        callback = other.callback;
        p = other.p;

        other.buffer_id = 0;
        other.buffer = nullptr;
        other.callback = nullptr;
        other.p = nullptr;

        return *this;
    }

    [[nodiscard]] char *data() const {
        return buffer;
    }
};

// BufferPool Class 用于管理实际buffer
class BufferPool {
    size_t buffer_size;
    size_t pool_size;

    char *buffer;
    bool *used;
public:
    BufferPool(size_t buffer_size, size_t pool_size)
        :buffer_size{buffer_size}, pool_size{pool_size} {
        buffer = new char[buffer_size * pool_size];
        used = new bool[pool_size]{false};
    }

    ~BufferPool() noexcept {
        delete[] buffer;
        delete[] used;
    }

    BufferPool(const BufferPool &) = delete;
    BufferPool(BufferPool && other) = delete;

    BufferPool &operator=(const BufferPool &) = delete;
    BufferPool &operator=(BufferPool && other) = delete;

    static void callback(size_t buffer_id, void *p) {
        auto other = static_cast<BufferPool *>(p);
        if (buffer_id + 1 > other->pool_size) return;

        other->used[buffer_id] = false;
    }

    Buffer acquire() {
        for (size_t buffer_id = 0; buffer_id < pool_size; buffer_id++) {
            if (!used[buffer_id]) {
                used[buffer_id] = true;
                return Buffer{
                    buffer_id,
                    buffer + buffer_id * buffer_size,
                    callback,
                    this
                };
            }
        }
        throw std::runtime_error{"Cannot find a free space"};
    }
};