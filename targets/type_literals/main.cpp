#include <span>
#include <string>
#include <string_view>

template<char... CharsT>
struct literal_type {
    static constexpr std::size_t SIZE = sizeof...(CharsT);
    static constexpr const char DATA[SIZE] {CharsT...};

    inline static consteval std::array<char, SIZE> to_array() noexcept { return std::array<char, SIZE> {{CharsT...}}; }

    inline static consteval std::string_view to_string_view() noexcept { return std::string_view(DATA, SIZE - 1); }
};

template<std::size_t N>
struct constexpr_char_array {
    static constexpr auto SIZE = N;
    const char data[N];

    template<std::size_t... Is>
    constexpr constexpr_char_array(const char (&src)[N], std::integer_sequence<std::size_t, Is...>) noexcept
        : data {src[Is]...}
    {
    }
    constexpr constexpr_char_array(const char (&src)[N]) noexcept
        : constexpr_char_array(src, std::make_integer_sequence<std::size_t, N>())
    {
    }

    constexpr char at(std::size_t i) const noexcept { return data[i]; }
};

template<std::size_t N, constexpr_char_array<N> ARR, std::size_t... Is>
static consteval auto make_literal_type(std::integer_sequence<std::size_t, Is...>)
{
    return literal_type<ARR.at(Is)...>();
}

template<std::size_t N, constexpr_char_array<N> ARR>
static consteval auto make_literal_type()
{
    return make_literal_type<N, ARR>(std::make_integer_sequence<std::size_t, N>());
}

template<std::size_t N>
struct literal_type_helper {
    static constexpr auto SIZE = N;
    constexpr_char_array<N> data;

    constexpr literal_type_helper(char const (&arr)[N])
        : data {arr}
    {
    }
};

template<literal_type_helper helper>
constexpr auto operator"" _t()
{
    return make_literal_type<decltype(helper)::SIZE, helper.data>();
}

// deprecated declarations must be enabled as warning
template<auto val>
[[deprecated]] constexpr void output() {};

static constexpr auto xxx = "Here i am! Finally!"_t;

int main()
{
    // Nice output in clang, compiles with MSVC  
    output<decltype(xxx)::DATA>();

    // Nice output in gcc
    // output<decltype("Here i am! Finally!"_t)::to_array()>();
    return 0;
}
