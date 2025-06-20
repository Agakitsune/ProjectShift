
#ifndef ALCHEMIST_MEMORY_TABLE_HPP
#define ALCHEMIST_MEMORY_TABLE_HPP

#include <cstdint> // for uint32_t

#include "memory/slice.hpp"

struct Table {
    void *data = nullptr; // Pointer to the data, allocated on demand

    uint32_t stride = 0; // Size of each element in bytes
    uint32_t count = 0; // Number of elements currently in the table
    uint32_t capacity = 0; // Maximum number of elements the table can hold

    ~Table();

    Table() = default; // Default constructor, no data allocated
    Table(uint32_t stride);
    Table(uint32_t stride, uint32_t capacity);

    Table(const Table &other) = delete;
    Table(Table &&other) noexcept;

    Table &operator=(const Table &other) = delete;
    Table &operator=(Table &&other) noexcept;

    void *get(uint32_t index) const;

    uint64_t load(uint32_t count, const void *data);
    uint64_t duplicate(uint64_t src, uint32_t count);
    uint64_t copy(uint64_t src, uint64_t dst, uint32_t count);
    void unload(uint64_t src, uint32_t count);
    void update(uint64_t src, uint32_t count, const void *data); // not really useful since we can just update from the slice directly, but kept in case we need it

    Slice slice(uint64_t src, uint32_t count) const;
};

#endif // ALCHEMIST_MEMORY_TABLE_HPP
