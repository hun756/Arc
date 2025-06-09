#ifndef LIB_ARC_DETAIL_REF_COUNT_HPP_AH87PN
#define LIB_ARC_DETAIL_REF_COUNT_HPP_AH87PN

#include "control_block_base.hpp"
#include <atomic>

namespace arc::detail
{

template <typename T>
inline void increment_strong(control_block_base* cb) noexcept
{
    cb->strong_count.fetch_add(1, std::memory_order_relaxed);
}

template <typename T>
inline void increment_weak(control_block_base* cb) noexcept
{
    cb->weak_count.fetch_add(1, std::memory_order_relaxed);
}

template <typename T>
inline void decrement_weak(control_block_base* cb) noexcept;

template <typename T>
inline void decrement_strong(control_block_base* cb) noexcept
{
    if (cb->strong_count.fetch_sub(1, std::memory_order_release) == 1)
    {
        std::atomic_thread_fence(std::memory_order_acquire);
        
        cb->dispose();
        
        decrement_weak<T>(cb);
    }
}

template <typename T>
inline void decrement_weak(control_block_base* cb) noexcept
{
    if (cb->weak_count.fetch_sub(1, std::memory_order_release) == 1)
    {
        std::atomic_thread_fence(std::memory_order_acquire);
        
        cb->destroy();
    }
}

} // namespace arc::detail

#endif ///< End of include guardÇ: LIB_ARC_DETAIL_REF_COUNT_HPP_AH87PN
