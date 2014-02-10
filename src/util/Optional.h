#ifndef OPTIONAL_H
#define OPTIONAL_H

#include <type_traits>
#include <utility>

struct NoneT { };

static const constexpr NoneT None;

template <typename T>
class Optional {
public:
    Optional() : ptr(nullptr) { }

    template <typename U>
    Optional(U &&val) : ptr(nullptr) {
        *this = std::forward<U>(val);
    }

    ~Optional() {
        callDestructor();
    }

    Optional<T> &operator=(NoneT) {
        callDestructor();
        return *this;
    }

    template <typename U>
    Optional<T> &operator=(const Optional<U> &opt) {
        callDestructor();
        if (opt) {
            callConstructor(*opt);
        }
        return *this;
    }

    template <typename U>
    Optional<T> &operator=(Optional<U> &&opt) {
        callDestructor();
        if (opt) {
            callConstructor(std::move(*opt));
            opt = None;
        }
        return *this;
    }    

    Optional<T> &operator=(const T &val) {
        callDestructor();
        callConstructor(val);
        return *this;
    }
    
    template <typename U,
              typename = typename std::enable_if<
                  !std::is_same<typename std::decay<U>::type,
                                Optional<T>>::value
                  >::type>
    Optional<T> &operator=(U &&val) {
        callDestructor();
        callConstructor(std::forward<U>(val));
        return *this;
    }
    
    explicit operator bool() const { return ptr != nullptr; }

    T &operator*() & { return *reinterpret_cast<T *>(ptr); }
    T &&operator*() && { return std::move(*reinterpret_cast<T *>(ptr)); }
    const T &operator*() const & { return *reinterpret_cast<const T *>(ptr); }
    T *operator->() { return reinterpret_cast<T *>(ptr); }
    const T *operator->() const { return reinterpret_cast<T *>(ptr); }

    template <typename E, typename... Args>
    T &or_throw(Args &&... args) & {
        if (*this) {
            return **this;
        }
        
        throw E(std::forward<Args>(args)...);
    }

    template <typename E, typename... Args>
    T &&or_throw(Args &&... args) && {
        return std::move(or_throw<T, Args...>(std::forward<Args>(args)...));
    }

    template <typename E, typename... Args>
    const T &or_throw(Args &&... args) const & {
        if (*this) {
            return **this;
        }
        
        throw E(std::forward<Args>(args)...);
    }
    
private:
    template <typename U>
    void callConstructor(U &&val) {
        ptr = new (&buf) T(std::forward<U>(val));
    }

    void callDestructor() {
        if (ptr != nullptr) {
            reinterpret_cast<T *>(ptr)->~T();
            ptr = nullptr;
        }
    }
    
    typename std::aligned_storage<sizeof(T)>::type buf;
    void *ptr;
};

template <typename T, typename U>
bool operator==(const Optional<T> &a, const Optional<U> &b) {
    if (static_cast<bool>(a) != static_cast<bool>(b)) {
        return false;
    }

    if (a && *a != *b) {
        return false;
    }

    return true;
}

template <typename T>
bool operator==(NoneT, const Optional<T> &b) {
    return !b;
}
    
#endif
