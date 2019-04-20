// ODE_solvers.h

//   File with ODE and BVP solvers

#ifndef ODE_SOLVERS_INCLUDE_GUARD
#define ODE_SOLVERS_INCLUDE_GUARD

#include "types.h"
#include <vector>   // std::vector


namespace RK_solvers
{
    //   1st-order ODE: y' = f(x, y), x0 < x
    Types::ODEAnswerType solve_RK2(Types::Function2ArgsType f, 
            double x0, double xn, double y0, double alpha = 0.5);

    Types::ODEAnswerType solve_RK2_wrap(const Types::ODETestType &test);

    Types::ODEAnswerType solve_RK4(Types::Function2ArgsType f, double x0, 
            double xn, double y0);

    Types::ODEAnswerType solve_RK4_wrap(const Types::ODETestType &test);


    //   System of 1st-order ODEs: y1' = f1(x, y1, y2), 
    // y2' = f2(x, y1, y2), x0 < x
    Types::ODESystemAnswerType solve_system_RK2(
            Types::Function3ArgsType f1, Types::Function3ArgsType f2, 
            double x0, double xn, double y10, double y20);

    Types::ODESystemAnswerType solve_system_RK2_wrap(
            const Types::ODESystemTestType &test);

    Types::ODESystemAnswerType solve_system_RK4(
            Types::Function3ArgsType f1, Types::Function3ArgsType f2, 
            double x0, double xn, double y10, double y20);

    Types::ODESystemAnswerType solve_system_RK4_wrap(
            const Types::ODESystemTestType &test);
}


namespace FDM_solvers
{
    //   2nd-order ODE: y'' + p(x)*y' + q(x)*y = f(x), 0 < x < 1
    Types::BVPAnswerType solve_FDM2(Types::Function1ArgsType p, 
            Types::Function1ArgsType q, Types::Function1ArgsType f,
            double sigma1, double gamma1, double delta1,
            double sigma2, double gamma2, double delta2,
            double x0, double xn);

    Types::BVPAnswerType solve_FDM2_wrap(const Types::BVPTestType &test);
}

#endif // ODE_SOLVERS_INCLUDE_GUARD
