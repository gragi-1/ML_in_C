# Machine Learning in C

Welcome to **Machine Learning in C**, a collection of hands-on mini-projects that demonstrate core machine learning concepts using the C programming language. This repository is designed for students, enthusiasts, and anyone interested in understanding how classic ML algorithms work under the hood, without relying on high-level libraries.

---

## 📚 Project Overview

This repository contains three independent mini-projects, each focusing on a different machine learning paradigm:

1. **K-Nearest Neighbors (KNN)** – Supervised Learning (Classification)
2. **K-Means Clustering** – Unsupervised Learning (Clustering)
3. **Tic-Tac-Toe with Q-Learning** – Reinforcement Learning (Game AI)

Each project is self-contained, with its own code, dataset, and results. All implementations are written in C for maximum transparency and educational value.

---

## 🗂️ Project Structure

```
ML_in_C/
├── K_means_ML/           # K-Means clustering implementation
│   ├── iris.data         # Iris dataset
│   ├── k_means_main.c    # Main C source file
│   ├── value_of_assignments_and_clusters.csv # Output results
│   └── README.md         # Project-specific instructions
│
├── KNN_ML/               # K-Nearest Neighbors implementation
│   ├── iris.data         # Iris dataset
│   ├── knn_algorithm_main.c # Main C source file
│   ├── results.csv       # Output results
│   └── README.md         # Project-specific instructions
│
├── Tic-Tac-Toe_ML/       # Tic-Tac-Toe with Q-Learning
│   ├── tic_tac_toe_main.c # Main C source file
│   ├── q_table.csv       # Q-table for the agent
│   ├── a.exe             # Compiled binary (example)
│   └── README.md         # Project-specific instructions
│
└── README.md             # (You are here)
```

---

## 1️⃣ K-Nearest Neighbors (KNN)

- **Type:** Supervised Learning (Classification)
- **Dataset:** [Iris Dataset](https://archive.ics.uci.edu/ml/datasets/iris)
- **Goal:** Classify iris flowers into three species based on sepal/petal measurements.
- **Features:**
  - Reads and normalizes the Iris dataset
  - Implements the KNN algorithm from scratch
  - Outputs predictions and accuracy to `results.csv`
- **Learning Objectives:**
  - Understand distance metrics and nearest neighbor search
  - Learn about data normalization and supervised learning

**How to Run:**
1. Navigate to `KNN_ML/`.
2. Compile: `gcc knn_algorithm_main.c -o knn`
3. Run: `./knn` (or `knn.exe` on Windows)

---

## 2️⃣ K-Means Clustering

- **Type:** Unsupervised Learning (Clustering)
- **Dataset:** [Iris Dataset](https://archive.ics.uci.edu/ml/datasets/iris)
- **Goal:** Cluster iris data into groups without using species labels.
- **Features:**
  - Random centroid initialization
  - Iterative assignment and centroid update
  - Outputs cluster assignments to `value_of_assignments_and_clusters.csv`
- **Learning Objectives:**
  - Understand unsupervised learning and clustering
  - Learn about centroid-based algorithms and convergence

**How to Run:**
1. Navigate to `K_means_ML/`.
2. Compile: `gcc k_means_main.c -o kmeans`
3. Run: `./kmeans` (or `kmeans.exe` on Windows)

---

## 3️⃣ Tic-Tac-Toe with Q-Learning

- **Type:** Reinforcement Learning (Game AI)
- **Goal:** Train an AI agent to play Tic-Tac-Toe using Q-Learning.
- **Features:**
  - Implements Q-Learning from scratch
  - Agent learns by playing games and updating a Q-table
  - Q-table can be saved/loaded (`q_table.csv`)
  - Play against the AI and watch it improve
- **Learning Objectives:**
  - Understand reinforcement learning concepts (states, actions, rewards)
  - Learn about Q-tables and epsilon-greedy policies

**How to Run:**
1. Navigate to `Tic-Tac-Toe_ML/`.
2. Compile: `gcc tic_tac_toe_main.c -o ttt`
3. Run: `./ttt` (or `ttt.exe` on Windows)

---

## 📦 Requirements

- GCC or any C99-compatible compiler
- No external libraries required (standard C only)
- (Optional) Python or spreadsheet software to view CSV results

---

## 🧑‍💻 How to Contribute

Contributions are welcome! Feel free to open issues or submit pull requests for improvements, bug fixes, or new ML mini-projects in C.

---

## 📜 License

This project is open-source and available under the MIT License.

---

## 🙏 Credits

- [UCI Machine Learning Repository – Iris Dataset](https://archive.ics.uci.edu/ml/datasets/iris)
- Inspired by classic ML textbooks and C programming tutorials.

---

## 🚀 Start Learning ML with C!

Explore each folder, read the code, and experiment with the algorithms. This project is a great way to demystify machine learning by building it from scratch in a low-level language.