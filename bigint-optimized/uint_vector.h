//
// Created by Yaroslav Ilin, M3138, KT ITMO y2019. 21.05.2020
//

#ifndef VECTOR_H
#define VECTOR_H

#include <vector>
#include <algorithm>
#include <cstdint>
#include <memory>

struct dynamic_buffer {
    size_t ref_cnt;
    std::vector<uint32_t> data;

    dynamic_buffer(std::vector<uint32_t> data) : ref_cnt(1), data(data) {}

    size_t use_count() {
        return ref_cnt;
    }

    ~dynamic_buffer() = default;
};


struct uint_vector {

    uint_vector() : uint_vector(1, 0) {}

    uint_vector(size_t size) : uint_vector(size, 0) {}

    uint_vector(size_t size, uint32_t init_val) : size_(size), small(size <= MAX_STATIC_SIZE) {
        if (size <= MAX_STATIC_SIZE) {
            std::fill(static_data, static_data + size, init_val);
        } else {
            dynamic_data = new dynamic_buffer(std::vector<uint32_t>(size, init_val));
        }
    }

    uint_vector(uint_vector const &other) : size_(other.size_), small(other.small) {
        if (is_small()) {
            std::copy_n(other.static_data, other.size_, static_data);
        } else {
            dynamic_data = other.dynamic_data;
            dynamic_data->ref_cnt++;
        }
    }

    ~uint_vector() {
        if (!is_small()) {
            dynamic_data->ref_cnt--;
            if (dynamic_data->ref_cnt == 0) {
                delete dynamic_data;
            }
        }
    }

    uint_vector &operator=(uint_vector const &other) {
        this->~uint_vector();
        size_ = other.size_;
        small = other.small;
        if (is_small()) {
            std::copy_n(other.static_data, other.size_, static_data);
        } else {
            dynamic_data = other.dynamic_data;
            dynamic_data->ref_cnt++;
        }
        return *this;
    }

    bool operator==(uint_vector const &other) const {
        if (size_ != other.size_) {
            return false;
        }
        for (size_t i = 0; i < size_; i++) {
            if (this->operator[](i) != other[i]) {
                return false;
            }
        }
        return true;

    }

    void push_back(uint32_t x) {
        if (is_small()) {
            if (size_ == MAX_STATIC_SIZE) {
                std::vector<uint32_t> tmp(static_data, static_data + size_);
                tmp.push_back(x);
                dynamic_data = new dynamic_buffer(tmp);
                size_++;
                small = false;
            } else {
                static_data[size_++] = x;
            }
        } else {
            unshare();
            dynamic_data->data.push_back(x);
            size_++;
        }
    }

    void pop_back() {
        if (is_small()) {
            static_data[--size_].~uint32_t();
        } else {
            unshare();
            dynamic_data->data.pop_back();
            size_--;
        }
    }

    uint32_t const &operator[](size_t ind) const {
        if (is_small()) {
            return static_data[ind];
        }
        return dynamic_data->data[ind];
    }

    uint32_t &operator[](size_t ind) {
        if (is_small()) {
            return static_data[ind];
        }
        unshare();
        return dynamic_data->data[ind];
    }

    uint32_t back() const {
        if (is_small()) {
            return static_data[size_ - 1];
        }
        return dynamic_data->data.back();
    }

    void reverse() {
        if (is_small()) {
            std::reverse(static_data, static_data + size_);
        } else {
            unshare();
            std::reverse(dynamic_data->data.begin(), dynamic_data->data.end());
        }
    }

    void assign(size_t size, uint32_t x) {
        this->~uint_vector();
        size_ = size;
        if (size <= MAX_STATIC_SIZE) {
            small = true;
            std::fill(static_data, static_data + size, x);
        } else {
            small = false;
            dynamic_data = new dynamic_buffer(std::vector<uint32_t>(size, x));
        }
    }

    void swap(uint_vector &other) {
        uint_vector tmp(other);
        other = *this;
        *this = tmp;
    }

    size_t size() const {
        return size_;
    }


private:
    static size_t constexpr MAX_STATIC_SIZE = sizeof(dynamic_buffer*) / sizeof(uint32_t);

    size_t size_;
    bool small;

    bool is_small() const {
        return small;
    }

    union {
        uint32_t static_data[MAX_STATIC_SIZE];
        dynamic_buffer *dynamic_data;
    };

    void unshare() {
        if (dynamic_data->use_count() != 1) {
            dynamic_data->ref_cnt--;
            dynamic_data = new dynamic_buffer(dynamic_data->data);
        }
    }
};

#endif //VECTOR_H
