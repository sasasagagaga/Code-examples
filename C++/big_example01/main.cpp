// main.cpp

//   Abbreviations:
// 1) SLE - system of linear equations
// 2) SOR - successive over-relaxation

//   A main file with code. Here I tesing SLE solvers, count 
// determinants of some matrices, find inverse matrices, determine whether 
// Gaussian elimination is stable and count the speed of convergence rate of
// iterations


#include <iostream>              // cin, cout
#include <iomanip>               // setprecision
#include <fstream>               // ofstream
#include <boost/filesystem.hpp>  // path, create_directory

#include "matrix.h"
#include "gaussian_method.h"
#include "tester.h"
#include "tests.h"
#include "matrix_functions.h"
#include "SLE_solvers.h"

using namespace std;

using namespace GaussianJordanElimination;
using namespace MatrixFunctions;
using namespace SLESolvers;
using SLESolvers::TesterT;
using SLESolvers::TesterA;

using element_type = double;
using matrix_type = Matrix<element_type>;
using Me = Matrix<element_type>;

//   Some constants
enum
{
    //   For random generator
    SEED = 0,
};

template <class T>
ostream &operator << (ostream &out, const pair<T, T> &a)
{
    out << a.first << endl;
    out << a.second;
    return out;
}

//   Create folder (delete it if it exists)
void new_folder(string path)
{
    boost::filesystem::path dir(path);
    boost::filesystem::remove_all(dir);
    boost::filesystem::create_directory(dir);
}

string get_fout_for_test(const string &pref, const string &name, 
        size_t number, size_t max_number, string type = ".txt")
{
    int test_number_digits = to_string(max_number).size();
    int make_zeros = pow(10, test_number_digits);

    return pref + name + to_string(make_zeros + number).substr(1) + type;
}

//   Write tests from Tester object to given folder
template <class T, class A>
void write_tests_to_folder(const Tester<T, A> &tester, 
        string path_to_tests = "tests/")
{
    //   Create folder for tests
    new_folder(path_to_tests);

    //   I use these variables to calculate number of digits for 
    // each test number
    int test_number_digits = to_string(tester.get_num_tests()).size();
    int make_zeros = pow(10, test_number_digits);

    //   Writing tests
    for (int i = 0; i < tester.get_num_tests(); ++i) {
        ofstream fout(path_to_tests + "/test" + 
                to_string(make_zeros + i + 1).substr(1) + ".txt");
        fout << tester.next_test() << endl;
        fout.close();
    }
}


int main()
{
    write_tests_to_folder<TesterT<element_type>, TesterA<element_type>>(
            Tests::create_tester_with_tests<element_type>());

    //   Testing SLEGU
    cout << "Testing SLEGU\n";
    test_SLE_solver<element_type>(SLEGU, "answer_SLEGU/");
    cout << endl;

    //   Testing SLEGM
    cout << "Testing SLEGM\n";
    test_SLE_solver<element_type>(SLEGM, "answer_SLEGM/");
    cout << endl;

    //   Testing SLE_SOR
    cout << "Testing SLE_SOR\n";
    test_SLE_solver<element_type>(SLE_SOR_standard, "answer_SLE_SOR/");
    cout << endl;

    //   Finding determinants
    cout << "Finding determinants" << endl;

    const string determinants = "determinants/";
    new_folder(determinants);

    // Determinants are stored here
    vector<element_type> dets;

    auto tester = Tests::create_tester_with_tests<element_type>();
    for (int i = 0; i < tester.get_num_tests(); ++i) {
        auto det = determinant(tester.next_test().first);
        dets.push_back(det);

        // Print found determinant
        ofstream fout(get_fout_for_test(determinants, "det", i + 1,
                tester.get_num_tests()));
        fout << det << endl;
        fout.close();
    }

    //   Finding inverse matrices
    cout << "Finding inverse matrices" << endl;

    const string inverse_matrices = "inverse_matrices/";
    new_folder(inverse_matrices);

    tester = Tests::create_tester_with_tests<element_type>();
    for (int i = 0; i < tester.get_num_tests(); ++i) {
        if (check_is_zero(dets[i])) {
            tester.next_test();
            continue;
        }

        // Print found inverse matrix
        ofstream fout(get_fout_for_test(inverse_matrices, "inv", i + 1,
                tester.get_num_tests()));
        fout << inverse_matrix(tester.next_test().first) << endl;
        fout.close();
    }

    //   Determine whether Gaussian elimination is stable
    cout << "Determining Gaussian elimination stability" << endl;

    const string stability = "gauss_stability/";
    new_folder(stability);

    element_type max_deviation = 0;

    tester = Tests::create_tester_with_tests<element_type>();
    for (int i = 0; i < tester.get_num_tests(); ++i) {
        //   Creating generator of random numbers
        std::mt19937 gen(SEED);
        const element_type eps1 = 1e-3;
        std::uniform_real_distribution<> urd(-eps1, eps1);

        auto test = tester.next_test();
        auto A = test.first;
        auto f = test.second;

        //   Solving SLE Ax = f
        Me sol1;
        try {
            sol1 = SLEGM(A, f);
        } catch (domain_error &e) {
            cerr << e.what() << endl;
            continue;
        }

        //   Solving SLE Bx = g, where B and g are slightly modified matrices 
        // A and f
        auto B = test.first;
        for (int row = 0; row < A.get_rows(); ++row) {
            for (int col = 0; col < A.get_cols(); ++col) {
                B[row][col] += urd(gen);
            }
        }

        auto g = test.second;
        for (int row = 0; row < g.get_rows(); ++row) {
            g[row][0] += urd(gen);
        }

        Me sol2;
        try {
            sol2 = SLEGM(B, g);
        } catch (domain_error &e) {
            cerr << e.what() << endl;
            continue;
        }

        //   Counting standard deviation
        element_type dif = 0;
        for (int row = 0; row < sol1.get_rows(); ++row) {
            dif += pow(sol1[row][0] - sol2[row][0], 2);
        }
        dif = sqrt(dif / sol1.get_rows());

        max_deviation = max(max_deviation, dif);

        //   Print it out
        ofstream fout(get_fout_for_test(stability, "stab", i + 1,
                tester.get_num_tests()));
        fout << dif << endl;
        fout.close();
    }
    cout << "\tMaximum standard deviation: " << max_deviation << endl;
    
    
    //   Counting speed of convergence rate of iterations
    cout << "Counting speed of convergence rate of iterations" << endl;

    const string iter_cov = "iter_convergance/";
    new_folder(iter_cov);

    tester = Tests::create_tester_with_tests<element_type>();
    for (int i = 0; i < tester.get_num_tests(); ++i) {
        auto test = tester.next_test();
        auto A = test.first;
        auto f = test.second;

        const double eps2 = 1e-3;

        int min_iters = -1;
        double wmin = -1;

        //   Looking for minimum number of iterations for every w with 
        // step eps2
        for (double w = eps2; w <= 2 - eps2; w += eps2) {
            //   Run SOR solver with w and count number of iterations
            int iters = -1;
            try {
                SLE_SOR(A, f, w, &iters);
            } catch (domain_error &e) {
                cerr << e.what() << endl;
                continue;
            }

            if (iters == -1) {
                continue;
            }

            //   Find minimum number of iterations
            if (min_iters == -1 || min_iters > iters) {
                min_iters = iters;
                wmin = w;
            }
        }

        // Print minimum number of iterations and coresponding w
        ofstream fout(get_fout_for_test(iter_cov, "cov", i + 1,
                tester.get_num_tests()));
        fout << min_iters << " (" << wmin << ")" << endl;
        fout.close();
    }

    return 0;
}
