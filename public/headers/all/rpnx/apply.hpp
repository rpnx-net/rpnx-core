

// Copyright 2015, 2016, 2020 Ryan P. Nicholl <rnicholl@protonmail.com>
// All rights reserved

#ifndef RPNX_APPLY_HH
#define RPNX_APPLY_HH

#include <functional>
#include <tuple>



namespace rpnx
{

  namespace detail
  {
    template <typename F, typename T, typename std::size_t ... Is>
    static constexpr decltype(auto) invoke_helper_f2(F && f, T && t, std::index_sequence<Is...>)
    {
      return f(std::get<Is>(std::forward<T>(t))...);
    }
      
    template <typename F, typename T, typename Idx = std::make_index_sequence< std::tuple_size< std::decay_t< T> >::value > >
    static constexpr decltype(auto) invoke_helper(F && f, T && t)
    {
      return invoke_helper_f2(std::forward<F>(f), std::forward<T>(t), Idx());
    }

          
    template <size_t N, typename F, typename T, size_t L  = std::tuple_size< std::decay_t< T> >::value, bool C = (N == L) >
    class apply_helper;
    
  

    template  <size_t N, typename F, typename T, size_t L >
    struct apply_helper<N, F, T, L, false>
    {
      static constexpr decltype(auto) exec(F && f, T && t)
      {
        f(std::get<N>(std::forward<T>(t)));
        return apply_helper<N+1, F, T, L, (N+1 == L)>::exec(std::forward<F>(f), std::forward<T>(t));
      }
    };

    
    template  <size_t N, typename F, typename T, size_t L>
    struct apply_helper<N, F, T, L, true>
    {
      static constexpr decltype(auto) exec(F && f, T && t)
      {
      
        return;
      }
     

    };

          
    template <size_t N, typename F, typename T, size_t L, bool C = (N == L) >
    class apply_n_helper;
    
  

    template  <size_t N, typename F, typename T, size_t L >
    struct apply_n_helper<N, F, T, L, false>
    {
      static constexpr void exec(F && f, T && t)
      {
        f.template operator()<N>(std::forward<T>(t));
        apply_n_helper<N+1, F, T, L>::exec(std::forward<F>(f), std::forward<T>(t));
      }
    };

    
    template  <size_t N, typename F, typename T, size_t L>
    struct apply_n_helper<N, F, T, L, true>
    {
      static constexpr decltype(auto) exec(F && f, T && t)
      {      
        return;
      }
     

    };

  }
  
  template <typename F, typename T>
  constexpr decltype(auto) invoke(F && f, T && t)
  {
    return detail::invoke_helper(std::forward<F>(f), std::forward<T>(t));
  }

  template <typename F, typename T>
  constexpr decltype(auto) apply(F && f, T && t)
  {
    return detail::apply_helper<0, F, T>::exec(std::forward<F>(f), std::forward<T>(t));
  }

  template < size_t L, typename F, typename T>
  constexpr void apply_n(F && f, T && t)
  {
    return detail::apply_n_helper<0, F, T, L>::exec(std::forward<F>(f), std::forward<T>(t));
  }


}

#endif
