#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <cstddef>
#include <type_traits>
#include <tuple>

// General purpose template wizardry collected here.


template <size_t N>
using size_t_ = std::integral_constant<size_t, N>;

// A compile time list of types
// Useful to delineate multiple type packs in one template.
template <typename... Ts>
struct TypeSequence { };

// Determines the index of a type in a type pack
template <typename A, typename... Ts>
struct TypeToIndex {
};

template <typename A, typename T, typename... Ts>
struct TypeToIndex<A, T, Ts...> {
    static constexpr size_t value = 1+TypeToIndex<A, Ts...>::value;
};

template <typename A, typename... Ts>
struct TypeToIndex<A, A, Ts...> {
    static constexpr size_t value = 0;
};

// Gets a type by index from a type pack
template <size_t N, typename... Ts>
struct IndexToType {
};

template <size_t N, typename T, typename... Ts>
struct IndexToType<N, T, Ts...> {
    using type = typename IndexToType<N-1, Ts...>::type;
};

template <typename T, typename... Ts>
struct IndexToType<0, T, Ts...> {
    using type = T;
};

namespace detail {
    template <typename, template <typename> class Pred, typename... Ts>
    struct FindTypeHelper {
    };
    
    template <template <typename> class Pred, typename T, typename... Ts>
    struct FindTypeHelper<typename std::enable_if<Pred<T>::value>::type,
                          Pred, T, Ts...> {
        using type = T;
    };

    template <template <typename> class Pred, typename T, typename... Ts>
    struct FindTypeHelper<typename std::enable_if<!Pred<T>::value>::type,
                          Pred, T, Ts...> {
        using type = typename FindTypeHelper<void, Pred, Ts...>::type;
    };
}

// Finds a type in a type pack satisfying a predicate
template <template <typename> class Pred, typename... Ts>
using FindType = typename detail::FindTypeHelper<void, Pred, Ts...>;

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

// TODO don't really need in MetaTableBuilder anymore
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

namespace detail {
    template <typename T, typename = void>
    struct CopyOrMove {
        static T exec(T &ref) { return ref; }        
    };

    template <typename T>
    struct CopyOrMove<T, typename std::enable_if<!std::is_copy_constructible<T>::value>::type> {
        static T exec(T &ref) { return std::move(ref); }
    };
}

template <typename T>
T copyOrMove(T &ref) {
    return detail::CopyOrMove<T>::exec(ref);
}

#endif
