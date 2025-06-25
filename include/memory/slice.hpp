
#ifndef ALCHEMIST_MEMORY_SLICE_HPP
#define ALCHEMIST_MEMORY_SLICE_HPP

#include <cstdint> // for size_t

// A static view of a memory region, no onwership of the data
// This is useful for passing around raw data without copying it
struct Slice {
    void *data = nullptr;
    uint64_t offset = 0;
    uint32_t size = 0;

    Slice() = default;
    Slice(void *data, uint64_t offset, uint32_t size);
    Slice(const Slice &other);
    Slice(Slice &&other) noexcept;

    Slice &operator=(const Slice &other);
    Slice &operator=(Slice &&other) noexcept;
};

#endif // ALCHEMIST_MEMORY_SLICE_HPP
