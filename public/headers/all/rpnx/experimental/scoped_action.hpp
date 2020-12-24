//
// Created by Ryan Nicholl on 12/23/20.
//

#ifndef RPNXCORE_SCOPED_ACTION_HPP
#define RPNXCORE_SCOPED_ACTION_HPP

namespace rpnx
{
    namespace experimental
    {
        template <typename T>
        class scoped_increment
        {
            T & m_val;

          public:
            scoped_increment(T & t)
            :m_val(t)
            {
                m_val++;
            }

            ~scoped_increment()
            {
                m_val--;
            }
        };

        template <typename T, typename L>
        class scoped_increment_with_lock
        {
            T & m_val;
            L & m_lock;

          public:
            scoped_increment_with_lock(T & t, L & l)
                :m_val(t), m_lock(l)
            {
                if (m_lock.owns_lock())
                {
                    m_val++;
                }
                else
                {
                    m_lock.lock();
                    m_val++;
                    m_lock.unlock();
                }
            }

            ~scoped_increment_with_lock()
            {
                if (m_lock.owns_lock())
                {
                    m_val--;
                }
                else
                {
                    try
                    {
                        m_lock.lock();
                        m_val--;
                        m_lock.unlock();
                    }
                    catch (...)
                    {
                         // No way to scoped increment guarantee if mutex throws exception, so abort the program
                         // This should never happen under normal conditions
                         // Better to abort than introduce a race condition.
                         std::terminate();
                    }
                }
            }
        };
    }

}

#endif // RPNXCORE_SCOPED_ACTION_HPP
