#include <arc/Arc.hpp>
#include <gtest/gtest.h>

class MockControlBlock : public arc::detail::control_block_base
{
public:
    void dispose() noexcept override {}
    void destroy() noexcept override {}
    void* get_deleter(const std::type_info&) noexcept override
    {
        return nullptr;
    }
};

class ControlBlockTest : public ::testing::Test
{
protected:
    void SetUp() override { cb = new MockControlBlock(); }

    void TearDown() override { delete cb; }

    MockControlBlock* cb;
};

TEST_F(ControlBlockTest, InitialState)
{
    ASSERT_EQ(cb->strong_count.load(), 1);

    ASSERT_EQ(cb->weak_count.load(), 1);
}

TEST_F(ControlBlockTest, Alignment)
{
    void* ptr = static_cast<void*>(cb);
    std::size_t address = reinterpret_cast<std::size_t>(ptr);

    ASSERT_EQ(address % arc::detail::cache_line_size, 0);
}
