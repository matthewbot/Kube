#ifndef METATABLEBUILDER_H
#define METATABLEBUILDER_H

#include "lua/UserData.h"
#include "lua/bindings.h"

#include <lua.hpp>
#include <typeinfo>

namespace detail {
    int metaIndex(lua_State *L);
    int metaNewIndex(lua_State *L);
    
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

    template <typename K, typename V>
    MetatableBuilder<T> &index() {
        lua_getglobal(L, clsname.c_str());
        lua_pushcclosure(L, [](lua_State *L) -> int {
            auto objptr = toCType<T *>(L, 1);
            auto &&key = toCType<K>(L, 2);
            auto &&val = (*objptr)[key];
            pushCType<V>(L, val);
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
        return index<K, V>().newindex<K, V>();
    }
    
private:
    lua_State *L;
    std::string clsname;
};

#endif
