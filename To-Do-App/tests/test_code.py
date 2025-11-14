from src.db import add, update, delete, init, reset, getNextTask, getTodayTask, getTomorrowTask, getTasks, Task
import src.db
from datetime import datetime, timedelta
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

# update valid task
def test_update_1():
    reset()
    assert "" == add(Task(1, "test", "test", datetime(2025, 9, 23), datetime(2026, 9, 23), None))
    db = getTasks(1)
    t = Task(1, "abab", "test", datetime(2025, 9, 23), datetime(2026, 9, 23), None)
    t.id = db[0][0]
    assert "" == update(t)
    db = getTasks(1)
    assert len(db) == 1
    assert db[0][2] == "abab"

# update nonexisting task
def test_update_2():
    reset()
    t = Task(1, "abab", "test", datetime(2025, 9, 23), datetime(2026, 9, 23), None)
    t.id = 114514
    assert "" != update(t)

# update invalid task
def test_update_3():
    reset()
    assert "" == add(Task(1, "test", "test", datetime(2025, 9, 23), datetime(2026, 9, 23), None))
    db = getTasks(1)
    t = Task(1, None, None, datetime(2025, 9, 23), datetime(2026, 9, 23), None)
    t.id = db[0][0]
    assert "" != update(t)

 # delete valid task
def test_delete_1():
    reset()
    assert "" == add(Task(1, "test", "test", datetime(2025, 9, 23), datetime(2026, 9, 23), None))
    db = getTasks(1)
    assert "" == delete(db[0][0])
    db = getTasks(1)
    assert len(db) == 0

# delete nonexisting task
def test_delete_2():
    reset()
    assert "" != delete(114514)

# next valid task
def test_next_1():
    reset()
    assert "" == add(Task(1, "a", "test", datetime(2025, 9, 23), datetime(2026, 9, 23), None))
    assert "" == add(Task(1, "b", "test", datetime(2015, 9, 23), datetime(2026, 9, 23), None))
    assert "b" == getNextTask(1)[2]

# no next task
def test_next_2():
    reset()
    assert None == getNextTask(1)

# check today
def test_today_1():
    reset()
    assert "" == add(Task(1, "a", "test", datetime(2025, 9, 23), datetime.now(), None))
    assert "" == add(Task(1, "b", "test", datetime(2015, 9, 23), datetime.now() + timedelta(days=1), None))
    res = getTodayTask(1)
    assert len(res) == 1
    assert res[0][2] == "a"

# not task today
def test_today_2():
    reset()
    assert None == getTodayTask(1)

# check tomorrow
def test_tomorrow_1():
    reset()
    assert "" == add(Task(1, "a", "test", datetime(2025, 9, 23), datetime.now(), None))
    assert "" == add(Task(1, "b", "test", datetime(2015, 9, 23), datetime.now() + timedelta(days=1), None))
    res = getTomorrowTask(1)
    assert len(res) == 1
    assert res[0][2] == "b"

# not task tomorrow
def test_tomorrow_2():
    reset()
    assert None == getTomorrowTask(1)