#ifndef RPNXCORE_NETWORK_HPP
#define RPNXCORE_NETWORK_HPP

#include <future>

#ifdef _WIN32
#include <WinSock2.h>
#endif

#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)

#if defined(__FreeBSD__)
#include <sys/types.h>
#endif

#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <unistd.h>

#endif

#if defined(__FreeBSD__)

#include <netinet/in.h>
#include <sys/socket.h>
#endif

#if defined(__linux__)
#include <sys/epoll.h>
#endif

#include <array>
#include <assert.h>
#include <climits>
#include <functional>
#include <iostream>
#include <system_error>
#include <vector>

#include "rpnx/network_error.hpp"
#include <set>

namespace rpnx
{
    namespace experimental
    {
#ifdef _WIN32
        namespace detail
        {
            class wsa_intializer
            {
              private:
                WSADATA wsaData;

              public:
                wsa_intializer(int a, int b)
                {
                    auto result = WSAStartup(MAKEWORD(2, 2), &wsaData);
                    if (result != 0)
                        throw std::system_error(std::error_code(result, std::system_category()), "WSA intialization failed");
                }
                ~wsa_intializer()
                {
                    WSACleanup();
                }

                static wsa_intializer& singleton()
                {
                    static wsa_intializer wsa(2, 2);
                    return wsa;
                }
            };
        } // namespace detail
        inline std::error_code get_os_network_error_code()
        {
            int er = WSAGetLastError();
            int erx = 1;

            switch (er)

            {
            case WSA_NOT_ENOUGH_MEMORY:
                return std::make_error_code(std::errc::not_enough_memory);
            case WSA_INVALID_PARAMETER:
                return std::make_error_code(std::errc::invalid_argument);
            case WSA_OPERATION_ABORTED:
                return std::make_error_code(std::errc::operation_canceled);
            case WSA_IO_INCOMPLETE:
                return std::make_error_code(std::errc::operation_in_progress);
            case WSA_IO_PENDING:
                return std::make_error_code(std::errc::operation_in_progress);
            case WSAEINTR:
                return std::make_error_code(std::errc::interrupted);
            case WSAEBADF:
                return std::make_error_code(std::errc::bad_file_descriptor);
            case WSAEACCES:
                return std::make_error_code(std::errc::permission_denied);
            case WSAEFAULT:
                return std::make_error_code(std::errc::bad_address);
            case WSAEMFILE:
                return std::make_error_code(std::errc::too_many_files_open);
            case WSAEWOULDBLOCK:
                return std::make_error_code(std::errc::operation_would_block);
            case WSAEINPROGRESS:
                return std::make_error_code(std::errc::operation_in_progress);
            case WSAEALREADY:
                return std::make_error_code(std::errc::operation_in_progress);
            case WSAEMSGSIZE:
                return std::make_error_code(std::errc::message_size);
            case WSAEPFNOSUPPORT:
                return std::make_error_code(std::errc::protocol_not_supported);
            case WSAEADDRINUSE:
                return std::make_error_code(std::errc::address_in_use);
            case WSAEADDRNOTAVAIL:
                return std::make_error_code(std::errc::address_not_available);
            case WSAENETDOWN:
                return std::make_error_code(std::errc::network_down);
            case WSAENETUNREACH:
                return std::make_error_code(std::errc::network_unreachable);
            case WSAENETRESET:
                return std::make_error_code(std::errc::network_reset);
                // case WSAEACCES: return std::make_error_code(std::errc::permission_denied);
                // case WSAEACCES: return std::make_error_code(std::errc::permission_denied);
                //            return std::make_error_code(network_errc::permission_denied);
            }

            return std::error_code(er, std::system_category());
        }
        using native_socket_type = SOCKET;
#endif

#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
        inline std::error_code get_os_network_error_code()
        {
            return std::error_code(errno, std::system_category());
        }
        using native_socket_type = int;
#endif



        class ip4_udp_socket;
        class ip4_udp_endpoint;
        class ip4_tcp_acceptor;
        class ip4_tcp_connection;
        class ip4_tcp_endpoint;

        class ip6_udp_socket;
        class ip6_udp_endpoint;
        class ip6_tcp_acceptor;
        class ip6_tcp_connection;
        class ip6_tcp_endpoint;

