#pragma once

namespace proto {
enum class WireType {
    VARINT = 0,  // int32, int64, uint32, uint64, sint32, sint64, bool, enum
    FIXED64 = 1, // fixed64, sfixed64, double
    LEN = 2,     // string, bytes, embedded messages, packed repeated fields
    // START_GROUP [[deprecated]] = 3,
    // END_GROUP [[deprecated]] = 4,
    FIXED32 = 5 // fixed32, sfixed32, float
};
}
