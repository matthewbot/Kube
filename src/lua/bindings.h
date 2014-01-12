#ifndef BINDINGS_H
#define BINDINGS_H

#include "util/templates.h"
#include <lua.hpp>
#include <typeinfo>
#include <type_traits>
#include <memory>
#include <boost/variant.hpp>

template <typename T>
struct IsUserDataPtrType : public std::false_type { };

template <typename T>
struct IsUserDataPtrType<T *> : public std::true_type { };

template <typename T>
struct IsUserDataPtrType<std::shared_ptr<T>> : public std::true_type { };

template <typename T>
struct IsUserDataPtrType<std::unique_ptr<T>> : public std::true_type { };

template <typename TPtr>
struct ToPtrVisitor;

template <typename T>
struct UserData {
    static_assert(!std::is_pointer<T>::value && !std::is_reference<T>::value,
                  "Trying to instantiate UserData for pointer or reference type");
    
    // Userdata can have no ownership, shared ownership, or unique ownership
    boost::variant<T *, std::shared_ptr<T>, std::unique_ptr<T>> ptr;

    // Push a pointer type to the stack. 
    template <typename TPtr>
    static void pushPtr(lua_State *L, TPtr &&ptr) {
        if (!ptr) {
            lua_pushnil(L);
            return;
        }
        
        void *mem = lua_newuserdata(L, sizeof(UserData<T>));
        new (mem) UserData{std::forward<TPtr>(ptr)};
        luaL_getmetatable(L, typeid(T).name());
        lua_setmetatable(L, -2);
    }

    // Convert a stack entry back to a pointer type.
    template <typename TPtr>
    static TPtr &getPtr(lua_State *L, int narg) {
        static_assert(IsUserDataPtrType<TPtr>::value,
                      "Trying to convert userdata to an unsupported pointer type");

        auto &ud = *static_cast<UserData<T> *>(
            luaL_checkudata(L, narg, typeid(T).name()));
        return boost::get<TPtr>(ud.ptr);
    }

    struct ToPtrVisitor :
        public boost::static_visitor<T *>
    {
        T *operator()(T *&ptr) const {
            return ptr;
        }
        
        template <typename PtrT>
        T *operator()(const PtrT &ptr) const {
            return ptr.get();
        }
    };
    
    // Converts a stack entry to a raw pointer,
    // also checking for nils and giving nullptr
    static T *toPtr(lua_State *L, int narg) {
        if (lua_isnil(L, narg)) {
            return nullptr;
        }

        auto &ud = *static_cast<UserData<T> *>(
            luaL_checkudata(L, narg, typeid(T).name()));
        return boost::apply_visitor(ToPtrVisitor(), ud.ptr);
    }

    // Converts a stack entry to a reference,
    // checking for null
    static T &getRef(lua_State *L, int narg) {
        auto ptr = toPtr(L, narg);
        if (ptr) {
            return *ptr;
        } else {
            throw std::runtime_error("Trying to convert null userdata to reference");
        }
    }
    
    static void destroy(lua_State *L, int narg) {
        auto &ud = *static_cast<UserData<T> *>(
            luaL_checkudata(L, narg, typeid(T).name()));
        ud.~UserData<T>();
    }
};

template <typename T>
using IsBuiltinLuaType =
    std::integral_constant<bool,
                           std::is_same<T, std::string>::value ||
                           std::is_same<T, bool>::value ||
                           std::is_enum<T>::value ||
                           std::is_arithmetic<T>::value> ;

template <typename T, typename = void>
struct RemoveReferenceFromBuiltinLuaType {
    using type = T;
};

template <typename T>
struct RemoveReferenceFromBuiltinLuaType<T &, typename std::enable_if<IsBuiltinLuaType<T>::value>::type> {
    using type = T;
};

template <typename T, typename = void>
struct ValueBinding {
    static_assert(!IsBuiltinLuaType<T>::value, "Should not happen");
    
    // Pushing a value allocates a copy and gives lua ownership
    static void push(lua_State *L, T val) {
        UserData<T>::pushPtr(L, std::unique_ptr<T>(
                                 new T{std::move(val)}));
    }

    // Converting to a value actually gives a const reference to it
    static T to(lua_State *L, int narg) {
        return UserData<T>::getRef(L, narg);
    }
};

template <typename T>
struct ValueBinding<T *> {
    static_assert(!IsBuiltinLuaType<T>::value, "Bindings for pointers to a builtin lua type not implemented");
    static_assert(!IsUserDataPtrType<T>::value, "Bindings for pointers to pointers not implemented");
    
