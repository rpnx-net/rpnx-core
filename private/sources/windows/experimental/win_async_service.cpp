#ifdef _WIN32
// Above is just to help the IDEs find the right place to stick functions...
// we should not be compiling this file unless we are on Windows...

#include "rpnx/experimental/network.hpp"

#include <windows.h>

namespace rpnx
{
    namespace experimental
    {
        inline namespace implementation
        {
            struct win32_async_service
            {

                win32_async_service();
                ~win32_async_service();

                HANDLE m_iocp_handle;

                void submit(const async_ip4_udp_send_request&)
                {
                    // TODO
                }
            };

            struct pending_operation
            {
                WSABUF buf;
            };
        } // namespace implementation
    }     // namespace experimental
} // namespace rpnx

static inline rpnx::experimental::win32_async_service* impl(void* ptr)
{

    return reinterpret_cast< rpnx::experimental::win32_async_service* >(ptr);
}

rpnx::experimental::async_service::async_service() : m_pimpl(nullptr)
{
    m_pimpl = reinterpret_cast< void* >(new win32_async_service());
}

rpnx::experimental::async_service::~async_service()
{
    delete reinterpret_cast< win32_async_service* >(m_pimpl);
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

rpnx::experimental::win32_async_service::win32_async_service()
{
    // TODO
    m_iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, ULONG_PTR(0), 0);
}

rpnx::experimental::win32_async_service::~win32_async_service()
{
    CloseHandle(m_iocp_handle);
    // TODO
}

#endif
