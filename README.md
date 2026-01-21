# WatToDo - Gamified Task Manager

A modular task management platform featuring real-time shared boards and gamified productivity elements to enhance user engagement and collaboration.

## Features

- **Shared Task Boards**: Collaborate with others in real-time on shared task lists
- **Gamification System**: 
  - Earn XP for completing tasks
  - Track progress on leaderboards
  - Compete with friends and teammates
- **Full CRUD Operations**: Create, read, update, and delete tasks with ease
- **User Authentication**: Secure login system with JWT authentication
- **Real-time Updates**: See changes instantly across all users

## 🛠️ Tech Stack

**Frontend:**
- HTML/CSS/JavaScript
- Responsive design for cross-device compatibility

**Backend:**
- **C++** with Crow framework for high-performance REST API
- **MySQL** database for persistent data storage
- JWT-based authentication

## 🚀 Getting Started

### Prerequisites

- C++ compiler (C++17 or later)
- MySQL 8.0+
- Python 3.x with Flask
- Docker (optional, for containerized deployment)

### Installation

1. Clone the repository
```bash
git clone https://github.com/yourusername/wattodo.git
cd wattodo
```

2. Set up MySQL database
```sql
CREATE DATABASE wattodo;
-- Run schema.sql to create tables
```

3. Configure environment variables
```bash
cp .env.example .env
# Edit .env with your database credentials
```

4. Build and run
```bash
mkdir build && cd build
cmake ..
make
./wattodo
```

---

## 🔒 Security

- Password hashing with bcrypt
- JWT tokens for session management
- SQL injection prevention with prepared statements
- Input validation on all endpoints