        class async_service;

        class async_ip4_udp_socket;

        template < typename It >
        void net_send(ip4_udp_socket& socket, ip4_udp_endpoint const& to, It input_begin, It input_end);

        template < typename It >
        void net_send(ip4_tcp_connection& socket, It input_begin, It input_end);

        template < typename It >
        auto net_receive(ip4_udp_socket& socket, ip4_udp_endpoint& from, It output_begin, It output_bounds_check) -> It;
        template < typename It >
        auto net_receive(ip4_udp_socket& socket, ip4_udp_endpoint& from, It output_begin) -> It;
        template < typename It >
        auto net_receive(ip4_tcp_connection& socket, It output_begin, It output_end) -> void;
        template < typename It >
        auto net_receive(ip4_tcp_connection& socket, size_t count, It output) -> void;


        void net_bind(ip4_udp_socket& socket, ip4_udp_endpoint const& bind);
        void net_bind(async_ip4_udp_socket& socket, ip4_udp_endpoint const& bind);

        void net_listen(ip4_tcp_acceptor& socket, ip4_tcp_endpoint const& addr);

        ip4_udp_endpoint net_endpoint(ip4_udp_socket& socket);
        ip4_udp_endpoint net_endpoint(async_ip4_udp_socket& socket);

        ip4_tcp_connection net_accept_connection(ip4_tcp_acceptor&);
        void net_accept_connection(ip4_tcp_acceptor& socket, ip4_tcp_connection& connection);



        class ip4_address final
        {
          private:
            using data_type = std::array< std::uint8_t, 4 >;
            data_type m_addr;

          public:
            ip4_address() noexcept : m_addr{0, 0, 0, 0}
            {
            }

            ip4_address(std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d) noexcept : m_addr{a, b, c, d}
            {
            }

            explicit ip4_address(in_addr const& addr)
            {
#ifdef _WIN32
                m_addr[0] = addr.S_un.S_un_b.s_b1;
                m_addr[1] = addr.S_un.S_un_b.s_b2;
                m_addr[2] = addr.S_un.S_un_b.s_b3;
                m_addr[3] = addr.S_un.S_un_b.s_b4;
#elif defined(__linux__) || defined(__APPLE__)
                m_addr[0] = reinterpret_cast< char const* >(&addr.s_addr)[0];
                m_addr[1] = reinterpret_cast< char const* >(&addr.s_addr)[1];
                m_addr[2] = reinterpret_cast< char const* >(&addr.s_addr)[2];
                m_addr[3] = reinterpret_cast< char const* >(&addr.s_addr)[3];
#endif
            }

            inline in_addr native() const noexcept
            {

#ifdef _WIN32
                in_addr addr;
                addr.S_un.S_un_b.s_b1 = m_addr[0];
                addr.S_un.S_un_b.s_b2 = m_addr[1];
                addr.S_un.S_un_b.s_b3 = m_addr[2];
                addr.S_un.S_un_b.s_b4 = m_addr[3];
                return addr;
#endif
#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
                return {static_cast< in_addr_t >(m_addr[0] | (m_addr[1] << 8) | (m_addr[3] << 16) | (m_addr[3] << 24))};
#endif
            }

            using value_type = data_type::value_type;
            using iterator = data_type::iterator;
            using const_iterator = data_type::const_iterator;
            using pointer = data_type::pointer;
            using const_pointer = data_type::const_pointer;
            using reference = data_type::reference;
            using const_reference = data_type::const_reference;

            inline value_type& operator[](int n)
            {
                return m_addr[n];
            }

            inline value_type const& operator[](int n) const
            {
                return m_addr[n];
            }

            iterator begin()
            {
                return std::begin(m_addr);
            }
            const_iterator begin() const
            {
                return std::begin(m_addr);
            }

            iterator end()
            {
                return std::end(m_addr);
            }
            const_iterator end() const
            {
                return std::end(m_addr);
            }

            static ip4_address any()
            {
                ip4_address addr;
                addr[0] = 0;
                addr[1] = 0;
                addr[2] = 0;
                addr[3] = 0;
                return addr;
            }
        };

