import pandas as pd
import numpy as np
from sklearn.svm import OneClassSVM
from sklearn.preprocessing import StandardScaler
import json
import os

def train_user_model(user_id, csv_file='./build/Desktop_Qt_6_10_0_MinGW_64_bit-Debug/data/biometric_data.csv'):
    """Train One-Class SVM for a user using unified biometric data"""
    
    print(f"\n{'='*60}")
    print(f"Training model for: {user_id}")
    print(f"{'='*60}")
    
    # Load data
    df = pd.read_csv(csv_file)
    user_data = df[df['user_id'] == user_id]
    
    print(f"Total sessions in CSV: {len(df)}")
    print(f"Sessions for {user_id}: {len(user_data)}")
    
    if len(user_data) < 10:
        print(f"⚠️  WARNING: Only {len(user_data)} sessions. Need at least 10 for reliable training!")
        if len(user_data) < 5:
            print("❌ Insufficient data. Skipping.")
            return None
    
    # Feature columns (must match BiometricSnapshot::toFeatureVector())
    feature_cols = [
        'avg_dwell_time',
        'std_dwell_time',
        'avg_flight_time',
        'std_flight_time',
        'typing_speed',
        'backspace_rate',
        'otp_response_time',
        'current_amount',
        'amount_vs_avg_last3',
        'time_since_last_txn',
        'is_known_device',
        'geo_distance_km',
        'current_hour_of_day',
        'otp_attempts_count'
    ]
    
    # Extract features
    X = user_data[feature_cols].values
    
    # Handle missing values
    X = np.nan_to_num(X, nan=0.0)
    
    print(f"\nFeature matrix shape: {X.shape}")
    print(f"Feature statistics:")
    print(f"  Mean typing speed: {X[:, 4].mean():.2f} chars/sec")
    print(f"  Mean OTP time: {X[:, 6].mean():.2f} seconds")
    print(f"  Mean amount: ${X[:, 7].mean():.2f}")
    
    # Normalize features
    scaler = StandardScaler()
    X_scaled = scaler.fit_transform(X)
    
    # Train One-Class SVM
    print("\n🧠 Training One-Class SVM...")
    model = OneClassSVM(
        kernel='rbf',
        gamma='auto',
        nu=0.15  # Expected outlier rate (15%)
    )
    
    model.fit(X_scaled)
    print("✅ Model trained!")
    
    # Extract model parameters for C++
    model_params = {
        'user_id': user_id,
        'n_samples': int(len(X)),
        'n_features': len(feature_cols),
        'feature_names': feature_cols,
        'scaler_mean': scaler.mean_.tolist(),
        'scaler_std': scaler.scale_.tolist(),
        'support_vectors': model.support_vectors_.tolist(),
        'dual_coef': model.dual_coef_.tolist(),
        'intercept': float(model.intercept_[0]),
        'gamma': float(model._gamma)
    }
    
    # Save model
    os.makedirs('./build/Desktop_Qt_6_10_0_MinGW_64_bit-Debug/data/models', exist_ok=True)
    output_file = f'./build/Desktop_Qt_6_10_0_MinGW_64_bit-Debug/data/models/{user_id}_model.json'
    
    with open(output_file, 'w') as f:
        json.dump(model_params, f, indent=2)
    
    print(f"💾 Model saved to: {output_file}")
    
    # Evaluate on training data
    predictions = model.predict(X_scaled)
    n_anomalies = (predictions == -1).sum()
    anomaly_rate = n_anomalies / len(predictions)
    
    print(f"\n📊 Training Statistics:")
    print(f"  Detected anomalies: {n_anomalies}/{len(predictions)} ({anomaly_rate:.1%})")
    print(f"  Support vectors: {len(model.support_vectors_)}")
    
    return model_params

def train_all_users(csv_file='./build/Desktop_Qt_6_10_0_MinGW_64_bit-Debug/data/biometric_data.csv'):
    """Train models for all users"""
    
    if not os.path.exists(csv_file):
        print(f"❌ Error: CSV file not found: {csv_file}")
        print("Please collect training data first!")
        return
    
    df = pd.read_csv(csv_file)
    users = df['user_id'].unique()
    
    print(f"\n🎯 Found {len(users)} unique users in dataset")
    print(f"Users: {', '.join(users)}\n")
    
    successful = 0
    for user_id in users:
        result = train_user_model(user_id, csv_file)
        if result is not None:
            successful += 1
    
    print(f"\n{'='*60}")
    print(f"✅ Training complete!")
    print(f"Successfully trained: {successful}/{len(users)} models")
    print(f"{'='*60}\n")

if __name__ == '__main__':
    print("\n" + "="*60)
    print("🔐 Keystroke Biometrics Model Training Pipeline")
    print("="*60)
    
    train_all_users()