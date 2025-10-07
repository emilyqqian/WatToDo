import mysql.connector
from datetime import datetime, timedelta
import getpass
import argparse

# the table we are currently using
# when query, remember to choose the correct table
# because sometimes the table is "Data", sometims it is "TestTable"
TABLE = "Data"

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
    username = args.username;
    password = args.password;
    
    # since this code is probably going to run in the back-end, I won't add any error handler. 
    # Its the front-end's responsibility to ensure they send us the correct username and password
    if not username or not password:
        raise ValueError("Username or Password not Specified!");
        
    print(f"Logging in with username '{username}' and password '{password}'");
    
    temp_connection = createConnection(username, password)
    if (temp_connection is None):
        raise ValueError("Error Connecting to the Database! Check if the username and passwords are correct?")
    
    global database
    global cursor
    (database, cursor) = temp_connection
    print("Connection Established")
    
if __name__ == "__main__":
    init()