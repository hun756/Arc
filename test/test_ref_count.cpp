#include <arc/detail/ref_count.hpp>
#include <gtest/gtest.h>

class MockRefCountControlBlock : public arc::detail::control_block_base
{
public:
    bool disposed = false;
    bool destroyed = false;

    MockRefCountControlBlock(long long strong, long long weak)
    {
        strong_count.store(strong);
        weak_count.store(weak);
    }

    void dispose() noexcept override { disposed = true; }
    void destroy() noexcept override { destroyed = true; }
    void* get_deleter(const std::type_info&) noexcept override
    {
        return nullptr;
    }
};

TEST(RefCountTest, IncrementStrong)
{
    MockRefCountControlBlock cb(5, 5);
    arc::detail::increment_strong<void>(&cb);
    EXPECT_EQ(cb.strong_count.load(), 6);
}

TEST(RefCountTest, IncrementWeak)
{
    MockRefCountControlBlock cb(5, 5);
    arc::detail::increment_weak<void>(&cb);
    EXPECT_EQ(cb.weak_count.load(), 6);
}

TEST(RefCountTest, DecrementStrongFromMultiple)
{
    MockRefCountControlBlock cb(2, 1);
    arc::detail::decrement_strong<void>(&cb);

    EXPECT_EQ(cb.strong_count.load(), 1);
    EXPECT_EQ(cb.weak_count.load(), 1);
    EXPECT_FALSE(cb.disposed);
    EXPECT_FALSE(cb.destroyed);
}

TEST(RefCountTest, DecrementStrongToZero)
{
    MockRefCountControlBlock cb(1, 2);
    arc::detail::decrement_strong<void>(&cb);

    EXPECT_EQ(cb.strong_count.load(), 0);
    EXPECT_EQ(cb.weak_count.load(), 1);
    EXPECT_TRUE(cb.disposed);
    EXPECT_FALSE(cb.destroyed);
}

TEST(RefCountTest, DecrementWeakFromMultiple)
{
    MockRefCountControlBlock cb(0, 3);
    arc::detail::decrement_weak<void>(&cb);

    EXPECT_EQ(cb.weak_count.load(), 2);
    EXPECT_FALSE(cb.destroyed);
}

TEST(RefCountTest, DecrementWeakToZero)
{
    MockRefCountControlBlock cb(0, 1);
    arc::detail::decrement_weak<void>(&cb);

    EXPECT_EQ(cb.weak_count.load(), 0);
    EXPECT_TRUE(cb.destroyed);
}

TEST(RefCountTest, DecrementLastStrongAndWeak)
{
    MockRefCountControlBlock cb(1, 1);
    arc::detail::decrement_strong<void>(&cb);

    EXPECT_EQ(cb.strong_count.load(), 0);
    EXPECT_EQ(cb.weak_count.load(), 0);
    EXPECT_TRUE(cb.disposed);
    EXPECT_TRUE(cb.destroyed);
}
