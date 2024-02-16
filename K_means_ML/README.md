# k-means clustering #

K-means is a clustering method, which aims to partition a set of n observations into k groups in which each observation belongs to the group whose mean value is closest. It is a method used in data mining.

## The data set we are going to use ##

We are going to use the (https://archive.ics.uci.edu/ml/datasets/iris "Iris") data set which is one of the earliest datasets used in the literature on classification methods and widely used in statistics and machine learning.  The data set contains 3 classes of 50 instances each, where each class refers to a type of iris plant. One class is linearly separable from the other 2; the latter are not linearly separable from each other.

### What do the instances in this dataset represent? ###

We are working with de data of the plants instead of the species.

### What is the idea of the project? ###

This project will provide you with a solid understanding of the K-Means algorithm and the concept of data clustering in general in the context of unsupervised learning. Using ML in C where we will learn about non-supervised learning.

### The process ###

**Centroid Initialization:** The algorithm begins by randomly selecting “K” points from the data set as initial centroids. These centroids represent the centers of the groups.

**Assignment of Points to Centroids:** For each point in the data set, the distance between that point and each of the centroids is calculated. The point is assigned to the nearest centroid, which creates "K" groups.

**Centroid Recalculation:** After assigning all points to centroids, the position of each centroid is recalculated as the average of all samples assigned to it.

**Iteration:** Steps 2 and 3 are repeated iteratively until some convergence criterion is met, such as stability of centroids or a maximum number of iterations.