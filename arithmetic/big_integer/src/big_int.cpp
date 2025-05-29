#include "../include/big_int.h"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>

unsigned long long BASE = 1ULL << (8 * sizeof(unsigned int));

bool is_zero(const std::vector<unsigned int, pp_allocator<unsigned int>> &digits) {
	return digits.size() == 1 && digits[0] == 0;
}

void optimise(std::vector<unsigned int, pp_allocator<unsigned int>> &digits) {
	while (digits.size() > 1 && digits.back() == 0) {
		digits.pop_back();
	}
}

std::strong_ordering big_int::operator<=>(const big_int &other) const noexcept {
	if (_sign != other._sign) return _sign ? std::strong_ordering::greater : std::strong_ordering::less;

	bool is_pos = _sign;
	if (_digits.size() != other._digits.size()) {
		return is_pos ? _digits.size() <=> other._digits.size() : other._digits.size() <=> _digits.size();
	}

	for (int i = static_cast<int>(_digits.size()) - 1; i >= 0; --i) {
		if (_digits[i] != other._digits[i]) {
			return is_pos ? _digits[i] <=> other._digits[i] : other._digits[i] <=> _digits[i];
		}
	}

	return std::strong_ordering::equal;
}

big_int::operator bool() const noexcept {
	return !is_zero(_digits);
}

big_int &big_int::operator++() & {
	*this += big_int(1, _digits.get_allocator());
	return *this;
}

big_int big_int::operator++(int) {
	auto tmp = *this;
	++(*this);
	return tmp;
}

big_int &big_int::operator--() & {
	*this -= big_int(1, _digits.get_allocator());
	return *this;
}

big_int big_int::operator--(int) {
	auto tmp = *this;
	--(*this);
	return tmp;
}

big_int &big_int::operator+=(const big_int &other) & {
	return plus_assign(other, 0);
}

big_int &big_int::operator-=(const big_int &other) & {
	return minus_assign(other, 0);
}

big_int big_int::operator+(const big_int &other) const {
	big_int tmp = *this;
	return tmp += other;
}

big_int big_int::operator-(const big_int &other) const {
	big_int tmp = *this;
	return tmp -= other;
}

big_int big_int::operator*(const big_int &other) const {
	big_int tmp = *this;
	return tmp *= other;
}

big_int big_int::operator/(const big_int &other) const {
	big_int tmp = *this;
	return tmp /= other;
}

big_int big_int::operator%(const big_int &other) const {
	big_int tmp = *this;
	return tmp %= other;
}

big_int big_int::operator&(const big_int &other) const {
	big_int tmp = *this;
	return tmp &= other;
}

big_int big_int::operator|(const big_int &other) const {
	big_int tmp = *this;
	return tmp |= other;
}

big_int big_int::operator^(const big_int &other) const {
	big_int tmp = *this;
	return tmp ^= other;
}

big_int big_int::operator<<(size_t shift) const {
	big_int tmp = *this;
	return tmp <<= shift;
}

big_int big_int::operator>>(size_t shift) const {
	big_int tmp = *this;
	return tmp >>= shift;
}

big_int &big_int::operator%=(const big_int &other) & {
	return modulo_assign(other, decide_div(other._digits.size()));
}

big_int big_int::operator~() const {
	big_int result(*this);
	for (auto &digit: result._digits) {
		digit = ~digit;
	}

	optimise(result._digits);
	return result;
}

big_int &big_int::operator&=(const big_int &other) & {
	const size_t max_size = std::max(_digits.size(), other._digits.size());
	_digits.resize(max_size, 0);
	for (size_t i = 0; i < max_size; ++i) {
		_digits[i] &= (i < other._digits.size()) ? other._digits[i] : 0;
	}

	optimise(_digits);
	return *this;
}

big_int &big_int::operator|=(const big_int &other) & {
	const size_t max_size = std::max(_digits.size(), other._digits.size());
	_digits.resize(max_size, 0);
	for (size_t i = 0; i < max_size; ++i) {
		_digits[i] |= (i < other._digits.size()) ? other._digits[i] : 0;
	}

	optimise(_digits);
	return *this;
}

