// test.cpp

#include "configure/configure.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

void test_case(const string& case_name);

void test_read_command_line_options();

int main(int argc, char* argv[]) {

	try {

    // read configuration file
    // =========================================================================
    string cfg_file = "test.cfg";
    cout << "reading configuration file '" << cfg_file << "'..." << endl;

    configure::read(cfg_file);
	configure::read(argc, argv);

    ifstream fin(cfg_file.c_str());
    cout << "content of file '" << cfg_file << "': " << endl;
    cout << "=======================================" << endl;
    cout << fin.rdbuf() << endl;
    cout << "=======================================" << endl;

	cout << endl;
    cout << "configuration information: " << endl;
    cout << "=======================================" << endl;

    // get configurations
    // =========================================================================
	string log_file = configure::default_get<string>("log_file", "log.txt");

	if (!configure::verify("cases")) {
		throw runtime_error("no 'cases' configuration");
	}
    vector<string> cases = configure::get<vector<string> >("cases");

    cout << "log_file: " << log_file << endl;
    cout << "cases: " << endl;
    for (size_t i=0; i<cases.size(); ++i)
        cout << "    " << i << ": " << cases[i] << endl;
    cout << endl;

    // get case configurations
    // =========================================================================
    for (size_t i=0; i<cases.size(); ++i)
        test_case(cases[i]);

	test_read_command_line_options();

	} catch (const exception& e) {
		cerr << e.what() << endl;
		return 1;
	}

	return 0;
}

void test_case(const string& case_name) {
	assert(configure::verify(case_name, "server_port"));
    string host = configure::get<string>("tt_server_host");
    int port = configure::get<int>(case_name, "server_port");
    size_t amount = configure::get<int>(case_name, "record_amount");

    cout << "in case: " << case_name << endl;
    cout << "    tt_server: " << host << ":" << port << endl;
    cout << "    record_amount: " << amount << endl;
    cout << endl;
}

void test_read_command_line_options() {
	char* options[] = {"cmd","sub-cmd", "--port", "1024", "--server", "127.0.0.1", "192.168.0.1", "--port", "1025", "1027"};
	configure::Configure cfg;
	cfg.read(sizeof(options)/sizeof(char*), options);
	vector<int> ports = cfg.get<vector<int> >("port");
	int ports_expected[] = {1025, 1027};
	bool passed = ports == vector<int>(ports_expected, ports_expected+sizeof(ports_expected)/sizeof(int));
	cout << "passed? " << passed << endl;
}