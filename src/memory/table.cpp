
#include <cstdlib>
#include <cstring>

#include "memory/table.hpp"

Table::~Table() {
    if (data) {
        free(data);
        data = nullptr;
    }
}

Table::Table(uint32_t stride) : data(nullptr), stride(stride), count(0), capacity(0) {}
Table::Table(uint32_t stride, uint32_t capacity) : 
    data(nullptr), stride(stride), count(0), capacity(capacity) {
    if (capacity > 0) {
        data = malloc(stride * capacity);
    }
}

Table::Table(Table &&other) noexcept {
    if (this == &other) {
        return; // Self-move, nothing to do
    }
    if (data) {
        free(data);
    }
    data = other.data;
    stride = other.stride;
    count = other.count;
    capacity = other.capacity;

    other.data = nullptr;
}

Table &Table::operator=(Table &&other) noexcept {
    if (this != &other) {
        if (data) {
            free(data);
        }

        data = other.data;
        stride = other.stride;
        count = other.count;
        capacity = other.capacity;

        other.data = nullptr;
    }
    return *this;
}

void *Table::get(uint32_t index) const {
    if (index >= count) {
        return nullptr; // Out of bounds
    }
    return static_cast<uint8_t *>(data) + index * stride;
}

uint64_t Table::load(uint32_t c, const void *d) {
    if (count + c >= capacity) {
        data = realloc(data, (count + c) * stride);
        capacity = count + c; // Update capacity
    }

    void *dst_ptr = static_cast<uint8_t *>(this->data) + count * stride;
    std::memcpy(dst_ptr, data, stride);
    count += c; // Update count
    return (uint64_t)dst_ptr - (uint64_t)data;
}

uint64_t Table::duplicate(uint64_t src, uint32_t count) {
    return load(count, static_cast<uint8_t *>(data) + src * stride);
}

uint64_t Table::copy(uint64_t src, uint64_t dst, uint32_t count) {
    if (src + count > this->count) {
        return 0; // Out of bounds
    }

    if (dst + count > this->capacity) {
        data = realloc(data, (dst + count) * stride);
        this->capacity = dst + count; // Update capacity
    }

    void *src_ptr = static_cast<uint8_t *>(data) + src * stride;
    void *dst_ptr = static_cast<uint8_t *>(data) + dst * stride;

    std::memcpy(dst_ptr, src_ptr, count * stride);
    return count;
}

void Table::unload(uint64_t src, uint32_t count) {
    if (src + count > this->count || count == 0) {
        return; // Out of bounds or no data to unload
    }

    void *src_ptr = static_cast<uint8_t *>(this->data) + src * stride;
    void *dst_ptr = static_cast<uint8_t *>(this->data) + (src + count) * stride;

    std::memmove(src_ptr, dst_ptr, (this->count - (src + count)) * stride);
    this->count -= count; // Reduce the count
}

void Table::update(uint64_t src, uint32_t count, const void *data) {
    if (src + count > this->count || count == 0) {
        return; // Out of bounds or no data to set
    }

    void *dst_ptr = static_cast<uint8_t *>(this->data) + src * stride;
    std::memcpy(dst_ptr, data, count * stride);
}

Slice Table::slice(uint64_t src, uint32_t count) const {
    if (src + count > this->count || count == 0) {
        return Slice(nullptr, 0, 0); // Out of bounds or no data to slice
    }

    void *slice_data = static_cast<uint8_t *>(this->data) + src * stride;
    return Slice(slice_data, src, count * stride);
}
