#include <arc/detail/arc_detail_helpers.hpp>
#include <gtest/gtest.h>

struct NoexceptDeleter
{
    void operator()(int* p) const noexcept { delete p; }
};

struct ThrowingDeleter
{
    void operator()(int* p) const { delete p; }
};

struct WrongArgDeleter
{
    void operator()(float* p) const noexcept { delete p; }
};

struct Base
{
};
struct Derived : Base
{
};
struct Unrelated
{
};

TEST(DetailHelpersTest, DeleterConcept) {
    static_assert(arc::detail::deleter_for<NoexceptDeleter>, "NoexceptDeleter should be a valid deleter for int*");
    
    static_assert(!arc::detail::deleter_for<ThrowingDeleter>, "ThrowingDeleter should not be a valid deleter");
    static_assert(!arc::detail::deleter_for<WrongArgDeleter>, "WrongArgDeleter should not be a valid deleter for int*");
    static_assert(!arc::detail::deleter_for<int>, "An int cannot be a deleter");
}

TEST(DetailHelpersTest, ConvertibleConcept) {
    static_assert(arc::detail::convertible_to_cv<Derived, Base>, "Derived* should be convertible to Base*");
    static_assert(!arc::detail::convertible_to_cv<Base, Derived>, "Base* should not be convertible to Derived*");
    static_assert(!arc::detail::convertible_to_cv<Derived, Unrelated>, "Derived* should not be convertible to Unrelated*");

    static_assert(arc::detail::convertible_to_cv<int, const int>, "Should be able to add const");
    static_assert(!arc::detail::convertible_to_cv<const int, int>, "Should not be able to remove const");
    static_assert(arc::detail::convertible_to_cv<const int, const int>, "const to const should be fine");
    static_assert(arc::detail::convertible_to_cv<int, int>, "T to T should be fine");

    static_assert(arc::detail::convertible_to_cv<int, volatile int>, "Should be able to add volatile");
    static_assert(!arc::detail::convertible_to_cv<volatile int, int>, "Should not be able to remove volatile");

    static_assert(arc::detail::convertible_to_cv<Derived, const Base>, "Derived* to const Base* should be fine");
    static_assert(arc::detail::convertible_to_cv<const Derived, const Base>, "const Derived* to const Base* should be fine");
    static_assert(!arc::detail::convertible_to_cv<const Derived, Base>, "const Derived* to Base* is not allowed (removing const)");
}

TEST(DetailHelpersTest, TypeIdentity) {
    static_assert(std::is_same_v<int, arc::detail::type_identity_t<int>>, "type_identity_t<int> should be int");
    static_assert(std::is_same_v<const Base*, arc::detail::type_identity_t<const Base*>>, "type_identity should preserve types exactly");
}