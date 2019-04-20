import numpy as np
from nearest_neighbors import KNNClassifier
from collections import defaultdict


def kfold(n, n_folds):
    base_len = n // n_folds
    cnt_with_1 = n - n_folds * base_len
    res = [None] * n_folds
    sum_len = 0
    cur_range = np.random.permutation(n)
    for i in range(n_folds):
        cur_len = base_len + (i >= n_folds - cnt_with_1)
        res[i] = cur_range[cur_len:], cur_range[:cur_len]
        cur_range = np.roll(cur_range, -cur_len)
    return res


def knn_cross_val_score(X, y, k_list, score='accuracy', cv=None, **kwargs):
    k_list = list(k_list)
    X = X.astype(float)
    y = y.astype(int)
    if cv is None:
        cv = kfold(len(X), 3)

    res = defaultdict(lambda: np.zeros(len(cv), dtype=float))
    for t, (train, test) in enumerate(cv):
        model = KNNClassifier(k=k_list[-1], **kwargs)
        model.fit(X[train], y[train])
        if not kwargs['weights']:
            neighbors = model.find_kneighbors(X[test], return_distance=False)
            weights = np.ones_like(neighbors, dtype=float)
        else:
            dists, neighbors = model.find_kneighbors(X[test],
                                                     return_distance=True)
            weights = 1 / (dists + 1e-5)
        labels = y[train][neighbors]
        for i, (cur_labels, cur_w) in enumerate(zip(labels, weights)):
            cnt = np.zeros(cur_labels.max() + 1)
            maxa, maxy = 0, cur_labels[0]
            for prev_k, cur_k in zip([0] + k_list, k_list):
                for label, w in zip(cur_labels[prev_k:cur_k],
                                    cur_w[prev_k:cur_k]):
                    cnt[label] += w
                    if cnt[label] > maxa:
                        maxa, maxy = cnt[label], label
                res[cur_k][t] += (maxy == y[test][i])
        for k in k_list:
            res[k][t] /= len(test)
    return dict(res)
