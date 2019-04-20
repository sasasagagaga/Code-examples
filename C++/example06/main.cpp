#include <string>
#include <sstream>
#include <tuple>
#include <algorithm>


template <class T>
class Range
{
private:
    T low = T();
    T high = T();

    template<typename U = T>
    void read(const std::string &s, typename std::enable_if<std::is_same<char, U>::value || std::is_same<signed char, U>::value>::type* = 0)
    {
        if (s.empty() || s[0] != '(' || s.back() != ')') {
            throw std::invalid_argument("couldn't parse a range");
        }

        char tmp;
        std::istringstream ss(s);
        short low_, high_;
        ss >> tmp >> low_ >> tmp >> high_ >> tmp;

        if (!ss) {
            if (low_ == std::numeric_limits<short>::min() || low_ == std::numeric_limits<short>::max()
                || high_ == std::numeric_limits<short>::min() || high_ == std::numeric_limits<short>::max()) {
                throw std::range_error("low or high overflowed");
            }
            throw std::invalid_argument("couldn't parse a range");
        }
        if (low_ < std::numeric_limits<signed char>::min() || low_ > std::numeric_limits<signed char>::max()
            || high_ < std::numeric_limits<signed char>::min() || high_ > std::numeric_limits<signed char>::max()) {
            throw std::range_error("low or high overflowed");
        }
        low = low_;
        high = high_;

        if (low > high) {
            throw std::invalid_argument("low should be not greater than high");
        }
    }

    template<typename U = T>
    void read(const std::string &s, typename std::enable_if<!(std::is_same<char, U>::value || std::is_same<signed char, U>::value) && std::is_fundamental<U>::value, int>::type* = 0)
    {
        if (s.empty() || s[0] != '(' || s.back() != ')') {
            throw std::invalid_argument("couldn't parse a range");
        }

        char tmp;
        std::istringstream ss(s);
        ss >> tmp >> low >> tmp >> high >> tmp;
        if (!ss) {
            if (low == std::numeric_limits<T>::min() || low == std::numeric_limits<T>::max()
                || high == std::numeric_limits<T>::min() || high == std::numeric_limits<T>::max()) {
                throw std::range_error("low or high overflowed");
            }
            throw std::invalid_argument("couldn't parse a range");
        }

        if (low > high) {
            throw std::invalid_argument("low should be not greater than high");
        }
    }

    template<typename U = T>
    void read(const std::string &s, typename std::enable_if<!std::is_fundamental<U>::value, short>::type* = 0)
    {
        if (s.empty() || s[0] != '(' || s.back() != ')') {
            throw std::invalid_argument("couldn't parse a range");
        }

        int cnt_primes = 0;
        size_t prime_pos = 0;
        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] == ',') {
                ++cnt_primes;
                prime_pos = i;

                if (cnt_primes > 1) {
                    throw std::invalid_argument("couldn't parse a range");
                }
            }
        }
        if (cnt_primes != 1) {
            throw std::invalid_argument("couldn't parse a range");
        }

        low = T(s.substr(1, prime_pos - 1));
        high = T(s.substr(prime_pos + 1, s.size() - prime_pos - 2));

        if (low > high) {
            throw std::invalid_argument("low should be not greater than high");
        }
    }


public:
    Range(T low_, T high_) : low(low_), high(high_)
    {
        static_assert((std::is_signed<T>::value && std::is_integral<T>::value) || !std::is_fundamental<T>::value);

        if (low > high) {
            throw std::invalid_argument("low should be not greater than high");
        }
    }

    Range(T val = T()) : low(val), high(T())
    {
        static_assert((std::is_signed<T>::value && std::is_integral<T>::value) || !std::is_fundamental<T>::value);

        if (low > high) {
            throw std::invalid_argument("low should be not greater than high");
        }
    }

    explicit Range(const std::string &s)
    {
        static_assert((std::is_signed<T>::value && std::is_integral<T>::value) || !std::is_fundamental<T>::value);

        read<T>(s);
    }

    std::string to_string() const
    {
        std::ostringstream ss;
        ss << '(' << +low << ',' << +high << ')';
        return ss.str();
    }

    T get_low() const
    {
        return low;
    }

    T get_high() const
    {
        return high;
    }
};


namespace RangeImpl
{

