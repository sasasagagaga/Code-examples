import numpy as np


def get_max_before_zero(x):
    y = np.roll(x == 0, 1)
    y[0] = False

    if x[y].size > 0:
        return x[y].max()
