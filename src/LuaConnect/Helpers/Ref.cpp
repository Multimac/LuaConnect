///////////////////////////////////////////////////////////////////////////////////////////////////
/// LuaConnect/Helpers/Ref.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "LuaConnect\Helpers\Ref.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Preprocessor
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "LuaConnect\Helpers\Headers.h"
#include "LuaConnect\Helpers\State.h"

#include <assert.h>

namespace LuaConnect
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Ref - Protected Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    Ref::Ref(std::shared_ptr<State> state) : m_state(state), m_ref(LUA_NOREF)
    { }

    Ref::Ref(const Ref& other) : m_state(other.m_state), m_ref(LUA_NOREF)
    {
        other.Push();
        m_ref = luaL_ref(m_state->state, LUA_REGISTRYINDEX);
    }
    Ref::Ref(Ref&& other) : m_state(other.m_state), m_ref(other.m_ref)
    {
        other.m_ref = LUA_NOREF;
    }

    Ref::~Ref()
    {
        if (m_ref != LUA_NOREF)
            luaL_unref(m_state->state, LUA_REGISTRYINDEX, m_ref);
    }

    Ref& Ref::operator=(const Ref& other)
    {
        m_state = other.m_state;

        other.Push();
        m_ref = luaL_ref(m_state->state, LUA_REGISTRYINDEX);

        return *this;
    }
    Ref& Ref::operator=(Ref&& other)
    {
        m_state = other.m_state;

        m_ref = other.m_ref;
        other.m_ref = LUA_NOREF;

        return *this;
    }

    bool Ref::operator==(const Ref& rhs) const
    {
        Push();
        rhs.Push();

        int comp = lua_compare(m_state->state, -2, -1, LUA_OPEQ);
        lua_pop(m_state->state, 2);

        return (comp == 1);
    }
    bool Ref::operator!=(const Ref& rhs) const
    {
        return !operator==(rhs);
    }

    void Ref::Push() const
    {
        lua_rawgeti(m_state->state, LUA_REGISTRYINDEX, m_ref);
    }
    void Ref::Set(int ref)
    {
        assert(ref != LUA_REFNIL);

        luaL_unref(m_state->state, LUA_REGISTRYINDEX, m_ref);
        m_ref = ref;
    }
}