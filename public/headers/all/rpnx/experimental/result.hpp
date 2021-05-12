//
// Created by Ryan on 4/26/2021.
//

#ifndef RPNXCORE_RESULT_HPP
#define RPNXCORE_RESULT_HPP

#include <variant>

namespace rpnx::experimental
{
    template <typename T>
    class result
    {
        std::variant<T, std::exception_ptr> m_variant;
      public:
        result(std::exception_ptr ptr)
        : m_variant(ptr)
        {}

        template <typename ... Ts>
        result(Ts && ... ts)
        : m_variant(std::forward<Ts>(ts)...)
        {

        }

        T & get()
        {
            if (m_variant.index() == 1)
            {
                auto exptr = std::get<1>(m_variant);
                std::rethrow_exception(exptr);
            }

            return std::get<0>(m_variant);
        }

        T const & get() const
        {
            if (m_variant.index() == 1)
            {
                auto exptr = std::get<1>(m_variant);
                std::rethrow_exception(exptr);
            }

            return std::get<0>(m_variant);
        }
    };

}

#endif // RPNXCORE_RESULT_HPP
