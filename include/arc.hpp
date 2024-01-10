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

        void release_memory()
        {
            if (ptr && counter->fetch_sub(1) == 1) {
                deleter(ptr);
                delete counter;
                if (weakCounter->fetch_sub(1) == 1) {
                    delete weakCounter;
                }
            }
        }

        void release_weak()
        {
            if (weakCounter && weakCounter->fetch_sub(1) == 1) {
                if (counter->load() == 0) {
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
            counter->fetch_add(1);
            weakCounter->fetch_add(1);
        }

        Arc& operator=(const Arc& other)
        {
            if (this != &other) {
                release_memory();
                ptr = other.ptr;
                counter = other.counter;
                weakCounter = other.weakCounter;
                deleter = other.deleter;
                counter->fetch_add(1);
                weakCounter->fetch_add(1);
            }
            return *this;
        }

        Arc(Arc&& other) noexcept
            : ptr(other.ptr),
              counter(other.counter),
              weakCounter(other.weakCounter),
              deleter(std::move(other.deleter))
        {
            other.ptr = nullptr;
            other.counter = nullptr;
            other.weakCounter = nullptr;
        }

        Arc& operator=(Arc&& other) noexcept
        {
            if (this != &other) {
                release_memory();
                ptr = other.ptr;
                counter = other.counter;
                weakCounter = other.weakCounter;
                deleter = std::move(other.deleter);
                other.ptr = nullptr;
                other.counter = nullptr;
                other.weakCounter = nullptr;
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

        bool unique() const { return counter && counter->load() == 1; }
        unsigned int use_count() const { return counter ? counter->load() : 0; }
    };

    template <typename T, typename Deleter = std::default_delete<T>>
    class WeakArc
    {
    private:
        T* ptr;
        std::atomic<unsigned int>* counter;
        std::atomic<unsigned int>* weakCounter;

    public:
        explicit WeakArc(const Arc<T, Deleter>& strongArc)
            : ptr(strongArc.ptr), counter(strongArc.counter), weakCounter(strongArc.weakCounter)
        {
            weakCounter->fetch_add(1);
        }

        ~WeakArc()
        {
            if (weakCounter && weakCounter->fetch_sub(1) == 1) {
                if (counter->load() == 0) {
                    delete counter;
                }
                delete weakCounter;
            }
        }

        Arc<T, Deleter> lock() const
        {
            if (counter && counter->load() > 0) {
                return Arc<T, Deleter>(*this);
            }
            return Arc<T, Deleter>();
        }
    };
} // namespace Arc

#endif /* end of namespace: ARC_HPP_yrpfns */