    // Pushing a pointer makes a non-owning userdata
    static void push(lua_State *L, T *val) {
        UserData<T>::pushPtr(L, val);
    }

    // Converting to a pointer takes the address from the userdata
    static T *to(lua_State *L, int narg) {
        return UserData<T>::toPtr(L, narg);
    }
};

template <typename T>
struct ValueBinding<T *&> {
    static_assert(!IsBuiltinLuaType<T>::value, "Bindings for pointers to a builtin lua type not implemented");
    static_assert(!IsUserDataPtrType<T>::value, "Bindings for pointers to pointers not implemented");

    // Pushing a reference to a pointer makes a non-owning userdata
    static void push(lua_State *L, T *val) {
        UserData<T>::pushPtr(L, val);
    }

    // Converting to a reference to a pointer gives a reference to the userdata's unowned pointer.
    static T *&to(lua_State *L, int narg) {
        return UserData<T>::template getPtr<T *>(L, narg);
    }
};

template <typename T>
struct ValueBinding<T *const &> : public ValueBinding<T *> { };

template <typename T>
struct ValueBinding<T &> {
    static_assert(!IsBuiltinLuaType<T>::value, "Bindings for references to a builtin lua type not implemented");
    static_assert(!IsUserDataPtrType<T>::value, "Should not happen");
    
    // Pushing a reference makes a non-owning userdata
    static void push(lua_State *L, T &val) {
        UserData<T>::pushPtr(L, &val);
    }

    // Converting to a reference gives the contents of the userdata
    static T &to(lua_State *L, int narg) {
        return UserData<T>::getRef(L, narg);
    }
};

// Reference wrapper is treated like an actual reference
template <typename T>
struct ValueBinding<std::reference_wrapper<T>> : public ValueBinding<T &> { };

template <typename T>
struct ValueBinding<std::shared_ptr<T>> {
    // Pushing a shared_ptr makes a shared owner userdata
    static void push(lua_State *L, std::shared_ptr<T> val) {
        UserData<T>::pushPtr(L, std::move(val));
    }

    // Converting to a shared_ptr requires a shared owner userdata
    static std::shared_ptr<T> to(lua_State *L, int narg) {
        return UserData<T>::template getPtr<std::shared_ptr<T>>(L, narg);
    }
};

template <typename T>
struct ValueBinding<std::shared_ptr<T> &> {
    // A reference to a std::unique_ptr cannot be pushed.
    // Only pushing by value is supported.
    
    // Converting to a shared_ptr reference requires a shared owner userdata
    static std::shared_ptr<T> &to(lua_State *L, int narg) {
        return UserData<T>::template getPtr<std::shared_ptr<T>>(L, narg);
    }
};

template <typename T>
struct ValueBinding<const std::shared_ptr<T>> : public ValueBinding<std::shared_ptr<T>> { };

template <typename T>
struct ValueBinding<std::unique_ptr<T>> {
    // pushing a unique_ptr moves it into an owning Userdata
    static void push(lua_State *L, std::unique_ptr<T> &&val) {
        UserData<T>::pushPtr(L, std::move(val));
    }

    // Converting to a unique_ptr transfers ownership out of an owning Userdata
    static std::unique_ptr<T> to(lua_State *L, int narg) {
        return std::move(UserData<T>::template getPtr<std::unique_ptr<T>>(L, narg));
    }
};

template <typename T>
struct ValueBinding<std::unique_ptr<T> &> {
    // A reference to a std::unique_ptr cannot be pushed.
    // Explicitly call get() and pass as a raw pointer instead if necessary.

    // Converting to a unique_ptr reference gives access to the
    // UserData's unique_ptr but does not move.
    static std::unique_ptr<T> &to(lua_State *L, int narg) {
        return UserData<T>::template getPtr<std::unique_ptr<T>>(L, narg);
    }
};

template <typename T>
struct ValueBinding<const std::unique_ptr<T>> : public ValueBinding<std::unique_ptr<T>> { };

template <typename T>
struct ValueBinding<const std::unique_ptr<T> &> {
    // Converting to a const unique_ptr reference gives access to the
    // UserData's unique_ptr but does not move.
    static const std::unique_ptr<T> &to(lua_State *L, int narg) {
        return UserData<T>::template getPtr<std::unique_ptr<T>>(L, narg);
    }
};

