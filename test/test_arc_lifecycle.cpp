#include <arc/arc.hpp>
#include <gtest/gtest.h>

struct LifetimeTracker
{
    static int constructor_calls;
    static int destructor_calls;

    LifetimeTracker() { constructor_calls++; }
    ~LifetimeTracker() { destructor_calls++; }

    static void reset()
    {
        constructor_calls = 0;
        destructor_calls = 0;
    }
};

int LifetimeTracker::constructor_calls = 0;
int LifetimeTracker::destructor_calls = 0;

class ArcLifecycleTest : public ::testing::Test
{
protected:
    void SetUp() override { LifetimeTracker::reset(); }
};

TEST_F(ArcLifecycleTest, DefaultConstruction)
{
    arc::arc<int> p;
    EXPECT_FALSE(p);
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(p.use_count(), 0);
}

TEST_F(ArcLifecycleTest, ConstructionFromRawPointer)
{
    {
        arc::arc<LifetimeTracker> p(new LifetimeTracker());
        EXPECT_EQ(LifetimeTracker::constructor_calls, 1);
        EXPECT_EQ(LifetimeTracker::destructor_calls, 0);
        EXPECT_TRUE(p);
        EXPECT_NE(p.get(), nullptr);
        EXPECT_EQ(p.use_count(), 1);
    }
    EXPECT_EQ(LifetimeTracker::destructor_calls, 1);
}

TEST_F(ArcLifecycleTest, CopyConstruction)
{
    arc::arc<LifetimeTracker> p1(new LifetimeTracker());
    EXPECT_EQ(p1.use_count(), 1);
    {
        arc::arc<LifetimeTracker> p2 = p1;
        EXPECT_EQ(p1.use_count(), 2);
        EXPECT_EQ(p2.use_count(), 2);
        EXPECT_EQ(p1.get(), p2.get());
    }
    EXPECT_EQ(p1.use_count(), 1);
    EXPECT_EQ(LifetimeTracker::destructor_calls, 0);
}

TEST_F(ArcLifecycleTest, MoveConstruction)
{
    arc::arc<LifetimeTracker> p1(new LifetimeTracker());
    LifetimeTracker* raw_ptr = p1.get();

    arc::arc<LifetimeTracker> p2 = std::move(p1);

    EXPECT_FALSE(p1);
    EXPECT_EQ(p1.get(), nullptr);
    EXPECT_EQ(p1.use_count(), 0);

    EXPECT_TRUE(p2);
    EXPECT_EQ(p2.get(), raw_ptr);
    EXPECT_EQ(p2.use_count(), 1);
    EXPECT_EQ(LifetimeTracker::destructor_calls, 0);
}

TEST_F(ArcLifecycleTest, CopyAssignment)
{
    arc::arc<LifetimeTracker> p1(new LifetimeTracker());
    arc::arc<LifetimeTracker> p2(new LifetimeTracker());
    EXPECT_EQ(LifetimeTracker::constructor_calls, 2);

    p1 = p2;
    EXPECT_EQ(LifetimeTracker::destructor_calls, 1);
    EXPECT_EQ(p1.use_count(), 2);
    EXPECT_EQ(p2.use_count(), 2);
    EXPECT_EQ(p1.get(), p2.get());
}

TEST_F(ArcLifecycleTest, Reset)
{
    arc::arc<LifetimeTracker> p(new LifetimeTracker());
    EXPECT_EQ(LifetimeTracker::constructor_calls, 1);

    p.reset();

    EXPECT_EQ(LifetimeTracker::destructor_calls, 1);
    EXPECT_FALSE(p);
    EXPECT_EQ(p.use_count(), 0);
}
