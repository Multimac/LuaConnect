///////////////////////////////////////////////////////////////////////////////////////////////////
/// LuaConnect/Table.inl
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "Table.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Preprocessor
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "Exceptions\LuaException.h"
#include "Function.h"
#include "Helpers\Balance.h"
#include "Helpers\Headers.h"
#include "Helpers\Stack.h"

namespace LuaConnect
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Table::CallHandler - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename R, typename K, typename... Args>
    R Table::CallHandler<R, K, Args...>::Call(Table& table, K key, const Args&... args)
    {
        Function f = table.Get<Function>(key);
        return f.Call<R>(args...);
    }
    template <typename K, typename... Args>
    void Table::CallHandler<void, K, Args...>::Call(Table& table, K key, const Args&... args)
    {
        Function f = table.Get<Function>(key);
        f.Call<void>(args...);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Table - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename K>
    bool Table::Exists(const K& key)
    {
        Balance b(m_state, 0);

        Ref::Push();
        Stack<K>::Push(m_state, key);

        lua_gettable(m_state->state, -2);

        bool exists = !lua_isnil(m_state->state, -1);
        lua_pop(m_state->state, 2);

        return exists;
    }
    template <typename V, typename K>
    V Table::Get(const K& key)
    {
        Balance b(m_state, 0);

        Ref::Push();
        Stack<K>::Push(m_state, key);

        lua_gettable(m_state->state, -2);

        try
        {
            V result = Stack<V>::Get(m_state, -1);
            lua_pop(m_state->state, 2);

            return result;
        }
        catch (LuaException&)
        {
            lua_pop(m_state->state, 2);
            throw;
        }
    }
    template <typename V, typename K>
    void Table::Set(const K& key, const V& value)
    {
        Balance b(m_state, 0);

        Ref::Push();
        Stack<K>::Push(m_state, key);
        Stack<V>::Push(m_state, value);

        lua_settable(m_state->state, -3);
        lua_pop(m_state->state, 1);
    }

    template <typename R, typename K, typename... Args>
    R Table::Call(const K& key, const Args&... args)
    {
        Balance b(m_state, 0);

        return Table::CallHandler<R, K, Args...>::Call(*this, key, args...);
    }
}