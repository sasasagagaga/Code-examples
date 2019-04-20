import numpy as np


def encode_rle(x):
    idx = np.append(np.diff(x).nonzero(), x.size - 1)
    return x[idx], np.diff(np.append(-1, idx))
