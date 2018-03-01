#pragma once

#include <qg/constd.hpp>

#include <boost/predef/other/endian.h>

#include <array>
#include <cassert>
#include <climits>
#include <cstdint>

namespace detail {
/*
 * Determine the size in bytes of the static_uint
 */
constexpr inline std::size_t required_sizeof(
    std::size_t size_in_bits) noexcept {
    if (size_in_bits % CHAR_BIT == 0) {
        return size_in_bits / CHAR_BIT;
    } else {
        return size_in_bits / CHAR_BIT + 1;
    }
}

/*
 * Compute the array size required, based on the platform's size_t
 * size
 */
constexpr inline std::size_t required_array_size(
    std::size_t size_in_bits) noexcept {
    auto const required = required_sizeof(size_in_bits);
    if (required % sizeof(std::size_t) == 0) {
        return required / sizeof(std::size_t);
    } else {
        return required / sizeof(std::size_t) + 1;
    }
}

/*
 * Helpers to make an array wider by copying its members into a
 * larger array
 * Used to widen static_uints easily and correctly with regards to
 * endianness
 */
#if BOOST_ENDIAN_BIG_BYTE
template <std::size_t bigger_size, std::size_t smaller_size,
          std::size_t... zeros, std::size_t... is>
constexpr std::array<std::size_t, bigger_size> widen_array_impl(
    std::array<std::size_t, smaller_size> const& small_array,
    std::index_sequence<zeros...>,
    std::index_sequence<is...>) noexcept {
    return {((void)zeros, 0)..., (small_array[is])...};
}

template <std::size_t bigger_size, std::size_t smaller_size>
constexpr std::array<std::size_t, bigger_size> widen_array(
    std::array<std::size_t, smaller_size> const&
        small_array) noexcept {
    constexpr auto z =
        std::make_index_sequence<bigger_size - smaller_size>();
    constexpr auto l = std::make_index_sequence<smaller_size>();
    return widen_array_impl<bigger_size>(small_array, z, l);
}
#elif BOOST_ENDIAN_LITTLE_BYTE
template <std::size_t bigger_size, std::size_t smaller_size,
          std::size_t... is>
constexpr std::array<std::size_t, bigger_size> widen_array_impl(
    std::array<std::size_t, smaller_size> const& small_array,
    std::index_sequence<is...>) noexcept {
    return {(small_array[is])...};
}

template <std::size_t bigger_size, std::size_t smaller_size>
constexpr std::array<std::size_t, bigger_size> widen_array(
    std::array<std::size_t, smaller_size> const&
        small_array) noexcept {
    constexpr auto l = std::make_index_sequence<smaller_size>();
    return widen_array_impl<bigger_size>(small_array, l);
}
#endif
}  // namespace detail

/**
 * static_uint: an unsigned integer parameterized by its bit size
 * Designed to be usable in arithmetic operations with normal unsigned
 * semantics
 */
template <std::size_t size>
struct static_uint {
  private:
    static constexpr auto ARR_SIZE =
        detail::required_array_size(size);

    static_assert(
        size % sizeof(std::size_t) == 0,
        "Currently, only multiples of platform size are supported");

    std::array<std::size_t, ARR_SIZE> data;

  public:
    /**
     * Constructors for default/value initialisation and
     * initialisation from platform-sized integers
     */
    constexpr static_uint() noexcept = default;
    constexpr static_uint(std::size_t in) noexcept
        : data{detail::widen_array<ARR_SIZE>(std::array{in})} {}

    /**
     * Conversion from narrower static_uints
     * This is safe and cannot lead to truncation
     */
    template <std::size_t other_size>
    friend struct static_uint;
    template <std::size_t other_size>
    constexpr static_uint(static_uint<other_size> in) noexcept
        : data(detail::widen_array<ARR_SIZE>(in.data)) {
        static_assert(other_size <= size,
                      "Cannot convert a static_uint to a "
                      "smaller-size static_uint");
    }

    /**
     * Iterators simply walk the underlying data from largest to
     * smallest platform-sized uint
     * This might be removed from public API in the future
     */
    using iterator =
        std::reverse_iterator<typename decltype(data)::iterator>;
    using const_iterator = std::reverse_iterator<typename decltype(
        data)::const_iterator>;
    using reverse_iterator = typename decltype(data)::iterator;
    using reverse_const_iterator =
        typename decltype(data)::const_iterator;

    constexpr iterator begin() noexcept { return data.rbegin(); }
    constexpr iterator end() noexcept { return data.rend(); }
    constexpr const_iterator begin() const noexcept {
        return data.rbegin();
    }
    constexpr const_iterator end() const noexcept {
        return data.rend();
    }
    constexpr reverse_iterator rbegin() noexcept {
        return data.begin();
    }
    constexpr reverse_iterator rend() noexcept { return data.end(); }
    constexpr reverse_const_iterator rbegin() const noexcept {
        return data.begin();
    }
    constexpr reverse_const_iterator rend() const noexcept {
        return data.end();
    }

    /**
     * Bytewise iterators
     * Defined depending on the underlying hardware
     */
    using bytes_iterator = unsigned char*;
    using reverse_bytes_iterator =
        std::reverse_iterator<unsigned char*>;
    auto bytes_begin() noexcept {
        return reinterpret_cast<unsigned char*>(data.data());
    }
    auto bytes_end() noexcept {
        return reinterpret_cast<unsigned char*>(&*data.end());
    }
    auto rbytes_begin() noexcept {
        return reverse_bytes_iterator{bytes_end()};
    }
    auto rbytes_end() noexcept {
        return reverse_bytes_iterator{bytes_begin()};
    }
#if BOOST_ENDIAN_BIG_BYTE
    using big_endian_iterator = bytes_iterator;
    using little_endian_iterator = reverse_bytes_iterator;

