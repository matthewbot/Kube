#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <cstddef>
#include <type_traits>
#include <tuple>

// General purpose template wizardry collected here.

// A compile time list of types
template <typename... Ts>
struct TypeSequence { };

// A compile time list of numbers
template <size_t... Ns>
struct NumericSequence { };

namespace detail {
    template <typename S>
    struct ExtendSequence { };

    template <size_t... Ns>
    struct ExtendSequence<NumericSequence<Ns...>> {
        using type = NumericSequence<Ns..., sizeof...(Ns)>;
    };

    template <size_t N, typename S=NumericSequence<>>
    struct GenerateSequence {
        using type = typename GenerateSequence<
            N-1, typename ExtendSequence<S>::type
            >::type;
    };

    template <typename S>
    struct GenerateSequence<0, S> {
        using type = S;
    };
}

// AscendingSequence, a template typedef for a NumericSequence of N ascending integers
template <size_t N>
using AscendingSequence = typename detail::GenerateSequence<N>::type;

namespace detail {
    template <typename Callable, typename... Args, size_t... ArgSeq>
    auto invokeWithTupleArgSeq(Callable &&callable,
                               const std::tuple<Args...> &args,
                               NumericSequence<ArgSeq...>)
        -> decltype(std::forward<Callable>(callable)(std::get<ArgSeq>(args)...))
    {
        return std::forward<Callable>(callable)(
            std::get<ArgSeq>(args)...);
    }
}

template <typename Callable, typename... Args>
auto invokeWithTuple(Callable &&callable,
                     const std::tuple<Args...> &args)
    -> decltype(detail::invokeWithTupleArgSeq(
                    std::forward<Callable>(callable),
                    args,
                    AscendingSequence<sizeof...(Args)>()))
{
    return detail::invokeWithTupleArgSeq(
        std::forward<Callable>(callable),
        args,
        AscendingSequence<sizeof...(Args)>());
}

namespace detail {
    template <typename T, typename MemFn>
    struct BindMemberFunction {
        T &t;
        MemFn memfn;

        template <typename... Args>
        auto operator()(Args&&... args)
        -> decltype((t.*memfn)(std::forward<Args>(args)...)) {
            return (t.*memfn)(std::forward<Args>(args)...);
        }
    };
}

// bindMemberFunction, takes an object and its member function,
// and returns a callable.
template <typename T, typename MemFn>
detail::BindMemberFunction<T, MemFn> bindMemberFunction(T &t, MemFn fn) {
    return {t, fn};
}

namespace detail {
    template <typename T>
    struct BindConstructors {
        template <typename... Args>
        T *operator()(Args&&... args) {
            return new T{std::forward<Args>(args)...};
        }
    };
}

template <typename T>
detail::BindConstructors<T> bindConstructors() {
    return {};
}

#endif
