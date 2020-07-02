//
// Created by Yaroslav Ilin, M3138, KT ITMO y2019. 05.05.2020
//

#include "big_integer.h"
#include <climits>
#include <algorithm>
#include <stdexcept>

big_integer::big_integer(int value) : sign(value >= 0 ? 1 : -1) {
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
    val = {tmp};
}

big_integer::big_integer(uint32_t value) {
    val = std::vector<uint32_t>(1, value);
    sign = 1;
}

big_integer::big_integer() {
    val = std::vector<uint32_t>(1, 0);
    sign = 1;
}

big_integer::big_integer(const std::string &str) : big_integer() {
    if (str == "0" || str.empty()) {
        return;
    }
    size_t i = 0;
    int tsign = 1;
    if (str[0] == '-') {
        tsign = -1;
        i = 1;
    }
    if (str[0] == '+') {
        tsign = 1;
        i = 1;
    }
    for (; i < str.size(); i++) {
        if (!(str[i] >= '0' && str[i] <= '9')) {
            throw std::invalid_argument("expected digit, found not digit at pos:" + std::to_string(i));
        }
        *this *= 10;
        *this += str[i] - '0';
    }
    sign = tsign;
}

big_integer &big_integer::operator=(const big_integer &other) {
    if (*this == other) {
        return *this;
    }
    big_integer tmp(other);
    swap(tmp);
    return *this;
}

