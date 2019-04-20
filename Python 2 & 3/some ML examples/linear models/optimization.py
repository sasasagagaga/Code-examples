import numpy as np
import time
import oracles
from scipy.special import expit


class GDClassifier:
    """
    Реализация метода градиентного спуска для произвольного
    оракула, соответствующего спецификации оракулов из модуля oracles.py
    """
    
    def __init__(self, loss_function, step_alpha=1, step_beta=1,
                 tolerance=1e-5, max_iter=1000, **kwargs):
        """
        loss_function - строка, отвечающая за функцию потерь классификатора. 
        Может принимать значения:
        - 'binary_logistic' - бинарная логистическая регрессия
        - 'multinomial_logistic' - многоклассовая логистическая регрессия

        step_alpha - float, параметр выбора шага из текста задания

        step_beta- float, параметр выбора шага из текста задания

        tolerance - точность, по достижении которой, необходимо прекратить оптимизацию.
        Необходимо использовать критерий выхода по модулю разности соседних значений функции:
        если (f(x_{k+1}) - f(x_{k})) < tolerance: то выход 

        max_iter - максимальное число итераций     

        **kwargs - аргументы, необходимые для инициализации   
        """
        self.loss_function = loss_function
        if loss_function == 'multinomial_logistic':
            self.loss = oracles.MulticlassLogistic(**kwargs)
        else:
            self.loss = oracles.BinaryLogistic(**kwargs)
        self.alpha = step_alpha
        self.beta = step_beta
        self.tolerance = tolerance
        self.max_iter = max_iter
        self.kwargs = kwargs

    def fit(self, X, y, w_0='random', trace=False, *, x_test=None, y_test=None):  #, predict_log_freq=5):
        """
        Обучение метода по выборке X с ответами y

        X - scipy.sparse.csr_matrix или двумерный numpy.array

        y - одномерный numpy array

        w_0 - начальное приближение в методе (если w_0='zeros', то заполняется нулями,
        если w_0='random', то случайное приближение)

        trace - переменная типа bool

        Если trace = True, то метод должен вернуть словарь history, содержащий информацию 
        о поведении метода. Длина словаря history = количество итераций + 1 (начальное приближение)

        history['time']: list of floats, содержит интервалы времени между двумя итерациями метода
        history['func']: list of floats, содержит значения функции на каждой итерации
        (0 для самой первой точки)
        """
        if self.loss_function == 'multinomial_logistic':
            self.classes_ = np.arange(self.loss.class_number, dtype=int)  # w_0.shape[0]
            w_0_shape = (self.loss.class_number, X.shape[1])
        else:
            self.classes_ = np.array([-1, 1])
            w_0_shape = (X.shape[1], )
        if isinstance(w_0, str):
            if w_0 == 'zeros':
                w_0 = np.zeros(w_0_shape)
            elif w_0 == 'random':
                w_0 = np.random.randn(*w_0_shape) / 100
        elif not isinstance(w_0, np.ndarray):
            raise TypeError('w_0 is incorrect')
        self.w = w = w_0

        f_prev = self.loss.func(X, y, w)

        if trace:
            history = {'func': [f_prev], 'time': [0]}
            time_prev = time.time()
            if x_test is not None:
                history['score'] = [0]

        for k in range(1, self.max_iter + 1):
            grad = self.loss.grad(X, y, w)
            eta = self.alpha / k ** self.beta
            w -= eta * grad
            f_cur = self.loss.func(X, y, w)

            if trace:
                history['func'].append(f_cur)
                history['time'].append(time.time() - time_prev)
                if x_test is not None:
                    history['score'].append(self.score(x_test, y_test))
                time_prev = time.time()

            if abs(f_cur - f_prev) < self.tolerance:
                break
            f_prev = f_cur
        self.n_iters_ = k + 1

        if trace:
            return history

    def predict(self, X):
        """
        Получение меток ответов на выборке X

        X - scipy.sparse.csr_matrix или двумерный numpy.array

        return: одномерный numpy array с предсказаниями
        """
        if self.loss_function == 'multinomial_logistic':
            return self.classes_[np.argmax(X.dot(self.w.T), axis=1)]
        else:
            return self.classes_[(X.dot(self.w) >= 0).astype(int, copy=False)]

    def score(self, X, y):
        return (self.predict(X) == y).mean()

    def predict_proba(self, X):
        """
        Получение вероятностей принадлежности X к классу k

        X - scipy.sparse.csr_matrix или двумерный numpy.array

        return: двумерной numpy array, [i, k] значение соответветствует вероятности
        принадлежности i-го объекта к классу k 
        """
        if self.loss_function == 'multinomial_logistic':
            a = X.dot(w.T)
            a -= a.max(axis=1, keepdims=True)
            z = np.exp(a)
            sumz = z.sum(axis=1, keepdims=True)
            P = z / sumz
            return P
        else:
            probas = expit(X.dot(self.w))
            return np.vstack((probas, 1 - probas)).T

    def get_objective(self, X, y):
        """
        Получение значения целевой функции на выборке X с ответами y

        X - scipy.sparse.csr_matrix или двумерный numpy.array
        y - одномерный numpy array

        return: float
        """
        return self.loss.func(X, y, self.w)

    def get_gradient(self, X, y):
        """
        Получение значения градиента функции на выборке X с ответами y

        X - scipy.sparse.csr_matrix или двумерный numpy.array
        y - одномерный numpy array

        return: numpy array, размерность зависит от задачи
        """
        return self.loss.grad(X, y, self.w)

    def get_weights(self):
        """
        Получение значения весов функционала
        """
        return self.w

