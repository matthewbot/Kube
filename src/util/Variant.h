#ifndef VARIANT_H
#define VARIANT_H

#include "util/templates.h"

namespace detail {
    template <typename... Ts>
    struct LargestSize {
        static constexpr size_t value = 0;
    };

    template <typename T, typename... Ts>
    struct LargestSize<T, Ts...> {
        static constexpr size_t ts = LargestSize<Ts...>::value;
        static constexpr size_t t = sizeof(T);
        static constexpr size_t value = ts > t ? ts : t;
    };

    template <typename To>
    struct CurriedDecaySame {
        template <typename From>
        struct templ : public std::is_same<typename std::decay<To>::type,
                                           typename std::decay<From>::type> { };
    };
}

template <typename... Ts>
class Variant {
public:
    Variant() : Variant(typename IndexToType<0, Ts...>::type{}) { }

    template <typename T>
    Variant(T &&t) {
        callConstructor(std::forward<T>(t));
    }

    Variant(const Variant<Ts...> &) = delete;
    Variant<Ts...> &operator=(const Variant<Ts...> &) = delete;

    ~Variant() { callDestructor(); }

    size_t getTag() const { return tag; }
    
    template <typename T>
    T *getPtr() {
        if (tag == TypeToIndex<T, Ts...>::value) {
            return reinterpret_cast<T *>(ptr);
        } else {
            return nullptr;
        }
    }

    template <typename T>
    const T *getPtr() const {
        if (tag == TypeToIndex<T, Ts...>::value) {
            return reinterpret_cast<const T *>(ptr);
        } else {
            return nullptr;
        }
    }
    
    template <typename Result=void, typename... Callables>
    Result match(Callables&&... calls) {
        static_assert(sizeof...(Callables) == sizeof...(Ts),
                      "Incorrect number of callables");
        
        return matchHelper<0, Result>(std::forward<Callables>(calls)...);
    }
    
    template <typename T>
    Variant &operator=(T &&t) {
        callDestructor();
        callConstructor(std::forward<T>(t));
        return *this;
    }
    
private:
    template <size_t curtag, typename Result, typename Callable, typename... Callables>
    Result matchHelper(Callable&& call, Callables&&... calls) {
        using T = typename IndexToType<curtag, Ts...>::type;
        if (tag == curtag) {
            return std::forward<Callable>(call)(*reinterpret_cast<T *>(ptr));
        } else {
            return matchHelper<curtag+1, Result, Callables...>(
                std::forward<Callables>(calls)...);
        }
    }

    template <size_t curtag, typename Result, typename Callable>
    Result matchHelper(Callable &&call) {
        static_assert(curtag+1 == sizeof...(Ts), "Should not happen");
        
        using T = typename IndexToType<curtag, Ts...>::type;
        return std::forward<Callable>(call)(*reinterpret_cast<T *>(ptr));
    }

    void callDestructor() {
        callDestructorHelper(size_t_<0>());
    }
    
    template <size_t curtag>
    void callDestructorHelper(size_t_<curtag>) {
        using T = typename IndexToType<curtag, Ts...>::type;
        
        if (tag == curtag) {
            reinterpret_cast<T *>(ptr)->~T();
        } else {
            return callDestructorHelper(size_t_<curtag+1>());
        }
    }

    void callDestructorHelper(size_t_<sizeof...(Ts)-1>) {
        constexpr size_t curtag = sizeof...(Ts)-1;
        using T = typename IndexToType<curtag, Ts...>::type;
        reinterpret_cast<T *>(ptr)->~T();        
    }

    template <typename T>
    void callConstructor(T &&t) {
        using TagT = typename FindType<
            detail::CurriedDecaySame<T>::template templ,
            Ts...
        >::type;
        tag = TypeToIndex<TagT, Ts...>::value;
        ptr = reinterpret_cast<void *>(new (&buf) TagT(std::forward<T>(t)));
    }

    size_t tag;
    void *ptr;
    typename std::aligned_storage<detail::LargestSize<Ts...>::value>::type buf;
};


#endif
