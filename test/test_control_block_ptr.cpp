#include <arc/arc.hpp>
#include <gtest/gtest.h>
#include <memory>


static int* g_deleted_ptr = nullptr;

struct TrackingDeleter
{
    void operator()(int* p) const noexcept
    {
        if (g_deleted_ptr)
        {
            *g_deleted_ptr = *p;
        }
        delete p;
    }
};

static bool g_control_block_destroyed = false;

template <typename T>
struct TrackingAllocator
{
    using value_type = T;

    TrackingAllocator() = default;
    template <class U>
    constexpr TrackingAllocator(const TrackingAllocator<U>&) noexcept
    {
    }

    T* allocate(std::size_t n) { return std::allocator<T>{}.allocate(n); }

    void deallocate(T* p, std::size_t n) noexcept
    {
        g_control_block_destroyed = true;
        std::allocator<T>{}.deallocate(p, n);
    }
};

class ControlBlockPtrTest : public ::testing::Test
{
protected:
    int* managed_object = nullptr;
    arc::detail::control_block_base* cb = nullptr;
    int deleted_value_tracker = 0;

    void SetUp() override
    {
        g_deleted_ptr = &deleted_value_tracker;
        *g_deleted_ptr = 0;
        g_control_block_destroyed = false;

        managed_object = new int(123);

        using CbType = arc::detail::control_block_ptr<int, TrackingDeleter,
                                                      TrackingAllocator<int>>;

        TrackingAllocator<CbType> alloc;
        cb = alloc.allocate(1);

        ::new (cb)
            CbType(managed_object, TrackingDeleter{}, TrackingAllocator<int>{});
    }

    void TearDown() override
    {
        if (!g_control_block_destroyed)
        {
            cb->dispose();
            cb->destroy();
        }
    }
};

TEST_F(ControlBlockPtrTest, DisposeCallsDeleter)
{
    ASSERT_NE(cb, nullptr);
    ASSERT_EQ(deleted_value_tracker, 0);

    cb->dispose();

    EXPECT_EQ(deleted_value_tracker, 123);
    EXPECT_FALSE(g_control_block_destroyed);
}

TEST_F(ControlBlockPtrTest, DestroyCallsAllocatorDeallocate)
{
    ASSERT_FALSE(g_control_block_destroyed);

    cb->destroy();

    EXPECT_TRUE(g_control_block_destroyed);
}

TEST_F(ControlBlockPtrTest, GetDeleterReturnsCorrectly)
{
    void* deleter_ptr = cb->get_deleter(typeid(TrackingDeleter));
    EXPECT_NE(deleter_ptr, nullptr);

    void* invalid_ptr = cb->get_deleter(typeid(int));
    EXPECT_EQ(invalid_ptr, nullptr);
}
