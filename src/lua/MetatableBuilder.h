#ifndef METATABLEBUILDER_H
#define METATABLEBUILDER_H

#include <lua.hpp>
#include <string>

// TODO naming convention
template <typename T>
class MetatableBuilder {
public:
    MetatableBuilder(lua_State *L, const std::string &clsname);

    template <typename... Args>
    MetatableBuilder<T> &constructor(const std::string &name);

    template <typename... Args>
    MetatableBuilder<T> &function(const std::string &name, void (T::*func)(Args...));

    template <typename Ret, typename... Args>
    MetatableBuilder<T> &function(const std::string &name, Ret (T::*func)(Args...));

    template <typename Ret, typename... Args>
    MetatableBuilder<T> &function_ref(const std::string &name, Ret (T::*func)(Args...));

    template <typename U>
    MetatableBuilder<T> &downCast(const std::string &name);
    
    template <typename V>
    MetatableBuilder<T> &getter(const std::string &name, V T::*mem);

    template <typename V>
    MetatableBuilder<T> &getter_ref(const std::string &name, V T::*mem);
    
    template <typename V>
    MetatableBuilder<T> &setter(const std::string &name, V T::*mem);

    template <typename V>
    MetatableBuilder<T> &field(const std::string &name, V T::*mem);

    template <typename V>
    MetatableBuilder<T> &field_ref(const std::string &name, V T::*mem);
    
    template <typename K>
    MetatableBuilder<T> &getindex();

    template <typename K, typename V>
    MetatableBuilder<T> &setindex();

    template <typename K, typename V>
    MetatableBuilder<T> &index();

    MetatableBuilder<T> &op_equals();

private:
    lua_State *L;
    std::string clsname;
};

#include "MetatableBuilder.icc"

#endif
