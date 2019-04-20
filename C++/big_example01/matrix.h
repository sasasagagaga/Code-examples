// matrix.h

//   Definition and implementation of class Matrix


#ifndef MATRIX_INCLUDE_GUARD
#define MATRIX_INCLUDE_GUARD

#include <iostream>  // istream, ostream
#include <vector>    // vector
#include <stdexcept> // invalid_argument, out_of_range
#include <string>    // to_string
#include <iomanip>   // setw, left, ...
#include <cmath>     // abs
#include <sstream>   // stringstream
#include <random>    // mt19937, uniform_real_distribution


//   Matrix class. I use it to store matrices and to operate with them
template <class T>
class Matrix
{
private:
    //   Limits for random matrix generator
    enum matrix_limits
    {
        //   Limits for number of rows
        MIN_RAND_ROWS = 1,
        MAX_RAND_ROWS = 10,

        //   Limits for number of columns
        MIN_RAND_COLS = 1,
        MAX_RAND_COLS = 10,

        //   Limits for each value in matrix
        MIN_RAND_VAL  = 0,
        MAX_RAND_VAL  = 10,

        //   Seed for random generator
        SEED = 0,

        //   Output precision of matrix elements
        OUTPUT_PREC = 2,
    };

    //   Exception's messages
    static const std::string exception_prefix;
    static const std::string exception_out_of_range;
    static const std::string exception_matrices_sizes_do_not_match;
    static const std::string exception_2Dvector_is_not_matrix;

    //   Stored data which describes matrix: rows - number of rows, 
    // cols - number of columns, data - 2D vector (vector is like array)
    // where all matrix elements are stored
    size_t rows = 0, cols = 0;
    std::vector<std::vector<T>> data = std::vector<std::vector<T>>();

public:
    //   Constructors. Contructor is used to create new instance of class
    Matrix() = default;
    Matrix(size_t rows_init, size_t cols_init, const T &val = T());
    Matrix(const std::vector<std::vector<T>> &vec_matrix);

    //   Copy constructor. Is's used when we need a copy of object
    Matrix(const Matrix<T> &M);

    //   Resize functions. Resize function is used to change sizes of 
    // matrix (rows and cols)
    //   'resize' - resize matrix to given sizes. It doesn't save what 
    // was stored in 'data' variable
    //   'fit' - Fit matrix in current rows and cols sizes. Doesn't 
    // save data
    void resize(size_t new_rows, size_t new_cols, const T &val = T());
    void fit(const T &val = T());

    //   Destructor. Desctruct created instance
    ~Matrix();

    //   Getters
    size_t get_rows() const;
    size_t get_cols() const;

    //   const [] - to access row which number is specified in 'i' argument
    const std::vector<T> &operator[] (size_t i) const;

    //   const 'at' - the same as const [] operator
    const std::vector<T> &at(size_t i) const;

    //   non-const [] - to access and modify row 'i'
    std::vector<T> &operator[] (size_t i);

    //   non-const 'at' - the same as non-const [] operator
    std::vector<T> &at(size_t i);

    //   Read matrix from given istream
    template<class U>
    friend std::istream &operator >> (std::istream &, Matrix<U> &);

    //   Write metrix to given ostream
    template <class U>
    friend std::ostream &operator << (std::ostream &, const Matrix<U> &);

    //   == operator - to compare two matrices for equality
    template <class U>
    friend bool operator == (const Matrix<U> &, const Matrix<U> &);

    //   - and * operators
    template <class U>
    friend Matrix<U> operator - (const Matrix<U> &, const Matrix<U> &);
    template <class U>
    friend Matrix<U> operator * (const Matrix<U> &, const Matrix<U> &);

    //   'transpose' - transpose matrix. 'get_transposed' - the same as 
    // 'transpose', but it doesn't change given matrix
    void transpose();
    Matrix<T> get_transposed() const;

    //   'generate_matrix' - generate matrix using given generator
    template <class F>
    static Matrix<T> generate_matrix(size_t rows, size_t cols, 
            const T &val, F generator);

    //   'make_random_matrix' - create and return random matrix
    static Matrix make_random_matrix(size_t min_rows  = MIN_RAND_ROWS,
                                     size_t max_rows  = MAX_RAND_ROWS,
                                     size_t min_cols  = MIN_RAND_COLS,
                                     size_t max_cols  = MAX_RAND_COLS, 
                                     const T &min_val = MIN_RAND_VAL,
                                     const T &max_val = MAX_RAND_VAL);

private:
    //   'check_is_zero' - checks whether given nubmber is zero or not. 
    // Returns true if zero and false otherwise
    static bool check_is_zero(const T &val);

    //   Setters (for private use only)
    size_t &set_rows();
    size_t &set_cols();

public:
    //   'get_I' returns square identity matrix with given size
    static Matrix<T> get_I(size_t n);
};


