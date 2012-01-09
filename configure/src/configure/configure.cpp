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
		throw std::runtime_error(ss.str());
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
			throw std::runtime_error(ss.str());
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
    std::string curr_key;
    for (int i=0; i<argc; ++i) {
        std::string arg(argv[i]);

        if (arg.find("--")==0) { // meet a key
            std::string::size_type equal_pos = arg.find('=');
            if (equal_pos!=std::string::npos) { // key-value pair
                std::string key = arg.substr(2,equal_pos-2);
                std::string value = arg.substr(equal_pos+1);
                table_[facility::trim(key)] = facility::trim(value);
                curr_key.clear(); // clear hanging key
            } else { // hanging key
                std::string key = arg.substr(2);
                curr_key = facility::trim(key);
                table_[curr_key] = ""; // clear old value of hanging key
            }

        } else if (!curr_key.empty()) { // there is a hanging key
            std::string& curr_value = table_[curr_key];
            if (!curr_value.empty())
                curr_value.append(" ");
            curr_value.append(arg);
        }
    }
}

const std::string& Configure::get_string(const std::string& key) const {
	std::map<std::string, std::string>::const_iterator iter=table_.find(key);
	if (table_.end()==iter) {
		std::stringstream ss;
		ss << "Configure::get_string(string) const: "
			<< "key '" << key << "' "
			<< "is not in configuration variable table";
		throw std::runtime_error(ss.str());
	}
	return iter->second;
}

}  // namespace configure
