// configure.cpp
#include "configure.h"
#include <cstddef>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace configure {

void Configure::read(const std::string& cfg_file) {
	std::ifstream fin(cfg_file.c_str());
	if (!fin) {  // fail to open configuration file
		std::stringstream ss;
		ss << "Configure::read(string): "
			<< "configuration file '" << cfg_file << "' "
			<< "open failed";
		throw std::invalid_argument(ss.str());
	}

	std::string text;
	std::string section;
	std::size_t line_num = 0;
	while (std::getline(fin, text)) {  // parse every line in file
		++line_num;

		std::stringstream ss(text);
		getline(ss,text,'#');  // trim off comments
        facility::trim(text);  // trim off spaces
		if (text.empty())  // space line or comments line
			continue;

		if ('['==*text.begin() && ']'==*text.rbegin()) {  // section name
			text = text.substr(1, text.size()-2);  // trim off '[' ']'
			section = facility::trim(text);
			continue;
		}

		std::string::size_type equal_pos=text.find('=');
		if (std::string::npos==equal_pos) {  // no key-value pair, report error
			std::stringstream ss;
			ss << "Configure::read(string):\n"
				<< "    configuration file '" << cfg_file << "'\n"
				<< "    line " << line_num << ":\n"
				<< "    text '" << text << "'\n"
				<< "    error line format!";
			throw std::invalid_argument(ss.str());
		}

		std::string key=text.substr(0,equal_pos);  // form key-value pair
		std::string value=text.substr(equal_pos+1);
        facility::trim(key);
		facility::trim(value);
        key = make_key(section, key);  // add section info

		table_[key] = value;  // insert to table, cover old one
	}
}

void Configure::read(int argc, char* argv[]) {
    const std::string prefix("--");
    for (int i=1; i<argc; ++i) {
        std::string text(argv[i]);

        // filter option format like '--key=value'
        if (text.substr(0,prefix.size()) != prefix)
            continue;
        std::string::size_type equal_pos=text.find('=');
        if (std::string::npos==equal_pos)
            continue;

        // form key-value pair, insert into table
        std::string key=text.substr(prefix.size(),equal_pos-prefix.size());
        std::string value=text.substr(equal_pos+1);
        facility::trim(key);
        facility::trim(value);
        table_[key] = value;
    }
}

const std::string& Configure::get_string(const std::string& key) const {
	std::map<std::string, std::string>::const_iterator iter=table_.find(key);
	if (table_.end()==iter) {
		std::stringstream ss;
		ss << "Configure::get_string(string) const: "
			<< "key '" << key << "' "
			<< "is not in configuration variable table";
		throw std::invalid_argument(ss.str());
	}
	return iter->second;
}

}  // namespace configure
