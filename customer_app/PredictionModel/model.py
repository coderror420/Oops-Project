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

# %%
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
df = pd.read_csv("data.csv")

TARGET = "Fraud_Label"


# %%
CAT_COLS = ['Transaction_Type', 'Device_Type',
       'Location', 'Merchant_Category', 'Authentication_Method']

DROP_COLS = ["Transaction_ID", "User_ID"]  # not used for training


# %%
CAT_COLS


# %%
class MultiColumnLabelEncoder(BaseEstimator, TransformerMixin):
    def __init__(self, columns):
        self.columns = columns
        self.encoders = {}

    def fit(self, X, y=None):
        for col in self.columns:
            le = LabelEncoder()
            le.fit(X[col].astype(str))
            self.encoders[col] = le
        return self

    def transform(self, X):
        X = X.copy()
        for col, le in self.encoders.items():
            X[col] = le.transform(X[col].astype(str))
        return X



# %%
X = df.drop(columns=[TARGET] + DROP_COLS)
y = df[TARGET]

X_train, X_test, y_train, y_test = train_test_split(
    X, y,
    test_size=0.2,
    stratify=y,
    random_state=0
)


# %%
train_pipeline = Pipeline(steps=[
    ("label_encode", MultiColumnLabelEncoder(CAT_COLS)),
    ("scaler", StandardScaler()),
    ("smote", SMOTE(random_state=42)),
    ("model", XGBClassifier(
        n_estimators=300,
        max_depth=6,
        learning_rate=0.05,
        subsample=0.8,
        colsample_bytree=0.8,
        scale_pos_weight=len(y_train[y_train==0]) / len(y_train[y_train==1]),
        eval_metric="logloss",
        random_state=42
    ))
])



# %%
train_pipeline.fit(X_train, y_train)

y_pred = train_pipeline.predict(X_test)
print(classification_report(y_test, y_pred))


# %%
inference_pipeline = Pipeline(steps=[
    ("label_encode", train_pipeline.named_steps["label_encode"]),
    ("scaler", train_pipeline.named_steps["scaler"]),
    ("model", train_pipeline.named_steps["model"])
])

joblib.dump(inference_pipeline, "model.pkl")
print("model.pkl")


# %%
# After training
y_test_pred = inference_pipeline.predict(X_test)
y_test_prob = inference_pipeline.predict_proba(X_test)[:, 1]

from sklearn.metrics import classification_report, roc_auc_score

print(classification_report(y_test, y_test_pred))
print("ROC AUC:", roc_auc_score(y_test, y_test_prob))


# %%
y_test_prob

# %%

# %%

# %%

# %%

# %%
