#include "arc.hpp"
#include <gtest/gtest.h>
// #include <thread>

namespace
{
    class TestResource
    {
    public:
        static int aliveCount;
        TestResource() { ++aliveCount; }
        ~TestResource() { --aliveCount; }
    };

    int TestResource::aliveCount = 0;

    class WeakArcTestResource : public ::testing::Test
    {
    protected:
        void SetUp() override { TestResource::aliveCount = 0; }

        void TearDown() override
        {
            // Additional teardown can be done here
        }
    };

} // namespace

TEST_F(WeakArcTestResource, SingleObjectLifetime)
{
    {
        Arc::Arc<TestResource> ptr(new TestResource());
        EXPECT_EQ(TestResource::aliveCount, 1);
    }
    EXPECT_EQ(TestResource::aliveCount, 0);
}

TEST_F(WeakArcTestResource, CopyConstructor)
{
    Arc::Arc<TestResource> ptr1(new TestResource());
    {
        Arc::Arc<TestResource> ptr2(ptr1);
        EXPECT_EQ(ptr1.use_count(), 2);
    }
    EXPECT_EQ(ptr1.use_count(), 1);
}

TEST_F(WeakArcTestResource, MoveConstructor)
{
    Arc::Arc<TestResource> ptr1(new TestResource());
    Arc::Arc<TestResource> ptr2(std::move(ptr1));
    EXPECT_EQ(ptr2.use_count(), 1);
    EXPECT_EQ(ptr1.use_count(), 0);
}

// ! Attention : May cannot work correctly on test main thread please test
// ! in local main
// ! !! problem to testing thread ordering !!
// TEST_F(WeakArcTestResource, ThreadSafety)
// {
//     Arc::Arc<TestResource> ptr(new TestResource());
//     std::thread t1([&ptr] {
//         Arc::Arc<TestResource> localPtr(ptr);
//         EXPECT_EQ(ptr.use_count(), 2);
//     });
//     std::thread t2([&ptr] {
//         Arc::Arc<TestResource> localPtr(ptr);
//         // EXPECT_EQ(ptr.use_count(), 2);
// or
//         EXPECT_EQ(ptr.use_count(), 3);
//     });
//     t1.join();
//     t2.join();
// }

TEST_F(WeakArcTestResource, WeakPointerExpired)
{
    Arc::WeakArc<TestResource> weakPtr;
    {
        Arc::Arc<TestResource> strongPtr(new TestResource());
        weakPtr = strongPtr;
    }
    EXPECT_TRUE(weakPtr.expired());
}