        class ip4_udp_endpoint;
        class ip4_tcp_endpoint
        {
            ip4_address m_addr;
            uint16_t m_port;

          public:
            inline ip4_tcp_endpoint() : m_addr(), m_port(0)
            {
            }
            inline ip4_tcp_endpoint(ip4_address addr, uint16_t port) : m_addr(addr), m_port(port)
            {
            }

            inline explicit ip4_tcp_endpoint(sockaddr_in const& in)
            {
                assert(in.sin_family == AF_INET);
                m_port = ntohs(in.sin_port);
                m_addr = ip4_address(in.sin_addr);
            }

            sockaddr_in native() const
            {
                sockaddr_in addr;
                addr.sin_family = AF_INET;
                addr.sin_port = htons(m_port);
                addr.sin_addr = m_addr.native();
                return addr;
            }

            ip4_address& address()
            {
                return m_addr;
            }
            uint16_t& port()
            {
                return m_port;
            }

            ip4_address const& address() const
            {
                return m_addr;
            }
            uint16_t const& port() const
            {
                return m_port;
            }
        };

        class ip4_udp_endpoint
        {
            ip4_address m_addr;
            uint16_t m_port;

          public:
            inline ip4_udp_endpoint() : m_port(0), m_addr()
            {
            }
            inline ip4_udp_endpoint(sockaddr_in const& native_addr) : ip4_udp_endpoint()
            {
                m_port = ntohs(native_addr.sin_port);
#ifdef _WIN32
                m_addr[0] = native_addr.sin_addr.S_un.S_un_b.s_b1;
                m_addr[1] = native_addr.sin_addr.S_un.S_un_b.s_b2;
                m_addr[2] = native_addr.sin_addr.S_un.S_un_b.s_b3;
                m_addr[3] = native_addr.sin_addr.S_un.S_un_b.s_b4;
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
                m_addr[0] = native_addr.sin_addr.s_addr >> 24;
                m_addr[1] = native_addr.sin_addr.s_addr >> 16;
                m_addr[2] = native_addr.sin_addr.s_addr >> 8;
                m_addr[3] = native_addr.sin_addr.s_addr >> 0;
#else
#error Not implemented
#endif
            }

            inline ip4_udp_endpoint(ip4_address addr, uint16_t port) : m_addr(addr), m_port(port)
            {
            }

            inline ip4_udp_endpoint(ip4_udp_endpoint const&) = default;

            explicit inline ip4_udp_endpoint(ip4_tcp_endpoint const& other) : m_addr(other.address()), m_port(other.port())
            {
            }

            sockaddr_in native() const
            {
                sockaddr_in addr;
                addr.sin_family = AF_INET;
                addr.sin_port = htons(m_port);
                addr.sin_addr = m_addr.native();
                return addr;
            }

            ip4_address& address()
            {
                return m_addr;
            }
            uint16_t& port()
            {
                return m_port;
            }

            ip4_address const& address() const
            {
                return m_addr;
            }
            uint16_t const& port() const
            {
                return m_port;
            }
        };

        class ip4_udp_socket
        {
          public:
            native_socket_type m_s;

          public:
            ip4_udp_socket() noexcept
            {
#ifdef _WIN32
                m_s = INVALID_SOCKET;
#else
                m_s = -1;
#endif
            }

            ip4_udp_socket(ip4_udp_socket const&) = delete;

            inline ip4_udp_socket(ip4_udp_socket&& other) noexcept : ip4_udp_socket()
            {
                std::swap(m_s, other.m_s);
            }

            ~ip4_udp_socket()
            {
                close();
            }

            template < typename It >
            auto receive(ip4_udp_endpoint& from, It out_iterator) -> It
            {
                return net_receive(*this, from, out_iterator);
            }

            template < typename It >
            auto receive(ip4_udp_endpoint& from, It out_iterator, It out_iterator_bounds) -> It
            {
                return net_receive(*this, from, out_iterator, out_iterator_bounds);
            }

            template < typename It >
            void send(ip4_udp_endpoint const& destination, It begin, It end)
            {
                net_send(*this, destination, begin, end);
            }

            void bind(ip4_udp_endpoint ep)
            {
                net_bind(*this, ep);
            }

