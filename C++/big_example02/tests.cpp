// tests.cpp

//   File with functions which fill Tester object with tests

#include "tests.h"
#include "tester.h"
#include "types.h"
#include <cmath>    // pow
#include <vector>   // std::vector

using namespace Types;


void Tests::create_ODE_tests(Tester<ODETestType, ODEAnswerType> &tester)
{
    const double e = 2.71828182845904523536;

    std::vector<std::pair<ODETestType, Function1ArgsType>> packed_tests;

    ODETestType test;
    Function2ArgsType f;
    Function1ArgsType sol;

    //   Tests from task
    // Test #1
    f = [](double x, double y) { return 3 - y - x; };
    test = { f, 0, 3, 0 };
    sol = [e](double x) { return 4 - x - 4 * pow(e, -x); };
    packed_tests.push_back(std::make_pair(test, sol));

    // Test #2
    f = [](double x, double y) { return sin(x) - y; };
    test = { f, 0, 3, 10 };
    sol = [e](double x) {
        return -0.5 * cos(x) + 0.5 * sin(x) + 21. / 2. * pow(e, -x);
    };
    packed_tests.push_back(std::make_pair(test, sol));

    // Test #3 <- test from my variant
    f = [](double x, double y) { return -y - x * x; };
    test = { f, 0, 3, 10 };
    sol = NULL;
    packed_tests.emplace_back(test, sol);

    // Test #4
    f = [](double x, double y) { return y - y * x; };
    test = { f, 0, 3, 5 };
    sol = [e](double x) { return 5 * pow(e, -0.5 * x * (x - 2)); };
    packed_tests.push_back(std::make_pair(test, sol));

    // Test #5
    f = [](double x, double y) { return (y - pow(y, 2)) * x; };
    test = { f, 0, 3, 3 };
    sol = [e](double x) {
        return 1 / (1 - 2. / 3. * pow(e, -0.5 * pow(x, 2)));
    };
    packed_tests.push_back(std::make_pair(test, sol));

    // Test #6
    f = [](double x, double y) { return (x - pow(x, 2)) * y; };
    test = { f, 0, 3, 1 };
    sol = [e](double x) {
        return pow(e, -1. / 6. * pow(x, 2) * (2 * x - 3));
    };
    packed_tests.push_back(std::make_pair(test, sol));


    //  My tests (made and checked by myself): 
    // y' = x, y(0) = 3 => y = x^2 / 2 + 3
    f = [](double x, double y) { return x; };
    test = { f, 0, 3, 3 };
    sol = [](double x) { return pow(x, 2) / 2 + 3; };
    packed_tests.push_back(std::make_pair(test, sol));

    // y' = xy, y(0) = 2.5 => y = 2.5 * exp(x^2 / 2)
    f = [](double x, double y) { return x * y; };
    test = { f, 0, 3, 2.5 };
    sol = [e](double x) { return 2.5 * pow(e, pow(x, 2) / 2); };
    packed_tests.push_back(std::make_pair(test, sol));

    // y' = 4x^3 + 3x^2 + 2x + 1, y(0) = 1 => y = x^4 + x^3 + x^2 + x + 1
    f = [](double x, double y) {
        return 4 * pow(x, 3) + 3 * pow(x, 2) + 2 * x + 1;
    };
    test = { f, 0, 3, 1 };
    sol = [](double x) {
        return pow(x, 4) + pow(x, 3) + pow(x, 2) + x + 1;
    };
    packed_tests.push_back(std::make_pair(test, sol));

    // y' = cos(x) * sin(x); y(0) = 1 => y = sin(x)^2 / 2 + 1
    f = [](double x, double y) { return cos(x) * sin(x); };
    test = { f, 0, 3, 1 };
    sol = [](double x) { return pow(sin(x), 2) / 2 + 1; };
    packed_tests.push_back(std::make_pair(test, sol));


    //   Unpacking packed tests and stroring them in Tester object
    for (auto test : packed_tests) {
        ODEAnswerType ans = Functions::generate_answer(
                test.second, test.first.x0, test.first.xn);

        tester.add_test(test.first, ans);
    }
}


