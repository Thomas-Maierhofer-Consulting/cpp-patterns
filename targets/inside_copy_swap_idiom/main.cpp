#include <iostream>
#include <vector>
#include <string>
#include <utility>

namespace N {
class test {
public:
    explicit test(std::string value): _value(std::move(value)) {std::cout << "VALUE CONSTRUCTED" << std::endl;}
    test() {std::cout << "DEFAULT CONSTRUCTED" << std::endl;};
    test(const test& other): _value(other._value){std::cout << "COPY CONSTRUCTED" << std::endl;}
    test(test&& other) noexcept: _value(std::move(other._value)){std::cout << "MOVE CONSTRUCTED" << std::endl;}
    ~test() = default;

    test& operator=(const test& other) 
    {
        std::cout << "COPY ASSIGNED" << std::endl;

        // temporary copy constructed object
        test(other).swap(*this);
        return *this;
    };

    test& operator=(test&& other) noexcept
    {
        std::cout << "MOVE ASSIGNED" << std::endl;

        // temporary move constructed object
        test(std::move(other)).swap(*this);
        return *this;
    };

    // implement the swap as public member to implement the copy swap idiom with temporary objects
    // this is also picked up by the std::swap function
    void swap(test& other) noexcept
    {
        std::cout << "SWAP : " << _value << " with " << other._value << std::endl;
        using std::swap;
        swap(_value, other._value);
    }

private:
    std::string _value {};

    friend std::ostream & operator <<(std::ostream& stream, const test& obj){ stream << obj._value; return stream; }    
};

// define free function to be picked up by argument depended lookup
void swap(test& lhs, test& rhs) noexcept { lhs.swap(rhs); }

} // namespace N

void copy_assignment()
{
    std::cout << "*** Copy Assignment ***" << std::endl;
    N::test source {"source"};
    N::test destination {"destination"};
    destination = source;
}

void move_assignment()
{
    std::cout << "*** Move Assignment ***" << std::endl;
    N::test destination {"destination"};
    destination = N::test{"temp"};
}

void swap_member_function(){
    std::cout << "*** Swap Member Function ***" << std::endl;
    N::test lhs{"lhs"};
    N::test rhs{"rhs"};
    lhs.swap(rhs);
    std::cout << "   Swap Result: " << lhs << " - " << rhs << std::endl << std::endl;
}

void swap_free_function(){
    std::cout << "*** Swap Free Function ***" << std::endl;
    N::test lhs{"lhs"};
    N::test rhs{"rhs"};
    N::swap(lhs, rhs);
    std::cout << "   Swap Result: " << lhs << " - " << rhs << std::endl << std::endl;
}

void swap_free_function_adl_lookup(){
    std::cout << "*** Swap Free Function ADL (Koenig) Lookup ***" << std::endl;
    N::test lhs{"lhs"};
    N::test rhs{"rhs"};
    swap(lhs, rhs);
    std::cout << "   Swap Result: " << lhs << " - " << rhs << std::endl << std::endl;
}

void std_swap_function(){
    std::cout << "*** Swap using std::swap() ***" << std::endl;
    N::test lhs{"lhs"};
    N::test rhs{"rhs"};
    std::swap(lhs, rhs);
    std::cout << "   Swap Result: " << lhs << " - " << rhs << std::endl << std::endl;
}

int main()
{
    copy_assignment();
    move_assignment();

    swap_member_function();
    swap_free_function();
    swap_free_function_adl_lookup();
    std_swap_function();
    
    return 0;
}
