// main.cpp

#include <iostream>
#include <stdexcept>

#include "waf_shell/waf_shell.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "usage: waf <command> <command-options>" << endl;
        cerr << "try 'waf help' for more information." << endl;
        return -1;
    }
    
    typedef int (*command_type) (int argc, char* argv[]);
    map<string, command_type> func_table;
    func_table["term-to-termid"] = &run_term_to_termid;
    func_table["termid-frequency"] = &run_termid_frequency;
    func_table["co-occurrence"] = &run_co_occurrence;
    func_table["word-activation-force"] = &run_word_activation_force;
    func_table["affinity-measure"] = &run_affinity_measure;
    func_table["analyze-matrix"] = &run_analyze_matrix;
    func_table["filter-termset"] = &run_filter_termset;
    func_table["help"] = &run_help;

    if (!func_table.count(argv[1])) {
        cerr << "error: unknown command '" << argv[1] << "'" << endl;
        cerr << "try 'waf help' for more information." << endl;
        return -1;
    }

    try {
        return (*func_table[argv[1]])(argc-2, argv+2);
    } catch (const exception& e) {
		cerr << "exception occured: " << e.what() << endl;
		return -1;
    }
}
