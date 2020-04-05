#ifndef RPNX_ALL_NETWORK_ERROR_HH
#define RPNX_ALL_NETWORK_ERROR_HH

#include <system_error>

namespace rpnx
{
    enum class network_errc : int
    {
        bad_address = 1, port_already_in_use, permission_denied, network_down, destination_unreachable,
    };

  // std::error_code make_error_code(rpnx::network_errc er);
    

}

namespace std
{
    template <>
    struct is_error_code_enum<rpnx::network_errc> : true_type {};


}

namespace rpnx
{
    class network_category_t final
        : public std::error_category
    {
        virtual const char* name() const noexcept override final  { return "rpnx_network"; }
        virtual std::string message(int erc) const override final
        {
            switch (static_cast<network_errc>(erc))
            {
            case network_errc::bad_address:
            {
                return "Bad address";
            }

            case network_errc::port_already_in_use:
            {
                return "Port already in use";
            }

            default:
            {
                return "Unknown error";
            }
            }
        }
    };

    std::error_category& network_category()
    {
        static network_category_t singleton;
        return singleton;
    }

    

    class network_error
        : public  std::system_error
    {
    private:
    public:
        network_error(std::string const& message, std::error_code er)
            : std::system_error(er, message)
        {

        }
    };


}

namespace std
{
    inline std::error_code make_error_code(rpnx::network_errc er)
    {
        return std::error_code(static_cast<int>(er), rpnx::network_category());
    }
}

#endif