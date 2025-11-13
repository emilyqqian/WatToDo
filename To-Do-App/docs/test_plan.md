# Test Plan #1: System Test
- Author: s969chen
- Date: Nov 13 2025
- ID: T-1
- Type: Dynamic
- Level: System

## Description:
- System test about the entire database connection level

## Coverage and Omissions:

### Coverage: 
- All CRUD operations about tasks

### Omissions:
- API

## Entry and Exit Criteria:

### Entry criteria:
- Database connection successful
- Test tables are enabled

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

### Fail:
- The database does not reflect the changes as expected
- Errors are not reported correctly
- Operation takes longer than expected