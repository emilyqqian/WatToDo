# Setup Guide:

The following instructions will (hopefully) help you set up the development environment for the backend of the To-Do App project.
- If not, ask chatgpt. Don't ask me. I spent 2h trying to compile my code

## Libraries:
Make sure you have the following libraries installed:
- Mysqlsh (Optional but Recommended)
- mysqlcppconn (Version 9.5)
- CMake
- Crow 

## Database Setup:
- After connecting to a database, run the script in `src/databaseScheme.sql` to set up the necessary tables.
- Note: This only applies if you do not have your database set-up already.
- If you are in Project-Team 17, Please do not run this script, as it will reset the entire database

## Build Instructions:
- Make sure you have CMake and mysqlcppconn installed
- Include the following in your CMakeLists.txt:
```
set(MYSQL_USER <YOUR USERNAME>)
set(MYSQL_PASSWORD <YOUR PASSWORD>)
set(MYSQL_DB_NAME <YOUR DATABASE NAME>)

set(MYSQL_CONCPP_DIR <YOUR mysqlcppconn DIRECTORY>)
include_directories("${MYSQL_CONCPP_DIR}/include")
link_directories("${MYSQL_CONCPP_DIR}/lib64/vs14")

add_executable(backend database.cpp)
target_link_libraries(backend mysqlcppconnx)

target_compile_definitions(backend PRIVATE
    MYSQL_USER="${MYSQL_USER}"
    MYSQL_PASSWORD="${MYSQL_PASSWORD}"
    MYSQL_DB_NAME="${MYSQL_DB_NAME}"
)

```

---

## Crow Build Instructions

Add this to your `CMakeLists.txt` to include Crow:

### Option 1 — Using Crow as a header‑only library (recommended)

Download `crow_all.h` from the Crow GitHub repo and place it in `src/`.

```cmake
include_directories(${CMAKE_SOURCE_DIR}/src)
```

Use normally:

```cpp
#include "crow_all.h"
```

No linking required.

### Option 2 — Using Crow via CMake FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(
    crow
    GIT_REPOSITORY https://github.com/CrowCpp/crow.git
    GIT_TAG v1.2.0
)
FetchContent_MakeAvailable(crow)

target_link_libraries(backend PRIVATE Crow::Crow)
```

---

## Windows Notes

If you're using Windows and installed mysqlcppconn in the default location, then:

```
MYSQL_CONCPP_DIR = "C:/Program Files/MySQL/MySQL Connector C++ 9.5"
```

---

## Troubleshooting
[Chatgpt goated](https://chatgpt.com/share/69028f5f-50b4-8003-8409-90088d953218)
