# Test Plan #3: Task Test
- Author: s969chen
- Date: Nov 3 2025
- ID: DB-U-T-1
- Type: Dynamic
- Level: Unit

## Description:
- Test operations about users

## Coverage and Omissions:

### Coverage: 
- Basic CRUD operations around Task including:
    - Create Task
    - Read Task
    - Update Task
    - Delete Task
- Reporting possible errors

### Omissions:
- Everything except for the coverage
- Varification of whether user has the permission to modify/delete task
    - This part has not been implemented yet, and will be implemented after TaskBoard related functions are all set-up
- Assigning User to Task
    - This part will be done in the integration test with user and task

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