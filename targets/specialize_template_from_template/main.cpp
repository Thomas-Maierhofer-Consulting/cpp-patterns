#include <typeinfo>
#include <utility>
#include <memory>
#include <string>
#include <cstdlib>

#ifdef _MSC_VER
#include <Windows.h>
#include <Dbghelp.h>
#else
#include <cxxabi.h>
#endif

template <typename... Args>
inline void log(const char* msg, [[maybe_unused]] Args&&... args) noexcept {
    static constexpr bool enable = true;
    if constexpr (enable) {
        std::printf(msg, std::forward<Args>(args)...);
        std::printf("\n");
    }
}

template <class T>
void log_typename(const char * format = "%s")
{
    typedef typename std::remove_reference<T>::type TR;

    std::unique_ptr<char, void(*)(void*)> own(
#ifdef _MSC_VER
        []() -> char*
        {
            const size_t size = 256;
            char* buffer = static_cast<char*>(std::malloc(size));
            if (buffer != nullptr)
                UnDecorateSymbolName(typeid(TR).name(), buffer, static_cast<DWORD>(size), UNDNAME_COMPLETE);
            return buffer;
        }()
#else
        abi::__cxa_demangle(typeid(TR).name(), nullptr, nullptr, nullptr)
#endif
        , std::free
    );

    std::string r = own != nullptr ? own.get() : typeid(TR).name();

    if (std::is_const<TR>::value)
        r += " const";
    if (std::is_volatile<TR>::value)
        r += " volatile";
    if (std::is_lvalue_reference<T>::value)
        r += "&";
    else if (std::is_rvalue_reference<T>::value)
        r += "&&";

    log(format, r.data());
}


// --------------------------------------------------------------------------------- 

template <template<typename...> class DEST_T, typename SOURCE_T>
struct specialize_from_type;

template <template<typename...> class DEST_T, template<typename...> class SOURCE_T, typename... Ts>
struct specialize_from_type<DEST_T, SOURCE_T<Ts...>> {
    using type = DEST_T<Ts...>;
};

template <template<typename...> class DEST_T, typename SOURCE_T>
using specialize_from_type_t = typename specialize_from_type<DEST_T, SOURCE_T>::type;

template<typename... Ts>
class SourceType{};

template<typename... Ts>
class DestinationType{};

using S = SourceType<bool, int, double, float>;
using D = specialize_from_type_t<DestinationType, S>;

int main() {
    log_typename<S>("Source: %s");
    log_typename<D>("Destination: %s");
    return 0;
}