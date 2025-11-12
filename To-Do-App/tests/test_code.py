from src.db import add, init, reset, getTasks, Task
import src.db
from datetime import datetime
import pytest

@pytest.fixture(scope="session", autouse=True)
def setup_environment():
    print("Setting up environment before tests...")
    init()
    src.db.TABLE = "TEST_Todo"
    yield
    print("Tearing down environment after tests...")
    reset()

# add valid task
def test_add_1():
    reset()
    assert "" == add(Task(1, "test", "test", datetime(2025, 9, 23), datetime(2026, 9, 23), None))
    db = getTasks(1)
    assert len(db) == 1

# add multiple tasks
def test_add_2():
    reset()
    assert "" == add(Task(1, "test1", "test1", datetime(2025, 9, 23), datetime(2026, 9, 23), None))
    assert "" == add(Task(1, "test2", "test2", datetime(2025, 9, 24), datetime(2026, 9, 24), None))
    db = getTasks(1)
    assert len(db) == 2

#add invalid task
def test_add_3():
    reset()
    assert "" != add(Task(1, None, None, None, None))