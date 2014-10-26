///////////////////////////////////////////////////////////////////////////////////////////////////
/// LuaConnect/Helpers/Stack.inl
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "Stack.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Preprocessor
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "State.h"
#include "Templates.h"

namespace LuaConnect
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Stack<const char[]> - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <std::size_t N>
    void Stack<const char[N]>::Push(std::shared_ptr<State> state, const char(&value)[N])
    {
        Stack<std::string>::Push(state, std::string(value));
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Stack<Userdata> - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename T>
    Userdata<T> Stack<Userdata<T>>::Get(std::shared_ptr<State> state, int index)
    {
        lua_pushvalue(state->state, index);
        return Userdata<T>(state);
    }

    template <typename T>
    Userdata<T> Stack<Userdata<T>>::Pop(std::shared_ptr<State> state)
    {
        Userdata<T> result = Stack<Userdata<T>>::Get(state, -1);
        lua_pop(state->state, 1);

        return result;
    }
    template <typename T>
    void Stack<Userdata<T>>::Push(std::shared_ptr<State> state, const Userdata<T>& value)
    {
        lua_rawgeti(state->state, LUA_REGISTRYINDEX, value.m_ref);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// StackHelper::StackPusher - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <int N, typename... Args>
    void StackHelper::StackPusher<N, Args...>::PushIndex(std::shared_ptr<State> state, std::tuple<const Args&...> args)
    {
        Stack<TypeList<Args...>::At<sizeof...(Args)-N>>::Push(state, std::get<sizeof...(Args)-N>(args));
    }
    template <int N, typename... Args>
    void StackHelper::StackPusher<N, Args...>::PushRecursive(std::shared_ptr<State> state, std::tuple<const Args&...> args)
    {
        StackHelper::StackPusher<N, Args...>::PushIndex(state, args);
        StackHelper::StackPusher<N - 1, Args...>::PushRecursive(state, args);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// StackHelper::StackRetriever - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <int N, typename... Args>
    void StackHelper::StackRetriever<N, Args...>::GetIndex(std::shared_ptr<State> state, std::tuple<Args&...> args)
    {
        const int index = N - ((int)sizeof...(Args)+1);
        std::get<N - 1>(args) = Stack<TypeList<Args...>::At<N - 1>>::Get(state, index);
    }
    template <int N, typename... Args>
    void StackHelper::StackRetriever<N, Args...>::GetRecursive(std::shared_ptr<State> state, std::tuple<Args&...> args)
    {
        StackHelper::StackRetriever<N, Args...>::GetIndex(state, args);
        StackHelper::StackRetriever<N - 1, Args...>::GetRecursive(state, args);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// StackHelper::UpvalueRetriever - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <int N, typename... Args>
    void StackHelper::UpvalueRetriever<N, Args...>::GetIndex(std::shared_ptr<State> state, std::tuple<Args&...> args)
    {
        const int index = lua_upvalueindex(N);
        std::get<N - 1>(args) = Stack<TypeList<Args...>::At<N - 1>>::Get(state, index);
    }
    template <int N, typename... Args>
    void StackHelper::UpvalueRetriever<N, Args...>::GetRecursive(std::shared_ptr<State> state, std::tuple<Args&...> args)
    {
        StackHelper::UpvalueRetriever<N, Args...>::GetIndex(state, args);
        StackHelper::UpvalueRetriever<N - 1, Args...>::GetRecursive(state, args);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// StackHelper - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename... Args>
    void StackHelper::GetStack(std::shared_ptr<State> state, std::tuple<Args&...> args)
    {
        Balance b(state, 0);
        StackHelper::StackRetriever<sizeof...(Args), Args...>::GetRecursive(state, args);
    }
    template <typename... Args>
    void StackHelper::GetUpvalues(std::shared_ptr<State> state, std::tuple<Args&...> args)
    {
        Balance b(state, 0);
        StackHelper::UpvalueRetriever<sizeof...(Args), Args...>::GetRecursive(state, args);
    }

    template <typename... Args>
    void StackHelper::Push(std::shared_ptr<State> state, std::tuple<const Args&...> args)
    {
        Balance b(state, (int)sizeof...(Args));
        StackHelper::StackPusher<sizeof...(Args), Args...>::PushRecursive(state, args);
    }
}