#pragma once

#include <concepts>
#include <cstddef>
#include <span>
#include <vector>

namespace proto {

template <class T>
concept Serializable = requires(T t, std::span<std::byte> buffer) {
    { t.size() } -> std::convertible_to<std::size_t>;
    { t.serialize(buffer) } -> std::convertible_to<std::size_t>;
};

template <Serializable T> std::vector<std::byte> serialize(const T &t) {
    std::vector<std::byte> buffer(t.size());
    t.serialize(buffer);
    return buffer;
}
} // namespace proto
