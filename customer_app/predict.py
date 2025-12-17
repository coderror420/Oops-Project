import sys
import pickle
import pandas as pd

# Load pipeline
pipeline = pickle.load(open("full_pipeline_model.pkl", "rb"))

# Input order must match training columns
columns = [
    "Transaction_ID", "User_ID", "Transaction_Amount", "Transaction_Type",
    "Account_Balance", "Device_Type", "Location",
    "Merchant_Category", "Previous_Fraudulent_Activity",
    "Daily_Transaction_Count", "Avg_Transaction_Amount_7d",
    "Failed_Transaction_Count_7d", "Transaction_Distance",
      "Authentication_Method","Hour","Day","Month","DayOfWeek"
]

# Collect CLI inputs in order
values = sys.argv[1:]  # all values as strings
df = pd.DataFrame([values], columns=columns)

# Predict
prediction = pipeline.predict(df)
fraud_score = pipeline.predict_proba(df)[:,1]  # if you want probability

print("Prediction:", prediction[0])
print("Fraud Score:", fraud_score[0])
