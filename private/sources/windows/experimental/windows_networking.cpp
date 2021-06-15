#ifdef _WIN32
// Above is just to help the IDEs find the right place to stick functions...
// we should not be compiling this file unless we are on Windows...

#include "rpnx/experimental/network.hpp"

#include <ioapiset.h>
#include <windows.h>

#include <map>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <shared_mutex>
#include <set>

namespace rpnx
{
    namespace experimental
    {
        inline namespace implementation
        {
            struct win32_async_service;

            struct iocp_handler
            {
                virtual ~iocp_handler() {};
                [[maybe_unused]] virtual void iocp_respond(win32_async_service& response) = 0;
            };

            struct ip6_acceptor_handler final
                : public iocp_handler
            {
                async_ip6_tcp_acceptor_ref m_socket;
                std::set<detail::async_ip6_tcp_autoacceptor_binding*> m_bindings;

                ip6_acceptor_handler(async_ip6_tcp_acceptor_ref which)
                : m_socket(which)
                {

                }

                virtual ~ip6_acceptor_handler();
                void iocp_respond(win32_async_service& srv) override;

                void attach(detail::async_ip6_tcp_autoacceptor_binding & binding, win32_async_service& srv);
                void detach(detail::async_ip6_tcp_autoacceptor_binding & binding, win32_async_service& srv);

                void activate_binding(detail::async_ip6_tcp_autoacceptor_binding & binding, win32_async_service& srv);
                void binding_wakeup(detail::async_ip6_tcp_autoacceptor_binding & binding, win32_async_service& srv);
                void deactivate_binding(detail::async_ip6_tcp_autoacceptor_binding & binding, win32_async_service& srv);
                //void submit_accept6(win32_async_service& srv, std::function<void(rpnx::experimental::result<async_ip6_tcp_connection>)>);
            };

            struct win32_async_service
            {
                friend class iocp_handler;

                std::vector<std::thread> m_threads;
                std::mutex m_mtx;
                std::condition_variable m_cond;
                std::atomic<bool> m_stop_signal;


                std::shared_mutex m_handlers_mutex;
                std::map<ULONG_PTR, std::unique_ptr<iocp_handler>> m_handlers;

                win32_async_service();
                ~win32_async_service();

                HANDLE m_iocp_handle;

                void submit(const async_ip4_udp_send_request&)
                {
                    // TODO
                }

                void bind_autoaccept6(detail::async_ip6_tcp_autoacceptor_binding & binding);
                void unbind_autoaccept6(detail::async_ip6_tcp_autoacceptor_binding & binding);

                void run_thread();
            };



            ip6_acceptor_handler::~ip6_acceptor_handler()
            {
            }

            void ip6_acceptor_handler::iocp_respond(win32_async_service& srv)
            {
            }

            enum class io_object_type
            {
                socket_acceptor6,

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

void rpnx::experimental::async_service::bind_autoaccept6(rpnx::experimental::detail::async_ip6_tcp_autoacceptor_binding& binding)
{
    reinterpret_cast< win32_async_service* >(m_pimpl)->bind_autoaccept6(binding);
}

void rpnx::experimental::async_service::unbind_autoaccept6(rpnx::experimental::detail::async_ip6_tcp_autoacceptor_binding& binding)
{
    reinterpret_cast< win32_async_service* >(m_pimpl)->unbind_autoaccept6(binding);
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
    m_iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, ULONG_PTR(0), 0);
    // TODO: Use proper error handling

    if (m_iocp_handle == INVALID_HANDLE_VALUE) throw std::runtime_error("failed");

    m_stop_signal.store(false, std::memory_order_relaxed);

    for (int i = 0; i < std::thread::hardware_concurrency()*2; i++)
    {
        m_threads.emplace_back([this]{ run_thread(); });
    }



}

rpnx::experimental::win32_async_service::~win32_async_service()
{

    m_stop_signal.store(true);

    for (int i = 0; i < m_threads.size(); i++)
    {
        PostQueuedCompletionStatus(m_iocp_handle, 0, 0, nullptr);
    }

    for (std::thread & th : m_threads)
    {
        th.join();
    }

    CloseHandle(m_iocp_handle);

}

void rpnx::experimental::win32_async_service::run_thread()
{
   // std::unique_lock m_lock(m_mtx);
    while (true)
    {
        DWORD count{};
        ULONG_PTR ptr{};
        OVERLAPPED* overlapped_ptr {};
        auto ok = GetQueuedCompletionStatus(m_iocp_handle, &count, &ptr, &overlapped_ptr, INFINITE);

        if (!ok)
        {
            std::cerr << "Err" << get_os_network_error_code().message() << std::endl;
            return;
        }

        if (ptr == 0)
        {
            if (m_stop_signal)
            {
                std::cerr << "STOP" << std::endl;
                return;
            }
        }

        std::shared_lock lock(m_handlers_mutex);
        m_handlers.at(ptr)->iocp_respond(*this);
    }
}
void rpnx::experimental::implementation::win32_async_service::bind_autoaccept6(rpnx::experimental::detail::async_ip6_tcp_autoacceptor_binding& binding)
{
    // We need to create a new handler if it doesn't exist
    std::unique_lock lock(m_handlers_mutex);
    ULONG_PTR ptr = static_cast<ULONG_PTR>(binding.m_socket.native());
    std::unique_ptr<iocp_handler> & cell = m_handlers[ptr];

    bool was_null = false;
    if (cell == nullptr)
    {
        cell = std::unique_ptr<iocp_handler>(new ip6_acceptor_handler(binding.m_socket));
        was_null = true;
    }
    ip6_acceptor_handler * handler = dynamic_cast<ip6_acceptor_handler*>(&*cell);
    // There should be no possibility that this value is of the wrong type apart from user error,
    // since we should delete this object from the map when all bindings are cleaned up.

    // If you hit this assert, then it means that you didn't destroy all binding objects (e.g. autoacceptor, autoreceiver, etc.)
    // before destroying some object (which isn't an acceptor6) and its handle was reused by the OS for a newly constructed object
    assert(handler != nullptr);
    [[maybe_unused]] auto err = ::CreateIoCompletionPort((HANDLE) binding.m_socket.native(), m_iocp_handle, binding.m_socket.native(), 0);
    [[maybe_unused]] auto err2 = GetLastError();
    handler->attach(binding, *this);
}

void rpnx::experimental::implementation::win32_async_service::unbind_autoaccept6(rpnx::experimental::detail::async_ip6_tcp_autoacceptor_binding& binding)
{
    std::unique_lock lock(m_handlers_mutex);
    ULONG_PTR ptr = static_cast<ULONG_PTR>(binding.m_socket.native());
    std::unique_ptr<iocp_handler> & cell = m_handlers[ptr];

    bool was_null = false;
    if (cell == nullptr)
    {
        was_null = true;
    }

    assert(was_null == false);

    ip6_acceptor_handler * handler = dynamic_cast<ip6_acceptor_handler*>(&*cell);

    assert(handler != nullptr);

    handler->detach(binding, *this);

}

void rpnx::experimental::implementation::ip6_acceptor_handler::attach(detail::async_ip6_tcp_autoacceptor_binding& binding, win32_async_service & svc)
{
    m_bindings.insert(&binding);

    activate_binding(binding, svc);
}


void rpnx::experimental::implementation::ip6_acceptor_handler::detach(detail::async_ip6_tcp_autoacceptor_binding& binding, win32_async_service & svc6)
{


}
void rpnx::experimental::implementation::ip6_acceptor_handler::activate_binding(rpnx::experimental::detail::async_ip6_tcp_autoacceptor_binding& binding, rpnx::experimental::win32_async_service& srv)
{
    binding.m_overlapped = {};
    binding.m_accept_on_socket = WSASocketW( AF_INET6, SOCK_STREAM, IPPROTO_TCP,
                                             nullptr, 0, WSA_FLAG_OVERLAPPED);

}
#endif
