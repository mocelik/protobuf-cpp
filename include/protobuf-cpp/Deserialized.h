#pragma once

#include <cstddef>

namespace proto {

template <typename Type> class Deserialized {
  public:
    constexpr Deserialized(Type value, std::size_t bytes_read) noexcept
        : m_value(value), m_bytes_read(bytes_read) {}

    [[nodiscard]] constexpr Type value() const noexcept { return m_value; }
    [[nodiscard]] constexpr std::size_t bytes_read() const noexcept {
        return m_bytes_read;
    }

  private:
    Type m_value;
    std::size_t m_bytes_read;
};
} // namespace proto
