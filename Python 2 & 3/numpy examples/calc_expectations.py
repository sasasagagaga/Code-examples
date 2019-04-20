import numpy as np


def calc_expectations(h, w, X, Q):
    Q = np.cumsum(np.cumsum(Q, axis=0), axis=1)
    Q[:, w:] -= Q[:, :-w]
    Q[h:, :] -= Q[:-h, :]
    return X * Q
