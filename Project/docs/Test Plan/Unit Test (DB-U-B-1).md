# Test Plan #3: Taskboard Test
- Author: s969chen
- Date: Nov 6 2025
- ID: DB-U-B-1
- Type: Dynamic
- Level: Unit

## Description:
- Test operations about taskboard

## Coverage and Omissions:

### Coverage: 
- Basic CRUD operations around Taskboard including:
    - Create Taskboard
    - Get Taskboard by id or by user
    - Updating Taskboard
    - Delete Taskboard
- Reporting possible errors

### Omissions:
- Everything except for the coverage
- Varification of whether user has the permission to modify/delete task
    - This part will be tested in the integration test

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
- Due to the compexity of taskboard ,each operation should takes less than 300 ms

### Fail:
- The database does not reflect the changes as expected
- Errors are not reported correctly
- Operation takes longer than expected