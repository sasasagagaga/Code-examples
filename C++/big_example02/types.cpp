// types.cpp

#include "types.h"
#include <cmath>      // abs
#include <vector>     // std::vector
#include <ostream>    // std::ostream

using namespace Types;


//   ODEAnswerType
ODEAnswerType::ODEAnswerType(const std::vector<double> &another)
{
    ans = another;
}

double ODEAnswerType::operator - (const ODEAnswerType &another) const
{
    if (ans.size() != another.size()) {
        return std::numeric_limits<double>::infinity();
    }

    double max_dif = 0;
    for (int i = 0; i < ans.size(); ++i) {
        max_dif = std::max(max_dif, abs(ans[i] - another.ans[i]));
    }

    return max_dif;
}

size_t ODEAnswerType::size() const
{
    return ans.size();
}

std::ostream &ODEAnswerType::print(std::ostream &out) const
{
    for (const auto &val : ans) {
        out << val << ' ';
    }

    return out;
}


//   ODESystemAnswerType
ODESystemAnswerType::ODESystemAnswerType(const std::vector<double> 
        &another1, const std::vector<double> &another2)
{
    ans1 = another1;
    ans2 = another2;
}

double ODESystemAnswerType::operator - (
        const ODESystemAnswerType &another) const
{
    return std::max(ans1 - another.ans1, ans2 - another.ans2);
}

size_t ODESystemAnswerType::size() const
{
    return std::max(ans1.size(), ans2.size());
}

std::ostream &ODESystemAnswerType::print(std::ostream &out) const
{
    ans1.print(out);
    out << std::endl;
    ans2.print(out);

    return out;
}

//   namespace Functions
std::vector<double> Functions::generate_answer(Function1ArgsType f, 
        double x0, double xn, int num_steps)
{
    if (!f) {
        return std::vector<double>();
    }

    std::vector<double> ans(num_steps + 1);

    double h = (xn - x0) / num_steps;
    for (int i = 0; i <= num_steps; ++i) {
        ans[i] = f(x0 + i * h);
    }

    return ans;
}
