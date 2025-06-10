#include <arc/arc.hpp>
#include <gtest/gtest.h>

struct Base
{
    virtual ~Base() = default;
    int base_val = 10;
};

struct Derived : public Base
{
    int derived_val = 20;
};

TEST(FactoryTest, MakeArcSimple)
{
    arc::arc<int> p = arc::make_arc<int>(123);
    ASSERT_TRUE(p);
    EXPECT_EQ(*p, 123);
    EXPECT_EQ(p.use_count(), 1);
}

TEST(FactoryTest, MakeArcComplex)
{
    struct Point
    {
        int x, y;
    };

    arc::arc<Point> p = arc::make_arc<Point>(1, 2);
    ASSERT_TRUE(p);
    EXPECT_EQ(p->x, 1);
    EXPECT_EQ(p->y, 2);
}

TEST(PointerCastTest, StaticCast)
{
    arc::arc<Derived> p_derived = arc::make_arc<Derived>();
    arc::arc<Base> p_base = arc::static_pointer_cast<Base>(p_derived);

    EXPECT_EQ(p_base.get(), p_derived.get());
    EXPECT_EQ(p_base.use_count(), 2);
    EXPECT_EQ(p_derived.use_count(), 2);
}

// TODO: Fix It! 
// TEST(PointerCastTest, DynamicCastSuccess)
// {
//     arc::arc<Base> p_base = arc::make_arc<Derived>();
//     arc::arc<Derived> p_derived = arc::dynamic_pointer_cast<Derived>(p_base);

//     ASSERT_TRUE(p_derived);
//     EXPECT_EQ(p_derived.get(), p_base.get());
//     EXPECT_EQ(p_derived.use_count(), 2);
//     EXPECT_EQ(p_base.use_count(), 2);
//     EXPECT_EQ(p_derived->derived_val, 20);
// }

TEST(PointerCastTest, DynamicCastFailure)
{
    arc::arc<Base> p_base =
        arc::make_arc<Base>();
    arc::arc<Derived> p_derived = arc::dynamic_pointer_cast<Derived>(p_base);

    EXPECT_FALSE(p_derived);
    EXPECT_EQ(p_derived.get(), nullptr);
    EXPECT_EQ(p_base.use_count(), 1);
    EXPECT_EQ(p_derived.use_count(), 0);
}

TEST(PointerCastTest, ConstCast)
{
    arc::arc<const int> p_const = arc::make_arc<const int>(50);
    arc::arc<int> p_non_const = arc::const_pointer_cast<int>(p_const);

    ASSERT_TRUE(p_non_const);
    *p_non_const = 100;

    EXPECT_EQ(*p_const, 100);
    EXPECT_EQ(p_const.use_count(), 2);
}