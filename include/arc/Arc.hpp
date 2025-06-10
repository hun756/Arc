#ifndef LIB_ARC_HPP_q2n5gy
#define LIB_ARC_HPP_q2n5gy

#include <arc/detail/control_block_base.hpp>
#include <arc/detail/control_block_make.hpp>
#include <arc/detail/control_block_ptr.hpp>
#include <arc/detail/ref_count.hpp>
#include <type_traits>

namespace arc
{
template <typename T>
class weak_arc;
}

namespace arc
{

template <typename T>
class arc
{
    template <typename U>
    friend class arc;

    template <typename U>
    friend class weak_arc;

public:
    using element_type = std::remove_extent<T>;
    using pointer = element_type*;
    using weak_type = weak_arc<T>;

private:
    pointer ptr_{};
    detail::control_block_base* cb_{};

    template <typename Y>
    arc(Y* p, detail::control_block_base* cb) noexcept : ptr_(p), cb_(cb)
    {
    }

public:
    constexpr arc() noexcept = default;
    constexpr arc(std::nullptr_t) noexcept {}

    template <typename Y>
        requires std::is_convertible_v<Y*, pointer>
    explicit arc(Y* p) : ptr_(p)
    {
        try
        {
            using deleter_type = std::default_delete<Y>;
            using allocator_type = std::allocator<Y>;
            cb_ =
                new detail::control_block_ptr<Y, deleter_type, allocator_type>(
                    p, deleter_type{}, allocator_type{});
        }
        catch (...)
        {
            delete p;
            throw;
        }
    }

    arc(const arc& other) noexcept : ptr_(other.ptr_), cb_(other.cb_)
    {
        if (cb_)
        {
            detail::increment_strong<element_type>(cb_);
        }
    }

    arc(arc&& other) noexcept : ptr_(other.ptr_), cb_(other.cb_)
    {
        other.ptr_ = nullptr;
        other.cb_ = nullptr;
    }

    ~arc()
    {
        if (cb_)
        {
            detail::decrement_strong<element_type>(cb_);
        }
    }

    arc& operator=(const arc& other) noexcept
    {
        arc(other).swap(*this);
        return *this;
    }

    arc& operator=(arc&& other) noexcept
    {
        arc(std::move(other)).swap(*this);
        return *this;
    }

    void reset() noexcept { arc().swap(*this); }

    template <typename Y>
        requires std::is_convertible_v<Y*, pointer>
    void reset(Y* p)
    {
        arc(p).swap(*this);
    }

    void swap(arc& other) noexcept
    {
        std::swap(ptr_, other.ptr_);
        std::swap(cb_, other.cb_);
    }

    pointer get() const noexcept { return ptr_; }

    long long use_count() const noexcept
    {
        return cb_ ? cb_->strong_count.load(std::memory_order_relaxed) : 0;
    }

    element_type& operator*() const noexcept { return *ptr_; }
    pointer operator->() const noexcept { return ptr_; }

    explicit operator bool() const noexcept { return ptr_ != nullptr; }
};

template <typename T>
void swap(arc<T>& a, arc<T>& b) noexcept
{
    a.swap(b);
}

} // namespace arc

#endif //< End of include guard: LIB_ARC_HPP_q2n5gy