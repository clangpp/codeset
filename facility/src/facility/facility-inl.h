// facility-inl.h
#ifndef FACILITY_INL_H_
#define FACILITY_INL_H_

#include "facility.h"
#include <sstream>
#include <string>
#include <vector>

namespace facility {
namespace internal {

// specialization for std::stringstream extractor (>>) to vector<T> 
template <typename T>
inline std::stringstream& operator >> (
		std::stringstream& ss, std::vector<T>& vec) {
	vec.clear();
	T value = T();
	while (ss >> value)
		vec.push_back(value);
	return ss;
}

// generic conversion from std::stringstream to ToType
template <typename ToType>
inline ToType to(std::stringstream& ss) {
	ToType result = ToType();
	ss >> result;
	return result;
}

// specialization for conversion from std::stringstream to std::string
template <>
inline std::string to<std::string>(std::stringstream& ss) {
	return ss.str();
}

}  // namespace facility::internal


// generic conversion API
template <typename ToType, typename FromType>
inline ToType to(const FromType& from) {
	std::stringstream ss;
	ss << from;
	return internal::to<ToType>(ss);
}

template <typename T, std::size_t N>
inline std::size_t array_length(T (&)[N]) { return N; }

}  // namespace facility

#endif  // FACILITY_INL_H_

