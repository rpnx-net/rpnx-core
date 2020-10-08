#ifndef RPNXCORE_NETWORK_HPP
#define RPNXCORE_NETWORK_HPP

#ifdef _WIN32
#include <winsock2.h>
#endif

#ifdef __linux__
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#endif

#include <climits>
#include <array>
#include <system_error>
#include <assert.h>
#include <vector>
#include <iostream>
#include "rpnx/network_error.hpp"

namespace rpnx
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
                if (result != 0) throw std::system_error(std::error_code(result, std::system_category()), "WSA intialization failed");
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
    }
    inline std::error_code get_net_os_error_code()
    {
        int er = WSAGetLastError();
        int erx = 1;

        if (er == WSAEACCES)
        {
            return std::make_error_code(network_errc::permission_denied);
        }

        return std::error_code(er, std::system_category());
    }
    using native_socket_type = SOCKET;
#endif

#ifdef __linux__
    inline std::error_code get_net_os_error_code()
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

    template <typename It>
    void net_send(ip4_udp_socket& socket, ip4_udp_endpoint const& to, It input_begin, It input_end);

    template <typename It>
    auto net_receive(ip4_udp_socket& socket, ip4_udp_endpoint& from, It output_begin, It output_bounds_check)->It;

    template <typename It>
    auto net_receive(ip4_udp_socket& socket, ip4_udp_endpoint& from, It output_begin)->It;

    void net_bind(ip4_udp_socket& socket, ip4_udp_endpoint const& bind);

    void net_listen(ip4_tcp_acceptor& socket, ip4_tcp_endpoint const& addr);

    ip4_tcp_connection net_accept_connection(ip4_tcp_acceptor&);
    void net_accept_connection(ip4_tcp_acceptor& socket, ip4_tcp_connection& connection);

    template <typename It>
    auto net_receive(ip4_tcp_connection& socket, It output_begin, It output_end)->void;

    template <typename It>
    auto net_receive(ip4_tcp_connection& socket, size_t count, It output ) -> void;

   

  class ip4_address final
  {
  private:
      using data_type = std::array<std::uint8_t, 4>;
      data_type m_addr;
  public:
      ip4_address() noexcept
          : m_addr{0,0,0,0}
      {

      }

      ip4_address(std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d) noexcept
          : m_addr{a,b,c,d}
      {

      }

      explicit ip4_address(in_addr const& addr)
      {
          #ifdef __WIN32
          m_addr[0] = addr.S_un.S_un_b.s_b1;
          m_addr[1] = addr.S_un.S_un_b.s_b2;
          m_addr[2] = addr.S_un.S_un_b.s_b3;
          m_addr[3] = addr.S_un.S_un_b.s_b4;
          #elif defined(__linux__)
          m_addr[0] = reinterpret_cast<char const*>(&addr.s_addr)[0];
          m_addr[1] = reinterpret_cast<char const*>(&addr.s_addr)[1];
          m_addr[2] = reinterpret_cast<char const*>(&addr.s_addr)[2];
          m_addr[3] = reinterpret_cast<char const*>(&addr.s_addr)[3];
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
#ifdef __linux__
      return { m_addr[0] | (m_addr[1] << 8) | (m_addr[3] << 16) | (m_addr[3] << 24) };
#endif
    }

    
    using value_type = data_type::value_type;
    using iterator = data_type::iterator;
    using const_iterator = data_type::const_iterator;
    using pointer = data_type::pointer;
    using const_pointer = data_type::const_pointer;
    using reference = data_type::reference;
    using const_reference = data_type::const_reference;

    inline value_type & operator [] (int n)
    {
        return m_addr[n];
    }

    inline value_type const& operator [](int n) const
    {
        return m_addr[n];
    }

    iterator begin() { return std::begin(m_addr); }
    const_iterator begin() const { return std::begin(m_addr); }

    iterator end() { return std::end(m_addr); }
    const_iterator end() const { return std::end(m_addr); }


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

      inline ip4_tcp_endpoint()
          :m_addr(), m_port(0)
      {

      }
      inline ip4_tcp_endpoint(ip4_address addr, uint16_t port)
      : m_addr(addr), m_port(port)
      {}

      inline explicit ip4_tcp_endpoint(sockaddr_in const & in)
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

      ip4_address& address() { return m_addr; }
      uint16_t& port() { return m_port;  }

      ip4_address const& address() const { return m_addr; }
      uint16_t const& port() const { return m_port;  }
  };


  class ip4_udp_endpoint
  {
      ip4_address m_addr;
      uint16_t m_port;
  public:
      inline ip4_udp_endpoint() :m_port(0), m_addr() {
          
      }
      inline ip4_udp_endpoint(sockaddr_in const & native_addr)
          : ip4_udp_endpoint()
      {
          m_port = ntohs(native_addr.sin_port);
          #ifdef _WIN32
          m_addr[0] = native_addr.sin_addr.S_un.S_un_b.s_b1;
          m_addr[1] = native_addr.sin_addr.S_un.S_un_b.s_b2;
          m_addr[2] = native_addr.sin_addr.S_un.S_un_b.s_b3;
          m_addr[3] = native_addr.sin_addr.S_un.S_un_b.s_b4;
          #else
          m_addr[0] = native_addr.sin_addr.s_addr >> 24;
          m_addr[1] = native_addr.sin_addr.s_addr >> 16;
          m_addr[2] = native_addr.sin_addr.s_addr >> 8;
          m_addr[3] = native_addr.sin_addr.s_addr >> 0;
          #endif
      }

      inline ip4_udp_endpoint(ip4_address addr, uint16_t port)
          : m_addr(addr), m_port(port)
      {
          
      }

      inline ip4_udp_endpoint(ip4_udp_endpoint const&) = default;
      
      explicit inline ip4_udp_endpoint(ip4_tcp_endpoint const& other)
          : m_addr(other.address()), m_port(other.port())
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

      ip4_address& address() { return m_addr; }
      uint16_t& port() { return m_port; }

      ip4_address const& address() const { return m_addr; }
      uint16_t const& port() const { return m_port; }
  };


  class ip4_udp_socket
  {
public:
#ifdef _WIN32
      native_socket_type m_s;
#else
      native_socket_type m_s;
#endif
  public:
      ip4_udp_socket() noexcept
      {     
#ifdef __WIN32
         m_s = INVALID_SOCKET;
#else
         m_s = -1;
#endif
      }

      ip4_udp_socket(ip4_udp_socket const&) = delete;

      inline ip4_udp_socket(ip4_udp_socket&& other) noexcept
          : ip4_udp_socket()
      {
          std::swap(m_s, other.m_s);
      }


      ~ip4_udp_socket()
      {
          close();
      }

      template <typename It> 
      auto receive(ip4_udp_endpoint & from, It  out_iterator) -> It
      {
          return net_receive(*this, from, out_iterator);
      }

      template <typename It>
      auto receive(ip4_udp_endpoint& from, It out_iterator, It out_iterator_bounds) -> It
      {
          return net_receive(*this, from, out_iterator, out_iterator_bounds);
      }

      template <typename It>
      void send(ip4_udp_endpoint const & destination, It begin, It end)
      {
          net_send(*this, destination, begin, end);
      }

      void bind(ip4_udp_endpoint ep)
      {
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
            throw network_error("upd_ip4_socket::endpoint()", get_net_os_error_code());
        }
        assert(len == sizeof(addr));
        return addr;
#else
        sockaddr_in addr;
        socklen_t len = sizeof(addr);

        auto result = ::getsockname(m_s, (sockaddr*)&addr, &len);

        if (result == -1)
        {
            throw network_error("upd_ip4_socket::endpoint()", get_net_os_error_code());
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
          if (m_s == INVALID_SOCKET) throw network_error("udp_ip4_socket::open()", get_net_os_error_code());
          #else
          this->close();
          m_s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
          if (m_s == -1) throw network_error("udp_ip4_socket::open()", get_net_os_error_code());
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
#ifdef _WIN32
      SOCKET m_sock;
#else
      int m_sock;
 #endif

  public:
      ip4_tcp_acceptor() noexcept
          : m_sock(-1)
      {

      }

      ~ip4_tcp_acceptor()
      {
          close();
      }

      ip4_tcp_acceptor(ip4_tcp_acceptor const&) = delete;

      ip4_tcp_acceptor(ip4_tcp_acceptor&& other) noexcept
          :m_sock(-1)
      {
          std::swap(m_sock, other.m_sock);
      }


      void close() noexcept
      {
#ifdef _WIN32
          //detail::wsa_intializer::singleton();
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
          if (m_sock == INVALID_SOCKET) throw network_error("tcp_ip4_socket::open()", get_net_os_error_code());
          #else
          close();
          m_sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
          if (m_sock == -1) throw network_error("tcp_ip4_socket::open()", get_net_os_error_code());
          #endif
      }

      void listen(ip4_tcp_endpoint const & listen_addr)
      {
          #ifdef _WIN32
          detail::wsa_intializer::singleton();
          open();
          ::sockaddr_in sock_addr = listen_addr.native();
          ::bind(m_sock, (sockaddr*)&sock_addr, sizeof(sockaddr_in));
          if (::listen(m_sock, SOMAXCONN) == SOCKET_ERROR) 
          {
              throw network_error("tcp_ip4_socket::open()", get_net_os_error_code());
          }
          #else
          this->open();
          ::sockaddr_in sock_addr = listen_addr.native();
          ::bind(m_sock, (sockaddr*)&sock_addr, sizeof(sockaddr_in));
          if (::listen(m_sock, SOMAXCONN) == -1) 
          {
              throw network_error("tcp_ip4_socket::open()", get_net_os_error_code());
          }
          #endif
              
      }

#ifdef _WIN32
      SOCKET native() const noexcept
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
      ip4_tcp_connection() noexcept
          : m_socket(-1)
      {

      }

      ip4_tcp_connection(ip4_tcp_connection const&) = delete;

      ip4_tcp_connection(ip4_tcp_connection&& other) noexcept
          : m_socket(-1)
      {
          std::swap(other.m_socket, m_socket);
      }

      /**
        Create a socket from a native SOCKET object.
        Ownership of the SOCKET is assumed by this class.
      */
      explicit inline ip4_tcp_connection(native_socket_type s) noexcept
          :m_socket(s)
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
              throw network_error("ip4_tcp_connection::endpoint", get_net_os_error_code());
          }

          return ip4_tcp_endpoint(saddr);
      }

      ip4_tcp_endpoint peer_endpoint()
      {
          sockaddr_in saddr;
          socklen_t sz = sizeof(saddr);
          if (getpeername(m_socket, (sockaddr*)&saddr, &sz) != 0)
          {
              throw network_error("peer_endpoint", get_net_os_error_code());
          }

          return ip4_tcp_endpoint(saddr);

      }

      /**
        Returns a copy of the internal SOCKET. This class retains ownership of the SOCKET.
      */
      native_socket_type native() const noexcept
      {
          return m_socket;
      }

      /**
       Extracts the internal SOCKET. The function relinquishes ownership of the socket and the class's internal socket becomes INVALID_SOCKET.
      */
      native_socket_type extract_native() noexcept
      {
          native_socket_type skt = m_socket;
          m_socket = -1;
          return skt;
      }

      bool valid() const noexcept
      {
          return m_socket != -1;
      }
  };

  template <typename It>
  void net_send(ip4_udp_socket& socket, ip4_udp_endpoint const & to, It begin_packet, It end_packet)
  {
      #ifdef _WIN32
      detail::wsa_intializer::singleton();
      sockaddr_in dest = to.native();
      std::vector<std::byte> data(begin_packet, end_packet);

      auto result = sendto(socket.native(), (char const*)data.data(), data.size(), 0, (sockaddr*)&dest, sizeof(dest));
      if (result == SOCKET_ERROR)
      {
          throw network_error("upd_ip4_socket::send()", get_wsa_error_code());
      }
      return;
      #else
      sockaddr_in dest = to.native();
      std::vector<std::byte> data(begin_packet, end_packet);

      auto result = sendto(socket.native(), (char const*)data.data(), data.size(), 0, (sockaddr*)&dest, sizeof(dest));
      if (result == -1)
      {
          throw network_error("upd_ip4_socket::send()", get_net_os_error_code());
      }
      return;
      #endif
  }


  template <typename It>
  void net_send(ip4_tcp_connection& socket, It begin_data, It end_data)
  {
      #ifdef _WIN32
      detail::wsa_intializer::singleton();
      
      std::vector<std::byte> data(begin_data, end_data);

      // TODO: Right now we don't support sending more than 2^32 bytes at once on windows
      // Ideally we should fix this
      if (data.size() >= INT_MAX) throw std::bad_alloc("send too large");
      
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
              throw network_error("rpnx::net_send", get_wsa_error_code());
          }

      }

      #else

      std::vector<std::byte> data(begin_data, end_data);
      
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
              throw network_error("rpnx::net_send", get_net_os_error_code());
          }

      }

      #endif
  }

  template <typename It>
  auto net_receive(ip4_udp_socket& socket, ip4_udp_endpoint& fr, It begin_packet, It end_packet) -> It
  {
      #ifdef _WIN32
      detail::wsa_intializer::singleton();
      #endif
      std::array<std::byte, 256 * 256> buffer;
      sockaddr_in from;
      static_assert(sizeof(from) < INT_MAX);
      #ifdef _WIN32
      int fromlen = sizeof(from);
      #else
      socklen_t fromlen = sizeof(from);
      #endif
      auto count = recvfrom(socket.native(), reinterpret_cast<char*>(buffer.data()), (int)buffer.size(), 0, reinterpret_cast<sockaddr*>(&from), &fromlen);
      if (count == -1)
      {
          throw network_error("upd_ip4_socket::receive()", get_net_os_error_code());
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

  template <typename It>
  auto net_receive(ip4_udp_socket& socket, ip4_udp_endpoint& fr, It begin_packet) -> It
  {
      #ifdef _WIN32
      detail::wsa_intializer::singleton();
      #endif
      std::array<std::byte, 256 * 256> buffer;
      sockaddr_in from;
      static_assert(sizeof(from) < INT_MAX);

      #ifdef _WIN32
      int fromlen = sizeof(from);
      #else
      socklen_t fromlen = sizeof(from);
      #endif

      auto count = recvfrom(socket.native(), reinterpret_cast<char*>(buffer.data()), (int)buffer.size(), 0, reinterpret_cast<sockaddr*>(&from), &fromlen);
      if (count == -1)
      {
          throw network_error("upd_ip4_socket::receive()", get_net_os_error_code());
      }

      It it = begin_packet;
      auto buf_it = buffer.begin();
      auto buf_end = buffer.begin() + count;
      while (buf_it != buf_end)
      {
          *it = *buf_it;
          ++it;
          ++buf_it;
      }
      fr = ip4_udp_endpoint(from);
      return it;
  }

  inline void net_bind(ip4_udp_socket& socket, ip4_udp_endpoint const & bind_addr)
  {
      #ifdef _WIN32
      detail::wsa_intializer::singleton();
      #endif
      sockaddr_in socket_addr = bind_addr.native();
      auto result = ::bind(socket.native(), (const sockaddr*)&socket_addr, sizeof(socket_addr));
      if (result == -1)
      {
          throw network_error("upd_ip4_socket::bind()", get_net_os_error_code());
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
          throw network_error("rpnx::net_accept_connection", get_wsa_error_code());
      }

      return std::move(con);
      #else
      ip4_tcp_connection con = ip4_tcp_connection(::accept(socket.native(), nullptr, nullptr));
      if (con.native() == -1)
      {
          throw network_error("rpnx::net_accept_connection", get_net_os_error_code());
      }

      return std::move(con);
      #endif

  }
}

inline std::ostream& operator << (std::ostream& lhs, rpnx::ip4_address const& addr)
{
    return lhs << (int) addr[0] << "." << (int) addr[1] << "." << (int) addr[2] << "." << (int) addr[3];
}

inline std::ostream& operator << (std::ostream& lhs, rpnx::ip4_udp_endpoint const& ep)
{
    return lhs << ep.address() << ":" << ep.port();
}

inline std::ostream& operator << (std::ostream& lhs, rpnx::ip4_tcp_endpoint const& ep)
{
    return lhs << ep.address() << ":" << ep.port();
}


#endif
