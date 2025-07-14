from flask import Flask, jsonify
from flask_cors import CORS
import subprocess

app = Flask(__name__)
CORS(app)

@app.route('/get-usage')
def get_usage():
    import random
    usage = [random.randint(50, 250) for _ in range(30)]
    with open('data/usage.txt', 'w') as f:
        f.write(','.join(map(str, usage)))
    return jsonify({"data": usage})

@app.route('/run-tracker')
def run_tracker():
    subprocess.run(['tracker.exe'])
    try:
        with open('data/bill_data.csv') as f:
            content = f.read()
        return jsonify({"csv": content})
    except:
        return jsonify({"csv": "❌ Failed to read output."})

app.run(debug=True, port=5050)

