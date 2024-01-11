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

    private:
        T* ptr;
        std::atomic<unsigned int>* counter;
        std::atomic<unsigned int>* weakCounter;
        Deleter deleter;

        void acquire()
        {
            if (ptr) {
                counter->fetch_add(1, std::memory_order_relaxed);
                weakCounter->fetch_add(1, std::memory_order_relaxed);
            }
        }

        void release_memory()
        {
            if (ptr && counter->fetch_sub(1, std::memory_order_release) == 1) {
                std::atomic_thread_fence(std::memory_order_acquire);
                deleter(ptr);
                release_weak(true);
            }
        }

        void release_weak(bool skipCounterDecrement = false)
        {
            if (weakCounter && (skipCounterDecrement || weakCounter->fetch_sub(1, std::memory_order_release) == 1)) {
                std::atomic_thread_fence(std::memory_order_acquire);
                if (counter->load(std::memory_order_relaxed) == 0) {
                    delete counter;
                }
                delete weakCounter;
            }
        }

    public:
        explicit Arc(T* p = nullptr, Deleter d = Deleter())
            : ptr(p),
              counter(new std::atomic<unsigned int>(1)),
              weakCounter(new std::atomic<unsigned int>(1)),
              deleter(d)
        {
        }

        ~Arc() { release_memory(); }

        Arc(const Arc& other)
            : ptr(other.ptr), counter(other.counter), weakCounter(other.weakCounter), deleter(other.deleter)
        {
            acquire();
        }

        Arc& operator=(const Arc& other)
        {
            if (this != &other) {
                release_memory();
                ptr = other.ptr;
                counter = other.counter;
                weakCounter = other.weakCounter;
                deleter = other.deleter;
                acquire();
            }
            return *this;
        }

        Arc(Arc&& other) noexcept
            : ptr(std::exchange(other.ptr, nullptr)),
              counter(std::exchange(other.counter, nullptr)),
              weakCounter(std::exchange(other.weakCounter, nullptr)),
              deleter(std::move(other.deleter))
        {
        }

        Arc& operator=(Arc&& other) noexcept
        {
            if (this != &other) {
                release_memory();
                ptr = std::exchange(other.ptr, nullptr);
                counter = std::exchange(other.counter, nullptr);
                weakCounter = std::exchange(other.weakCounter, nullptr);
                deleter = std::move(other.deleter);
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

        bool unique() const { return counter && counter->load(std::memory_order_relaxed) == 1; }
        unsigned int use_count() const { return counter ? counter->load(std::memory_order_relaxed) : 0; }
    };

    template <typename T, typename Deleter = std::default_delete<T>>
    class WeakArc
    {
    private:
        T* ptr;
        std::atomic<unsigned int>* counter;
        std::atomic<unsigned int>* weakCounter;

    public:
        WeakArc() : ptr(nullptr), counter(nullptr), weakCounter(nullptr) {}

        explicit WeakArc(const Arc<T, Deleter>& strongArc)
            : ptr(strongArc.ptr), counter(strongArc.counter), weakCounter(strongArc.weakCounter)
        {
            if (weakCounter) {
                weakCounter->fetch_add(1, std::memory_order_relaxed);
            }
        }

        ~WeakArc()
        {
            if (weakCounter && weakCounter->fetch_sub(1, std::memory_order_release) == 1) {
                std::atomic_thread_fence(std::memory_order_acquire);
                if (counter->load(std::memory_order_relaxed) == 0) {
                    delete counter;
                }
                delete weakCounter;
            }
        }

        Arc<T, Deleter> lock() const
        {
            if (counter && counter->load(std::memory_order_relaxed) > 0) {
                return Arc<T, Deleter>(ptr);
            }
            return Arc<T, Deleter>();
        }
    };
} // namespace Arc

#endif /* end of namespace: ARC_HPP_yrpfns */
