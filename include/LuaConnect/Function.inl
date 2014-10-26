///////////////////////////////////////////////////////////////////////////////////////////////////
/// LuaConnect/Function.inl
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "Function.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Preprocessor
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "Helpers\Balance.h"
#include "Helpers\Headers.h"
#include "Helpers\Stack.h"

#include <iostream>

namespace LuaConnect
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Function::CallHandler - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename R, typename... Args>
    R Function::CallHandler<R, Args...>::Call(Function& function, const Args&... args)
    {
        try
        {
            function.PerformCall(std::forward_as_tuple(args...));
            return Stack<R>::Pop(function.m_state);
        }
        catch (...)
        {
            lua_pop(function.m_state->state, 1);
            throw;
        }
    }
    template <typename... Args>
    void Function::CallHandler<void, Args...>::Call(Function& function, const Args&... args)
    {
        try
        {
            function.PerformCall(std::forward_as_tuple(args...));
            lua_pop(function.m_state->state, 1);
        }
        catch (...)
        {
            lua_pop(function.m_state->state, 1);
            throw;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Function::Callback - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename R, typename... Args>
    template <int... Seq>
    R Function::Callback<R(*)(Args...)>::PerformCallback(FuncPtr func, std::tuple<const Args&...> args, Index<Seq...>)
    {
        return (*func)(std::get<Seq>(args)...);
    }

    template <typename R, typename... Args>
    template <typename... Upvalues>
    int Function::Callback<R(*)(Args...)>::Call(lua_State* state)
    {
        // Check we got the correct number of arguments
        int reqArgs = static_cast<int>(sizeof...(Args));
        int upvalueCount = static_cast<int>(sizeof...(Upvalues));

        if (lua_gettop(state) < reqArgs - upvalueCount)
            return luaL_error(state, "Not enough arguments, expected %d got %d.", reqArgs - upvalueCount + 1, lua_gettop(state));

        // Create the State needed for calls to C++ code
        std::shared_ptr<State> luaState(new State(state));

        // Get the function pointer from the upvalues
        FuncPtr* func = (FuncPtr*)lua_touserdata(state, lua_upvalueindex(upvalueCount + 1));

        // Create the tuple of arguments
        std::tuple<Args...> args;

        // Get the upvalues
        auto upvalueArgs = Template::Split<0, sizeof...(Upvalues)>(Template::Tie(args));
        try
        {
            StackHelper::GetUpvalues(luaState, upvalueArgs);
        }
        catch (LuaException& e)
        {
            return luaL_error(state, "%s", e.what());
        }

        // Get the arguments from the stack
        auto stackArgs = Template::Split<sizeof...(Upvalues), sizeof...(Args)-sizeof...(Upvalues)>(Template::Tie(args));
        try
        {
            StackHelper::GetStack(luaState, stackArgs);
        }
        catch (LuaException& e)
        {
            return luaL_error(state, "%s", e.what());
        }

        // Call the callback handler
        try
        {
            return Handler<FuncPtr>::Call(luaState, *func, Template::ConstTie(args));
        }
        catch (const std::exception& e)
        {
            return luaL_error(state, "Exception during execution: %s", e.what());
        }
        catch (...)
        {
            return luaL_error(state, "Unknown exception during execution.");
        }
    }

    template <typename R, typename T, typename... Args>
    template <int... Seq>
    R Function::Callback<R(T::*)(Args...)>::PerformCallback(FuncPtr func, T* obj, std::tuple<const Args&...> args, Index<Seq...>)
    {
        return (obj->*func)(std::get<Seq>(args)...);
    }

    template <typename R, typename T, typename... Args>
    template <typename... Upvalues>
    int Function::Callback<R(T::*)(Args...)>::Call(lua_State* state)
    {
        // Check we got the correct number of arguments
        int reqArgs = static_cast<int>(sizeof...(Args));
        int upvalueCount = static_cast<int>(sizeof...(Upvalues));

        if (lua_gettop(state) < reqArgs - upvalueCount + 1)
            return luaL_error(state, "Not enough arguments, expected %d got %d.", reqArgs - upvalueCount + 1, lua_gettop(state));

        // Create the State needed for calls to C++ code
        std::shared_ptr<State> luaState(new State(state));

        // Get the function pointer from the upvalues
        FuncPtr* func = (FuncPtr*)lua_touserdata(state, lua_upvalueindex(upvalueCount + 1));
        T* obj = (T*)lua_touserdata(state, 1);

        // Create the tuple of arguments
        std::tuple<Args...> args;

        // Get the upvalues
        auto upvalueArgs = Template::Split<0, sizeof...(Upvalues)>(Template::Tie(args));
        try
        {
            StackHelper::GetUpvalues(luaState, upvalueArgs);
        }
        catch (LuaException& e)
        {
            return luaL_error(state, "%s", e.what());
        }

        // Get the arguments from the stack
        auto stackArgs = Template::Split<sizeof...(Upvalues), sizeof...(Args)-sizeof...(Upvalues)>(Template::Tie(args));
        try
        {
            StackHelper::GetStack(luaState, stackArgs);
        }
        catch (LuaException& e)
        {
            return luaL_error(state, "%s", e.what());
        }

        // Call the callback handler
        try
        {
            return Handler<FuncPtr>::Call(luaState, *func, obj, Template::ConstTie(args));
        }
        catch (const std::exception& e)
        {
            return luaL_error(state, "Exception during execution: %s", e.what());
        }
        catch (...)
        {
            return luaL_error(state, "Unknown exception during execution.");
        }
    }

    template <typename R, typename T, typename... Args>
    template <int... Seq>
    R Function::Callback<R(T::*)(Args...) const>::PerformCallback(FuncPtr func, const T* obj, std::tuple<const Args&...> args, Index<Seq...>)
    {
        return (obj->*func)(std::get<Seq>(args)...);
    }

    template <typename R, typename T, typename... Args>
    template <typename... Upvalues>
    int Function::Callback<R(T::*)(Args...) const>::Call(lua_State* state)
    {
        // Check we got the correct number of arguments
        int reqArgs = static_cast<int>(sizeof...(Args));
        int upvalueCount = static_cast<int>(sizeof...(Upvalues));

        if (lua_gettop(state) < reqArgs - upvalueCount + 1)
            return luaL_error(state, "Not enough arguments, expected %d got %d.", reqArgs - upvalueCount + 1, lua_gettop(state));

        // Create the State needed for calls to C++ code
        std::shared_ptr<State> luaState(new State(state));

        // Get the function pointer from the upvalues
        FuncPtr* func = (FuncPtr*)lua_touserdata(state, lua_upvalueindex(upvalueCount + 1));
        const T* obj = (const T*)lua_touserdata(state, 1);

        // Create the tuple of arguments
        std::tuple<Args...> args;

        // Get the upvalues
        auto upvalueArgs = Template::Split<0, sizeof...(Upvalues)>(Template::Tie(args));
        try
        {
            StackHelper::GetUpvalues(luaState, upvalueArgs);
        }
        catch (LuaException& e)
        {
            return luaL_error(state, "%s", e.what());
        }

        // Get the arguments from the stack
        auto stackArgs = Template::Split<sizeof...(Upvalues), sizeof...(Args)-sizeof...(Upvalues)>(Template::Tie(args));
        try
        {
            StackHelper::GetStack(luaState, stackArgs);
        }
        catch (LuaException& e)
        {
            return luaL_error(state, "%s", e.what());
        }

        // Call the callback handler
        try
        {
            return Handler<FuncPtr>::Call(luaState, *func, obj, Template::ConstTie(args));
        }
        catch (const std::exception& e)
        {
            return luaL_error(state, "Exception during execution: %s", e.what());
        }
        catch (...)
        {
            return luaL_error(state, "Unknown exception during execution.");
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Function::Handler - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename R, typename... Args>
    int Function::Handler<R(*)(Args...)>::Call(std::shared_ptr<State> state, FuncPtr func, std::tuple<const Args&...> args)
    {
        R result = Callback<FuncPtr>::PerformCallback(func, args, GenSequence<sizeof...(Args)>{});
        Stack<R>::Push(state, result);

        return 1;
    }
    template <typename... Args>
    int Function::Handler<void(*)(Args...)>::Call(std::shared_ptr<State> state, FuncPtr func, std::tuple<const Args&...> args)
    {
        Callback<FuncPtr>::PerformCallback(func, args, GenSequence<sizeof...(Args)>{});

        return 0;
    }
    template <typename T, typename R, typename... Args>
    int Function::Handler<R(T::*)(Args...)>::Call(std::shared_ptr<State> state, FuncPtr func, T* obj, std::tuple<const Args&...> args)
    {
        R result = Callback<FuncPtr>::PerformCallback(func, obj, args, GenSequence<sizeof...(Args)>{});
        Stack<R>::Push(state, result);

        return 1;
    }
    template <typename T, typename... Args>
    int Function::Handler<void(T::*)(Args...)>::Call(std::shared_ptr<State> state, FuncPtr func, T* obj, std::tuple<const Args&...> args)
    {
        Callback<FuncPtr>::PerformCallback(func, obj, args, GenSequence<sizeof...(Args)>{});

        return 0;
    }
    template <typename T, typename R, typename... Args>
    int Function::Handler<R(T::*)(Args...) const>::Call(std::shared_ptr<State> state, FuncPtr func, const T* obj, std::tuple<const Args&...> args)
    {
        R result = Callback<FuncPtr>::PerformCallback(func, obj, args, GenSequence<sizeof...(Args)>{});
        Stack<R>::Push(state, result);

        return 1;
    }
    template <typename T, typename... Args>
    int Function::Handler<void(T::*)(Args...) const>::Call(std::shared_ptr<State> state, FuncPtr func, const T* obj, std::tuple<const Args&...> args)
    {
        Callback<FuncPtr>::PerformCallback(func, obj, args, GenSequence<sizeof...(Args)>{});

        return 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Function - Public Static Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename F, typename... Upvalues>
    Function Function::CreateFunction(VM& vm, F func, const Upvalues&... upvalues)
    {
        Balance b(vm.m_state, 0);

        // Push other upvalues onto the stack
        StackHelper::Push(vm.m_state, std::tuple<const Upvalues&...>(upvalues...));

        // Create a copy of the function pointer as a lua userdata (on the stack)
        F* fp = (F*)lua_newuserdata(vm.m_state->state, sizeof(F));
        new (fp)F(func);

        // Create the C closure, binding the upvalues with it
        lua_pushcclosure(vm.m_state->state, &Callback<F>::Call<Upvalues...>, sizeof...(Upvalues)+1);

        return Function(vm.m_state);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Function - Private Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename... Args>
    void Function::PerformCall(std::tuple<const Args&...> args)
    {
        Balance b(m_state, 1);

        Ref::Push();
        StackHelper::Push(m_state, args);

        int err = lua_pcall(m_state->state, std::tuple_size<decltype(args)>::value, 1, 0);
        if (err != LUA_OK)
        {
            std::string errStr = Stack<std::string>::Pop(m_state);

            // Push nil to balance stack
            Stack<Nil>::Push(m_state);

            switch (err)
            {
            case LUA_ERRERR:
                throw LuaException("LUA_ERRERR: " + errStr);
            case LUA_ERRGCMM:
                throw LuaException("LUA_ERRGCMM: " + errStr);
            case LUA_ERRMEM:
                throw LuaException("LUA_ERRMEM: " + errStr);
            case LUA_ERRRUN:
                throw LuaException("LUA_ERRRUN: " + errStr);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Function - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename R, typename... Args>
    R Function::Call(const Args&... args)
    {
        Balance b(m_state, 0);
        return Function::CallHandler<R, Args...>::Call(*this, args...);
    }
}