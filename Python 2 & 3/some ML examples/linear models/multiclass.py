class MulticlassStrategy:   
    def __init__(self, classifier, mode, **kwargs):
        """
        Инициализация мультиклассового классификатора
        
        classifier - базовый бинарный классификатор
        
        mode - способ решения многоклассовой задачи,
        либо 'one_vs_all', либо 'all_vs_all'
        
        **kwargs - параметры классификатор
        """
        self.classifier = classifier
        self.args = kwargs
        self.mode = mode
        
    def fit(self, X, y):
        """
        Обучение классификатора
        """
        self.n_classes = np.unique(y).shape[0]
        if self.mode == 'one_vs_all':
            self.classifiers = [None] * self.n_classes
            for i in range(self.n_classes):
                self.classifiers[i] = self.classifier(**self.args)
                self.classifiers[i].fit(X, 2 * (y == i) - 1)
        else:
            self.classifiers = [[None] * self.n_classes for _ in range(self.n_classes)]
            for s in range(self.n_classes):
                for j in range(s + 1, self.n_classes):
                    self.classifiers[s][j] = self.classifier(**self.args)
                    idx = (y == s) | (y == j)
                    #   f(x) = 2 (x - j) / (s - j) - 1;  f(s) = 1, f(j) = -1
                    self.classifiers[s][j].fit(X[idx], 2 * (y[idx] - j) / (s - j) - 1)

    def predict(self, X):
        """
        Выдача предсказаний классификатором
        """
        if self.mode == 'one_vs_all':
            probs = [classifier.predict_proba(X)[:, 1] for classifier in self.classifiers]
            return np.argmax(probs, axis=0)
        else:
            pred = np.zeros((X.shape[0], self.n_classes), dtype=int)
            for s in range(self.n_classes):
                for j in range(s + 1, self.n_classes):
                    inds = self.classifiers[s][j].predict(X) == 1
                    pred[inds, j] += 1
                    pred[~inds, s] += 1
            return np.argmax(pred, axis=1)