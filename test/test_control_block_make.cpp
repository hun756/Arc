#include <arc/detail/control_block_make.hpp>
#include <gtest/gtest.h>
#include <memory>
#include <string>

struct LifetimeTracker
{
    static int constructor_calls;
    static int destructor_calls;

    std::string data;

    LifetimeTracker(const std::string& d) : data(d) { constructor_calls++; }
    LifetimeTracker(int val) : data(std::to_string(val))
    {
        constructor_calls++;
    }

    ~LifetimeTracker() { destructor_calls++; }

    static void reset()
    {
        constructor_calls = 0;
        destructor_calls = 0;
    }
};

int LifetimeTracker::constructor_calls = 0;
int LifetimeTracker::destructor_calls = 0;

class ControlBlockMakeTest : public ::testing::Test
{
protected:
    arc::detail::control_block_base* cb = nullptr;

    void SetUp() override { LifetimeTracker::reset(); }

    void TearDown() override
    {
        if (cb)
        {
            cb->destroy();
            cb = nullptr;
        }
    }

    template <typename... Args>
    void create_control_block(Args&&... args)
    {
        using Alloc = std::allocator<LifetimeTracker>;
        using CbType = arc::detail::control_block_make<LifetimeTracker, Alloc>;

        std::allocator<CbType> cb_alloc;
        cb = cb_alloc.allocate(1);

        ::new (cb) CbType(Alloc{}, std::forward<Args>(args)...);
    }
};

TEST_F(ControlBlockMakeTest, ConstructorCallsObjectConstructor)
{
    ASSERT_EQ(LifetimeTracker::constructor_calls, 0);

    create_control_block("test_data");

    ASSERT_EQ(LifetimeTracker::constructor_calls, 1);
    ASSERT_EQ(LifetimeTracker::destructor_calls, 0);

    auto* cb_make = static_cast<arc::detail::control_block_make<
        LifetimeTracker, std::allocator<LifetimeTracker>>*>(cb);
    ASSERT_EQ(cb_make->get_ptr()->data, "test_data");
}

TEST_F(ControlBlockMakeTest, DisposeCallsObjectDestructor)
{
    create_control_block(42);
    ASSERT_EQ(LifetimeTracker::constructor_calls, 1);
    ASSERT_EQ(LifetimeTracker::destructor_calls, 0);

    cb->dispose();

    ASSERT_EQ(LifetimeTracker::destructor_calls, 1);
}

TEST_F(ControlBlockMakeTest, GetDeleterAlwaysReturnsNull)
{
    create_control_block("hello");
    void* deleter_ptr = cb->get_deleter(typeid(void));
    ASSERT_EQ(deleter_ptr, nullptr);
}