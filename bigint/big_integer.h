//
// Created by Yaroslav Ilin, M3138, KT ITMO y2019. 05.05.2020
//

#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H


#include <string>
#include <vector>
#include <algorithm>

__extension__ typedef unsigned __int128 uint128_t;

struct big_integer {
    big_integer();

    big_integer(int value);

    big_integer(big_integer const &other) = default;

    explicit big_integer(std::string const &str);

    ~big_integer();

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

    std::string to_string() const;

private:
    void swap(big_integer &other);

    void shrink_to_fit();

    friend big_integer div_bi_short(big_integer &, uint32_t);

    size_t size() const
    {
        return val.size();
    }

    friend void difference(big_integer  &, big_integer const &, size_t);

    friend bool smaller(big_integer const &, big_integer const &, size_t);

private:
    std::vector<uint32_t> val;
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

big_integer operator>>(big_integer a, int value);

big_integer operator<<(big_integer a, int value);

std::ostream &operator<<(std::ostream &os, big_integer const &other);

#endif //BIG_INTEGER_H
