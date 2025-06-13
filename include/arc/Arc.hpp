#ifndef LIB_ARC_HPP_q2n5gy
#define LIB_ARC_HPP_q2n5gy

#include <arc/detail/arc_detail_helpers.hpp>
#include <arc/detail/control_block_base.hpp>
#include <arc/detail/control_block_make.hpp>
#include <arc/detail/control_block_ptr.hpp>
#include <arc/detail/ref_count.hpp>
#include <memory>
#include <type_traits>

namespace arc
{
template <typename T>
class weak_arc;

template <typename T>
class arc;
} // namespace arc

namespace arc
{

namespace detail
{
struct arc_access
{
    template <typename T, typename U>
    static arc<T> create_from(const arc<U>& other,
                              typename arc<T>::pointer p) noexcept
    {
        if (p)
            detail::increment_strong<T>(other.cb_);
        return arc<T>(p, p ? other.cb_ : nullptr);
    }
};
} // namespace detail

template <typename T>
class arc
{
    template <typename U>
    friend class arc;

    template <typename U>
    friend class weak_arc;

    template <typename U, typename... Args>
        requires(!std::is_array_v<U>)
    friend arc<U> make_arc(Args&&...);

    template <typename U, typename A, typename... Args>
        requires(!std::is_array_v<U>)
    friend arc<U> allocate_arc(const A&, Args&&...);

    friend struct detail::arc_access;

public:
    using element_type = std::remove_extent_t<T>;
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

    template <typename Y>
        requires std::is_convertible_v<Y*, pointer>
    explicit arc(const weak_arc<Y>& r)
    {
        auto temp = r.lock();

        if (!temp)
        {
            throw std::bad_weak_ptr();
        }

        this->swap(temp);
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

template <typename T>
class weak_arc
{
    template <typename U>
    friend class arc;

    template <typename U>
    friend class weak_arc;

public:
    using element_type = std::remove_extent_t<T>;

private:
    element_type* ptr_{};
    detail::control_block_base* cb_{};

public:
    constexpr weak_arc() noexcept = default;

    template <typename Y>
        requires std::is_convertible_v<Y*, element_type*>
    weak_arc(const arc<Y>& other) noexcept : ptr_(other.ptr_), cb_(other.cb_)
    {
        if (cb_)
        {
            detail::increment_weak<element_type>(cb_);
        }
    }

    weak_arc(const weak_arc& other) noexcept : ptr_(other.ptr_), cb_(other.cb_)
    {
        if (cb_)
        {
            detail::increment_weak<element_type>(cb_);
        }
    }

    weak_arc(weak_arc&& other) noexcept : ptr_(other.ptr_), cb_(other.cb_)
    {
        other.ptr_ = nullptr;
        other.cb_ = nullptr;
    }

    ~weak_arc()
    {
        if (cb_)
        {
            detail::decrement_weak<element_type>(cb_);
        }
    }

    weak_arc& operator=(const weak_arc& other) noexcept
    {
        if (this != &other)
        {
            weak_arc(other).swap(*this);
        }
        return *this;
    }

    weak_arc& operator=(weak_arc&& other) noexcept
    {
        if (this != &other)
        {
            weak_arc(std::move(other)).swap(*this);
        }
        return *this;
    }

    template <typename Y>
        requires std::is_convertible_v<Y*, element_type*>
    weak_arc& operator=(const arc<Y>& other) noexcept
    {
        weak_arc(other).swap(*this);
        return *this;
    }

    void swap(weak_arc& other) noexcept
    {
        std::swap(ptr_, other.ptr_);
        std::swap(cb_, other.cb_);
    }

    long long use_count() const noexcept
    {
        return cb_ ? cb_->strong_count.load(std::memory_order_relaxed) : 0;
    }

    bool expired() const noexcept { return use_count() == 0; }

    arc<T> lock() const noexcept
    {
        if (!cb_)
        {
            return arc<T>();
        }

        std::int64_t count = cb_->strong_count.load(std::memory_order_relaxed);
        while (count != 0)
        {
            if (cb_->strong_count.compare_exchange_weak(
                    count, count + 1, std::memory_order_acquire,
                    std::memory_order_relaxed))
            {
                return arc<T>(ptr_, cb_);
            }
        }

        return arc<T>();
    }
};

template <typename T, typename... Args>
    requires(!std::is_array_v<T>)
arc<T> make_arc(Args&&... args)
{
    using non_cv_T = std::remove_cv_t<T>;
    using allocator_type = std::allocator<non_cv_T>;
    using control_type = detail::control_block_make<non_cv_T, allocator_type>;
    using cb_allocator = typename std::allocator_traits<
        allocator_type>::template rebind_alloc<control_type>;

    allocator_type alloc{};
    cb_allocator cb_alloc(alloc);
    auto cb_ptr = std::allocator_traits<cb_allocator>::allocate(cb_alloc, 1);

    try
    {
        std::allocator_traits<cb_allocator>::construct(
            cb_alloc, cb_ptr, alloc, std::forward<Args>(args)...);
        return arc<T>(static_cast<typename arc<T>::pointer>(cb_ptr->get_ptr()),
                      cb_ptr);
    }
    catch (...)
    {
        std::allocator_traits<cb_allocator>::deallocate(cb_alloc, cb_ptr, 1);
        throw;
    }
}

template <typename T, typename A, typename... Args>
    requires(!std::is_array_v<T>)
arc<T> allocate_arc(const A& alloc, Args&&... args)
{
    using non_cv_T = std::remove_cv_t<T>;
    using ControlBlock = detail::control_block_make<non_cv_T, A>;
    using CBAllocator =
        typename std::allocator_traits<A>::template rebind_alloc<ControlBlock>;

    CBAllocator cb_alloc(alloc);
    ControlBlock* cb_ptr =
        std::allocator_traits<CBAllocator>::allocate(cb_alloc, 1);

    try
    {
        std::allocator_traits<CBAllocator>::construct(
            cb_alloc, cb_ptr, alloc, std::forward<Args>(args)...);
        return arc<T>(static_cast<typename arc<T>::pointer>(cb_ptr->get_ptr()),
                      cb_ptr);
    }
    catch (...)
    {
        std::allocator_traits<CBAllocator>::deallocate(cb_alloc, cb_ptr, 1);
        throw;
    }
}

template <typename T, typename U>
    requires(!std::is_array_v<T> && !std::is_array_v<U>)
arc<T> static_pointer_cast(const arc<U>& r) noexcept
{
    auto p = static_cast<typename arc<T>::pointer>(r.get());
    return detail::arc_access::create_from<T, U>(r, p);
}

template <typename T, typename U>
    requires(!std::is_array_v<T> && !std::is_array_v<U>)
arc<T> const_pointer_cast(const arc<U>& r) noexcept
{
    auto p = const_cast<typename arc<T>::pointer>(r.get());
    return detail::arc_access::create_from<T, U>(r, p);
}

template <typename T, typename U>
    requires(!std::is_array_v<T> && !std::is_array_v<U>)
arc<T> dynamic_pointer_cast(const arc<U>& r) noexcept
{
    auto p = dynamic_cast<typename arc<T>::pointer>(r.get());
    return detail::arc_access::create_from<T, U>(r, p);
}

template <typename T, typename U>
arc<T> reinterpret_pointer_cast(const arc<U>& r) noexcept
{
    auto p = reinterpret_cast<typename arc<T>::pointer>(r.get());
    return detail::arc_access::create_from<T, U>(r, p);
}

} // namespace arc

#endif //< End of include guard: LIB_ARC_HPP_q2n5gy