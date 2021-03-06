///////////////////////////////////////////////////////////////////////////////////////////////////
/// LuaConnect/Helpers/State.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "LuaConnect\Helpers\State.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Preprocessor
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "LuaConnect\Helpers\Headers.h"

namespace LuaConnect
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// State - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    State::State() : m_managed(true), state(luaL_newstate())
    {
        luaL_openlibs(state);
    }
    State::State(lua_State* state) : m_managed(false), state(state)
    { }

    State::~State()
    {
        if (m_managed)
            lua_close(state);
    }
}