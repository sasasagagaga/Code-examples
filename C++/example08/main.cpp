
#include <iostream>
#include <cmath>
#include <cstring>

using namespace std;

namespace numbers
{
    class complex
    {
    public:
        constexpr static size_t buf_size = 100;

        complex(double a_ = 0, double b_ = 0)
            : a(a_), b(b_)
        {
        }

        explicit complex(const char *s)
        {
            sscanf(s, "(%lf,%lf)", &a, &b);
        }

        double get_re() const
        {
            return a;
        }

        double get_im() const
        {
            return b;
        }

        double abs2() const
        {
            return a * a + b * b;
        }

        double abs() const
        {
            return sqrt(abs2());
        }

        char *to_string(char *buf, size_t size) const
        {
            snprintf(buf, size, "(%.10g,%.10g)", a, b);
            return buf;
        }

        complex operator ~ () const
        {
            return complex(a, -b);
        }

        complex operator - () const
        {
            return complex(-a, -b);
        }

    private:
        double a = 0, b = 0;
    };

    complex operator + (const complex &a, const complex &c)
    {
        return complex(a.get_re() + c.get_re(), a.get_im() + c.get_im());
    }

    complex operator - (const complex &a, const complex &c)
    {
        return complex(a.get_re() - c.get_re(), a.get_im() - c.get_im());
    }

    complex operator * (const complex &a, const complex &c)
    {
        return complex(a.get_re() * c.get_re() - a.get_im() * c.get_im(),
                a.get_re() * c.get_im() + a.get_im() * c.get_re());
    }

    complex operator / (const complex &a, const complex &c)
    {
        complex c2 = a * (~c);
        return complex(c2.get_re() / c.abs2(), c2.get_im() / c.abs2());
    }


    class complex_stack
    {
    public:
        complex_stack()
        {
            sz = 0;
            alloc = 4;
            data = new complex[alloc];
        }

        complex_stack(const complex_stack &st)
        {
            if (this != &st) {
                sz = st.sz;
                alloc = st.alloc;
                data = new complex[alloc];
                memcpy(data, st.data, alloc * sizeof(data[0]));
            }
        }

        complex_stack(complex_stack &&st)
        {
            if (this != &st) {
                sz = st.sz;
                alloc = st.alloc;
                data = st.data;
                st.data = nullptr;
            }
        }

        size_t size() const
        {
            return sz;
        }

        complex operator [] (size_t i) const
        {
            if (i < sz) {
                return data[i];
            }
            return complex();
        }

        complex_stack operator << (const complex &c) const
        {
            complex_stack st = *this;
            st.push(c);
            return st;
        }

        complex operator + () const
        {
            if (sz) {
                return data[sz - 1];
            }
            return complex();
        }

        complex_stack operator ~ () const
        {
            complex_stack st = *this;
            st.sz--;
            return st;
        }

        complex_stack &operator = (const complex_stack &st)
        {
            if (this != &st) {
                sz = st.sz;
                alloc = st.alloc;
                delete[] data;
                data = new complex[alloc];
                memcpy(data, st.data, alloc * sizeof(data[0]));
            }
            return *this;
        }

        complex_stack &operator = (complex_stack &&st)
        {
            if (this != &st) {
                sz = st.sz;
                alloc = st.alloc;
                delete[] data;
                data = st.data;
                st.data = nullptr;
            }
            return *this;
        }

        ~complex_stack()
        {
            delete[] data;
        }

    private:
        size_t sz = 0, alloc = 0;
        complex *data = nullptr;

        void push(const complex &c)
        {
            if (sz == alloc) {
                complex *tmp = new complex[alloc * 2];
                memcpy(tmp, data, alloc * sizeof(data[0]));
                alloc *= 2;
                delete[] data;
                data = tmp;
            }

            data[sz++] = c;
        }
    };

    complex eval(char **args, const complex &z)
    {
        complex_stack st;
        for (int i = 0; args[i]; ++i) {
            complex a, b;

            switch (args[i][0]) {
            case '(':
                st = (st << complex(args[i]));
                break;
            case 'z':
                st = (st << z);
                break;
            case '+':
                a = +st;
                st = ~st;
                b = +st;
                st = ~st;

                st = (st << (a + b));
                break;
            case '*':
                a = +st;
                st = ~st;
                b = +st;
                st = ~st;

                st = (st << (a * b));
                break;
            case '/':
                a = +st;
                st = ~st;
                b = +st;
                st = ~st;

                st = (st << (b / a));
                break;
            case '-':
                a = +st;
                st = ~st;
                b = +st;
                st = ~st;

                st = (st << (b - a));
                break;
            case '!':
                a = +st;
                st = (st << a);
                break;
            case ';':
                st = ~st;
                break;
            case '~':
                a = +st;
                a = ~a;
                st = ~st;
                st = (st << a);
                break;
            case '#':
                a = +st;
                a = -a;
                st = ~st;
                st = (st << a);
                break;
            }
        }

        return +st;
    }
}

using namespace numbers;

int main(int argc, char **argv)
{
    complex c(argv[1]);
    double r = stod(argv[2]);
    int n = stoi(argv[3]);

    complex ans = 0;
    auto prev = c + r;
    for (int i = 1; i <= n; ++i) {
        auto arg = 2. * M_PI * i / n;
        auto cur = c + r * complex(cos(arg), sin(arg));
        ans = ans + eval(argv + 4, (cur + prev) / 2.) * (cur - prev);
        prev = cur;
    }

    char buf[complex::buf_size];
    cout << ans.to_string(buf, complex::buf_size) << endl;
}

