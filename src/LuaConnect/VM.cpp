///////////////////////////////////////////////////////////////////////////////////////////////////
/// LuaConnect/VM.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "LuaConnect\VM.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Preprocessor
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "LuaConnect\Helpers\State.h"
#include "LuaConnect\Helpers\Stack.h"
#include "LuaConnect\Table.h"

namespace LuaConnect
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// VM - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    VM::VM() : m_state(new State())
    { }

    Table VM::GetGlobalTable()
    {
        Balance b(m_state, 0);

        lua_rawgeti(m_state->state, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
        return Stack<Table>::Pop(m_state);
    }

    Function VM::LoadBuffer(std::string buffer, const Table* environment)
    {
        Balance b(m_state, 0);

        int err = luaL_loadbufferx(m_state->state, buffer.c_str(), buffer.size(), "buffer", "t");
        if (err != LUA_OK)
        {
            std::string errStr = Stack<std::string>::Pop(m_state);
            switch (err)
            {
            case LUA_ERRGCMM:
                throw LuaException("LUA_ERRGCMM: " + errStr);
            case LUA_ERRMEM:
                throw LuaException("LUA_ERRMEM: " + errStr);
            case LUA_ERRSYNTAX:
                throw LuaException("LUA_ERRSYNTAX: " + errStr);
            }
        }

        if (environment)
        {
            Stack<Table>::Push(m_state, *environment);
            lua_setupvalue(m_state->state, -2, 1);
        }

        return Stack<Function>::Pop(m_state);
    }
    Function VM::LoadFile(std::string filename, const Table* environment)
    {
        Balance b(m_state, 0);

        int err = luaL_loadfilex(m_state->state, filename.c_str(), "t");
        if (err != LUA_OK)
        {
            std::string errStr = Stack<std::string>::Pop(m_state);
            switch (err)
            {
            case LUA_ERRFILE:
                throw LuaException("LUA_ERRFILE: " + errStr);
            case LUA_ERRGCMM:
                throw LuaException("LUA_ERRGCMM: " + errStr);
            case LUA_ERRMEM:
                throw LuaException("LUA_ERRMEM: " + errStr);
            case LUA_ERRSYNTAX:
                throw LuaException("LUA_ERRSYNTAX: " + errStr);
            }
        }

        if (environment)
        {
            Stack<Table>::Push(m_state, *environment);
            lua_setupvalue(m_state->state, -2, 1);
        }

        return Stack<Function>::Pop(m_state);
    }
}