#include <utility>
#include <cstdio>

template<typename... Args>
inline void log(const char* msg, [[maybe_unused]] Args&&... args) noexcept
{
    static constexpr bool enable = true;

    if constexpr (enable) {
        std::printf(msg, std::forward<Args>(args)...);
        std::printf("\n");
    }
}

// ---------------------------------------------------
#include <tuple>
#include <variant>

template<typename KEY_T, typename... ARGS_T>
constexpr std::size_t find_type()
{
    constexpr std::size_t param_pack_size = sizeof...(ARGS_T);
    constexpr bool typeMatches[] = {std::is_same_v<KEY_T, ARGS_T>...};

    for (std::size_t i = 0; i < param_pack_size; ++i) {
        if (typeMatches[i]) {
            return i;
        }
    }

    return param_pack_size; // Type not found
}

template<typename KEY_T, typename PACK_T>
struct type_pos;

template<typename KEY_T, template<typename...> class SPECIALIZED_T, typename... Ts>
struct type_pos<KEY_T, SPECIALIZED_T<Ts...>> {
    constexpr static auto value = find_type<KEY_T, Ts...>();
};

template<typename KEY_T, typename SPECIALIZED_T>
inline constexpr auto type_pos_v = type_pos<KEY_T, SPECIALIZED_T>::value;

// Forward declarations are enough
struct A;
struct B;
struct C;

using SpecializedTuple = std::tuple<A, B, C>;
using SpecializedVariant = std::variant<A, B, C>;

int main()
{
    static constexpr auto ppIndex = find_type<B, A, B, C>();
    log("Position in Parameter Pack: %zu", ppIndex);

    static constexpr auto tupleIndex = type_pos_v<B, SpecializedTuple>;
    log("Position in Tuple: %zu", tupleIndex);

    static constexpr auto variantIndex = type_pos_v<B, SpecializedVariant>;
    log("Position in Variant: %zu", variantIndex);

    return 0;
}