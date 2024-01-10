#ifndef ARC_HPP_yrpfns
#define ARC_HPP_yrpfns

#include <atomic>
#include <memory>
#include <type_traits>

namespace Arc
{
    template <typename T, typename Deleter = std::default_delete<T>>
    class Arc
    {
        static_assert(!std::is_array<T>::value, "Arc does not support array types.");

    private:
        T* ptr;
        std::atomic<unsigned int>* counter;
        Deleter deleter;

        void release_memory()
        {
            if (ptr && counter->fetch_sub(1) == 1) {
                deleter(ptr);
                delete counter;
            }
        }

    public:
        explicit Arc(T* p = nullptr, Deleter d = Deleter())
            : ptr(p), counter(new std::atomic<unsigned int>(1)), deleter(d)
        {
        }

        ~Arc() { release_memory(); }

        Arc(const Arc& other) : ptr(other.ptr), counter(other.counter), deleter(other.deleter)
        {
            counter->fetch_add(1);
        }

        Arc& operator=(const Arc& other)
        {
            if (this != &other) {
                release_memory();
                ptr = other.ptr;
                counter = other.counter;
                deleter = other.deleter;
                counter->fetch_add(1);
            }
            return *this;
        }

        Arc(Arc&& other) noexcept : ptr(other.ptr), counter(other.counter), deleter(std::move(other.deleter))
        {
            other.ptr = nullptr;
            other.counter = nullptr;
        }

        Arc& operator=(Arc&& other) noexcept
        {
            if (this != &other) {
                release_memory();
                ptr = other.ptr;
                counter = other.counter;
                deleter = std::move(other.deleter);
                other.ptr = nullptr;
                other.counter = nullptr;
            }
            return *this;
        }

        T& operator*() const { return *ptr; }

        T* operator->() const { return ptr; }

        bool unique() const { return counter && counter->load() == 1; }

        unsigned int use_count() const { return counter ? counter->load() : 0; }
    };
} // namespace Arc

#endif /* end of namespace: ARC_HPP_yrpfns */