big_integer &big_integer::operator+=(const big_integer &other) {
    if (sign == other.sign) {
        big_integer res;
        res.val.resize(std::max(size(), other.size()) + 1, 0);
        uint64_t cur = 0;
        uint64_t sum = 0;
        for (size_t i = 0; i < std::max(size(), other.size()); i++) {
            sum = cur;
            if (i < size()) {
                sum += val[i];
            }
            if (i < other.size()) {
                sum += other.val[i];
            }
            cur = sum >> 32;
            res.val[i] = static_cast<uint32_t>(sum);
        }
        res.val[std::max(size(), other.size())] = static_cast<uint32_t>(cur);
        res.sign = sign;
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
            *this = (other - *this);
            sign = -sign;
            return *this;
        }
        uint64_t cur = 0;
        val.reserve(std::max(size(), other.size()));
        for (size_t i = 0; i < std::max(size(), other.size()); i++) {
            if (i >= size()) {
                val[i] = 0;
            }
            uint64_t b = 0;
            if (i < other.size()) {
                b = 1LL * other.val[i];
            }
            cur += 1LL * val[i] - b;
            val[i] = static_cast<uint32_t>(cur + (1LL << 32));
            cur = ((cur + (1LL << 32)) >> 32) - 1;
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
    if (*this == 0 || other == 0) {
        *this = 0;
    }
    big_integer res;
    res.val.assign(size() + other.size(), 0);
    for (size_t i = 0; i < size(); i++) {
        uint32_t cur = 0;
        for (size_t j = 0; j < other.size(); j++) {
            uint64_t temp = static_cast<uint64_t>(val[i]) * other.val[j] + res.val[i + j] + cur;
            res.val[i + j] = static_cast<uint32_t> (temp);
            cur = static_cast<uint32_t> (temp >> 32);
        }
        res.val[i + other.size()] += cur;
    }
    res.sign = sign * other.sign;
    res.shrink_to_fit();
    *this = res;
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

uint32_t trial(big_integer const &a, big_integer const &b) {
    uint128_t t1 =
            (static_cast<uint128_t>(a.val[a.size() - 1]) << 64) + (static_cast<uint128_t>(a.val[a.size() - 2]) << 32) +
            static_cast<uint128_t>(a.val[a.size() - 3]);
    uint128_t t2 = (static_cast<uint128_t>(b.val[b.size() - 1]) << 32) + b.val[b.size() - 2];
    return std::min(static_cast<uint32_t>(t1 / t2), UINT32_MAX);
}

bool smaller(big_integer const &a, big_integer const &b, uint32_t k) {
    for (size_t i = 1; i <= a.size(); i++) {
        uint32_t tmp = k - i < b.size() ? b.val[k - i] : 0;
        if (a.val[a.size() - i] != tmp) {
            return a.val[a.size() - i] < tmp;
        }
    }
    return false;
}

void difference(big_integer &a, big_integer const &b, size_t k) {
    int borrow = 0;
    for (size_t i = 0; i < a.size() - k; i++) {
        uint32_t diff = a.val[i + k] - (i < b.size() ? b.val[i] : 0) - borrow;
        borrow = ((i < b.size() ? b.val[i] : 0) + borrow > a.val[i + k]);
        a.val[i + k] = static_cast<uint32_t> (diff);
    }
}

big_integer div_bi_short(big_integer &a, uint32_t b) {
    if (b == 0) {
        throw std::runtime_error("found divide by zero");
    }
    big_integer ans;
    ans.val.resize(a.size());
    uint64_t cur = 0;
    for (size_t i = 0; i < a.size(); i++) {
        uint64_t temp = (cur << 32) | a.val[a.size() - 1 - i];
        ans.val[i] = static_cast<uint32_t>((temp / b));
        cur = temp % b;
    }
    reverse(ans.val.begin(), ans.val.end());
    ans.shrink_to_fit();
    return ans;
}

big_integer &big_integer::operator/=(const big_integer &other) {
    int ans_sign = sign * other.sign;
    int tmp = sign;
    sign = other.sign;
    if ((sign == 1 && *this < other) || (sign == -1 && *this > other)) {
        *this = 0;
        return *this;
    }
    sign = tmp;
    big_integer a = *this;
    big_integer b = other;
    big_integer x;
    a.sign = 1;
    b.sign = 1;
    if (b.size() == 1) {
        x = div_bi_short(a, b.val[0]);
        x.sign = ans_sign;
        *this = x;
        return *this;
    }
    a.val.push_back(0);
    size_t n = a.size();
    size_t m = b.size() + 1;
    x.val.resize(n - m + 2);
    for (size_t k = n - m + 1; k != 0; k--) {
        uint32_t qt = trial(a, b);
        big_integer dq = b * qt;
        dq.val.push_back(0);
        if (smaller(a, dq, m)) {
            qt--;
            dq -= b;
        }
        x.val[k - 1] = qt;
        difference(a, dq, a.size() - m);
        if (!a.val.back()) a.val.pop_back();
    }
    x.shrink_to_fit();
    x.sign = ans_sign;
    *this = x;
    return *this;
}

big_integer &big_integer::operator%=(const big_integer &other) {
    *this = *this % other;
    return *this;
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


big_integer operator/(big_integer a, const big_integer &b) {
    a /= b;
    return a;
}

big_integer operator%(big_integer a, const big_integer &b) {
    return a - ((a / b) * b);
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

void calc_func(const big_integer &a, const big_integer &b, big_integer &ans, uint32_t (*f)(uint32_t, uint32_t)) {
    for (size_t i = 0; i < a.size(); i++) {
        ans.val[i] = (*f)(a.val[i], b.val[i]);
    }
}

big_integer b_op(const big_integer &a, const big_integer &b, uint32_t (*f)(uint32_t, uint32_t)) {
    size_t max_size = std::max(a.size(), b.size());
    big_integer ans;
    ans.add_up(max_size);
    if (a.sign == 1 && b.sign == 1 && a.size() == b.size()) {
        calc_func(a, b, ans, f);
    } else {
        big_integer x = a;
        big_integer y = b;
        x.add_up(max_size);
        y.add_up(max_size);
        ans.add_up(max_size);
        calc_func(x, y, ans, f);
    }
    if ((*f)(static_cast<uint32_t>(a.sign == 1 ? 0 : 1), static_cast<uint32_t>(b.sign == 1 ? 0 : 1))) {
        ans.sign = -ans.sign;
        ans.add_up(max_size);
        ans.sign = -ans.sign;
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

big_integer &big_integer::operator>>=(int value) {
    if (value < 0) {
        *this = *this << -value;
        return *this;
    }
    *this /= static_cast<uint32_t>(1) << (value % 32);
    reverse(val.begin(), val.end());
    for (int i = 0; i < value / 32 && size() != 0; i++) {
        val.pop_back();
    }
    reverse(val.begin(), val.end());
    if (sign == -1) {
        *this -= 1;
    }
    return *this;
}

big_integer &big_integer::operator<<=(int value) {
    if (value < 0) {
        *this = *this >> -value;
        return *this;
    }
    *this *= (static_cast<uint32_t>(1) << (value % 32));
    reverse(val.begin(), val.end());
    for (int i = 0; i < value / 32; i++) {
        val.push_back(0);
    }
    reverse(val.begin(), val.end());
    return *this;
}

big_integer &big_integer::operator&=(const big_integer &other) {
    *this = b_op(*this, other, b_and);
    return *this;
}

big_integer &big_integer::operator|=(const big_integer &other) {
    *this = b_op(*this, other, b_or);
    return *this;
}

big_integer &big_integer::operator^=(const big_integer &other) {
    *this = b_op(*this, other, b_xor);
    return *this;
}

big_integer operator&(big_integer a, const big_integer &b) {
    a &= b;
    return a;
}

big_integer operator|(big_integer a, const big_integer &b) {
    a |= b;
    return a;
}

big_integer operator^(big_integer a, const big_integer &b) {
    a ^= b;
    return a;
}

std::ostream &operator<<(std::ostream &os, big_integer const &other) {
    os << to_string(other);
    return os;
}

big_integer operator>>(big_integer a, int value) {
    a >>= value;
    return a;
}

big_integer operator<<(big_integer a, int value) {
    a <<= value;
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