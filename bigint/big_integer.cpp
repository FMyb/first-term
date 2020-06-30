//
// Created by Yaroslav Ilin, M3138, KT ITMO y2019. 05.05.2020
//

#include "big_integer.h"
#include <climits>
#include <algorithm>

big_integer::big_integer(int value) {
    uint32_t tmp;
    if (value < 0) {
        if (value == INT_MIN) {
            tmp = static_cast<uint32_t>(INT_MAX) + 1;
        } else {
            tmp = -value;
        }
    } else {
        tmp = value;
    }
    val = std::vector<uint32_t>(1, tmp);
    sign = value >= 0 ? 1 : -1;
}

big_integer::big_integer(uint32_t value) {
    val = std::vector<uint32_t>(1, value);
    sign = 1;
}

big_integer::big_integer() {
    val = std::vector<uint32_t>(1, 0);
    sign = 1;
}


big_integer::~big_integer() {
    val.clear(); // TODO
}

big_integer::big_integer(const std::string &str) {
    *this = big_integer(0);
    if (str == "0") {
        return;
    }
    size_t i = 0;
    int tsign = 1;
    if (str[0] == '-') {
        tsign = -1;
        i = 1;
    }
    for (; i < str.size(); i++) {
        *this *= 10;
        *this += str[i] - '0';
    }
    sign = tsign;
}

big_integer &big_integer::operator=(const big_integer &other) {
    big_integer tmp(other);
    swap(tmp);
    return *this;
}

big_integer &big_integer::operator+=(const big_integer &other) {
    if (sign == other.sign) {
        big_integer res;
        res.val.resize(std::max(size(), other.size()) + 1, 0);
        long long cur = 0;
        long long sum = 0;
        for (size_t i = 0; i < std::max(size(), other.size()); i++) {
            sum = cur;
            if (i < size()) {
                sum += val[i];
            }
            if (i < other.size()) {
                sum += other.val[i];
            }
            cur = sum / (1LL << 32);
            res.val[i] = static_cast<uint32_t>(sum % (1LL << 32));
        }
        res.val[std::max(size(), other.size())] = static_cast<uint32_t>(cur);
        res.sign = sign;
        res.shrink_to_fit();
        *this = res;
    } else {
        (sign == -1 ? *this = other - (-*this) : *this -= -other);
    }
    shrink_to_fit();
    return *this;
}

big_integer &big_integer::operator-=(const big_integer &other) {
    if (other.sign == 1 || other == 0) {
        if (*this < other) {
            *this = -(other - *this);
            return *this;
        }
        long long cur = 0;
        long long b = 0;
        for (size_t i = 0; i < std::max(size(), other.size()); i++) {
            if (i >= size()) {
                val.push_back(0);
            }
            if (i >= other.size()) {
                b = 0;
            } else {
                b = 1LL * other.val[i];
            }
            cur += 1LL * val[i] - b;
            val[i] = static_cast<uint32_t>((cur + (1LL << 32)) % (1LL << 32));
            cur = ((cur + (1LL << 32)) / (1LL << 32)) - 1;
        }
        if (cur != 0) {
            val.push_back(cur);
            sign = 0 - sign;
        }
    } else {
        *this += -other;
    }
    shrink_to_fit();
    return *this;
}

big_integer big_integer::operator-() const {
    if (*this == 0) {
        return *this;
    }
    big_integer ret(*this);
    ret.sign = -sign;
    return ret;
}

big_integer big_integer::operator+() const {
    return big_integer(*this);
}

big_integer &big_integer::operator*=(const big_integer &other) {
    if (*this == 0 || other == 0) *this = 0;
    big_integer res;
    res.val.assign(size() + other.size(), 0);
    for (size_t i = 0; i < size(); i++) {
        uint32_t cur = 0;
        for (size_t j = 0; j < other.size(); j++) {
            uint64_t temp = (uint64_t) val[i] * other.val[j] + res.val[i + j] + cur;
            res.val[i + j] = static_cast<uint32_t> ((temp % (1LL << 32)));
            cur = static_cast<uint32_t> (temp / (1LL << 32));
        }
        res.val[i + other.size()] += cur;
    }
    res.sign = sign * other.sign;
    res.shrink_to_fit();
    *this = res;
    return *this;
}

big_integer &big_integer::operator<<=(int value) {
    *this = *this << value;
    return *this;
}