big_int &big_int::operator^=(const big_int &other) & {
	const size_t max_size = std::max(_digits.size(), other._digits.size());
	_digits.resize(max_size, 0);
	for (size_t i = 0; i < max_size; ++i) {
		_digits[i] ^= (i < other._digits.size()) ? other._digits[i] : 0;
	}

	optimise(_digits);
	return *this;
}

big_int &big_int::operator<<=(size_t shift) & {
	if (shift == 0 || (_digits.size() == 1 && _digits[0] == 0)) return *this;

	const size_t bits_count = sizeof(unsigned int) << 3;

	size_t int_shift = shift / bits_count;
	size_t bit_shift = shift % bits_count;

	if (int_shift > 0) {
		_digits.insert(_digits.begin(), int_shift, 0);
	}

	if (bit_shift > 0) {
		unsigned int carry = 0;
		for (size_t i = 0; i < _digits.size(); ++i) {
			unsigned int current = _digits[i];
			_digits[i] = (current << bit_shift) | carry;
			carry = current >> (bits_count - bit_shift);
		}

		if (carry != 0) {
			_digits.push_back(carry);
		}
	}

	optimise(_digits);
	return *this;
}


big_int &big_int::operator>>=(size_t shift) & {
	if (shift == 0 || (_digits.size() == 1 && _digits[0] == 0)) return *this;

	const size_t bits_count = sizeof(unsigned int) << 3;

	size_t int_shift = shift / bits_count;
	size_t bit_shift = shift % bits_count;

	if (int_shift >= _digits.size()) {
		_digits.clear();
		_digits.push_back(0);
		_sign = true;
		return *this;
	}

	if (int_shift > 0) {
		_digits.erase(_digits.begin(), _digits.begin() + int_shift);
	}

	if (bit_shift > 0) {
		unsigned int carry = 0;
		for (int i = static_cast<int>(_digits.size()) - 1; i >= 0; --i) {
			unsigned int current = _digits[i];
			_digits[i] = (current >> bit_shift) | carry;
			carry = current << (bits_count - bit_shift);
		}
	}

	optimise(_digits);
	if (is_zero(_digits)) {
		_sign = true;
	}

	return *this;
}


big_int &big_int::minus_assign(const big_int &other, size_t shift) & {
    if (is_zero(other._digits)) return *this;

    if (_sign != other._sign) {
        big_int temp(other);
        temp._sign = _sign;
        return plus_assign(temp, shift);
    }

    big_int abs_this(*this);
    abs_this._sign = true;
    big_int abs_other(other);
    abs_other._sign = true;
    abs_other <<= shift;

    bool result_sign = _sign;
    if (abs_this < abs_other) {
        result_sign = !result_sign;
        std::swap(abs_this._digits, abs_other._digits);
    }

    size_t max_size = std::max(abs_this._digits.size(), abs_other._digits.size());
    std::vector<unsigned int, pp_allocator<unsigned int>> result(max_size, 0, _digits.get_allocator());

    constexpr unsigned int half_bits = sizeof(unsigned int) << 2;
    constexpr unsigned int half_mask = (1u << half_bits) - 1;

    unsigned int borrow = 0;
    for (size_t i = 0; i < max_size; ++i) {
        unsigned int a = i < abs_this._digits.size() ? abs_this._digits[i] : 0;
        unsigned int b = i < abs_other._digits.size() ? abs_other._digits[i] : 0;

        unsigned int low_a = a & half_mask;
        unsigned int low_b = b & half_mask;
        unsigned int high_a = a >> half_bits;
        unsigned int high_b = b >> half_bits;

        int low_diff = static_cast<int>(low_a) - static_cast<int>(low_b) - (borrow & half_mask);
        borrow = 0;
        if (low_diff < 0) {
            low_diff += (1 << half_bits);
            borrow = 1;
        }

        int high_diff = static_cast<int>(high_a) - static_cast<int>(high_b) - borrow;
        if (high_diff < 0) {
            high_diff += (1 << half_bits);
            borrow = 1;
        } else {
            borrow = 0;
        }

        result[i] = (static_cast<unsigned int>(high_diff) << half_bits) | static_cast<unsigned int>(low_diff);
    }

    _digits = std::move(result);
    _sign = result_sign;
    if (is_zero(_digits)) _sign = true;

	optimise(_digits);
    return *this;
}


