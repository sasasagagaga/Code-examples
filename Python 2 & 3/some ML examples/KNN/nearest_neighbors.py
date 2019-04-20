import numpy as np
import distances
from sklearn.neighbors import NearestNeighbors


class KNNClassifier:
    def __init__(self, k=2, strategy='my_own', metric='euclidean',
                 weights=False, test_block_size=1000):
        print(metric, weights, strategy, k, test_block_size)
        if not isinstance(weights, bool):
            raise TypeError('weights should be bool')
        self.k = int(k)
        self.strategy = strategy
        self.metric = metric
        self.weights = weights
        self.test_block_size = test_block_size
        self.eps = 1e-5
        if self.strategy == 'my_own':
            if metric == 'euclidean':
                self.dist_func = distances.euclidean_distance
            elif metric == 'cosine':
                self.dist_func = distances.cosine_distance
            else:
                raise ValueError('unrecognized metric: {}'.format(metric))
        elif strategy in ['brute', 'kd_tree', 'ball_tree']:
            self.model = NearestNeighbors(n_neighbors=self.k,
                                          algorithm=self.strategy,
                                          metric=self.metric)
        else:
            raise ValueError('unrecognized strategy: {}'.format(strategy))

    def fit(self, X, y):
        if self.strategy == 'my_own':
            self.X = X.astype(float)
        else:
            self.model.fit(X, y)
        self.y = y.astype(int)

    def _find_kneighbors(self, X, return_distance):
        if self.strategy != 'my_own':
            return self.model.kneighbors(X, return_distance=return_distance)
        dists = self.dist_func(X, self.X)
        neighbors = np.argsort(dists, axis=1)[:, :self.k]
        if return_distance:
            return np.sort(dists, axis=1)[:, :self.k], neighbors
        return neighbors
    
    def find_kneighbors(self, X, return_distance):
        dists = np.empty((len(X), self.k), dtype=float)
        neighbors = np.empty_like(dists, dtype=int)
        for i in range((len(X) + self.test_block_size - 1) // self.test_block_size):
            start = i * self.test_block_size
            end = start + self.test_block_size
            if return_distance:
                dists[start:end], neighbors[start:end] = self._find_kneighbors(X[start:end], return_distance)
            else:
                neighbors[start:end] = self._find_kneighbors(X[start:end], return_distance)
        if return_distance:
            return dists, neighbors
        return neighbors

    def predict(self, X):
        X = X.astype(float)
        predictions = np.empty(len(X), dtype=int)
        if self.weights:
            dists, neighbors = self.find_kneighbors(X, return_distance=True)
            for i, (row, dist) in enumerate(zip(neighbors, dists)):
                predictions[i] = np.bincount(self.y[row], weights=1 /
                                             (dist + self.eps)).argmax()
        else:
            neighbors = self.find_kneighbors(X, return_distance=False)
            for i, row in enumerate(neighbors):
                predictions[i] = np.bincount(self.y[row]).argmax()
        return predictions
