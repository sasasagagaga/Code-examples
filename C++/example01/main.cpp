#include <iostream>
#include <cmath>
#include <vector>
#include <future>
#include <iomanip>

using namespace std;

pair<unsigned, unsigned> work(unsigned ind, unsigned cnt_iters)
{
    unsigned cnt_inside = 0;
    for (unsigned i = 0; i < cnt_iters; ++i) {
        double x = rand_r(&ind) / (RAND_MAX + 1.);
        double y = rand_r(&ind) / (RAND_MAX + 1.);
        cnt_inside += x * x + y * y <= 1;
    }

    return {cnt_inside, cnt_iters};
}

int main(int argc, char *argv[]) {
    unsigned thr_cnt = stoul(argv[1]);
    unsigned iter_cnt = stoul(argv[2]);

    vector<future<pair<unsigned, unsigned>>> fs(thr_cnt);
    for (unsigned i = 0; i < thr_cnt; ++i) {
        fs[i] = async(launch::async, work, i, iter_cnt);
    }

    unsigned cnt_inside = 0, cnt_all = 0;
    for (auto &f : fs) {
        auto res = f.get();
        cnt_inside += res.first;
        cnt_all += res.second;
    }

    cout << setprecision(5) << fixed << 4. * cnt_inside / cnt_all << endl;
}