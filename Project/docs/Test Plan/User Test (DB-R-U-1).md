# Test Plan #2: User Test
- Author: s969chen
- Date: Nov 3 2025
- ID: DB-R-U-1
- Type: Dynamic
- Level: Regression

## Description:
- Test operations about users

## Coverage and Omissions:

### Coverage: 
- Basic CRUD operations around User including:
    - User registration
    - Obtain user using userid/username
    - Update user
- Obtaining leaderboard
- Reporting possible errors

### Omissions:
- Everything except for the coverage
- Deleting user is not tested since we wants to keep all data

## Entry and Exit Criteria:

### Entry criteria:
- The Smoke Test is passed

### Exit criteria:
- Abort if any exceptions occured

## Expected Result:
- All CRUD Operations are reflected in the database
- Querried values are correct and up-to-date

## Pass/Fail Criteria:

### Pass: 
- The database reflects the changes as expected
- Values obtained from the database is correct and up-to-date
- Errors are reported
- Each operation takes less than 100 ms

### Fail:
- The database does not reflect the changes as expected
- Errors are not reported correctly
- Operation takes longer than expected