# Smart Transaction Fraud Detection System

## CS 207 - Object-Oriented Programming Concepts

A financial fraud detection system combining machine learning-based transaction analysis with behavioral biometric verification to identify fraudulent financial activities in real-time.

---

## Project Deliverables

1. **Video Demonstration**: https://drive.google.com/file/d/15ofEOsJfSjx5c4aErwlPBi8aHb_EZb8F/view?usp=drive_link
2. **Project Documentation (Word)**: https://docs.google.com/document/d/1H1ojiO3ueEvIRD8pAxLTPMuoz5jItatq/edit?usp=drive_link&ouid=110747621292753602584&rtpof=true&sd=true
3. **Project Documentation (PDF)**: https://drive.google.com/file/d/1XP1m_dxHs7P-1iPN0bLUV5nlcvkt80n6/view?usp=drive_link
4. **Project Presentation**: https://drive.google.com/file/d/1vtfQEOQcVxfwcS1I-_V36S4j1rdEX6sm/view?usp=drive_link

---

## Table of Contents

1. [Overview](#overview)
2. [Features](#features)
3. [System Architecture](#system-architecture)
4. [Technology Stack](#technology-stack)
5. [Directory Structure](#directory-structure)
6. [Prerequisites](#prerequisites)
7. [Installation and Setup](#installation-and-setup)
8. [Running the Application](#running-the-application)
9. [How It Works](#how-it-works)
10. [Future Enhancements](#future-enhancements)
11. [Contributors](#contributors)
12. [References](#references)

---

## Overview

With the rapid growth of online payment systems, financial fraud has become increasingly sophisticated. This project presents a **Smart Transaction Fraud Detection System** that integrates machine learning, behavioral biometrics, and real-time transaction analysis to assess the risk associated with financial transactions.

The system consists of two applications:
- **Customer-Side Application**: Simulates online payment interactions and performs real-time fraud detection
- **Analyst Dashboard**: Provides visualization and analysis of flagged transactions and behavioral patterns

---

## Features

### **Dual-Layer Fraud Detection**
- **Transaction Analysis**: Evaluates financial attributes using XGBoost model
- **Behavioral Biometrics**: Verifies user identity through keystroke dynamics using One-Class SVM

### **Real-Time Processing**
- Transaction analysis: < 100 ms
- Biometric analysis: < 50 ms
- Total evaluation time: < 200 ms

### **Keystroke Dynamics**
- Captures dwell time, flight time, typing rhythm, and statistical measures
- Privacy-preserving: only timing data stored, no actual characters logged

### **Continuous Learning**
- All transactions and sessions logged in CSV format
- Periodic model retraining for improved accuracy

### **Comprehensive Analytics**
- Dashboard visualization of suspicious activities
- Historical transaction and biometric data analysis
- Real-time fraud scoring and flagging

---

## System Architecture

### **User Interface Layer**
- Customer Application: Payment simulation interface
- Analyst Dashboard: Risk analysis and visualization interface

### **Application Layer**
- UI Event Handling
- Transaction & Biometric Class Management
- CSV Logger Module

### **Fraud Detection Layer**
- **Transaction-Based Analysis**: General transaction features (amount, merchant, location, device, temporal patterns)
- **Behavioral Biometrics**: User-specific keystroke dynamics and OTP response patterns

---

## Technology Stack

### **Frontend**
- Qt 6 Widgets
- Qt Designer
- Custom keystroke capture widgets

### **Backend**
- C++17
- Qt Core
- CSV/JSON data storage

### **Machine Learning**
- Python 3.x
- scikit-learn
- XGBoost
- pandas
- NumPy
- imbalanced-learn
- joblib

### **Visualization**
- matplotlib
- seaborn

---

## Directory Structure

```
Oops-Project/
│
├── customer_app/
│   ├── build/Desktop_Qt_6_x_x_MinGW_64_bit-Debug/
│   │   ├── data/
│   │   │   ├── models/
│   │   │   │   └── userXX.json
│   │   │   ├── biometric_data.csv
│   │   │   ├── transaction_data.csv
│   │   │   ├── graph1.png
│   │   │   └── graph2.png
│   │   ├── predict.py
│   │   └── xgb_model.json
│   ├── CMakeLists.txt
│   ├── mainwindow.cpp
│   ├── mainwindow.h
│   └── ...
│
├── analyst_side_app/
│   ├── build/Desktop_Qt_6_x_x_MinGW_64_bit-Debug/
│   │   ├── data/
│   │   │   ├── biometric_data.csv
│   │   │   ├── transaction_data.csv
│   │   │   ├── graph1.png
│   │   │   └── graph2.png
│   │   └── generate_graphs.py
│   ├── CMakeLists.txt
│   ├── mainwindow.cpp
│   ├── mainwindow.h
│   └── ...
│
└── README.md
```

---

## Prerequisites

### **Software Requirements**
- Qt Creator 6.x or higher
- Qt 6.x framework
- Python 3.8 or higher
- MinGW 64-bit compiler (for Windows)
- Git

### **Hardware Requirements**
- Minimum 4GB RAM
- 2GB free disk space

---

## Installation and Setup

### **Step 1: Clone the Repository**

```bash
git clone https://github.com/coderror420/Oops-Project.git
cd Oops-Project
```

### **Step 2: Setup Analyst Side Application**

```bash
cd analyst_side_app
python -m venv venv
```

**On Windows (PowerShell):**
```powershell
.\venv\Scripts\activate.ps1
```

**On Linux/Mac:**
```bash
source venv/bin/activate
```

**Install Dependencies:**
```bash
pip install matplotlib seaborn pandas
```

### **Step 3: Setup Customer Side Application**

```bash
cd ../customer_app
python -m venv venv
```

**On Windows (PowerShell):**
```powershell
.\venv\Scripts\activate.ps1
```

**On Linux/Mac:**
```bash
source venv/bin/activate
```

**Install Dependencies:**
```bash
pip install numpy pandas scikit-learn imbalanced-learn joblib xgboost
```

### **Step 4: Backup Build Folders (Important)**

Before opening the project in Qt Creator:
1. Navigate to each application folder (customer_app and analyst_side_app)
2. Copy the `build/` folder to a safe location outside the project directory
3. Delete the `build/` folder from the project directory

This prevents Qt Creator from using cached build configurations.

### **Step 5: Configure Qt Creator Projects**

**For Customer App:**
1. Open Qt Creator
2. File → Open File or Project
3. Navigate to `customer_app/CMakeLists.txt`
4. Configure the project with MinGW 64-bit kit
5. Build → Clean All
6. Build → Build Project

**For Analyst Dashboard:**
1. File → Open File or Project
2. Navigate to `analyst_side_app/CMakeLists.txt`
3. Configure the project with MinGW 64-bit kit
4. Build → Clean All
5. Build → Build Project

### **Step 6: Restore Build Folders**

After successful build:
1. Copy the backed-up `build/Desktop_Qt_6_x_x_MinGW_64_bit-Debug/` folder
2. Paste it into the newly created build directory
3. Ensure `data/` folder and Python scripts are present in the build output directory

---

## Running the Application

### **Customer-Side Application**
1. Open Qt Creator
2. Open the customer_app project
3. Ensure virtual environment is activated
4. Run the application (Ctrl+R or Play button)
5. Perform transactions to test fraud detection

### **Analyst Dashboard**
1. Open Qt Creator
2. Open the analyst_side_app project
3. Ensure virtual environment is activated
4. Run the application (Ctrl+R or Play button)
5. View dashboard with three modes:
   - **Dashboard View**: Shows flagged suspicious activities
   - **Biometric View**: Displays all biometric history
   - **Transaction View**: Shows all transaction history

---

## How It Works

### **Transaction Flow**

1. **Transaction Initiation**: User enters payment details
2. **Biometric Capture**: Keystroke dynamics recorded during OTP entry
3. **Feature Extraction**: Transaction and biometric features prepared
4. **ML Inference**: Dual-model prediction (transaction + biometric)
5. **Decision Fusion**: Combined fraud score calculation
6. **Response**: Transaction approved or flagged
7. **Logging**: All data logged for analysis and retraining

### **Fraud Detection Models**

**Transaction Model (XGBoost):**
- Analyzes: Amount, merchant, frequency, location, device, temporal patterns
- Output: Probability score for transaction fraud

**Biometric Model (One-Class SVM):**
- Analyzes: Dwell time, flight time, typing speed, pause patterns
- Output: User authenticity score

### **Decision Fusion**
- Weighted combination of both model outputs
- Transactions exceeding threshold flagged as suspicious
- Adaptive thresholds based on transaction risk level

---

## Future Enhancements

### **Scalability**
- Replace command-line integration with microservices
- Support high-volume transaction processing

### **Additional Biometrics**
- Mouse movement dynamics
- Touch gestures
- Navigation patterns

### **Advanced Model Optimization**
- Online learning and incremental updates
- Deep learning architectures
- Ensemble methods

### **Explainable AI**
- XAI techniques for transparent fraud reasoning
- Improved regulatory compliance

### **Production Features**
- Database-backed storage (SQLite/PostgreSQL)
- Banking API integration
- Multi-user support
- Multilingual interface

---

## Contributors

- **Mannan Bajaj**
- **Kanishka Pandey**

**Course**: CS 207 - Object-Oriented Programming Concepts

---

## References

1. Kaggle Dataset - [Fraud Detection Transactions Dataset](https://www.kaggle.com/datasets/samayashar/fraud-detection-transactions-dataset)
2. Scikit-learn Documentation - [Machine Learning in Python](https://scikit-learn.org)
3. Qt Documentation - [Qt 6 Widgets and Event Handling](https://doc.qt.io)
4. Pandas Documentation - [Data Analysis in Python](https://pandas.pydata.org)

---

## License

This project is developed for academic purposes as part of CS 207 coursework.

---

## Support

For issues or questions, please open an issue on the GitHub repository or contact the project contributors.

---

**Note**: Ensure all Python virtual environments are activated before running either application. The system requires proper directory structure and data files in the build output folder for correct operation.