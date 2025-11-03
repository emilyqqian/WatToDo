#include <string>
#include <set>
#include <ctime>
#include <vector>
#include <cstddef>

#ifndef _DATABASE_CLASSES_H
#define _DATABASE_CLASSES_H

std::vector<uint64_t> mysqlx_raw_as_u64_vector(const mysqlx::Value& in_value){
  std::vector<uint64_t> out;

  const auto bytes = in_value.getRawBytes();
  auto ptr = reinterpret_cast<const std::byte*>(bytes.first);
  auto end = reinterpret_cast<const std::byte*>(bytes.first) + bytes.second;

  while (ptr != end) {
    static constexpr std::byte carry_flag{0b1000'0000};
    static constexpr std::byte value_mask{0b0111'1111};

    uint64_t v = 0;
    uint64_t shift = 0;
    bool is_carry;
    do {
      auto byte = *ptr;
      is_carry = (byte & carry_flag) == carry_flag;
      v |= std::to_integer<uint64_t>(byte & value_mask) << shift;

      ++ptr;
      shift += 7;
    } while (is_carry && ptr != end && shift <= 63);

    out.push_back(v);
  }

  return out;
}

class date {
public:

    int year;
    int month;
    int day;

    date(int year, int month, int day) {
        this->year = year;
        this->month = month;
        this->day = day;
    }

    date() {
        // initialize to current date
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        year = now->tm_year + 1900;
        month = now->tm_mon + 1;
        day = now->tm_mday;
    }

    date(const mysqlx::Value& value){
        const auto vector = mysqlx_raw_as_u64_vector(value);
        if (vector.size() < 3)
        throw std::out_of_range{"Value is not a valid DATE"};

        year = static_cast<int>(vector.at(0));
        month = static_cast<int>(vector.at(1));
        day = static_cast<int>(vector.at(2));
        //return std::chrono::year{static_cast<int>(vector.at(0))} / static_cast<int>(vector.at(1)) / static_cast<int>(vector.at(2));
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

    // default init
    Task(){
        
    }

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

    TaskBoard& operator=(const TaskBoard& other){
        this->name = other.name;
        this->tasklist = other.tasklist;
        this->users = other.users;
        this->admins = other.admins;
    }
};

#endif