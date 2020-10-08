#ifndef RPNXCORE_LINUX_NETWORK_HPP
#define RPNXCORE_LINUX_NETWORK_HPP

#include "rpnx/network_error.hpp"

#include <array>
#include <cinttypes>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <unistd.h>
#include <assert.h>



namespace rpnx
{

  inline std::error_code get_linux_error_code()
  {
    return std::error_code(errno, std::system_category());
  }
  
  class ip4_address final
  {
    std::array<uint8_t, 4> m_addr;
    using data_type = std::array<std::uint8_t, 4>;
  public:
    inline ip4_address()
    {}
    
    inline ip4_address(uint8_t a, uint8_t b, uint8_t c, uint8_t d) : m_addr{a,b, c,d }
    {}
    
    ip4_address(ip4_address const &) = default;
    
    ip4_address & operator= (ip4_address const &) = default;

    ip4_address(in_addr const & addr)
    {
      m_addr[0] = reinterpret_cast<char const*>(&addr.s_addr)[0];
      m_addr[1] = reinterpret_cast<char const*>(&addr.s_addr)[1];
      m_addr[2] = reinterpret_cast<char const*>(&addr.s_addr)[2];
      m_addr[3] = reinterpret_cast<char const*>(&addr.s_addr)[3];
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


    in_addr native() const
    {
      return { htonl(m_addr[0] | (m_addr[1] << 8) | (m_addr[2] << 16) | (m_addr[3] << 24)) };
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
  
  class ip4_udp_endpoint final
  {
    ip4_address m_addr;
    std::uint16_t m_port;
    
    
  public:
    inline ip4_udp_endpoint() {}
    
    inline ip4_udp_endpoint(ip4_address const & addr, std::uint16_t port)
     : m_addr(addr),  m_port(port)
    {
      
    }
    
    sockaddr_in native() const 
    {
      sockaddr_in addr;
      addr.sin_family = AF_INET;
      addr.sin_addr  = m_addr.native();
      addr.sin_port = htons(m_port);
      return addr;
    }
    

  };

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
  
  
  class ip4_udp_socket final
  {
    int m_s;
    
  public:
    inline ip4_udp_socket() : m_s(-1){}
    
    operator bool()
    {
      return m_s != -1;
    }
    
    void open()
    {
      m_s = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      if (m_s == -1) throw std::system_error(std::error_code(errno, std::system_category()), "ip4_udp_socket::open");
    }
  };
  
  class ip4_tcp_acceptor final
  {
    int m_sock;
  public:
    ip4_tcp_acceptor() : m_sock(-1) {}
    ip4_tcp_acceptor(ip4_tcp_acceptor const &) = delete;
    
    void close()
    {
      if (m_sock != -1)
      {
        ::close(m_sock);
        m_sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      }
    }

    void listen( ip4_tcp_endpoint const & listen_addr )
    {
      this->close();
      m_sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

      ::sockaddr_in sock_addr = listen_addr.native();
      ::bind(m_sock, (sockaddr*)&sock_addr, sizeof(sockaddr_in));
      if (::listen(m_sock, SOMAXCONN) == -1) 
      {
          throw network_error("tcp_ip4_socket::open()", get_linux_error_code());
      }
    }    

    
  };
  
  class ip4_tcp_connection
  {
  private:
      int m_socket;
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
        Create a socket from a native fd.
        Ownership of the fd is assumed by this class.
      */
      explicit inline ip4_tcp_connection(int s) noexcept
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
          socklen_t sz = sizeof(saddr);
          if (getsockname(m_socket, (sockaddr*)&saddr, &sz) != 0)
          {
              throw network_error("ip4_tcp_connection::endpoint", get_linux_error_code());
          }

          return ip4_tcp_endpoint(saddr);
      }

      ip4_tcp_endpoint peer_endpoint()
      {
          sockaddr_in saddr;
          socklen_t sz = sizeof(saddr);
          if (getpeername(m_socket, (sockaddr*)&saddr, &sz) != 0)
          {
              throw network_error("peer_endpoint", get_linux_error_code());
          }

          return ip4_tcp_endpoint(saddr);

      }

      /**
        Returns a copy of the internal SOCKET. This class retains ownership of the SOCKET.
      */
      int native() const noexcept
      {
          return m_socket;
      }

      /**
       Extracts the internal SOCKET. The function relinquishes ownership of the socket and the class's internal socket becomes INVALID_SOCKET.
      */
      int extract_native() noexcept
      {
          int skt = m_socket;
          m_socket = -1;
          return skt;
      }

      bool valid() const noexcept
      {
          return m_socket != -1;
      }
  };
}

#endif