void big_integer::shrink_to_fit() {
    while (size() > 1 && val.back() == 0) {
        val.pop_back();
    }
}

void big_integer::swap(big_integer &other) {
    std::swap(val, other.val);
    std::swap(sign, other.sign);
}

bool operator==(const big_integer &a, const big_integer &b) {
    if (a.sign == b.sign) {
        return a.val == b.val;
    } else {
        return a.size() == 1 && a.val[0] == 0 && b.size() == 1 && b.val[0] == 0;
    }
}


void difference(big_integer &a, big_integer const &b, size_t index) {
    size_t start = a.size() - index;
    bool borrow = false;
    for (size_t i = 0; i < index; ++i) {
        uint32_t x = a.val[start + i];
        uint32_t y = (i < b.size() ? b.val[i] : 0);
        uint64_t c = static_cast<uint64_t> (x - y - borrow);
        borrow = (y + borrow > x);
        c %= (1LL << 32);
        a.val[start + i] = static_cast<uint32_t> (c);
    }
}

big_integer div_bi_short(big_integer &a, uint32_t b) {
    big_integer ans;
    uint64_t cur = 0;
    uint64_t temp = 0;
    for (size_t i = 0; i < a.size(); i++) {
        temp = (cur << 32) | a.val[a.size() - 1 - i];
        ans.val.push_back(static_cast<uint32_t>((temp / b)));
        cur = temp % b;
    }
    reverse(ans.val.begin(), ans.val.end());
    ans.shrink_to_fit();
    return ans;
}


bool smaller(big_integer const &a, big_integer const &b, size_t ind) {
    for (size_t i = 1; i <= a.size(); i++) {
        uint32_t tmp = ind - i < b.size() ? b.val[ind - i] : 0;
        if (a.val[a.size() - i] != tmp) {
            return a.val[a.size() - i] > tmp;
        }
    }
    return true;
}

big_integer &big_integer::operator/=(const big_integer &other) {
    big_integer a = *this;
    big_integer b = other;
    big_integer tmp;
    big_integer dq;
    a.sign = 1;
    b.sign = 1;
    if (a < b) {
        *this = 0;
        return *this;
    }
    if (b.size() == 1) {
        tmp = div_bi_short(a, b.val[0]);
        tmp.sign = sign * other.sign;
        *this = tmp;
        return *this;
    }
    a.val.push_back(0);
    size_t m = b.size() + 1;
    size_t n = a.size();
    tmp.val.resize(n - m + 1);
    uint32_t qt = 0;
    for (size_t i = m, j = tmp.val.size() - 1; i <= n; ++i, --j) {
        uint128_t x = ((static_cast<uint128_t> (a.val[a.size() - 1]) << 64) |
                       (static_cast<uint128_t> (a.val[a.size() - 2]) << 32) |
                       (static_cast<uint128_t> (a.val[a.size() - 3])));
        uint128_t y = ((static_cast<uint128_t> (b.val[b.size() - 1]) << 32) |
                       static_cast<uint128_t> (b.val[b.size() - 2]));
        qt = std::min(static_cast<uint32_t> ((x / y)), UINT32_MAX);
        dq = b * qt;
        if (!smaller(a, dq, m)) {
            qt--;
            dq -= b;
        }
        tmp.val[j] = qt;
        difference(a, dq, m);
        if (!a.val.back()) a.val.pop_back();
    }
    tmp.shrink_to_fit();
    tmp.sign = sign * other.sign;
    *this = tmp;
    return *this;
}

bool operator!=(const big_integer &a, const big_integer &b) {
    return !(a == b);
}

bool operator<(const big_integer &a, const big_integer &b) {
    if (a.sign != b.sign) {
        return a.sign < 0;
    }
    if (a.sign == -1) {
        return -a > -b;
    }
    if (a.size() != b.size()) {
        return a.size() < b.size();
    }
    size_t i = a.size();
    while (i--) {
        if (a.val[i] != b.val[i]) {
            return a.val[i] < b.val[i];
        }
    }
    return false;
}

bool operator>(const big_integer &a, const big_integer &b) {
    return b < a;
}

big_integer &big_integer::operator%=(const big_integer &other) {
    *this = *this % other;
    return *this;
}

big_integer &big_integer::operator>>=(int value) {
    *this = *this >> value;
    return *this;
}

