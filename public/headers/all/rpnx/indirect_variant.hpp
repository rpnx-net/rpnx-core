#ifndef RPNX_INDIRECT_VARIANT_HPP
#define RPNX_INDIRECT_VARIANT_HPP

// Copyright 2015, 2016, 2020 Ryan P. Nicholl <rnicholl@protonmail.com>
// All rights reserved


#include <typeinfo>
#include <utility>
#include <typeindex>

#include "rpnx/util.hpp"
#include "rpnx/apply.hpp"

#include <iostream>

namespace rpnx
{

  namespace detail
  {

  class variant_type_info
  {


  public:
    std::type_index in_type_index;
    void (*in_deleter)(void*);
    void* (*in_copier)(void*);
  //  bool (*in_equals)(void const*, void const *);
  //  bool (*in_less)(void const*, void const *);
    std::string (*in_typename)();
    int in_which;

  };

  template <typename T, typename ... Ts>
  variant_type_info const type_info_for_type
      {
          std::type_index(typeid(T)),
          +[](void*ptr){ delete static_cast<T*>(ptr); },
          +[](void*ptr){ return static_cast<void*> (new T (*static_cast<T*>(ptr))); },
         // +[](void const*lhs, void const* rhs) -> bool { return *reinterpret_cast<T const *>(lhs) ==  *reinterpret_cast<T const *>(rhs);  },
         // +[](void const*lhs, void const* rhs) -> bool { return *reinterpret_cast<T const *>(lhs) <  *reinterpret_cast<T const *>(rhs);  },
          +[]()->std::string{return typeid(T).name();},
          pack_type_index<T, 0, Ts...>()
      };
  }


  template <typename ... Ts>
  class indirect_variant
  {
  public:
    using value_types = std::tuple<Ts...>;

    indirect_variant()
        : in_data(nullptr), in_type(nullptr)
    {

    }


    indirect_variant(indirect_variant<Ts...> && other)
        : indirect_variant()
    {
      std::swap(this->in_data, other.in_data);
      std::swap(this->in_type, other.in_type);
    }

    indirect_variant(indirect_variant<Ts...> const & other) // This function is required because the template variant binds too strongly...
        : indirect_variant()
    {

      init_variant(other);
    }


    template <typename T>
    indirect_variant(T && t)
    {
      init_variant(std::forward<T>(t));
    }

    ~indirect_variant()
    {
      if (!this->in_data) return;
      this->in_type->in_deleter(this->in_data);
    }


    int which() const
    {
      return in_type->in_which;
    }

    bool empty() const
    {
      return this->in_data == nullptr;
    }

    const std::type_index & type_index() const
    {
      return this->in_type->in_type_index;
    }

    template <typename T>
    indirect_variant & operator = (T && other)
    {
      indirect_variant v (std::forward<T>(other));
      this->swap(v);
      return *this;
    }

    void swap (indirect_variant & v) noexcept
    {
      std::swap(v.in_data, this->in_data);
      std::swap(v.in_type, this->in_type);
    }


    template <typename T>
    T & value()
    {

      if (!this->in_type || this->in_type->in_typename() != detail::type_info_for_type<typename std::remove_reference<T>::type, Ts...>.in_typename())
      {
        //std::cout << "Bad cast from type " << this->in_type->in_typename() << " to " << detail::type_info_for_type<typename std::remove_reference<T>::type, Ts...>.in_typename() << std::endl;
        throw std::invalid_argument("bad cast");
      }
      return *static_cast<T*>(this->in_data);
    }

    template <typename T>
    T const & value() const
    {
      //      std::cout << this->in_type->in_typename() << std::endl;
      if (!this->in_type || this->in_type->in_typename() != detail::type_info_for_type<typename std::remove_reference<T>::type, Ts...>.in_typename())
        //      if (this->in_type != &detail::type_info_for_type<typename std::remove_reference<T>::type, Ts...>)
      {
        //std::cout << "Bad cast from type " << this->in_type->in_typename() << " to " << detail::type_info_for_type<typename std::remove_reference<T>::type, Ts...>.in_typename() << std::endl;
        throw std::invalid_argument("bad cast");
      }
      return *static_cast<T const*>(this->in_data);
    }

    template <typename T>
    bool is() const
    {
      return (this->in_type == &detail::type_info_for_type<typename std::remove_reference<T>::type, Ts...>);
    }


