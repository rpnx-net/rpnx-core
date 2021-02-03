#ifndef RPNXCORE_SOURCE_ITERATOR_HPP
#define RPNXCORE_SOURCE_ITERATOR_HPP

#include <memory>
#include <string>

namespace rpnx::experimental
{
    template < typename It >
    class source_iterator
    {
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
        source_iterator(It val = It(), std::string fname = "") : m_iter(val), m_filename(std::make_shared< std::string >(fname))
        {
        }
        source_iterator(source_iterator< It > const&) = default;
        source_iterator(source_iterator< It >&&) = default;

        source_iterator< It >& operator=(source_iterator< It > const&) = default;
        source_iterator< It >& operator=(source_iterator< It >&&) = default;

        bool operator==(source_iterator< It > const& other) const
        {
            return m_iter == other.m_iter;
        }
        bool operator!=(source_iterator< It > const& other) const
        {
            return m_iter != other.m_iter;
        }
        bool operator<(source_iterator< It > const& other) const
        {
            return m_iter < other.m_iter;
        }
        bool operator<=(source_iterator< It > const& other) const
        {
            return m_iter <= other.m_iter;
        }
        bool operator>(source_iterator< It > const& other) const
        {
            return m_iter > other.m_iter;
        }
        bool operator>=(source_iterator< It > const& other) const
        {
            return m_iter >= other.m_iter;
        }

        source_iterator< It >& operator++()
        {
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
        source_iterator< It > operator++(int)
        {
            source_iterator< It > copy = *this;
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
        std::string name() const
        {
            return *m_filename;
        }

        // TODO: figure out how to implement this efficiently
        /*
         *
        source_iterator<It>& operator--()
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
        return source_iterator<It>(at, filename, line, col);
    }

    template <typename C>
    inline auto make_source_iterator_pair(std::string const & fname, C&& container)
    {
        auto begin = std::begin(container);
        auto end = std::end(container);
        return std::make_pair(source_iterator<decltype(begin)>(begin, fname), source_iterator<decltype(begin)>(end));
    }
} // namespace rpnx::experimental

#endif // RPNXCORE_SOURCE_ITERATOR_HPP
