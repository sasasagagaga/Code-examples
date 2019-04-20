import numpy as np


def get_nonzero_diag_product(X):
    X = np.diag(X)
    X = X[X != 0]
    if X.shape[0] > 0:
        return np.prod(X)
