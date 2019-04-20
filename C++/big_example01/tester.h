// tester.h

//   Here I define class Tester. I use class Tester for testing SLE solvers


#ifndef TESTER_INCLUDE_GUARD
#define TESTER_INCLUDE_GUARD

#include <iostream>  // ostream
#include <vector>    // vector

//   I use this class to test many of written algorithms. I just create 
// instance of class Tester, then I add tests to it, and then I can run 
// all these tests to test what I want.
template <class T, class A>
class Tester {
private:
    static const std::string exception_prefix;
    static const std::string exception_no_tests;
    static const std::string exception_incorrect_implementation;

    //   Tests and answers to tests
    std::vector<T> tests;
    std::vector<A> answers;

    //   Variables 'cur_test' and 'last_test_given' are used to track the 
    // work with current test. 'last_test_given' variable shows the number 
    // of last test which was given. 'cur_test' variable shows the number 
    // of test which will be proceeded next
    mutable size_t cur_test = 0;
    mutable size_t last_test_given = 0;

public:
    //   Constructor
    Tester();

    //   Returns number of tests in collection
    auto inline get_num_tests() const;

    //   Get current 'running' test
    auto inline get_cur_test() const;

    //   Set current 'running' test
    void set_cur_test(size_t new_cur_test) const;

    //   Start testing from beginning
    void reset_testing() const;

    //   Add new test to collection
    void add_test(const T &test, const A &answer);

    //   Returns next test from collection. If tests run out, 
    // they go around the circle
    const T &next_test() const;

    //   Compare user answer with right answer
    bool check_answer(const A &usr_ans, std::ostream &out = std::cout, 
            std::ostream &err = std::cerr) const;


    //   Run all tests on solution function
    //   Returns true if all tests passed and false otherwise
    template <class F>
    bool run_all_tests(F solution, std::ostream &out = std::cout) const;
};


template <class T, class A>
const std::string Tester<T, A>::exception_prefix = "class Tester: ";

template <class T, class A>
const std::string Tester<T, A>::exception_no_tests = exception_prefix + 
        "there are no tests in collection";

template <class T, class A>
const std::string Tester<T, A>::exception_incorrect_implementation = 
        exception_prefix + "incorrect implementation: ";

template <class T, class A>
Tester<T, A>::Tester()
{
    cur_test = 0;
    last_test_given = 0;
}

template <class T, class A>
auto inline Tester<T, A>::get_num_tests() const
{
    return tests.size();
}

template <class T, class A>
auto inline Tester<T, A>::get_cur_test() const
{
    return cur_test;
}

template <class T, class A>
void Tester<T, A>::set_cur_test(size_t new_cur_test) const
{
    if (get_num_tests() == 0) {
        new_cur_test = 0;
    } else {
        new_cur_test = std::min(new_cur_test, get_num_tests() - 1);
    }

    cur_test = new_cur_test;
    last_test_given = new_cur_test;
}

template <class T, class A>
void Tester<T, A>::reset_testing() const
{
    set_cur_test(0);
}

template <class T, class A>
void Tester<T, A>::add_test(const T &test, const A &answer)
{
    tests.push_back(test);
    answers.push_back(answer);
}

template <class T, class A>
const T &Tester<T, A>::next_test() const
{
    //   If there are no tests we can't give at least one test, so stop
    if (get_num_tests() == 0) {
        throw std::out_of_range(exception_no_tests);
    }

    //   If tests ended we start testing from the beginning. I compare 
    // 'cur_test' here because 'cur_test' is test which will be proceeded 
    // now, after 'next_test' call
    if (cur_test >= get_num_tests()) {
        reset_testing();
    }

    last_test_given = cur_test;
    ++cur_test;

    return tests[last_test_given];
}

template <class T, class A>
bool Tester<T, A>::check_answer(const A &usr_ans, 
        std::ostream &out, std::ostream &err) const
{
    if (get_num_tests() == 0) {
        throw std::out_of_range(exception_no_tests);
    }

    //   If tests ended it means that there is error in realisation. 
    // I compare 'last_test_given' here because 'last_test_given' is 
    // test which is proceeding now
    if (last_test_given >= get_num_tests()) {
        throw std::runtime_error(exception_incorrect_implementation + 
                "last_test_given can't be equal or greater then number "
                "of tests");
    }

    //   Compare user answer 'usr_ans' and right answer
    bool ret_val = (answers[last_test_given] == usr_ans);

    //   Print information about testing
    //   OK - test passed, WA - wrong answer, PR - pending review (for 
    // cases when there is no answer for test)
    if (answers[last_test_given].get_rows() == 0) {
        out << "[PR] Test #" << last_test_given + 1 << " can't be checked\n";

        err << "[PR] Test #" << last_test_given + 1 << " can't be checked\n";
    } else if (ret_val) {
        out << "[OK] Test #" << last_test_given + 1 << " passed\n";

        err << "[OK] Test #" << last_test_given + 1 << " passed\n";
    } else {
        out << "[WA] Test #" << last_test_given + 1 << " failed\n";

        err << "[WA] Test #" << last_test_given + 1 << " failed:\n";
        err << "Test:\n" << tests[last_test_given] << std::endl;
        err << "Rigth answer:\n" << answers[last_test_given] << std::endl;
        err << "Your answer:\n" << usr_ans << std::endl;
    }

    return ret_val;
}

template <class T, class A>
template <class F>
bool Tester<T, A>::run_all_tests(F solution, std::ostream &out) const 
{
    //   Start testing from the beginning
    reset_testing();

    //   Run all tests
    bool all_tests_passed = true;
    for (int i = 0; i < get_num_tests(); ++i) {
        //   Here I handle with exceptions which solution can throw
        try {
            all_tests_passed &= check_answer(solution(next_test()));
        } catch (std::logic_error &e) {
            all_tests_passed = false;
            out << e.what() << std::endl;
        }
    }

    //   Print message about testing
    if (all_tests_passed) {
        out << "Testing finished successfully!\n";
    } else {
        out << "Some tests were failed\n";
    }

    return all_tests_passed;
}

#endif // TESTER_INCLUDE_GUARD