big_int &big_int::operator*=(const big_int &other) & {
	return multiply_assign(other, decide_mult((other._digits.size())));
}

big_int &big_int::plus_assign(const big_int &other, size_t shift) & {
	if (is_zero(other._digits)) return *this;

	if (is_zero(_digits)) {
		*this = other;
		if (shift > 0) {
			_digits.insert(_digits.begin(), shift, 0);
		}
		return *this;
	}

	if (_sign == other._sign) {
		size_t max_size = std::max(_digits.size(), other._digits.size() + shift);
		_digits.resize(max_size + 1, 0);

		constexpr int half_bits = sizeof(unsigned int) << 2;
		constexpr unsigned int half_mask = (1u << half_bits) - 1;

		unsigned int carry = 0;
		for (size_t i = 0; i < max_size; ++i) {
			unsigned int a = _digits[i];
			unsigned int b = (i >= shift && (i - shift) < other._digits.size()) ? other._digits[i - shift] : 0;

			unsigned int low_a = a & half_mask;
			unsigned int low_b = b & half_mask;
			unsigned int high_a = a >> half_bits;
			unsigned int high_b = b >> half_bits;

			unsigned int low_sum = low_a + low_b + (carry & half_mask);
			carry = low_sum >> half_bits;

			unsigned int high_sum = high_a + high_b + (carry & half_mask);
			carry = high_sum >> half_bits;

			_digits[i] = ((high_sum & half_mask) << half_bits) | (low_sum & half_mask);
		}

		if (carry != 0) {
			_digits[max_size] = carry;
		}
	} else {
		big_int temp = other;
		temp._sign = _sign;
		minus_assign(temp, shift);
	}

	optimise(_digits);

	if (is_zero(_digits)) _sign = true;

	return *this;
}


big_int &big_int::operator/=(const big_int &other) & {
	return divide_assign(other, decide_div(other._digits.size()));
}

std::string big_int::to_string() const {
	if (is_zero(_digits)) return "0";

	std::string str;
	big_int tmp = *this;
	tmp._sign = true;
	while (tmp) {
		auto val = tmp % big_int(10, _digits.get_allocator());
		str += static_cast<char>('0' + val._digits[0]);
		tmp /=  big_int(10, _digits.get_allocator());
	}

	if (!_sign) str += '-';
	std::reverse(str.begin(), str.end());
	return str;
}

std::ostream &operator<<(std::ostream &stream, const big_int &value) {
	stream << value.to_string();
	return stream;
}

std::istream &operator>>(std::istream &stream, big_int &value) {
	std::string val;
	stream >> val;
	value = big_int(val, 10, value._digits.get_allocator());
	return stream;
}

bool big_int::operator==(const big_int &other) const noexcept {
	return std::strong_ordering::equal == (*this <=> other);
}

big_int::big_int(const std::vector<unsigned int, pp_allocator<unsigned int>> &digits, bool sign) : _digits(digits), _sign(sign) {
	if (_digits.empty()) {
		_digits.push_back(0);
	}
	optimise(_digits);
}

big_int::big_int(std::vector<unsigned int, pp_allocator<unsigned int>> &&digits, bool sign) noexcept : _digits(std::move(digits)), _sign(sign) {
	if (_digits.empty()) {
		_digits.push_back(0);
	}
	optimise(_digits);
}


