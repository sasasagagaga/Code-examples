// main.cpp

//   Abbreviations:
// 1) RK - Runge Kutta
// 2) ODE - ordinary differential equation
// 3) FDM - finite difference method
// 4) BVP - boundary value problem

//   A main file with code. Here I tesing ODE and BVP solvers.

#include "tester.h"
#include "tests.h"
#include "types.h"
#include "ODE_solvers.h"
#include <iostream>       // std::cin, std::cout

using namespace std;
using namespace Types;


int main()
{
    cout << "\tODE solver testing\n";
    Tester<ODETestType, ODEAnswerType> ODE_tester;
    Tests::create_ODE_tests(ODE_tester);

    cout << "  RK2\n";
    ODE_tester.run_all_tests(RK_solvers::solve_RK2_wrap, cerr);
    cout << "  RK4\n";
    ODE_tester.run_all_tests(RK_solvers::solve_RK4_wrap, cerr);

    
    cout << "\n\tODE system solver testing\n";
    Tester<ODESystemTestType, ODESystemAnswerType> ODE_system_tester;
    Tests::create_ODE_system_tests(ODE_system_tester);

    cout << "  RK2\n";
    ODE_system_tester.run_all_tests(RK_solvers::solve_system_RK2_wrap, cerr);
    cout << "  RK4\n";
    ODE_system_tester.run_all_tests(RK_solvers::solve_system_RK4_wrap, cerr);


    cout << "\n\tBVP solver testing\n";
    Tester<BVPTestType, BVPAnswerType> BVP_tester;
    Tests::create_BVP_tests(BVP_tester);

    cout << "  FDM2\n";
    BVP_tester.run_all_tests(FDM_solvers::solve_FDM2_wrap, cerr);
}

