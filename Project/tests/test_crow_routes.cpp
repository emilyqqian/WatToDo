#include <gtest/gtest.h>
#include <crow.h>
#include "../src/database.h"

class CrowRoutesTest : public ::testing::Test {
protected:
    void SetUp() override {
        initDatabase();
    }

    void TearDown() override {
        closeDatabaseConnection();
    }
};

// Test that verifies the Crow route handlers work with database
TEST_F(CrowRoutesTest, RegisterRouteIntegration) {
    // Test the exact same logic your /register route uses
    std::string username = "test_user";
    unsigned int password = 123;
    
    DatabaseResult result = registerUser(username, password);
    EXPECT_EQ(SUCCESS, result);
    
    // Verify the user was actually created
    User user;
    DatabaseResult get_result = getUser(username, user);
    EXPECT_EQ(SUCCESS, get_result);
    EXPECT_EQ(username, user.username);
}

TEST_F(CrowRoutesTest, GetUserByUsernameRouteIntegration) {
    // Setup: Create a user first
    registerUser("test_user", 123);
    
    // Test the exact same logic your /users/<string> route uses
    std::string username = "test_user";
    User user;
    DatabaseResult result = getUser(username, user);
    
    EXPECT_EQ(SUCCESS, result);
    EXPECT_EQ("test_user", user.username);
    EXPECT_GT(user.userid, 0); // Should have a valid ID
}

TEST_F(CrowRoutesTest, GetUserByIdRouteIntegration) {
    // Setup: Create a user and get their ID
    registerUser("test_user", 123);
    User user_by_name;
    getUser("test_user", user_by_name);
    
    // Test the exact same logic your /users/id/<int> route uses
    User user_by_id;
    DatabaseResult result = getUser(user_by_name.userid, user_by_id);
    
    EXPECT_EQ(SUCCESS, result);
    EXPECT_EQ("test_user", user_by_id.username);
    EXPECT_EQ(user_by_name.userid, user_by_id.userid);
}

TEST_F(CrowRoutesTest, LeaderboardRouteIntegration) {
    // Setup: Create multiple users
    registerUser("user1", 123);
    registerUser("user2", 456);
    
    // Test the exact same logic your /leaderboard route uses
    auto leaderboard = getLeaderboard();
    
    EXPECT_GE(leaderboard.size(), 2);
    EXPECT_TRUE(leaderboard.find("user1") != leaderboard.end());
    EXPECT_TRUE(leaderboard.find("user2") != leaderboard.end());
}