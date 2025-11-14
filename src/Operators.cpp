#include <protobuf-cpp/Field.h>
#include <protobuf-cpp/WireType.h>

#include <ostream>
#include <utility>

namespace proto {
std::ostream &operator<<(std::ostream &os, WireType w) {
    switch (w) {
    case WireType::VARINT:
        return os << "VARINT";
    case WireType::LEN:
        return os << "LEN";
    case WireType::FIXED32:
        return os << "FIXED32";
    case WireType::FIXED64:
        return os << "FIXED64";
    default:
        return os << "Unknown-WireType{" << static_cast<std::uint32_t>(w)
                  << '}';
    }
}

std::ostream &operator<<(std::ostream &os, Field f) {
    return os << "Field{" << std::to_underlying(f) << '}';
}
} // namespace proto
