#pragma once

#include <protobuf-cpp/Encoding.h>
#include <protobuf-cpp/Fixint.h>

#include <cstdint>

namespace test {

struct SingleInt {
    std::uint32_t value;
    using members = proto::Members<&SingleInt::value>;

    constexpr auto operator<=>(const SingleInt &) const = default;
};

struct DoubleInt {
    std::uint32_t value1;
    std::int32_t value2;

    using members = proto::Members<&DoubleInt::value1, &DoubleInt::value2>;

    constexpr auto operator<=>(const DoubleInt &) const = default;
};

struct SingleInt_asFixed {
    std::uint32_t value;

    using members = proto::Members<&SingleInt_asFixed::value>;

    constexpr auto operator<=>(const SingleInt_asFixed &) const = default;
};

struct IntAndFloat_asFixed {
    std::int32_t value1;
    float value2;

    using members = proto::Members<&IntAndFloat_asFixed::value1,
                                   &IntAndFloat_asFixed::value2>;

    constexpr auto operator<=>(const IntAndFloat_asFixed &) const = default;
};

} // namespace test

namespace proto {

// Specialization so that SingleInt_asFixed is serialized as a Fixint
template <> struct MemberEncoding<&test::SingleInt_asFixed::value> {
    using type = Fixint64;
};

template <> struct MemberEncoding<&test::IntAndFloat_asFixed::value2> {
    using type = Fixint32;
};

} // namespace proto
