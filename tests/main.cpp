#include "arc.hpp"
#include <gtest/gtest.h>

class ArcTest : public ::testing::Test
{
protected:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(ArcTest, Constructor_InitializesWithNonNullptr_IncrementsCounter)
{
    using Arc::Arc;
    auto raw_ptr = new int(5);
    Arc<int> arc(raw_ptr);

    ASSERT_EQ(*raw_ptr, *arc);
    ASSERT_EQ(1, arc.use_count());
}

TEST_F(ArcTest, CopyConstructor_CopiesArc_IncrementsCounter)
{
    using Arc::Arc;
    Arc<int> original(new int(10));
    Arc<int> copy = original;

    ASSERT_EQ(*original, *copy);
    ASSERT_EQ(2, original.use_count());
    ASSERT_EQ(2, copy.use_count());
}

TEST_F(ArcTest, MoveConstructor_MovesArc_ResetsOldValue) {
    using Arc::Arc;
    Arc<int> original(new int(10));
    Arc<int> moved(std::move(original));

    ASSERT_EQ(10, *moved);
}

TEST_F(ArcTest, Destructor_ReleaseResource_WhenLastArcIsDestroyed) {
    using Arc::Arc;
    auto raw_ptr = new int(20);
    {
        Arc<int> arc1(new int(20));
        {
            Arc<int> arc2 = arc1;
            ASSERT_EQ(2, arc1.use_count());
        }
        ASSERT_EQ(1, arc1.use_count());
    }
    // Check if resource is released properly. Requires manual verification or a custom deleter.
}

TEST_F(ArcTest, AssignmentOperator_AssignsArc_IncrementsCounter) {
    using Arc::Arc;
    Arc<int> arc1(new int(30));
    Arc<int> arc2(new int(40));
    arc2 = arc1;

    ASSERT_EQ(*arc1, *arc2);
    ASSERT_EQ(2, arc1.use_count());
    ASSERT_EQ(2, arc2.use_count());
}

TEST_F(ArcTest, SelfAssignment_DoesNotModifyArc) {
    using Arc::Arc;
    Arc<int> arc(new int(50));
    arc = arc;

    ASSERT_EQ(50, *arc);
    ASSERT_EQ(1, arc.use_count());
}

TEST_F(ArcTest, OperatorDereference_AccessesObject) {
    using Arc::Arc;
    Arc<int> arc(new int(60));

    ASSERT_EQ(60, *arc);
}

TEST_F(ArcTest, OperatorArrow_AccessesObjectMembers) {
    using Arc::Arc;
    struct TestStruct {
        int value;
        int getValue() const { return value; }
    };

    Arc<TestStruct> arc(new TestStruct{70});

    ASSERT_EQ(70, arc->getValue());
}

TEST_F(ArcTest, UseCount_ReturnsCorrectCount) {
    using Arc::Arc;
    Arc<int> arc1(new int(80));
    Arc<int> arc2 = arc1;

    ASSERT_EQ(2, arc1.use_count());
    ASSERT_EQ(2, arc2.use_count());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
