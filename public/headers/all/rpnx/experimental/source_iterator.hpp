#ifndef RPNXCORE_SOURCE_ITERATOR_HPP
#define RPNXCORE_SOURCE_ITERATOR_HPP

#include <memory>
#include <string>
#include <iterator>

namespace rpnx::experimental
{
    template < typename It >
    class forward_source_iterator
    {
      public:
        // using reference =
      private:
        It m_iter;
        std::shared_ptr< std::string > m_filename;

        std::size_t m_line = 0;
        std::size_t m_col = 0;
        enum class state
        {
            text,
            r,
            n
        };
        state m_state = state::text;

      public:
        forward_source_iterator(It val = It(), std::string fname = "") : m_iter(val), m_filename(std::make_shared< std::string >(fname))
        {
        }
        forward_source_iterator(forward_source_iterator< It > const&) = default;
        forward_source_iterator(forward_source_iterator< It >&&) = default;

        forward_source_iterator< It >& operator=(forward_source_iterator< It > const&) = default;
        forward_source_iterator< It >& operator=(forward_source_iterator< It >&&) = default;

        bool operator==(forward_source_iterator< It > const& other) const
        {
            return m_iter == other.m_iter;
        }
        bool operator!=(forward_source_iterator< It > const& other) const
        {
            return m_iter != other.m_iter;
        }
        bool operator<(forward_source_iterator< It > const& other) const
        {
            return m_iter < other.m_iter;
        }
        bool operator<=(forward_source_iterator< It > const& other) const
        {
            return m_iter <= other.m_iter;
        }
        bool operator>(forward_source_iterator< It > const& other) const
        {
            return m_iter > other.m_iter;
        }
        bool operator>=(forward_source_iterator< It > const& other) const
        {
            return m_iter >= other.m_iter;
        }

        forward_source_iterator< It >& operator++()
        {
            m_col++;
            if (*m_iter == '\n' && !(m_state == state::r))
            {
                m_state = state::n;
                m_line++;
                m_col = 0;
            }
            else if (*m_iter == '\r')
            {
                m_state = state::r;
                m_line++;
                m_col = 0;
            }
            else
            {
                m_state = state::text;
            }
            ++m_iter;
            return *this;
        }
        forward_source_iterator< It > operator++(int)
        {
            forward_source_iterator< It > copy = *this;
            ++*this;
            return copy;
        }

        auto operator*() const
        {
            return *m_iter;
        }

        std::size_t line() const
        {
            if (m_state == state::r)
            {
                if (*m_iter == '\n')
                {
                    return m_line;
                }
                else
                {
                    return m_line;
                }
            }
            return m_line;
        }

        std::size_t column() const
        {
            if (m_state == state::r)
            {
                if (*m_iter == '\n')
                {
                    return m_col;
                }
                else
                {
                    return 0;
                }
            }
            return m_col;
        }
        std::string file_name() const
        {
            return *m_filename;
        }

        // TODO: figure out how to implement this efficiently
        /*
         *
        forward_source_iterator<It>& operator--()
        {
            m_iter --;

            if ((*m_iter == '\r' || m_iter == '\n') && !(*m_iter == '\r' && m_state == '\n'))
            {
                m
            }

            return *this;
        }
         */
    };


    template <typename It>
    inline auto make_source_iterator(std::string const & filename, It at, std::size_t line = 0, std::size_t col = 0)
    {
        return forward_source_iterator< It >(at, filename, line, col);
    }

    template < typename C >
    inline auto make_forward_source_iterator_pair(std::string const& fname, C&& container)
    {
        auto begin = std::begin(container);
        auto end = std::end(container);
        return std::make_pair(forward_source_iterator< decltype(begin) >(begin, fname), forward_source_iterator< decltype(begin) >(end));
    }
} // namespace rpnx::experimental


namespace std
{
    template <typename It>
    struct iterator_traits< rpnx::experimental::forward_source_iterator< It > >
    {
        using value_type = typename std::iterator_traits< It >::value_type;
        using pointer = typename std::iterator_traits< It >::pointer;
        using iterator_category = std::forward_iterator_tag;
        using reference = typename std::iterator_traits< It >::reference;
    };
}




#endif // RPNXCORE_SOURCE_ITERATOR_HPP
