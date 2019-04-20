from collections import defaultdict
import copy


class CooSparseMatrix:
    def _prepare_coords(self, coords):
        i, j = tuple(map(int, coords))
        if 0 > i or i >= self.Shape[0] or 0 > j or j >= self.Shape[1]:
            raise TypeError
        return i, j

    def __get_copy(self):
        return copy.deepcopy(self)

    def __init__(self, ijx_list, shape=None):
        self.data = defaultdict(dict)
        for *coords, x in ijx_list:
            i, j = tuple(map(int, coords))
            if (i, j) in self.data:
                raise TypeError
            self.data[i][j] = x
        self.Shape = shape

    def __getitem__(self, coords):
        if len(coords) == 2:
            i, j = self._prepare_coords(coords)
            return self.data.get(coords[0], dict()).get(coords[1], 0.)

        i, _ = self._prepare_coords((coords[0], 0))
        return CooSparseMatrix([(i, j, v) for j, v in self.data[i].items()],
                               Shape=(1, self.Shape[1]))

    def __setitem__(self, coords, value):
        i, j = self._prepare_coords(coords)
        if value == 0:
            if i in self.data and j in self.data[i]:
                del self.data[i][j]
        else:
            self.data[i][j] = value

    def __add__(self, other):
        if self.Shape != other.Shape:
            raise TypeError
        res = self.__get_copy()
        for i, d in other.data.items():
            if i not in res.data:
                res.data[i] = copy.copy(other.data[i])
                continue
            for j, v in d.items():
                cur_val = res.data[i].get(j, 0) + v
                if cur_val == 0:
                    res.data[i].pop(j, None)
                else:
                    res.data[i][j] = cur_val
            if len(res.data[i]) == 0:
                del res.data[i]
        return res

    def __mul__(self, value):
        if value == 0:
            return CooSparseMatrix([], Shape=self.Shape)
        res = self.__get_copy()
        for i in self.data.keys():
            for j in self.data[i].keys():
                res.data[i][j] *= value
        return res
    
    def __rmul__(self, value):
        return self * value

    def __sub__(self, other):
        return self + other * -1

    def __deepcopy__(self, memo):
        res = CooSparseMatrix([], shape=self.Shape)
        res.data = copy.deepcopy(self.data)
        return res
    
    def __setattr__(self, attr, value):
        if attr == 'shape':
            if not isinstance(value, tuple):
                raise TypeError
            if len(value) != 2 or type(value[0]) is not int or type(value[1]) is not int:
                raise TypeError
            if value[0] * value[1] != self.Shape[0] * self.Shape[1]:
                raise TypeError

            res = CooSparseMatrix([], value)
            for i, d in self.data.items():
                for j, v in d.items():
                    pos = i * self.Shape[1] + j
                    res[pos // value[1], pos % value[1]] = v
            self.Shape = value
            self.data = res.data
        elif attr == 'T':
            raise AttributeError
        else:
            self.__dict__[attr] = value

    def __getattr__(self, attr):
        if attr == 'shape':
            return self.Shape
        elif attr == 'T':
            res = CooSparseMatrix([], self.Shape[::-1])
            for i, d in self.data.items():
                for j, v in d.items():
                    res[j, i] = v
            return res
        else:
            return self.__dict__[attr]
