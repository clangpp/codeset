#include "facility.h"

#include <algorithm>
#include <cctype>
#include <functional>
#include <iterator>

namespace facility {

std::string& trim_left(std::string& str) {
  std::string::iterator nsp_beg = std::find_if(
      str.begin(), str.end(), [](const auto& c) { return !std::isspace(c); });
  str.erase(str.begin(), nsp_beg);
  return str;
}

std::string& trim_right(std::string& str) {
  std::string::reverse_iterator nsp_rbeg = std::find_if(
      str.rbegin(), str.rend(), [](const auto& c) { return !std::isspace(c); });
  str.erase(nsp_rbeg.base(), str.end());
  return str;
}

std::string& trim(std::string& str) { return trim_left(trim_right(str)); }

std::string& copy_little_endian(const std::wstring& src, std::size_t wide,
                                std::string& dest) {
  return copy_little_endian(src.begin(), src.end(), wide, dest);
}

std::wstring& copy_little_endian(const std::string& src, std::size_t wide,
                                 std::wstring& dest) {
  return copy_little_endian(src.begin(), src.end(), wide, dest);
}

std::string& copy_big_endian(const std::wstring& src, std::size_t wide,
                             std::string& dest) {
  return copy_big_endian(src.begin(), src.end(), wide, dest);
}

std::wstring& copy_big_endian(const std::string& src, std::size_t wide,
                              std::wstring& dest) {
  return copy_big_endian(src.begin(), src.end(), wide, dest);
}

std::vector<std::string> split(const std::string& str,
                               const std::string& token) {
  std::vector<std::string> parts;
  for (std::string::size_type begin = 0, cursor = 0;
       cursor != std::string::npos; begin = cursor + token.length()) {
    cursor = str.find(token, begin);
    parts.push_back(str.substr(begin, cursor - begin));
  }
  return parts;
}

}  // namespace facility
