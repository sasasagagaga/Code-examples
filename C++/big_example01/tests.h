// tests.h

//   This file I use for testing Gaussian-Jordan elimination and SOR
// (successive over-relaxation)


#ifndef TESTS_INCLUDE_GUARD
#define TESTS_INCLUDE_GUARD

#include <utility>      // pair
#include <functional>   // function
#include "matrix.h"
#include "tester.h"
#include "matrix_functions.h"

namespace Tests
{
    //   In TestInfoMain class I store information about tests which will 
    // be generated. 'f' variable is a function which generates right part
    // of SLE. For each example (1 and 2) I create class derived from 
    // TestInfoMain class. There are two more variables nn these derived 
    // classes -- 'n' and 'm', which I took from task (and their meanings 
    // are the same with meanings of these variables from task).
    template <class T>
    class TestInfoMain
    {
    public:
        using type = T;
        using function_type = typename std::function<T(int, int, size_t, 
                size_t, const T &)>;

        function_type f;
    };

    //   I wrote this #define to not copy derived class in each example 
    // namespace
#define TEST_INFO_CLASS                                  \
    template <class T>                                   \
    class TestInfo : public TestInfoMain<T>              \
    {                                                    \
    public:                                              \
        Size n, m;                                       \
                                                         \
        TestInfo(Size N, Size M,                         \
            typename TestInfoMain<T>::function_type F)   \
        {                                                \
            n = N;                                       \
            m = M;                                       \
            this->f = F;                                 \
        }                                                \
    }


    //   Example namespaces. In each one I have enumeration of constant 
    // sizes of matrices. These sizes I took from task from attachment 2.
    // 'get_A' function returns left part of SLE which is the same for 
    // all tests in each example. Then I define class derived from 
    // TestInfoMain class
    namespace example1
    {
        enum Size
        {
            N1 = 40, M1 = 10,
            N2 = 20, M2 = 8,
            N3 = 30, M3 = 9,
            N4 = 50, M4 = 15,
            N5 = 30, M5 = 20,
            N6 = 25, M6 = 10
        };

        template <class T>
        auto get_A(size_t n, size_t M)
        {
            return
            Matrix<T>::generate_matrix(n, n, M, 
            [](int i, int j, size_t rows, size_t cols, const T &val) {
                ++i;
                ++j;

                if (i == j) {
                    return rows + val * val + T(j) / val + T(i) / rows;
                }
                return T(i + j) / (val + rows);
            });
        }

        TEST_INFO_CLASS;
    }

    namespace example2
    {
        enum Size
        {
            N1 = 50,  M1 = 1, 
            N2 = 40,  M2 = 2, 
            N3 = 30,  M3 = 3, 
            N4 = 100, M4 = 4, 
            N5 = 100, M5 = 5, 
            N6 = 100, M6 = 6
        };

        template <class T>
        auto get_A(size_t n, size_t M)
        {
            return
            Matrix<T>::generate_matrix(n, n, M, 
            [](int i, int j, size_t rows, size_t cols, const T &val) {
                ++i;
                ++j;

                T q = 1.001 - 2 * val * 1e-3;

                if (i == j) {
                    return pow(q - 1, i + j);
                }
                return pow(q, i + j) + 0.1 * (j - i);
            });
        }

        TEST_INFO_CLASS;
    }


