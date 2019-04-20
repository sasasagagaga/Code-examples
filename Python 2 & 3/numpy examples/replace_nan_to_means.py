import numpy as np


def replace_nan_to_means(X):
    bad = np.all(np.isnan(X), axis=0)
    X[:, bad] = 0

    y = np.nanmean(X, axis=0)
    y = np.broadcast_to(y, X.shape)

    t = np.isnan(X)
    X[t] = y[t]

    return X
