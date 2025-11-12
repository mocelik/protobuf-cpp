
#pragma once

#include "Field.h"

#include <iostream>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace proto {

// Default SFINAE case for member_type
template <typename MemberPtr> struct member_type;

// Extract the MemberType from the Class Member Pointer as `type`
// E.g. extract the `uint32_t` from `uint32_t &Class::member_field`
template <typename Class, typename MemberType>
struct member_type<MemberType Class::*> {
    using type = MemberType;
};

template <typename MemberPtr>
using member_type_t = typename member_type<MemberPtr>::type;

// Default SFINAE case for member_types_tuple
template <typename TupleOfPtrs> struct member_types_tuple;

// Convert a tuple of Class Member Pointers to a tuple of Class Member Types
// E.g. convert tuple<&A::x, &A::y>, to tuple<decltype(A::x), decltype(A::y)>
template <typename... Ptrs> struct member_types_tuple<std::tuple<Ptrs...>> {
    using type = std::tuple<member_type_t<Ptrs>...>;
};

template <typename TupleOfPtrs>
using member_types_tuple_t =
    typename member_types_tuple<std::remove_cvref_t<TupleOfPtrs>>::type;

// Trait to detect an inner 'value()' member and its return type
// (SFINAE-friendly)
template <typename T, typename = void> struct inner_value_type {
    using type = void;
};
template <typename T>
struct inner_value_type<T, std::void_t<decltype(std::declval<T>().value())>> {
    using type = decltype(std::declval<T>().value());
};

template <std::size_t I, typename T, typename Tuple>
void assign_to_index(const T &value, Tuple &tuple) {
    using ElemT = std::remove_cvref_t<
        std::tuple_element_t<I, std::remove_cvref_t<Tuple>>>;
    using InnerT = typename inner_value_type<T>::type;
    if constexpr (std::is_convertible_v<T, ElemT>) {
        std::get<I>(tuple) = static_cast<ElemT>(value);
    } else if constexpr (!std::is_same_v<InnerT, void> &&
                         std::is_convertible_v<InnerT, ElemT>) {
        std::get<I>(tuple) = static_cast<ElemT>(value.value());
    } else if constexpr (std::is_constructible_v<ElemT, T>) {
        std::get<I>(tuple) = ElemT{value};
    } else if constexpr (!std::is_same_v<InnerT, void> &&
                         std::is_constructible_v<ElemT, InnerT>) {
        std::get<I>(tuple) = ElemT{value.value()};
    } else {
        /* Unable to assign/construct: ignore */
        throw std::logic_error(
            "Tried to construct a field from an incompatible type");
    }
}

template <typename T, typename Tuple, std::size_t... Is>
void set_impl(std::size_t idx, const T &value, Tuple &tuple,
              std::index_sequence<Is...>) {
    // Build a fold-expression that will call assign_to_index for the matching
    // index
    ((idx == Is ? (assign_to_index<Is, T, Tuple>(value, tuple)) : void{}), ...);
}

template <typename T, typename Tuple>
void set(Field field, const T &value, Tuple &tuple) {
    constexpr std::size_t N = std::tuple_size_v<std::remove_cvref_t<Tuple>>;
    // use `field - 1` since field is 1-indexed but tuples are 0-indexed
    set_impl<T, Tuple>(std::to_underlying(field) - 1, value, tuple,
                       std::make_index_sequence<N>{});
}

} // namespace proto
