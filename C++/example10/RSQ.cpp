// Пример кода для спортивного программирования


/* RSQ */
#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;

typedef long double ld;

void build(int a[], int v, int tl, int tr);
int get_sum(int v, int tl, int tr, int l, int r);

const int maxn = 100000;
int t[4 * maxn];


int main()
{
    int n, a[maxn], k, l, r;

    cin >> n;
    for(int i = 0; i < n; ++i)
        cin >> a[i];

    build(a, 1, 0, n - 1);

    cin >> k;
    for(int i = 0; i < k; ++i)
    {
        cin >> l >> r;
        cout << get_sum(1, 0, n - 1, l - 1, r - 1) << endl;
    }

    return 0;
}

void build(int a[], int v, int tl, int tr)
{
    if (tl == tr) {
        t[v] = a[tl];
    } else {
        int tm = (tl + tr) / 2;
        build(a, 2 * v, tl, tm);
        build(a, 2 * v + 1, tm + 1, tr);
        t[v] = t[2 * v] + t[2 * v + 1];
    }
}

int get_sum(int v, int tl, int tr, int l, int r)
{
    if (l > r) {
        return 0;
    }
    if (l == tl && r == tr) {
        return t[v];
    }

    int tm = (tl + tr) / 2;
    return get_sum(2 * v, tl, tm, l, min(r, tm)) +
           get_sum(2 * v + 1, tm + 1, tr, max(l, tm + 1), r);
}