    template<class T>
    Range<typename std::enable_if<std::is_fundamental<T>::value, T>::type>
    operator + (const Range<T> &r1, const Range<T> &r2)
    {
        T a, b;
        if (__builtin_add_overflow(r1.get_low(), r2.get_low(), &a) || __builtin_add_overflow(r1.get_high(), r2.get_high(), &b)) {
            throw std::overflow_error("overflow");
        }
        return Range<T>(a, b);
    }

    template<class T>
    Range<typename std::enable_if<!std::is_fundamental<T>::value, T>::type>
    operator + (const Range<T> &r1, const Range<T> &r2)
    {
        T a, b;
        a = r1.get_low() + r2.get_low();
        b = r1.get_high() + r2.get_high();
        return Range<T>(a, b);
    }


    template <class T>
    Range<T> operator + (const T &r1, const Range<T> &r2)
    {
        return Range<T>(r1) + r2;
    }

    template <class T>
    Range<T> operator + (const Range<T> &r1, const T &r2)
    {
        return r1 + Range<T>(r2);
    }


    template<class T>
    Range<typename std::enable_if<std::is_fundamental<T>::value, T>::type>
    operator - (const Range<T> &r1, const Range<T> &r2)
    {
        T a, b;
        if (__builtin_sub_overflow(r1.get_low(), r2.get_high(), &a) || __builtin_sub_overflow(r1.get_high(), r2.get_low(), &b)) {
            throw std::overflow_error("overflow");
        }
        return Range<T>(a, b);
    }

    template<class T>
    Range<typename std::enable_if<!std::is_fundamental<T>::value, T>::type>
    operator - (const Range<T> &r1, const Range<T> &r2)
    {
        T a, b;
        a = r1.get_low() - r2.get_high();
        b = r1.get_high() - r2.get_low();
        return Range<T>(a, b);
    }

    template <class T>
    Range<T> operator - (const T &r1, const Range<T> &r2)
    {
        return Range<T>(r1) - r2;
    }

    template <class T>
    Range<T> operator - (const Range<T> &r1, const T &r2)
    {
        return r1 - Range<T>(r2);
    }


    template<class T>
    Range<typename std::enable_if<std::is_fundamental<T>::value, T>::type>
    operator * (const Range<T> &r1, const Range<T> &r2)
    {
        T a, b, c, d;
        if (__builtin_mul_overflow(r1.get_low(), r2.get_low(), &a) || __builtin_mul_overflow(r1.get_low(), r2.get_high(), &b)
            || __builtin_mul_overflow(r1.get_high(), r2.get_low(), &c) || __builtin_mul_overflow(r1.get_high(), r2.get_high(), &d)) {
            throw std::overflow_error("overflow");
        }
        std::tie(a, b) = std::minmax({a, b, c, d});
        return Range<T>(a, b);
    }

    template<class T>
    Range<typename std::enable_if<!std::is_fundamental<T>::value, T>::type>
    operator * (const Range<T> &r1, const Range<T> &r2)
    {
        T a, b, c, d;
        a = r1.get_low() * r2.get_low();
        b = r1.get_high() * r2.get_high();
        c = r1.get_low() * r2.get_high();
        d = r1.get_high() * r2.get_low();
        std::tie(a, b) = std::minmax({a, b, c, d});
        return Range<T>(a, b);
    }

    template <class T>
    Range<T> operator * (const T &r1, const Range<T> &r2)
    {
        return Range<T>(r1) * r2;
    }

    template <class T>
    Range<T> operator * (const Range<T> &r1, const T &r2)
    {
        return r1 * Range<T>(r2);
    }


    template<class T>
    Range<typename std::enable_if<std::is_fundamental<T>::value, T>::type>
    operator - (const Range<T> &r)
    {
        T a, b;
        if (__builtin_sub_overflow(T(0), r.get_high(), &a) || __builtin_sub_overflow(T(0), r.get_low(), &b)) {
            throw std::overflow_error("overflow");
        }
        return Range<T>(a, b);
    }

    template<class T>
    Range<typename std::enable_if<!std::is_fundamental<T>::value, T>::type>
    operator - (const Range<T> &r)
    {
        return Range<T>(-r.get_high(), -r.get_low());
    }
}

using namespace RangeImpl;