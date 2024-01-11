#ifndef ARC_HPP_yrpfns
#define ARC_HPP_yrpfns

#include <atomic>
#include <cassert>
#include <memory>
#include <type_traits>

namespace Arc
{
    template <typename T, typename Deleter>
    class WeakArc;

    template <typename T, typename Deleter = std::default_delete<T>>
    class Arc
    {
        friend class WeakArc<T, Deleter>;

        static_assert(!std::is_array<T>::value, "Arc does not support array types.");

        struct ControlBlock {
            std::atomic<unsigned int> strongCount;
            std::atomic<unsigned int> weakCount;
            Deleter deleter;

            ControlBlock(Deleter d) : strongCount(1), weakCount(1), deleter(std::move(d)) {}
        };

    private:
        T* ptr;
        ControlBlock* control;

        void acquire()
        {
            if (control) {
                control->strongCount.fetch_add(1, std::memory_order_relaxed);
                control->weakCount.fetch_add(1, std::memory_order_relaxed);
            }
        }

        void release_memory()
        {
            if (control && control->strongCount.fetch_sub(1, std::memory_order_release) == 1) {
                std::atomic_thread_fence(std::memory_order_acquire);
                control->deleter(ptr);
                release_weak(true);
            }
        }

        void release_weak(bool skipCounterDecrement = false)
        {
            if (control && (skipCounterDecrement || control->weakCount.fetch_sub(1, std::memory_order_release) == 1)) {
                std::atomic_thread_fence(std::memory_order_acquire);
                if (control->strongCount.load(std::memory_order_relaxed) == 0) {
                    delete control;
                }
            }
        }

    public:
        explicit Arc(T* p = nullptr, Deleter d = Deleter()) : ptr(p), control(p ? new ControlBlock(d) : nullptr) {}

        ~Arc() { release_memory(); }

        Arc(const Arc& other) : ptr(other.ptr), control(other.control) { acquire(); }

        Arc& operator=(const Arc& other)
        {
            if (this != &other) {
                release_memory();
                ptr = other.ptr;
                control = other.control;
                acquire();
            }
            return *this;
        }

        Arc(Arc&& other) noexcept
            : ptr(std::exchange(other.ptr, nullptr)), control(std::exchange(other.control, nullptr))
        {
        }

        Arc& operator=(Arc&& other) noexcept
        {
            if (this != &other) {
                release_memory();
                ptr = std::exchange(other.ptr, nullptr);
                control = std::exchange(other.control, nullptr);
            }
            return *this;
        }

        T& operator*() const
        {
            assert(ptr);
            return *ptr;
        }

        T* operator->() const
        {
            assert(ptr);
            return ptr;
        }

        bool unique() const { return control && control->strongCount.load(std::memory_order_relaxed) == 1; }
        unsigned int use_count() const { return control ? control->strongCount.load(std::memory_order_relaxed) : 0; }
    };

    template <typename T, typename Deleter = std::default_delete<T>>
    class WeakArc
    {
    private:
        T* ptr;
        typename Arc<T, Deleter>::ControlBlock* control;

    public:
        WeakArc() : ptr(nullptr), control(nullptr) {}

        explicit WeakArc(const Arc<T, Deleter>& strongArc) : ptr(strongArc.ptr), control(strongArc.control)
        {
            if (control) {
                control->weakCount.fetch_add(1, std::memory_order_relaxed);
            }
        }

        ~WeakArc()
        {
            if (control && control->weakCount.fetch_sub(1, std::memory_order_release) == 1) {
                std::atomic_thread_fence(std::memory_order_acquire);
                if (control->strongCount.load(std::memory_order_relaxed) == 0) {
                    delete control;
                }
            }
        }

        Arc<T, Deleter> lock() const
        {
            if (control && control->strongCount.load(std::memory_order_relaxed) > 0) {
                return Arc<T, Deleter>(ptr);
            }
            return Arc<T, Deleter>();
        }
    };
} // namespace Arc

#endif /* end of namespace: ARC_HPP_yrpfns */
