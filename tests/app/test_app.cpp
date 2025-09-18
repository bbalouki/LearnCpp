#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>
#include <string>

#include "run.h"


TEST(GreeterAppTests, RunsWithoutCrashing) {
    // We can now call the app's core logic directly
    EXPECT_EQ(run_app(0, nullptr), 0);
}

class AppLogTest : public testing::Test {
   protected:
    const std::string log_filename{"app_log.txt"};
    void TearDown() override {
        // Clean up the log file after each test
        std::remove(log_filename.c_str());
    }
};

TEST_F(AppLogTest, LogsStartupMessageToFile) {
    run_app(0, nullptr);

    std::ifstream log_file(log_filename);
    ASSERT_TRUE(log_file.is_open());

    std::string line;
    std::getline(log_file, line);

    // Use a matcher to check the format
    EXPECT_THAT(line, testing::HasSubstr("App starting up"));
}
