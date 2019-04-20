import numpy as np
from bisect import bisect_right


class RleSequence:
    @staticmethod
    def _encode_rle(x):
        idx = np.append(np.diff(x).nonzero(), x.size - 1)
        return x[idx], np.diff(np.append(-1, idx))

    def __init__(self, input_sequence):
        self.nums, self.cnts = self._encode_rle(input_sequence)
        self.cumcnts = np.cumsum(self.cnts)
        self.len = self.cumcnts[-1]

    def __iter__(self):
        self.i = -1
        return self

    def __next__(self):
        self.i += 1
        if self.i >= self.len:
            raise StopIteration
        return self[self.i]

    def _normilize_ind(self, i):
        if not (-self.len <= i < self.len):
            raise ValueError('index out of range')
        return i if i >= 0 else self.len + i

    def _normilize_slice(self, i):
        if i < 0:
            i += self.len
        return int(min(max(0, i), self.len))

    def __getitem__(self, i):
        if isinstance(i, int):
            i = self._normilize_ind(i)
            return self.nums[bisect_right(self.cumcnts, i)]
        elif isinstance(i, slice):
            ind = self._normilize_slice(i.start or 0)
            end = self._normilize_slice(i.stop or self.len)
            step = i.step or 1
            if ind >= end:
                return np.array([])

            ans = []
            while ind < end:
                ans.append(self[ind])
                ind += step
            return np.array(ans)

    def __contains__(self, x):
        return x in self.nums
