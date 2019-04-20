#include <iostream>
#include <sstream>
#include <functional>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    map<char, function<Figure *(const string &)>> makes = {
        { 'R', Rectangle::make },
        { 'S', Square::make },
        { 'C', Circle::make }
    };
    
    vector<Figure *> figs;

    string s;
    while (getline(cin, s)) {
        istringstream ss(s);
        char type;
        ss >> type;
        getline(ss, s);

        figs.emplace_back(makes[type](s));
    }

    stable_sort(figs.begin(), figs.end(), [] (Figure *f1, Figure *f2) {
        return f1->get_square() < f2->get_square();
    });

    for (Figure *f : figs) {
        cout << f->to_string() << endl;
    }

    for (Figure *f : figs) {
        delete f;
    }
}
