///////////////////////////////////////////////////////////////////////////////////////////////////
/// LuaConnect/Table.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "LuaConnect\Table.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Preprocessor
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "LuaConnect\Exceptions\LuaException.h"
#include "LuaConnect\Helpers\Headers.h"

namespace LuaConnect
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Table - Private Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    Table::Table(std::shared_ptr<State> state) : Ref(state)
    {
        Balance b(m_state, -1);

        if (!lua_istable(state->state, -1))
        {
            std::string name = lua_typename(state->state, lua_type(state->state, -1));
            lua_pop(state->state, 1);

            throw LuaException("Object at top of stack is not a Table (is " + name + ").");
        }

        int ref = luaL_ref(state->state, LUA_REGISTRYINDEX);
        Ref::Set(ref);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Table - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    Table::Table() : Ref(std::shared_ptr<State>())
    { }
    Table::Table(Table&& other) : Ref(static_cast<Ref&&>(other))
    { }
    Table::Table(VM& vm) : Ref(vm.m_state)
    {
        Balance b(m_state, 0);

        lua_newtable(vm.m_state->state);

        int ref = luaL_ref(vm.m_state->state, LUA_REGISTRYINDEX);
        Ref::Set(ref);
    }

    Table& Table::operator=(Table&& other)
    {
        Ref::operator=(static_cast<Ref&&>(other));
        return *this;
    }

    bool Table::operator==(const Table& rhs) const
    {
        return Ref::operator==(rhs);
    }
    bool Table::operator!=(const Table& rhs) const
    {
        return !operator==(rhs);
    }
}