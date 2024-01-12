#include "arc.hpp"
#include <gtest/gtest.h>

namespace
{
    class ArcWeakArcTest : public ::testing::Test
    {
    protected:
        struct TestObject {
            static int aliveCount;
            TestObject() { ++aliveCount; }
            ~TestObject() { --aliveCount; }
        };

        void SetUp() override { TestObject::aliveCount = 0; }
    };

    int ArcWeakArcTest::TestObject::aliveCount = 0;
} // namespace

// TODO : ! Fix it
// TEST_F(ArcWeakArcTest, MultipleStrongArcsOneWeakArc)
// {
//     auto strongArc1 = Arc::Arc<TestObject>(new TestObject());
//     auto strongArc2 = strongArc1;
//     Arc::WeakArc<TestObject> weakArc(strongArc1);

//     EXPECT_EQ(weakArc.lock().use_count(), 2);
//     EXPECT_EQ(TestObject::aliveCount, 1);
// }
//! ----------------

TEST_F(ArcWeakArcTest, CustomDeleterWithWeakArc)
{
    bool deleted = false;
    auto deleter = [&deleted](TestObject* obj) {
        delete obj;
        deleted = true;
    };

    {
        auto strongArc = Arc::Arc<TestObject, decltype(deleter)>(new TestObject(), deleter);
        Arc::WeakArc<TestObject, decltype(deleter)> weakArc(strongArc);
    }

    EXPECT_TRUE(deleted);
    EXPECT_EQ(TestObject::aliveCount, 0);
}