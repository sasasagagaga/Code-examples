// SLE_solvers.h

//   There are SLE solvers


#ifndef SLE_SOLVERS_INCLUDE_GUARD
#define SLE_SOLVERS_INCLUDE_GUARD

#include <boost/filesystem.hpp>  // path, create_directory
#include "matrix.h"
#include "gaussian_method.h"
#include "matrix_functions.h"
#include "tester.h"
#include "tests.h"

namespace SLESolvers
{
    template <class T>
    using TesterT = std::pair<Matrix<T>, Matrix<T>>;

    template <class T>
    using TesterA = Matrix<T>;

    template <class T>
    using TesterSLE = Tester<TesterT<T>, TesterA<T>>;    

    template <class T>
    using SLE_solver_type = Matrix<T>(*)(const Matrix<T> &, const Matrix<T> &);
    

    //   Solver of SLE which uses gauss method. Direct motion performs by
    // given 'right_get_direct_motion' function
    template <class T, class F>
    auto SLE_gauss_solver(const Matrix<T> &A, const Matrix<T> &f, 
            F right_get_direct_motion)
    {
        //   Do all Gaussian-Jordan elimination
        auto TMP = right_get_direct_motion(A, f);
        GaussianJordanElimination::counter_motion(TMP.first, TMP.second);
    
        //   Calculating ranks of matrices A and B. B is matrix which is 
        // constructed from A and f. 'Ar' and 'Br' - ranks of A and B (after 
        // direct motion)
        int Ar = MatrixFunctions::rank_matrix(TMP.first);
    
        int Br = 0;
        for (int i = (int) TMP.second.get_rows() - 1; i >= 0; --i) {
            if (!GaussianJordanElimination::
                        check_is_zero(TMP.second[i][0])) {
                Br = i + 1;
            }
        }
        Br = std::max(Br, Ar);
    
        //   And some checks (0 solutions, 1 solution, infinity solutions).
        // Ar < Br => 0 solutions, Ar == Br == min(sizes of matrix) => 1 
        // solution, Ar == Br != min(...) => infinite number of solutions
        if (Ar < Br) {
            throw std::domain_error("SLE_gauss_solver: SLE has no solutions");
        } else if (Ar < std::min(A.get_rows(), A.get_cols())) {
            throw std::domain_error("SLE_gauss_solver: SLE has infinite "
                    "number of solutions");
        }
    
        return TMP.second;
    }
    
    
    //   Solver of SLE which uses usual gauss method
    template <class T>
    auto SLEGU(const Matrix<T> &A, const Matrix<T> &f)
    {
        //   Prepare direct motion function to pass it to SLE_solver
        auto tmp_direct_motion = [](const Matrix<T> &A, const Matrix<T> &f) {
            return GaussianJordanElimination::
                    get_direct_motion_usual<T>(A, f);
        };
        return SLE_gauss_solver(A, f, tmp_direct_motion);
    }
    
    //   Solver of SLE which uses gauss method with finding maximum pivot
    template <class T>
    auto SLEGM(const Matrix<T> &A, const Matrix<T> &f)
    {
        //   Prepare direct motion function to pass it to SLE_solver
        auto func = [](const Matrix<T> &A, const Matrix<T> &f) {
            return GaussianJordanElimination::
                    get_direct_motion_max_element<T>(A, f);
        };
        return SLE_gauss_solver(A, f, func);
    }

