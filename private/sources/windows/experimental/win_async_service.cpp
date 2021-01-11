#ifdef _WIN32
// Above is just to help the IDEs find the right place to stick functions...
// we should not be compiling this file unless we are on Windows...
#include "rpnx/experimental/network.hpp"


namespace implementation
{
    class win32_async_service
    {

    };
    struct pending_operation
    {
        WSABUF buf;
    };
}

void rpnx::experimental::async_service::submit(const async_ip4_udp_send_request& req)
{

}

void rpnx::experimental::async_service::cancel_all()
{

}

#endif
