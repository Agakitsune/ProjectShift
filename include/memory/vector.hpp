
#ifndef ALCHEMIST_MEMORY_VECTOR_HPP
#define ALCHEMIST_MEMORY_VECTOR_HPP

namespace alchemist {
template <typename T> struct vector {
    T *data = nullptr;
    uint32_t size = 0;
    uint32_t capacity = 0;

    vector() = default;
    vector(uint32_t initial_capacity) : size(0), capacity(initial_capacity) {
        data = new T[capacity];
    }
    vector(uint32_t initial_capacity, const T &initial_value)
        : size(initial_capacity), capacity(initial_capacity) {
        data = new T[capacity];
        for (uint32_t i = 0; i < capacity; ++i) {
            data[i] = initial_value;
        }
    }
    vector(const vector &other) : size(other.size), capacity(other.capacity) {
        data = new T[capacity];
        for (uint32_t i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
    }
    vector(vector &&other) noexcept
        : data(other.data), size(other.size), capacity(other.capacity) {
        other.data = nullptr;
        other.size = 0;
        other.capacity = 0;
    }
    ~vector() { delete[] data; }

    vector &operator=(const vector &other) {
        if (this != &other) {
            delete[] data;
            size = other.size;
            capacity = other.capacity;
            data = new T[capacity];
            for (uint32_t i = 0; i < size; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    vector &operator=(vector &&other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            size = other.size;
            capacity = other.capacity;
            other.data = nullptr;
            other.size = 0;
            other.capacity = 0;
        }
        return *this;
    }

    void push(const T &value) {
        if (size >= capacity) {
            capacity = capacity ? capacity << 1 : 1;
            T *new_data = new T[capacity];
            for (uint32_t i = 0; i < size; ++i) {
                new_data[i] = data[i];
            }
            delete[] data;
            data = new_data;
        }
        data[size++] = value;
    }

    void reserve(uint32_t new_capacity) {
        if (new_capacity > capacity) {
            T *new_data = new T[new_capacity];
            for (uint32_t i = 0; i < size; ++i) {
                new_data[i] = data[i];
            }
            delete[] data;
            data = new_data;
            capacity = new_capacity;
        }
    }
};

template <typename T, typename U> struct dual_vector {
    T *data1 = nullptr;
    U *data2 = nullptr;
    uint32_t size = 0;
    uint32_t capacity = 0;

    dual_vector() = default;
    dual_vector(uint32_t initial_capacity)
        : size(0), capacity(initial_capacity) {
        data1 = new T[capacity];
        data2 = new U[capacity];
    }
    dual_vector(uint32_t initial_capacity, const T &initial_value,
                const U &initial_value2)
        : size(initial_capacity), capacity(initial_capacity) {
        data1 = new T[capacity];
        data2 = new U[capacity];
        for (uint32_t i = 0; i < capacity; ++i) {
            data1[i] = initial_value;
            data2[i] = initial_value2;
        }
    }
    dual_vector(const dual_vector &other)
        : size(other.size), capacity(other.capacity) {
        data1 = new T[capacity];
        data2 = new U[capacity];
        for (uint32_t i = 0; i < size; ++i) {
            data1[i] = other.data1[i];
            data2[i] = other.data2[i];
        }
    }
    dual_vector(dual_vector &&other) noexcept
        : data1(other.data1), data2(other.data2), size(other.size),
          capacity(other.capacity) {
        other.data1 = nullptr;
        other.data2 = nullptr;
        other.size = 0;
        other.capacity = 0;
    }
    ~dual_vector() {
        delete[] data1;
        delete[] data2;
    }

    dual_vector &operator=(const dual_vector &other) {
        if (this != &other) {
            delete[] data1;
            delete[] data2;
            size = other.size;
            capacity = other.capacity;
            data1 = new T[capacity];
            data2 = new U[capacity];
            for (uint32_t i = 0; i < size; ++i) {
                data1[i] = other.data1[i];
                data2[i] = other.data2[i];
            }
        }
        return *this;
    }

    dual_vector &operator=(dual_vector &&other) noexcept {
        if (this != &other) {
            delete[] data1;
            delete[] data2;
            data1 = other.data1;
            data2 = other.data2;
            size = other.size;
            capacity = other.capacity;
            other.data1 = nullptr;
            other.data2 = nullptr;
            other.size = 0;
            other.capacity = 0;
        }
        return *this;
    }

    void push(const T &value1, const U &value2) {
        if (size >= capacity) {
            capacity = capacity ? capacity << 1 : 1;
            T *new_data1 = new T[capacity];
            U *new_data2 = new U[capacity];
            for (uint32_t i = 0; i < size; ++i) {
                new_data1[i] = data1[i];
                new_data2[i] = data2[i];
            }
            delete[] data1;
            delete[] data2;
            data1 = new_data1;
            data2 = new_data2;
        }
        data1[size] = value1;
        data2[size++] = value2;
    }
};
} // namespace alchemist

#endif // ALCHEMIST_MEMORY_VECTOR_HPP
