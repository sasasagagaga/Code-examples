// gaussian_method.h

//   Here I define some methods which calculate parts of Gaussian-Jordan 
// elimination. I use them to calculate inverse matrix, determinant of 
// matrix and solve SLE using Gaussian method


#ifndef GAUSSIAN_METHOD_INCLUDE_GUARD
#define GAUSSIAN_METHOD_INCLUDE_GUARD

#include "matrix.h"
#include <utility>  // swap
#include <cmath>    // abs

namespace GaussianJordanElimination
{
    //   Exception's messages
    const std::string exception_prefix = "GaussianJordanElimination "
            "namespace: ";
    const std::string exception_matrices_rows_size_do_not_match = 
            exception_prefix + "matrices must have the same number of rows!";
    const std::string exception_matrix_in_not_row_echelon_form = 
            exception_prefix + "matrix doesn't have a row echelon form";

    //   Check wheter val is zero or not.
    //   Returns 1 if val is equal to zero (with defined precision)
    // or 0 otherwise.
    template <class T>
    bool check_is_zero(const T &val)
    {
        const T EPS = T(1e-9);
        return std::abs(val) <= EPS;
    }

    // DIRECT MOTION

    //   A - main part of system, B - right part of system.
    //   This method makes direct motion of Gaussian-Jordan elimination.
    //   Returns number of swaps occured during the method work. 
    // Is's necessary for computing determinant: when swap occured 
    // the sign of determinant changes
    template <class T, class F>
    size_t direct_motion(Matrix<T> &A, Matrix<T> &B, F find_pivot)
    {
        //   If sizes of matrices are not the same we can't continue
        if (A.get_rows() != B.get_rows()) {
            throw std::invalid_argument(
                    exception_matrices_rows_size_do_not_match);
        }

        size_t cnt_swaps = 0;

        int ccol = 0;
        for (int row = 0; row < std::min(A.get_rows(), A.get_cols()); ++row) {
            //   Looking for pivot

            //   Given function 'find_pivot' returns number of row 
            // which will be replaced with current row
            //   'find_pivot' returns A.get_rows() if pivot was not found
            //   'find_pivot' function brings column number, where we're 
            // looking for pivot as 2nd argument, and whole matrix as 3rd 
            // argument. 'find_pivot' starts searching of pivot with row, 
            // which number is given in 1st argument

            //   'ccol' is current column
            int pivot;
            for (; ccol < A.get_cols(); ++ccol) {
                pivot = find_pivot(row, ccol, A);

                if (pivot < A.get_rows()) {
                    break;
                }
            }

            if (ccol >= A.get_cols()) {
                break;
            }

            //   Swapping line with pivot and current line
            if (row != pivot) {
                std::swap(A[row], A[pivot]);
                std::swap(B[row], B[pivot]);

                ++cnt_swaps;
            }

            //   Here we perform all necessary operations with matrix
            // such that in current column all elements starting with
            // 'row' row are zeros. 'crow' is current row
            for (int crow = row + 1; crow < A.get_rows(); ++crow) {
                T coef = A[crow][ccol] / A[row][ccol];

                for (int col = ccol; col < A.get_cols(); ++col) {
                    A[crow][col] -= coef * A[row][col];
                }

                for (int col = 0; col < B.get_cols(); ++col) {
                    B[crow][col] -= coef * B[row][col];
                }
            }
        }

        return cnt_swaps;
    }

    //   Direct motion for case when B is omitted
    template <class T, class F>
    size_t direct_motion(Matrix<T> &A, F find_pivot)
    {
        Matrix<T> TMP(A.get_rows(), 0);

        return direct_motion(A, TMP, find_pivot);
    }

    //   Direct motion for case when given matrices shouldn't be changed
    //   Returns pair of matrices created from given ones and changed by 
    // direct motion method
    template <class T, class F>
    std::pair<Matrix<T>, Matrix<T>> get_direct_motion(
            const Matrix<T> &Aarg, const Matrix<T> &Barg, 
            size_t &cnt_swaps, F find_pivot)
    {
        Matrix<T> A = Aarg;
        Matrix<T> B = Barg;

        cnt_swaps = direct_motion(A, B, find_pivot);

        return std::make_pair(A, B);
    }

    //   It's get_direct_motion for case when we don't need number of 
    // swaps
    template <class T, class F>
    std::pair<Matrix<T>, Matrix<T>> get_direct_motion(
            const Matrix<T> &Aarg, const Matrix<T> &Barg, F find_pivot)
    {
        size_t tmp;
        return get_direct_motion(Aarg, Barg, tmp, find_pivot);
    }

    //   It's get_direct_motion for case when B is omitted
    template <class T, class F>
    Matrix<T> get_direct_motion(const Matrix<T> &Aarg, size_t &cnt_swaps, 
            F find_pivot)
    {
        Matrix<T> A = Aarg;

        cnt_swaps = direct_motion(A, find_pivot);

        return A;
    }

    //   It's get_direct_motion for case when we don't need and B
    // and number of swaps
    template <class T, class F>
    Matrix<T> get_direct_motion(const Matrix<T> &Aarg, F find_pivot)
    {
        size_t tmp;
        return get_direct_motion(Aarg, tmp, find_pivot);
    }


    //   SPECIAL DIRECT MOTIONS

