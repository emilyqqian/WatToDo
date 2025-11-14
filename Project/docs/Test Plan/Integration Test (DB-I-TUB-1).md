# Test Plan #4: Integration Test
- Author: s969chen
- Date: Nov 6 2025
- ID: DB-I-TUD-1
- Type: Dynamic
- Level: Integration

## Description:
- Integration test about all database functions

## Coverage and Omissions:

### Coverage: 
- CRUD about taskboard and tasks with or without appropriate permission
- invite, add, and kick-out users

### Omissions:
- Everything except for the coverage

## Entry and Exit Criteria:

### Entry criteria:
- The Smoke Test is passed
- All Unit Tests are passed

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
- Each operation takes less than 300 ms

### Fail:
- The database does not reflect the changes as expected
- Errors are not reported correctly
- Operation takes longer than expected