    auto little_endian_begin() noexcept { return rbytes_begin(); }
    auto little_endian_end() noexcept { return rbytes_end(); }
    auto big_endian_begin() noexcept { return bytes_begin(); }
    auto big_endian_end() noexcept { return bytes_end(); }
#elif BOOST_ENDIAN_LITTLE_BYTE
    using big_endian_iterator = reverse_bytes_iterator;
    using little_endian_iterator = bytes_iterator;

    auto little_endian_begin() noexcept { return bytes_begin(); }
    auto little_endian_end() noexcept { return bytes_end(); }
    auto big_endian_begin() noexcept { return rbytes_begin(); }
    auto big_endian_end() noexcept { return rbytes_end(); }
#endif

    // comparisons
    friend constexpr int compare(
        static_uint<size> const& lhs,
        static_uint<size> const& rhs) noexcept {
        for (std::size_t i = static_uint<size>::ARR_SIZE; i != 0;
             --i) {
            if (lhs.data[i - 1] > rhs.data[i - 1]) {
                return 1;
            }
            if (lhs.data[i - 1] < rhs.data[i - 1]) {
                return -1;
            }
        }
        return 0;
    }

    friend constexpr bool operator==(
        static_uint<size> const& lhs,
        static_uint<size> const& rhs) noexcept {
        return compare(lhs, rhs) == 0;
    }
    friend constexpr bool operator>(
        static_uint<size> const& lhs,
        static_uint<size> const& rhs) noexcept {
        return compare(lhs, rhs) > 0;
    }
    friend constexpr bool operator<(
        static_uint<size> const& lhs,
        static_uint<size> const& rhs) noexcept {
        return compare(lhs, rhs) < 0;
    }
    friend constexpr bool operator>=(
        static_uint<size> const& lhs,
        static_uint<size> const& rhs) noexcept {
        return compare(lhs, rhs) >= 0;
    }
    friend constexpr bool operator<=(
        static_uint<size> const& lhs,
        static_uint<size> const& rhs) noexcept {
        return compare(lhs, rhs) <= 0;
    }
    friend constexpr bool operator!=(
        static_uint<size> const& lhs,
        static_uint<size> const& rhs) noexcept {
        return compare(lhs, rhs) != 0;
    }

    // arithmetic
    constexpr static_uint& operator+=(
        static_uint const& other) noexcept {
        bool overflow = false;
        for (std::size_t i = 0; i < static_uint<size>::ARR_SIZE;
             ++i) {
            data[i] += other.data[i] + (overflow ? 1 : 0);
            overflow = data[i] < other.data[i];
        }
        return *this;
    }
    friend constexpr static_uint<size> operator+(
        static_uint<size> const& lhs,
        static_uint<size> const& rhs) noexcept {
        auto result = lhs;
        return result += rhs;
    }

    // bitwise operations
    constexpr static_uint& operator&=(
        static_uint const& rhs) noexcept {
        for (std::size_t i = 0; i < static_uint::ARR_SIZE; ++i) {
            data[i] &= rhs.data[i];
        }
        return *this;
    }
    friend constexpr static_uint<size> operator&(
        static_uint const& lhs, static_uint const& rhs) noexcept {
        auto result = lhs;
        result &= rhs;
        return result;
    }

    constexpr static_uint& operator>>=(unsigned int shift) noexcept {
        // each time, bottom 'shift' bits become the top bits of the
        // lower level

        constexpr auto elem_bits = sizeof(std::size_t) * CHAR_BIT;

        // for every full elem shift, just move full elements right
        if (auto const right_elem_shift = shift / elem_bits) {
            constd::copy(rbegin() + right_elem_shift, rend(),
                         rbegin());
            constd::fill(begin(), begin() + right_elem_shift, 0);
        }

        // perform the smaller bitwise shift, if any
        auto const bitshift = shift % elem_bits;
        if (bitshift) {
            constd::accumulate(
                begin(), end(), std::size_t{0},
                [bitshift](std::size_t carry, auto& elem) {
                    auto const next_carry = elem
                                            << (elem_bits - bitshift);
                    elem >>= bitshift;
                    elem |= carry;
                    return next_carry;
                });
        }
        return *this;
    }

    friend constexpr static_uint<size> operator>>(
        static_uint const& lhs, unsigned int shift) noexcept {
        auto result = lhs;
        result >>= shift;
        return result;
    }
};

namespace std {
// NOTE(quentin): this is not a full specialisation, which
// isn't ideal
template <std::size_t size>
struct numeric_limits<static_uint<size>> {
    static constexpr bool is_specialized() noexcept { return true; }
    static constexpr bool is_integer() noexcept { return true; }
    static constexpr bool is_exact() noexcept { return true; }
    static constexpr bool is_signed() noexcept { return false; }
    static constexpr static_uint<size> max() noexcept {
        auto result = static_uint<size>{};
        ::constd::fill(result.begin(), result.end(),
                       std::numeric_limits<std::size_t>::max());
        return result;
    }
};
}  // namespace std
