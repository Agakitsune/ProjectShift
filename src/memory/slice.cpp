
#include "memory/slice.hpp"

Slice::Slice(void *data, uint64_t offset, uint32_t size) : data(data), offset(offset), size(size) {}
Slice::Slice(const Slice &other) : data(other.data), offset(other.offset), size(other.size) {}
Slice::Slice(Slice &&other) noexcept : data(other.data), offset(other.offset), size(other.size) {}

Slice &Slice::operator=(const Slice &other) {
    data = other.data;
    offset = other.offset;
    size = other.size;
    return *this;
}

Slice &Slice::operator=(Slice &&other) noexcept {
    data = other.data;
    offset = other.offset;
    size = other.size;
    return *this;
}
