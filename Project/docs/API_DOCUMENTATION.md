# WatToDo Backend — API Documentation

This document outlines all backend routes, request/response formats, and behaviors for the **WatToDo** server built using the **Crow C++ Web Framework**.

---

## Server Overview

* **Framework:** Crow (C++ Web Framework)
* **Database:** MySQL (via `database.h`)
* **Port:** `18080`
* **CORS:** Enabled for:

  * `http://localhost:3000`
  * `http://127.0.0.1:3000`

The server automatically initializes the database connection at startup and closes it at shutdown.

---

# API Endpoints

---

## 1. Register a User

### **POST** `/register`

Creates a new user in the database.

### **Request Body**

```json
{
  "username": "john",
  "password": 1234
}
```

### **Responses**

| Status  | Meaning                           |
| ------- | --------------------------------- |
| **201** | User registered successfully      |
| **409** | Username already exists           |
| **400** | Invalid JSON or username too long |
| **500** | Internal server error             |

---

## 2. Update User Information

### **PUT** `/users/<id>`

Updates an existing user's username, password, or XP points.

### **Request Body**

```json
{
  "username": "newname",
  "password": 5678,
  "xp_points": 250
}
```

### **Responses**

| Status  | Meaning                           |
| ------- | --------------------------------- |
| **200** | User updated successfully         |
| **409** | Username already exists           |
| **404** | User not found                    |
| **400** | Invalid JSON or username too long |
| **500** | Internal server error             |

---

## 3. Get User by Username

### **GET** `/users/<username>`

Returns information for a specific user.

### **Successful Response (200)**

```json
{
  "user_id": 1,
  "username": "john",
  "xp_points": 120
}
```

### **Error Responses**

| Status  | Meaning        |
| ------- | -------------- |
| **404** | User not found |
| **500** | Server error   |

---

## 4. Get User by ID

### **GET** `/users/id/<id>`

Fetches user information based on numeric ID.

### **Successful Response (200)**

```json
{
  "user_id": 3,
  "username": "alice",
  "xp_points": 500
}
```

### **Error Responses**

| Status  | Meaning        |
| ------- | -------------- |
| **404** | User not found |
| **500** | Server error   |

---

## 5. Leaderboard

### **GET** `/leaderboard`

Returns all users sorted by XP (based on database logic).

### **Successful Response (200)**

```json
{
  "leaderboard": [
    { "username": "alice", "xp": 500 },
    { "username": "john",  "xp": 120 }
  ]
}
```

### **Error Responses**

| Status  | Meaning      |
| ------- | ------------ |
| **500** | Server error |

---

## 6. Test Endpoint

### **GET** `/test`

Basic server health check.

### **Response**

```
Crow server is running with complete user management!
```

