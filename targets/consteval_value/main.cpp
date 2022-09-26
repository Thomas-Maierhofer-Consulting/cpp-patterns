#include <array>

template <auto V>
constexpr const auto& constexpr_ref = V;

template <auto V>
consteval const auto& consteval_value(){
    return constexpr_ref<V>;
}

consteval auto & pick_second(const std::array<int,3>& arr){
    return arr[2];
}

int main() {
    static constexpr auto RET = consteval_value<pick_second(consteval_value<std::array{1,2,3}>()) + consteval_value<3>()>();
    return RET;
}
