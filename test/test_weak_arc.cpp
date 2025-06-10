#include <arc/arc.hpp>
#include <gtest/gtest.h>
#include <vector>

TEST(WeakArcTest, ConstructionFromArc)
{
    arc::arc<int> p_strong(new int(10));
    EXPECT_EQ(p_strong.use_count(), 1);

    {
        arc::weak_arc<int> p_weak(p_strong);
        EXPECT_EQ(p_strong.use_count(), 1);
        EXPECT_EQ(p_weak.use_count(), 1);
    }
    EXPECT_EQ(p_strong.use_count(), 1);
}

TEST(WeakArcTest, Expired)
{
    arc::weak_arc<int> p_weak;
    {
        arc::arc<int> p_strong(new int(10));
        p_weak = p_strong;
        EXPECT_FALSE(p_weak.expired());
    }

    EXPECT_TRUE(p_weak.expired());
    EXPECT_EQ(p_weak.use_count(), 0);
}

TEST(WeakArcTest, LockSuccess)
{
    arc::arc<int> p_strong(new int(20));
    arc::weak_arc<int> p_weak(p_strong);

    arc::arc<int> p_locked = p_weak.lock();

    EXPECT_TRUE(p_locked);
    EXPECT_EQ(p_strong.use_count(), 2);
    EXPECT_EQ(*p_locked, 20);
}

TEST(WeakArcTest, LockFailure)
{
    arc::weak_arc<int> p_weak;
    {
        arc::arc<int> p_strong(new int(30));
        p_weak = p_strong;
    }

    EXPECT_TRUE(p_weak.expired());
    arc::arc<int> p_locked = p_weak.lock();
    EXPECT_FALSE(p_locked);
}

TEST(WeakArcTest, ConstructionFromExpiredWeak)
{
    arc::weak_arc<int> p_weak;
    {
        arc::arc<int> p_strong(new int(40));
        p_weak = p_strong;
    }
    EXPECT_TRUE(p_weak.expired());

    EXPECT_THROW(arc::arc<int> p_fail(p_weak), std::bad_weak_ptr);
}

TEST(WeakArcTest, CircularReferenceSolution)
{
    struct Node;
    struct Edge
    {
        arc::weak_arc<Node> target;
    };
    struct Node
    {
        std::vector<Edge> edges;
        ~Node() { /* ? */ }
    };

    arc::arc<Node> n1 = arc::arc<Node>(new Node());
    arc::arc<Node> n2 = arc::arc<Node>(new Node());

    n1->edges.push_back({n2});
    n2->edges.push_back({n1});

    EXPECT_EQ(n1.use_count(), 1);
    EXPECT_EQ(n2.use_count(), 1);
}
