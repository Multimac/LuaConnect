///////////////////////////////////////////////////////////////////////////////////////////////////
/// LuaConnect/Type.inl
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "Type.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Preprocessor
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "Helpers\Balance.h"
#include "Helpers\Headers.h"
#include "Helpers\Stack.h"
#include "VM.h"

#include <assert.h>
#include <iostream>
#include <sstream>

namespace LuaConnect
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Type::ConstructHandler - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename T>
    template <typename... Args>
    int Type<T>::ConstructHandler<Args...>::Call(lua_State* state)
    {
        // Check we got the correct number of arguments
        int reqArgs = std::tuple_size<std::tuple<Args...>>::value;
        if (lua_gettop(state) != reqArgs)
            return luaL_error(state, "Not enough arguments, expected %d got %d.", reqArgs, lua_gettop(state));

        // Create the State needed for calls to C++ code
        std::shared_ptr<State> luaState(new State(state));

        Balance b(luaState, 1);

        // Grab all the arguments off the stack
        std::tuple<Args...> args;
        try
        {
            StackHelper::GetStack(luaState, Template::Tie(args));
        }
        catch (LuaException& e)
        {
            return luaL_error(state, "%s", e.what());
        }

        // Create the userdata
        try
        {
            Userdata<T> userdata(luaState, Template::ConstTie(args));

            // Push it onto the stack
            Stack<Userdata<T>>::Push(luaState, userdata);
        }
        catch (LuaException& e)
        {
            return luaL_error(state, "%s", e.what());
        }

        return 1;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Type - Private Static Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename T>
    unsigned char Type<T>::s_key = 0;

    template <typename T>
    Table Type<T>::CreateMetatable(VM& vm, std::string name)
    {
        Balance b(vm.m_state, 0);

        // Create the metatable, registering the deconstructor, index and new index metamethods
        Table meta(vm);
        meta.Set("__gc", &GenericMeta::DummyDeconstruct);
        meta.Set("__index", &GenericMeta::Index);
        meta.Set("__newindex", &GenericMeta::NewIndex);

        // Create an empty table for functions in the metatable
        meta.Set("__functions", Table(vm));

        // Add the __tostring metamethod
        meta.Set("__name", name);
        meta.Set("__tostring", &GenericMeta::ToString);

        return meta;
    }

    template <typename T>
    int Type<T>::Deconstruct(lua_State* state)
    {
        // Create the State needed for calls to C++ code
        std::shared_ptr<State> luaState(new State(state));

        Balance b(luaState, 1);

        // Grab the userdata from the stack
        Userdata<T> userdata(luaState);;

        // Deconstruct it
        userdata.GetPointer()->~T();

        return 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Type - Public Static Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename T>
    Table Type<T>::GetMetatable(std::shared_ptr<State> state)
    {
        Balance b(state, 0);

        // Push the metatable from the registry
        lua_rawgetp(state->state, LUA_REGISTRYINDEX, Type<T>::ClassKey());

        // Check it exists
        if (lua_isnil(state->state, -1))
        {
            lua_pop(state->state, 1);
            throw LuaException("Type has not been registered with Lua.");
        }

        return Stack<Table>::Pop(state);
    }

    template <typename T>
    bool Type<T>::Exists(VM& vm)
    {
        return !vm.GetMetatableName(Type<T>::ClassKey()).empty();
    }
    template <typename T>
    void Type<T>::RegisterType(VM& vm, std::string name)
    {
        assert(!name.empty());

        Balance b(vm.m_state, 0);

        // Check if the metatable already exists
        lua_rawgetp(vm.m_state->state, LUA_REGISTRYINDEX, Type<T>::ClassKey());
        if (lua_istable(vm.m_state->state, -1))
        {
            lua_pop(vm.m_state->state, 1);
            return;
        }

        // Pop the value returned from the registry
        lua_pop(vm.m_state->state, 1);

        // Store the type name
        Table meta = CreateMetatable(vm, name);

        Stack<Table>::Push(vm.m_state, meta);
        lua_rawsetp(vm.m_state->state, LUA_REGISTRYINDEX, Type<T>::ClassKey());
    }

    template <typename T>
    template <typename... Args>
    void Type<T>::AddConstructor(VM& vm, Table& typeTable)
    {
        Balance b(vm.m_state, 0);

        // Get the metatable from the registry
        Table meta = GetMetatable(vm.m_state);

        // Use the name as the key, and the function as the value, and store it in the type table
        typeTable.Set(meta.Get<std::string>("__name"), &Type<T>::ConstructHandler<Args...>::Call);
    }
    template <typename T>
    void Type<T>::AddDeconstructor(VM& vm)
    {
        Balance b(vm.m_state, 0);
            
        // Get the metatable from the registry
        lua_rawgetp(vm.m_state->state, LUA_REGISTRYINDEX, Type<T>::ClassKey());
        Table meta = Stack<Table>::Pop(vm.m_state);

        meta.Set("__gc", &Type<T>::Deconstruct);
    }

    template <typename T>
    template <typename F, typename... Upvalues>
    void Type<T>::AddFunction(VM& vm, std::string name, F func, const Upvalues&... upvalues)
    {
        Balance b(vm.m_state, 0);

        // Get the metatable from the registry
        Table meta = GetMetatable(vm.m_state);

        // Get the functions table from the metatable
        Table functions = meta.Get<Table>(std::string("__functions"));

        // Set the function in the functions table
        functions.Set(name, Function::CreateFunction(vm, func, upvalues...));
    }
}