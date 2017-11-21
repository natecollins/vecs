#include "Pool.h"
#include <thread>
#include <chrono>

namespace vecs
{

Pool::Pool(std::size_t block_size, std::size_t chunk_size)
  : spinlock ATOMIC_FLAG_INIT, block_size (((block_size + 15) / 16) * 16), chunk_size (((chunk_size + 15) / 16) * 16) {
    // Align block size to 16 bytes, and make sure it divides evenly into the chunks
    // Reserve and set the starting top
    reserve(1);
    top = chunks.front();
}

Pool::~Pool() {
    for (auto ch : chunks) {
        delete[] ch;
    }
    chunks.clear();
}

void Pool::lock() {
    while (spinlock.test_and_set()) {
        std::this_thread::sleep_for( std::chrono::milliseconds(1) );
    }
}

void Pool::unlock() {
    spinlock.clear();
}

std::size_t Pool::getChunkSize() const {
    return chunk_size;
}

std::size_t Pool::getBlocksPerChunk() const {
    return chunk_size / block_size;
}

std::size_t Pool::getBlockSize() const {
    return block_size;
}

std::size_t Pool::getCurrentBlockMax() const {
    return chunks.size() * getBlocksPerChunk();
}

void Pool::reserve(std::size_t chunk_count, bool bypass_lock) {
    if (!bypass_lock) { lock(); }

    for (std::size_t i=0; i < chunk_count; ++i) {
        // Allocate chunk
        char* chunk = new char[chunk_size];
        chunks.push_back(chunk);
    }

    if (!bypass_lock) { unlock(); }
}

}
