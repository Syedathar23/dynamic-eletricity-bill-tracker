from flask import Flask, jsonify
from flask_cors import CORS
import subprocess
import random
import os

app = Flask(__name__)
CORS(app)  # 🔓 Allow requests from browser (different port)

DATA_FOLDER = os.path.join(os.getcwd(), 'data')
USAGE_FILE = os.path.join(DATA_FOLDER, 'usage.txt')
RESULT_FILE = os.path.join(DATA_FOLDER, 'bill_data.csv')

# Create data folder if missing
os.makedirs(DATA_FOLDER, exist_ok=True)

@app.route('/get-usage', methods=['GET'])
def get_usage():
    # 🔧 Simulate 30 days of random electricity usage (units)
    usage = [random.randint(10, 30) for _ in range(30)]

    # 💾 Save to usage.txt
    with open(USAGE_FILE, 'w') as f:
        f.write(','.join(map(str, usage)))

    return jsonify({
        "message": "Usage data generated and saved.",
        "data": usage
    })


@app.route('/run-tracker', methods=['GET'])
def run_tracker():
    try:
        # ▶️ Run your compiled C++ tracker (assumes tracker.exe is in same folder)
        result = subprocess.run(['./tracker.exe'], check=True, capture_output=True, text=True)
        
        # 📄 Read the output CSV
        if os.path.exists(RESULT_FILE):
            with open(RESULT_FILE, 'r') as f:
                csv_data = f.read()
        else:
            csv_data = "No bill_data.csv found."

        return jsonify({
            "message": "Tracker ran successfully.",
            "csv": csv_data
        })

    except subprocess.CalledProcessError as e:
        return jsonify({
            "error": "Failed to run tracker.exe",
            "details": e.stderr
        }), 500

@app.route('/')
def hello():
    return "🎉 Flask API is running on port 3000!"

if __name__ == '__main__':
    app.run(debug=True, port=5000)
