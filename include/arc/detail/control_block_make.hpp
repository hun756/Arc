#ifndef LIB_ARC_DETAIL_CONTROL_BLOCK_MAKE_HPP_x28hov
#define LIB_ARC_DETAIL_CONTROL_BLOCK_MAKE_HPP_x28hov

#include "control_block_base.hpp"
#include <cstddef>
#include <memory>
#include <utility>

namespace arc::detail
{

template <typename T, typename Allocator>
class control_block_make final : public control_block_base
{
    [[no_unique_address]] Allocator allocator_;

    union
    {
        T object;
    };

public:
    template <typename... Args>
    control_block_make(Allocator a, Args&&... args) noexcept(
        std::is_nothrow_move_constructible_v<Allocator> &&
        std::is_nothrow_constructible_v<T, Args...>)
        : allocator_(std::move(a))
    {
        std::allocator_traits<Allocator>::construct(
            allocator_, std::addressof(object), std::forward<Args>(args)...);
    }

    void dispose() noexcept override
    {
        std::destroy_at(std::addressof(object));
    }

    void destroy() noexcept override
    {
        using alloc_traits = std::allocator_traits<Allocator>;
        using rebound_alloc_type =
            typename alloc_traits::template rebind_alloc<control_block_make>;

        rebound_alloc_type alloc(allocator_);

        this->~control_block_make();
        alloc_traits::deallocate(alloc, this, 1);
    }

    void* get_deleter(const std::type_info&) noexcept override
    {
        return nullptr;
    }

    T* get_ptr() noexcept { return std::addressof(object); }
};

} // namespace arc::detail

#endif ///< End of include guard LIB_ARC_DETAIL_CONTROL_BLOCK_MAKE_HPP_x28hov
