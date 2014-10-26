///////////////////////////////////////////////////////////////////////////////////////////////////
/// LuaConnect/Function.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LUACONNECT_FUNCTION
#define LUACONNECT_FUNCTION

#include "Config.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Preprocessor
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "Helpers\Ref.h"
#include "Helpers\Templates.h"

#include <memory>
#include <tuple>

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Forward Declarations
///////////////////////////////////////////////////////////////////////////////////////////////////
struct lua_State;

namespace LuaConnect
{
    class State;
    class VM;
}

namespace LuaConnect
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Class - Function
    ///////////////////////////////////////////////////////////////////////////////////////////////
    class LUACONNECT_API Function : private Ref
    {
        friend VM;

        template <typename T>
        friend class Stack;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Nested Types
    ///////////////////////////////////////////////////////////////////////////////////////////////
    private:
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Struct - CallHandler
        ///////////////////////////////////////////////////////////////////////////////////////////
        template <typename R, typename... Args>
        struct CallHandler
        {
            static R Call(Function& function, const Args&... args);
        };
        template <typename... Args>
        struct CallHandler<void, Args...>
        {
            static void Call(Function& function, const Args&... args);
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Class - Callback
        ///////////////////////////////////////////////////////////////////////////////////////////
        template <typename F>
        class Callback;

        template <typename R, typename... Args>
        class Callback<R(*)(Args...)>
        {
        private:
            using FuncPtr = R(*)(Args...);

        public:
            template <int... Seq>
            static R PerformCallback(FuncPtr func, std::tuple<const Args&...> args, Index<Seq...>);

            template <typename... Upvalues>
            static int Call(lua_State* state);
        };
        template <typename R, typename T, typename... Args>
        class Callback<R(T::*)(Args...)>
        {
        private:
            using FuncPtr = R(T::*)(Args...);

        public:
            template <int... Seq>
            static R PerformCallback(FuncPtr func, T* obj, std::tuple<const Args&...> args, Index<Seq...>);

            template <typename... Upvalues>
            static int Call(lua_State* state);
        };
        template <typename R, typename T, typename... Args>
        class Callback<R(T::*)(Args...) const>
        {
        private:
            using FuncPtr = R(T::*)(Args...) const;

        public:
            template <int... Seq>
            static R PerformCallback(FuncPtr func, const T* obj, std::tuple<const Args&...> args, Index<Seq...>);

            template <typename... Upvalues>
            static int Call(lua_State* state);
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Struct - Handler
        ///////////////////////////////////////////////////////////////////////////////////////////
        template <typename F>
        struct Handler;

        template <typename R, typename... Args>
        struct Handler<R(*)(Args...)>
        {
            using FuncPtr = R(*)(Args...);
            static int Call(std::shared_ptr<State> state, FuncPtr func, std::tuple<const Args&...> args);
        };
        template <typename... Args>
        struct Handler<void(*)(Args...)>
        {
            using FuncPtr = void(*)(Args...);
            static int Call(std::shared_ptr<State> state, FuncPtr func, std::tuple<const Args&...> args);
        };
        template <typename T, typename R, typename... Args>
        struct Handler<R(T::*)(Args...)>
        {
            using FuncPtr = R(T::*)(Args...);
            static int Call(std::shared_ptr<State> state, FuncPtr func, T* obj, std::tuple<const Args&...> args);
        };
        template <typename T, typename... Args>
        struct Handler<void(T::*)(Args...)>
        {
            using FuncPtr = void(T::*)(Args...);
            static int Call(std::shared_ptr<State> state, FuncPtr func, T* obj, std::tuple<const Args&...> args);
        };
        template <typename T, typename R, typename... Args>
        struct Handler<R(T::*)(Args...) const>
        {
            using FuncPtr = R(T::*)(Args...) const;
            static int Call(std::shared_ptr<State> state, FuncPtr func, const T* obj, std::tuple<const Args&...> args);
        };
        template <typename T, typename... Args>
        struct Handler<void(T::*)(Args...) const>
        {
            using FuncPtr = void(T::*)(Args...) const;
            static int Call(std::shared_ptr<State> state, FuncPtr func, const T* obj, std::tuple<const Args&...> args);
        };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Static Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    public:
        template <typename F, typename... Upvalues>
        static Function CreateFunction(VM& vm, F func, const Upvalues&... upvalues);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    private:
        Function(std::shared_ptr<State> state);

        template <typename... Args>
        void PerformCall(std::tuple<const Args&...> args);

    public:
        Function();
        Function(Function&& other);

        Function& operator=(Function&& other);

        bool operator==(const Function& rhs) const;
        bool operator!=(const Function& rhs) const;

        template <typename R, typename... Args>
        R Call(const Args&... args);
    };
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Inline Includes
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "Function.inl"

#endif LUACONNECT_FUNCTION