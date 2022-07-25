#include <iostream>

template <typename... T>
class object_layout_test;  // Not defined to produce an error if instanciated

// Common part - without (additional) data
template <>
class object_layout_test<void>
 {
    public:
        object_layout_test(){} 
};


template <typename T>
class object_layout_test<T> : public object_layout_test<void>
{
    public:
        // Some additional Constructor
        object_layout_test(const T& value) : additional_(value) {}

        // Some additional methods
        void set_data(const T& value) {additional_ = value;}
        T get_data() {return additional_;}

    private:
        T additional_;  // Some additional Data
};

// Deduction guidelines 
object_layout_test() -> object_layout_test<void>; // parameterless constructor

template<typename T>
object_layout_test(T) -> object_layout_test<T>; // Generic one argument constructor

object_layout_test(float) -> object_layout_test<double>; // float is deduced as double


int main()
{
    // template arguments / class versions are deducible
    object_layout_test objVoid;
    object_layout_test objDouble1{40.0};
    object_layout_test objDouble2{20.0f};  // Will be double, not float due to deduction guideline
    object_layout_test objInt(10);
    
    objDouble2.set_data(42.42);
    objDouble1 = objDouble2;  // Assignable, it is the same type


    std::cout << "objDouble2 data is " << objDouble2.get_data() << std::endl; 
    std::cout << "sizeof objVoid is " << sizeof(objVoid) << std::endl; 
    std::cout << "sizeof objDouble1 is " << sizeof(objDouble1) << std::endl; 
    std::cout << "sizeof objDouble2 is " << sizeof(objDouble2) << std::endl; 
    std::cout << "sizeof objInt is " << sizeof(objInt) << std::endl; 

    return sizeof(objVoid);
}
