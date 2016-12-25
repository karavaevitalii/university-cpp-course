#ifndef BIND_H
#define BIND_H

#include <tuple>
#include <functional>

namespace bind
{
    template<size_t N>
    struct placeholder
    {};

    constexpr placeholder<0> _1;
    constexpr placeholder<1> _2;
    constexpr placeholder<2> _3;

    template<typename F, typename ... Args>
    struct bind_t
    {
        template<typename ... Args1>
        auto operator()(Args1&& ... args1) const
        {
            return call(typename geter<std::tuple_size<tuple_t>::value>::next()
                        , std::forward<Args1>(args1) ...);
        }

    private:
        typedef typename std::decay<F>::type func_t;
        typedef std::tuple<Args ...> tuple_t;

        bind_t(F&& func, Args&& ... args)
            : func(std::forward<F>(func))
            , args(std::forward<Args>(args) ...)
        {}

        template<typename Arg, typename ... Args1>
        auto&& getarg(Arg const& arg, Args1& ...) const
        {
            return arg;
        }

        template<size_t N, typename ... Args1>
        auto&& getarg(placeholder<N> const&, Args1& ... args1) const
        {
            return std::get<N>(std::forward_as_tuple(args1 ...));
        }

        template<typename B, typename ... Args1, typename ... Args2>
        auto getarg(bind_t<B, Args1 ...> const& b, Args2& ... args2) const
        {
            return b(args2 ...);
        }

        template<size_t ... N>
        struct arguments
        {};

        template<size_t N, size_t ... Seq>
        struct geter
        {
            typedef typename geter<N - 1, N - 1, Seq ...>::next next;
        };

        template<size_t ... Seq>
        struct geter<0, Seq ...>
        {
            typedef arguments<Seq ...> next;
        };

        template<size_t ... N, typename ... Args1>
        auto call(arguments<N ...> const&, Args1&& ... args1) const
        {
            return func(getarg(std::get<N>(args), args1 ...) ...);
        }

        template<typename Fn, typename ... Args1>
        friend bind_t<Fn, Args1 ...> bind(Fn&& f, Args1&& ... args);

        func_t func;
        tuple_t args;
    };

    template<typename F, typename ... Args>
    bind_t<F, Args ...> bind(F&& f, Args&& ... args)
    {
        return bind_t<F, Args ...>(std::forward<F>(f), std::forward<Args>(args) ...);
    }
}

#endif
