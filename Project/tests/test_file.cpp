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

class GlobalEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        std::cout << "Global setup before all tests.\n";
        initDatabase();
        userTable->remove().execute();
    }

    void TearDown() override {
        std::cout << "Global teardown after all tests.\n";
        closeDatabaseConnection();
    }
};

TEST(UserTest, ForValidUserRegistration) {
    EXPECT_EQ(SUCCESS, registerUser("676767", 676767));
}

TEST(UserTest, ForDuplicateUserRegistration) {
    registerUser("676767", 676767);
    EXPECT_EQ(DUPLICATE_NAME, registerUser("676767", 676767));
}

TEST(UserTest, ForInvalidNameUser){
    EXPECT_EQ(NAME_OVERFLOW, registerUser("3.14159265357asdjfhgahjgsdouidhqwuodgiuashjdghjguqigweuygduygasjhsgdhaghgwqdhghwgdhgshagsdhgashghagsdjhgqwjkshjdgakbfghudjlknbshhiujkw", 0));
}

TEST(UserTest, ForGetValidUser) {
    User *user;
    registerUser("676767", 676767);
    EXPECT_EQ(SUCCESS, getUser("676767", &user));
    EXPECT_EQ(user->username, "676767");
    EXPECT_EQ(user->password, 676767);
    EXPECT_EQ(user->points, 0);
}

TEST(UserTest, ForGetNonExistUser) {
    User *user;
    EXPECT_EQ(DOES_NOT_EXIST, getUser("676767", &user));
}

TEST(UserTest, ForUpdateValidUser) {
    User *user;
    registerUser("676767", 676767);
    EXPECT_EQ(SUCCESS, getUser("676767", &user));
    user->password = 767676;
    user->username = "767676";
    user->points = 676767;
    EXPECT_EQ(SUCCESS, updateUserInfo(*user));
    EXPECT_EQ(SUCCESS, getUser("767676", &user));
    EXPECT_EQ(767676, user->password);
    EXPECT_EQ("767676", user->username);
    EXPECT_EQ(676767, user->points);
}

TEST(UserTest, ForUpdateInvalidNameUser){
    User *user;
    registerUser("676767", 676767);
    EXPECT_EQ(SUCCESS, getUser("676767", &user));
    user->username = "11451419198101145141919810sadj,fkjaghshjakfhdshvdakbgkakbhgknbfvjhksa";
    EXPECT_EQ(NAME_OVERFLOW, updateUserInfo(*user));
}

TEST(UserTest, ForUpdateNonExistingUser){
    User *user;
    registerUser("676767", 676767);
    EXPECT_EQ(SUCCESS, getUser("676767", &user));
    user->userid = 676767;
    EXPECT_EQ(DOES_NOT_EXIST, updateUserInfo(*user));
}

TEST(UserTest, ForLeaderboard){
    registerUser("676767", 676767);
    registerUser("676768", 676767);
    User *user;
    getUser("676767", &user);
    user->points = 114514;
    updateUserInfo(*user);
    std::unordered_map<std::string, unsigned int> points = getLeaderboard();
    EXPECT_EQ(2, points.size());
    EXPECT_EQ(114514, points["676767"]);
    EXPECT_EQ(0, points["676768"]);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new GlobalEnvironment);
    return RUN_ALL_TESTS();
}