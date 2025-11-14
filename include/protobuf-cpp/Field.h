#pragma once

#include <cstdint>
#include <iosfwd>

namespace proto {
// Strongly typed Field number
enum class Field : std::uint64_t {};

std::ostream &operator<<(std::ostream &, Field);

} // namespace proto