// const values which are built in lua types are treated though they
// were non-const. For example, pushing an explicit const int pushes
// an int.
template <typename T>
struct ValueBinding<const T, typename std::enable_if<IsBuiltinLuaType<T>::value>::type> :
    public ValueBinding<T> { };

template <typename T>
struct ValueBinding<T, typename std::enable_if<std::is_integral<T>::value ||
                                               std::is_enum<T>::value>::type> {
    // Pushing an integer creates a lua integer
    static void push(lua_State *L, T val) {
        lua_pushinteger(L, static_cast<lua_Integer>(val));
    }

    // Converting to an integer casts from a lua integer
    static T to(lua_State *L, int narg) {
        return static_cast<T>(luaL_checkint(L, narg));
    }
};

template <typename T>
struct ValueBinding<T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
    // Pushing a floating point number creates a lua number
    static void push(lua_State *L, T val) {
        lua_pushnumber(L, static_cast<lua_Number>(val));
    }

    // Converting to a floating point number casts from a lua number
    static T to(lua_State *L, int narg) {
        return static_cast<T>(luaL_checknumber(L, narg));
    }
};

template <>
struct ValueBinding<std::string> {
    static void push(lua_State *L, const std::string &str) {
        lua_pushlstring(L, str.c_str(), str.length());
    }

    static std::string to(lua_State *L, int narg) {
        return {luaL_checkstring(L, narg)};
    }
};

template <>
struct ValueBinding<bool> {
    static void push(lua_State *L, bool b) {
        lua_pushboolean(L, b);
    }

    static bool to(lua_State *L, int narg) {
        return lua_toboolean(L, narg) != 0;
    }
};

template <typename B, typename T>
void pushCType(lua_State *L, T && val) {
    ValueBinding<B>::push(L, std::forward<T>(val));
}

inline void pushAllCType(lua_State *L, TypeSequence<>) {
}    

template <typename B, typename... Bs, typename T, typename... Ts>
void pushAllCType(lua_State *L, TypeSequence<B, Bs...>, T&& val, Ts&&... vals) {
    pushCType<B>(L, std::forward<T>(val));
    pushAllCType(L, TypeSequence<Bs...>(), std::forward<Ts>(vals)...);
}

template <typename T>
T toCType(lua_State *L, int narg) {
    return ValueBinding<T>::to(L, narg);
}

namespace detail {
    template <typename... Ts, size_t... Seq>
    std::tuple<Ts...> toAllHelper(lua_State* L,
                                  int narg,
                                  NumericSequence<Seq...>) {
        return std::tuple<Ts...>(std::forward<Ts>(toCType<Ts>(L, narg+Seq))...);
    }
}

template <typename... Ts>
std::tuple<Ts...> toAllCType(lua_State *L, int narg) {
    return detail::toAllHelper<Ts...>(L, narg, AscendingSequence<sizeof...(Ts)>());
}

namespace detail {
    // TODO writing these in Lua almost certainly faster
    inline int metaIndex(lua_State *L) {
        // Check for getter function
        if (lua_isstring(L, 2)) {
            std::string key = lua_tostring(L, 2);
            lua_pushstring(L, ("get_" + key).c_str());
            lua_gettable(L, lua_upvalueindex(1));
            if (!lua_isnil(L, -1)) {
                lua_pushvalue(L, 1);
                lua_call(L, 1, 1);
                return 1;
            }
        }

        // Check for method
        lua_pushvalue(L, 2);
        lua_gettable(L, lua_upvalueindex(1));
        if (!lua_isnil(L, -1)) {
            return 1;
        }

        // Check for index method
        lua_pushstring(L, "index");
        lua_gettable(L, lua_upvalueindex(1));
        if (!lua_isnil(L, -1)) {
            lua_pushvalue(L, 1);
            lua_pushvalue(L, 2);
            lua_call(L, 2, 1);
            return 1;
        }

        return 1;
    }

    inline int metaNewIndex(lua_State *L) {
        if (lua_isstring(L, 2)) {
            std::string key = lua_tostring(L, 2);
            lua_pushstring(L, ("set_" + key).c_str());
            lua_gettable(L, lua_upvalueindex(1));
            if (!lua_isnil(L, -1)) {
                lua_pushvalue(L, 1);
                lua_pushvalue(L, 3);
                lua_call(L, 2, 0);
                return 0;
            }
        }

        lua_pushstring(L, "newindex");
        lua_gettable(L, lua_upvalueindex(1));
        if (!lua_isnil(L, -1)) {
            lua_pushvalue(L, 1);
            lua_pushvalue(L, 2);
            lua_pushvalue(L, 3);
            lua_call(L, 3, 0);
            return 0;
        }

        lua_pushstring(L, "Attempt to write to unknown field");
        return lua_error(L);
    }
    
