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
        userTable->remove().where("username != \"placeholder\" and username != \"test_user\"").execute();
    }
};

class TaskTest : public ::testing::Test {
protected:
    void SetUp() override {
        taskTable->remove().execute();
    }
};

class TaskBoardTest : public ::testing::Test {
protected:
    void SetUp() override {
        requireInvitation = false;
        taskboardTable->remove().where("board_id != 1").execute();
    }
};

class TaskUserInteg : public ::testing::Test{
protected:
    void SetUp() override {
        userTable->remove().where("username != \"placeholder\" and username != \"test_user\"").execute();
        taskTable->remove().execute();
    }
};

class TaskUserBoardInteg : public ::testing::Test{
    protected:
    void SetUp() override {
        requireInvitation = true;
        userTable->remove().where("username != \"placeholder\" and username != \"test_user\"").execute();
        taskboardTable->remove().where("board_id != 1").execute();
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
    EXPECT_EQ(4, points.size());
    EXPECT_EQ(114514, points["676767"]);
    EXPECT_EQ(0, points["676768"]);
}

TEST_F(TaskTest, ForAddingValidTask){
    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(SUCCESS, addTask(1, task, 125));
}

TEST_F(TaskTest, ForAddingInvalidTaskName){
    Task task(1, "asdfgaskdjhfgahjhgduwyqegfdguyefgqywgewuygydgsyaugduygwyuguwefqfasdghasdfuhasdfhasfsdiufsdaufhusadhfusadhfuiahsdiufhuaishdfuihasduifhsdauifhsaudihfuisadhfusadfdsafas", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(NAME_OVERFLOW, addTask(1, task, 125));
    task = Task(1, "test", "asdgfhjgasdgfjhasgdhfgahjsdghjfasghjfdghjasgfhjsagfhsdgfhjasdahsdfhagshdgfuyegudysgyugeywugfyugduysgauyfgewuyfgyudgyauguyfeguyfguydaguyewguyfgaysudgfuyewgfuyg", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(NAME_OVERFLOW, addTask(1, task, 125));
}

TEST_F(TaskTest, ForAddingTaskToNonExisitingTaskBoard){
    Task t = Task();
    EXPECT_EQ(DOES_NOT_EXIST, addTask(114514, t, 125));
}

TEST_F(TaskTest, ForAddingTaskWithIncorrectDates){
    Task t(1, "test", "test", date(2025, 10, 15), date(2015, 10, 15));
    EXPECT_EQ(TIME_CONFLICT, addTask(1, t, 125));
    t = Task(1, "test", "test", date(2005, 10, 15), date(2015, 10, 15));
    EXPECT_EQ(TIME_CONFLICT, addTask(1, t, 125));
}

TEST_F(TaskTest, ForAddingRepetitiveTasks){
    Task t(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(SUCCESS, addTask(1, t, 125));
    t = Task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(ALREADY_EXIST, addTask(1, t, 125));
}

TEST_F(TaskTest, ForDeletingValidTask){
    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(SUCCESS, addTask(1, task, 125));
    EXPECT_EQ(SUCCESS, deleteTask(task.taskID, 125));
}

TEST_F(TaskTest, ForDeletingNonExistingTask){
    EXPECT_EQ(DOES_NOT_EXIST, deleteTask(114514, 125));
}

TEST_F(TaskTest, ForGettingValidTask){
    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    Task original = task;
    EXPECT_EQ(SUCCESS, addTask(1, task, 125));
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
    EXPECT_EQ(SUCCESS, addTask(1, task, 125));
    task.title = "debug";
    EXPECT_EQ(SUCCESS, updateTask(task, 125));
    EXPECT_EQ(SUCCESS, getTask(task.taskID, task));
    EXPECT_EQ(task.title, "debug");
}

TEST_F(TaskTest, ForUpdatingNonExistingTask){
    Task task(1, "test", "test", date(), date());
    task.taskID = 114514;
    EXPECT_EQ(DOES_NOT_EXIST, updateTask(task, 125));
}

TEST_F(TaskTest, ForUpdaingTaskWithInvalidTaskBoard){
    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(SUCCESS, addTask(1, task, 125));
    task.boardID = 114514;
    EXPECT_EQ(DOES_NOT_EXIST, updateTask(task, 125));
}

TEST_F(TaskTest, ForUpdatingConflictedTask){
    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(SUCCESS, addTask(1, task, 125));
    task.startDate = date(2045, 10, 15);
    EXPECT_EQ(TIME_CONFLICT, updateTask(task, 125));
    task.startDate = date(2025, 10, 15);
    task.duedate = date(2015, 10, 15);
    EXPECT_EQ(TIME_CONFLICT, updateTask(task, 125));
    task.duedate = date(2035, 10, 15);
    task.finished = true;
    task.finishedOn = date(2005, 10, 15);
    EXPECT_EQ(TIME_CONFLICT, updateTask(task, 125));
}

TEST_F(TaskTest, ForUpdatingTaskWithInvalidName){
    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(SUCCESS, addTask(1, task, 125));
    task.title = "askdfjgsdfhjghsjdgfjhgawjhefgjhdsgafuyagweuydgsajhfgeuywfgduysagfyuewgfyudsgfsdfasdfkhdjfhsajkhdfiashdfiuhasdiufhaisuhdfuiasdhfuihasiudhfuiasdhfiuasdhfuisadhfiusdfhisadf";
    EXPECT_EQ(NAME_OVERFLOW, updateTask(task, 125));
    task.title = "test";
    task.type = "asdhfghjgfewqgdsjhgkafuwjhdgxskuygfjhgdvsuayjfgdvsuahjgfuadsgfufewfewfdasfasdfkjashjkdhjkfhjkhjkhajkshdjkfhkjhsakjdhfjkshadjfkhasdjkfhsjakdfhsjkadfhsjakfhjkasdsadf";
    EXPECT_EQ(NAME_OVERFLOW, updateTask(task, 125));
}

TEST_F(TaskUserInteg, ForGettingAssignedUser){
    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(SUCCESS, addTask(1, task, 125));
    task.assigned = true;
    task.assignedUser = User(125, "placeholder", 0, 0);
    EXPECT_EQ(SUCCESS, updateTask(task, 125));
    Task t2;
    EXPECT_EQ(SUCCESS, getTask(task.taskID, t2));
    EXPECT_EQ("placeholder", t2.assignedUser.username);
}

TEST_F(TaskUserInteg, ForAssigningInvalidUser){
    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(SUCCESS, addTask(1, task, 125));
    task.assigned = true;
    task.assignedUser = User(114514, "placeholder", 0, 0);
    EXPECT_EQ(DOES_NOT_EXIST, updateTask(task, 125));
}

TEST_F(TaskUserInteg, ForGettingTaskByAssignedUser){
    std::vector<Task> tasks;
    EXPECT_EQ(SUCCESS, getAssignedTaskForUser(125, tasks));
    EXPECT_EQ(0, tasks.size());

    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(SUCCESS, addTask(1, task, 125));
    task.assigned = true;
    task.assignedUser = User(125, "placeholder", 0, 0);
    EXPECT_EQ(SUCCESS, updateTask(task, 125));

    EXPECT_EQ(SUCCESS, getAssignedTaskForUser(125, tasks));
    EXPECT_EQ(1, tasks.size());
    EXPECT_EQ("test", tasks[0].title);
}

TEST_F(TaskUserInteg, ForGettingTaskFromNonExistingUser){
    std::vector<Task> v = std::vector<Task>();
    EXPECT_EQ(DOES_NOT_EXIST, getAssignedTaskForUser(0, v));
}

TEST_F(TaskUserInteg, ForAddingTaskWithoutPermission){
    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(USER_ACCESS_DENINED, addTask(1, task, 216));
}

TEST_F(TaskUserInteg, ForDeletingTaskWithoutPermission){
    Task task(1, "test", "test", date(2025, 10, 15), date(2035, 10, 15));
    EXPECT_EQ(SUCCESS, addTask(1, task, 125));
    EXPECT_EQ(USER_ACCESS_DENINED, deleteTask(task.taskID, 216));
}

TEST_F(TaskBoardTest, ForCreatingTaskBoard){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ("new board", b.name);
    EXPECT_EQ(1, b.admins.size());
}

TEST_F(TaskBoardTest, ForCreatingTaskBoardWithInvalidName){
    TaskBoard b;
    EXPECT_EQ(NAME_OVERFLOW, createTaskBoard(125, "ajshdgfkjahsgdfhjgasjhdgfhjsdgfhjgsdhjfghsdgafhsdgjhfgsdjhfgsdhjfghsdjagfhjsdgfhjsdgfhjsdgfhdsgfhjdsgfhjsdagfhjdsgfhjgashjghjdghjfgshjgfhsdgfhjsdagfjhgsdahjfgdshjfgadsj", b));
}

TEST_F(TaskBoardTest, ForCreatingTaskBoardWithInvalidOwner){
    TaskBoard b;
    EXPECT_EQ(DOES_NOT_EXIST, createTaskBoard(114514, "", b));
}

TEST_F(TaskBoardTest, ForGettingTaskBoardByID){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, getTaskBoardByID(1, b));
    EXPECT_EQ(1, b.taskboard_id);
}

TEST_F(TaskBoardTest, ForGettingTaskBoardByInvalidID){
    TaskBoard b;
    EXPECT_EQ(DOES_NOT_EXIST, getTaskBoardByID(114514, b));
}

TEST_F(TaskBoardTest, ForGettingTaskBoardByUser){
    std::vector<TaskBoard> returnedTaskList = std::vector<TaskBoard>();
    EXPECT_EQ(SUCCESS, getTaskBoardByUser(125, returnedTaskList));
    EXPECT_EQ(1, returnedTaskList.size());
    EXPECT_EQ(1, returnedTaskList[0].taskboard_id);
}

TEST_F(TaskBoardTest, ForRenamingTaskBoard){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(SUCCESS, renameTaskBoard(b.taskboard_id, "old board", 125));
    EXPECT_EQ(SUCCESS, getTaskBoardByID(b.taskboard_id, b));
    EXPECT_EQ("old board", b.name);
}

TEST_F(TaskBoardTest, ForRenamingTaskBoardWithInvalidName){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(NAME_OVERFLOW, renameTaskBoard(b.taskboard_id, "asdfhjgajshdgfhjasgdfhgsahjdfghsajdgfhjsdagfhjasdgfhjsdagfhjasdgfjhasdgjfhgasdjhfgadshjfgasdhjgfhjadsgfhjasdgfjhgasdjhgfjhsdagjhjgh", 125));
}

TEST_F(TaskUserBoardInteg, ForRenamingTaskboardWithoutpermission){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(SUCCESS, renameTaskBoard(b.taskboard_id, "old board", 125));
}

TEST_F(TaskBoardTest, ForUpdateUserStatus){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(SUCCESS, addUserToTaskboard(216, b.taskboard_id, 125));
    EXPECT_EQ(SUCCESS, updateUserStatus(216, b.taskboard_id, true, 125));
    EXPECT_EQ(SUCCESS, getTaskBoardByID(b.taskboard_id, b));
    EXPECT_EQ(2, b.admins.size());
}

TEST_F(TaskBoardTest, ForUpdateUserStatusWithNonExistingUser){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(DOES_NOT_EXIST, updateUserStatus(100, b.taskboard_id, true, 125));
}

TEST_F(TaskBoardTest, ForUpdateUserStatusWithNonExistingBoard){
    EXPECT_EQ(DOES_NOT_EXIST, updateUserStatus(100, 0, true, 125));
}

TEST_F(TaskUserBoardInteg, ForUpdateUserStatusWithoutpermission){
    TaskBoard b;
    requireInvitation = false;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(SUCCESS, addUserToTaskboard(216, b.taskboard_id, 125));
    EXPECT_EQ(USER_ACCESS_DENINED, updateUserStatus(125, b.taskboard_id, false, 216));
    EXPECT_EQ(SUCCESS, getTaskBoardByID(b.taskboard_id, b));
    EXPECT_EQ(1, b.admins.size());
}

TEST_F(TaskUserBoardInteg, ForUpdateUserStatusCausingHangingBoard){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(USER_CONFLICT, updateUserStatus(125, b.taskboard_id, false, 125));
}

TEST_F(TaskBoardTest, ForAddingUser){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(SUCCESS, addUserToTaskboard(216, b.taskboard_id, 125));
    EXPECT_EQ(SUCCESS, getTaskBoardByID(b.taskboard_id, b));
    EXPECT_EQ(2, b.users.size());
}

TEST_F(TaskBoardTest, ForAddinguSERWithNonExistingUser){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(DOES_NOT_EXIST, addUserToTaskboard(0, b.taskboard_id, 125));
}

TEST_F(TaskBoardTest, ForAddingUserWithNonExistingBoard){
    EXPECT_EQ(DOES_NOT_EXIST, addUserToTaskboard(0, 0, 125));
}

TEST_F(TaskUserBoardInteg, ForAddingUserWithoutpermission){
    TaskBoard b;
    requireInvitation = false;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(SUCCESS, addUserToTaskboard(216, b.taskboard_id, 125));
    EXPECT_EQ(USER_ACCESS_DENINED, addUserToTaskboard(125, b.taskboard_id, 216));
}

TEST_F(TaskUserBoardInteg, ForAddingUserWithoutInvitation){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(USER_ACCESS_DENINED, addUserToTaskboard(216, b.taskboard_id, 125));
}

TEST_F(TaskBoardTest, ForAddingExistingUser){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(ALREADY_EXIST, addUserToTaskboard(125, b.taskboard_id, 125));
}

TEST_F(TaskBoardTest, ForKickingOutUser){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(SUCCESS, addUserToTaskboard(216, b.taskboard_id, 125));
    EXPECT_EQ(SUCCESS, kickOutUserFromTaskboard(216, b.taskboard_id, 125));
    EXPECT_EQ(SUCCESS, getTaskBoardByID(b.taskboard_id, b));
    EXPECT_EQ(1, b.users.size());
}

TEST_F(TaskBoardTest, ForKickingOutNonExistingUser){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(DOES_NOT_EXIST, kickOutUserFromTaskboard(216, b.taskboard_id, 125));
}

TEST_F(TaskBoardTest, ForKickingOutUserWithNonExistingBoard){
    EXPECT_EQ(DOES_NOT_EXIST, kickOutUserFromTaskboard(216, 0, 125));
}

TEST_F(TaskUserBoardInteg, ForKickingOutUserWithoutpermission){
    TaskBoard b;
    requireInvitation = false;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(SUCCESS, addUserToTaskboard(216, b.taskboard_id, 125));
    EXPECT_EQ(USER_ACCESS_DENINED, kickOutUserFromTaskboard(125, b.taskboard_id, 216));     
}

TEST_F(TaskUserBoardInteg, ForDeletingUserStatusCausingHangingBoard){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(USER_CONFLICT, kickOutUserFromTaskboard(125, b.taskboard_id, 125));
}

TEST_F(TaskBoardTest, ForDeletingBoard){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(SUCCESS, deleteTaskBoard(b.taskboard_id, 125));
}

TEST_F(TaskBoardTest, ForDeletingBoardWithNonExistingUser){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(USER_ACCESS_DENINED, deleteTaskBoard(b.taskboard_id, 0));
}

TEST_F(TaskBoardTest, ForDeletingBoardWithNonExistingBoard){
    EXPECT_EQ(DOES_NOT_EXIST, deleteTaskBoard(0, 0));
}

TEST_F(TaskUserBoardInteg, ForDeletingBoardWithoutpermission){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(USER_ACCESS_DENINED, deleteTaskBoard(b.taskboard_id, 0));
}

TEST_F(TaskUserBoardInteg, ForInviting){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(SUCCESS, inviteUser(125, 216, b.taskboard_id));
    std::unordered_multimap<User, TaskBoard, UserHasher> returnedInvitations;
    EXPECT_EQ(SUCCESS, getAllInvitation(216, returnedInvitations));
    EXPECT_EQ(1, returnedInvitations.size());
}

TEST_F(TaskUserBoardInteg, ForInvitingNonExistingUser){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(DOES_NOT_EXIST, inviteUser(125, 0, b.taskboard_id));
}

TEST_F(TaskUserBoardInteg, ForInvitingUserToNonExistingBoard){
    EXPECT_EQ(DOES_NOT_EXIST, inviteUser(125, 216, 0));
}

TEST_F(TaskUserBoardInteg, ForInvitingUserWithoutpermission){
    requireInvitation = false;
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(SUCCESS, addUserToTaskboard(216, b.taskboard_id, 125));
    EXPECT_EQ(USER_ACCESS_DENINED, inviteUser(216, 125, b.taskboard_id));
}

TEST_F(TaskUserBoardInteg, ForInvitingExistingUser){
    TaskBoard b;
    EXPECT_EQ(SUCCESS, createTaskBoard(125, "new board", b));
    EXPECT_EQ(ALREADY_EXIST, inviteUser(125, 125, b.taskboard_id));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new GlobalEnvironment);
    return RUN_ALL_TESTS();
}