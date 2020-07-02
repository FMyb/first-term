//
// Created by Yaroslav Ilin, M3138, KT ITMO y2019. 05.05.2020
//

#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H


#include <string>
#include "uint_vector.h"
#include <algorithm>

__extension__ typedef unsigned __int128 uint128_t;

struct big_integer {
    big_integer();

    big_integer(int value);

    big_integer(uint32_t);

    big_integer(big_integer const &other) = default;

    explicit big_integer(std::string const &str);

    ~big_integer() = default;

    big_integer &operator=(big_integer const &other);

    big_integer &operator+=(big_integer const &other);

    big_integer &operator-=(big_integer const &other);

    big_integer &operator*=(big_integer const &other);

    big_integer &operator/=(big_integer const &other);

    big_integer &operator%=(big_integer const &other);

    big_integer &operator&=(big_integer const &other);

    big_integer &operator|=(big_integer const &other);

    big_integer &operator^=(big_integer const &other);

    big_integer &operator<<=(int value);

    big_integer &operator>>=(int value);

    big_integer operator+() const;

    big_integer operator-() const;

    big_integer operator~() const;

    big_integer &operator++();

    big_integer operator++(int);

    big_integer &operator--();

    big_integer operator--(int);

    friend bool operator==(big_integer const &, big_integer const &);

    friend bool operator!=(big_integer const &, big_integer const &);

    friend bool operator<(big_integer const &, big_integer const &);

    friend bool operator>(big_integer const &, big_integer const &);

    friend bool operator<=(big_integer const &, big_integer const &);

    friend bool operator>=(big_integer const &, big_integer const &);

    friend big_integer operator<<(big_integer a, int value);

    friend big_integer operator>>(big_integer a, int value);

    friend std::string to_string(big_integer const& a);

private:

    void swap(big_integer &other);

    void shrink_to_fit();

    friend uint32_t trial(big_integer const &, big_integer const &);

    friend void difference(big_integer &, big_integer const &, size_t);

    friend big_integer div_bi_short(big_integer &, uint32_t);

    friend bool smaller(big_integer const &, big_integer const &, uint32_t);

    friend big_integer b_op(big_integer const &, big_integer const &, uint32_t (*f)(uint32_t, uint32_t));

    friend void calc_func(const big_integer &, const big_integer &, big_integer &, uint32_t (*f)(uint32_t, uint32_t));

    void add_up(size_t);

    size_t size() const {
        return val.size();
    }

private:
    uint_vector val;
    int sign;
};

big_integer operator+(big_integer a, big_integer const &b);

big_integer operator-(big_integer a, big_integer const &b);

big_integer operator*(big_integer a, big_integer const &b);

big_integer operator/(big_integer a, big_integer const &b);

big_integer operator%(big_integer a, big_integer const &b);

big_integer operator&(big_integer a, big_integer const &b);

big_integer operator|(big_integer a, big_integer const &b);

big_integer operator^(big_integer a, big_integer const &b);

std::ostream &operator<<(std::ostream &os, big_integer const &other);

#endif //BIG_INTEGER_H
