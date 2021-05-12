#include "rpnx/experimental/network.hpp"

#include <sys/epoll.h>

namespace rpnx
{
    namespace experimental
    {
        inline namespace implementation
        {
            struct epoll_async_service
            {

                epoll_async_service();
                ~epoll_async_service();

                int m_epoll_fd;

                void submit(const async_ip4_udp_send_request&)
                {
                    // TODO
                }
            };


        } // namespace implementation
    }     // namespace experimental
} // namespace rpnx

static inline rpnx::experimental::epoll_async_service* impl(void* ptr)
{

    return reinterpret_cast< rpnx::experimental::epoll_async_service* >(ptr);
}

rpnx::experimental::async_service::async_service() : m_pimpl(nullptr)
{
    m_pimpl = reinterpret_cast< void* >(new epoll_async_service());
}

rpnx::experimental::async_service::~async_service()
{
    delete reinterpret_cast< epoll_async_service* >(m_pimpl);
}
/*
void rpnx::experimental::async_service::submit(const async_ip4_udp_send_request& req)
{
    // TODO
   // impl(m_pimpl)->submit(req);
}
*/

/*
void rpnx::experimental::async_service::cancel_all()
{
    // TODO
}
 */

rpnx::experimental::epoll_async_service::epoll_async_service()
{
    // TODO
}

rpnx::experimental::epoll_async_service::~epoll_async_service()
{
    ::close(m_epoll_fd);
    // TODO
}
