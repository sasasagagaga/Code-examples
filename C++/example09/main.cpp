#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    string s;
    getline(cin, s);
    while (isspace(static_cast<unsigned char>(s.back()))) {
        s.pop_back();
    }

    vector<size_t> ans(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        ans[i] = i;
    }

    sort(ans.begin(), ans.end(), [&s](size_t i, size_t j) {
        return string_view(s).substr(i) < string_view(s).substr(j);
    });

    for (auto x : ans) {
        cout << x << endl;
    }
}