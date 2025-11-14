# Test Plan #1: Smoke Test
- Author: s969chen
- Date: Nov 3 2025
- ID: DB-S-1
- Type: Dynamic
- Level: Smoke Test

## Description:
- Smoke test for the system including:
    - Ensure proper connection to the database
    - Ensure test environment is enabled

## Coverage and Omissions:

### Coverage: 
- Database connection exists and is valid
- Data tables exist and valid
- Test environment is enabled

### Omissions:
- Everything except for the coverage

## Entry and Exit Criteria:

### Entry criteria:
- None. This is the first test to be run

### Exit criteria:
- Abort if any test objectives failed

## Expected Result:
- The database connection is established and test env is enabled

## Pass/Fail Criteria:

### Pass: 
- All test cases are passed

### Fail:
- Any of the test cases failed