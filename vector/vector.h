//
// Created by Yaroslav Ilin, M3138, KT ITMO y2019. 16.05.2020
//

#ifndef VECTOR_H
#define VECTOR_H

#include <cstdio>
#include <utility>
#include <cassert>

template<typename T>
struct vector {
    typedef T *iterator;
    typedef T const *const_iterator;

    vector();                               // O(1) nothrow
    vector(vector const &);                  // O(N) strong
    vector &operator=(vector const &other); // O(N) strong

    ~vector();                              // O(N) nothrow

    T &operator[](size_t i);                // O(1) nothrow
    T const &operator[](size_t i) const;    // O(1) nothrow

    T *data();                              // O(1) nothrow
    T const *data() const;                  // O(1) nothrow
    size_t size() const;                    // O(1) nothrow

    T &front();                             // O(1) nothrow
    T const &front() const;                 // O(1) nothrow

    T &back();                              // O(1) nothrow
    T const &back() const;                  // O(1) nothrow
    void push_back(T const &);               // O(1)* strong
    void pop_back();                        // O(1) nothrow

    bool empty() const;                     // O(1) nothrow

    size_t capacity() const;                // O(1) nothrow
    void reserve(size_t);                   // O(N) strong
    void shrink_to_fit();                   // O(N) strong

    void clear();                           // O(N) nothrow

    void swap(vector &);                     // O(1) nothrow

    iterator begin();                        // O(1) nothrow

    iterator end();                          // O(1) nothrow

    const_iterator begin() const;            // O(1) nothrow

    const_iterator end() const;             // O(1) nothrow

    iterator insert(const_iterator pos, T const &); // O(N) weak

    iterator erase(const_iterator pos);     // O(N) weak

    iterator erase(const_iterator first, const_iterator last); // O(N) weak

    static void del_data(size_t, T *data);

    static T *copy_data(size_t size, size_t capacity, T *other_data);

private:

    size_t increase_capacity() const;

private:
    T *data_;
    size_t size_;
    size_t capacity_;
};

template<typename T>
vector<T>::vector(): data_(nullptr), size_(0), capacity_(0) {}

template<typename T>
vector<T>::vector(const vector &other) : vector() {
    T *new_data = copy_data(other.size_, other.size_, other.data_);
    data_ = new_data;
    size_ = other.size_;
    capacity_ = other.size_;
}

template<typename T>
vector<T> &vector<T>::operator=(const vector &other) {
    vector<T> temp(other);
    swap(temp);
    return *this;
}

template<typename T>
void vector<T>::swap(vector &other) {
    using std::swap;
    swap(data_, other.data_);
    swap(capacity_, other.capacity_);
    swap(size_, other.size_);
}

template<typename T>
vector<T>::~vector() {
    del_data(size_, data_);
    operator delete(data_);
}

template<typename T>
T &vector<T>::operator[](size_t i) {
    assert (i < size_);
    return data_[i];
}

template<typename T>
T const &vector<T>::operator[](size_t i) const {
    assert (i < size_);
    return data_[i];
}

template<typename T>
T *vector<T>::data() {
    return data_;
}

template<typename T>
T const *vector<T>::data() const {
    return data_;
}

template<typename T>
size_t vector<T>::size() const {
    return size_;
}

template<typename T>
T &vector<T>::front() {
    return data_[0];
}

template<typename T>
T const &vector<T>::front() const {
    return data_[0];
}

template<typename T>
T &vector<T>::back() {
    return data_[size_ - 1];
}

template<typename T>
T const &vector<T>::back() const {
    return data_[size_ - 1];
}

template<typename T>
void vector<T>::push_back(const T &value) {
    if (size_ < capacity_) {
        new(data_ + size_) T(value);
    } else {
        T tmp(value);
        reserve(increase_capacity());
        new(data_ + size_) T(tmp);
    }
    size_++;
}

template<typename T>
size_t vector<T>::increase_capacity() const {
    return capacity_ == 0 ? 1 : 2 * capacity_;
}

template<typename T>
void vector<T>::reserve(size_t len) {
    if (len > capacity_) {
        T *new_data = copy_data(size_, len, data_);
        del_data(size_, data_);
        size_t temp_size = size_;
        operator delete(data_);
        data_ = new_data;
        size_ = temp_size;
        capacity_ = len;
    }
}

template<typename T>
void vector<T>::pop_back() {
    data_[size_ - 1].~T();
    size_--;
}

template<typename T>
bool vector<T>::empty() const {
    return size_ == 0;
}

template<typename T>
size_t vector<T>::capacity() const {
    return capacity_;
}

template<typename T>
void vector<T>::shrink_to_fit() {
    if (capacity_ == size_) {
        return;
    }
    T *new_data = copy_data(size_, size_, data_);
    del_data(size_, data_);
    size_t temp_size = size_;
    operator delete(data_);  // MB BUG
    data_ = new_data;
    capacity_ = temp_size;
    size_ = temp_size;
}

template<typename T>
void vector<T>::clear() {
    del_data(size_, data_);
    size_ = 0;
}

template<typename T>
void vector<T>::del_data(size_t size, T *data) {
    for (size_t i = size; i != 0; i--) {
        data[i - 1].~T();
    }
}

template<typename T>
T *vector<T>::copy_data(size_t size, size_t capacity, T *other_data) {
    if (capacity < size) {
        capacity = size;
    }
    T *new_data = nullptr;
    if (capacity > 0) {
        new_data = static_cast<T *>(operator new(capacity * sizeof(T)));
    }
    size_t i = 0;
    try {
        for (; i < size; i++) {
            new(new_data + i) T(other_data[i]);
        }
    } catch (...) {
        del_data(i, new_data);
        operator delete(new_data);
        throw;
    }
    return new_data;
}

template<typename T>
typename vector<T>::iterator vector<T>::insert(vector::const_iterator pos, const T &value) {
    using std::swap;
    size_t it = pos - begin();
    push_back(value);
    for (int i = size_ - 1; i > it; i--) {
        swap(data_[i], data_[i - 1]);
    }
    return data_ + it;
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(vector::const_iterator pos) {
    return erase(pos, pos + 1);
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(vector::const_iterator first, vector::const_iterator last) {
    auto fst = first - begin();
    if (first > last) {
        return fst + begin();
    }
    std::ptrdiff_t sz = last - first;
    for (std::ptrdiff_t i = fst; i < size_ - sz; i++) {
        data_[i] = data_[i + sz];
    }
    for (std::ptrdiff_t i = 0; i < sz; i++) {
        pop_back();
    }

    return fst + begin();
}

template<typename T>
typename vector<T>::iterator vector<T>::begin() {
    return data_;
}

template<typename T>
typename vector<T>::iterator vector<T>::end() {
    return data_ + size_;
}

template<typename T>
typename vector<T>::const_iterator vector<T>::begin() const {
    return data_;
}

template<typename T>
typename vector<T>::const_iterator vector<T>::end() const {
    return data_ + size_;
}

#endif //VECTOR_H
