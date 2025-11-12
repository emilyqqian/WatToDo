# Test Plan #5: Integration Test
- Author: s969chen
- Date: Nov 6 2025
- ID: DB-I-TU-2
- Type: Dynamic
- Level: Integration

## Description:
- Integration test about Users and Tasks

## Coverage and Omissions:

### Coverage: 
- Adding Task without Permission
- Deleting Task without permission

### Omissions:
- Everything except for the coverage

## Entry and Exit Criteria:

### Entry criteria:
- The Smoke Test is passed
- Unit test for both user and tasks are passed

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