template <class T>
const std::string Matrix<T>::exception_prefix = "class Matrix: ";

template <class T>
const std::string Matrix<T>::exception_out_of_range = exception_prefix + 
        "index in Matrix[] is greater then number of rows in matrix";

template <class T>
const std::string Matrix<T>::exception_matrices_sizes_do_not_match = 
        exception_prefix + "sizes of matrices do not match";

template <class T>
const std::string Matrix<T>::exception_2Dvector_is_not_matrix = 
        exception_prefix + "given 2D vector is not a matrix due to "
        "different sizes of rows";

template <class T>
Matrix<T>::Matrix(size_t rows_init, size_t cols_init, const T &val)
{
    resize(rows_init, cols_init, val);
}

template <class T>
Matrix<T>::Matrix(const std::vector<std::vector<T>> &vec_matrix)
{
    //   Case when given vector is empty
    if (vec_matrix.size() == 0) {
        set_rows() = 0;
        set_cols() = 0;
        return;
    }

    //   Check whether given vector of vectors has all vectors of the 
    // same size
    size_t cols0 = vec_matrix[0].size();
    for (int i = 0; i < vec_matrix.size(); ++i) {
        if (cols0 != vec_matrix[i].size()) {
            throw std::invalid_argument(exception_2Dvector_is_not_matrix);
        }
    }

    //   Set all class fields to correct values
    set_rows() = vec_matrix.size();
    set_cols() = cols0;
    data = vec_matrix;
}

template <class T>
Matrix<T>::Matrix(const Matrix<T> &M)
{
    //   Just copy all class fields
    rows = M.get_rows();
    cols = M.get_cols();
    data = M.data;
}

template <class T>
void Matrix<T>::resize(size_t new_rows, size_t new_cols, const T &val)
{
    //   Set sizes
    rows = new_rows;
    cols = new_cols;

    //   Reallocation of memory
    data.clear();
    data.resize(rows, std::vector<T>(cols, val));
}

template <class T>
void Matrix<T>::fit(const T &val)
{
    resize(get_rows(), get_cols(), val);
}

template <class T>
Matrix<T>::~Matrix() {}

template <class T>
size_t Matrix<T>::get_rows() const
{
    return rows;
}

template <class T>
size_t Matrix<T>::get_cols() const
{
    return cols;
}

template <class T>
size_t & Matrix<T>::set_rows()
{
    return rows;
}

template <class T>
size_t & Matrix<T>::set_cols()
{
    return cols;
}

template <class T>
const std::vector<T> & Matrix<T>::operator[] (size_t i) const
{
    //   Check whether 'i' is out of range
    if (i >= rows) {
        throw std::out_of_range(exception_out_of_range);
    }

    return data[i];
}

template <class T>
const std::vector<T> & Matrix<T>::at(size_t i) const
{
    //   Check whether 'i' is out of range
    if (i >= rows) {
        throw std::out_of_range(exception_out_of_range);
    }

    return data[i];
}

template <class T>
std::vector<T> & Matrix<T>::operator[] (size_t i)
{
    //   Check whether 'i' is out of range
    if (i >= rows) {
        throw std::out_of_range(exception_out_of_range);
    }

    return data[i];
}

template <class T>
std::vector<T> & Matrix<T>::at(size_t i)
{
    //   Check whether 'i' is out of range
    if (i >= rows) {
        throw std::out_of_range(exception_out_of_range);
    }

    return data[i];
}

template <class T>
std::istream &operator >> (std::istream &in, Matrix<T> &A)
{
    //   Read all elements from 'in' stream
    for (int i = 0; i < A.get_rows(); ++i) {
        for (int j = 0; j < A.get_cols(); ++j) {
            in >> A[i][j];
        }
    }

    return in;
}

template <class T>
std::ostream &operator << (std::ostream &out, const Matrix<T> &A)
{
    //   Saving flags of ostream 'out'
    std::ios_base::fmtflags originalFlags = out.flags();

    //   If matrix is empty output nothing
    if (A.get_rows() == 0 || A.get_cols() == 0) {
        return out;
    }

    //   Here I count a maximum length of matrix elements (to output 
    // them goodly)
    size_t max_num_len = 0;
    for (int i = 0; i < A.get_rows(); ++i) {
        for (int j = 0; j < A.get_cols(); ++j) {
            std::stringstream stream;

            stream << std::setprecision(Matrix<T>::matrix_limits::OUTPUT_PREC)
                    << std::fixed << A[i][j];

            max_num_len = std::max(max_num_len, stream.str().length());
        }
    }
    ++max_num_len;

    //   Here I find length of separating line for output
    size_t num_dash = max_num_len * A.get_cols() + (2 * A.get_cols() + 1);
    std::string separating_line = std::string(num_dash, '-');

    //   Output
    out << separating_line << std::endl;
    for (int i = 0; i < A.get_rows(); ++i) {
        out << "|";
        for (int j = 0; j < A.get_cols(); ++j) {
            out << ' ' << std::setw(max_num_len) << std::left
                    << std::setprecision(Matrix<T>::matrix_limits::OUTPUT_PREC)
                    << std::fixed << A[i][j] << "|";
        }

        out << std::endl << separating_line << std::endl;
    }

    //   Restoring flags for 'out'
    out.flags(originalFlags);

    return out;
}

