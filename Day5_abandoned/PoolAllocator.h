//
// Created by Ayr on 2025/12/25.
//

#ifndef DAY5_POOLALLOCATOR_H
#define DAY5_POOLALLOCATOR_H

/*
 * 私有内存分配器
 * 自动处理对齐要求 + 自动扩容并拼接Chunk
 * 由于align只允许2的幂次，那么以此为标准完成扩容
 * 考虑到扩容需求，需要有按chunkInfo->FreeListHead
 * 并且，要求有线程锁
 */
#include <cstddef>
#include <vector>
#include <new>
#include <stdexcept>

using std::vector;

struct FreeList {
    char* ptr = nullptr;
    FreeList* next = nullptr;
};

struct chunkInfo {
    size_t chunkSize;
    size_t blockSize;
    size_t freeBlocks;
    FreeList* freeList;
};

struct RawPtrInfo {
    char* ptr;
    size_t size;
};

class PoolAllocator {
    vector<chunkInfo> chunks;
    vector<RawPtrInfo> rawPtrInfos;
public:
    PoolAllocator() {
        // chunkSize - BuketSize
        size_t initchunk[][2] = {
            {64, 8},
            {64, 16},
            {32, 32},
            {32, 64},
            {16, 128},
            {16, 256}
        };
        for (auto& chunk : initchunk) {
            char *ptr = static_cast<char *>(::operator new(chunk[0] * chunk[1], std::align_val_t(16)));

            auto* first = new FreeList(ptr, nullptr);
            auto* freeList = first;
            for (int i = 1; i < chunk[0]; i++) {
                freeList->next = new FreeList(ptr + i * chunk[1], nullptr);
                freeList = freeList->next;
            }
            chunks.emplace_back(chunk[0], chunk[1], chunk[0], freeList);
            rawPtrInfos.emplace_back(ptr, chunk[0] * chunk[1]);
        }
    }
    ~PoolAllocator() {
        for (auto& chunk : chunks) {
            while (chunk.freeList) {
                auto* tPtr = chunk.freeList;
                chunk.freeList = chunk.freeList->next;
                delete tPtr;
            }
        }

        for (auto& rawPtrInfo : rawPtrInfos) {
            ::operator delete(rawPtrInfo.ptr, std::align_val_t(16));
        }
    }

    template<typename T>
    T* allocate(size_t n) {
        for (auto& chunk : chunks) {
            if (chunk.blockSize < n * sizeof(T)) continue;
            if (!chunk.freeBlocks) {
                // 触发扩容
                constexpr size_t k = 1; // 扩容倍数
                char *ptr = static_cast<char *>(::operator new(chunk.chunkSize * chunk.blockSize * k, std::align_val_t(16)));

                auto* first = new FreeList(ptr, nullptr);
                auto* freeList = first;
                for (int i = 1; i < chunk.chunkSize; i++) {
                    freeList->next = new FreeList(ptr + i * chunk.blockSize, nullptr);
                    freeList = freeList->next;
                }

                chunk.chunkSize += chunk.chunkSize * k;
                chunk.freeBlocks += chunk.chunkSize * k;
                chunk.freeList = first;
                rawPtrInfos.emplace_back(ptr, chunk.chunkSize * chunk.blockSize * k);
            }

            chunk.freeBlocks--;
            auto* tPtr = chunk.freeList->ptr;
            auto* tFreeList = chunk.freeList->next;

            delete chunk.freeList;
            chunk.freeList = tFreeList;
            return reinterpret_cast<T*>(tPtr);
        }

        throw std::runtime_error("Not enough memory available");
    }

    template<typename T>
    void deallocate(T* p, size_t n) {
        // 对于chunk和rawPtr的查找理论上是可接受的
        auto* tPtr = reinterpret_cast<char*>(p);
        for (auto& chunk : chunks) {
            if (chunk.blockSize < n * sizeof(T)) continue;
            for (auto& rawPtrInfo : rawPtrInfos) {
                if (tPtr < rawPtrInfo.ptr || tPtr >= rawPtrInfo.ptr + rawPtrInfo.size) continue;

                auto* freeList = new FreeList(reinterpret_cast<char*>(p), nullptr);
                freeList->next = chunk.freeList;
                chunk.freeList = freeList;
                chunk.freeBlocks++;

                return;
            }
        }
    }

};


#endif //DAY5_POOLALLOCATOR_H