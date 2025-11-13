import flask
import db
import datetime

app = flask.Flask(__name__)


@app.route('/')
def home():
    return flask.render_template('login.html')

@app.route('/api/add/<name>', methods=['POST'])
def add(name):
    form = flask.request.form

    try:
        start = datetime.datetime.strptime(form['start'], '%Y-%m-%dT%H:%M')
        due = datetime.datetime.strptime(form['due'], '%Y-%m-%dT%H:%M')
    except:
        return flask.jsonify(success=False, message='Error parsing start and due date!'), 400

    task = db.Task(int(name), form['title'], form['type'], start, due)
    res = db.add(task)
    if res != '':
        return flask.jsonify(success=False, message=res), 400
    return flask.jsonify(success=True, redirect_url=flask.url_for('dashboard', name=name))

@app.route('/api/update/<name>/<id>', methods=['POST'])
def update(name, id):
    form = flask.request.form

    try:
        start = datetime.datetime.strptime(form['start'], '%Y-%m-%dT%H:%M')
        due = datetime.datetime.strptime(form['due'], '%Y-%m-%dT%H:%M')
        finished = None if form['finished'] == "" else datetime.datetime.strptime(form['finished'], '%Y-%m-%dT%H:%M')
    except:
        return flask.jsonify(success=False, message='Error parsing start and due date!'), 400

    task = db.Task(int(name), form['title'], form['type'], start, due, done=finished)
    task.id = id

    res = db.update(task)
    if res != '':
        return flask.jsonify(success=False, message=res), 400
    return flask.jsonify(success=True, redirect_url=flask.url_for('dashboard', name=name))

@app.route('/api/delete/<id>', methods=['GET'])
def delete(id):
    res = db.delete(id)
    if res != '':
        return flask.jsonify(success=False, message=res), 400
    return flask.jsonify(success=True, message=""), 200


@app.route('/newTask/<name>')
def newTask(name):
    return flask.render_template('newTask.html', user=name, api="add")

@app.route('/updateTask/<name>/<id>')
def updateTask(name, id):
    task = db.getTask(id)
    print(f"day: {task[6]}")
    return flask.render_template('newTask.html', user=name, api="update", 
                                 taskid=id, title=task[2], type=task[3],
                                 start=task[4], due=task[5], finished=task[6])


@app.route('/dashboard/<name>')
def dashboard(name):
    return flask.render_template('dashboard.html', user=name, tasks=db.getTasks(int(name)))


@app.route('/login', methods=['POST'])
def login():
    print('from login')
    user = db.getUser(flask.request.form['userid'])
    if (user == None):
        return flask.jsonify(success=False, message="Username not found"), 400
    return flask.jsonify(success=True, redirect_url=flask.url_for('dashboard', name=user))

if __name__ == "__main__":
    db.init()
    app.run()