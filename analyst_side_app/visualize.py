import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os
import sys

# Set style
sns.set(style="whitegrid")
trans_path = "./build/Desktop_Qt_6_10_0_MinGW_64_bit-Debug/data/transaction_data.csv"
bio_path = "./build/Desktop_Qt_6_10_0_MinGW_64_bit-Debug/data/biometric_data.csv"
def clean_columns(df):
    df.columns = df.columns.str.strip()
    return df
def find_flag_column(df):
    candidates = ['is_flagged', 'Is_Flagged', 'fraud', 'class']
    for col in df.columns:
        if col in candidates: return col
        if 'flag' in col.lower(): return col
    return None
def generate_graphs():
    
    
    df_trans = pd.DataFrame()
    df_bio = pd.DataFrame()

    # --- GRAPH 1 ---
    if os.path.exists(trans_path):
        try:
            df_trans = pd.read_csv(trans_path)
            df_trans = clean_columns(df_trans)
            flag_col = find_flag_column(df_trans)
            
            plt.figure(figsize=(10, 6))
            if 'Hour' in df_trans.columns and 'Transaction_Amount' in df_trans.columns:
                sns.scatterplot(data=df_trans, x='Hour', y='Transaction_Amount', 
                                hue=flag_col if flag_col else None, style=flag_col if flag_col else None,
                                palette={0: 'green', 1: 'red'} if flag_col else None, s=100)
                plt.title('Transaction Analysis')
            plt.tight_layout()
            plt.savefig("graph1.png")
            plt.close()
        except Exception: pass

    # --- GRAPH 2 ---
    if os.path.exists(bio_path):
        try:
            df_bio = pd.read_csv(bio_path)
            df_bio = clean_columns(df_bio)
            flag_col = find_flag_column(df_bio)
            
            plt.figure(figsize=(10, 6))
            if 'typing_speed' in df_bio.columns and 'otp_response_time' in df_bio.columns:
                sns.scatterplot(data=df_bio, x='typing_speed', y='otp_response_time', 
                                hue=flag_col if flag_col else None,
                                palette={0: 'blue', 1: 'orange'} if flag_col else None, s=100)
                plt.title('Biometric Analysis')
            plt.tight_layout()
            plt.savefig("graph2.png")
            plt.close()
        except Exception: pass

if __name__ == "__main__":
    generate_graphs()