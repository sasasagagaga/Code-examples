// matrix_functions.h

//   Some extra functions for matrices (inverse matrix, determinant of 
// matrix, rank of matrix


#ifndef EXTRA_MATRIX_INCLUDE_GUARD
#define EXTRA_MATRIX_INCLUDE_GUARD

#include <vector>              // vector
#include "matrix.h"
#include "gaussian_method.h"

namespace MatrixFunctions
{
    //   'determinant' and 'inverse_matrix' - for square matrices only
    //   'determinant' function - compute determinant of matrix
    template <class T>
    T determinant(const Matrix<T> &A)
    {
        if (A.get_rows() != A.get_cols()) {
            throw std::invalid_argument("'determinant': matrix must be square");
        }

        //   Transform matrix to row echelon form
        auto TMP = GaussianJordanElimination::
            get_direct_motion_max_element<T>(A);

        //   Multiply all diagonals elements -- it's determinant
        std::vector<T> diag;
        for (int i = 0; i < TMP.get_rows(); ++i) {
            diag.push_back(TMP[i][i]);
        }
        sort(diag.rbegin(), diag.rend());

        T det = 1;
        for (T val : diag) {
            det *= val;
        }

        return det;
    }

    //   'inverse_matrix' function - compute inverse matrix
    template <class T>
    Matrix<T> inverse_matrix(const Matrix<T> &A)
    {
        //   If A is not square we leave
        if (A.get_rows() != A.get_cols()) {
            throw std::invalid_argument("'inverse_matrix': matrix must be "
                    "square");
        }

        //   If A is nondegenerate we leave
        if (GaussianJordanElimination::check_is_zero(determinant(A))) {
            throw std::invalid_argument("'inverse_matrix: inverse matrix "
                    "exists only for nondegenerate matrix");
        }

        //   Here I inverse matrix using Gauss-Jordan elimination
        auto B = Matrix<T>::get_I(A.get_rows());

        auto TMP = GaussianJordanElimination::
                get_direct_motion_max_element<T>(A, B);
        auto TMP2 = GaussianJordanElimination::
                get_counter_motion(TMP.first, TMP.second);

        return TMP2.second;
    }
    
    //   'rank_matrix' function - compute rank of matrix
    template <class T>
    size_t rank_matrix(const Matrix<T> &A)
    {
        //   Here we find rank of matrix after we transform it in a row echelon form
        auto TMP = GaussianJordanElimination::
                get_direct_motion_max_element<T>(A);

        for (int i = 0; i < TMP.get_rows(); ++i) {
            bool nonzero = false;

            for (int j = 0; j < TMP.get_cols(); ++j) {
                if (!GaussianJordanElimination::check_is_zero(TMP[i][j])) {
                    nonzero = true;
                    break;
                }
            }

            if (!nonzero) {
                return i;
            }
        }

        return TMP.get_rows();
    }
}

#endif // EXTRA_MATRIX_INCLUDE_GUARD
