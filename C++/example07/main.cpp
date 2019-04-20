#include <stdexcept>
#include <algorithm>

namespace Game
{
    template <class T>
    class Matrix
    {
    private:
        int rows = 0;
        int cols = 0;
        T *m = nullptr;

        void swap(Matrix &other)
        {
            std::swap(m, other.m);
            std::swap(rows, other.rows);
            std::swap(cols, other.cols);
        }

        class Proxy
        {
        public:
            Proxy(T *off_, int cols_) : off(off_), cols(cols_) {}

            T &operator[] (int col)
            {
                if (0 > col || col >= cols) {
                    throw std::range_error("column number should be between 0 and number of cols");
                }
                return off[col];
            }

            T operator[] (int col) const
            {
                if (0 > col || col >= cols) {
                    throw std::range_error("column number should be between 0 and number of cols");
                }
                return off[col];
            }

        private:
            T *off = nullptr;
            int cols = 0;
        };

    public:
        static constexpr int ROWS_MAX = 16384;
        static constexpr int COLS_MAX = 16384;
        static constexpr int MEM_MAX = 1073741824;

        Matrix(int rows_, int cols_, T val = T())
                : rows(rows_), cols(cols_)
        {
            if (0 < rows && rows <= ROWS_MAX && 0 < cols && cols <= COLS_MAX &&
                    1U * cols * rows <= MEM_MAX / sizeof(T)) {
                m = reinterpret_cast<T *>(::operator new(rows * cols * sizeof(T)));
                for (int i = 0; i < rows * cols; ++i) {
                    new (m + i) T(val);
                }
            } else {
                throw std::invalid_argument("invalid sizes");
            }
        }

        Matrix(const Matrix &other) : rows(other.rows), cols(other.cols)
        {
            m = reinterpret_cast<T *>(::operator new(rows * cols * sizeof(T)));
            for (int i = 0; i < rows * cols; ++i) {
                new (m + i) T(other.m[i]);
            }
        }

        Matrix(Matrix &&other)
        {
            swap(other);
        }

        Matrix &operator = (Matrix m)
        {
            swap(m);
            return *this;
        }

        auto get_rows() const
        {
            return rows;
        }

        auto get_cols() const
        {
            return cols;
        }

        Proxy operator [] (int row)
        {
            if (0 > row || row >= rows) {
                throw std::range_error("row number should be between 0 and number of rows");
            }
            return Proxy(m + row * cols, cols);
        }

        const Proxy operator [] (int row) const
        {
            if (0 > row || row >= rows) {
                throw std::range_error("row number should be between 0 and number of rows");
            }
            return Proxy(m + row * cols, cols);
        }

        ~Matrix()
        {
            for (int i = 0; i < rows * cols; ++i) {
                m[i].~T();
            }
            ::operator delete(m, rows * cols * sizeof(T));
        }
    };
}