void Tests::create_ODE_system_tests(
        Tester<ODESystemTestType, ODESystemAnswerType> &tester)
{
    const double e = 2.71828182845904523536;

    std::vector<std::pair<ODESystemTestType, 
            std::pair<Function1ArgsType, Function1ArgsType>>> packed_tests;

    ODESystemTestType test;
    Function3ArgsType f1, f2;
    Function1ArgsType sol1, sol2;

    //   Tests for testing (they have answers)
    //  My tests (made and checked by myself):
    // y1' = x, y2' = x * y1; y1(0) = 0, y2(0) = 0
    // Answer: y1 = x^2 / 2, y2 = x^4 / 8
    f1 = [](double x, double u, double v) { return x; };
    f2 = [](double x, double u, double v) { return x * u; };
    test = { f1, f2, 0, 3, 0, 0 };
    sol1 = [](double x) { return x * x / 2; };
    sol2 = [](double x) { return pow(x, 4) / 8; };
    packed_tests.push_back(std::make_pair(test, make_pair(sol1, sol2)));

    // y1' = e^x, y2' = y1; y1(0) = 2, y2(0) = 2
    // Answer: y1 = e^x + 1, y2 = e^x + x + 1
    f1 = [e](double x, double u, double v) { return pow(e, x); };
    f2 = [](double x, double u, double v) { return u; };
    test = { f1, f2, 0, 3, 2, 2 };
    sol1 = [e](double x) { return pow(e, x) + 1; };
    sol2 = [e](double x) { return pow(e, x) + x + 1; };
    packed_tests.push_back(std::make_pair(test, make_pair(sol1, sol2)));

    // y1' = y2, y2' = -y1; y1(0) = 0, y2(0) = 1
    // Answer: y1 = sin(x), y2 = cos(x)
    f1 = [](double x, double u, double v) { return v; };
    f2 = [](double x, double u, double v) { return -u; };
    test = { f1, f2, 0, 3, 0, 1 };
    sol1 = [](double x) { return sin(x); };
    sol2 = [](double x) { return cos(x); };
    packed_tests.push_back(std::make_pair(test, make_pair(sol1, sol2)));


    //   Tests from task (some of them without answers)
    //  Test #1
    // I found solution on wolframalpha.com site. Here is a short link 
    // to solution: https://goo.gl/UuhH8E
    // So answer is: y1 = x(cos(log(x)) - sin(log(x)), 
    // y2 = x(cos(log(x)) + sin(log(x))
    f1 = [](double x, double u, double v) { return (u - v) / x; };
    f2 = [](double x, double u, double v) { return (u + v) / x; };
    test = { f1, f2, 1, 4, 1, 1 };
    sol1 = [](double x) { return x * (cos(log(x)) - sin(log(x))); };
    sol2 = [](double x) { return x * (cos(log(x)) + sin(log(x))); };
    packed_tests.push_back(std::make_pair(test, make_pair(sol1, sol2)));

    //  Test #10
    // Solution on wolframalpha.com: https://goo.gl/YHPtp8
    // Answer: y1 = -sin(x), y2 = 0
    f1 = [](double x, double u, double v) { return v - cos(x); };
    f2 = [](double x, double u, double v) { return u + sin(x); };
    test = { f1, f2, 0, 3, 0, 0 };
    sol1 = [](double x) { return -sin(x); };
    sol2 = [](double x) { return 0; };
    packed_tests.push_back(std::make_pair(test, make_pair(sol1, sol2)));

    //  Test #17
    f1 = [](double x, double u, double v) { 
        return sin(1.4 * pow(u, 2)) - x + v;
    };
    f2 = [](double x, double u, double v) {
        return x + u - 2.2 * pow(v, 2) + 1;
    };
    test = { f1, f2, 0, 3, 1, 0.5 };
    sol1 = NULL;
    sol2 = NULL;
    packed_tests.emplace_back(test, make_pair(sol1, sol2));


    //   Unpacking packed tests and stroring them in Tester object
    for (auto test : packed_tests) {
        auto f1 = test.second.first;
        auto f2 = test.second.second;
        auto x0 = test.first.x0;
        auto xn = test.first.xn;

        ODEAnswerType ans1 = Functions::generate_answer(f1, x0, xn);
        ODEAnswerType ans2 = Functions::generate_answer(f2, x0, xn);

        tester.add_test(test.first, { ans1.ans, ans2.ans });
    }
}


void Tests::create_BVP_tests(Tester<BVPTestType, BVPAnswerType> &tester)
{
    const double e = 2.71828182845904523536;
    const double Pi = 3.14159265358979323846;

    std::vector<std::pair<BVPTestType, Function1ArgsType>> packed_tests;

    BVPTestType test;
    Function1ArgsType p, q, f, sol;

    //   Tests for testing (tests with answers)
    //  My tests (made and checked by myself):
    // y'' - y = 0; y(0) = e, y(1) = 1 => y = exp(1 - x)
    p = [](double x) { return 0; };
    q = [](double x) { return -1; };
    f = [](double x) { return 0; };
    test = { p, q, f, 0, 1, 1, 0, e, 1, 0, 1 };
    sol = [e](double x) { return pow(e, 1 - x); };
    packed_tests.emplace_back(test, sol);

    // y'' - 2y' + y = 0; y(0) = 0, y(1) = e => y = xe^x
    p = [](double x) { return -2; };
    q = [](double x) { return 1; };
    f = [](double x) { return 0; };
    test = { p, q, f, 0, 1, 1, 0, 0, 1, 0, e };
    sol = [e](double x) { return x * pow(e, x); };
    packed_tests.emplace_back(test, sol);

    // y'' + y = 0; y'(0) + y(0) = 2, y'(Pi/2) + y(Pi/2) = 0 
    // => y = cos(x) + sin(x)
    p = [](double x) { return 0; };
    q = [](double x) { return 1; };
    f = [](double x) { return 0; };
    test = { p, q, f, 0, Pi / 2, 1, 1, 2, 1, 1, 0 };
    sol = [](double x) { return cos(x) + sin(x); };
    packed_tests.emplace_back(test, sol);

    // y'' - y' / x = 0; y'(1) = 2, y(4) = 0 => y = x^2 - 16
    p = [](double x) { return -1 / x; };
    q = [](double x) { return 0; };
    f = [](double x) { return 0; };
    test = { p, q, f, 1, 4, 0, 1, 2, 1, 0, 0 };
    sol = [](double x) { return pow(x, 2) - 16; };
    packed_tests.emplace_back(test, sol);


    //   Tests from task (some of them without answers)
    //  Test #8
    p = [](double x) { return 3; };
    q = [](double x) { return -1 / x; };
    f = [](double x) { return x + 1; };
    test = { p, q, f, 1.2, 1.5, 0, 1, 1, 2, -1, 0.5 };
    sol = NULL;
    packed_tests.emplace_back(test, sol);


    //   Unpacking packed tests and stroring them in Tester object
    for (auto test : packed_tests) {
        BVPAnswerType ans = Functions::generate_answer(
                test.second, test.first.x0, test.first.xn);

        tester.add_test(test.first, ans);
    }
}