    template <typename T>
    int metaGC(lua_State *L) {
        UserData<T>::destroy(L, 1);
        return 0;
    }        
}

template <typename T>
class MetatableBuilder {
public:
    MetatableBuilder(lua_State *L, const std::string &clsname) : L(L), clsname(clsname) {
        luaL_newmetatable(L, typeid(T).name());
        lua_newtable(L);
        lua_pushvalue(L, -1);
        lua_setglobal(L, clsname.c_str());
        lua_pushvalue(L, -1);
        lua_pushcclosure(L, detail::metaIndex, 1);
        lua_setfield(L, -3, "__index");
        lua_pushcclosure(L, detail::metaNewIndex, 1);
        lua_setfield(L, -2, "__newindex");
        lua_pushcfunction(L, detail::metaGC<T>);
        lua_setfield(L, -2, "__gc");
        lua_pop(L, 1);
    }

    template <typename... Args>
    MetatableBuilder<T> &constructor(const char *name) {
        lua_getglobal(L, clsname.c_str());
        lua_pushcfunction(L, [](lua_State *L) -> int {
            auto argstuple = toAllCType<Args...>(L, 1);
            pushCType<std::unique_ptr<T>>(
                L, std::unique_ptr<T>{invokeWithTuple(
                        bindConstructors<T>(),
                        argstuple)});
            return 1;
        });
        lua_setfield(L, -2, name);
        lua_pop(L, 1);
        return *this;
    }

    template <typename Ret, typename... Args>
    MetatableBuilder<T> &function(const char *name, Ret (T::*func)(Args...)) {
        lua_getglobal(L, clsname.c_str());
        
        void *funcstorage = lua_newuserdata(L, sizeof(func));
        new (funcstorage) decltype(func) { func };

        lua_pushcclosure(L, [](lua_State *L) -> int {
            auto objptr = toCType<T *>(L, 1);
            auto argstuple = toAllCType<Args...>(L, 2);
            auto funcptr = reinterpret_cast<decltype(func) *>(
                lua_touserdata(L, lua_upvalueindex(1)));
            pushCType<Ret>(L, invokeWithTuple(
                               bindMemberFunction(*objptr, *funcptr),
                               argstuple));
            return 1;
        }, 1);
        lua_setfield(L, -2, name);
        lua_pop(L, 1);
        return *this;
    }

    template <typename... Args>
    MetatableBuilder<T> &function(const char *name, void (T::*func)(Args...)) {
        lua_getglobal(L, clsname.c_str());

        void *funcstorage = lua_newuserdata(L, sizeof(func));
        new (funcstorage) decltype(func) { func };
        
        lua_pushcclosure(L, [](lua_State *L) -> int {
            auto objptr = toCType<T *>(L, 1);
            auto argstuple = toAllCType<Args...>(L, 2);
            auto funcptr = reinterpret_cast<decltype(func) *>(
                lua_touserdata(L, lua_upvalueindex(1)));
            invokeWithTuple(
                bindMemberFunction(*objptr, *funcptr),
                argstuple);
            return 0;
        }, 1);
        lua_setfield(L, -2, name);
        lua_pop(L, 1);
        return *this;
    }

    template <typename V>
    MetatableBuilder<T> &getter(const char *name, V T::*mem) {
        lua_getglobal(L, clsname.c_str());

        void *memstorage = lua_newuserdata(L, sizeof(mem));
        new (memstorage) decltype(mem) { mem };
        
        lua_pushcclosure(L, [](lua_State *L) -> int {
            auto objptr = toCType<T *>(L, 1);
            auto memptr = reinterpret_cast<decltype(mem) *>(
                lua_touserdata(L, lua_upvalueindex(1)));

            using Result = typename RemoveReferenceFromBuiltinLuaType<V &>::type;
            pushCType<Result>(L, objptr->**memptr);
            return 1;
        }, 1);
        lua_setfield(L, -2, (std::string{"get_"} + name).c_str());
        lua_pop(L, 1);
        return *this;
    }

