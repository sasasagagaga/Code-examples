#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;

void func(vector<mutex> &ms, size_t cnt_iters, vector<double> &accs, size_t a1, double d1, size_t a2, double d2)
{
    if (a1 > a2) {
        swap(a1, a2);
        swap(d1, d2);
    }
    for (size_t i = 0; i < cnt_iters; ++i) {
        lock_guard<mutex> lg1(ms[a1]);
        lock_guard<mutex> lg2(ms[a2]);

        accs[a1] += d1;
        accs[a2] += d2;
    }
}

int main()
{
    uint32_t acc_count, thr_count;
    cin >> acc_count >> thr_count;

    vector<double> accs(acc_count, 0);
    vector<mutex> ms(acc_count);

    vector<thread> ts(thr_count);
    for (size_t i = 0; i < thr_count; ++i) {
        uint32_t cnt_iters, a1, a2;
        double d1, d2;
        cin >> cnt_iters >> a1 >> d1 >> a2 >> d2;

        thread t(func, ref(ms), cnt_iters, ref(accs), a1, d1, a2, d2);
        ts[i] = move(t);
    }

    for (auto &t : ts) {
        t.join();
    }

    for (auto d : accs) {
        printf("%.10g\n", d);
    }
}