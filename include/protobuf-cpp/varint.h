#pragma once

#include <cstddef> // std::byte
#include <cstdint>
#include <vector>

namespace proto {

constexpr std::vector<std::byte> serialize_varint(std::uint64_t value) {
    constexpr std::byte continue_bit{0b1000'0000};
    std::vector<std::byte> result;
    do {
        std::byte byte = static_cast<std::byte>(value & 0x7f);
        value >>= 7;
        if (value != 0) {
            byte |= continue_bit;
        }
        result.push_back(byte);
    } while (value != 0);
    return result;
}

constexpr std::uint64_t deserialize_varint(const std::vector<std::byte> &data) {
    constexpr std::byte continue_bit{0b1000'0000};
    std::uint64_t result = 0;
    int shift = 0;
    for (const auto &byte : data) {
        result |= (static_cast<std::uint64_t>(byte & std::byte{0b0111'1111})
                   << shift);
        if (!bool(byte & continue_bit)) {
            break;
        }
        shift += 7;
    }
    return result;
}

} // namespace proto
