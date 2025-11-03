#include <gtest/gtest.h>

#define TEST_ENV

#include "../src/database.cpp"

// script: (in tests folder)
// first, delete build folder
// mkdir build
// cmake -S . -B build 
// copy lib files to build folder
// cmake --build build
// cmake --build build --target coverage

//gtest_discover_tests(test_file)

class GlobalEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        std::cout << "Global setup before all tests.\n";
        initDatabase();

    }

    void TearDown() override {
        std::cout << "Global teardown after all tests.\n";
        closeDatabaseConnection();
    }
};

class UserTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Per-test setup operations
        userTable->remove().where("username != \"placeholder\"").execute();
    }
};

TEST (SmokeTest, EnsureConnection){
    ASSERT_TRUE(session != nullptr);
    ASSERT_TRUE(userTable.get() != nullptr);
    ASSERT_TRUE(taskTable.get() != nullptr);
}

TEST(SmokeTest, EnsureTestEnvironment){
    ASSERT_TRUE(debug);
}

TEST_F(UserTest, ForValidUserRegistration) {
    EXPECT_EQ(SUCCESS, registerUser("676767", 676767));
}

TEST_F(UserTest, ForDuplicateUserRegistration) {
    registerUser("676767", 676767);
    EXPECT_EQ(DUPLICATE_NAME, registerUser("676767", 676767));
}

TEST_F(UserTest, ForInvalidNameUser){
    EXPECT_EQ(NAME_OVERFLOW, registerUser("3.14159265357asdjfhgahjgsdouidhqwuodgiuashjdghjguqigweuygduygasjhsgdhaghgwqdhghwgdhgshagsdhgashghagsdjhgqwjkshjdgakbfghudjlknbshhiujkw", 0));
}

TEST_F(UserTest, ForGetValidUserByName) {
    User user;
    registerUser("676767", 676767);
    EXPECT_EQ(SUCCESS, getUser("676767", user));
    EXPECT_EQ(user.username, "676767");
    EXPECT_EQ(user.password, 676767);
    EXPECT_EQ(user.points, 0);
}

TEST_F(UserTest, ForGetValidUserByID) {
    User user;
    registerUser("676767", 676767);
    EXPECT_EQ(SUCCESS, getUser("676767", user));
    EXPECT_EQ(SUCCESS, getUser(user.userid, user));
    EXPECT_EQ(user.username, "676767");
    EXPECT_EQ(user.password, 676767);
    EXPECT_EQ(user.points, 0);
}

TEST_F(UserTest, ForGetNonExistUser) {
    User user;
    EXPECT_EQ(DOES_NOT_EXIST, getUser("676767", user));
}

TEST_F(UserTest, ForUpdateValidUser) {
    User user;
    registerUser("676767", 676767);
    EXPECT_EQ(SUCCESS, getUser("676767", user));
    user.password = 767676;
    user.username = "767676";
    user.points = 676767;
    EXPECT_EQ(SUCCESS, updateUserInfo(user));
    EXPECT_EQ(SUCCESS, getUser("767676", user));
    EXPECT_EQ(767676, user.password);
    EXPECT_EQ("767676", user.username);
    EXPECT_EQ(676767, user.points);
}

TEST_F(UserTest, ForUpdateInvalidNameUser){
    User user;
    registerUser("676767", 676767);
    EXPECT_EQ(SUCCESS, getUser("676767", user));
    user.username = "11451419198101145141919810sadj,fkjaghshjakfhdshvdakbgkakbhgknbfvjhksa";
    EXPECT_EQ(NAME_OVERFLOW, updateUserInfo(user));
}

TEST_F(UserTest, ForUpdateNonExistingUser){
    User user;
    registerUser("676767", 676767);
    EXPECT_EQ(SUCCESS, getUser("676767", user));
    user.userid = 676767;
    EXPECT_EQ(DOES_NOT_EXIST, updateUserInfo(user));
}

TEST_F(UserTest, ForLeaderboard){
    registerUser("676767", 676767);
    registerUser("676768", 676767);
    User user;
    getUser("676767", user);
    user.points = 114514;
    updateUserInfo(user);
    std::unordered_map<std::string, unsigned int> points = getLeaderboard();
    EXPECT_EQ(3, points.size());
    EXPECT_EQ(114514, points["676767"]);
    EXPECT_EQ(0, points["676768"]);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new GlobalEnvironment);
    return RUN_ALL_TESTS();
}