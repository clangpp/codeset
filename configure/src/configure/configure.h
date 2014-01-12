// configure.h
// a simple toolset to read configuration file
// Author: Yongtian Zhang (yongtianzhang@gmail.com)
// $LastChangedDate: 2011-10-22 16:42:32 +0800 (周六, 2011-10-22) $

#ifndef CONFIGURE_H_
#define CONFIGURE_H_

#include <map>
#include <stdexcept>
#include <string>
#include "../../../facility/src/facility/facility.h"

namespace configure {

// module configure object
// =============================================================================
class Configure {
public:
	typedef std::map<std::string, std::string>::const_iterator iterator;
    iterator begin() const { return table_.begin(); }
    iterator end() const { return table_.end(); }

public:
	// read configuration variables from file
	void read(const std::string& cfg_file);

    // filter key-value pair in command line options that meets any of formats:
    //    1. --key=value
    //    2. --key value1 value2 value3 ...
    void read(int argc, char* argv[]);

	// clear configuration variables
	void clear() { table_.clear(); }
	
	// get configuration by key string and value type
    // if operation failed, throw exception
	template <typename T>
	T get(const std::string& key) const {
        return facility::to<T>(get_string(key));
    }

	// get configuration by key string and value type
    // if operation failed, use default_value
	template <typename T>
	T default_get(const std::string& key, const T& default_value) const {
        try {
            return facility::to<T>(get_string(key));
        } catch (const std::runtime_error&) {
            return default_value;
        }
    }

    // verify whether a key exists
    bool search(const std::string& key) const { return table_.count(key)>0; }

	// get configuration by section string, key string and value type
    // if operation failed, throw exception
	template <typename T>
	T get(const std::string& section, const std::string& key) const {
		return get<T>(make_key(section, key));
	}

	// get configuration by section string, key string and value type
    // if operation failed, use default_value
	template <typename T>
	T default_get(const std::string& section, const std::string& key,
            const T& default_value) const {
        return default_get<T>(make_key(section, key), default_value);
	}

    // verify whether a key of a section exists
    bool search(const std::string& section, const std::string& key) const {
        return search(make_key(section, key));
    }

private:
	const std::string& get_string(const std::string& key) const;
	static std::string make_key(
            const std::string& section, const std::string& key) {
        return section.empty() ? key : section + "." + key;
    }

private:
	std::map<std::string, std::string> table_;
};

// define a standard global Configure object
// =============================================================================
inline Configure& standard_configure() {
	static Configure cfg;
	return cfg;
}

// namespace level free functions
// just map member functions of standard Configure object
// =============================================================================

inline void read(const std::string& cfg_file) {
	standard_configure().read(cfg_file);
}

inline void read(int argc, char* argv[]) {
    standard_configure().read(argc, argv);
}

inline void clear() { standard_configure().clear(); }

template <typename T>
inline T get(const std::string& key) { return standard_configure().get<T>(key); }

template <typename T>
inline T default_get(const std::string& key, const T& default_value) {
    return standard_configure().default_get<T>(key, default_value);
}

inline bool search(const std::string& key) {
    return standard_configure().search(key);
}

template <typename T>
inline T get(const std::string& section, const std::string& key) {
   	return standard_configure().get<T>(section, key);
}

template <typename T>
inline T default_get(const std::string& section, const std::string& key, 
        const T& default_value) {
   	return standard_configure().default_get<T>(section, key, default_value);
}

inline bool search(const std::string& section, const std::string& key) {
    return standard_configure().search(section, key);
}

inline Configure::iterator begin() { return standard_configure().begin(); }
inline Configure::iterator end() { return standard_configure().end(); }

}  // namespace configure

#endif  // CONFIGURE_H_
