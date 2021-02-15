//
// Created by rnicholl on 2/14/2021.
//

#ifndef RPNXCORE_ENVIRONMENT_HPP
#define RPNXCORE_ENVIRONMENT_HPP

#include <string>
#include <map>
#include <cstdlib>
#include <regex>
#include <mutex>
#include <optional>
#include <stdlib.h>
#include <filesystem>

#ifndef _WIN32
extern "C"
{
    extern char** environ;
}
#endif

namespace rpnx

{

    inline std::mutex environment_mutex;

    /** A thread safe way to get all environment variables
     *
     * @return
     */
    inline std::map<std::string, std::string> get_environment()
    {
        std::unique_lock lock (environment_mutex);

#ifdef _WIN32
        getenv("");
#endif
        std::map<std::string, std::string> result;


        for (int i = 0; environ[i]; i++)
        {
          std::string line = environ[i];
          std::regex re("([^=]*)=([^=]*)");
          std::smatch match;
          if (std::regex_match(line, match, re))
          {
              std::string key = match[1];
              std::string value = match[2];
              result[key] = value;
          }

        }
        return result;
    }

    inline std::optional<std::string> get_environment_variable(std::string const & key)
    {
        std::unique_lock lock (environment_mutex);
        auto result = getenv(key.c_str());
        if (result != nullptr)
        {
            return std::string(result);
        }
        return std::nullopt;
    }

    inline void set_environment_variable(std::string const & key, std::string const & value)
    {
        std::unique_lock lock (environment_mutex);

        putenv(const_cast<char*>((key + "=" + value).c_str()));
        // TODO check for enomem etc
    }

    inline std::vector<std::filesystem::path> get_path()
    {
        std::string path_string;
        std::vector<std::filesystem::path> results;
        {
            auto path_val = get_environment_variable("PATH");
            if (path_val.has_value())
            {
                path_string = *path_val;
            }
            else return results;
        }

        // TODO: Modify this to properly accept path values with semicolons/colons in them (e.g. /mnt/c:"/foo/bar:baz";
#ifdef _WIN32
        std::regex re("[^;]+");
#else
        std::regex re("[^:]+");
#endif
        auto re_begin = std::sregex_iterator(path_string.begin(), path_string.end(), re);
        auto re_end = std::sregex_iterator();

        for (std::sregex_iterator i = re_begin; i != re_end; ++i)
        {
            results.push_back(i->str());
        }

        return results;
    }

    /** Like std::filesystem::current_path, except thread-safe when used exclusively with other rpnx functions to mutate the environment.
     */
    inline std::filesystem::path current_path()
    {
#ifdef _WIN32
      wchar_t * path_value = _wgetcwd(NULL, 0);
      if (path_value == nullptr) return {};
      // TODO: throw error here

      std::filesystem::path result;
      result = path_value;

      free(path_value);
      // TODO: Scope guard on free

      return result;
#else
      return get_environment_variable("PWD").value_or("");
#endif
    }

}




#endif // RPNXCORE_ENVIRONMENT_HPP
