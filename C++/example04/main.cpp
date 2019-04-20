#include <sstream>
#include <iomanip>

class Account
{
public:
    constexpr static size_t DATA_SIZE = 4;

    Account()
    {
    }

    Account(const Account &a)
    {
        for (size_t i = 0; i < DATA_SIZE; ++i) {
            data[i] = a.data[i];
        }
    }

    Account(const std::string &s)
    {
        if (s == "0") {
            return;
        }

        std::string cs = s.substr(2);
        cs = std::string(8 - cs.size() % 8, '0') + cs;

        for (auto &cur : data) {
            if (cs.empty()) {
                break;
            }

            std::string curs = cs.substr(cs.size() - 8);
            std::stringstream(curs) >> std::hex >> cur;
            cs = cs.substr(0, cs.size() - 8);
        }
    }

    Account(const uint32_t *data)
    {
        for (size_t i = 0; i < DATA_SIZE; ++i) {
            Account::data[i] = data[i];
        }
    }

    explicit operator bool() const
    {
        for (const auto &cur : data) {
            if (cur) {
                return true;
            }
        }
        return false;
    }

    bool operator ! () const
    {
        return !(bool(*this));
    }

    std::string to_string() const
    {
        if (!*this) {
            return "0";
        }

        std::stringstream ans;
        ans << "0x" << std::hex << std::setfill('0');
        for (size_t i = 0; i < DATA_SIZE; ++i) {
            ans << std::setw(8) << data[DATA_SIZE - i - 1];
        }

        return ans.str();
    }

    const uint32_t *cdata() const
    {
        return data;
    }

    friend bool operator < (const Account &a, const Account &b);
    friend bool operator > (const Account &a, const Account &b);
    friend bool operator == (const Account &a, const Account &b);
    friend bool operator != (const Account &a, const Account &b);
    friend bool operator <= (const Account &a, const Account &b);
    friend bool operator >= (const Account &a, const Account &b);

private:
    uint32_t data[DATA_SIZE] = {};
};

bool operator < (const Account &a, const Account &b)
{
    for (size_t i = 0; i < Account::DATA_SIZE; ++i) {
        size_t j = Account::DATA_SIZE - i - 1;
        if (a.data[j] < b.data[j]) {
            return true;
        } else if (a.data[j] > b.data[j]) {
            return false;
        }
    }

    return false;
}

bool operator > (const Account &a, const Account &b)
{
    for (size_t i = 0; i < Account::DATA_SIZE; ++i) {
        size_t j = Account::DATA_SIZE - i - 1;
        if (a.data[j] > b.data[j]) {
            return true;
        } else if (a.data[j] < b.data[j]) {
            return false;
        }
    }

    return false;
}

bool operator == (const Account &a, const Account &b)
{
    for (size_t i = 0; i < Account::DATA_SIZE; ++i) {
        if (a.data[i] != b.data[i]) {
            return false;
        }
    }
    return true;
}

bool operator != (const Account &a, const Account &b)
{
    return !(a == b);
}

bool operator <= (const Account &a, const Account &b)
{
    return a < b || a == b;
}

bool operator >= (const Account &a, const Account &b)
{
    return a > b || a == b;
}

namespace std
{
    template<>
    struct hash <Account>
    {
        size_t operator () (const Account &a) const
        {
            const uint32_t *p = a.cdata();

            size_t ret = 0;
            for (size_t i = 0; i < Account::DATA_SIZE; ++i) {
                ret ^= p[i];
            }
            return ret;
        }
    };
}