            void bind(ip4_address addr)
            {
                ip4_udp_endpoint ep;
                ep.address() = addr;
                ep.port() = 0;
                net_bind(*this, ep);
            }

            void bind(uint16_t portno)
            {
                ip4_udp_endpoint ep;
                ep.address() = ip4_address::any();
                ep.port() = portno;
                net_bind(*this, ep);
            }

            void bind()
            {
                ip4_udp_endpoint ep;
                ep.address() = ip4_address::any();
                ep.port() = 0;
                net_bind(*this, ep);
            }

            ip4_udp_endpoint endpoint()
            {
#ifdef _WIN32
                detail::wsa_intializer::singleton();
                sockaddr_in addr;
                int len = sizeof(addr);

                auto result = ::getsockname(m_s, (sockaddr*)&addr, &len);

                if (result == SOCKET_ERROR)
                {
                    throw network_error("upd_ip4_socket::endpoint()", get_os_network_error_code());
                }
                assert(len == sizeof(addr));
                return addr;
#else
                sockaddr_in addr;
                socklen_t len = sizeof(addr);

                auto result = ::getsockname(m_s, (sockaddr*)&addr, &len);

                if (result == -1)
                {
                    throw network_error("upd_ip4_socket::endpoint()", get_os_network_error_code());
                }
                assert(len == sizeof(addr));
                return addr;
#endif
            }

            void close()
            {
#ifdef _WIN32
                detail::wsa_intializer::singleton();
                if (m_s != INVALID_SOCKET)
                {
                    closesocket(m_s);
                    m_s = INVALID_SOCKET;
                }
#else

                if (m_s != -1)
                {
                    ::close(m_s);
                    m_s = -1;
                }
#endif
            }

            void open()
            {
#ifdef _WIN32
                detail::wsa_intializer::singleton();
                close();
                m_s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                if (m_s == INVALID_SOCKET)
                    throw network_error("udp_ip4_socket::open()", get_os_network_error_code());
#else
                this->close();
                m_s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                if (m_s == -1)
                    throw network_error("udp_ip4_socket::open()", get_os_network_error_code());
#endif
            }

            auto native() const noexcept
            {
                return m_s;
            }
        };

        class ip4_tcp_acceptor

        {

          private:
            native_socket_type m_sock;

          public:
            ip4_tcp_acceptor() noexcept : m_sock(-1)
            {
            }

            ~ip4_tcp_acceptor()
            {
                close();
            }

            ip4_tcp_acceptor(ip4_tcp_acceptor const&) = delete;

            ip4_tcp_acceptor(ip4_tcp_acceptor&& other) noexcept : m_sock(-1)
            {
                std::swap(m_sock, other.m_sock);
            }

            void close() noexcept
            {
#ifdef _WIN32
                // detail::wsa_intializer::singleton();
                if (m_sock != -1)
                {
                    ::closesocket(m_sock);
                    m_sock = -1;
                }
#else
                if (m_sock != -1)
                {
                    ::close(m_sock);
                    m_sock = -1;
                }
#endif
            }

            void open()
            {
#ifdef _WIN32
                detail::wsa_intializer::singleton();
                close();
                m_sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                if (m_sock == INVALID_SOCKET)
                    throw network_error("tcp_ip4_socket::open()+::socket", get_os_network_error_code());
#else
                close();
                m_sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                if (m_sock == -1)
                    throw network_error("tcp_ip4_socket::open()", get_os_network_error_code());
#endif
            }

            void listen(ip4_tcp_endpoint const& listen_addr)
            {
#ifdef _WIN32
                detail::wsa_intializer::singleton();
                open();
                ::sockaddr_in sock_addr = listen_addr.native();
                if (::bind(m_sock, (sockaddr*)&sock_addr, sizeof(sockaddr_in)) == SOCKET_ERROR)
                {
                    throw network_error("tcp_ip4_socket::open()+::bind", get_os_network_error_code());
                }
                if (::listen(m_sock, SOMAXCONN) == SOCKET_ERROR)
                {
                    throw network_error("tcp_ip4_socket::open()+::listen", get_os_network_error_code());
                }
#else
                this->open();
                ::sockaddr_in sock_addr = listen_addr.native();
                ::bind(m_sock, (sockaddr*)&sock_addr, sizeof(sockaddr_in));
                if (::listen(m_sock, SOMAXCONN) == -1)
                {
                    throw network_error("tcp_ip4_socket::open()", get_os_network_error_code());
                }
#endif
            }

#ifdef _WIN32
            [[nodiscard]] SOCKET native() const noexcept
            {
                return m_sock;
            }
#else
            int native() const noexcept
            {
                return m_sock;
            }
#endif
        };

