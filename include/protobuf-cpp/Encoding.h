#pragma once

#include "Concepts.h"
#include "Fixint.h"
#include "Varint.h"
#include "Varlen.h"

#include <cstddef>
#include <type_traits>

namespace proto {

//
template <auto... Ptrs> struct Members {
    using type = Members;
    static constexpr std::size_t s_num_elems{sizeof...(Ptrs)};
};

//
// Primary template: Get compiler error if the encoding is not set
template <auto MemberPtr> struct MemberEncoding;

// For any integer member, default to Varint
template <typename Class, typename M, M Class::*MemberPtr>
    requires std::is_integral_v<M>
struct MemberEncoding<MemberPtr> {
    using type = Varint;
};

// For any floating point member, default to Fixint
template <typename Class, typename M, M Class::*MemberPtr>
    requires std::is_floating_point_v<M>
struct MemberEncoding<MemberPtr> {
    using type =
        std::conditional_t<std::is_same_v<M, float>, Fixint<std::uint32_t>,
                           Fixint<std::uint64_t>>;
};

} // namespace proto
