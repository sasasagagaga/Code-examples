import numpy as np


def euclidean_distance(X, Y):
    return (X ** 2).sum(axis=1, keepdims=True) -\
        2 * X.dot(Y.T) + (Y.T ** 2).sum(axis=0)  # np.sqrt()


def cosine_distance(X, Y):
    return 1 - X.dot(Y.T) / np.sqrt((X ** 2).sum(axis=1, keepdims=True)) / \
        np.sqrt((Y.T ** 2).sum(axis=0))
