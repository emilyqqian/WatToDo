import mysql.connector
from datetime import datetime, timedelta
import argparse
import os
from dotenv import load_dotenv

# the table we are currently using
# when query, remember to choose the correct table
# because sometimes the table is "Data", sometims it is "TestTable"
TABLE = "Todo"

class Task:
    def __init__(self, user:int, item:str, type:str, started:datetime, due:datetime, done:datetime=None):
        self.item = item
        self.user = user
        self.type = type
        self.started = started
        self.due = due
        self.done = done
        self.id = 0

    def toTuple(self, includeDone) -> tuple:
        if (includeDone):
            return self.item, self.type, self.started, self.due, self.done, self.id
        else:
            return self.user, self.item, self.type, self.started, self.due

########## Logging In Functions - These are functions that handles database connections ##########

# this function creates a connection to the target database
# with specified username and password
def createConnection(user, password):
    try:
        mydb = mysql.connector.connect(
            host="riku.shoshin.uwaterloo.ca",
            user = user,
            password = password
        )    
        mycursor = mydb.cursor()
        mycursor.execute(f"USE SE101_Team_17")
        mydb.commit()
        return (mydb, mycursor)
    except mysql.connector.Error as err:
        print(f"Error connecting to database: {err}")
        return None
        
# this function reads username and password, then init the connection to the target database
def init():
    # try to get username and password from command line arguments
    parser = argparse.ArgumentParser(description="Parse command-line arguments.")
    parser.add_argument('-u', '--username', default=None, help="Username")
    parser.add_argument('-p', '--password', default=None, help="Password")
    args = parser.parse_args()
    username = args.username
    password = args.password

    refreshEnv = False

    load_dotenv()
    if not password:
        password = os.getenv("DB_PASSWORD")
        refreshEnv = True
    if not username:
        username = os.getenv("DB_USERNAME")
        refreshEnv = True
    
    if not username or not password:
        # I think ValueError is too harsh, maybe?
        #raise ValueError("Username or Password not Specified!");
        print("Username or Password not Specified! Usage: python3 code.py -u <username> -p <password>")
        quit()
        
    print(f"Logging in with username '{username}' and password '{password}'");
    
    temp_connection = createConnection(username, password)
    if (temp_connection is None):
        raise ValueError("Error Connecting to the Database! Check if the username and passwords are correct?")
    
    global database
    global cursor
    (database, cursor) = temp_connection
    print("Connection Established")

    if refreshEnv:
        with open('.env', 'a') as env_file:  # Use 'a' mode to append or 'w' to overwrite
            env_file.write(f'DB_PASSWORD={password}\n')
            env_file.write(f'DB_USERNAME={username}\n')
    
########## Helper Functions - These are functions that might be useful for the six functions or testcases ##########
    
# this is a helper function that checks if a task is valid
# for the six functions, consider calling this to check if the input is valid
def assertTaskValid(task:Task) -> str:
    if not task.item:
        return "Empty title!"
    if not task.type:
        return "Empty type!"
    if not task.due:
        return "Empty due date!"
    if not task.started:
        return "Empty start date!"
    if task.due < task.started:
        return "Due date must be later than start date"
    if len(task.item) > 255:
        return "Title too long"
    if len(task.type) > 255:
        return "Type too long"
    if task.id != 0:
        sql = f"SELECT * FROM {TABLE} WHERE taskid = %s"
        cursor.execute(sql, [task.id])
        if len(cursor.fetchall()) != 1:
            return f"Task with id {task.id} not found"
    return ""

def reset():
    sql = f"DELETE FROM {TABLE}"
    cursor.execute(sql)
    database.commit()

def getUser(username:str) -> int:
    sql = f"SELECT * FROM User WHERE username = %s"
    cursor.execute(sql, [username])
    result = cursor.fetchall()
    if len(result) == 0:
        print("User Not Found")
        return None
    else:
        return result[0][0]

def add(task:Task) -> str:
    res = assertTaskValid(task)
    if res != "":
        return res

    # add item
    sql = f"INSERT INTO {TABLE} (userid, item, type, start, due) VALUES (%s, %s, %s, %s, %s)"
    cursor.execute(sql, task.toTuple(False))
    database.commit()
    return ""

def update(task:Task)-> str:
    res = assertTaskValid(task)
    if res != "":
        return res

    # update item
    sql = f"UPDATE {TABLE} SET item = %s, type = %s, start = %s, due = %s, done = %s WHERE taskid = %s"
    cursor.execute(sql, task.toTuple(True))
    database.commit()
    return ""

def setDone(taskid:int, day:datetime):
    sql = f"UPDATE {TABLE} SET done = %s WHERE taskid = %s"
    cursor.execute(sql, [day, taskid])
    database.commit()

def delete(taskid:int)->str:
    print("selecting")
    sql = f"SELECT * FROM {TABLE} WHERE taskid = %s"
    cursor.execute(sql, [taskid])
    print("selected")
    if len(cursor.fetchall()) != 1:
        print("task not found")
        return f"Task with id {taskid} not found"
    
    # update item
    print("task found")
    sql = f"DELETE FROM {TABLE} WHERE taskid = %s"
    cursor.execute(sql, [taskid])
    database.commit()
    print("task deleted")
    return ""

def getTask(taskid:int):
    sql = f"SELECT * FROM {TABLE} WHERE taskid = %s"
    cursor.execute(sql, [taskid])
    res = cursor.fetchall()
    if (len(res) == 1):
        return res[0]
    print(f"Task with id {taskid} not found!")
    return None

def getTasks(userid:int):
    sql = f"SELECT * FROM {TABLE} WHERE userid = %s"
    cursor.execute(sql, [userid])
    return cursor.fetchall()

def getNextTask(userid:int):
    sql = f"SELECT * FROM {TABLE} WHERE userid = %s and done IS NULL ORDER BY due ASC, start ASC LIMIT 1"
    cursor.execute(sql, [userid])
    res = cursor.fetchall()
    return None if len(res) == 0 else res[0]

def getTodayTask(userid:int):
    sql = f"SELECT * FROM {TABLE} WHERE userid = %s and done IS NULL and DATE(due) = CURDATE()"
    cursor.execute(sql, [userid])
    res = cursor.fetchall()
    return None if len(res) == 0 else res