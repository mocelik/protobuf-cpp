#pragma once

#include <cstdint>
#include <tuple>

namespace test {
struct SingleInt {
    std::uint32_t value;
    static constexpr auto members = std::make_tuple(&SingleInt::value);

    constexpr auto operator<=>(const SingleInt &) const = default;
};

struct DoubleInt {
    std::uint32_t value1;
    std::uint32_t value2;
    static constexpr auto members =
        std::make_tuple(&DoubleInt::value1, &DoubleInt::value2);

    constexpr auto operator<=>(const DoubleInt &) const = default;
};

} // namespace test