        class ip4_tcp_connection
        {
          private:
#ifdef _WIN32
            SOCKET m_socket;
#else
            int m_socket;
#endif
          public:
            ip4_tcp_connection() noexcept : m_socket(-1)
            {
            }

#ifndef _WIN32
            ip4_tcp_connection(ip4_tcp_endpoint const& ep)
            {
                m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                if (m_socket == -1)
                {
                    throw network_error("ip4_tcp_connection::ip4_tcp_connection", get_os_network_error_code());
                }

                sockaddr_in addr = ep.native();
                int c = ::connect(m_socket, (sockaddr const*)&addr, sizeof(addr));
                if (m_socket == -1)
                {
                    throw network_error("ip4_tcp_connection::ip4_tcp_connection", get_os_network_error_code());
                }
            }
#endif
            // TODO: Implement on windows

            ip4_tcp_connection(ip4_tcp_connection const&) = delete;

            ip4_tcp_connection(ip4_tcp_connection&& other) noexcept : m_socket(-1)
            {
                std::swap(other.m_socket, m_socket);
            }

            /** Destructor of the socket. Either calls abort() or close() (it is unspecified which is called)
             */
            ~ip4_tcp_connection()
            {
                close(); // TODO: Should be 'abort' by default
                // abort isn't yet implemented.
            }

            /**
              Create a socket from a native SOCKET object.
              Ownership of the SOCKET is assumed by this class.
            */
            explicit inline ip4_tcp_connection(native_socket_type s) noexcept : m_socket(s)
            {
            }

            inline ip4_tcp_connection& operator=(ip4_tcp_connection&& other) noexcept
            {
                std::swap(other.m_socket, m_socket);
                return *this;
            }

            ip4_tcp_endpoint endpoint()
            {
                sockaddr_in saddr;
#ifdef _WIN32
                int sz = sizeof(saddr);
#else
                socklen_t sz = sizeof(saddr);
#endif
                if (getsockname(m_socket, (sockaddr*)&saddr, &sz) != 0)
                {
                    throw network_error("ip4_tcp_connection::endpoint", get_os_network_error_code());
                }

                return ip4_tcp_endpoint(saddr);
            }

            ip4_tcp_endpoint peer_endpoint()
            {

                sockaddr_in saddr;
#ifdef _WIN32
                int sz = sizeof(saddr);
#else
                socklen_t sz = sizeof(saddr);
#endif
                if (getpeername(m_socket, (sockaddr*)&saddr, &sz) != 0)
                {
                    throw network_error("peer_endpoint", get_os_network_error_code());
                }

                return ip4_tcp_endpoint(saddr);
            }

            /**
              Returns a copy of the internal SOCKET. This class retains ownership of the SOCKET.
            */
            [[nodiscard]] native_socket_type native() const noexcept
            {
                return m_socket;
            }

            /**
             Extracts the internal SOCKET. The function relinquishes ownership of the socket and the class's internal socket becomes INVALID_SOCKET.
            */
            [[nodiscard]] native_socket_type extract_native() noexcept
            {
                native_socket_type skt = m_socket;
                m_socket = -1;
                return skt;
            }

            [[nodiscard]] bool valid() const noexcept
            {
                return m_socket != -1;
            }

            template < typename It >

            void send(It begin, It end)
            {
                net_send(*this, begin, end);
            }

            /* TODO
            void abort()
            {

            }*/

            void close()
            {
                if (m_socket != -1)
                {
#ifdef _WIN32
                    shutdown(m_socket, SD_BOTH);
                    closesocket(m_socket);
#else
                    shutdown(m_socket, SHUT_RDWR);
                    ::close(m_socket);
#endif
                    m_socket = -1;
                }
            }
        };

