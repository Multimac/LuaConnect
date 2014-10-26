///////////////////////////////////////////////////////////////////////////////////////////////////
/// LuaConnect/Type.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "LuaConnect\Type.h"

namespace LuaConnect
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// UserdataMeta - Public Static Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    int GenericMeta::DummyDeconstruct(lua_State* state)
    {
        return 0;
    }

    int GenericMeta::Index(lua_State* state)
    {
        lua_getmetatable(state, 1);

        // Load the 'methods' table in the userdata metatable
        lua_pushstring(state, "__functions");
        lua_rawget(state, -2);

        // Try loading the name in the userdata 'methods' table
        lua_pushvalue(state, 2);
        lua_rawget(state, -2);

        return 1;
    }
    int GenericMeta::NewIndex(lua_State* state)
    {
        return 0;
    }

    int GenericMeta::ToString(lua_State* state)
    {
        // Create the State needed for calls to C++ code
        std::shared_ptr<State> luaState(new State(state));

        lua_getmetatable(state, -1);
        Table meta = Stack<Table>::Pop(luaState);

        std::string str = "userdata: " + meta.Get<std::string>("__name");
        Stack<std::string>::Push(luaState, str);

        return 1;
    }
}