import numpy as np
import scipy
from scipy.special import logsumexp, expit
from scipy.sparse import csr_matrix


class BaseSmoothOracle:
    """
    Базовый класс для реализации оракулов.
    """

    def func(self, w):
        """
        Вычислить значение функции в точке w.
        """
        raise NotImplementedError('Func oracle is not implemented.')

    def grad(self, w):
        """
        Вычислить значение градиента функции в точке w.
        """
        raise NotImplementedError('Grad oracle is not implemented.')


class BinaryLogistic(BaseSmoothOracle):
    """
    Оракул для задачи двухклассовой логистической регрессии.

    Оракул должен поддерживать l2 регуляризацию.
    """

    def __init__(self, l2_coef=1.):
        """
        Задание параметров оракула.

        l2_coef - коэффициент l2 регуляризации
        """
        self.l2_coef = l2_coef

    def func(self, X, y, w):
        """
        Вычислить значение функционала в точке w на выборке X с ответами y.

        X - scipy.sparse.csr_matrix или двумерный numpy.array

        y - одномерный numpy array

        w - одномерный numpy array
        """
        if not isinstance(X, np.ndarray) and not isinstance(X, csr_matrix):
            raise TypeError('X should be numpy array or csr_matrix')

        loss = np.logaddexp(0, -y * X.dot(w)).mean()
        reg = self.l2_coef / 2 * (w ** 2).sum()
        return loss + reg

    def grad(self, X, y, w):
        """
        Вычислить градиент функционала в точке w на выборке X с ответами y.

        X - scipy.sparse.csr_matrix или двумерный numpy.array

        y - одномерный numpy array

        w - одномерный numpy array
        """
        if not isinstance(X, np.ndarray) and not isinstance(X, csr_matrix):
            raise TypeError('X should be numpy array or csr_matrix')

        margins = y * X.dot(w)
        sigmoid = expit(margins)
        reg = self.l2_coef * w

        if isinstance(X, np.ndarray):
            grad = -((y * (1 - sigmoid))[:, None] * X).mean(axis=0)
        elif isinstance(X, csr_matrix):
            grad = np.array(-(X.multiply((y * (1 - sigmoid))[:, None])).mean(axis=0)).ravel()
        return grad + reg


class MulticlassLogistic(BaseSmoothOracle):
    """
    Оракул для задачи многоклассовой логистической регрессии.

    Оракул должен поддерживать l2 регуляризацию.

    w в этом случае двумерный numpy array размера (class_number, d),
    где class_number - количество классов в задаче, d - размерность задачи
    """

    def __init__(self, class_number, l2_coef=1.):  # class_number=-1
        """
        Задание параметров оракула.

        class_number - количество классов в задаче

        l2_coef - коэффициент l2 регуляризации
        """
        self.class_number = class_number
        self.l2_coef = l2_coef

    def func(self, X, y, w):
        """
        Вычислить значение функционала в точке w на выборке X с ответами y.

        X - scipy.sparse.csr_matrix или двумерный numpy.array

        y - одномерный numpy array

        w - двумерный numpy array
        """
        if not isinstance(X, np.ndarray) and not isinstance(X, csr_matrix):
            raise TypeError('X should be numpy array or csr_matrix')

        a = X.dot(w.T)
        z = logsumexp(a, axis=1)
        reg = self.l2_coef / 2 * (w ** 2).sum()
        return -(a[np.arange(X.shape[0]), y] - z).mean() + reg

    def grad(self, X, y, w):
        """
        Вычислить значение функционала в точке w на выборке X с ответами y.

        X - scipy.sparse.csr_matrix или двумерный numpy.array

        y - одномерный numpy array

        w - двумерный numpy array
        """
        if not isinstance(X, np.ndarray) and not isinstance(X, csr_matrix):
            raise TypeError('X should be numpy array or csr_matrix')

        reg = self.l2_coef * w
        a = X.dot(w.T)
        a -= a.max(axis=1, keepdims=True)
        z = np.exp(a)
        sumz = z.sum(axis=1, keepdims=True)
        P = z / sumz
        TMP = np.zeros((w.shape[0], X.shape[0]))
        TMP[y, np.arange(X.shape[0])] = 1

        if isinstance(X, np.ndarray):
            grad = ((TMP - P.T)[:, :, None] * X).mean(axis=1)
        elif isinstance(X, csr_matrix):
            grad = X.multiply((TMP - P.T)[:, :, None]).mean(axis=1)
        return -grad + reg
