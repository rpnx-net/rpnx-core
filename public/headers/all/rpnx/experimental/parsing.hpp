//
// Created by rnicholl on 2/2/2021.
//

#ifndef RPNXCORE_PARSING_HPP
#define RPNXCORE_PARSING_HPP

#include <string>
#include <regex>

namespace rpnx::experimental
{
    namespace parse
    {
        template <typename It>
        struct parsing_state
        {
            It m_begin;
            It m_end;
            It m_pos;

            parsing_state(It a_begin, It a_end)
            : m_begin(a_begin), m_end(a_end),
                  m_pos(a_begin)
            {

            }

            std::string examine_next(std::regex a_re) const
            {

            }



        };


    }
}

#endif // RPNXCORE_PARSING_HPP
