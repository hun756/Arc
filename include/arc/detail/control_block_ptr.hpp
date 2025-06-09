#ifndef LIB_ARC_DETAIL_CONTROL_BLOCK_PTR_HPP_rvs2h0
#define LIB_ARC_DETAIL_CONTROL_BLOCK_PTR_HPP_rvs2h0

#include "control_block_base.hpp"
#include <memory>

namespace arc::detail
{
template <typename T, typename Deleter, typename Allocator>
class control_block_ptr final : public control_block_base
{
    [[no_unique_address]] Deleter deleter_;
    [[no_unique_address]] Allocator allocator_;

    T* ptr_;

public:
    control_block_ptr(T* p, Deleter d, Allocator a) noexcept(
        std::is_nothrow_move_constructible_v<Deleter> &&
        std::is_nothrow_move_constructible_v<Allocator>)
        : deleter_(std::move(d)), allocator_(std::move(a)), ptr_(p)
    {
    }

    void dispose() noexcept override
    {
        if (ptr_)
        {
            deleter_(ptr_);
            ptr_ = nullptr;
        }
    }

    void destroy() noexcept override
    {
        using alloc_traits = std::allocator_traits<Allocator>;
        using rebound_alloc_type =
            typename alloc_traits::template rebind_alloc<control_block_ptr>;
        using rebound_alloc_traits = std::allocator_traits<rebound_alloc_type>;

        rebound_alloc_type alloc(allocator_);

        this->~control_block_ptr();
        rebound_alloc_traits::deallocate(alloc, this, 1);
    }

    void* get_deleter(const std::type_info& ti) noexcept override
    {
        return ti == typeid(Deleter) ? std::addressof(deleter_) : nullptr;
    }
};
} // namespace arc::detail

#endif //< LIB_ARC_DETAIL_CONTROL_BLOCK_PTR_HPP_rvs2h0
