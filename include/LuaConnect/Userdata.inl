///////////////////////////////////////////////////////////////////////////////////////////////////
/// LuaConnect/Userdata.inl
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "Userdata.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Preprocessor
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "Helpers\Balance.h"
#include "Helpers\Headers.h"
#include "VM.h"
#include "Type.h"

namespace LuaConnect
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Userdata - Public Static Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename T>
    Userdata<T> Userdata<T>::CreateCopy(VM& vm, const T& value)
    {
        return CreateCustomCopy<T>(vm, value);
    }
    template <typename T>
    Userdata<T> Userdata<T>::CreateRef(VM& vm, const T& value)
    {
        return CreateCustomRef<T>(vm, value);
    }

    template <typename T>
    template <typename U>
    Userdata<T> Userdata<T>::CreateCustomCopy(VM& vm, const T& value)
    {
        Balance b(m_state, 0);

        return CreateCustomCopy(vm, value, Type<U>::Metatable(vm.m_state));
    }
    template <typename T>
    Userdata<T> Userdata<T>::CreateCustomCopy(VM& vm, const T& value, const Table& metatable)
    {
        Balance b(vm.m_state, 0);

        T* data = static_cast<T*>(lua_newuserdata(vm.m_state->state, sizeof(T)));
        new (data)T(value);

        Userdata<T> userdata(vm.m_state);
        userdata.SetMetatable(metatable);

        return userdata;
    }
    template <typename T>
    template <typename U>
    Userdata<T> Userdata<T>::CreateCustomRef(VM& vm, const T& value)
    {
        Balance b(m_state, 0);

        return CreateCustomRef(vm, value, Type<U>::GetMetatable(vm.m_state));
    }
    template <typename T>
    Userdata<T> Userdata<T>::CreateCustomRef(VM& vm, const T& value, const Table& metatable)
    {
        Balance b(vm.m_state, 0);

        lua_pushlightuserdata(vm.m_state->state, (void*)&value);

        Userdata<T> userdata(vm.m_state);
        userdata.SetMetatable(metatable);

        return userdata;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Userdata - Private Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename T>
    Userdata<T>::Userdata() : Ref(std::shared_ptr<State>())
    { }
    template <typename T>
    Userdata<T>::Userdata(std::shared_ptr<State> state) : Ref(state)
    {
        Balance b(m_state, -1);

        if (!lua_isuserdata(state->state, -1))
        {
            std::string name = lua_typename(state->state, lua_type(state->state, -1));
            lua_pop(state->state, 1);

            throw LuaException("Object at top of stack is not Userdata (is " + name + ").");
        }

        // Get the metatable of the object on top of the stack;
        lua_getmetatable(state->state, -1);

        Table meta(state);

        // Get the metatable we expect
        Table expected = Type<T>::GetMetatable(state);
            
        // Compare actual to expected
        if (meta != expected)
        {
            lua_pop(state->state, 1);
            throw LuaException("Object at top of stack has wrong metatable.");
        }

        int ref = luaL_ref(state->state, LUA_REGISTRYINDEX);
        Ref::Set(ref);
    }
    template <typename T>
    template <typename... Args>
    Userdata<T>::Userdata(std::shared_ptr<State> state, std::tuple<const Args&...> args) : Ref(state)
    {
        Balance b(state, 0);

        T* userdata = static_cast<T*>(lua_newuserdata(state->state, sizeof(T)));

        try
        {
            Template::Place(userdata, args);
        }
        catch (const std::exception& e)
        {
            lua_pop(state->state, 1);
            throw LuaException(std::string("Exception during construction: ") + e.what());
        }
        catch (...)
        {
            lua_pop(state->state, 1);
            throw LuaException("Unknown exception during construction.");
        }

        int ref = luaL_ref(state->state, LUA_REGISTRYINDEX);
        Ref::Set(ref);

        SetMetatable(Type<T>::GetMetatable(state));
    }

    template <typename T>
    void Userdata<T>::SetMetatable(const Table& metatable)
    {
        Balance b(m_state, 0);

        Stack<Userdata<T>>::Push(m_state, *this);
        Stack<Table>::Push(m_state, metatable);

        lua_setmetatable(m_state->state, -2);

        lua_pop(m_state->state, 1);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Userdata - Public Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename T>
    Userdata<T>::Userdata(Userdata<T>&& other) : Ref(static_cast<Ref&&>(other))
    { }
    template <typename T>
    template <typename... Args>
    Userdata<T>::Userdata(VM& vm, const Args&... args) : Userdata(vm.m_state, std::tuple<const Args&...>(args...))
    { }

    template <typename T>
    Userdata<T>& Userdata<T>::operator=(Userdata&& other)
    {
        Ref::operator=(static_cast<Ref&&>(other));
        return *this;
    }

    template <typename T>
    bool Userdata<T>::operator==(const Userdata<T>& rhs) const
    {
        return Ref::operator==(rhs);
    }
    template <typename T>
    bool Userdata<T>::operator!=(const Userdata<T>& rhs) const
    {
        return !operator==(rhs);
    }

    template <typename T>
    T* Userdata<T>::GetPointer()
    {
        Balance b(m_state, 0);

        Ref::Push();
        T* result = static_cast<T*>(lua_touserdata(m_state->state, -1));

        lua_pop(m_state->state, 1);

        return result;
    }
}