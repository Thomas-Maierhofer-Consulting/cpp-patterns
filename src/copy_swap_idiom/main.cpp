#include <vector>
#include <string>
#include <utility>

namespace N {
class test {
public:
    test() = default;
    test(const test& other) = default;
    test(test&& other) = default;
    ~test() = default;

    test& operator=(const test& other) 
    {
        // temporary copy constructed object
        test(other).swap(*this);
        return *this;
    };

    test& operator=(test&& other) noexcept
    {
        // temporary move constructed object
        test(std::move(other)).swap(*this);
        return *this;
    };

    // provide a non-member swap() to be used with std::swap
    // friend void swap(test& lhs, test& rhs) noexcept { lhs.swap(rhs); }


    // implement the swap as private member to implement the copy swap idiom with temporary objects
    // but force the use of the non-member swap from the outside
    void swap(test& rhs) noexcept
    {
        using std::swap;
        swap(_value, rhs._value); // std::swap(std::vector) is noexcept since C++17
    }

private:
    std::vector<std::string> _value {};
};

// define free fun
void swap(test& lhs, test& rhs) noexcept { lhs.swap(rhs); }

} // namespace N


int main()
{
    N::test source {};
    N::test destination {};
    destination = source;       // copy assigment
    destination = N::test{};    // move assignment

    std::string lhsString{};
    std::string rhsString{};

    swap(lhsString, rhsString); 

    N::test lhs{};
    N::test rhs{};

    swap(lhs, rhs);  // Argument-Dependent Lookup (ADL / Koenig lookup) resolves the correct swap() function; 
    

    return 0;
}