    //   SOR method. w -- iteration coefficient, eps -- precision, 
    // max_iters -- maximum number of iterations, cnt_iter -- pointer to 
    // variable where number of performed iterations is stored
    template <class T>
    auto SLE_SOR(const Matrix<T> &A, const Matrix<T> &f, 
            double w = 1, int *cnt_iter = NULL, const T &eps = 1e-10, 
            size_t max_iters = 1000)
    {
        //   If A is not square we leave
        if (A.get_rows() != A.get_cols()) {
            throw std::invalid_argument("SLE_SOR: left part of SLE must be "
                    "square");
        }

        //   If number of rows of A and f are not equal we leave
        if (A.get_rows() != f.get_rows()) {
            throw std::invalid_argument("SLE_SOR: left and right parts of"
                    "SLE must have the same number of rows");
        }

        //   Value -1 means that some problems occured during the work
        // of method
        if (cnt_iter) {
            *cnt_iter = -1;
        }

        //   prev - vector x from previous step, cur - vector x from 
        // current step
        Matrix<T> prev(A.get_rows(), 1, 0);
        Matrix<T> cur(A.get_rows(), 1);

        //   Here I implemented formula (124) from page 50 from book [1]
        int iter;
        for (iter = 0; iter < max_iters; ++iter) {
            for (int i = 0; i < A.get_rows(); ++i) {
                T sum = f[i][0];
                for (int j = 0; j < i; ++j) {
                    sum -= A[i][j] * cur[j][0];
                }

                for (int j = i; j < A.get_rows(); ++j) {
                    sum -= A[i][j] * prev[j][0];
                }

                cur[i][0] = prev[i][0] + w / A[i][i] * sum;
            }

            //   If at least one of vector's coordinates is infinity or NaN, 
            // stop
            for (int i = 0; i < A.get_rows(); ++i) {
                if (!isfinite(cur[i][0])) {
                    throw std::domain_error("SLE_SOR: such both parts of SLE "
                            "caused discrepancy of method");
                }
            }

            //   If current and previous vectors x are close enough, stop 
            // (we use here constant eps for this check)
            bool cur_close_to_prev = true;
            for (int i = 0; i < A.get_rows(); ++i) {
                if (std::abs(cur[i][0] - prev[i][0]) >= eps) {
                    cur_close_to_prev = false;
                    break;
                }
            }

            if (cur_close_to_prev) {
                break;
            }

            prev = cur;
        }

        //   Save number of iterations
        if (cnt_iter) {
            *cnt_iter = iter;
        }
        return cur;
    }

    //   SOR method with some standard constants
    template <class T>
    auto SLE_SOR_standard(const Matrix<T> &A, const Matrix<T> &f)
    {
        return SLE_SOR(A, f);
    }


    //   This function tests given solver of SLE
    template <class T>
    void test_SLE_solver(SLE_solver_type<T> SLE_solver, 
            std::string path_to_answers = "answers/", 
            std::ostream &out = std::cout)
    {
        //   Create folder for answers (delete it if it exists)
        boost::filesystem::path dir_answers(path_to_answers);
        boost::filesystem::remove_all(dir_answers);
        boost::filesystem::create_directory(dir_answers);
    
        //   Create Tester object and fill it with tests
        TesterSLE<T> tester;
        Tests::create_tests(tester);
    
        //   Testing given SLE solver
        for (int i = 0; i < tester.get_num_tests(); ++i) {
            //   Bring next test and store both parts of SLE in 'A' and 'f' 
            // matrices
            auto Af = tester.next_test();
            auto A = Af.first;
            auto f = Af.second;
    
            //   Try to find solution. If exception is thrown, print message
            // about it
            try {
                //   Solving SLE
                auto ans = SLE_solver(A, f); // here exception can be thrown
    
                //   I use these variables to calculate number of digits for 
                // each test number
                int test_number_digits = 
                        std::to_string(tester.get_num_tests()).size();
                int make_zeros = pow(10, test_number_digits);
    
                //   Print answer to file
                std::ofstream fout(path_to_answers + "/ans" + 
                        std::to_string(make_zeros + i + 1).substr(1) + ".txt");
                fout << ans << std::endl;
                fout.close();
    
                //   Check answer
                tester.check_answer(ans);
            } catch (std::domain_error &e) {
                //   IT - incorrect test
                out << "[IT] Test #" << i + 1 << ": " << e.what() << std::endl;
            }
        }
    }
}

#endif // SLE_SOLVERS_INCLUDE_GUARD
