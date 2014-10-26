///////////////////////////////////////////////////////////////////////////////////////////////////
/// LuaConnect/Function.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "LuaConnect\Function.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Preprocessor
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "LuaConnect\Exceptions\LuaException.h"
#include "LuaConnect\Helpers\Headers.h"

namespace LuaConnect
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Function - Private Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    Function::Function(std::shared_ptr<State> state) : Ref(state)
    {
        Balance b(m_state, -1);

        if (!lua_isfunction(state->state, -1))
        {
            std::string name = lua_typename(state->state, lua_type(state->state, -1));
            lua_pop(state->state, 1);

            throw LuaException("Object at top of stack is not a Function (is " + name + ").");
        }

        int ref = luaL_ref(state->state, LUA_REGISTRYINDEX);
        Ref::Set(ref);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Function - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    Function::Function() : Ref(std::shared_ptr<State>())
    { }
    Function::Function(Function&& other) : Ref(static_cast<Ref&&>(other))
    { }

    Function& Function::operator=(Function&& other)
    {
        Ref::operator=(static_cast<Ref&&>(other));
        return *this;
    }

    bool Function::operator==(const Function& rhs) const
    {
        return Ref::operator==(rhs);
    }
    bool Function::operator!=(const Function& rhs) const
    {
        return !operator==(rhs);
    }
}