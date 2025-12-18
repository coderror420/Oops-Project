import sys
import os
import warnings
warnings.filterwarnings("ignore")
os.environ['PYTHONWARNINGS'] = 'ignore'

# Suppress stderr during imports
import io
_stderr = sys.stderr
sys.stderr = io.StringIO()

try:
    import json
    import xgboost as xgb
    import numpy as np
except ImportError:
    sys.stderr = _stderr
    print("0.0")
    sys.exit(0)

# Restore stderr
sys.stderr = _stderr

def predict():
    try:
        # Load configuration
        with open("model_config.json", "r") as f:
            config = json.load(f)
        
        # Load XGBoost model
        model = xgb.Booster()
        model.load_model("xgb_model.json")
        
        if len(sys.argv) < 16:
            return 0.0
        
        # Parse command line arguments
        raw_data = {
            'Transaction_Amount': float(sys.argv[1]),
            'Account_Balance': float(sys.argv[2]),
            'Daily_Transaction_Count': int(sys.argv[3]),
            'Avg_Transaction_Amount_7d': float(sys.argv[4]),
            'Failed_Transaction_Count_7d': int(sys.argv[5]),
            'Transaction_Distance': float(sys.argv[6]),
            'Hour': int(sys.argv[7]),
            'Day': int(sys.argv[8]),
            'Month': int(sys.argv[9]),
            'DayOfWeek': int(sys.argv[10]),
            'Transaction_Type': sys.argv[11],
            'Device_Type': sys.argv[12],
            'Location': sys.argv[13],
            'Merchant_Category': sys.argv[14],
            'Authentication_Method': sys.argv[15]
        }
        
        # Encode categorical features
        for col in config["categorical_columns"]:
            label = raw_data[col]
            if label in config["label_encoders"][col]:
                raw_data[col] = config["label_encoders"][col][label]
            else:
                # Unknown category - use first class
                raw_data[col] = 0
        
        # Build feature vector in correct order
        features = [raw_data[name] for name in config["feature_names"]]
        
        # Scale features
        scaled = [
            (features[i] - config["scaler_mean"][i]) / (config["scaler_std"][i] + 1e-8)
            for i in range(len(features))
        ]
        
        # Create DMatrix
        dmatrix = xgb.DMatrix([scaled])
        
        # Predict (returns probability)
        prediction = model.predict(dmatrix)[0]
        
        return float(prediction)
        
    except Exception:
        return 0.0

if __name__ == "__main__":
    result = predict()
    print(f"{result:.6f}")