class SGDClassifier(GDClassifier):
    """
    Реализация метода стохастического градиентного спуска для произвольного
    оракула, соответствующего спецификации оракулов из модуля oracles.py
    """

    def __init__(self, loss_function, batch_size=1, step_alpha=1, step_beta=1,
                 tolerance=1e-5, max_iter=1000, random_seed=153, **kwargs):
        """
        loss_function - строка, отвечающая за функцию потерь классификатора. 
        Может принимать значения:
        - 'binary_logistic' - бинарная логистическая регрессия
        - 'multinomial_logistic' - многоклассовая логистическая регрессия

        batch_size - размер подвыборки, по которой считается градиент

        step_alpha - float, параметр выбора шага из текста задания

        step_beta- float, параметр выбора шага из текста задания

        tolerance - точность, по достижении которой, необходимо прекратить оптимизацию
        Необходимо использовать критерий выхода по модулю разности соседних значений функции:
        если (f(x_{k+1}) - f(x_{k})) < tolerance: то выход

        max_iter - максимальное число итераций

        random_seed - в начале метода fit необходимо вызвать np.random.seed(random_seed).
        Этот параметр нужен для воспроизводимости результатов на разных машинах.

        **kwargs - аргументы, необходимые для инициализации
        """
        self.loss_function = loss_function
        if loss_function == 'multinomial_logistic':
            self.loss = oracles.MulticlassLogistic(**kwargs)
        else:
            self.loss = oracles.BinaryLogistic(**kwargs)
        self.batch_size = batch_size
        self.alpha = step_alpha
        self.beta = step_beta
        self.tolerance = tolerance
        self.max_iter = max_iter
        self.random_seed = random_seed
        self.kwargs = kwargs

    def fit(self, X, y, w_0='random', trace=False, log_freq=1, *, x_test=None, y_test=None):
        """
        Обучение метода по выборке X с ответами y

        X - scipy.sparse.csr_matrix или двумерный numpy.array

        y - одномерный numpy array

        w_0 - начальное приближение в методе (если w_0='zeros', то заполняется нулями,
        если w_0='random', то случайное приближение)

        Если trace = True, то метод должен вернуть словарь history, содержащий информацию 
        о поведении метода. Если обновлять history после каждой итерации, метод перестанет 
        превосходить в скорости метод GD. Поэтому, необходимо обновлять историю метода лишь
        после некоторого числа обработанных объектов в зависимости от приближённого номера эпохи.
        Приближённый номер эпохи:
            {количество объектов, обработанных методом SGD} / {количество объектов в выборке}

        log_freq - float от 0 до 1, параметр, отвечающий за частоту обновления. 
        Обновление должно проиходить каждый раз, когда разница между двумя значениями приближённого номера эпохи
        будет превосходить log_freq.

        history['epoch_num']: list of floats, в каждом элементе списка будет записан приближённый номер эпохи:
        history['time']: list of floats, содержит интервалы времени между двумя соседними замерами
        history['func']: list of floats, содержит значения функции после текущего приближённого номера эпохи
        history['weights_diff']: list of floats, содержит квадрат нормы разности векторов весов с соседних замеров
        (0 для самой первой точки)
        """
        np.random.seed(self.random_seed)

        if self.loss_function == 'multinomial_logistic':
            self.classes_ = np.arange(self.loss.class_number, dtype=int)  # w_0.shape[0]
            w_0_shape = (self.loss.class_number, X.shape[1])
        else:
            self.classes_ = np.array([-1, 1])
            w_0_shape = (X.shape[1], )
        if isinstance(w_0, str):
            if w_0 == 'zeros':
                w_0 = np.zeros(w_0_shape)
            elif w_0 == 'random':
                w_0 = np.random.randn(*w_0_shape) / 100
        elif not isinstance(w_0, np.ndarray):
            raise TypeError('w_0 is incorrect')
        self.w = w = w_0

        f_prev = self.loss.func(X, y, w)

        if trace:
            history = {'func': [f_prev], 'time': [0], 'weights_diff': [0.], 'epoch_num': [0.]}
            time_prev = time.time()
            prev_epoch_num = 0
            if x_test is not None:
                history['score'] = [0]


        num_obj_watched = 0  # количество просмотренных объектов
        N = X.shape[0]
        ind = N
        for k in range(1, self.max_iter + 1):
            # если осталось меньше batch_size / 2 непросмотренныхобъектов в выборке
            if ind >= N - self.batch_size / 2:
                perm = np.random.permutation(N)
                ind = 0
            idxs = perm[ind:ind + self.batch_size]
            num_obj_watched += idxs.shape[0]
            ind += self.batch_size

            grad = self.loss.grad(X[idxs], y[idxs], w)
            eta = self.alpha / k ** self.beta
            w -= eta * grad
            f_cur = self.loss.func(X, y, w)
            
            if trace:
                cur_epcoh_num = num_obj_watched / N
                if cur_epcoh_num - prev_epoch_num > log_freq:
                    history['func'].append(f_cur)
                    history['time'].append(time.time() - time_prev)
                    history['epoch_num'].append(cur_epcoh_num)
                    history['weights_diff'].append(np.linalg.norm(eta * grad))
                    prev_epoch_num = cur_epcoh_num
                    if x_test is not None:
                        history['score'].append(self.score(x_test, y_test))
                    time_prev = time.time()

            if abs(f_cur - f_prev) < self.tolerance:
                break
            f_prev = f_cur
        self.n_iters_ = k + 1

        if trace:
            return history
