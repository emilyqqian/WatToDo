-- This scheme is used to create the tables required
-- PLEASE DO NOT RUN THIS SCRIPT
-- It will recreat all the tables and therefore clear the data
-- Only use this script if the database is damaged

USE SE101_Team_17

-- Drop Previous Table
DROP INDEX IF EXISTS idx_task_assigned_user ON Task(assigned_user_id);
DROP TABLE IF EXISTS User;
DROP TABLE IF EXISTS TaskBoard;
DROP TABLE IF EXISTS Task;
DROP TABLE IF EXISTS TaskBoardUser;

-- USER TABLE
CREATE TABLE User (
    user_id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) NOT NULL UNIQUE,
    password_hash INT NOT NULL,
	xp INT NOT NULL DEFAULT 0
);

-- TASKBOARD TABLE
CREATE TABLE TaskBoard (
    board_id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL
);

-- TASK TABLE (now includes assigned_user_id)
CREATE TABLE Task (
    task_id INT AUTO_INCREMENT PRIMARY KEY,
    board_id INT NOT NULL,
    assigned_user_id INT DEFAULT NULL,
    title VARCHAR(100) NOT NULL,
	type VARCHAR(100) NOT NULL,
    due_date DATE NOT NULL,
	start_date DATE NOT NULL,
	finished_date DATE DEFAULT NULL,
    FOREIGN KEY (board_id) REFERENCES TaskBoard(board_id)
        ON DELETE CASCADE,
    FOREIGN KEY (assigned_user_id) REFERENCES User(user_id)
        ON DELETE SET NULL
);

-- MANY-TO-MANY RELATIONSHIP BETWEEN USER AND TASKBOARD
CREATE TABLE TaskBoardUser (
    board_id INT NOT NULL,
    user_id INT NOT NULL,
    isAdmin BOOL NOT NULL DEFAULT FALSE,
    PRIMARY KEY (board_id, user_id),
    FOREIGN KEY (board_id) REFERENCES TaskBoard(board_id)
        ON DELETE CASCADE,
    FOREIGN KEY (user_id) REFERENCES User(user_id)
        ON DELETE CASCADE
);

-- INDEX to improve queries filtering by assigned_user_id
CREATE INDEX idx_task_assigned_user ON Task(assigned_user_id);
