#ifndef ARC_yrpfns
#define ARC_yrpfns

#include <atomic>
// #include <memory>

namespace Arc
{
    template <typename T>
    class Arc
    {
    private:
        T* ptr;
        std::atomic<unsigned int>* counter;

    public:
        Arc(T* p = nullptr) : ptr(p), counter(new std::atomic<unsigned int>(1)) {}

        ~Arc()
        {
            if (ptr && counter->fetch_sub(1) == 1) {
                delete ptr;
                delete counter;
            }
        }

        Arc(const Arc& other) : ptr(other.ptr), counter(other.counter) { counter->fetch_add(1); }

        Arc& operator=(const Arc& other)
        {
            if (this != &other) {
                if (ptr && counter->fetch_sub(1) == 1) {
                    delete ptr;
                    delete counter;
                }
                ptr = other.ptr;
                counter = other.counter;
                counter->fetch_add(1);
            }
            return *this;
        }

        T& operator*() const { return *ptr; }

        T* operator->() const { return ptr; }

        unsigned int use_count() const { return counter ? counter->load() : 0; }
    };
} // namespace Arc

#endif /* end of namespace: ARC_yrpfns */