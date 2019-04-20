// types.h

//   File with DFL_NUM_STEPS constant, types which are for internal usage 
// and function 'generate_answer' for generating mesh function from real 
// solution

#ifndef TYPES_INCLUDE_GUARD
#define TYPES_INCLUDE_GUARD

#include <vector>       // std::vector
#include <ostream>      // std::ostream
#include <functional>   // std::function


namespace Constants
{
    enum
    {
        DFL_NUM_STEPS = 10000,
    };
}

namespace Types
{
    using Function1ArgsType = std::function<double(double)>;
    using Function2ArgsType = std::function<double(double, double)>;
    using Function3ArgsType = std::function<double(double, double, double)>;

    //   Types for 1st-order ODE
    struct ODETestType
    {
        Function2ArgsType f;
        double x0, xn;
        double y0;

        ODETestType(Function2ArgsType F, double X0, double Xn, double Y0)
        {
            f = F;
            x0 = X0;
            xn = Xn;
            y0 = Y0;
        }

        ODETestType() = default;
    };

    struct ODEAnswerType
    {
        std::vector<double> ans;

        ODEAnswerType() = default;

        ODEAnswerType(const std::vector<double> &another);

        double operator - (const ODEAnswerType &another) const;

        size_t size() const;

        std::ostream &print(std::ostream &out) const;
    };

    //   Types for system of 1st-order ODEs
    struct ODESystemTestType
    {
        Function3ArgsType f1, f2;
        double x0, xn;
        double y10, y20;

        ODESystemTestType(Function3ArgsType F1, Function3ArgsType F2, 
                double X0, double Xn, double Y10, double Y20)
        {
            f1 = F1;
            f2 = F2;
            x0 = X0;
            xn = Xn;
            y10 = Y10;
            y20 = Y20;
        }

        ODESystemTestType() = default;
    };

    struct ODESystemAnswerType
    {
        ODEAnswerType ans1, ans2;

        ODESystemAnswerType() = default;

        ODESystemAnswerType(const std::vector<double> &another1, 
                const std::vector<double> &another2);

        double operator - (const ODESystemAnswerType &another) const;

        size_t size() const;

        std::ostream &print(std::ostream &out) const;
    };

    //   Types for BVP
    struct BVPTestType
    {
        Function1ArgsType p, q, f;
        double sigma1, sigma2, gamma1, gamma2, delta1, delta2;
        double x0, xn;

        BVPTestType (Function1ArgsType P, Function1ArgsType Q, 
                Function1ArgsType F, double X0, double Xn, double Sigma1, 
                double Gamma1, double Delta1, double Sigma2, double Gamma2, 
                double Delta2)
        {
            p = P;
            q = Q;
            f = F;
            x0 = X0;
            xn = Xn;
            sigma1 = Sigma1;
            gamma1 = Gamma1;
            delta1 = Delta1;
            sigma2 = Sigma2;
            gamma2 = Gamma2;
            delta2 = Delta2;
        }

        BVPTestType() = default;
    };

    using BVPAnswerType = ODEAnswerType;
}

namespace Functions
{
    //   Generates answer (vector of points) for test using right 
    // solution (function)
    std::vector<double> generate_answer(Types::Function1ArgsType f, 
            double x0, double xn, int num_steps = Constants::DFL_NUM_STEPS);
}

#endif // TYPES_INCLUDE_GUARD
