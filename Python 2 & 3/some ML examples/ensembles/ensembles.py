import numpy as np
from sklearn.tree import DecisionTreeRegressor
from scipy.optimize import minimize_scalar
from sklearn.metrics import mean_squared_error


rmse = lambda x, y: np.sqrt(mean_squared_error(x, y))


def bootstrap(X, y):
    idx = np.random.randint(0, X.shape[0], X.shape[0])
    return X[idx], y[idx]


def make_sample(X, y, fss, use_bootstrap=True):
    X_cur, y_cur = bootstrap(X, y) if use_bootstrap else (X, y)
    idx = np.random.permutation(X.shape[1])[:int(fss * X.shape[1])]
    X_cur = X_cur[:, idx]
    return X_cur, y_cur, idx


class RandomForestMSE:
    def __init__(self, n_estimators, max_depth=None, feature_subsample_size=None,
                 **trees_parameters):
        """
        n_estimators : int
            The number of trees in the forest.

        max_depth : int
            The maximum depth of the tree. If None then there is no limits.

        feature_subsample_size : float
            The size of feature set for each tree. If None then use recommendations.
        """
        self.n_estimators = n_estimators
        self.max_depth = max_depth
        self.feature_subsample_size = feature_subsample_size
        self.trees_parameters = trees_parameters
        self.regs = []
        self._regs_ftr_permutation = []

    def fit(self, X, y):
        """
        X : numpy ndarray
            Array of size n_objects, n_features

        y : numpy ndarray
            Array of size n_objects
        """
        fss = self.feature_subsample_size or 1 / 3
        for i in range(self.n_estimators):
            X_cur, y_cur, idx = make_sample(X, y, fss, use_bootstrap=True)
            self._regs_ftr_permutation.append(idx)
            reg = DecisionTreeRegressor(**self.trees_parameters, max_depth=self.max_depth,
                                        random_state=179)
            reg.fit(X_cur, y_cur)
            self.regs.append(reg)
        return self

    def predict(self, X):
        """
        X : numpy ndarray
            Array of size n_objects, n_features

        Returns
        -------
        y : numpy ndarray
            Array of size n_objects
        """
        return np.mean([reg.predict(X[:, idx]) for (reg, idx) in zip(self.regs, self._regs_ftr_permutation)], axis=0)


class GradientBoostingMSE:
    def __init__(self, n_estimators, learning_rate=0.1, max_depth=5, feature_subsample_size=None,
                 **trees_parameters):
        """
        n_estimators : int
            The number of trees in the forest.

        learning_rate : float
            Use learning_rate * gamma instead of gamma

        max_depth : int
            The maximum depth of the tree. If None then there is no limits.

        feature_subsample_size : float
            The size of feature set for each tree. If None then use recommendations.
        """
        self.n_estimators = n_estimators
        self.learning_rate = learning_rate
        self.max_depth = max_depth
        self.feature_subsample_size = feature_subsample_size
        self.trees_parameters = trees_parameters
        self.loss = lambda x, y: ((x - y) ** 2).sum() / 2
        self.loss_grad = lambda x, y: y - x
        self.is_fitted = False

    def fit(self, X, y, verbose=True):
        """
        X : numpy ndarray
            Array of size n_objects, n_features

        y : numpy ndarray
            Array of size n_objects
        """
        if self.is_fitted:
            raise AttributeError('Model is already fitted')

        fss = self.feature_subsample_size or 1 / 3

        self.regs = []
        self.gammas = []
        self._regs_ftr_permutation = []

        fit_preds = np.zeros(X.shape[0])
        for i in range(self.n_estimators):
            s = -self.loss_grad(y, fit_preds)
            X_cur, y_cur, idx = make_sample(X, s, fss, use_bootstrap=False)
            self._regs_ftr_permutation.append(idx)
            cur_reg = DecisionTreeRegressor(**self.trees_parameters, max_depth=self.max_depth,
                                            random_state=179)
            cur_reg.fit(X_cur, y_cur)
            cur_preds = cur_reg.predict(X_cur)
            gamma = (y_cur * cur_preds).sum() / (cur_preds ** 2).sum()
            fit_preds += self.learning_rate * gamma * cur_preds
            self.regs.append(cur_reg)
            self.gammas.append(gamma)
            if verbose:
                print(f'iter {i}, train error = {rmse(fit_preds, y):0.3f}')  # Gamma = {gamma:0.3f}
        self.is_fitted = True
        return self

    def predict(self, X):
        """
        X : numpy ndarray
            Array of size n_objects, n_features

        Returns
        -------
        y : numpy ndarray
            Array of size n_objects
        """
        return self.learning_rate * sum(gamma * reg.predict(X[:, idx]) for (reg, gamma, idx) in zip(self.regs, self.gammas, self._regs_ftr_permutation))

