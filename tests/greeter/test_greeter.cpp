#include <gmock/gmock.h>
#include <greeter/Greeter.h>
#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <tuple>

TEST(GreeterTests, test_get_greeting) {
    const std::string expected{"Hello from the modular greeter library"};
    EXPECT_EQ(get_greeting(), expected);
}

// A test fixture for Greeter tests
class GreeterTest : public testing::Test {
   protected:
    Greeter my_greeter;
};

// Use the TEST_F macro to use the fixture
TEST_F(GreeterTest, test_simple_greet) {
    EXPECT_EQ(my_greeter.simple_greet(), "Hello from the Greeter class!");
}

TEST_F(GreeterTest, test_simple_greet_format) {
    my_greeter.setEnthusiasm(0.8f);
    EXPECT_THAT(my_greeter.simple_greet(),
                testing::MatchesRegex("Hello from the Greeter class!*"));
}

TEST_F(GreeterTest, test_get_enthusiam) {
    my_greeter.setEnthusiasm(0.7f);
    EXPECT_NEAR(my_greeter.getEnthusiasm(), 0.7f, 0.001);
}

TEST_F(GreeterTest, test_set_enthusiam_invalid) {
    EXPECT_THROW(my_greeter.setEnthusiasm(1.1f), std::out_of_range);
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

TEST(GreeterWithMockDate, greet_for_monday) {
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
    testing::Values(
        std::make_tuple(1, "Happy Monday! Hello from the Greeter class!"),
        std::make_tuple(2, "Have a nice day! Hello from the Greeter class!"),
        std::make_tuple(5, "Happy Friday! Hello from the Greeter class!")));

class GreeterEnthusiasmTest
    : public testing::TestWithParam<std::tuple<float, std::string> > {
   protected:
    Greeter my_greeter;
};

TEST_P(GreeterEnthusiasmTest, test_greet_exclamations) {
    auto [enthusiasm, expected_greeting]{GetParam()};
    my_greeter.setEnthusiasm(enthusiasm);
    EXPECT_EQ(my_greeter.simple_greet(), expected_greeting);
}

INSTANTIATE_TEST_SUITE_P(
    EnthusiasmLevels, GreeterEnthusiasmTest,
    testing::Values(std::make_tuple(0.0f, "Hello from the Greeter class"),
                    std::make_tuple(0.3f, "Hello from the Greeter class"),
                    std::make_tuple(0.4f, "Hello from the Greeter class!"),
                    std::make_tuple(0.7f, "Hello from the Greeter class!!"),
                    std::make_tuple(1.0f, "Hello from the Greeter class!!!")));


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
