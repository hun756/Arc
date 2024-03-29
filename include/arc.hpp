#ifndef ARC_HPP_yrpfns
#define ARC_HPP_yrpfns

#include <atomic>
#include <cassert>
#include <memory>
#include <type_traits>
#include <utility>

namespace Arc
{
    template <typename T, typename Deleter = std::default_delete<T>>
    class Arc;

    template <typename T, typename Deleter = std::default_delete<T>>
    class WeakArc;

    template <typename T, typename Deleter>
    class ControlBlock
    {
        friend class Arc<T, Deleter>;
        friend class WeakArc<T, Deleter>;

        std::atomic<unsigned int> strongCount;
        std::atomic<unsigned int> weakCount;
        Deleter deleter;

    public:
        ControlBlock(Deleter d) : strongCount(1), weakCount(1), deleter(std::move(d)) {}
    };

    template <typename T, typename Deleter>
    class Arc
    {
        friend class WeakArc<T, Deleter>;

        static_assert(!std::is_array<T>::value, "Arc does not support array types.");

    private:
        T* ptr;
        ControlBlock<T, Deleter>* control;

        void acquire()
        {
            if (control) {
                control->strongCount.fetch_add(1, std::memory_order_relaxed);
                control->weakCount.fetch_add(1, std::memory_order_relaxed);
            }
        }

        void release()
        {
            if (control && control->strongCount.fetch_sub(1, std::memory_order_release) == 1) {
                std::atomic_thread_fence(std::memory_order_acquire);
                control->deleter(ptr);
                release_weak(true);
            }
        }

        void release_weak(bool skipCounterDecrement = false)
        {
            if (control) {
                if (!skipCounterDecrement) {
                    control->weakCount.fetch_sub(1, std::memory_order_release);
                }
                std::atomic_thread_fence(std::memory_order_acquire);
                if (control->strongCount.load(std::memory_order_relaxed) == 0 &&
                    control->weakCount.load(std::memory_order_relaxed) == 0) {
                    delete control;
                    control = nullptr;
                }
            }
        }

    public:
        explicit Arc(T* p = nullptr, Deleter d = Deleter())
            : ptr(p), control(p ? new ControlBlock<T, Deleter>(d) : nullptr)
        {
        }

        ~Arc() { release(); }

        Arc(const Arc& other) : ptr(other.ptr), control(other.control) { acquire(); }

        Arc& operator=(const Arc& other)
        {
            if (this != &other) {
                release();
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
                release();
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

        bool expired() const { return !control || control->strongCount.load(std::memory_order_relaxed) == 0; }

        template <typename CustomDeleter>
        void set_deleter(CustomDeleter&& deleter)
        {
            if (control) {
                control->deleter = std::forward<CustomDeleter>(deleter);
            }
        }

        template <typename... Args>
        static Arc<T, Deleter> make_arc(Args&&... args)
        {
            T* obj = new T(std::forward<Args>(args)...);
            return Arc<T, Deleter>(obj);
        }

        const Deleter& get_deleter() const { return control->deleter; }
    };

    template <typename T, typename Deleter>
    class WeakArc
    {
    private:
        T* ptr;
        ControlBlock<T, Deleter>* control;

    public:
        WeakArc() : ptr(nullptr), control(nullptr) {}

        explicit WeakArc(const Arc<T, Deleter>& strongArc) : ptr(strongArc.ptr), control(strongArc.control)
        {
            if (control) {
                control->weakCount.fetch_add(1, std::memory_order_relaxed);
            }
        }

        WeakArc(const WeakArc& other) : ptr(other.ptr), control(other.control)
        {
            if (control) {
                control->weakCount.fetch_add(1, std::memory_order_relaxed);
            }
        }

        WeakArc& operator=(const WeakArc& other)
        {
            if (this != &other) {
                if (control && control->weakCount.fetch_sub(1, std::memory_order_release) == 1) {
                    std::atomic_thread_fence(std::memory_order_acquire);
                    if (control->strongCount.load(std::memory_order_relaxed) == 0) {
                        delete control;
                    }
                }
                ptr = other.ptr;
                control = other.control;
                if (control) {
                    control->weakCount.fetch_add(1, std::memory_order_relaxed);
                }
            }
            return *this;
        }

        WeakArc& operator=(const Arc<T, Deleter>& other)
        {
            if (this->ptr != other.ptr) {
                if (control && control->weakCount.fetch_sub(1, std::memory_order_release) == 1) {
                    std::atomic_thread_fence(std::memory_order_acquire);
                    if (control->strongCount.load(std::memory_order_relaxed) == 0) {
                        delete control;
                    }
                }
                ptr = other.ptr;
                control = other.control;
                if (control) {
                    control->weakCount.fetch_add(1, std::memory_order_relaxed);
                }
            }
            return *this;
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
                return Arc<T, Deleter>(ptr, *control->deleter);
            }
            return Arc<T, Deleter>();
        }

        bool expired() const { return !control || control->strongCount.load(std::memory_order_relaxed) == 0; }
    };

} // namespace Arc

#endif /* end of include guard: ARC_HPP_yrpfns */
