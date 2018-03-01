# static_uint: an arbitrary known size integer

This library implements an unsigned integer of a set size without requiring dynamic allocations. The most common use is larger than 64-bit integers.

The aim is to eventually support all arithmetic operations one would expect from a built-in integer type, and to allow them to be used in constexpr contexts. Operations are usually added as I need them in other projects using this.

The underlying data should be identical to a native unsigned type it if it existed (ie. it respects things like endianness).

This is an extremely early work-in-progress, so it shouldn't really be used for anything serious as of right now.

## Interface

### Comparisons
```cpp
    constexpr bool operator==(
        static_uint<size> const& lhs,
        static_uint<size> const& rhs) noexcept;
    constexpr bool operator>(
        static_uint<size> const& lhs,
        static_uint<size> const& rhs) noexcept;
    constexpr bool operator<(
        static_uint<size> const& lhs,
        static_uint<size> const& rhs) noexcept;
    constexpr bool operator>=(
        static_uint<size> const& lhs,
        static_uint<size> const& rhs) noexcept;
    constexpr bool operator<=(
        static_uint<size> const& lhs,
        static_uint<size> const& rhs) noexcept;
    constexpr bool operator!=(
        static_uint<size> const& lhs,
        static_uint<size> const& rhs) noexcept;
```

### Arithmetic

```cpp
    constexpr static_uint& operator+=(
        static_uint const& other) noexcept;
    constexpr static_uint<size> operator+(
        static_uint<size> const& lhs,
        static_uint<size> const& rhs) noexcept;
```

### Bitwise ops

```cpp
    constexpr static_uint& operator&=(
        static_uint const& rhs) noexcept;
    constexpr static_uint<size> operator&(
        static_uint const& lhs, static_uint const& rhs) noexcept;

    constexpr static_uint& operator>>=(unsigned int shift) noexcept;
    constexpr static_uint<size> operator>>(
        static_uint const& lhs, unsigned int shift) noexcept;
```

### Bytewise iteration

```cpp
    using bytes_iterator = unsigned char*;
    using reverse_bytes_iterator =
        std::reverse_iterator<unsigned char*>;
    bytes_iterator bytes_begin() noexcept;
    bytes_iterator bytes_end() noexcept;
    reverse_bytes_iterator rbytes_begin() noexcept;
    reverse_bytes_iterator rbytes_end() noexcept;

    using big_endian_iterator = /* depends on target endianness */
    using little_endian_iterator = /* depends on target endianness */

    little_endian_iterator little_endian_begin() noexcept;
    little_endian_iterator little_endian_end() noexcept;
    big_endian_iterator big_endian_begin() noexcept;
    big_endian_iterator big_endian_end() noexcept;
```

### Conversions

Conversion from platform ints and smaller static_uints works.

## Dependencies

- my own constd for compile time stdlib algorithms
- boost predef to find out endianness (this can be just the single required header)

## Expected improvements

- more artithmetic operations
- ostream operations
- hex, binary, decimal literals