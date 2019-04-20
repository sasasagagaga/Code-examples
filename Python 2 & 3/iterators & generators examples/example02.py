import numpy as np


class BatchGenerator:
    def __init__(self, list_of_sequences, batch_size, shuffle=False):
        self.lst = list_of_sequences
        self.bsize = batch_size
        self.shuffle = shuffle
        
    def __iter__(self):
        if self.shuffle:
            indices = np.random.permutation(len(self.lst[0]))
        else:
            indices = np.arange(len(self.lst[0]))
        for i in range((indices.shape[0] + self.bsize - 1) // self.bsize):
            st = i * self.bsize
            end = st + self.bsize
            yield [[x[ind] for ind in indices[st:end]] for x in self.lst]
