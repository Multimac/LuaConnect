///////////////////////////////////////////////////////////////////////////////////////////////////
/// LuaConnect/Helpers/Templates.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LUACONNECT_HELPERS_TEMPLATES
#define LUACONNECT_HELPERS_TEMPLATES

#include "..\Config.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Preprocessor
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <tuple>

namespace LuaConnect
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Struct - Index
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <int... I>
    struct Index { };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Struct - GenSequence
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <int N, int... Seq>
    struct GenSequence : GenSequence<N - 1, N - 1, Seq...> { };

    template <int... Seq>
    struct GenSequence<0, Seq...> : Index<Seq...>{};

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Struct - GenOffsetSequence
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <int Offset, int N, int... Seq>
    struct GenOffsetSequence : GenOffsetSequence<Offset, N - 1, N + Offset - 1, Seq...> { };

    template <int Offset, int... Seq>
    struct GenOffsetSequence<Offset, 0, Seq...> : Index<Seq...>{};

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Struct - TypeList
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename... Args>
    struct TypeList
    {
        template <int N>
        using At = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Struct - None
    ///////////////////////////////////////////////////////////////////////////////////////////////
    struct None { };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Struct - FunctionType
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename F>
    struct FunctionType;

    template <typename R, typename... Args>
    struct FunctionType<R(*)(Args...)>
    {
        using Ptr = R(*)(Args...);

        using Class = None;
        using Return = R;

        using Arguments = std::tuple<Args...>;
        using Arguments_Ref = std::tuple<Args&...>;
        using Arguments_ConstRef = std::tuple<const Args&...>;
    };
    template <typename T, typename R, typename... Args>
    struct FunctionType<R(T::*)(Args...)>
    {
        using Ptr = R(T::*)(Args...);

        using Class = T;
        using Return = R;

        using Arguments = std::tuple<Args...>;
        using Arguments_Ref = std::tuple<Args&...>;
        using Arguments_ConstRef = std::tuple<const Args&...>;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Class - Template
    ///////////////////////////////////////////////////////////////////////////////////////////////
    class Template
    {
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Members
    ///////////////////////////////////////////////////////////////////////////////////////////////
    private:
        template <typename... Args, int... Seq>
        static std::tuple<Args&...> MakeTie(std::tuple<Args...>& args, Index<Seq...>)
        {
            return std::tie(std::get<Seq>(args)...);
        }

        template <typename... Args, int... Seq>
        static auto DoSplit(std::tuple<Args&...> args, Index<Seq...>) -> decltype(std::forward_as_tuple(std::get<Seq>(args)...))
        {
            return std::forward_as_tuple(std::get<Seq>(args)...);
        }

        template <typename T, typename... Args, int... Seq>
        static void DoPlace(T* mem, std::tuple<const Args&...> args, Index<Seq...>)
        {
            new (mem)T(std::get<Seq>(args)...);
        }

    public:
        template <typename... Args>
        static std::tuple<const Args&...> ConstTie(std::tuple<Args...>& args)
        {
            return MakeTie(args, GenSequence<sizeof...(Args)>{});
        }
        template <typename... Args>
        static std::tuple<Args&...> Tie(std::tuple<Args...>& args)
        {
            return MakeTie(args, GenSequence<sizeof...(Args)>{});
        }

        template <int Start, int Count, typename... Args>
        static auto Split(std::tuple<Args&...> args) -> decltype(DoSplit(args, GenOffsetSequence<Start, Count>{}))
        {
            return DoSplit(args, GenOffsetSequence<Start, Count>{});
        }

        template <typename T, typename... Args>
        static void Place(T* mem, std::tuple<const Args&...> args)
        {
            DoPlace(mem, args, GenSequence<sizeof...(Args)>{});
        }
    };
}

#endif LUACONNECT_HELPERS_TEMPLATES