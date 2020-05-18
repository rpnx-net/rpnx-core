
// Copyright 2015, 2016, 2020 Ryan P. Nicholl <rnicholl@protonmail.com>
// All rights reserved

#ifndef RPNX_UTIL_HH
#define RPNX_UTIL_HH
#include <cinttypes>

namespace rpnx
{
  template <typename T>
  constexpr bool is_type_in_pack();
  
  template <typename T, typename T2, typename ... Ts>
  constexpr bool is_type_in_pack()
  {
    if (std::is_same<T, T2>::value) return true;
    return is_type_in_pack<T, Ts...>();
  }

  template <typename T>
  constexpr bool is_type_in_pack()
  {
    return false;
  }
  
  template <typename T, size_t N>
  constexpr int pack_type_index();

  template <typename T, size_t N, typename T2, typename ... Ts>
  constexpr int pack_type_index()
  {
    if (std::is_same<T, T2>::value) return N;
    return pack_type_index<T, N+1, Ts...>();
  }

  template <typename T, size_t N>
  constexpr int pack_type_index()
  {
    return -1;
  }

  template <typename T, typename ... Ts>
  class type_in
  {
    static constexpr bool value = (is_type_in_pack<T, Ts...>());
  };

  template <typename T, typename ... Ts>
  constexpr int tuple_type_index_helper(std::tuple<Ts...> const &)
  {
    return pack_type_index<T, 0, Ts...>();
  }

  template <typename T, typename Tpl>
  class tuple_type_index
  {
    static constexpr int value = tuple_type_index_helper<T>(*reinterpret_cast<Tpl*>(nullptr));
  };

  static_assert(is_type_in_pack<bool, int, bool>() == true, "");
  static_assert(is_type_in_pack<int, int, bool>() == true, "");
  static_assert(is_type_in_pack<float, int, bool>() == false, "");
}


#endif