    template <typename V>
    MetatableBuilder<T> &setter(const char *name, V T::*mem) {
        lua_getglobal(L, clsname.c_str());

        void *memstorage = lua_newuserdata(L, sizeof(mem));
        new (memstorage) decltype(mem) { mem };

        lua_pushcclosure(L, [](lua_State *L) -> int {
            auto objptr = toCType<T *>(L, 1);
            auto memptr = reinterpret_cast<decltype(mem) *>(
                lua_touserdata(L, lua_upvalueindex(1)));
            auto &&val = toCType<V>(L, 2);
            objptr->**memptr = val;
            return 0;
        }, 1);
        lua_setfield(L, -2, (std::string{"set_"} + name).c_str());
        lua_pop(L, 1);
        return *this;        
    }

    template <typename V>
    MetatableBuilder<T> &field(const char *name, V T::*mem) {
        return getter(name, mem).setter(name, mem);
    }

    template <typename K>
    MetatableBuilder<T> &index() {
        lua_getglobal(L, clsname.c_str());
        lua_pushcclosure(L, [](lua_State *L) -> int {
            auto objptr = toCType<T *>(L, 1);
            auto &&key = toCType<K>(L, 2);
            auto &&val = (*objptr)[key];

            using Result = typename RemoveReferenceFromBuiltinLuaType<
                decltype((*objptr)[key])>::type;
          
            pushCType<Result>(L, val);
            return 1;
        }, 0);
        lua_setfield(L, -2, "index");
        lua_pop(L, 1);
        return *this;
    }

    template <typename K, typename V>
    MetatableBuilder<T> &newindex() {
        lua_getglobal(L, clsname.c_str());
        lua_pushcfunction(L, [](lua_State *L) -> int {
            auto objptr = toCType<T *>(L, 1);
            auto &&keyptr = toCType<K>(L, 2);
            auto &&valptr = toCType<V>(L, 3);
            (*objptr)[keyptr] = valptr;
            return 0;
        });
        lua_setfield(L, -2, "newindex");
        lua_pop(L, 1);
        return *this;
    }

    template <typename K, typename V>
    MetatableBuilder<T> &rwindex() {
        return index<K>().newindex<K, V>();
    }
    
private:
    lua_State *L;
    std::string clsname;
};

namespace detail {
    inline int traceback(lua_State *L) {
        lua_getglobal(L, "debug");
        lua_getfield(L, -1, "traceback");
        lua_pushvalue(L, 1);
        lua_pushinteger(L, 2);
        lua_call(L, 2, 1);
        return 1;
    }
    
    template <typename Callable>
    struct CallLuaHelper { };

    template <typename... Args>
    struct CallLuaHelper<void (*)(Args...)> {
        template <typename... Ts>
        static void call(lua_State *L, const std::string &name, Ts&&... args) {
            static_assert(sizeof...(Args) == sizeof...(Ts), "Incorrect number of arguments");

            int top = lua_gettop(L);
            lua_pushcfunction(L, traceback);
            lua_getglobal(L, name.c_str());
            pushAllCType(L, TypeSequence<Args...>(), std::forward<Ts>(args)...);
            if (lua_pcall(L, sizeof...(Args), 0, top+1) != 0) {
                throw std::runtime_error(lua_tostring(L, -1));
            }
            lua_pop(L, 1);
        }
    };
    
    template <typename Ret, typename... Args>
    struct CallLuaHelper<Ret (*)(Args...)> {
        template <typename... Ts>
        static Ret call(lua_State *L, const std::string &name, Ts&&... args) {
            static_assert(sizeof...(Args) == sizeof...(Ts), "Incorrect number of arguments");

            int top = lua_gettop(L);
            lua_pushcfunction(L, traceback);
            lua_getglobal(L, name.c_str());
            pushAllCType(L, TypeSequence<Args...>(), std::forward<Ts>(args)...);
            if (lua_pcall(L, sizeof...(Args), 1, top+1) != 0) {
                throw std::runtime_error(lua_tostring(L, -1));
            }
            Ret val = toCType<Ret>(L, -1);
            lua_pop(L, 2);
            return val;
        }
    };
}
    
template <typename Callable, typename... Ts>
auto callLua(lua_State *L, const std::string &name, Ts&&... args)
-> decltype(detail::CallLuaHelper<Callable *>::call(L, name, std::forward<Ts>(args)...)) {
    return detail::CallLuaHelper<Callable *>::call(L, name, std::forward<Ts>(args)...);
}

#endif
