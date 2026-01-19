#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>
#include <string>
#include <vector>

#include "run.h"

// Helper function to create valid argv
std::vector<char*> create_args(const std::vector<std::string>& args) {
    std::vector<char*> argv;
    for (const auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    return argv;
}

TEST(GreeterAppTests, RunsWithoutCrashing) {
    std::vector<std::string> args = {"GreeterApp"};
    auto                     argv = create_args(args);

    // Provide valid argc=1 and argv
    EXPECT_EQ(run_app(static_cast<int>(argv.size()), argv.data()), 0);

    // Cleanup so it doesn't pollute other tests
    std::remove("app_log.txt");
}

class AppLogTest : public testing::Test {
   public:
    const std::string log_filename {"app_log.txt"};

   protected:
    void TearDown() override {
        // Clean up the log file after each test
        (void)std::remove(log_filename.c_str());
    }
};

TEST_F(AppLogTest, LogsStartupMessageToFile) {
    std::vector<std::string> args = {"GreeterApp", "--greeting", "Hello Test"};
    auto                     argv = create_args(args);

    run_app(static_cast<int>(argv.size()), argv.data());

    std::ifstream log_file(log_filename);
    ASSERT_TRUE(log_file.is_open());

    std::string line;
    std::getline(log_file, line);

    // Use a matcher to check the format
    EXPECT_THAT(line, testing::HasSubstr("App starting up"));
}