    //   Filling Tester object with tests. Some tests are written in 
    // code, some tests are generated with formulas from task
    template <class T>
    void create_tests(Tester<std::pair<Matrix<T>, Matrix<T>>, Matrix<T>> 
            &tester)
    {
        //   I chose X equal to 1.23 for generating right parts of SLE 
        // (in attachment #2 in example #2)
        const T X = 1.23;
        const T e = 2.71828182845904523536;


        // *** MY TESTS *** //

        //   Test 1 - very simple
        tester.add_test(
            std::make_pair(
                Matrix<T>({
                    { 4, 0 },
                    { 0, 5 }
                }),
                Matrix<T>({ { 1, 1 } }).get_transposed()
            ), Matrix<T>({ { 0.25, 0.2 } }).get_transposed()
        );

        //   Tests 2-6 I brought from mathprofi.net site. Full link: 
        // https://goo.gl/J1mecg (this link is a short link to 
        // mathprofi.net site)

        //   Test 2
        tester.add_test(
            std::make_pair(
                Matrix<T>({
                    { 8, 7, 3 },
                    { -7, -4, -4 },
                    { -6, 5, -4 }
                }),
                Matrix<T>({ { 18, -11, -15 } }).get_transposed()
            ), Matrix<T>({ { 5, -1, -5 } }).get_transposed()
        );

        //   Test 3
        tester.add_test(
            std::make_pair(
                Matrix<T>({
                    { 1, 2, 3 },
                    { 2, -1, 2 },
                    { 1, 1, 5 }
                }),
                Matrix<T>({ { 1, 6, -1 } }).get_transposed()
            ), Matrix<T>({ { 4, 0, -1 } }).get_transposed()
        );

        //   Test 4
        tester.add_test(
            std::make_pair(
                Matrix<T>({
                    { 3, 2, -5 },
                    { 2, -1, 3 },
                    { 1, 2, -1 }
                }),
                Matrix<T>({ { -1, 13, 9 } }).get_transposed()
            ), Matrix<T>({ { 3, 5, 4 } }).get_transposed()
        );

        //   Test 5
        tester.add_test(
            std::make_pair(
                Matrix<T>({
                    { 4, 2, -1 },
                    { 5, 3, -2 },
                    { 3, 2, -3 }
                }),
                Matrix<T>({ { 1, 2, 0 } }).get_transposed()
            ), Matrix<T>({ { -1, 3, 1 } }).get_transposed()
        );

        //   Test 6
        tester.add_test(
            std::make_pair(
                Matrix<T>({
                    { 2, 5, 4, 1 },
                    { 1, 3, 2, 1 },
                    { 2, 10, 9, 7 },
                    { 3, 8, 9, 2 }
                }),
                Matrix<T>({ { 20, 11, 40, 37 } }).get_transposed()
            ), Matrix<T>({ { 1, 2, 2, 0 } }).get_transposed()
        );


        // *** ATTACHMENT 1 *** //

        //  * Variant 12 *  //
        //   First SLE
        tester.add_test(
            std::make_pair(
                Matrix<T>({
                    { 2, -2, 0, 1 },
                    { 2, 3, 1, -3 },
                    { 3, 4, -1, 2 },
                    { 1, 3, 1, -1 }
                }),
                Matrix<T>({ { -3, -6, 0, 2 } }).get_transposed()
            ), Matrix<T>().get_transposed()
        );

        //   Second SLE
        // This matrix has determinant equal to 0.
        // Proof: https://goo.gl/Kp2isJ  (this link is a short link to 
        // wolframalpha.com site)
        tester.add_test(
            std::make_pair(
                Matrix<T>({
                    { 1, 3, 2, 1 },
                    { 2, -1, 3, -2 },
                    { 3, -5, 4, -3 },
                    { 1, 17, 4, -23 }
                }),
                Matrix<T>({ { 0, 0, 0, 0 } }).get_transposed()
            ), Matrix<T>().get_transposed()
        );

        // Third SLE
        tester.add_test(
            std::make_pair(
                Matrix<T>({
                    { 45, -28, 34, -52 },
                    { 36, -23, 29, -43 },
                    { 47, -32, 36, -48 },
                    { 27, -19, 22, -35 }
                }),
                Matrix<T>({ { 9, 3, -17, 6 } }).get_transposed()
            ), Matrix<T>().get_transposed()
        );


        // *** ATTACHMENT 2 *** //

        //   In 'generated_tests_example#' (where # is 1 or 2 - number of 
        // example) I store TestInfo's objects. Here I fill this vector 
        // with information about tests from example

        //   Example 1
        // Variant 1
        using TestInfo1 = example1::TestInfo<T>;
        using func_type1 = typename TestInfo1::function_type;

        std::vector<example1::TestInfo<T>> generated_tests_example1;

        // Variant 1
        generated_tests_example1.push_back(
                TestInfo1(example1::N1, example1::M1, (func_type1) (
                [] (int i, int j, size_t rows, size_t cols, 
                        const T &val) -> T {
                    ++i;
                    ++j;

                    return rows * i + val;
                })
        ));

        // Variant 2
        generated_tests_example1.push_back(
                TestInfo1(example1::N2, example1::M2, (func_type1) (
                [] (int i, int j, size_t rows, size_t cols, 
                        const T &val) -> T {
                    ++i;
                    ++j;

                    return 200 + 50 * i;
                })
        ));

        // Variant 3
        generated_tests_example1.push_back(
                TestInfo1(example1::N3, example1::M3, (func_type1) (
                [] (int i, int j, size_t rows, size_t cols, 
                        const T &val) -> T {
                    ++i;
                    ++j;

                    return i * i - 100;
                })
        ));

        // Variant 4
        generated_tests_example1.push_back(
                TestInfo1(example1::N4, example1::M4, (func_type1) (
                [] (int i, int j, size_t rows, size_t cols, 
                        const T &val) -> T {
                    ++i;
                    ++j;

                    return val * rows - i * i * i;
                })
        ));

        // Variant 5
        generated_tests_example1.push_back(
                TestInfo1(example1::N5, example1::M5, (func_type1) (
                [] (int i, int j, size_t rows, size_t cols, 
                        const T &val) -> T {
                    ++i;
                    ++j;

                    return val * i + rows;
                })
        ));

        // Variant 6
        generated_tests_example1.push_back(
                TestInfo1(example1::N6, example1::M6, (func_type1) (
                [] (int i, int j, size_t rows, size_t cols, 
                        const T &val) -> T {
                    ++i;
                    ++j;

                    return i * i - rows;
                })
        ));


        //   Example 2
        using TestInfo2 = example2::TestInfo<T>;
        using func_type2 = typename TestInfo2::function_type;

        std::vector<example2::TestInfo<T>> generated_tests_example2;

        // Variant 1
        generated_tests_example2.push_back(
                TestInfo2(example2::N1, example2::M1, (func_type2) (
                [X, e] (int i, int j, size_t rows, size_t cols, 
                        const T &val) -> T {
                    ++i;
                    ++j;

                    return rows * pow(e, X / i) * cos(X);
                })
        ));

        // Variant 2
        generated_tests_example2.push_back(
                TestInfo2(example2::N2, example2::M2, (func_type2) (
                [X] (int i, int j, size_t rows, size_t cols, 
                        const T &val) -> T {
                    ++i;
                    ++j;

                    return abs(X - T(rows) / 10) * i * sin(X);
                })
        ));

        // Variant 3
        generated_tests_example2.push_back(
                TestInfo2(example2::N3, example2::M3, (func_type2) (
                [X, e] (int i, int j, size_t rows, size_t cols, 
                        const T &val) -> T {
                    ++i;
                    ++j;

                    return X * pow(e, X / i) * cos(X / i);
                })
        ));

        // Variant 4
        generated_tests_example2.push_back(
                TestInfo2(example2::N4, example2::M4, (func_type2) (
                [X, e] (int i, int j, size_t rows, size_t cols, 
                        const T &val) -> T {
                    ++i;
                    ++j;

                    return rows * pow(e, X / i) * cos(X);
                })
        ));

        // Variant 5
        generated_tests_example2.push_back(
                TestInfo2(example2::N5, example2::M5, (func_type2) (
                [X] (int i, int j, size_t rows, size_t cols, 
                        const T &val) -> T {
                    ++i;
                    ++j;

                    return abs(X - T(rows) / 10) * i * sin(X);
                })
        ));

        // Variant 6
        generated_tests_example2.push_back(
                TestInfo2(example2::N6, example2::M6, (func_type2) (
                [X, e] (int i, int j, size_t rows, size_t cols, 
                        const T &val) -> T {
                    ++i;
                    ++j;

                    return X * pow(e, X / i) * cos(X / i);
                })
        ));

        //   Here I create tests from information about them and add this 
        // tests to Tester object
        //   Example 1
        for (auto test_info : generated_tests_example1) {
            tester.add_test(std::make_pair(
                example1::get_A<T>(test_info.n, test_info.m),
                Matrix<T>::generate_matrix(test_info.n, 1, 
                        test_info.m, test_info.f)
            ), Matrix<T>());
        }

        //   Example 2
        for (auto test_info : generated_tests_example2) {
            tester.add_test(std::make_pair(
                example2::get_A<T>(test_info.n, test_info.m),
                Matrix<T>::generate_matrix(test_info.n, 1, 
                        test_info.m, test_info.f)
            ), Matrix<T>());
        }


        //   And some positive definite matrices
        std::vector<size_t> pos_def_sizes = { 10, 20, 30, 40, 50 };
        for (auto n : pos_def_sizes) {
            //   Generating random matrix n x n with rank n
            Matrix<T> A;
            while (MatrixFunctions::rank_matrix(A = 
                    Matrix<T>::make_random_matrix(n, n, n, n)) < n) {}

            //   Generating diagonal matrix
            auto diag = Matrix<T>::generate_matrix(n, n, 0, 
            [](int i, int j, size_t rows, size_t cols, const T &val) {
                return i == j ? i + 1 : 0;
            });

            auto B = A * diag * MatrixFunctions::inverse_matrix(A);

            //   And here we get positive definite matrix
            tester.add_test(std::make_pair(
                    B * B.get_transposed(),
                    Matrix<T>::make_random_matrix(n, n, 1, 1)
                    ), Matrix<T>());
        }

        //   One more test with small positive definite left matrix for which 
        // we know answer (I compute it with wolframalpha.com: 
        // https://goo.gl/sAJdmc (this is a short link to propriate site). 
        // Left part of matrix I brought from https://goo.gl/2tgK3K (this is 
        // a short link to site http://twt.mpei.ac.ru/...)
        tester.add_test(
            std::make_pair(
                Matrix<T>({
                    { 2, -1, 0 },
                    { -1, 2, -1 },
                    { 0, -1, 2 }
                }),
                Matrix<T>({ { 0, 1, 2 } }).get_transposed()
            ), Matrix<T>({ { 1, 2, 2 } }).get_transposed()
        );
    }

    //   Creating and filling Tester object with tests using 
    // 'create_tests' function
    template <class T>
    Tester<std::pair<Matrix<T>, Matrix<T>>, Matrix<T>> 
            create_tester_with_tests()
    {
       Tester<std::pair<Matrix<T>, Matrix<T>>, Matrix<T>> tester;

       create_tests(tester);

       return tester;
    }
}

#endif // TESTS_INCLUDE_GUARD