big_int::big_int(const std::string &num, unsigned int radix, pp_allocator<unsigned int> allocator) : _sign(true), _digits(allocator) {
	if (num.empty()) {
		_digits.push_back(0);
		return;
	}

	std::string num1 = num;
	bool is_neg = false;
	if (num1[0] == '-') {
		is_neg = true;
		num1 = num1.substr(1);
	} else if (num1[0] == '+') {
		num1 = num1.substr(1);
	}

	while (num1.size() > 1 && num1[0] == '0') {
		num1 = num1.substr(1);
	}

	if (num1.empty()) {
		_digits.push_back(0);
		return;
	}

	_digits.push_back(0);
	for (const char ch: num1) {
		if (!std::isdigit(ch)) {
			throw std::logic_error("Error: not a number in number string during big int initialization");
		}

		const unsigned int digit = ch - '0';
		*this *= big_int(10, allocator);
		*this += big_int(static_cast<long long>(digit), allocator);
	}

	_sign = !is_neg;
	if (is_zero(_digits)) {
		_sign = true;
	}
}

big_int::big_int(pp_allocator<unsigned int> allocator) : _digits(allocator), _sign(true) {
	_digits.push_back(0);
}

big_int &big_int::multiply_assign(const big_int &other, multiplication_rule rule) & {
	if (is_zero(_digits) || is_zero(other._digits)) {
		_digits.assign(1, 0);
		_sign = true;
		return *this;
	}

	if (rule == multiplication_rule::Karatsuba) {
		big_int result = multiply_karatsuba(*this, other);
		_digits = std::move(result._digits);
		_sign = (_sign == other._sign);
		optimise(_digits);
		return *this;
	}

	constexpr unsigned int half_bits = sizeof(unsigned int) << 2;
	constexpr unsigned int half_mask = (1u << half_bits) - 1;

	big_int result(_digits.get_allocator());
	result._digits.assign(1, 0);

	for (size_t j = 0; j < other._digits.size(); ++j) {
		unsigned int b = other._digits[j];
		if (b == 0) continue;

		unsigned int b_low = b & half_mask;
		unsigned int b_high = b >> half_bits;

		std::vector<unsigned int, pp_allocator<unsigned int>> temp(_digits.size() + 1, 0, _digits.get_allocator());

		unsigned int carry = 0;

		for (size_t i = 0; i < _digits.size(); ++i) {
			unsigned int a = _digits[i];
			unsigned int a_low = a & half_mask;
			unsigned int a_high = a >> half_bits;

			unsigned int prod_ll = a_low * b_low;
			unsigned int prod_lh = a_low * b_high;
			unsigned int prod_hl = a_high * b_low;
			unsigned int prod_hh = a_high * b_high;

			// sum and shift
			unsigned int low = (prod_ll & half_mask) + carry;
			unsigned int mid = (prod_ll >> half_bits) + (prod_lh & half_mask) + (prod_hl & half_mask) + (low >> half_bits);
			low &= half_mask;
			unsigned int high = (prod_lh >> half_bits) + (prod_hl >> half_bits) + prod_hh + (mid >> half_bits);

			unsigned int combined = (mid & half_mask) << half_bits | low;
			carry = high;
			temp[i] = combined;
		}

		temp[_digits.size()] = carry;

		big_int temp_big;
		temp_big._digits = std::move(temp);
		temp_big._sign = true;

		result.plus_assign(temp_big, j);
	}

	_digits = std::move(result._digits);
	_sign = (_sign == other._sign);
	optimise(_digits);
	return *this;
}



big_int &big_int::divide_assign(const big_int &other, division_rule rule) & {
	if (is_zero(_digits)) return *this;
	if (is_zero(other._digits)) throw std::logic_error("Division by zero");

	big_int abs_this = *this;
	abs_this._sign = true;
	big_int abs_other = other;
	abs_other._sign = true;

	if (abs_this < abs_other) {
		_digits.assign(1, 0);
		_sign = true;
		return *this;
	}

	std::vector<unsigned int, pp_allocator<unsigned int>> res(_digits.size(), 0, _digits.get_allocator());
	big_int remain(_digits.get_allocator());
	remain._digits.assign(1, 0);

	for (int i = static_cast<int>(_digits.size()) - 1; i >= 0; --i) {
		remain._digits.insert(remain._digits.begin(), _digits[i]);
		optimise(remain._digits);

		unsigned int left = 0, right = BASE - 1, q = 0;
		while (left <= right) {
			unsigned int mid = left + (right - left) / 2;
			big_int temp = abs_other * big_int(static_cast<long long>(mid), _digits.get_allocator());
			if (remain >= temp) {
				q = mid;
				left = mid + 1;
			} else {
				right = mid - 1;
			}
		}

		if (q > 0) {
			big_int temp = abs_other * big_int(static_cast<long long>(q), _digits.get_allocator());
			remain -= temp;
		}
		res[i] = q;
	}

	_digits = std::move(res);
	_sign = (_sign == other._sign);
	optimise(_digits);
	return *this;
}


