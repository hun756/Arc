#ifndef LIB_ARC_DETAIL_HELPERS_HPP_zvfkwd
#define LIB_ARC_DETAIL_HELPERS_HPP_zvfkwd

#include <concepts>
#include <type_traits>

namespace arc::detail
{

template <typename T, typename U = int>
concept deleter_for = requires(T deleter, U* ptr) {
    { deleter(ptr) } noexcept;
};

template <typename From, typename To>
concept convertible_to_cv =
    std::convertible_to<std::remove_cv_t<From>*, std::remove_cv_t<To>*> &&
    (std::is_const_v<To> || !std::is_const_v<From>) &&
    (std::is_volatile_v<To> || !std::is_volatile_v<From>);

template <typename T>
struct type_identity
{
    using type = T;
};

template <typename T>
using type_identity_t = typename type_identity<T>::type;

} // namespace arc::detail

#endif ///< End of include guard: LIB_ARC_DETAIL_HELPERS_HPP_zvfkwd