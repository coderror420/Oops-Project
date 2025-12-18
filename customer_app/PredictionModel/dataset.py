# ---
# jupyter:
#   jupytext:
#     text_representation:
#       extension: .py
#       format_name: percent
#       format_version: '1.3'
#       jupytext_version: 1.18.1
#   kernelspec:
#     display_name: Python 3
#     language: python
#     name: python3
# ---

# %% jupyter={"is_executing": true}
# train_pipeline.py

import pandas as pd
import joblib

from sklearn.preprocessing import LabelEncoder, StandardScaler
from sklearn.pipeline import Pipeline
from sklearn.base import BaseEstimator, TransformerMixin
from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report

from imblearn.over_sampling import SMOTE
from imblearn.pipeline import Pipeline 

from xgboost import XGBClassifier


# %%

df = pd.read_csv('data.csv')

# %%
df['Timestamp'] = pd.to_datetime(df['Timestamp'])

df['Hour'] = df['Timestamp'].dt.hour
df['Day'] = df['Timestamp'].dt.day
df['Month'] = df['Timestamp'].dt.month
df['DayOfWeek'] = df['Timestamp'].dt.dayofweek

df = df.sort_values(['User_ID'])
df.drop(columns=['Timestamp'],inplace=True)

# %%
# # Transactions per user
# user_txn_count = df.groupby('User_ID')['Transaction_ID'].transform('count')
# df['user_transaction_count'] = user_txn_count

# # Average amount per user
# df['user_avg_amount'] = df.groupby('User_ID')['Transaction_Amount'].transform('mean')

# #standard deviation in transaction amount
# df['user_amt_std'] = (
#     df.groupby('User_ID')['Transaction_Amount'].transform('std')
#     .fillna(0)
# )

# # Time gap from previous transaction (seconds)
# df['time_gap_prev_txn'] = (
#     df.groupby('User_ID')['Timestamp']
#       .diff()
#       .dt.total_seconds()
#       .fillna(0)
# )

# %%
df.drop(columns=['Card_Type','Card_Age','Risk_Score','Is_Weekend','IP_Address_Flag'], inplace=True)
df.columns

# %%
df.to_csv("data.csv", index=False)

# %%
# Numerical
num_cols = df.select_dtypes(include=['int64', 'float64']).columns
df[num_cols] = df[num_cols].fillna(df[num_cols].median())

# Categorical
cat_cols = df.select_dtypes(include='object').columns
df[cat_cols] = df[cat_cols].fillna("Unknown")


# %%

le = LabelEncoder()
for col in cat_cols:
    df[col] = le.fit_transform(df[col])



# %%
# Numeric correlation only
corr_with_target = df.corr()['Fraud_Label']
corr_with_target


# %%
df

# %%
X = df.drop(columns=['Fraud_Label','User_ID','Transaction_ID'])
y = df['Fraud_Label']


# %%

scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)


# %%
smote = SMOTE(random_state=42)
X_resampled, y_resampled = smote.fit_resample(X_scaled, y)

# %%

X_train, X_test, y_train, y_test = train_test_split(
    X_resampled, y_resampled,
    test_size=0.2,
    random_state=42,
    stratify=y_resampled
)


# %%
from sklearn.ensemble import RandomForestClassifier

rf = RandomForestClassifier(
    n_estimators=200,
    max_depth=12,
    min_samples_split=10,
    class_weight='balanced',
    random_state=42
)

rf.fit(X_train, y_train)


# %%
from sklearn.metrics import classification_report, roc_auc_score

y_pred_rf = rf.predict(X_test)
y_prob_rf = rf.predict_proba(X_test)[:,1]

print(classification_report(y_test, y_pred_rf))
print("ROC-AUC:", roc_auc_score(y_test, y_prob_rf))


# %%
from xgboost import XGBClassifier

xgb = XGBClassifier(
    n_estimators=300,
    max_depth=6,
    learning_rate=0.05,
    subsample=0.8,
    colsample_bytree=0.8,
    scale_pos_weight=len(y_train[y_train==0]) / len(y_train[y_train==1]),
    eval_metric='logloss',
    random_state=42
)

xgb.fit(X_train, y_train)


# %%
from sklearn.metrics import classification_report, roc_auc_score

y_pred_xgb = xgb.predict(X_test)
y_prob_xgb = xgb.predict_proba(X_test)[:,1]

print(classification_report(y_test, y_pred_xgb))
print("ROC-AUC:", roc_auc_score(y_test, y_prob_xgb))


# %%
y_train.value_counts()

# %%
train_pipeline = ImbPipeline([
    ("user_features", UserFeatureEngineer()),
    ("preprocessing", preprocessor),

    ("model", xgb)
])

# %%
from sklearn.pipeline import Pipeline

# %%
TARGET = "Fraud_Label"

NUM_FEATURES = [
    "Transaction_Amount",
    "Account_Balance",
    "Daily_Transaction_Count",
    "Avg_Transaction_Amount_7d",
    "Failed_Transaction_Count_7d",
    "Transaction_Distance",
    "Hour",
    "Day",
    "Month",
    "DayOfWeek"
]

CAT_FEATURES = [
    "Transaction_Type",
    "Device_Type",
    "Merchant_Category",
    "Authentication_Method",
    "IP_Address_Flag"
]

X = df[NUM_FEATURES + CAT_FEATURES]
y = df[TARGET]


