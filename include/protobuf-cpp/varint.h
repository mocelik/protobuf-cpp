#pragma once

#include <cstddef> // std::byte
#include <cstdint>
#include <vector>

namespace proto {

std::vector<std::byte> serialize_varint(std::uint64_t value);

} // namespace proto
