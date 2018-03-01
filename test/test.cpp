#include <qg/static_uint.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <numeric>

/*
 * Test-only printing of static_uint until a good solution is built in
 */
template <std::size_t size>
std::ostream& operator<<(std::ostream& os,
                         static_uint<size> const& u) {
    for (auto const& d : u) {
        os << d;
    }
    return os;
}

TEST_CASE("default construction") {
    static_uint<256> t1{};
    static_uint<256> t2{0};

    CHECK(t1 == t2);
    CHECK(t1 == 0);
    CHECK(t2 == 0);
}

TEST_CASE("additions with double size_t's size") {
    constexpr auto uint =
        static_uint<2 * sizeof(std::size_t) * CHAR_BIT>{
            std::numeric_limits<std::size_t>::max()};
    static_assert(sizeof(uint) == sizeof(std::size_t[2]));
    CHECK(uint == std::numeric_limits<std::size_t>::max());

    SUBCASE("size_t max plus 1") {
        constexpr std::array<std::size_t, 2> expected{1, 0};

        constexpr auto test_uint = uint + 1;
        CHECK(std::equal(test_uint.begin(), test_uint.end(),
                         expected.begin(), expected.end()));
    }
    SUBCASE("size_t max times 2") {
        constexpr std::array<std::size_t, 2> expected{
            1, std::numeric_limits<std::size_t>::max() * 2};

        constexpr auto test_uint = uint + uint;
        CHECK(std::equal(test_uint.begin(), test_uint.end(),
                         expected.begin(), expected.end()));
    }
}

TEST_CASE("right shifting more than 64 bits") {
    constexpr auto u64_max =
        std::numeric_limits<std::uint64_t>::max();
    constexpr auto u128_max =
        std::numeric_limits<static_uint<128>>::max();
    constexpr auto u192_max =
        std::numeric_limits<static_uint<192>>::max();
    constexpr auto u256_max =
        std::numeric_limits<static_uint<256>>::max();

    std::array<unsigned int, 64> cases{};
    std::iota(cases.begin(), cases.end(), 0);
    SUBCASE("128") {
        for (auto i : cases) {
            CAPTURE(i);
            CHECK((u128_max >> (i + 64)) == (u64_max >> i));
        }
    }
    SUBCASE("192") {
        for (auto i : cases) {
            CAPTURE(i);
            CHECK((u192_max >> (i + 128)) == (u64_max >> i));
        }
    }
    SUBCASE("256") {
        for (auto i : cases) {
            CAPTURE(i);
            CHECK((u256_max >> (i + 192)) == (u64_max >> i));
        }
    }

    SUBCASE("constexpr") {
        constexpr auto test128 = u256_max >> 128;
        constexpr auto test192 = u256_max >> 64;
        CHECK(test128 == static_uint<256>{u128_max});
        CHECK(test192 == static_uint<256>{u192_max});
    }
}

TEST_CASE("operator+") {
    // some checks shamelessly lifted from
    // https://github.com/cerevra/int/
    using u512 = static_uint<512>;
    u512 a1{3};
    auto begin = a1.big_endian_begin();
    // auto end2 = big_endian_iterator<512>::end(a11);;
    begin[55] = 1;
    CAPTURE(a1);
    for (int i = 0; i < 55; ++i) {
        CAPTURE(i);
        CHECK(begin[i] == 0);
    }

    SUBCASE("") {
        u512 a11 = a1 + 5;
        auto begin = a11.big_endian_begin();
        // auto end2 = big_endian_iterator<512>::end(a11);;
        for (int i = 0; i < 55; ++i) {
            CAPTURE(i);
            CHECK(begin[i] == 0);
        }
        CHECK(begin[55] == 1);
        for (int i = 56; i < 63; ++i) {
            CAPTURE(i);
            CHECK(begin[i] == 0);
        }
        CHECK(begin[63] == 8);
    }

    SUBCASE("") {
        u512 a12 = a1 + 0;
        auto begin = a12.big_endian_begin();
        for (int i = 0; i < 55; ++i) {
            CAPTURE(i);
            CHECK(begin[i] == 0);
        }
        CHECK(begin[55] == 1);
        for (int i = 56; i < 63; ++i) {
            CAPTURE(i);
            CHECK(begin[i] == 0);
        }
        CHECK(begin[63] == 3);
    }

    SUBCASE("") {
        u512 a14 = a1 + std::numeric_limits<std::uint64_t>::max();
        auto it = a14.big_endian_begin();
        for (int i = 0; i < 55; ++i) {
            CAPTURE(i);
            CHECK(it[i] == 0);
        }
        CHECK(it[55] == 2);
        for (int i = 56; i < 63; ++i) {
            CAPTURE(i);
            CHECK(it[i] == 0);
        }
        CHECK(it[63] == 2);
    }
}
/*
static_assert(static_uint<256>{
                  std::numeric_limits<static_uint<255>>::max()} <
              std::numeric_limits<static_uint<256>>::max());
*/
