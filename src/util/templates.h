#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <cstddef>
#include <type_traits>
#include <tuple>

// General purpose template wizardry collected here.

// Predeclare stuff needed for type declarations.
namespace detail {
    template <typename T, typename MemFn>
    struct BindMemberFunction;
}

// Handy type alias for type-level integers.
template <size_t N>
using size_t_ = std::integral_constant<size_t, N>;

// A compile time list of types
// Useful to delineate multiple type packs in one template.
template <typename... Ts>
struct TypeSequence { };

// Maps a type function over a TypeSequence
template <template <typename> class TFunc, typename... Ts>
struct MapTypeSequence;

// Determines the index of a type in a type pack
template <typename A, typename... Ts>
struct TypeToIndex;

// Gets a type by index from a type pack
template <size_t N, typename... Ts>
struct IndexToType;

// Finds a type in a type pack satisfying a predicate
template <template <typename> class Pred, typename... Ts>
struct FindType;

// A compile time list of numbers
template <size_t... Ns>
struct NumericSequence { };

// AscendingSequence, a template typedef for a NumericSequence of N ascending integers
template <size_t N>
struct AscendingSequence;

// Invokes a callable type with values from a tuple.
template <typename Callable, typename... Args>
auto invokeWithTuple(Callable &&callable,
                     const std::tuple<Args...> &args)
    -> typename std::result_of<Callable(Args...)>::type;

// bindMemberFunction, takes an object and its member function,
// and returns a callable.
template <typename T, typename MemFn>
detail::BindMemberFunction<T, MemFn> bindMemberFunction(T &t, MemFn fn);

// Creates a new T by copying from ref if possible, otherwise moves from ref.
template <typename T>
T copyOrMove(T &ref);

#include "templates.icc"

#endif