template <class T>
bool operator == (const Matrix<T> &A, const Matrix<T> &B)
{
    //   If sizes of matrices are not the same they are not equal
    if (A.get_rows() != B.get_rows() || A.get_cols() != B.get_cols()) {
        return 0;
    }

    //   Just compare all coresponding elements in matrices and if 
    // there are two unequal elements, return false
    for (int row = 0; row < A.get_rows(); ++row) {
        for (int col = 0; col < A.get_cols(); ++col) {
            if (!Matrix<T>::check_is_zero(A[row][col] - B[row][col])) {
                return false;
            }
        }
    }

    //   Otherwise return true
    return true;
}

template <class T>
Matrix<T> operator - (const Matrix<T> &A, const Matrix<T> &B)
{
    if (A.get_rows() != B.get_rows() || A.get_cols() != B.get_cols()) {
        throw std::invalid_argument(Matrix<T>::exception_matrices_sizes_do_not_match);
    }

    Matrix<T> C(A.get_rows(), A.get_cols());

    for (int i = 0; i < A.get_rows(); ++i) {
        for (int j = 0; j < B.get_cols(); ++j) {
            C[i][j] = A[i][j] - B[i][j];
        }
    }

    return C;
}

template <class T>
Matrix<T> operator * (const Matrix<T> &A, const Matrix<T> &B)
{
    if (A.get_cols() != B.get_rows()) {
        throw std::invalid_argument(Matrix<T>::
                exception_matrices_sizes_do_not_match);
    }

    Matrix<T> C(A.get_rows(), B.get_cols(), 0);

    for (int i = 0; i < A.get_rows(); ++i) {
        for (int j = 0; j < B.get_cols(); ++j) {
            for (int k = 0; k < B.get_rows(); ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return C;
}

template <class T>
void Matrix<T>::transpose()
{
    //   Create a copy of matrix
    Matrix<T> Atmp = *this;

    //   Then turn matrix
    std::swap(set_rows(), set_cols());
    fit();

    //   Then copy correctly all elements from 'Atmp' to matrix
    for (int i = 0; i < get_rows(); ++i) {
        for (int j = 0; j < get_cols(); ++j) {
            at(i)[j] = Atmp[j][i];
        }
    }
}

template <class T>
Matrix<T> Matrix<T>::get_transposed() const
{
    Matrix<T> TMP = *this;

    TMP.transpose();

    return TMP;
}

template <class T>
template <class F>
Matrix<T> Matrix<T>::generate_matrix(size_t rows, size_t cols, 
        const T &val, F gen)
{
    //   Create matrix
    Matrix<T> A(rows, cols);

    //   Fill matrix
    for (int i = 0; i < A.get_rows(); ++i) {
        for (int j = 0; j < A.get_cols(); ++j) {
            A[i][j] = gen(i, j, rows, cols, val);
        }
    }

    return A;
}

template <class T>
Matrix<T> Matrix<T>::make_random_matrix(size_t min_rows, size_t max_rows, 
                                        size_t min_cols, size_t max_cols, 
                                        const T &min_val, const T &max_val)
{
    //   Creating generator of random numbers
    std::mt19937 gen(SEED);
    std::uniform_real_distribution<> urd(0, 1);

    //   'get_rand' returns random number from segment [low, high)
    auto get_rand = [&urd, &gen](T low, T high) -> T {
        T r = urd(gen);
        return T(r * (high - low)) + low;
    };

    //   Generate number of rows and columns
    size_t rows = get_rand(min_rows, max_rows);
    size_t cols = get_rand(min_cols, max_cols);

    //   Generating whole matrix
    return Matrix<T>::generate_matrix(rows, cols, 0, 
            [&](int i, int j, size_t rows, size_t cols, const T &val) {
                return get_rand(min_val, max_val);
            }
    );
}

template <class T>
bool Matrix<T>::check_is_zero(const T &val)
{
    //   'EPS' - precision of equality
    const T EPS = T(1e-5);

    return std::abs(val) < EPS;
}

template <class T>
Matrix<T> Matrix<T>::get_I(size_t n)
{
    //   Create matrix with proper sizes
    Matrix<T> TMP(n, n, 0);

    //   Fill matrix's diagonal with 1
    for (int i = 0; i < n; ++i) {
        TMP[i][i] = T(1);
    }

    return TMP;
}

#endif // MATRIX_INCLUDE_GUARD
