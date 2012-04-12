// facility.cpp
#include "facility.h"

#include <algorithm>
#include <cctype>
#include <iterator>

namespace facility {

std::string& trim_left(std::string& str) {
	std::string::iterator iter = str.begin();
	for (; iter!=str.end() && std::isspace(*iter); ++iter) {}
	str.erase(str.begin(), iter);
	return str;
}

std::string& trim_right(std::string& str) {
	std::string::reverse_iterator riter = str.rbegin();
	for (; riter!=str.rend() && std::isspace(*riter); ++riter) {}
	str.erase(riter.base(), str.rbegin().base());
	return str;
}

std::vector<std::string> split(
        std::string& str, const std::string& token) {
    std::vector<std::string> parts;
    for (std::string::size_type begin=0, cursor=0;
            cursor!=std::string::npos; begin=cursor+token.length()) {
        cursor = str.find(token, begin);
        parts.push_back(str.substr(begin,cursor-begin));
    }
    return parts;
}

}  // namespace facility