# %%
class UserFeatureEngineer(BaseEstimator, TransformerMixin):
    def __init__(self, user_col="User_ID", txn_col="Transaction_ID", amount_col="Transaction_Amount", time_col="Timestamp"):
        self.user_col = user_col
        self.txn_col = txn_col
        self.amount_col = amount_col
        self.time_col = time_col

    def fit(self, X, y=None):
        return self

    def transform(self, X):
        df = X.copy()
        
        # Ensure Timestamp is datetime
        df[self.time_col] = pd.to_datetime(df[self.time_col])

        # Number of transactions per user
        df['user_transaction_count'] = df.groupby(self.user_col)[self.txn_col].transform('count')
        
        # Average transaction amount per user
        df['user_avg_amount'] = df.groupby(self.user_col)[self.amount_col].transform('mean')
        
        # Std of transaction amount per user
        df['user_amt_std'] = df.groupby(self.user_col)[self.amount_col].transform('std').fillna(0)
        
        # Time gap from previous transaction per user (seconds)
        df['time_gap_prev_txn'] = df.groupby(self.user_col)[self.time_col].diff().dt.total_seconds().fillna(0)
        
        # Drop Transaction_ID and User_ID
        df = df.drop(columns=[self.txn_col, self.user_col])
        return df



# %%
class LabelEncodeTransformer(BaseEstimator, TransformerMixin):
    def __init__(self):
        self.encoders = {}

    def fit(self, X, y=None):
        for col in X.columns:
            le = LabelEncoder()
            X[col] = X[col].astype(str)
            le.fit(X[col])
            self.encoders[col] = le
        return self

    def transform(self, X):
        X_out = X.copy()
        for col, le in self.encoders.items():
            X_out[col] = le.transform(X_out[col].astype(str))
        return X_out



# %%
class UserFeatureEngineer(BaseEstimator, TransformerMixin):
    def __init__(self, user_col="User_ID", txn_col="Transaction_ID", amount_col="Transaction_Amount"):
        self.user_col = user_col
        self.txn_col = txn_col
        self.amount_col = amount_col
        # self.time_col = time_col

    def fit(self, X, y=None):
        return self

    def transform(self, X):
        df = X.copy()
        df[self.time_col] = pd.to_datetime(df[self.time_col])

        # Transactions per user
        df['user_transaction_count'] = df.groupby(self.user_col)[self.txn_col].transform('count')

        # Average transaction amount per user
        df['user_avg_amount'] = df.groupby(self.user_col)[self.amount_col].transform('mean')

        # Std of transaction amount per user
        df['user_amt_std'] = df.groupby(self.user_col)[self.amount_col].transform('std').fillna(0)

        # # Time gap from previous transaction
        # df['time_gap_prev_txn'] = df.groupby(self.user_col)[self.time_col].diff().dt.total_seconds().fillna(0)

        # Drop Transaction_ID and User_ID
        df = df.drop(columns=[self.txn_col, self.user_col])
        return df

# ----------------------------
# 2️⃣ Custom transformer for label encoding categorical columns
# ----------------------------
class LabelEncodeTransformer(BaseEstimator, TransformerMixin):
    def __init__(self):
        self.encoders = {}

    def fit(self, X, y=None):
        for col in X.columns:
            le = LabelEncoder()
            X[col] = X[col].astype(str)
            le.fit(X[col])
            self.encoders[col] = le
        return self

    def transform(self, X):
        X_out = X.copy()
        for col, le in self.encoders.items():
            X_out[col] = le.transform(X_out[col].astype(str))
        return X_out

# ----------------------------
# 3️⃣ Load dataset
# ----------------------------


TARGET = "Fraud_Label"

NUM_FEATURES = [
    "Transaction_Amount", "Account_Balance", "Daily_Transaction_Count",
    "Avg_Transaction_Amount_7d", "Failed_Transaction_Count_7d",
    "Transaction_Distance", "Hour", "Day", "Month", "DayOfWeek",
    "user_transaction_count", "user_avg_amount", "user_amt_std", "time_gap_prev_txn"
]

CAT_FEATURES = [
    "Transaction_Type", "Device_Type", "Location",
    "Merchant_Category", "IP_Address_Flag", "Authentication_Method"
]

X = df.drop(columns=[TARGET])
y = df[TARGET]

# ----------------------------
# 4️⃣ Train-test split
# ----------------------------
X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.2, random_state=42, stratify=y
)

# ----------------------------
# 5️⃣ Preprocessing pipeline
# ----------------------------
preprocessor = ColumnTransformer(
    transformers=[
        ("num", StandardScaler(), NUM_FEATURES),
        ("cat", LabelEncodeTransformer(), CAT_FEATURES)
    ]
)

# ----------------------------
# 6️⃣ Full pipeline with SMOTE (training only)
# ----------------------------
from imblearn.pipeline import Pipeline as ImbPipeline

pipeline_train = ImbPipeline([
    ("user_features", UserFeatureEngineer()),
    ("preprocessing", preprocessor),
    ("smote", SMOTE(random_state=42)),
    ("classifier", XGBClassifier(
        n_estimators=500, max_depth=5, learning_rate=0.1, scale_pos_weight=1, use_label_encoder=False, eval_metric='logloss'
    ))
])

# ----------------------------
# 7️⃣ Train model
# ----------------------------
pipeline_train.fit(X_train, y_train)

# ----------------------------
# 8️⃣ Evaluate
# ----------------------------
y_pred = pipeline_train.predict(X_test)
print(classification_report(y_test, y_pred))

# ----------------------------
# 9️⃣ Save pipeline components for inference (no SMOTE)
# ----------------------------
joblib.dump(pipeline_train.named_steps['user_features'], "user_feature_engineer.pkl")
joblib.dump(pipeline_train.named_steps['preprocessing'], "preprocessor.pkl")
joblib.dump(pipeline_train.named_steps['classifier'], "xgb_model.pkl")
print("Saved user_feature_engineer.pkl, preprocessor.pkl, xgb_model.pkl")


# %%

# %%

# %%

# %%

# %%

# %%

# %%

# %%

# %%

# %%

# %%
