
#ifndef ALCHEMIST_MEMORY_MAP_HPP
#define ALCHEMIST_MEMORY_MAP_HPP

#include <cstring>

template <typename T>
struct StringMap {
    const char *__strings = nullptr; // strings are constant

    const char **key = nullptr; // keys are constant
    T *data = nullptr;
    uint32_t size = 0;
    uint32_t capacity = 0;

    uint32_t atlas_size = 0;

    StringMap(uint32_t initial = 0) : size(0), capacity(initial) {
        key = new const char*[initial];
        data = new T[initial];
    }

    ~StringMap() {
        delete[] key;
        delete[] data;
        delete[] __strings;
    }

    StringMap(const StringMap &other) : size(other.size), capacity(other.capacity), atlas_size(other.atlas_size) {
        __strings = new char[atlas_size];

        memcpy(__strings, other.__strings, atlas_size);

        key = new const char*[capacity];
        data = new T[capacity];
        for (uint32_t i = 0; i < size; ++i) {
            key[i] = other.key[i];
            data[i] = other.data[i];
        }
    }

    StringMap(StringMap &&other) noexcept : key(other.key), data(other.data), size(other.size), capacity(other.capacity), atlas_size(other.atlas_size), __strings(other.__strings) {
        other.key = nullptr;
        other.data = nullptr;
        other.__strings = nullptr;
        other.size = 0;
        other.capacity = 0;
    }

    StringMap &operator=(const StringMap &other) {
        __strings = new char[other.atlas_size];

        size = other.size;
        capacity = other.capacity;
        atlas_size = other.atlas_size;

        memcpy(__strings, other.__strings, atlas_size);

        key = new const char*[capacity];
        data = new T[capacity];
        for (uint32_t i = 0; i < size; ++i) {
            key[i] = other.key[i];
            data[i] = other.data[i];
        }
    }

    StringMap &operator=(StringMap &&other) {
        key = other.key;
        data = other.data;
        __strings = other.__strings;
        size = other.size;
        capacity = other.capacity;
        atlas_size = other.atlas_size;
    }

    void put(const char *key, const T &value) {
        uint64_t len = strlen(key);
        const char *old = __strings;
        __strings = new char[atlas_size + len + 1];

        memcpy(__strings, old, atlas_size);
        memcpy(&__strings[atlas_size], key, len + 1);

        if (size >= capacity) {
            capacity = capacity ? capacity << 1 : 1;
            T *new_data = new T[capacity];
            const char **new_key = new const char*[capacity];
            for (uint32_t i = 0; i < size; ++i) {
                new_data[i] = data[i];
                new_key[i] = key[i];
            }
            delete[] data;
            delete[] key;
            data = new_data;
            key = new_key;
        }

        data[size] = value;
        key[size] = &__strings[atlas_size];

        atlas_size += len + 1;
    }

    void put(const char *key, T &&value) {
        uint64_t len = strlen(key);
        const char *old = __strings;
        __strings = new char[atlas_size + len + 1];

        memcpy(__strings, old, atlas_size);
        memcpy(&__strings[atlas_size], key, len + 1);

        if (size >= capacity) {
            capacity = capacity ? capacity << 1 : 1;
            T *new_data = new T[capacity];
            const char **new_key = new const char*[capacity];
            for (uint32_t i = 0; i < size; ++i) {
                new_data[i] = data[i];
                new_key[i] = key[i];
            }
            delete[] data;
            delete[] key;
            data = new_data;
            key = new_key;
        }

        data[size] = std::move(value);
        key[size] = &__strings[atlas_size];

        atlas_size += len + 1;
    }

    const T &get(const char *key) {
        for (uint32_t i = 0; i < size; i++) {
            if (strcmp(key, key[i]) == 0) {
                return data[i];
            }
        }
        return data[size];
    }

    uint32_t has(const char *key) {
        for (uint32_t i = 0; i < size; i++) {
            if (strcmp(key, key[i]) == 0) {
                return 1;
            }
        }
        return 0;
    }
};

#endif // ALCHEMIST_MEMORY_MAP_HPP