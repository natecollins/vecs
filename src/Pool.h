#ifndef POOL_H_
#define POOL_H_

#include <cstdint>
#include <cstdlib>
#include <deque>
#include <vector>
#include <tuple>
#include <atomic>
#include "entdef.h"

namespace vecs
{

class Pool {
    friend class Domain;
private:
    // Spinlock for allocations/de-allocations
    std::atomic_flag spinlock;
    // Assignment allocation size; e.g. 32
    const std::size_t block_size;
    // Allocation chunk size; e.g. 65536
    const std::size_t chunk_size;
    // Allocated chunks
    std::vector<char*> chunks;
    // Freed Blocks availability
    std::vector<char*> blocks;
    // Top of stack
    char* top;

public:
    Pool(std::size_t block_size, std::size_t chunk_size=65536);
    virtual ~Pool();

    void lock();
    void unlock();

    std::size_t getChunkSize() const;
    std::size_t getBlocksPerChunk() const;
    std::size_t getBlockSize() const;
    std::size_t getCurrentBlockMax() const;

    /**
     * Allocate more memory chunks
     */
    void reserve(std::size_t chunk_count=1, bool bypass_lock=false);

    template<typename T>
    T* allocate();

    template<typename T>
    void release(T* ptr);

};

template<typename T>
T* Pool::allocate() {
    void* mem = nullptr;
    lock();
    if (blocks.size() != 0) {
        mem = (void*)blocks.back();
        blocks.pop_back();
    }
    else {
        if ((top + block_size) > (chunks.back() + chunk_size)) {
            reserve(1, true);
            top = chunks.back();
        }
        mem = (void*)top;
        top += block_size;
    }
    unlock();
    return static_cast<T*>(mem);
}

template<typename T>
void Pool::release(T *ptr) {
    // Call Component virtual deconstructor
    ptr->~T();
    // Mark memory as available
    lock();
    blocks.push_back( (char*)ptr );
    unlock();
}

}

#endif // POOL_H_
