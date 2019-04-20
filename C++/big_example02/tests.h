// tests.h

//   File with functions which fill Tester object with tests

#ifndef TESTS_INCLUDE_GUARD
#define TESTS_INCLUDE_GUARD

#include "tester.h"
#include "types.h"


namespace Tests
{
    //   Fills Tester object with some predefined tests for usual 1st-order 
    // ODE equation
    void create_ODE_tests(Tester<Types::ODETestType, Types::ODEAnswerType> 
            &tester);

    //   Fills Tester object with some predefined tests for usual 1st-order 
    // system of 2 ODE equations
    void create_ODE_system_tests(Tester<Types::ODESystemTestType, 
            Types::ODESystemAnswerType> &tester);

    //   Fills Tester object with some predefined tests for linear 2nd-order 
    // ODE (BVP)
    void create_BVP_tests(Tester<Types::BVPTestType, Types::BVPAnswerType> 
            &tester);
}

#endif // TESTS_INCLUDE_GUARD
