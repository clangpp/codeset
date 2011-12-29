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

}  // namespace facility
