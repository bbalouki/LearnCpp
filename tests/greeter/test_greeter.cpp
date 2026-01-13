#include <gmock/gmock.h>
#include <greeter/Greeter.h>
#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <tuple>

TEST(GreeterTests, TestGetGreeting) {
    const std::string expected{"Hello from the modular greeter library"};
    EXPECT_EQ(get_greeting(), expected);
}

// A test fixture for Greeter tests
class GreeterTest : public testing::Test {
   protected:
    Greeter my_greeter;
};

// Use the TEST_F macro to use the fixture
TEST_F(GreeterTest, TestSimpleGreet) {
    EXPECT_EQ(my_greeter.simple_greet(), "Hello from the Greeter class!");
}

TEST_F(GreeterTest, TestSimpleGreetFormat) {
  my_greeter.setEnthusiasm(0.8F);  // NOLINT
  EXPECT_THAT(my_greeter.simple_greet(),
              testing::MatchesRegex("Hello from the Greeter class!*"));
}

TEST_F(GreeterTest, TestGetEnthusiam) {
  my_greeter.setEnthusiasm(0.7F);  // NOLINT
  EXPECT_NEAR(my_greeter.getEnthusiasm(), 0.7F, 0.001);  // NOLINT
}

TEST_F(GreeterTest, TestSetEnthusiamInvalid) {
  EXPECT_THROW(my_greeter.setEnthusiasm(1.1F), std::out_of_range);  // NOLINT
}

// Mock implementation of the IDateProvider interface
class MockDateProvider : public IDateProvider {
   public:
    // getDayOfWeek returns int, takes no arguments, and is
    // const.  We can also state that this is an override
    MOCK_METHOD(int, getDayOfWeek, (), (const, override));
};
// A parameterized test fixture
class GreeterDayTest
    : public testing::TestWithParam<std::tuple<int, std::string> > {};

TEST(GreeterWithMockDate, GreetForMonday) {
    auto mock_provider{std::make_unique<MockDateProvider>()};
    EXPECT_CALL(*mock_provider, getDayOfWeek()).WillOnce(testing::Return(1));
    Greeter greeter(std::move(mock_provider));
    EXPECT_EQ(greeter.greet(), "Happy Monday! Hello from the Greeter class!");
    // ...
}
TEST_P(GreeterDayTest, GreetsCorrectlyForDay) {
    auto [day_of_week, expected_greeting] = GetParam();

    auto mock_provider{std::make_unique<MockDateProvider>()};
    EXPECT_CALL(*mock_provider, getDayOfWeek())
        .WillOnce(testing::Return(day_of_week));

    Greeter greeter(std::move(mock_provider));
    EXPECT_EQ(greeter.greet(), expected_greeting);
}

// Instantiate the test suite with our test data
INSTANTIATE_TEST_SUITE_P(
    DayOfWeekGreetings, GreeterDayTest,
    // NOLINTBEGIN
    testing::Values(
        std::make_tuple(1, "Happy Monday! Hello from the Greeter class!"),
        std::make_tuple(2, "Have a nice day! Hello from the Greeter class!"),
        std::make_tuple(5, "Happy Friday! Hello from the Greeter class!")));
    // NOLINTEND
    
class GreeterEnthusiasmTest
    : public testing::TestWithParam<std::tuple<float, std::string> > {
   protected:
    Greeter my_greeter;
};

TEST_P(GreeterEnthusiasmTest, TestGreetExclamations) {
    auto [enthusiasm, expected_greeting]{GetParam()};
    my_greeter.setEnthusiasm(enthusiasm);
    EXPECT_EQ(my_greeter.simple_greet(), expected_greeting);
}

INSTANTIATE_TEST_SUITE_P(
    EnthusiasmLevels, GreeterEnthusiasmTest,
    // NOLINTBEGIN
    testing::Values(std::make_tuple(0.0F, "Hello from the Greeter class"),
                    std::make_tuple(0.3F, "Hello from the Greeter class"),
                    std::make_tuple(0.4F, "Hello from the Greeter class!"),
                    std::make_tuple(0.7F, "Hello from the Greeter class!!"),
                    std::make_tuple(1.0F, "Hello from the Greeter class!!!")));
    // NOLINTEND

class GreeterNameTest
    : public testing::TestWithParam<std::tuple<std::string, std::string> > {};

TEST_P(GreeterNameTest, GreetsCorrectlyForName) {
    auto [name, expected_greeting]{GetParam()};
    Greeter greeter{name.empty() ? Greeter() : Greeter(name)};
    EXPECT_EQ(greeter.greet_name(), expected_greeting);
}

INSTANTIATE_TEST_SUITE_P(
    NamedGreetings, GreeterNameTest,
    testing::Values(std::make_tuple("", "Hello, User!"),
                    std::make_tuple("John", "Hello, John!"),
                    std::make_tuple("Jane", "Hello, Jane!")));
