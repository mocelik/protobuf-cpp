#pragma once

#include <cstddef>

namespace proto {

template <typename Type> struct Deserialized {
    Type value;
    std::size_t num_bytes_read;
};
} // namespace proto
