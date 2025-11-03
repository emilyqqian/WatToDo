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
        userTable->remove().where("username != \"placeholder\"").execute();
    }
};

class TaskTest : public ::testing::Test {
protected:
    void SetUp() override {
        taskTable->remove().execute();
    }
};

class TaskUserInteg : public ::testing::Test{
protected:
    void SetUp() override {
        userTable->remove().where("username != \"placeholder\"").execute();
        taskTable->remove().execute();
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

TEST_F(TaskTest, ForAddingValidTask){
    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(SUCCESS, addTask(1, task));
}

TEST_F(TaskTest, ForAddingInvalidTaskName){
    Task task(1, "asdfgaskdjhfgahjhgduwyqegfdguyefgqywgewuygydgsyaugduygwyuguwefqfasdghasdfuhasdfhasfsdiufsdaufhusadhfusadhfuiahsdiufhuaishdfuihasduifhsdauifhsaudihfuisadhfusadfdsafas", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(NAME_OVERFLOW, addTask(1, task));
    task = Task(1, "test", "asdgfhjgasdgfjhasgdhfgahjsdghjfasghjfdghjasgfhjsagfhsdgfhjasdahsdfhagshdgfuyegudysgyugeywugfyugduysgauyfgewuyfgyudgyauguyfeguyfguydaguyewguyfgaysudgfuyewgfuyg", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(NAME_OVERFLOW, addTask(1, task));
}

TEST_F(TaskTest, ForAddingTaskToNonExisitingTaskBoard){
    Task t = Task();
    EXPECT_EQ(DOES_NOT_EXIST, addTask(114514, t));
}

TEST_F(TaskTest, ForAddingTaskWithIncorrectDates){
    Task t(1, "test", "test", date(2025, 10, 15), date(2015, 10, 15));
    EXPECT_EQ(TIME_CONFLICT, addTask(1, t));
    t = Task(1, "test", "test", date(2005, 10, 15), date(2015, 10, 15));
    EXPECT_EQ(TIME_CONFLICT, addTask(1, t));
}

TEST_F(TaskTest, ForAddingRepetitiveTasks){
    Task t(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(SUCCESS, addTask(1, t));
    t = Task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(ALREADY_EXIST, addTask(1, t));
}

TEST_F(TaskTest, ForDeletingValidTask){
    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(SUCCESS, addTask(1, task));
    EXPECT_EQ(SUCCESS, deleteTask(task.taskID, 1));
}

TEST_F(TaskTest, ForDeletingNonExistingTask){
    EXPECT_EQ(DOES_NOT_EXIST, deleteTask(114514, 1));
}

TEST_F(TaskTest, ForGettingValidTask){
    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    Task original = task;
    EXPECT_EQ(SUCCESS, addTask(1, task));
    EXPECT_EQ(SUCCESS, getTask(task.taskID, task));
    EXPECT_EQ("test", task.title);
    EXPECT_EQ("test", task.type);
    EXPECT_EQ(original.startDate, task.startDate);
    EXPECT_EQ(original.duedate, task.duedate);
}

TEST_F(TaskTest, ForGettingNonExistingTask){
    Task t = Task();
    EXPECT_EQ(DOES_NOT_EXIST, getTask(114514, t));
}

TEST_F(TaskTest, ForUpdatingValidTask){
    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(SUCCESS, addTask(1, task));
    task.title = "debug";
    EXPECT_EQ(SUCCESS, updateTask(task, 1));
    EXPECT_EQ(SUCCESS, getTask(task.taskID, task));
    EXPECT_EQ(task.title, "debug");
}

TEST_F(TaskTest, ForUpdatingNonExistingTask){
    Task task(1, "test", "test", date(), date());
    task.taskID = 114514;
    EXPECT_EQ(DOES_NOT_EXIST, updateTask(task, 1));
}

TEST_F(TaskTest, ForUpdaingTaskWithInvalidTaskBoard){
    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(SUCCESS, addTask(1, task));
    task.boardID = 114514;
    EXPECT_EQ(DOES_NOT_EXIST, updateTask(task, 1));
}

TEST_F(TaskTest, ForUpdatingConflictedTask){
    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(SUCCESS, addTask(1, task));
    task.startDate = date(2045, 10, 15);
    EXPECT_EQ(TIME_CONFLICT, updateTask(task, 1));
    task.startDate = date(2025, 10, 15);
    task.duedate = date(2015, 10, 15);
    EXPECT_EQ(TIME_CONFLICT, updateTask(task, 1));
    task.duedate = date(2035, 10, 15);
    task.finished = true;
    task.finishedOn = date(2005, 10, 15);
    EXPECT_EQ(TIME_CONFLICT, updateTask(task, 1));
}

TEST_F(TaskTest, ForUpdatingTaskWithInvalidName){
    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(SUCCESS, addTask(1, task));
    task.title = "askdfjgsdfhjghsjdgfjhgawjhefgjhdsgafuyagweuydgsajhfgeuywfgduysagfyuewgfyudsgfsdfasdfkhdjfhsajkhdfiashdfiuhasdiufhaisuhdfuiasdhfuihasiudhfuiasdhfiuasdhfuisadhfiusdfhisadf";
    EXPECT_EQ(NAME_OVERFLOW, updateTask(task, 1));
    task.title = "test";
    task.type = "asdhfghjgfewqgdsjhgkafuwjhdgxskuygfjhgdvsuayjfgdvsuahjgfuadsgfufewfewfdasfasdfkjashjkdhjkfhjkhjkhajkshdjkfhkjhsakjdhfjkshadjfkhasdjkfhsjakdfhsjkadfhsjakfhjkasdsadf";
    EXPECT_EQ(NAME_OVERFLOW, updateTask(task, 1));
}

TEST_F(TaskUserInteg, ForGettingAssignedUser){
    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(SUCCESS, addTask(1, task));
    task.assigned = true;
    task.assignedUser = User(125, "placeholder", 0, 0);
    EXPECT_EQ(SUCCESS, updateTask(task, 1));
    Task t2;
    EXPECT_EQ(SUCCESS, getTask(task.taskID, t2));
    EXPECT_EQ("placeholder", t2.assignedUser.username);
}

TEST_F(TaskUserInteg, ForAssigningInvalidUser){
    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(SUCCESS, addTask(1, task));
    task.assigned = true;
    task.assignedUser = User(114514, "placeholder", 0, 0);
    EXPECT_EQ(DOES_NOT_EXIST, updateTask(task, 1));
}

TEST_F(TaskUserInteg, ForGettingTaskByAssignedUser){
    std::vector<Task> tasks;
    EXPECT_EQ(SUCCESS, getAssignedTaskForUser(125, tasks));
    EXPECT_EQ(0, tasks.size());

    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(SUCCESS, addTask(1, task));
    task.assigned = true;
    task.assignedUser = User(125, "placeholder", 0, 0);
    EXPECT_EQ(SUCCESS, updateTask(task, 1));

    EXPECT_EQ(SUCCESS, getAssignedTaskForUser(125, tasks));
    EXPECT_EQ(1, tasks.size());
    EXPECT_EQ("test", tasks[0].title);
}

TEST_F(TaskUserInteg, ForGettingTaskFromNonExistingUser){
    std::vector<Task> v = std::vector<Task>();
    EXPECT_EQ(DOES_NOT_EXIST, getAssignedTaskForUser(0, v));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new GlobalEnvironment);
    return RUN_ALL_TESTS();
}