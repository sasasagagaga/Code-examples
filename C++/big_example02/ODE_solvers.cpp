// ODE_solvers.cpp

//   File with ODE and BVP solvers

#include "ODE_solvers.h"
#include <vector>        // std::vector
#include <cmath>         // pow

using namespace Types;
using namespace Constants;


ODEAnswerType RK_solvers::solve_RK2(Function2ArgsType f, 
        double x0, double xn, double y0, double alpha)
{
    const int num_steps = DFL_NUM_STEPS;

    std::vector<double> y(num_steps + 1);
    y[0] = y0;

    double h = (xn - x0) / num_steps;
    for (int i = 0; i < num_steps; ++i) {
        double xi = x0 + i * h;
        double fi = f(xi, y[i]);
        double step = h / 2 / alpha;

        y[i + 1] = y[i] + (
                (1 - alpha) * fi + 
                alpha * f(xi + step, y[i] + step * fi)
                ) * h;
    }

    return y;
}

ODEAnswerType RK_solvers::solve_RK2_wrap(const ODETestType &test)
{
    return RK_solvers::solve_RK2(test.f, test.x0, test.xn, test.y0);
}

ODEAnswerType RK_solvers::solve_RK4(Function2ArgsType f, 
        double x0, double xn, double y0)
{
    const int num_steps = DFL_NUM_STEPS;

    std::vector<double> y(num_steps + 1);
    y[0] = y0;

    double h = (xn - x0) / num_steps;
    for (int i = 0; i < num_steps; ++i) {
        double xi = x0 + i * h;
        double step = h / 2;

        double k1 = f(xi, y[i]);
        double k2 = f(xi + step, y[i] + step * k1);
        double k3 = f(xi + step, y[i] + step * k2);
        double k4 = f(xi + h, y[i] + h * k3);

        y[i + 1] = y[i] + (k1 + 2 * k2 + 2 * k3 + k4) / 6 * h;
    }

    return y;
}

ODEAnswerType RK_solvers::solve_RK4_wrap(const ODETestType &test)
{
    return RK_solvers::solve_RK4(test.f, test.x0, test.xn, test.y0);
}

ODESystemAnswerType RK_solvers::solve_system_RK2(
        Function3ArgsType f1, Function3ArgsType f2, 
        double x0, double xn, double y10, double y20)
{
    const int num_steps = DFL_NUM_STEPS;

    std::vector<double> y1(num_steps + 1), y2(num_steps + 1);
    y1[0] = y10;
    y2[0] = y20;

    double h = (xn - x0) / num_steps;
    for (int i = 0; i < num_steps; ++i) {
        double xi = x0 + i * h;
        double step = h / 2;

        double k1 = f1(xi, y1[i], y2[i]);
        double m1 = f2(xi, y1[i], y2[i]);

        double k2 = f1(xi + h, y1[i] + h * k1, y2[i] + h * m1);
        double m2 = f2(xi + h, y1[i] + h * k1, y2[i] + h * m1);

        y1[i + 1] = y1[i] + step * (k1 + k2);
        y2[i + 1] = y2[i] + step * (m1 + m2);
    }

    return ODESystemAnswerType(y1, y2);
}

ODESystemAnswerType RK_solvers::solve_system_RK2_wrap(
        const ODESystemTestType &test)
{
    return RK_solvers::solve_system_RK2(test.f1, test.f2, 
            test.x0, test.xn, test.y10, test.y20);
}

ODESystemAnswerType RK_solvers::solve_system_RK4(
        Function3ArgsType f1, Function3ArgsType f2, 
        double x0, double xn, double y10, double y20)
{
    const int num_steps = DFL_NUM_STEPS;

    std::vector<double> y1(num_steps + 1), y2(num_steps + 1);
    y1[0] = y10;
    y2[0] = y20;

    double h = (xn - x0) / num_steps;
    for (int i = 0; i < num_steps; ++i) {
        double xi = x0 + i * h;
        double step = h / 2;

        double k1 = f1(xi, y1[i], y2[i]);
        double m1 = f2(xi, y1[i], y2[i]);

        double k2 = f1(xi + step, y1[i] + step * k1, y2[i] + step * m1);
        double m2 = f2(xi + step, y1[i] + step * k1, y2[i] + step * m1);

        double k3 = f1(xi + step, y1[i] + step * k2, y2[i] + step * m2);
        double m3 = f2(xi + step, y1[i] + step * k2, y2[i] + step * m2);

        double k4 = f1(xi + h, y1[i] + h * k3, y2[i] + h * m3);
        double m4 = f2(xi + h, y1[i] + h * k3, y2[i] + h * m3);

        y1[i + 1] = y1[i] + (k1 + 2 * k2 + 2 * k3 + k4) / 6 * h;
        y2[i + 1] = y2[i] + (m1 + 2 * m2 + 2 * m3 + m4) / 6 * h;
    }

    return ODESystemAnswerType(y1, y2);
}

ODESystemAnswerType RK_solvers::solve_system_RK4_wrap(
        const ODESystemTestType &test)
{
    return RK_solvers::solve_system_RK4(test.f1, test.f2, 
            test.x0, test.xn, test.y10, test.y20);
}


BVPAnswerType FDM_solvers::solve_FDM2(Types::Function1ArgsType p, 
        Types::Function1ArgsType q, Types::Function1ArgsType f,
        double x0, double xn, double sigma1, double gamma1, 
        double delta1, double sigma2, double gamma2, double delta2)
{
    const int num_steps = DFL_NUM_STEPS;

    double h = (xn - x0) / num_steps;

    std::vector<double> k(num_steps), m(num_steps);
    for (int i = 0; i <= num_steps - 2; ++i) {
        double xi = x0 + i * h;
        double pi = p(xi);
        double qi = q(xi);

        m[i] = -2 + h * pi;
        k[i] = 1 - h * pi + h * h * qi;
    }

    std::vector<double> c(num_steps), d(num_steps);

    double coef = gamma1 - sigma1 * h;
    double f0 = f(x0);
    c[0] = coef / (m[0] * coef + k[0] * gamma1);
    d[0] = k[0] * delta1 * h / coef + f0 * pow(h, 2);

    for (int i = 1; i <= num_steps - 2; ++i) {
        double xi = x0 + i * h;
        double fi = f(xi);

        c[i] = 1 / (m[i] - k[i] * c[i - 1]);
        d[i] = fi * pow(h, 2) - k[i] * c[i - 1] * d[i - 1];
    }

    
    std::vector<double> y(num_steps + 1);
    y[num_steps] = (gamma2 * c[num_steps - 2] * d[num_steps - 2] + 
            delta2 * h) / (gamma2 * (1 + c[num_steps - 2]) + sigma2 * h);
    for (int i = num_steps - 1; i > 0; --i) {
        y[i] = c[i - 1] * (d[i - 1] - y[i + 1]);
    }
    y[0] = (gamma1 * y[1] - delta1 * h) / coef;

    return ODEAnswerType(y);
}

BVPAnswerType FDM_solvers::solve_FDM2_wrap(const Types::BVPTestType &test)
{
    return FDM_solvers::solve_FDM2(test.p, test.q, test.f, test.x0, test.xn, 
            test.sigma1, test.gamma1, test.delta1, test.sigma2, test.gamma2, 
            test.delta2);
}
