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

    iterator begin() {
        return data_;
    }                                        // O(1) nothrow

    iterator end() {
        return data_ + size_;
    }                                        // O(1) nothrow

    const_iterator begin() const {           // O(1) nothrow
        return data_;
    }

    const_iterator end() const {             // O(1) nothrow
        return data_ + size_;
    }

    iterator insert(const_iterator pos, T const &); // O(N) weak

    iterator erase(const_iterator pos);     // O(N) weak

    iterator erase(const_iterator first, const_iterator last); // O(N) weak

private:
    size_t increase_capacity() const;

    void push_back_realloc(T const &);

private:
    T *data_;
    size_t size_;
    size_t capacity_;
};

template<typename T>
vector<T>::vector() {
    size_ = 0;
    capacity_ = 0;
    data_ = nullptr;
}

template<typename T>
vector<T>::vector(const vector &other) : vector() {
    T *new_data = nullptr;
    if (other.size_ > 0) {
        new_data = static_cast<T *>(operator new(other.size_ * sizeof(T)));
    }
    size_t i = 0;
    try {
        for (; i < other.size_; i++) {
            new(new_data + i) T(other.data_[i]);
        }
    } catch (...) {
        for (; i != 0; i--) {
            new_data[i - 1].~T();
        }
        operator delete(new_data);
        throw;
    }
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
    for (size_t i = 0; i < size_; i++) {
        data_[i].~T();
    }
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
        push_back_realloc(value);
    }
    size_++;
}

template<typename T>
size_t vector<T>::increase_capacity() const {
    if (capacity_ == 0) {
        return 1;
    }
    return 2 * capacity_;
}

template<typename T>
void vector<T>::push_back_realloc(const T &value) {
    T tmp(value);
    reserve(increase_capacity());
    new(data_ + size_) T(tmp);
}

template<typename T>
void vector<T>::reserve(size_t len) {
    if (len > capacity_) {
        vector<T> temp;
        temp.data_ = static_cast<T *>(operator new(len * sizeof(T)));
        temp.capacity_ = len;
        for (size_t i = 0; i < size_; i++) {
            temp.push_back(data_[i]);
        }
        swap(temp);
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
    T *new_data = nullptr;
    if (size_ > 0) {
        new_data = static_cast<T *>(operator new(size_ * sizeof(T)));
    }
    for (size_t i = 0; i < size_; i++) {
        new(new_data + i) T(data_[i]);
    }
    for (size_t i = 0; i < size_; i++) {
        data_[i].~T();
    }
    size_t temp_size = size_;
    operator delete(data_);  // MB BUG
    data_ = new_data;
    capacity_ = temp_size;
    size_ = temp_size;
}

template<typename T>
void vector<T>::clear() {
    for (size_t i = 0; i < size_; i++) {
        data_[i].~T();
    }
    size_ = 0;
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
    iterator it = (pos - begin()) + begin();
    iterator ret = it + 1;
    while (it + 1 != end()) {
        std::swap(*it, *(it + 1));
        ++it;
    }
    pop_back();
    return data_ + (it - begin());
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(vector::const_iterator first, vector::const_iterator last) {
    vector new_data;
    for (size_t i = 0; begin() + i != first; i++) {
        new_data.push_back(data_[i]);
    }
    iterator ret = new_data.end();
    for (size_t i = last - begin(); begin() + i != end(); i++) {
        new_data.push_back(data_[i]);
    }
    swap(new_data);
    return ret;
}

#endif //VECTOR_H
