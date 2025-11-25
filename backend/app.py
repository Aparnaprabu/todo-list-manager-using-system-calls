from flask import Flask, request, jsonify, send_from_directory
import subprocess

app = Flask(__name__, static_folder='static')

def run_cmd(args):
    res = subprocess.run(args, capture_output=True, text=True)
    #text=True → treats the output as a normal string instead of bytes.
    return res.stdout.strip()

@app.route('/api/add', methods=['POST'])
#@app.route-Connects a URL to a Python function
def add_task():
    task = request.json.get('task')
    output = run_cmd(['./todo_cli', 'add', task])
    return jsonify({'result': output})

@app.route('/api/view', methods=['GET'])
def view_tasks():
    output = run_cmd(['./todo_cli', 'view'])
    tasks = []
    for line in output.splitlines():
        if '|' in line:
            no, text = line.split('|', 1)
            tasks.append({'no': no, 'text': text})
    return jsonify({'tasks': tasks})

@app.route('/api/mark', methods=['POST'])
def mark_task():
    no = str(request.json.get('no'))
    output = run_cmd(['./todo_cli', 'mark', no])
    return jsonify({'result': output})

@app.route('/api/delete', methods=['POST'])
def delete_task():
    no = str(request.json.get('no'))
    output = run_cmd(['./todo_cli', 'delete', no])
    return jsonify({'result': output})

@app.route('/api/backup', methods=['POST'])
def backup_tasks():
    output = run_cmd(['./todo_cli', 'backup'])
    return jsonify({'result': output})

@app.route('/')
def index():
    return send_from_directory('static', 'index.html')

if __name__ == '__main__':
    app.run(debug=True)
