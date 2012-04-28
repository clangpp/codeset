// facility.cpp
#include "facility.h"

#include <algorithm>
#include <cctype>
#include <functional>
#include <iterator>

namespace facility {

std::string& trim_left(std::string& str) {
	std::string::iterator nsp_beg =
		std::find_if(str.begin(), str.end(),
				std::not1(std::ptr_fun<int, int>(&std::isspace)));
	str.erase(str.begin(), nsp_beg);
	return str;
}

std::string& trim_right(std::string& str) {
	std::string::reverse_iterator nsp_rbeg =
		std::find_if(str.rbegin(), str.rend(),
				std::not1(std::ptr_fun<int, int>(&std::isspace)));
	str.erase(nsp_rbeg.base(), str.end());
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
