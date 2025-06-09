/**
 * @file control_block_base.hpp
 * @brief Atomic Reference Counter - A lock-free, cache-optimized smart pointer
 * implementation
 * @author Mehmet Ekemen <ekemenms@gmail.com>
 * @date June 2025
 *
 * This header provides a high-performance atomic reference counting system
 * designed for maximum throughput in multi-threaded environments. Built with
 * cache-line alignment and memory ordering guarantees for zero-contention
 * shared ownership.
 */
#ifndef ARC_HPP_yrpfns
#define ARC_HPP_yrpfns

#include <atomic>
#include <cstddef>
#include <new>
#include <typeinfo>

/**
 * @namespace arc
 * @brief Atomic Reference Counter namespace - The fortress of thread-safe
 * memory management
 */
namespace arc
{

/**
 * @namespace arc::detail
 * @brief Internal implementation details - Here be dragons and cache-line magic
 */
namespace detail
{

/**
 * @brief Cache line size constant - The holy grail of CPU cache optimization
 *
 * Uses hardware destructive interference size to prevent false sharing.
 * This is where performance dreams come true and cache misses go to die.
 */
inline constexpr std::size_t cache_line_size =
    std::hardware_destructive_interference_size;

/**
 * @class control_block_base
 * @brief The nuclear reactor core of reference counting - Handles atomic
 * operations with surgical precision
 *
 * This beast manages the lifetime of shared objects through atomic reference
 * counting. Each instance is a fortress of thread safety, aligned to cache
 * boundaries for maximum performance. Strong counts track live references, weak
 * counts handle dangling pointer scenarios like a boss.
 *
 * @warning This class is pure virtual - you don't instantiate this directly.
 *          It's the foundation upon which empires of smart pointers are built.
 *
 * @warning Completely thread-safe. Atomic operations guarantee
 * consistency across all cores without locks. Performance is god-tier.
 */
struct control_block_base
{
    /**
     * @brief Strong reference counter - The lifeline of your precious data
     *
     * Tracks active shared_ptr-style references. When this hits zero,
     * your object gets obliterated faster than you can say "segfault".
     * Cache-line aligned because we're not savages.
     */
    alignas(cache_line_size) std::atomic<std::int64_t> strong_count{1};

    /**
     * @brief Weak reference counter - The ghost tracker for dangling scenarios
     *
     * Manages weak_ptr-style references that don't keep objects alive.
     * When both strong and weak counts hit zero, this control block
     * commits digital seppuku and frees itself.
     */
    alignas(cache_line_size) std::atomic<std::int64_t> weak_count{1};

    /**
     * @brief Custom aligned allocator - Memory allocation with style and
     * precision
     * @param size Number of bytes to allocate
     * @return Pointer to cache-line aligned memory block
     *
     * Allocates memory aligned to cache line boundaries because we're
     * performance obsessed and proud of it. False sharing is the enemy.
     */
    static void* operator new(std::size_t size)
    {
        return ::operator new(size, std::align_val_t{cache_line_size});
    }

    /**
     * @brief Custom aligned deallocator - Cleanup with the same precision as
     * allocation
     * @param ptr Pointer to deallocate
     * @param size Size of the memory block
     *
     * Frees aligned memory blocks. Matches the allocation strategy because
     * symmetry is beautiful and undefined behavior is not.
     */
    static void operator delete(void* ptr, std::size_t size) noexcept
    {
        ::operator delete(ptr, size, std::align_val_t{cache_line_size});
    }

    control_block_base() = default;
    control_block_base(const control_block_base&) = delete;
    control_block_base& operator=(const control_block_base&) = delete;

    virtual ~control_block_base() = default;

    /**
     * @brief Object disposal interface - Where objects go to meet their maker
     *
     * Called when strong_count reaches zero. This is your object's final moment
     * - implement this to properly destruct your managed object. The control
     * block itself might survive if weak references still exist.
     */
    virtual void dispose() noexcept = 0;

    /**
     * @brief Control block destruction interface - The final countdown
     *
     * Called when both strong and weak counts reach zero. This is where
     * the control block itself gets annihilated. Implement this to handle
     * any custom cleanup beyond basic destruction.
     */
    virtual void destroy() noexcept = 0;

    /**
     * @brief Custom deleter retrieval interface - For when you need surgical
     * precision
     * @param ti Type information for the requested deleter type
     * @return Pointer to deleter if type matches, nullptr otherwise
     *
     * Enables custom deletion strategies. If you stored a custom deleter during
     * construction, this is how you retrieve it. Type safety is enforced
     * through RTTI because we're civilized developers.
     */
    virtual void* get_deleter(const std::type_info& ti) noexcept = 0;
};

} // namespace detail

} // namespace arc

#endif //< End of include guard ARC_HPP_yrpfns