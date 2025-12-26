//
// Created by Ayr on 2025/12/19.
//
#include "BufferPool.h"
#include <stdexcept>

Buffer::Buffer(size_t buffer_id, char *buffer, void(*callback)(size_t, void*), void* p)
    :buffer_id{buffer_id}, buffer{buffer}, callback{callback}, p{p} {}

Buffer::~Buffer() noexcept {
    callback(buffer_id, p);
}

Buffer::Buffer(Buffer && other) noexcept {
    buffer_id = other.buffer_id;
    buffer = other.buffer;
    callback = other.callback;
    p = other.p;

    other.buffer_id = 0;
    other.buffer = nullptr;
    other.callback = nullptr;
    other.p = nullptr;
}

Buffer &Buffer::operator=(Buffer && other) noexcept {
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

[[nodiscard]] char *Buffer::data() const {
    return buffer;
}

// ----

BufferPool::BufferPool(size_t buffer_size, size_t pool_size)
    :buffer_size{buffer_size}, pool_size{pool_size} {
    buffer = new char[buffer_size * pool_size];
    used = new bool[pool_size]{false};
}

BufferPool::~BufferPool() noexcept {
    delete[] buffer;
    delete[] used;
}

void BufferPool::callback(size_t buffer_id, void *p) {
    auto other = static_cast<BufferPool *>(p);
    if (buffer_id + 1 > other->pool_size) return;

    other->used[buffer_id] = false;
}

Buffer BufferPool::acquire() {
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