big_int &big_int::modulo_assign(const big_int &other, division_rule rule) & {
	if (is_zero(_digits)) return *this;
	if (is_zero(other._digits)) throw std::logic_error("Division by zero");

	big_int abs_this = *this;
	abs_this._sign = true;
	big_int abs_other = other;
	abs_other._sign = true;

	if (abs_this < abs_other) {
		_sign = true;
		return *this;
	}

	big_int remain(_digits.get_allocator());
	remain._digits.assign(1, 0);

	for (int i = static_cast<int>(_digits.size()) - 1; i >= 0; --i) {
		remain._digits.insert(remain._digits.begin(), _digits[i]);
		optimise(remain._digits);

		unsigned int left = 0, right = BASE - 1, q = 0;
		while (left <= right) {
			unsigned int mid = left + (right - left) / 2;
			big_int temp = abs_other * big_int(static_cast<long long>(mid), _digits.get_allocator());
			if (remain >= temp) {
				q = mid;
				left = mid + 1;
			} else {
				right = mid - 1;
			}
		}

		if (q > 0) {
			big_int temp = abs_other * big_int(static_cast<long long>(q), _digits.get_allocator());
			remain -= temp;
		}
	}

	_digits = std::move(remain._digits);
	_sign = true;
	optimise(_digits);
	return *this;
}


big_int::multiplication_rule big_int::decide_mult(size_t rhs) const noexcept {
	return rhs > 64 ? big_int::multiplication_rule::Karatsuba : big_int::multiplication_rule::trivial;
}

big_int::division_rule big_int::decide_div(size_t rhs) const noexcept {
	return big_int::division_rule::trivial;
}

big_int operator""_bi(unsigned long long n) {
	return {n};
}

big_int multiply_karatsuba(const big_int &a, const big_int &b) {
	if (a._digits.size() < 32 || b._digits.size() < 32) {
		big_int result = a;
		result.multiply_assign(b, big_int::multiplication_rule::trivial);
		return result;
	}

	size_t half_size = std::max(a._digits.size(), b._digits.size()) / 2;
	big_int a_low(a._digits.get_allocator()), a_high(a._digits.get_allocator());
	big_int b_low(b._digits.get_allocator()), b_high(b._digits.get_allocator());

	a_low._digits.assign(a._digits.begin(), a._digits.begin() + std::min(half_size, a._digits.size()));
	if (a._digits.size() > half_size) {
		a_high._digits.assign(a._digits.begin() + half_size, a._digits.end());
	}

	b_low._digits.assign(b._digits.begin(), b._digits.begin() + std::min(half_size, b._digits.size()));
	if (b._digits.size() > half_size) {
		b_high._digits.assign(b._digits.begin() + half_size, b._digits.end());
	}

	a_low._sign = true;
	a_high._sign = true;
	b_low._sign = true;
	b_high._sign = true;

	big_int z0 = multiply_karatsuba(a_low, b_low);
	big_int z2 = multiply_karatsuba(a_high, b_high);

	big_int a_sum = a_low;
	a_sum += a_high;
	big_int b_sum = b_low;
	b_sum += b_high;

	// Z1 = (a_low + a_high) * (b_low + b_high) - Z0 - Z2
	big_int z1 = multiply_karatsuba(a_sum, b_sum);
	z1 -= z0;
	z1 -= z2;

	z2 <<= half_size * 2 * 32;
	z1 <<= half_size * 32;

	z2 += z1;
	z2 += z0;

	z2._sign = (a._sign == b._sign);
	optimise(z2._digits);
	return z2;
}
