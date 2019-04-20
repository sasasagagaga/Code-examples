import numpy as np


def grad_finite_diff(function, w, eps=1e-8):
    """
    Возвращает численное значение градиента, подсчитанное по следующией формуле:
        result_i := (f(w + eps * e_i) - f(w)) / eps,
        где e_i - следующий вектор:
        e_i = (0, 0, ..., 0, 1, 0, ..., 0)
                          >> i <<
    """
    res = np.empty(w.shape)
    fw = function(w)
    for coords, _ in np.ndenumerate(w):
        w[coords] += eps
        res[coords] = (function(w) - fw) / eps
        w[coords] -= eps
    return res
