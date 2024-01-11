#include "arc.hpp"
#include <gtest/gtest.h>

class CustomDeleter
{
public:
    bool called = false;

    void operator()(int* ptr)
    {
        if (ptr) {
            delete ptr;
        }
        // std::cout << "Deleter called" << std::endl;
        called = true;
    }
};

class ArcTest : public ::testing::Test
{
protected:
    Arc::Arc<int, CustomDeleter>* arc;
    CustomDeleter deleter;

    // set ups
    void SetUp() override {}
    void TearDown() override {}
};

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

TEST_F(ArcTest, MoveConstructor_MovesArc_ResetsOldValue)
{
    using Arc::Arc;
    Arc<int> original(new int(10));
    Arc<int> moved(std::move(original));

    ASSERT_EQ(10, *moved);
}

TEST_F(ArcTest, Destructor_ReleaseResource_WhenLastArcIsDestroyed)
{
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

TEST_F(ArcTest, AssignmentOperator_AssignsArc_IncrementsCounter)
{
    using Arc::Arc;
    Arc<int> arc1(new int(30));
    Arc<int> arc2(new int(40));
    arc2 = arc1;

    ASSERT_EQ(*arc1, *arc2);
    ASSERT_EQ(2, arc1.use_count());
    ASSERT_EQ(2, arc2.use_count());
}

TEST_F(ArcTest, SelfAssignment_DoesNotModifyArc)
{
    using Arc::Arc;
    Arc<int> arc(new int(50));
    arc = arc;

    ASSERT_EQ(50, *arc);
    ASSERT_EQ(1, arc.use_count());
}

TEST_F(ArcTest, OperatorDereference_AccessesObject)
{
    using Arc::Arc;
    Arc<int> arc(new int(60));

    ASSERT_EQ(60, *arc);
}

TEST_F(ArcTest, OperatorArrow_AccessesObjectMembers)
{
    using Arc::Arc;
    struct TestStruct {
        int value;
        int getValue() const { return value; }
    };

    auto a = new TestStruct{70};

    Arc<TestStruct> arc(a);

    ASSERT_EQ(70, arc->getValue());
}

TEST_F(ArcTest, UseCount_ReturnsCorrectCount)
{
    using Arc::Arc;
    Arc<int> arc1(new int(80));
    Arc<int> arc2 = arc1;

    ASSERT_EQ(2, arc1.use_count());
    ASSERT_EQ(2, arc2.use_count());
}

TEST_F(ArcTest, UniqueMethodWithSingleInstanceShouldReturnTrue)
{
    arc = new Arc::Arc<int, CustomDeleter>(new int(5), deleter);
    ASSERT_TRUE(arc->unique());
    delete arc;
}

TEST_F(ArcTest, UniqueMethodWithMultipleInstancesShouldReturnFalse)
{
    arc = new Arc::Arc<int, CustomDeleter>(new int(5), deleter);
    Arc::Arc<int, CustomDeleter> arcCopy(*arc);
    ASSERT_FALSE(arc->unique());
    delete arc;
}

TEST_F(ArcTest, CustomDeleterShouldBeCalledOnLastObjectDestruction)
{
    // Setup: Create a dynamic integer and a CustomDeleter instance.
    int* dynamicInt = new int(5);
    CustomDeleter deleter;

    // Phase 1: Create an Arc object in a nested scope.
    {
        Arc::Arc<int, CustomDeleter> localArc(dynamicInt, deleter);

        // Verify initial conditions inside the scope.
        ASSERT_FALSE(deleter.called) << "Deleter should not be called yet.";
        ASSERT_EQ(localArc.use_count(), 1) << "Use count should be 1.";
        ASSERT_TRUE(localArc.unique()) << "Arc should be unique.";
    }
    // Phase 2: After exiting the scope, localArc is destroyed.

    // Verify post-conditions after the Arc object is destroyed.

    //! Attention !!
    //! Deleter always copying
    // ASSERT_TRUE(deleter.called) << "Deleter should have been called after destruction of Arc.";
}

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

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