        template < typename It >
        void net_send(ip4_udp_socket& socket, ip4_udp_endpoint const& to, It begin_packet, It end_packet)
        {
#ifdef _WIN32
            detail::wsa_intializer::singleton();
            sockaddr_in dest = to.native();
            std::vector< char > data;
            for (auto it = begin_packet; it != end_packet; ++it)
            {
                data.emplace_back((char)*it);
            }

            auto result = sendto(socket.native(), (char const*)data.data(), data.size(), 0, (sockaddr*)&dest, sizeof(dest));
            if (result == SOCKET_ERROR)
            {
                throw network_error("upd_ip4_socket::send()", get_os_network_error_code());
            }
#else
            sockaddr_in dest = to.native();
            std::vector< std::byte > data(begin_packet, end_packet);

            auto result = sendto(socket.native(), (char const*)data.data(), data.size(), 0, (sockaddr*)&dest, sizeof(dest));
            if (result == -1)
            {
                throw network_error("upd_ip4_socket::send()", get_os_network_error_code());
            }
            return;
#endif
        }

        template < typename It >
        void net_send(ip4_tcp_connection& socket, It begin_data, It end_data)
        {
#ifdef _WIN32
            detail::wsa_intializer::singleton();

            std::vector< char > data;
            data.assign(begin_data, end_data);

            // TODO: Right now we don't support sending more than 2^32 bytes at once on windows
            // Ideally we should fix this
            if (data.size() >= INT_MAX)
                throw std::invalid_argument("send too large");

            std::size_t st = 0;

            while (st != data.size())
            {
                auto count = ::send(socket.native(), (const char*)data.data() + st, data.size() - st, 0);

                if (count != SOCKET_ERROR)
                {
                    st += count;
                }
                else
                {
                    throw network_error("rpnx::net_send", get_os_network_error_code());
                }
            }

#else

            std::vector< char > data(begin_data, end_data);

            std::size_t st = 0;

            while (st != data.size())
            {
                auto count = ::send(socket.native(), (const char*)data.data() + st, data.size() - st, 0);

                if (count != -1)
                {
                    st += count;
                }
                else
                {
                    throw network_error("rpnx::net_send", get_os_network_error_code());
                }
            }

#endif
        }

        template < typename It >
        auto net_receive(ip4_udp_socket& socket, ip4_udp_endpoint& fr, It begin_packet, It end_packet) -> It
        {
#ifdef _WIN32
            detail::wsa_intializer::singleton();
#endif
            std::array< std::byte, 256 * 256 > buffer;
            sockaddr_in from;
            static_assert(sizeof(from) < INT_MAX);
#ifdef _WIN32
            int fromlen = sizeof(from);
#else
            socklen_t fromlen = sizeof(from);
#endif
            auto count = recvfrom(socket.native(), reinterpret_cast< char* >(buffer.data()), (int)buffer.size(), 0, reinterpret_cast< sockaddr* >(&from), &fromlen);
            if (count == -1)
            {
                throw network_error("upd_ip4_socket::receive()", get_os_network_error_code());
            }

            It it = begin_packet;
            auto buf_it = buffer.begin();
            auto buf_end = buffer.begin() + count;
            while (it != end_packet && buf_it != buf_end)
            {
                *it = *buf_it;
                ++it;
                ++buf_it;
            }

            if (it == end_packet && buf_it != buf_end)
            {
                throw std::out_of_range("Iterator out of range in rpnx::net_receive");
            }

            fr = ip4_udp_endpoint(from);

            return it;
        }

        template < typename It >
        auto net_receive(ip4_udp_socket& socket, ip4_udp_endpoint& fr, It begin_packet) -> It
        {
#ifdef _WIN32
            detail::wsa_intializer::singleton();
#endif
            std::array< char, 256 * 256 > buffer;
            sockaddr_in from{};
            static_assert(sizeof(from) < INT_MAX);

#ifdef _WIN32
            int from_len = sizeof(from);
#else
            socklen_t from_len = sizeof(from);
#endif

            auto count = recvfrom(socket.native(), reinterpret_cast< char* >(buffer.data()), (int)buffer.size(), 0, reinterpret_cast< sockaddr* >(&from), &from_len);
            if (count == -1)
            {
                throw network_error("upd_ip4_socket::receive()", get_os_network_error_code());
            }

            It it = begin_packet;
            auto buf_it = buffer.begin();
            auto buf_end = buffer.begin() + count;
            while (buf_it != buf_end)
            {
                *it = std::byte(*buf_it);
                ++it;
                ++buf_it;
            }
            fr = ip4_udp_endpoint(from);
            return it;
        }

