//
// Created by Ayr on 2025/12/25.
//

#ifndef DAY5_POOLALLOCATOR_H
#define DAY5_POOLALLOCATOR_H

/*
 * 私有内存分配器
 * 自动处理对齐要求 + 自动扩容并拼接Chunk
 * 由于align只允许2的幂次，那么以此为标准完成扩容
 * 考虑到扩容需求，需要有按chunkInfo->FreeNode
 * 并且，要求有线程锁
 */
#include <cstddef>
#include <vector>
#include <new>
#include <stdexcept>
#include <mutex>
#include <iostream>

using std::vector;

struct FreeNode {
    FreeNode* next = nullptr;
};

struct chunkInfo {
    size_t chunkSize;
    size_t blockSize;
    size_t freeBlocks;
    FreeNode* freeNode;
};

struct RawPtrInfo {
    char* ptr;
    size_t size;
};

class PoolAllocator {
    std::mutex mtx;
    vector<chunkInfo> chunks;
    vector<RawPtrInfo> rawPtrInfos;
public:
    PoolAllocator() {
        // chunkSize - BuketSize
        size_t initChunk[][2] = {
            // 注意：不得小于8byte
            {64, 8},
            {64, 16},
            {32, 32},
            {32, 64},
            {16, 128},
            {16, 256}
        };
        for (auto& chunk : initChunk) {
            char *ptr = static_cast<char *>(::operator new(chunk[0] * chunk[1], std::align_val_t(16)));

            auto *first = reinterpret_cast<FreeNode*>(ptr);
            auto* freeNode = first;
            for (int i = 1; i < chunk[0]; i++) {
                freeNode->next = reinterpret_cast<FreeNode*>(ptr + i * chunk[1]);
                freeNode = freeNode->next;
            }
            freeNode->next = nullptr;
            chunks.emplace_back(chunk[0], chunk[1], chunk[0], first);
            rawPtrInfos.emplace_back(ptr, chunk[0] * chunk[1]);
        }
    }
    ~PoolAllocator() {
        for (auto& rawPtrInfo : rawPtrInfos) {
            ::operator delete(rawPtrInfo.ptr, std::align_val_t(16));
        }
    }

    template<typename T>
    T* allocate(size_t n) {
        for (auto& chunk : chunks) {
            if (chunk.blockSize < n * sizeof(T)) continue;

            std::lock_guard<std::mutex> lock(mtx);
            if (!chunk.freeBlocks) {
                // 触发扩容
                constexpr size_t k = 1; // 增加倍数
                char *ptr = static_cast<char *>(::operator new(chunk.chunkSize * chunk.blockSize * k, std::align_val_t(16)));

                auto *first = reinterpret_cast<FreeNode*>(ptr);
                auto *freeNode = first;
                for (size_t i = 1; i < chunk.chunkSize * k; i++) {
                    freeNode->next = reinterpret_cast<FreeNode*>(ptr + i * chunk.blockSize);
                    freeNode = freeNode->next;
                }
                freeNode->next = nullptr;

                size_t rawChunkSize = chunk.chunkSize;
                chunk.freeBlocks += rawChunkSize * k;
                chunk.chunkSize += rawChunkSize * k;

                chunk.freeNode = first;
                rawPtrInfos.emplace_back(ptr, rawChunkSize * chunk.blockSize * k);
            }

            chunk.freeBlocks--;
            auto *tPtr = chunk.freeNode;
            chunk.freeNode = chunk.freeNode->next;
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

                std::lock_guard<std::mutex> lock(mtx);
                auto *freeNode = reinterpret_cast<FreeNode*>(p);
                freeNode->next = chunk.freeNode;
                chunk.freeNode = freeNode;
                chunk.freeBlocks++;

                return;
            }
        }
    }

};


#endif //DAY5_POOLALLOCATOR_H