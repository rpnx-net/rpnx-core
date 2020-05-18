#ifndef RPNXCORE_LINUX_NETWORK_HPP
#define RPNXCORE_LINUX_NETWORK_HPP

#include "rpnx/network_error.hpp"

#include <array>
#include <cinttypes>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 


namespace rpnx
{
  
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
  
  class ip4_tcp_listener final
  {
    int m_fd;
  public:
    ip4_tcp_listener() : m_fd(-1) {}
    ip4_tcp_listener(ip4_tcp_listener const &) = delete;
    
    
    
  };
  
  class ip4_tcp_connection final
  {
  };
}

#endif
