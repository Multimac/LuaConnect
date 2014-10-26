///////////////////////////////////////////////////////////////////////////////////////////////////
/// LuaConnect/Helpers/Stack.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "LuaConnect\Helpers\Stack.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Preprocessor
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "LuaConnect\Helpers\Headers.h"

namespace LuaConnect
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Stack<Nil> - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    void Stack<Nil>::Push(std::shared_ptr<State> state)
    {
        lua_pushnil(state->state);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Stack<bool> - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool Stack<bool>::Get(std::shared_ptr<State> state, int index)
    {
        return (lua_toboolean(state->state, index) == 1);
    }

    bool Stack<bool>::Pop(std::shared_ptr<State> state)
    {
        bool result = Stack<bool>::Get(state, -1);
        lua_pop(state->state, 1);

        return result;
    }
    void Stack<bool>::Push(std::shared_ptr<State> state, const bool& value)
    {
        lua_pushboolean(state->state, value);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Stack<lua_Integer> - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    lua_Integer Stack<lua_Integer>::Get(std::shared_ptr<State> state, int index)
    {
        return lua_tointeger(state->state, index);
    }

    lua_Integer Stack<lua_Integer>::Pop(std::shared_ptr<State> state)
    {
        lua_Integer result = Stack<lua_Integer>::Get(state, -1);
        lua_pop(state->state, 1);

        return result;
    }
    void Stack<lua_Integer>::Push(std::shared_ptr<State> state, const lua_Integer& value)
    {
        lua_pushinteger(state->state, value);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Stack<lua_Unsigned> - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    lua_Unsigned Stack<lua_Unsigned>::Get(std::shared_ptr<State> state, int index)
    {
        return lua_tounsigned(state->state, index);
    }

    lua_Unsigned Stack<lua_Unsigned>::Pop(std::shared_ptr<State> state)
    {
        lua_Unsigned result = Stack<lua_Unsigned>::Get(state, -1);
        lua_pop(state->state, 1);

        return result;
    }
    void Stack<lua_Unsigned>::Push(std::shared_ptr<State> state, const lua_Unsigned& value)
    {
        lua_pushunsigned(state->state, value);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Stack<lua_Number> - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    lua_Number Stack<lua_Number>::Get(std::shared_ptr<State> state, int index)
    {
        return lua_tonumber(state->state, index);
    }

    lua_Number Stack<lua_Number>::Pop(std::shared_ptr<State> state)
    {
        lua_Number result = Stack<lua_Number>::Get(state, -1);
        lua_pop(state->state, 1);

        return result;
    }
    void Stack<lua_Number>::Push(std::shared_ptr<State> state, const lua_Number& value)
    {
        lua_pushnumber(state->state, value);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Stack<const char*> - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    void Stack<const char*>::Push(std::shared_ptr<State> state, const char* value)
    {
        Stack<std::string>::Push(state, std::string(value));
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Stack<std::string> - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    std::string Stack<std::string>::Get(std::shared_ptr<State> state, int index)
    {
        size_t len = 0;
        const char* str = lua_tolstring(state->state, index, &len);

        if (!str) // string constructor has undefined behaviour if str is NULL
            return std::string();

        return std::string(str, len);
    }

    std::string Stack<std::string>::Pop(std::shared_ptr<State> state)
    {
        std::string result = Stack<std::string>::Get(state, -1);
        lua_pop(state->state, 1);

        return result;
    }
    void Stack<std::string>::Push(std::shared_ptr<State> state, const std::string& value)
    {
        lua_pushlstring(state->state, value.c_str(), value.size());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Stack<lua_CFunction> - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    lua_CFunction Stack<lua_CFunction>::Get(std::shared_ptr<State> state, int index)
    {
        return lua_tocfunction(state->state, index);
    }

    lua_CFunction Stack<lua_CFunction>::Pop(std::shared_ptr<State> state)
    {
        lua_CFunction result = Stack<lua_CFunction>::Get(state, -1);
        lua_pop(state->state, 1);

        return result;
    }
    void Stack<lua_CFunction>::Push(std::shared_ptr<State> state, const lua_CFunction& value, int upvalueCount)
    {
        lua_pushcclosure(state->state, value, upvalueCount);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Stack<Function> - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    Function Stack<Function>::Get(std::shared_ptr<State> state, int index)
    {
        lua_pushvalue(state->state, index);
        return Function(state);
    }

    Function Stack<Function>::Pop(std::shared_ptr<State> state)
    {
        Function result = Stack<Function>::Get(state, -1);
        lua_pop(state->state, 1);

        return result;
    }
    void Stack<Function>::Push(std::shared_ptr<State> state, const Function& value)
    {
        lua_rawgeti(state->state, LUA_REGISTRYINDEX, value.m_ref);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Stack<Table> - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    Table Stack<Table>::Get(std::shared_ptr<State> state, int index)
    {
        lua_pushvalue(state->state, index);
        return Table(state);
    }

    Table Stack<Table>::Pop(std::shared_ptr<State> state)
    {
        Table result = Stack<Table>::Get(state, -1);
        lua_pop(state->state, 1);

        return result;
    }
    void Stack<Table>::Push(std::shared_ptr<State> state, const Table& value)
    {
        lua_rawgeti(state->state, LUA_REGISTRYINDEX, value.m_ref);
    }
}