#include <arc.hpp>
#include <gtest/gtest.h>

namespace
{
    struct CustomDeleter {
        void operator()(int* ptr) const { delete ptr; }
    };

} // namespace

TEST(ArcMakeTest, BasicObjectCreation)
{
    Arc::Arc<int> arc = Arc::Arc<int>::make_arc(167);
    EXPECT_EQ(*arc, 167);
    EXPECT_EQ(arc.use_count(), 1);
    EXPECT_TRUE(arc.unique());
}

TEST(ArcMakeTest, CustomDeleter)
{
    Arc::Arc<int, CustomDeleter> arc = Arc::Arc<int, CustomDeleter>::make_arc(167);
    EXPECT_EQ(*arc, 167);
    EXPECT_EQ(arc.use_count(), 1);
    EXPECT_TRUE(arc.unique());
    CustomDeleter newDeleter;
    arc.set_deleter(newDeleter);
}
