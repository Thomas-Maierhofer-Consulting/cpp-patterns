#include <typeindex>
#include <typeinfo>
#include <memory>
#include <functional>
#include <tuple>
#include <type_traits>
#include <cstdio>

template <typename... Args>
inline void log(const char* msg, [[maybe_unused]] Args&&... args) noexcept {
    static constexpr bool enable = true;
    if constexpr (enable) {
        std::printf(msg, std::forward<Args>(args)...);
        std::printf("\n");
    }
}

namespace detail {

/*-------------------------------------------------------------------------*/
// Primary template for FunctionTraits.
template <typename FunctionType>
struct function_traits;

// Partial specialization for a plain function type ReturnType(ArgumentTypes...).
template <typename ReturnType, typename... ArgumentTypes>
struct function_traits<ReturnType(ArgumentTypes...)> {
    using result_type = ReturnType;             // The return type of the function.
    using arg_types = std::tuple<ArgumentTypes...>; // A tuple containing all argument types of the function.
};

// Partial specialization for a function pointer type ReturnType(*)(ArgumentTypes...).
template <typename ReturnType, typename... ArgumentTypes>
struct function_traits<ReturnType(*)(ArgumentTypes...)> : function_traits<ReturnType(ArgumentTypes...)> {};

// Partial specialization for an std::function type std::function<ReturnType(ArgumentTypes...)>.
template <typename ReturnType, typename... ArgumentTypes>
struct function_traits<std::function<ReturnType(ArgumentTypes...)>> : function_traits<ReturnType(ArgumentTypes...)> {};

// Partial specialization for a member function pointer type ReturnType(ClassType::*)(ArgumentTypes...).
template <typename ClassType, typename ReturnType, typename... ArgumentTypes>
struct function_traits<ReturnType(ClassType::*)(ArgumentTypes...)> : function_traits<ReturnType(ArgumentTypes...)> {};

// Partial specialization for a const member function pointer type ReturnType(ClassType::*)(ArgumentTypes...) const.
template <typename ClassType, typename ReturnType, typename... ArgumentTypes>
struct function_traits<ReturnType(ClassType::*)(ArgumentTypes...) const> : function_traits<ReturnType(ArgumentTypes...)> {};

// Partial specialization for a Callable (e.g., lambda, functor) by deducing its
// operator() member function type using decltype.
template <typename CallableType>
struct function_traits : function_traits<decltype(&CallableType::operator())> {};


template <bool IsConst>
    class TransientProxyImpl {

    template <typename F>
    using first_arg_t = std::remove_reference_t<std::tuple_element_t<0, typename function_traits<F>::arg_types>>;


    public:
        TransientProxyImpl() noexcept = default;
        TransientProxyImpl(const TransientProxyImpl&) noexcept = default;
        TransientProxyImpl& operator=(const TransientProxyImpl&) noexcept = default;
        TransientProxyImpl(TransientProxyImpl&&) noexcept = default;
        TransientProxyImpl& operator=(TransientProxyImpl&&) noexcept = default;

        template <typename T>
        TransientProxyImpl(T& object) noexcept
            : typeId_{typeid(T)}, objectPointer_{std::addressof(object)} {
                static_assert(IsConst || !std::is_const_v<T>, "Use ConstTransientProxy with a const T");                
            }

        std::type_index type() const noexcept { return typeId_; }

        template <typename T>
        auto get() const -> std::enable_if_t<IsConst, const T&> {
            if (std::type_index(typeid(T)) != typeId_) [[unlikely]] throw std::bad_cast();
            return unchecked_value<T>();
        }

        template <typename T>
        auto get() const -> std::enable_if_t<!IsConst, T&> {
            if (std::type_index(typeid(T)) != typeId_) [[unlikely]] throw std::bad_cast();
            return unchecked_value<T>();
        }

        template <typename F, typename... Args>
        bool try_invokeX(F&& func, Args&&... args) const {
            using arg_types = function_traits<F>::arg_types;
            using first_arg_type = std::remove_reference<std::tuple_element_t<0, arg_types>>::type;

            if (std::type_index(typeid(first_arg_type)) != typeId_) return false;

            std::invoke(std::forward<F>(func), unchecked_value<first_arg_type>(), std::forward<Args>(args)...);
            return true;
        }

        template <typename F, typename... Args>
        requires std::invocable<F, first_arg_t<F>, Args...>
        auto try_invoke(F&& func, Args&&... args) const
        noexcept(std::is_nothrow_invocable_v<F, decltype(std::declval<first_arg_t<F>>()), Args...>)
        -> bool {
        if (std::type_index(typeid(first_arg_t<F>)) != typeId_) return false;

        std::invoke(std::forward<F>(func), unchecked_value<first_arg_t<F>>(), std::forward<Args>(args)...);
        return true;
        }

    private:

        template <typename T>
        auto unchecked_value() const -> std::enable_if_t<IsConst, const T&> {
            return *static_cast<const T*>(objectPointer_);
        }

        template <typename T>
        auto unchecked_value() const -> std::enable_if_t<!IsConst, T&> {
            return *static_cast<T*>(objectPointer_);
        }

        std::type_index typeId_ = typeid(void);
        std::conditional_t<IsConst, const void*, void*> objectPointer_ {nullptr};
    };
}



using ConstTransientProxy = detail::TransientProxyImpl<true>;
using TransientProxy = detail::TransientProxyImpl<false>;

static constexpr int bla = 12;
static ConstTransientProxy intRef(bla);

int main() {
    log("Int value: %d", intRef.get<int>());

    intRef.try_invoke([](const int & value, const char * msg) -> void { 
        log("Apply Int value: %d - %s",value, msg);
    }, "Hello from Outside");

    return 0;
}