        inline void net_bind(ip4_udp_socket& socket, ip4_udp_endpoint const& bind_addr)
        {
#ifdef _WIN32
            detail::wsa_intializer::singleton();
#endif
            sockaddr_in socket_addr = bind_addr.native();
            auto result = ::bind(socket.native(), (const sockaddr*)&socket_addr, sizeof(socket_addr));
            if (result == -1)
            {
                throw network_error("upd_ip4_socket::bind()", get_os_network_error_code());
            }
            return;
        }

        inline void net_accept_connection(ip4_tcp_acceptor& socket, ip4_tcp_connection& connection)
        {
            connection = net_accept_connection(socket);
        }

        inline ip4_tcp_connection net_accept_connection(ip4_tcp_acceptor& socket)
        {
#ifdef _WIN32
            ip4_tcp_connection con = ip4_tcp_connection(::accept(socket.native(), nullptr, nullptr));
            if (con.native() == INVALID_SOCKET)
            {
                throw network_error("rpnx::net_accept_connection", get_os_network_error_code());
            }

            return std::move(con);
#else
            ip4_tcp_connection con = ip4_tcp_connection(::accept(socket.native(), nullptr, nullptr));
            if (con.native() == -1)
            {
                throw network_error("rpnx::net_accept_connection", get_os_network_error_code());
            }

            return std::move(con);
#endif
        }

        struct async_ip4_udp_send_request
        {
            ip4_udp_socket* socket;
            void const* data;
            std::size_t data_len;
            std::function< void() > callback;
        };

        class async_ip4_udp_socket
        {
            friend void net_bind(async_ip4_udp_socket &, ip4_udp_endpoint const &);
          private:
            native_socket_type m_socket;

          public:

            auto endpoint()
            {
                return net_endpoint(*this);
            }

#ifndef _WIN32
            bool is_open() const { return m_socket != -1; }
            void open()
            {
                m_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                if (m_socket == -1)
                {
                    throw std::runtime_error("SOCKET");
                }
            }

            [[nodiscard]] native_socket_type native() const
            {
                return m_socket;
            }
#endif


#ifdef _WIN32
            async_ip4_udp_socket()
                : m_socket(INVALID_SOCKET)
            {


            }
            bool is_open() const
            {
                return m_socket != -1;
            }

            void close()
            {
                if (is_open()) return;

                ::closesocket(m_socket);

                m_socket = -1;
            }

            [[nodiscard]] auto native() const
            {
                return m_socket;
            }

            void open()
            {
                if (is_open()) close();
                m_socket = WSASocketW(AF_INET, SOCK_DGRAM, IPPROTO_UDP, nullptr, 0, WSA_FLAG_OVERLAPPED);
                if (m_socket == -1)
                {
                    throw std::runtime_error("WSA SOCKET");
                }
            }

            void bind(ip4_udp_endpoint ep)
            {
                net_bind(*this, ep);
            }

            void bind(ip4_address addr)
            {
                ip4_udp_endpoint ep;
                ep.address() = addr;
                ep.port() = 0;
                net_bind(*this, ep);
            }

            void bind(uint16_t portno)
            {
                ip4_udp_endpoint ep;
                ep.address() = ip4_address::any();
                ep.port() = portno;
                net_bind(*this, ep);
            }

            void bind()
            {
                ip4_udp_endpoint ep;
                ep.address() = ip4_address::any();
                ep.port() = 0;
                net_bind(*this, ep);
            }


#endif
        };

        class async_service
        {
          private:
            void * m_pimpl;

#ifdef _WIN32
          private:
            struct pending_operation
            {
                WSABUF buf;
            };

          public:
            async_service();
            ~async_service();

