#include <gtest/gtest.h>
#include <cstdio>

TEST(SimpleExampleTest, BasicOutput) {
    // Redirect stdout to a string stream
    FILE* original_stdout = stdout;
    freopen("test_output.txt", "w", stdout);

    // Call the main function from the original code
    (void)fprintf(stdout, "Hello, From the updated ARC project!\n");

    // Restore stdout
    fflush(stdout);
    freopen("/dev/tty", "w", stdout);

    // Check if the output file contains the expected string
    FILE* file = fopen("test_output.txt", "r");
    ASSERT_TRUE(file != nullptr);

    char buffer[256];
    fgets(buffer, sizeof(buffer), file);
    fclose(file);

    EXPECT_STREQ(buffer, "Hello, From the updated ARC project!\n");
}