    //   FindPivot functions are used to find pivot in direct motion
    namespace FindPivotFunctions
    {
        //   'find_pivot' for usual searching (find first non-zero 
        // element in column)
        template <class T>
        auto find_pivot_usual = 
        [](size_t row, size_t col, const Matrix<T> &A)
        {
            int pivot;
            for (pivot = row; pivot < A.get_rows() &&
                    check_is_zero(A[pivot][col]); ++pivot) {}

            return pivot;
        };

        //   'find_pivot' which finds maximum element in column
        template <class T>
        auto find_pivot_max_element = 
        [](size_t row, size_t col, const Matrix<T> &A)
        {
            //   'crow' is current row
            int pivot = row;
            for (int crow = row + 1; crow < A.get_rows(); ++crow) {
                if (std::abs(A[pivot][col]) < std::abs(A[crow][col])) {
                    pivot = crow;
                }
            }

            if (check_is_zero(A[pivot][col])) {
                pivot = A.get_rows();
            }

            return pivot;
        };
    }

    //   In next 4 functions I just run apropriate 'direct_motion' 
    // function with specific 'find_pivot' function from 
    // FindPivotFunctions namespace

    //   'direct_motion' method with usual 'find_pivot' function
    template <class T, class ...Ts>
    auto direct_motion_usual(Ts &...args)
    {
        return direct_motion(args..., 
                FindPivotFunctions::find_pivot_usual<T>);
    }

    //   'get_direct_motion' method with usual find_pivot function
    template <class T, class ...Ts>
    auto get_direct_motion_usual(Ts &...args)
    {
        return get_direct_motion(args..., 
                FindPivotFunctions::find_pivot_usual<T>);
    }


    //   'direct_motion' method wich finds maximum element in column 
    // at each step.
    template <class T, class ...Ts>
    auto direct_motion_max_element(Ts &...args)
    {
        return direct_motion(args...,
                FindPivotFunctions::find_pivot_max_element<T>);
    }

    template <class T, class ...Ts>
    auto get_direct_motion_max_element(Ts &...args)
    {
        return get_direct_motion(args..., 
                FindPivotFunctions::find_pivot_max_element<T>);
    }


    // COUNTER MOTION

    //   A - main part of system, B - right part of system
    //   This method makes counter motion of Gaussian-Jordan elimination.
    template <class T>
    void counter_motion(Matrix<T> &A, Matrix<T> &B)
    {
        if (A.get_rows() != B.get_rows()) {
            throw std::invalid_argument(
                    exception_matrices_rows_size_do_not_match);
        }

        //   Here I check matrix to have row echelon form
        int first_nz_element = -1; // first non zero element in previous line
        for (int i = 0; i < A.get_rows(); ++i) {
            for (int j = 0; j < A.get_cols(); ++j) {
                if (!check_is_zero(A[i][j])) {
                    if (first_nz_element >= j) {
                        throw std::invalid_argument(
                                exception_matrix_in_not_row_echelon_form);
                    }

                    first_nz_element = j;
                    break;
                }
            }
        }

        for (int row = (int) A.get_rows() - 1; row >= 0; --row) {
            //   first_nz - first element in row 'row' which is not 
            // equal to zero
            int first_nz;
            for (first_nz = 0; first_nz < A.get_cols(); ++first_nz) {
                if (!check_is_zero(A[row][first_nz])) {
                    break;
                }
            }

            //   Case when current row consists of only zeros
            if (first_nz == A.get_cols()) {
                continue;
            }

            //   Normalizing row
            T main_element = A[row][first_nz];
            for (int col = first_nz; col < A.get_cols(); ++col) {
                A[row][col] /= main_element;
            }
            for (int col = 0; col < B.get_cols(); ++col) {
                B[row][col] /= main_element;
            }

            //   Making unit matrix from A with appropriate changes in B.
            // 'crow' is current row
            for (int crow = 0; crow < row; ++crow) {
                T coef = A[crow][first_nz];

                for (int col = first_nz; col < A.get_cols(); ++col) {
                    A[crow][col] -= coef * A[row][col];
                }

                for (int col = 0; col < B.get_cols(); ++col) {
                    B[crow][col] -= coef * B[row][col];
                }
            }
        }
    }

    //   Counter motion for case when B is omitted.
    template <class T>
    void counter_motion(Matrix<T> &A)
    {
        Matrix<T> TMP(A.get_rows(), 0);

        counter_motion(A, TMP);
    }

    //   Counter motion for case when given matrices shouldn't be changed
    //   Returns pair of matrices created from given ones and changed by 
    // counter motion method
    template <class T>
    std::pair<Matrix<T>, Matrix<T>> get_counter_motion(
            const Matrix<T> &Aarg, const Matrix<T> &Barg)
    {
        Matrix<T> A = Aarg;
        Matrix<T> B = Barg;

        counter_motion(A, B);

        return std::make_pair(A, B);
    }

    //   It's get_counter_motion for case when B is omitted
    template <class T>
    Matrix<T> get_counter_motion(const Matrix<T> &Aarg)
    {
        Matrix<T> A = Aarg;
        Matrix<T> TMP(A.get_rows(), 0);

        return get_counter_motion(A, TMP).first;
    }
}

#endif // GAUSSIAN_METHOD_INCLUDE_GUARD
