#include <string>
#include <set>
#include <ctime>

#ifndef _DATABASE_CLASSES_H
#define _DATABASE_CLASSES_H

class date {
public:

    int year;
    int month;
    int day;

    date(std::string src) {
        // expected format: YYYY-MM-DD
        year = std::stoi(src.substr(0, 4));
        month = std::stoi(src.substr(5, 2));
        day = std::stoi(src.substr(8, 2));
    }

    date() {
        // initialize to current date
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        year = now->tm_year + 1900;
        month = now->tm_mon + 1;
        day = now->tm_mday;
    }

    int operator <(const date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }

    bool operator ==(const date& other) const {
        return year == other.year && month == other.month && day == other.day;
    }

    bool operator >(const date& other) const {
        return other < *this;
    }

    date& operator=(const date& other) {
        day = other.day;
        month = other.month;
        year = other.year;
        return *this;
    }

    std::string toString() const {
        return std::to_string(year) + "-" + (month < 10 ? "0" : "") + std::to_string(month) + "-" + (day < 10 ? "0" : "") + std::to_string(day);
    }
};

class User {
public:

    unsigned int userid = 0;
    std::string username;// username has to be unique
    unsigned int password = 0;// hash of password
    unsigned int points = 0;// points the user has earned

    // used internally
    User(unsigned int userid, std::string username, unsigned int password, unsigned int xpPoint) {
        this->username = username;
        this->password = password;
        this->userid = userid;
        this->points = xpPoint;
    }

    // default garbage-value constructor
    User() {};

	User &operator=(const User& other) {
		userid = other.userid;
		username = other.username;
		password = other.password;
		points = other.points;
		return *this;
	}
};

class Task {
public:
    unsigned int taskID;
    unsigned int boardID;
    std::string title;
    std::string type;
    bool pinned = false;
    date startDate;
    date duedate;
    date finishedOn;
    bool finished = false;
    bool assigned = false;
    User assignedUser;

    // used to create a new task, only necessary informations are included
    Task(unsigned int boardID, std::string title, std::string type, date startDate, date duedate) {
        this->title = title;
        this->type = type;
        this->startDate = startDate;
        this->duedate = duedate;
        // initialized but garbage value
        this->assignedUser = User();
        this->boardID = boardID;
    }

    // used to recreate a task form database/frontend
    Task(std::string title, std::string type, unsigned int boardID, unsigned int taskID, bool pinned, date startDate, date duedate, date finished, bool isFinished, bool isAssigned, User assignedUser) {
        this->title = title;
        this->type = type;
        this->taskID = taskID;
        this->pinned = pinned;
        this->startDate = startDate;
        this->duedate = duedate;
        this->finished = isFinished;
        this->finishedOn = finished;
        this->assigned = isAssigned;
        this->assignedUser = assignedUser;
        this->boardID = boardID;
    }

	Task& operator=(const Task& other) {
		this->title = other.title;
		this->type = other.type;
		this->taskID = other.taskID;
		this->pinned = other.pinned;
		this->startDate = other.startDate;
		this->duedate = other.duedate;
		this->finished = other.finished;
		this->finishedOn = other.finishedOn;
		this->assigned = other.assigned;
		this->assignedUser = other.assignedUser;
		this->boardID = other.boardID;
		return *this;
	}
};

class TaskBoard {
public:
    unsigned int taskboard_id;
    std::string name;
    std::set<Task> tasklist;
    std::set<User> users;
    std::set<User> admins;
};

#endif