-- This scheme is used to create the tables required
-- PLEASE DO NOT RUN THIS SCRIPT
-- It will recreat all the tables and therefore clear the data
-- Only use this script if the database is damaged

USE SE101_Team_17

-- Drop Previous Table
DROP TABLE IF EXISTS Todo;

CREATE TABLE Todo(
    taskid int AUTO_INCREMENT PRIMARY KEY,
    userid int NOT NULL,
    item varchar(255) NOT NULL,
    type varchar(255) NOT NULL,
    start datetime NOT NULL,
    due datetime NOT NULL,
    done datetime DEFAULT NULL,
    FOREIGN KEY (userid) REFERENCES User(user_id)
        ON DELETE CASCADE
);
