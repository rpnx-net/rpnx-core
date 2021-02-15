//
// Created by rnicholl on 2/14/2021.
//

#ifndef RPNXCORE_ENVIRONMENT_WINDOWS_HPP
#define RPNXCORE_ENVIRONMENT_WINDOWS_HPP

#include <string>
#include <map>
#include <cstdlib>
#include <regex>

namespace rpnx

{
    namespace os
    {
        std::map<std::string, std::string> const & environment()
        {
            static std::map<std::string, std::string> const environm = []{
              std::map<std::string, std::string> result;
              getenv("");

              for (int i = 0; _environ[i]; i++)
              {
                  std::string line = _environ[i];
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
            }();
            return environm;
        }
    }


}

#endif // RPNXCORE_ENVIRONMENT_HPP
