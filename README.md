# static_uint: an arbitrary known size integer

This library implements an unsigned integer of a set size without requiring dynamic allocations. The most common use is larger than 64-bit integers.

The aim is to eventually support all arithmetic operations one would expect from a built-in integer type, and to allow them to be used in constexpr contexts. Operations are usually added as I need them in other projects using this.

This is an extremely early work-in-progress, so it shouldn't really be used for anything serious as of right now.

## Dependencies

- my own constd for compile time stdlib algorithms
- Boost.Iterator (specifically iterator_facade) for bytewise iterators

## Expected improvements

- more artithmetic operations
- ostream operations
- multiple iterator types to view the uint as little/big/platform-endian
- hex, binary, decimal literals