    template <typename T>
    explicit operator T&()
    {
      if (!this->is<T>()) throw std::invalid_argument("bad cast");
      return this->value<T>();
    }

    template <typename T>
    explicit operator T() const
    {
      if (!this->is<T>()) throw std::invalid_argument("bad cast");
      return this->value<T>();
    }



/*
    bool operator == (indirect_variant<Ts ...> const & other ) const
    {
      if  (this->which() != other.which()) return false;
      return this->in_type->in_equals(this->in_data, other.in_data);
    }

    bool operator < (indirect_variant<Ts ...> const & other ) const
    {
      if (this->which() < other.which()) return true;
      if (this->which() > other.which()) return false;

      return this->in_type->in_less(this->in_data, other.in_data);
    }
*/
  private:
    void init_variant(indirect_variant<Ts...> const& other)
    {
      if (!other.in_data)
      {
        this->in_data = nullptr;
        this->in_type = nullptr;
        return;
      }
      this->in_data = other.in_type->in_copier(other.in_data);
      this->in_type = other.in_type;
    }

    void init_variant(indirect_variant<Ts...> & other)
    {
      if (!other.in_data)
      {
        this->in_data = nullptr;
        this->in_type = nullptr;
        return;
      };
      this->in_data = other.in_type->in_copier(other.in_data);
      this->in_type = other.in_type;
    }


    template <typename T>
    auto init_variant(T && t) -> typename std::enable_if< is_type_in_pack<typename std::remove_const_t <typename std::remove_reference<T>::type>, Ts...>() >::type
    {
      this->in_type = &detail::type_info_for_type<typename std::remove_const_t <typename std::remove_reference<T>::type>, Ts...>;
      this->in_data = (void *) new typename std::remove_const_t <typename std::remove_reference<T>::type>(std::forward<T>(t));
    }

   // template <typename T>
   // auto /*ERROR: type T is not in variant type, no implicit conversions to member types. */ init_variant(T && t, bool b = false) -> typename std::enable_if< ! is_type_in_pack<typename std::remove_const_t <typename std::remove_reference<T>::type>, Ts...>() >::type = delete;




  private:
    void * in_data;
    detail::variant_type_info const * in_type;
  };



namespace detail
{
  template <typename F>
  class variant_invoke_helper
  {
    F & functor;

  public:
    variant_invoke_helper(F & input_arg)
        : functor(input_arg) {}


    template <size_t N, typename ...  Ts>
    inline void operator() (indirect_variant<Ts...>  const & var)
    {
      if (var.which() == N)
      {
        functor(std::forward<decltype(var)>(var).template value<typename std::tuple_element<N, std::tuple<Ts...> >::type>());
      }
    }

    template <size_t N, typename ...  Ts>
    inline void operator() (indirect_variant<Ts...>  & var)
    {
      if (var.which() == N)
      {
        functor(std::forward<decltype(var)>(var).template value<typename std::tuple_element<N, std::tuple<Ts...> >::type>());
      }
    }


    template <size_t N, typename ...  Ts>
    inline void operator() (indirect_variant<Ts...>  && var)
    {
      if (var.which() == N)
      {
        functor(std::forward<decltype(var)>(var).template value<typename std::tuple_element<N, std::tuple<Ts...> >::type>());
      }
    }



  };
}

template <typename F2, typename ... Ts>
auto variant_invoke(F2 && f, rpnx::indirect_variant<Ts...> && v)
{
  detail::variant_invoke_helper<F2> h(f);
  rpnx::apply_n<std::tuple_size<std::tuple<Ts...> >::value>(h, std::forward<decltype(v)>(v));
}

template <typename F2, typename ... Ts>
auto variant_invoke(F2 && f, rpnx::indirect_variant<Ts...> const& v)
{
  detail::variant_invoke_helper<F2> h(f);
  rpnx::apply_n<std::tuple_size<std::tuple<Ts...> >::value>(h, std::forward<decltype(v)>(v));
}

template <typename F2, typename ... Ts>
auto variant_invoke(F2 && f, rpnx::indirect_variant<Ts...> & v)
{
  detail::variant_invoke_helper<F2> h(f);
  rpnx::apply_n<std::tuple_size<std::tuple<Ts...> >::value>(h, std::forward<decltype(v)>(v));
}


}
#endif