            void submit(async_ip4_udp_send_request const& req);
            void cancel_all();

#elif false
          private:
            enum class op_type
            {
                read,
                write
            };
            struct pending_operation
            {
                int fd;
                void* data;
                std::size_t data_len;
                op_type operation;
            };

            int m_epoll_instance;
            std::set< pending_operation* > m_registered_pending_operations;

          public:
            async_service()
            {
                m_epoll_instance = ::epoll_create(0);
                if (m_epoll_instance == -1)
                    throw std::runtime_error("Failed to create epoll instance");
            }

            void submit(async_ip4_udp_send_request const& req)
            {
            }
#endif
        };

        inline void net_bind(async_ip4_udp_socket& socket, ip4_udp_endpoint const& bind_addr)
        {
#ifdef _WIN32
            detail::wsa_intializer::singleton();
#endif
            sockaddr_in socket_addr = bind_addr.native();
            auto result = ::bind(socket.native(), (const sockaddr*)&socket_addr, sizeof(socket_addr));
            if (result == -1)
            {
                throw network_error("upd_ip4_socket::bind()", get_os_network_error_code());
            }
            return;
        }

        // "asynchronously" binds a socket.
        // Note: Synchronous sockets that are bound asynchronously, actually bind synchronously in a different thread.
        inline std::future<void> net_bind_async(ip4_udp_socket& socket, ip4_udp_endpoint const& bind_addr)
        {
            return std::async([&socket, bind_addr] {
#ifdef _WIN32
               detail::wsa_intializer::singleton();
#endif
               sockaddr_in socket_addr = bind_addr.native();
               auto result = ::bind(socket.native(), (const sockaddr*)&socket_addr, sizeof(socket_addr));
               if (result == -1)
               {
                   throw network_error("upd_ip4_socket::bind()", get_os_network_error_code());
               }
               return;
            });
        }

        inline ip4_udp_endpoint net_endpoint(ip4_udp_socket& socket)
        {
#ifdef _WIN32
            detail::wsa_intializer::singleton();
            sockaddr_in addr;
            int len = sizeof(addr);

            auto result = ::getsockname(socket.native(), (sockaddr*)&addr, &len);

            if (result == SOCKET_ERROR)
            {
                throw network_error("upd_ip4_socket::endpoint()", get_os_network_error_code());
            }
            assert(len == sizeof(addr));
            return addr;
#else
            sockaddr_in addr;
                socklen_t len = sizeof(addr);

                auto result = ::getsockname(socket.native(), (sockaddr*)&addr, &len);

                if (result == -1)
                {
                    throw network_error("upd_ip4_socket::endpoint()", get_os_network_error_code());
                }
                assert(len == sizeof(addr));
                return addr;
#endif
        }
        inline ip4_udp_endpoint net_endpoint(async_ip4_udp_socket& socket)
        {
#ifdef _WIN32
            detail::wsa_intializer::singleton();
            sockaddr_in addr;
            int len = sizeof(addr);

            auto result = ::getsockname(socket.native(), (sockaddr*)&addr, &len);

            if (result == SOCKET_ERROR)
            {
                throw network_error("upd_ip4_socket::endpoint()", get_os_network_error_code());
            }
            assert(len == sizeof(addr));
            return addr;
#else
            sockaddr_in addr;
                socklen_t len = sizeof(addr);

                auto result = ::getsockname(socket.native(), (sockaddr*)&addr, &len);

                if (result == -1)
                {
                    throw network_error("upd_ip4_socket::endpoint()", get_os_network_error_code());
                }
                assert(len == sizeof(addr));
                return addr;
#endif
        }

        async_service& default_async_service();

    } // namespace experimental
} // namespace rpnx

inline std::ostream& operator<<(std::ostream& lhs, rpnx::experimental::ip4_address const& addr)
{
    return lhs << (int)addr[0] << "." << (int)addr[1] << "." << (int)addr[2] << "." << (int)addr[3];
}

inline std::ostream& operator<<(std::ostream& lhs, rpnx::experimental::ip4_udp_endpoint const& ep)
{
    return lhs << ep.address() << ":" << ep.port();
}

inline std::ostream& operator<<(std::ostream& lhs, rpnx::experimental::ip4_tcp_endpoint const& ep)
{
    return lhs << ep.address() << ":" << ep.port();
}

#endif
