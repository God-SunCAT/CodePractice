//
// Created by Ayr on 2025/12/19.
//

#ifndef DAY1_BUFFERPOOL_H
#define DAY1_BUFFERPOOL_H
#include <cstddef>
class Buffer {
    size_t buffer_id;
    char *buffer;
    void(*callback)(size_t, void*);
    void *p;
public:
    Buffer(size_t buffer_id, char *buffer, void(*callback)(size_t, void*), void* p);
    ~Buffer() noexcept;

    Buffer(const Buffer &) = delete;
    Buffer(Buffer && other) noexcept;

    Buffer &operator=(const Buffer &) = delete;
    Buffer &operator=(Buffer && other) noexcept;

    [[nodiscard]] char *data() const;
};

class BufferPool {
    size_t buffer_size;
    size_t pool_size;

    char *buffer;
    bool *used;
public:
    BufferPool(size_t buffer_size, size_t pool_size);
    ~BufferPool() noexcept;

    BufferPool(const BufferPool &) = delete;
    BufferPool(BufferPool && other) = delete;

    BufferPool &operator=(const BufferPool &) = delete;
    BufferPool &operator=(BufferPool && other) = delete;

    static void callback(size_t buffer_id, void *p);

    Buffer acquire();
};
#endif DAY1_BUFFERPOOL_H