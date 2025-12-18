import sys
import os

# ============================================
# SUPPRESS ALL WARNINGS BEFORE ANY IMPORTS
# ============================================
import warnings
warnings.filterwarnings("ignore")

# Suppress environment warnings
os.environ['PYTHONWARNINGS'] = 'ignore'
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

# Redirect stderr to suppress frozen importlib warnings
import io
original_stderr = sys.stderr
sys.stderr = io.StringIO()
from sklearn.base import BaseEstimator, TransformerMixin

class MultiColumnLabelEncoder(BaseEstimator, TransformerMixin):
    def __init__(self, columns=None):
        self.columns = columns  # List of columns to encode
        self.encoders = {}      # Dictionary to store LabelEncoders

    def fit(self, X, y=None):
        return self # We just need the class structure for loading

    def transform(self, X):
        return X
# ============================================
# NOW IMPORT LIBRARIES
# ============================================
try:
    import pandas as pd
    import joblib
    import json
    import numpy as np
    
    # Suppress numpy-specific warnings
    np.seterr(all='ignore')
    
except ImportError as e:
    sys.stderr = original_stderr
    print(f"❌ Error importing libraries: {e}")
    exit(1)

# Restore stderr after imports
sys.stderr = original_stderr

print("="*60)
print("🔧 Exporting Model to JSON Format")
print("="*60)

# Load your trained pipeline
try:
    model = joblib.load("model.pkl")
    print("✅ Loaded model.pkl")
except FileNotFoundError:
    print("❌ Error: model.pkl not found!")
    print("Please run your training notebook first to create model.pkl")
    exit(1)

# Extract pipeline components
label_encoder = model.named_steps["label_encode"]
scaler = model.named_steps["scaler"]
xgb_model = model.named_steps["model"]

print("\n📊 Model Components:")
print(f"  - Label Encoder: {len(label_encoder.columns)} categorical features")
print(f"  - Scaler: {len(scaler.mean_)} numerical features")
print(f"  - XGBoost Model: {xgb_model.n_estimators} estimators")

# Export configuration
model_config = {
    "feature_names": [
        "Transaction_Amount",
        "Account_Balance", 
        "Daily_Transaction_Count",
        "Avg_Transaction_Amount_7d",
        "Failed_Transaction_Count_7d",
        "Transaction_Distance",
        "Hour",
        "Day",
        "Month",
        "DayOfWeek",
        "Transaction_Type",
        "Device_Type",
        "Location",
        "Merchant_Category",
        "Authentication_Method"
    ],
    
    "categorical_columns": [
        "Transaction_Type",
        "Device_Type",
        "Location",
        "Merchant_Category",
        "Authentication_Method"
    ],
    
    "scaler_mean": scaler.mean_.tolist(),
    "scaler_std": scaler.scale_.tolist(),
    
    "label_encoders": {}
}

# Extract label encoder mappings
print("\n🏷️  Extracting Label Encoders:")
for col in label_encoder.columns:
    encoder = label_encoder.encoders[col]
    mapping = {str(label): int(idx) for idx, label in enumerate(encoder.classes_)}
    model_config["label_encoders"][col] = mapping
    print(f"  - {col}: {len(mapping)} classes")

# Save configuration
with open("model_config.json", "w") as f:
    json.dump(model_config, f, indent=2)
print("\n✅ Saved model_config.json")

# Save XGBoost model
xgb_model.save_model("xgb_model.json")
print("✅ Saved xgb_model.json")

print("\n" + "="*60)
print("🎉 Export Complete!")
print("="*60)
print("\nGenerated files:")
print("  1. model_config.json  - Preprocessing configuration")
print("  2. xgb_model.json     - XGBoost model")
print("\nYou can now use these files in your C++ application.")
print("="*60)