bool operator<=(const big_integer &a, const big_integer &b) {
    return a < b || a == b;
}

bool operator>=(const big_integer &a, const big_integer &b) {
    return a > b || a == b;
}

big_integer big_integer::operator~() const {
    return -(*this) - 1;
}

big_integer &big_integer::operator++() {
    return (*this += 1);
}

big_integer &big_integer::operator--() {
    return (*this -= 1);
}

big_integer big_integer::operator++(int) {
    auto ret = *this;
    *this += 1;
    return ret;
}

big_integer big_integer::operator--(int) {
    auto ret = *this;
    *this -= 1;
    return ret;
}

uint32_t b_and(uint32_t a, uint32_t b) {
    return a & b;
}

uint32_t b_or(uint32_t a, uint32_t b) {
    return a | b;
}

uint32_t b_xor(uint32_t a, uint32_t b) {
    return a ^ b;
}

big_integer b_op(const big_integer &a, const big_integer &b, uint32_t (*f)(uint32_t, uint32_t)) {
    big_integer x = a;
    big_integer y = b;
    big_integer ans;
    size_t max_size = std::max(a.size(), b.size());
    x.add_up(max_size);
    y.add_up(max_size);
    ans.add_up(max_size);
    for (size_t i = 0; i < max_size; i++) {
        ans.val[i] = (*f)(x.val[i], y.val[i]);
    }
    if ((*f)(static_cast<uint32_t>(a.sign == 1 ? 0 : 1), static_cast<uint32_t>(b.sign == 1 ? 0 : 1))) {
        ans = -ans;
        ans.add_up(max_size);
        ans = -ans;
    }
    return ans;
}

void big_integer::add_up(size_t n) {
    for (size_t i = size(); i < n; i++) {
        val.push_back(0);
    }
    if (sign == -1) {
        sign = 1;
        for (size_t i = 0; i < n; i++) {
            val[i] = ~val[i];
        }
        *this += 1;
    }
}

big_integer &big_integer::operator&=(const big_integer &other) {
    *this = *this & other;
    return *this;
}

big_integer &big_integer::operator|=(const big_integer &other) {
    *this = *this | other;
    return *this;
}

big_integer &big_integer::operator^=(const big_integer &other) {
    *this = *this ^ other;
    return *this;
}

big_integer operator&(big_integer a, const big_integer &b) {
    return b_op(a, b, b_and);
}

big_integer operator|(big_integer a, const big_integer &b) {
    return b_op(a, b, b_or);
}

big_integer operator^(big_integer a, const big_integer &b) {
    return b_op(a, b, b_xor);
}

std::ostream &operator<<(std::ostream &os, big_integer const &other) {
    os << to_string(other);
    return os;
}

big_integer operator+(big_integer a, const big_integer &b) {
    return a += b;
}

big_integer operator-(big_integer a, const big_integer &b) {
    return a -= b;
}

big_integer operator*(big_integer a, const big_integer &b) {
    return a *= b;
}

big_integer operator>>(big_integer a, int value) {
    if (value < 0) return a << -value;
    a /= static_cast<uint32_t>(1) << (value % 32);
    reverse(a.val.begin(), a.val.end());
    for (int i = 0; i < value / 32 && a.size(); i++) {
        a.val.pop_back();
    }
    reverse(a.val.begin(), a.val.end());
    if (a.sign == -1) {
        return a - 1;
    }
    return a;
}

big_integer operator<<(big_integer a, int value) {
    if (value < 0) return a >> -value;
    a *= (static_cast<uint32_t>(1) << (value % 32));
    reverse(a.val.begin(), a.val.end());
    for (int i = 0; i < value / 32; i++) {
        a.val.push_back(0);
    }
    reverse(a.val.begin(), a.val.end());
    return a;
}

big_integer operator%(big_integer a, const big_integer &b) {
    return a - ((a / b) * b);
}

big_integer operator/(big_integer a, const big_integer &b) {
    a /= b;
    return a;
}

std::string to_string(const big_integer &a) {
    if (a == 0) {
        return "0";
    }
    std::string ans;
    big_integer tmp = a;
    while (tmp != 0) {
        ans += char('0' + (tmp % 10).val[0]);
        tmp /= 10;
    }
    if (a.sign == -1) ans += '-';
    reverse(ans.begin(), ans.end());
    return ans;
}
