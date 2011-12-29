// main.cpp
/**************************************************************************
module: waf
file(s): cross_list.h, sparse_matrix.h, waf.h, waf_text.h, main.cpp
function: provide toolset for text Word Activation Force (or WAF) analysis.

	This module provides an shell interface of waf analysis. They use data
	structures and call functions of other modules.

author: Yongtian Zhang, yongtianzhang@gmail.com
finish-time: 2011.04.21
last-modified: 2011.06.02

license:
	This code is not under any license, anyone can get it, use it, modified it, redistribute it without notice me. 
	I don't think there is no bug in it, so before you use it, please check and test carefully, I don't promise its quality. ^_^
	If you find any bug, please report it to me, I'll be very grateful to you. Discussion is also welcome.

**************************************************************************/

#include "waf/waf.h"
#include "waf/waf_text.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <utility>

#include "logging/logging.h"

using namespace logging;
using namespace std;

int run_task(int argc, char* argv[]);

template <typename InputIterator>
int run_ti(InputIterator arg_beg, InputIterator arg_end);

template <typename InputIterator>
int run_tf(InputIterator arg_beg, InputIterator arg_end);

template <typename InputIterator>
int run_co(InputIterator arg_beg, InputIterator arg_end);

template <typename InputIterator>
int run_waf(InputIterator arg_beg, InputIterator arg_end);

template <typename InputIterator>
int run_a(InputIterator arg_beg, InputIterator arg_end);

template <typename InputIterator>
int run_info(InputIterator arg_beg, InputIterator arg_end);

int print_help_info();

template <typename InputIterator>
void init_option_table(
		InputIterator opt_beg, InputIterator opt_end,
		map<string,vector<string> >& option_table);

template <typename InputIterator>
void fill_option_table(
		InputIterator arg_beg, InputIterator arg_end,
		map<string,vector<string> >& option_table);

bool resolve_unknown_otions(
		const vector<string>& args, const string& func_hint);

bool resolve_file_input_option(
		const vector<string>& args, const string& opt_hint,
		const string& func_hint, istream*& is_ptr,
		ifstream& fin, istream& is_dft=cin);

bool resolve_files_input_option(
		const vector<string>& args, const string& opt_hint,
		const string& func_hint, int limit,
		bool& exist, vector<string>& file_vec);

bool resolve_file_output_option(
		const vector<string>& args, const string& opt_hint,
		const string& func_hint, ostream*& os_ptr,
		ofstream& fout, ostream& os_dft=cout);

bool resolve_files_output_option(
		const vector<string>& args, const string& opt_hint,
		const string& func_hint, int limit,
		bool& exist, vector<string>& file_vec);

bool resolve_file_log_option(
		const vector<string>& args, const string& opt_hint,
		const string& func_hint, ostream*& os_ptr,
		ofstream& flog, ostream& os_dft=clog);

bool resolve_file_log_option(
		const vector<string>& args, const string& opt_hint,
		const string& func_hint, ofstream& flog, Logger& logger);


bool resolve_term_info_option(
		const vector<string>& args, const string& opt_hint,
		const string& func_hint, bool& exist, ifstream& fs);

bool resolve_multi_arg_option(
		const vector<string>& args, const string& opt_hint,
		const string& func_hint, size_t argc_min, int argc_max,
		bool& exist, vector<string>& arg_vec);

bool resolve_single_arg_option(
		const vector<string>& args, const string& opt_hint,
		const string& func_hint, bool& exist, string& arg);

bool resolve_none_arg_option(
		const vector<string>& args, const string& opt_hint,
		const string& func_hint, bool& exist);

template <typename T, size_t N>
bool resolve_arg_in_table(
		const string& arg, const T (&table)[N],
		const string& opt_hint, const string& func_hint);

bool touch_input_file(const string& filename, const string& func_hint);

bool touch_output_file(const string& filename, const string& func_hint);

bool touch_input_files(const vector<string>& filenames, const string& func_hint);

bool touch_output_files(const vector<string>& filenames, const string& func_hint);

// append filenames in list_file to file_vec
void extract_filenames(const string& list_file, vector<string>& file_vec);

// append filenames in list_files to file_vec
void extract_filenames(const vector<string>& list_file_vec, vector<string>& file_vec);

string current_time();

void log(ostream* os_ptr, const string& func_hint, const string& message);

void report_exception(ostream* os_ptr, const string& func_hint, const string& ex_msg);

template <typename T1, typename T2>
ostream& operator << (ostream& os, const pair<T1,T2>& p);

template <typename T1, typename T2>
istream& operator >> (istream& is, pair<T1,T2>& p);

template <typename T>
ostream& operator << (ostream& os, const node_info<T>& ni);

template <typename ToType, typename FromType>
ToType cast(const FromType& from);

string& strip(string& s);

// parameter: Iterator, should be cross_list<T>::row_iterator/col_iterator/iterator
// parameter: OutputIterator, should reference to node_info<T> object
template <typename Iterator, typename OutputIterator>
void extract_node_info(Iterator first, Iterator last, OutputIterator out);

template <typename T>
void analyze_outlink(const cross_list<T>& mat,
		deque<deque<node_info<T> > >& node_vecs);

template <typename T>
void analyze_inlink(const cross_list<T>& mat,
		deque<deque<node_info<T> > >& node_vecs);

template <typename T>
void analyze_pair(const cross_list<T>& mat,
		deque<node_info<T> >& node_vec);

template <typename T>
bool analyze_matrix(const cross_list<T>& mat, const string& type,
		const string& func_hint, waf::size_type node_limit,
		deque<deque<node_info<T> > >& node_vecs);

template <typename T, typename Term>
bool analyze_process(
		istream& is, bool use_term_info,term_info<Term>& tinfo,
		const string& analyze_type, const string& func_hint,
		waf::size_type node_limit, ostream& os);

int main(int argc, char* argv[]) {
	try {
		return run_task(argc,argv);
	} catch (const exception& e) {
		cerr << "exception uncatched in task: " << e.what() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int run_task(int argc, char* argv[]) {
	// check command line arguments
	if (argc<2)
		return print_help_info();

	// dispatch task
	string task_type=argv[1];
	char** arg_beg=argv+2;
	char** arg_end=argv+argc;
	if (task_type=="-ti")
		return run_ti(arg_beg,arg_end);
	else if (task_type=="-tf")
		return run_tf(arg_beg,arg_end);
	else if (task_type=="-co")  // calculate co-occurrence
		return run_co(arg_beg,arg_end);
	else if (task_type=="-waf")  // calculate waf matrix
		return run_waf(arg_beg,arg_end);
	else if (task_type=="-a")  // calculate affinity matrix
		return run_a(arg_beg,arg_end);
	else if (task_type=="-info")  // analyze information
		return run_info(arg_beg,arg_end);
	else if (task_type=="-h" || task_type=="--help")  // print help information
		return print_help_info();
	else {
		cerr << "error task type: " << task_type << endl;
		return EXIT_FAILURE;
	}
}

template <typename InputIterator>
int run_ti(InputIterator arg_beg, InputIterator arg_end) {
	// init option table
	map<string,vector<string> > option_table;
	string options[]={"-f","-m","-t","-delim","-o","-mo","-to","-log","-s"};
	init_option_table(options,options+sizeof(options)/sizeof(string),option_table);
	fill_option_table(arg_beg,arg_end,option_table);
	string func_hint("term-to-termid: ");
	if (!resolve_unknown_otions(option_table[""],func_hint))
		return EXIT_FAILURE;

	// data source setting
	vector<string> term_file_vec, list_file_vec;
	bool use_term_file_opt=false, use_list_file_opt=false;
	if (!resolve_files_input_option(option_table["-f"],"-f",func_hint,-1,use_term_file_opt,term_file_vec))
		return EXIT_FAILURE;
	if (!resolve_files_input_option(option_table["-m"],"-m",func_hint,-1,use_list_file_opt,list_file_vec))
		return EXIT_FAILURE;
	if (term_file_vec.empty() && list_file_vec.empty()) {
		cerr << func_hint << "no data source" << endl;
		return EXIT_FAILURE;
	}

	// data output setting
	vector<string> term_ofile_vec, list_ofile_vec;
	bool have_term_ofile_opt=false, have_list_ofile_opt=false;
	if (!resolve_files_output_option(option_table["-o"],"-o",func_hint,-1,have_term_ofile_opt,term_ofile_vec))
		return EXIT_FAILURE;
	if (!resolve_files_input_option(option_table["-mo"],"-mo",func_hint,-1,have_list_ofile_opt,list_ofile_vec))
		return EXIT_FAILURE;

	// data input and output verify
	if (term_file_vec.size()!=term_ofile_vec.size()) {
		cerr << func_hint << "argument amount of -o must be exactly the same as that of -f" << endl;
		return EXIT_FAILURE;
	}
	extract_filenames(list_file_vec,term_file_vec);
	extract_filenames(list_ofile_vec,term_ofile_vec);
	if (term_file_vec.size()!=term_ofile_vec.size()) {
		cerr << term_file_vec.size() << " " << term_ofile_vec.size() << endl;
		cerr << func_hint << "file amount in list_files of -mo must be exactly the same as that of -m" << endl;
		return EXIT_FAILURE;
	}
	if (!touch_output_files(term_ofile_vec,func_hint))
		return EXIT_FAILURE;

	// input term info setting
	ifstream fti;
	bool use_term_info=false;
	if (!resolve_term_info_option(option_table["-t"],"-t",func_hint,use_term_info,fti))
		return EXIT_FAILURE;

	// output term info setting
	bool gen_term_info=false;
	string tinfo_file;
	if (!resolve_single_arg_option(option_table["-to"],"-to",func_hint,gen_term_info,tinfo_file))
		return EXIT_FAILURE;
	if (gen_term_info && !touch_output_file(tinfo_file,func_hint))
		return EXIT_FAILURE;

	// delimiters setting
	vector<string> delim_vec;
	bool have_delim_opt=false;
	if (!resolve_multi_arg_option(option_table["-delim"],"-delim",func_hint,0,-1,have_delim_opt,delim_vec))
		return EXIT_FAILURE;
	set<string> delim_set;  // build delim term set
	copy(delim_vec.begin(),delim_vec.end(),inserter(delim_set,delim_set.end()));

	// log setting
    Logger logger;
    Indent indent(func_hint, logger);
	ofstream flog;
	if (!resolve_file_log_option(option_table["-log"],"-log",func_hint,flog,logger))
		return EXIT_FAILURE;

    // separate workmode setting
    bool separate_working=false;
    if (!resolve_none_arg_option(option_table["-s"],"-s",func_hint,separate_working))
        return EXIT_FAILURE;

	// work
	try {
		term_info<string> tinfo;
        term_info<string>::freq_of_id_type freqs;
		if (use_term_info) {  // read input term info
            logger(INFO_) << "reading term information from file..." << endl;
			fti >> tinfo;
			fti.close();  // close term info file
            if (separate_working)  // term_info used only for reference
                freqs=tinfo.freq_of_id();
		}
		for (vector<string>::size_type i=0; i<term_file_vec.size(); ++i) {  // scan term file
			logger(INFO_) << "converting term sequence file '" << term_file_vec[i]
                << "' to termid sequence file '" << term_ofile_vec[i] << "'..." << endl;
			scan_term_file(term_file_vec[i],delim_set,tinfo,term_ofile_vec[i]);
		}
		if (gen_term_info) {  // output term info file
			logger(INFO_) << "writting term information into file '"
                << tinfo_file << "'..." << endl;
            if (separate_working) {  // term_info used only for reference
                for (size_t i=0; i<freqs.size(); ++i)
                    tinfo.freq_of_id(i) -= freqs[i];
            }
			ofstream ftio(tinfo_file.c_str());
			ftio << tinfo;
		}
		logger(INFO_) << "operation finished successfully." << endl;
		return EXIT_SUCCESS;
	} catch (const exception& e) {
		logger(ERROR_) << "operation terminated with exception: " << e.what() << endl;
		return EXIT_FAILURE;
	}
}

template <typename InputIterator>
int run_tf(InputIterator arg_beg, InputIterator arg_end) {
	// init option table
	map<string,vector<string> > option_table;
	string options[]={"-m","-f","-t","-o","-log","-s"};
	init_option_table(options,options+sizeof(options)/sizeof(string),option_table);
	fill_option_table(arg_beg,arg_end,option_table);
	string func_hint("term-frequency: ");
	if (!resolve_unknown_otions(option_table[""],func_hint))
		return EXIT_FAILURE;

	// data source setting
	vector<string> id_file_vec, list_file_vec;
	bool use_id_file_opt=false, use_list_file_opt=false;
	if (!resolve_files_input_option(option_table["-f"],"-f",func_hint,-1,use_id_file_opt,id_file_vec))
		return EXIT_FAILURE;
	if (!resolve_files_input_option(option_table["-m"],"-m",func_hint,-1,use_list_file_opt,list_file_vec))
		return EXIT_FAILURE;
	if (id_file_vec.empty() && list_file_vec.empty()) {
		cerr << func_hint << "no data source" << endl;
		return EXIT_FAILURE;
	}
	extract_filenames(list_file_vec,id_file_vec);

	// term info setting
	ifstream fti;
	bool use_term_info=false;
	if (!resolve_term_info_option(option_table["-t"],"-t",func_hint,use_term_info,fti))
		return EXIT_FAILURE;

	// output stream setting
	ofstream fout;
	ostream* os_ptr=NULL;
	if (!resolve_file_output_option(option_table["-o"],"-o",func_hint,os_ptr,fout))
		return EXIT_FAILURE;

	// log setting
	ofstream flog;
	ostream* olog_ptr=NULL;
	if (!resolve_file_log_option(option_table["-log"],"-log",func_hint,olog_ptr,flog))
		return EXIT_FAILURE;

    // separate workmode setting
    bool separate_working=false;
    if (!resolve_none_arg_option(option_table["-s"],"-s",func_hint,separate_working))
        return EXIT_FAILURE;

	// work
	try {
		term_info<string> tinfo;
		if (use_term_info) {
			log(olog_ptr,func_hint,"reading term information from file...");
			fti >> tinfo;
			fti.close();
            if (separate_working) {  // term_info used only for reference
                term_info<string>::freq_of_id_type& freqs=tinfo.freq_of_id();
                for (size_t i=0; i<freqs.size(); ++i)
                    if (freqs[i]!=0)
                        freqs[i] = 1;
            }
		}
		for (vector<string>::size_type i=0; i<id_file_vec.size(); ++i) {  // scan termid file
			log(olog_ptr,func_hint,
					"extracting term frequency from file '"+id_file_vec[i]+"'...");
			if (use_term_info)
				scan_termid_file(id_file_vec[i],tinfo.freq_identifier(),tinfo);
			else
				scan_termid_file(id_file_vec[i],tinfo);
			log(olog_ptr,func_hint,
					"term amount increased to "+cast<string>(tinfo.termid_size()));
		}
		log(olog_ptr,func_hint,"writting term information to output...");
        if (separate_working) {  // term_info used only for reference
            term_info<string>::freq_of_id_type& freqs=tinfo.freq_of_id();
            for (size_t i=0; i<freqs.size(); ++i)
                if (freqs[i]!=0)
                    freqs[i] -= 1;
        }
		*os_ptr << tinfo;

		log(olog_ptr,func_hint,"operation finished successfully.");
		return EXIT_SUCCESS;
	} catch (const exception& e) {
		report_exception(olog_ptr,func_hint,e.what());
		return EXIT_FAILURE;
	}
}

template <typename InputIterator>
int run_co(InputIterator arg_beg, InputIterator arg_end) {
	// init option table
	map<string,vector<string> > option_table;
	string options[]={"-win","-o","-m","-f","-t","-b","-log"};
	init_option_table(options,options+sizeof(options)/sizeof(string),option_table);
	fill_option_table(arg_beg,arg_end,option_table);
	string func_hint("co-occurrence: ");
	if (!resolve_unknown_otions(option_table[""],func_hint))
		return EXIT_FAILURE;

	// data source setting
	vector<string> id_file_vec, list_file_vec;
	bool use_id_file_opt=false, use_list_file_opt=false;
	if (!resolve_files_input_option(option_table["-f"],"-f",func_hint,-1,use_id_file_opt,id_file_vec))
		return EXIT_FAILURE;
	if (!resolve_files_input_option(option_table["-m"],"-m",func_hint,-1,use_list_file_opt,list_file_vec))
		return EXIT_FAILURE;
	if (id_file_vec.empty() && list_file_vec.empty()) {
		cerr << func_hint << "no data source" << endl;
		return EXIT_FAILURE;
	}
	extract_filenames(list_file_vec,id_file_vec);

	// co-occurrence window size
	bool have_win_opt=false;
	string win_opt;
	if (!resolve_single_arg_option(option_table["-win"],"-win",func_hint,have_win_opt,win_opt))
		return EXIT_FAILURE;
	if (!have_win_opt) {
		cerr << func_hint << "missing option -win" << endl;
		return EXIT_FAILURE;
	}
	waf::size_type co_win=cast<waf::size_type>(win_opt);

	// output stream setting
	ofstream fout;
	ostream* os_ptr=NULL;
	if (!resolve_file_output_option(option_table["-o"],"-o",func_hint,os_ptr,fout))
		return EXIT_FAILURE;

	// term info setting
	term_info<string> tinfo;
	ifstream fti;
	bool use_term_info=true;
	if (!resolve_term_info_option(option_table["-t"],"-t",func_hint,use_term_info,fti))
		return EXIT_FAILURE;

	// background term info setting
	term_info<string> btinfo;
	ifstream fbti;
	bool use_bterm_info=true;
	if (!resolve_term_info_option(option_table["-b"],"-b",func_hint,use_bterm_info,fbti))
		return EXIT_FAILURE;
	if (!use_term_info && use_bterm_info) {
		cerr << func_hint << "-t must be specified when use -b" << endl;
		return EXIT_FAILURE;
	}

	// log setting
	ofstream flog;
	ostream* olog_ptr=NULL;
	if (!resolve_file_log_option(option_table["-log"],"-log",func_hint,olog_ptr,flog))
		return EXIT_FAILURE;

	// work
	try {
		typedef pair<waf::distance_type,waf::size_type> co_type;
		sparse_matrix<co_type> co_mat;
		co_mat.sparse(50,50);
		if (use_term_info) {
			log(olog_ptr,func_hint,"reading term information from file...");
			fti >> tinfo;
			fti.close();
		}
		if (use_bterm_info) {
			log(olog_ptr,func_hint,"reading background term information from file...");
			fbti >> btinfo;
			fbti.close();
		}
		for (vector<string>::size_type i=0; i<id_file_vec.size(); ++i) {  // scan id file
			log(olog_ptr,func_hint,
					"extracting co-occurrence information from file '"+id_file_vec[i]+"'...");
			if (use_term_info) {
				if (use_bterm_info)  // consider background terms
					scan_termid_file(id_file_vec[i],tinfo.freq_identifier(),btinfo.freq_identifier(),co_win,co_mat);
				else
					scan_termid_file(id_file_vec[i],tinfo.freq_identifier(),co_win,co_mat);
			} else
				scan_termid_file(id_file_vec[i],co_win,co_mat);
			log(olog_ptr,func_hint,
					"co-occurrence pair amount increased to "+cast<string>(co_mat.size())+".");
		}
		waf::mean_distance(co_mat.begin(),co_mat.end());

		log(olog_ptr,func_hint,"writting co-occurrence matrix to output...");
		*os_ptr << dynamic_cast<const cross_list<co_type>&>(co_mat);

		log(olog_ptr,func_hint,"operation finished successfully.");
		return EXIT_SUCCESS;
	} catch (const exception& e) {
		report_exception(olog_ptr,func_hint,e.what());
		return EXIT_FAILURE;
	}
}

template <typename InputIterator>
int run_waf(InputIterator arg_beg, InputIterator arg_end) {
	// init option table
	map<string,vector<string> > option_table;
	string options[]={"-p","-f","-t","-o","-log"};
	init_option_table(options,options+sizeof(options)/sizeof(string),option_table);
	fill_option_table(arg_beg,arg_end,option_table);
	string func_hint("word-activation-force: ");
	if (!resolve_unknown_otions(option_table[""],func_hint))
		return EXIT_FAILURE;

	// precision setting
	bool have_prec_opt=false;
	string prec_opt;
	if (!resolve_single_arg_option(option_table["-p"],"-p",func_hint,have_prec_opt,prec_opt))
		return EXIT_FAILURE;
	waf::force_type prec=have_prec_opt ? cast<waf::force_type>(prec_opt) : 0;

	// input stream setting
	ifstream fin;
	istream* is_ptr=NULL;
	if (!resolve_file_input_option(option_table["-f"],"-f",func_hint,is_ptr,fin))
		return EXIT_FAILURE;

	// term information input setting
	term_info<string> tinfo;
	bool have_term_info=false;
	ifstream fti;
	if (!resolve_term_info_option(option_table["-t"],"-t",func_hint,have_term_info,fti))
		return EXIT_FAILURE;
	if (!have_term_info) {
		cerr << func_hint << "missing option -t" << endl;
		return EXIT_FAILURE;
	}

	// output stream setting
	ofstream fout;
	ostream* os_ptr=NULL;
	if (!resolve_file_output_option(option_table["-o"],"-o",func_hint,os_ptr,fout))
		return EXIT_FAILURE;

	// log setting
	ofstream flog;
	ostream* olog_ptr=NULL;
	if (!resolve_file_log_option(option_table["-log"],"-log",func_hint,olog_ptr,flog))
		return EXIT_FAILURE;

	// work
	try {
		typedef pair<waf::distance_type,waf::size_type> co_type;
		cross_list<co_type> co_mat;
		cross_list<waf::force_type> waf_mat;
		log(olog_ptr,func_hint,"reading co-occurrence matrix from input...");
		*is_ptr >> co_mat;

		waf_mat.resize(co_mat.row_size(),co_mat.col_size());
		log(olog_ptr,func_hint,"reading term information from file...");
		fti >> tinfo;

		log(olog_ptr,func_hint,"calculating word-activation-force matrix...");
		waf::word_activation_force(co_mat,tinfo.freq_identifier(),
				tinfo.freq_identifier(),prec,waf_mat);

		log(olog_ptr,func_hint,"writting word-activation-force matrix to output...");
		*os_ptr << waf_mat;

		log(olog_ptr,func_hint,"operation finished successfully.");
		return EXIT_SUCCESS;
	} catch (const exception& e) {
		report_exception(olog_ptr,func_hint,e.what());
		return EXIT_FAILURE;
	}
}

template <typename InputIterator>
int run_a(InputIterator arg_beg, InputIterator arg_end) {
	// init option table
	map<string,vector<string> > option_table;
	string options[]={"-p","-f","-o","-t","-vec","-log"};
	init_option_table(options,options+sizeof(options)/sizeof(string),option_table);
	fill_option_table(arg_beg,arg_end,option_table);
	string func_hint("affinity-measure: ");
	if (!resolve_unknown_otions(option_table[""],func_hint))
		return EXIT_FAILURE;

	// input stream setting
	ifstream fin,fin2;
	istream* is_ptr=NULL, *is_ptr2=NULL;
	bool have_file_opt=false;
	vector<string> file_vec;
	if (!resolve_files_input_option(option_table["-f"],"-f",func_hint,2,have_file_opt,file_vec))
		return EXIT_FAILURE;
	if (!have_file_opt)
		is_ptr = &cin;
	else {
		fin.open(file_vec[0].c_str());
		is_ptr = &fin;
		if (file_vec.size()==2) {
			fin2.open(file_vec[1].c_str());
			is_ptr2 = &fin2;
		}
	}

	// output stream setting
	ofstream fout;
	ostream* os_ptr=NULL;
	if (!resolve_file_output_option(option_table["-o"],"-o",func_hint,os_ptr,fout))
		return EXIT_FAILURE;

	// term info setting
	term_info<string> tinfo;
	ifstream fti;
	bool use_term_info=true;
	if (!resolve_term_info_option(option_table["-t"],"-t",func_hint,use_term_info,fti))
		return EXIT_FAILURE;

	// vector mode setting
	bool to_a_vec=false;
	if (!resolve_none_arg_option(option_table["-vec"],"-vec",func_hint,to_a_vec))
		return EXIT_FAILURE;
	if (to_a_vec && NULL==is_ptr2) {
		cerr << func_hint << "too few input files for option -vec" << endl;
		return EXIT_FAILURE;
	}

	// precision setting
	bool have_prec_opt=false;
	string prec_opt;
	if (!resolve_single_arg_option(option_table["-p"],"-p",func_hint,have_prec_opt,prec_opt))
		return EXIT_FAILURE;
	waf::affinity_type prec=have_prec_opt ? cast<waf::affinity_type>(prec_opt) : 0;

	// log setting
	ofstream flog;
	ostream* olog_ptr=NULL;
	if (!resolve_file_log_option(option_table["-log"],"-log",func_hint,olog_ptr,flog))
		return EXIT_FAILURE;

	// work
	try {
		cross_list<waf::force_type> waf_mat,waf_mat2;
		cross_list<waf::affinity_type> a_mat;
		deque<waf::affinity_type> a_vec;
		if (use_term_info) {
			log(olog_ptr,func_hint,"reading term information from file...");
			fti >> tinfo;
			fti.close();
		}

		log(olog_ptr,func_hint,"reading word-activation-force matrix from input...");
		*is_ptr >> waf_mat;
		a_mat.resize(waf_mat.row_size(),waf_mat.col_size());
		if (NULL==is_ptr2) {
			log(olog_ptr,func_hint,"calculating affinity matrix...");
			if (use_term_info)
				waf::affinity_measure(waf_mat,tinfo.freq_identifier(),prec,a_mat);
			else
				waf::affinity_measure(waf_mat,prec,a_mat);
		} else {  // two waf matrices
			log(olog_ptr,func_hint,"reading word-activation-force matrix from file...");
			*is_ptr2 >> waf_mat2;
			if (waf_mat2.row_size()!=waf_mat.row_size() || 
					waf_mat2.col_size()!=waf_mat.col_size()) {
				cerr << func_hint << "two waf matrices have different dimension" << endl;
				return EXIT_FAILURE;
			}
			if (!to_a_vec) {
				log(olog_ptr,func_hint,"calculating affinity matrix...");
				if (use_term_info)
					waf::affinity_measure(waf_mat,waf_mat2,tinfo.freq_identifier(),prec,a_mat);
				else
					waf::affinity_measure(waf_mat,waf_mat2,prec,a_mat);
			} else {
				log(olog_ptr,func_hint,"calculating affinity vector...");
				a_vec.resize(waf_mat.row_size());
				if (use_term_info)
					waf::affinity_measure(
							waf_mat,waf_mat2,tinfo.freq_identifier(),prec,a_vec.begin());
				else
					waf::affinity_measure(waf_mat,waf_mat2,prec,a_vec.begin());
			}
		}

		log(olog_ptr,func_hint,"writting affinity matrix (vector) to output...");
		if (!to_a_vec)
			*os_ptr << a_mat;
		else
			copy(a_vec.begin(),a_vec.end(),ostream_iterator<waf::affinity_type>(*os_ptr,"\n"));

		log(olog_ptr,func_hint,"operation finished successfully.");
		return EXIT_SUCCESS;
	} catch (const exception& e) {
		report_exception(olog_ptr,func_hint,e.what());
		return EXIT_FAILURE;
	}
}

template <typename InputIterator>
int run_info(InputIterator arg_beg, InputIterator arg_end) {
	// init option table
	map<string,vector<string> > option_table;
	string options[]={"-f","-o","-t","-n","-mat","-type","-log"};
	init_option_table(options,options+sizeof(options)/sizeof(string),option_table);
	fill_option_table(arg_beg,arg_end,option_table);
	string func_hint("information-analysis: ");
	if (!resolve_unknown_otions(option_table[""],func_hint))
		return EXIT_FAILURE;

	// input stream setting
	ifstream fin;
	istream* is_ptr=NULL;
	if (!resolve_file_input_option(option_table["-f"],"-f",func_hint,is_ptr,fin))
		return EXIT_FAILURE;

	// term info setting
	ifstream fti;
	bool use_term_info=true;
	if (!resolve_term_info_option(option_table["-t"],"-t",func_hint,use_term_info,fti))
		return EXIT_FAILURE;

	// output stream setting
	ofstream fout;
	ostream* os_ptr=NULL;
	if (!resolve_file_output_option(option_table["-o"],"-o",func_hint,os_ptr,fout))
		return EXIT_FAILURE;

	// matrix type
	string mat_opts[]={"waf","affinity"};
	string mat_type(mat_opts[0]);  // default to first of list
	bool have_mat_opt=false;
	if (!resolve_single_arg_option(option_table["-mat"],"-mat",func_hint,have_mat_opt,mat_type))
		return EXIT_FAILURE;
	if (have_mat_opt && !resolve_arg_in_table(mat_type,mat_opts,"-mat",func_hint))
		return EXIT_FAILURE;

	// analysis type
	string type_opts[]={"pair","in-link","out-link"};
	string analyze_type(type_opts[0]);  // default to first of list
	bool have_type_opt=false;
	if (!resolve_single_arg_option(option_table["-type"],"-type",func_hint,have_type_opt,analyze_type))
		return EXIT_FAILURE;
	if (have_type_opt && !resolve_arg_in_table(analyze_type,type_opts,"-type",func_hint))
		return EXIT_FAILURE;

	// presenting limit size
	waf::size_type node_limit=numeric_limits<waf::size_type>::max();  // default to non-limit size
	bool have_limit_opt=false;
	string limit_opt;
	if (!resolve_single_arg_option(option_table["-n"],"-n",func_hint,have_limit_opt,limit_opt))
		return EXIT_FAILURE;
	if (have_limit_opt)
		node_limit = cast<waf::size_type>(limit_opt);

	// log setting
	ofstream flog;
	ostream* olog_ptr=NULL;
	if (!resolve_file_log_option(option_table["-log"],"-log",func_hint,olog_ptr,flog))
		return EXIT_FAILURE;

	// work
	try {
		term_info<string> tinfo;
		if (use_term_info) {
			log(olog_ptr,func_hint,"reading term information from file...");
			fti >> tinfo;
			fti.close();
		}

		log(olog_ptr,func_hint,"analyzing information in matrix...");
		if (mat_type=="waf") {
			if (!analyze_process<waf::force_type>(*is_ptr,use_term_info,tinfo,
						analyze_type,func_hint,node_limit,*os_ptr))
				return EXIT_FAILURE;
		} else if (mat_type=="affinity") {
			if (!analyze_process<waf::affinity_type>(*is_ptr,use_term_info,tinfo,
						analyze_type,func_hint,node_limit,*os_ptr))
				return EXIT_FAILURE;
		} else {
			cerr << func_hint << "fail to read matrix" << endl;
			return EXIT_FAILURE;
		}

		log(olog_ptr,func_hint,"operation finished successfully.");
		return EXIT_SUCCESS;
	} catch (const exception& e) {
		report_exception(olog_ptr,func_hint,e.what());
		return EXIT_FAILURE;
	}
}

int print_help_info() {
	ostream& os=cout;
	os << "waf <task type> <task options>                      " << endl;
	os << "    <task type>:                                    " << endl;
	os << "        -a: calculate affinity measure              " << endl;
	os << "        -co: calculate co-occurrence matrix         " << endl;
	os << "        -h, --help: print help information          " << endl;
	os << "        -info: analyze matrix information           " << endl;
	os << "        -waf: calculate waf matrix                  " << endl;
	os << "        -tf: calculate term frequency               " << endl;
	os << "        -ti: convert terms to termid's              " << endl;
	os << "                                                    " << endl;
	os << "    -a <task options>:                              " << endl;
	os << "        -f: set input waf matrix file(s), if not    " << endl;
	os << "            set, read one waf matrix from stdin, if " << endl;
	os << "            set one file, calculate affinity measure" << endl;
	os << "            of this waf matrix, if set two files,   " << endl;
	os << "            calculate affinity measure of the two   " << endl;
	os << "            waf matrix, report error otherwise      " << endl;
	os << "        -log: if set with no arguments, write log   " << endl;
	os << "            to stderr; if set with one filename,    " << endl;
	os << "            write log to that file; if not set, no  " << endl;
	os << "            log generated.                          " << endl;
	os << "        -o: set output affinity matrix file, if not " << endl;
	os << "            set, write affinity matrix to stdout    " << endl;
	os << "        -p: set precision, affinity measure less    " << endl;
	os << "            than it will not be stored. if not set, " << endl;
	os << "            precision default to 0                  " << endl;
	os << "        -t: set input term information file, term   " << endl;
	os << "            with zero frequency will not calculate  " << endl;
	os << "            affinity measure, if not set, all terms " << endl;
	os << "            will calculate affinity measure         " << endl;
	os << "        -vec: if set, calculate affinity measure of " << endl;
	os << "            each pair of same termid's in separate  " << endl;
	os << "            waf matrices. if not set calculate      " << endl;
	os << "            affinity matrix of input waf matrix     " << endl;
	os << "            (matrices)                              " << endl;
	os << "                                                    " << endl;
	os << "    -info <task options>:                           " << endl;
	os << "        -f: set input matrix file, if not set, read " << endl;
	os << "            matrix from stdin                       " << endl;
	os << "        -log: if set with no arguments, write log   " << endl;
	os << "            to stderr; if set with one filename,    " << endl;
	os << "            write log to that file; if not set, no  " << endl;
	os << "            log generated.                          " << endl;
	os << "        -mat: set matrix type:                      " << endl;
	os << "            'affinity': treat input as affinity     " << endl;
	os << "                matrix                              " << endl;
	os << "            'waf': treat input as waf matrix        " << endl;
	os << "        -n: set present number 'num' of results. if " << endl;
	os << "            '-type' is 'pair', present highest num  " << endl;
	os << "            node values to output, if '-type' is    " << endl;
	os << "            'in-link', present highest num node     " << endl;
	os << "            values of each column to output, if     " << endl;
	os << "            '-type' is 'out-link', present highest  " << endl;
	os << "            num node values of each row to output,  " << endl;
	os << "            if not set, present all node values to  " << endl;
	os << "            output.                                 " << endl;
	os << "        -o: set info output file, if not set, write " << endl;
	os << "            info to stdout                          " << endl;
	os << "        -t: set input term information file, term   " << endl;
	os << "            with zero frequency will not analyze    " << endl;
	os << "            information, and output will be <term,  " << endl;
	os << "            term, value> tuple. if not set, all     " << endl;
	os << "            nodes in matrix will analyze information" << endl;
	os << "            and output will be <termid,termid,value>" << endl;
	os << "            tuple                                   " << endl;
	os << "        -type: set analyze type:                    " << endl;
	os << "            'in-link': sort nodes of each column in " << endl;
	os << "                desc order                          " << endl;
	os << "            'out-link': sort nodes of each row in   " << endl;
	os << "                desc order                          " << endl;
	os << "            'pair'(default): sort all nodes in      " << endl;
	os << "                matrix in desc order                " << endl;
	os << "                                                    " << endl;
	os << "    -co <task options>:                             " << endl;
	os << "        -f: set input termid file, delimited with   " << endl;
	os << "            space or '\\n', more than one files is  " << endl;
	os << "            also ok                                 " << endl;
	os << "        -log: if set with no arguments, write log   " << endl;
	os << "            to stderr; if set with one filename,    " << endl;
	os << "            write log to that file; if not set, no  " << endl;
	os << "            log generated.                          " << endl;
	os << "        -m: set input list_file, which list each    " << endl;
	os << "            termid file in a line, more than one    " << endl;
	os << "            list_file(s) is also ok                 " << endl;
	os << "        -o: set output co-occurrence matrix file, if" << endl;
	os << "            not set, write co-occurrence matrix to  " << endl;
	os << "            stdout                                  " << endl;
	os << "        -t: set input term information file, term   " << endl;
	os << "            with zero frequency will not calculate  " << endl;
	os << "            co-occurrence, if not set, all terms    " << endl;
	os << "            will calculate co-occurrence            " << endl;
	os << "        -b: set background term information, term   " << endl;
	os << "            with non-zero frequency will be treated " << endl;
	os << "            as background term, work together with  " << endl;
	os << "            -t                                      " << endl;
	os << "        -win: set co-occurrence window size co_win, " << endl;
	os << "            then max co-occurrence pair distance is " << endl;
	os << "            co_win-1                                " << endl;
	os << "                                                    " << endl;
	os << "    -tf <task options>:                             " << endl;
	os << "        -f: set input termid file, delimited with   " << endl;
	os << "            space or '\\n', more than one files is  " << endl;
	os << "            also ok                                 " << endl;
	os << "        -log: if set with no arguments, write log   " << endl;
	os << "            to stderr; if set with one filename,    " << endl;
	os << "            write log to that file; if not set, no  " << endl;
	os << "            log generated.                          " << endl;
	os << "        -m: set input list_file, which list each    " << endl;
	os << "            termid file in a line, more than one    " << endl;
	os << "            list_file(s) is also ok                 " << endl;
	os << "        -o: set output term information file, if not" << endl;
	os << "            set, write term information to stdout   " << endl;
	os << "        -t: set input term information file, term   " << endl;
	os << "            with zero frequency will not calculate  " << endl;
	os << "            term information, if not set, all terms " << endl;
	os << "            will calculate term information         " << endl;
	os << "        -s: specify working mode that only calculate" << endl;
	os << "            input files' term frequency, no conside-" << endl;
	os << "            ring the -t's frequency information.    " << endl;
	os << "                                                    " << endl;
	os << "    -ti <task options>:                             " << endl;
	os << "        -delim: set delimiter term(s), which will be" << endl;
	os << "            treated as end of one term list unit    " << endl;
	os << "        -f: set input term file, delimited with     " << endl;
	os << "            space or '\\n'. more than one files is  " << endl;
	os << "            also ok                                 " << endl;
	os << "        -log: if set with no arguments, write log   " << endl;
	os << "            to stderr; if set with one filename,    " << endl;
	os << "            write log to that file; if not set, no  " << endl;
	os << "            log generated.                          " << endl;
	os << "        -m: set input list file which lists each    " << endl;
	os << "            term file in one line, more than one    " << endl;
	os << "            list file is also ok                    " << endl;
	os << "        -mo: set output list file which lists each  " << endl;
	os << "            termid file in one line, for each term  " << endl;
	os << "            file specified by the -m option to      " << endl;
	os << "            store its termid file. more than one    " << endl;
	os << "            list files is also ok                   " << endl;
	os << "        -t: set input term information file, term   " << endl;
	os << "            information of the imput term list file " << endl;
	os << "            will be appended to the input term      " << endl;
	os << "            term information                        " << endl;
	os << "        -o: set output termid file for term file    " << endl;
	os << "            specified by the -f option, its argument" << endl;
	os << "            amount must be exactly the same as that " << endl;
	os << "            of -f                                   " << endl;
	os << "        -to: set output term information file, term " << endl;
	os << "            information of -f, -m argument and -t   " << endl;
	os << "            argument add up will be output into it  " << endl;
	os << "        -s: specify working mode that only calculate" << endl;
	os << "            input files' term frequency, no conside-" << endl;
	os << "            ring the -t's frequency information.    " << endl;
	os << "                                                    " << endl;
	os << "    -waf <task options>:                            " << endl;
	os << "        -f: set input co-occurrence matrix file, if " << endl;
	os << "            not set, read co-occurrence matrix from " << endl;
	os << "            stdin                                   " << endl;
	os << "        -log: if set with no arguments, write log   " << endl;
	os << "            to stderr; if set with one filename,    " << endl;
	os << "            write log to that file; if not set, no  " << endl;
	os << "            log generated.                          " << endl;
	os << "        -o: set output waf matrix file, if not set, " << endl;
	os << "            write waf matrix to stdout              " << endl;
	os << "        -p: set precision, waf value less than it   " << endl;
	os << "            will not be stored. if not set,         " << endl;
	os << "            precision default to 0                  " << endl;
	os << "        -t: set input term information file, term   " << endl;
	os << "            with zero frequency will not calculate  " << endl;
	os << "            waf value, other terms' frequencies will" << endl;
	os << "            be used to calculate waf value.         " << endl;
	os << "                                                    " << endl;
	return EXIT_SUCCESS;
}

template <typename InputIterator>
void init_option_table(
		InputIterator opt_beg, InputIterator opt_end,
		map<string,vector<string> >& option_table) {
	option_table.clear();
	for (; opt_beg!=opt_end; ++opt_beg)
		option_table[*opt_beg].clear();
}

template <typename InputIterator>
void fill_option_table(
		InputIterator arg_beg, InputIterator arg_end,
		map<string,vector<string> >& option_table) {
	while (arg_beg!=arg_end) {
		string arg=*arg_beg;
		if (option_table.count(arg)) {
			vector<string>& v=option_table[arg];
			if (v.empty())
				v.push_back(arg);
			for (++arg_beg; arg_beg!=arg_end &&
					(arg=*arg_beg).substr(0,1)!="-"; ++arg_beg)
				v.push_back(arg);
		} else {
			option_table[""].push_back(arg);
			++arg_beg;
		}
	}
}

bool resolve_unknown_otions(
		const vector<string>& args, const string& func_hint) {
	if (args.empty())
		return true;
	cerr << func_hint << "unknown option(s): ";
	copy(args.begin(),args.end(), ostream_iterator<string>(cerr," "));
	cerr << endl;
	return false;
}

bool resolve_file_input_option(
		const vector<string>& args, const string& opt_hint,
		const string& func_hint, istream*& is_ptr,
		ifstream& fin, istream& is_dft) {
	is_ptr = NULL;  // init to NULL
	bool exist=false;
	string file;
	if (!resolve_single_arg_option(args,opt_hint,func_hint,exist,file))
		return false;
	if (!exist) {  // arg not exist, use defalut istream
		is_ptr = &is_dft;
		return true;
	}
	if (!touch_input_file(file,func_hint))  // use file stream
		return false;
	fin.close(); fin.clear();
	fin.open(file.c_str(), ios::binary);
	is_ptr = &fin;
	return true;
}

bool resolve_files_input_option(
		const vector<string>& args, const string& opt_hint,
		const string& func_hint, int limit,
		bool& exist, vector<string>& file_vec) {
	if (!resolve_multi_arg_option(args,opt_hint,func_hint,0,limit,exist,file_vec))
		return false;
	if (!exist)  // arg not exist, return true
		return true;
	return touch_input_files(file_vec,func_hint);
}

bool resolve_file_output_option(
		const vector<string>& args, const string& opt_hint,
		const string& func_hint, ostream*& os_ptr,
		ofstream& fout, ostream& os_dft) {
	os_ptr = NULL;  // init to NULL
	bool exist=false;
	string file;
	if (!resolve_single_arg_option(args,opt_hint,func_hint,exist,file))
		return false;
	if (!exist) {  // arg not exist, use default ostream
		os_ptr = &os_dft;
		return true;
	}
	if (!touch_output_file(file,func_hint))  // use file stream
		return false;
	fout.close(); fout.clear();
	fout.open(file.c_str(),ios::binary);
	os_ptr = &fout;
	return true;
}

bool resolve_files_output_option(
		const vector<string>& args, const string& opt_hint,
		const string& func_hint, int limit,
		bool& exist, vector<string>& file_vec) {
	if (!resolve_multi_arg_option(args,opt_hint,func_hint,0,limit,exist,file_vec))
		return false;
	if (!exist)  // arg not exist, return true
		return true;
	return touch_output_files(file_vec,func_hint);
}

bool resolve_file_log_option(
		const vector<string>& args, const string& opt_hint,
		const string& func_hint, ostream*& os_ptr,
		ofstream& flog, ostream& os_dft) {
	os_ptr = NULL;  // init to NULL
	bool exist=false;
	vector<string> files;
	if (!resolve_multi_arg_option(args,opt_hint,func_hint,0,1,exist,files))
		return false;
	if (!exist)  // arg not exist, no log
		return true;
	if (files.empty()) {  // zero arguments, use default stream
		os_ptr = &os_dft;
		return true;
	}
	if (!touch_output_file(files.at(0),func_hint))  // use file stream
		return false;
	flog.close(); flog.clear();
	flog.open(files.at(0).c_str(),ios::binary);
	os_ptr = &flog;
	return true;
}

bool resolve_file_log_option(
		const vector<string>& args, const string& opt_hint,
		const string& func_hint, ofstream& flog, Logger& logger) {
	bool exist=false;
	vector<string> files;
	if (!resolve_multi_arg_option(args,opt_hint,func_hint,0,1,exist,files))
		return false;
	if (!exist)  // arg not exist, no log
		return true;
	if (files.empty()) {  // zero arguments, use default stream
        logger.attach_sink(clog, INFO_);
		return true;
	}
	if (!touch_output_file(files.at(0),func_hint))  // use file stream
		return false;
	flog.close(); flog.clear();
	flog.open(files.at(0).c_str(),ios::binary);
    logger.attach_sink(flog, INFO_);
	return true;
}

bool resolve_term_info_option(
		const vector<string>& args, const string& opt_hint,
		const string& func_hint, bool& exist, ifstream& fs) {
	string file;
	if (!resolve_single_arg_option(args,opt_hint,func_hint,exist,file))
		return false;
	if (!exist)  // arg not exist, return true
		return true;
	fs.close(); fs.clear();
	fs.open(file.c_str(),ios::binary);
	if (!fs) {
		cerr << func_hint << "cannot open term info file '"
			<< file << "'" << endl;
		return false;
	}
	return true;
}

bool resolve_multi_arg_option(
		const vector<string>& args, const string& opt_hint,
		const string& func_hint, size_t argc_min, int argc_max,
		bool& exist, vector<string>& arg_vec) {
	if (args.empty()) {
		exist = false;
		return true;
	}
	exist = true;
	if (args.size()-1 < argc_min) {
		cerr << func_hint << "too few arguments for option " << opt_hint << endl;
		return false;
	}
	if (args.size()-1 > static_cast<size_t>(argc_max)) {
		cerr << func_hint << "too many arguments for option " << opt_hint << endl;
		return false;
	}
	for (vector<string>::size_type i=1; i<args.size(); ++i)
		arg_vec.push_back(args[i]);
	return true;
}

bool resolve_single_arg_option(
		const vector<string>& args, const string& opt_hint,
		const string& func_hint, bool& exist, string& arg) {
	vector<string> arg_vec;
	if(!resolve_multi_arg_option(args,opt_hint,func_hint,1,1,exist,arg_vec))
		return false;
	if (!exist)
		return true;
	arg = arg_vec[0];
	return true;
}

bool resolve_none_arg_option(
		const vector<string>& args, const string& opt_hint,
		const string& func_hint, bool& exist) {
	vector<string> arg_vec;
	return resolve_multi_arg_option(args,opt_hint,func_hint,0,0,exist,arg_vec);
}

template <typename T, size_t N>
bool resolve_arg_in_table(
		const string& arg, const T (&table)[N],
		const string& opt_hint, const string& func_hint) {
	if (find(table,table+N,arg)==table+N) {
		cerr << func_hint << "invalid argument for option " << opt_hint << endl;
		return false;
	}
	return true;
}

bool touch_input_file(const string& filename, const string& func_hint) {
	ifstream fin(filename.c_str(),ios::binary);
	if (fin)
		return true;
	cerr << func_hint << "cannot open input file '"
		<< filename << "'" << endl;
	return false;
}

bool touch_output_file(const string& filename, const string& func_hint) {
	ofstream fout(filename.c_str(),ios::binary);
	if (fout)
		return true;
	cerr << func_hint << "cannot open output file '"
		<< filename << "'" << endl;
	return false;
}

bool touch_input_files(const vector<string>& filenames, const string& func_hint) {
	for (vector<string>::size_type i=0; i<filenames.size(); ++i)
		if (!touch_input_file(filenames[i],func_hint))
			return false;
	return true;
}

bool touch_output_files(const vector<string>& filenames, const string& func_hint) {
	for (vector<string>::size_type i=0; i<filenames.size(); ++i)
		if (!touch_output_file(filenames[i],func_hint))
			return false;
	return true;
}

void extract_filenames(const string& list_file, vector<string>& file_vec) {
	ifstream fin(list_file.c_str(),ios::binary);
	string text;
	while (getline(fin,text)) {
		if (strip(text)!="")
			file_vec.push_back(text);
	}
}

void extract_filenames(const vector<string>& list_file_vec, vector<string>& file_vec) {
	for (vector<string>::size_type i=0; i<list_file_vec.size(); ++i)
		extract_filenames(list_file_vec[i],file_vec);
}

string current_time() {
	time_t t=time(NULL);
	char strt[100]={0};
	strftime(strt,sizeof(strt),"%Y.%m.%d %H:%M:%S",localtime(&t));
	return string(strt);
}

void log(ostream* os_ptr, const string& func_hint, const string& message) {
	if (os_ptr)
		*os_ptr << current_time() << " " << func_hint << message << endl;
}

void report_exception(ostream* os_ptr, const string& func_hint, const string& ex_msg) {
	log(os_ptr,func_hint,"operation terminated with exception: "+ex_msg);
	cerr << func_hint << "exception occured: " << ex_msg << endl;
}

template <typename T1, typename T2>
ostream& operator << (ostream& os, const pair<T1,T2>& p) {
	return os << "(" << p.first << "," << p.second << ")";
}

template <typename T1, typename T2>
istream& operator >> (istream& is, pair<T1,T2>& p) {
	string text;
	getline(is,text,'(');
	getline(is,text,',');
	stringstream(text) >> p.first;
	getline(is,text,')');
	stringstream(text) >> p.second;
	return is;
}

template <typename T>
ostream& operator << (ostream& os, const node_info<T>& ni) {
	return os << ni.row << "\t" << ni.col << "\t" << ni.value;
}

template <typename ToType, typename FromType>
ToType cast(const FromType& from) {
	ToType to=ToType(); stringstream ss;
	ss << from; ss >> to; return to;
}

string& strip(string& s) {
	string::iterator it=s.begin();
	for (; it!=s.end() && isspace(*it); ++it);  // strip prefix
	copy(it,s.end(),s.begin());
	s.resize(s.size()-(it-s.begin()));
	for (it=s.end(); it!=s.begin() && isspace(*(it-1)); --it);  // strip suffix
	s.resize(it-s.begin());
	return s;
}

template <typename Iterator, typename OutputIterator>
void extract_node_info(Iterator first, Iterator last, OutputIterator out) {
	for (; first!=last; ++first, ++out)
		*out = make_node_info(first.row_index(),first.col_index(),*first);
}

template <typename T>
void analyze_outlink(const cross_list<T>& mat,
		deque<deque<node_info<T> > >& node_vecs) {
	typedef typename cross_list<T>::size_type size_type;
	node_vecs.resize(mat.row_size()); {
		for (size_type r=0; r<mat.row_size(); ++r) {
			node_vecs[r].clear();  // sort nodes of each row by value desc
			extract_node_info(mat.cbegin_of_row(r),mat.cend_of_row(r),
					back_inserter(node_vecs[r]));
			sort(node_vecs[r].begin(),node_vecs[r].end(),
					node_info<T>::value_greater);
		}
	}
}


template <typename T>
void analyze_inlink(const cross_list<T>& mat,
		deque<deque<node_info<T> > >& node_vecs) {
	typedef typename cross_list<T>::size_type size_type;
	node_vecs.resize(mat.col_size()); {
		for (size_type c=0; c<mat.col_size(); ++c) {
			node_vecs[c].clear();  // sort nodes of each column by value desc
			extract_node_info(mat.cbegin_of_col(c),mat.cend_of_col(c),
					back_inserter(node_vecs[c]));
			sort(node_vecs[c].begin(),node_vecs[c].end(),
					node_info<T>::value_greater);
		}
	}
}


template <typename T>
void analyze_pair(const cross_list<T>& mat,
		deque<node_info<T> >& node_vec) {
	node_vec.clear();
	extract_node_info(mat.cbegin(),mat.cend(),back_inserter(node_vec));
	sort(node_vec.begin(),node_vec.end(),
			node_info<T>::value_greater);  // sort nodes by value desc
}

template <typename T>
bool analyze_matrix(const cross_list<T>& mat, const string& type,
		const string& func_hint, waf::size_type node_limit,
		deque<deque<node_info<T> > >& node_vecs) {
	typedef deque<deque<node_info<T> > > node_vecs_type;
	node_vecs.resize(1);
	if ("pair"==type)
		analyze_pair(mat,node_vecs.front());
	else if ("in-link"==type)
		analyze_inlink(mat,node_vecs);
	else if ("out-link"==type)
		analyze_outlink(mat,node_vecs);
	else {
		cerr << func_hint << "invalid analyze type '" << type << "'" << endl;
		return false;
	}

	// match node limit size
	for (typename node_vecs_type::iterator iter=node_vecs.begin();
			iter!=node_vecs.end(); ++iter) {
		if (iter->size()>node_limit)
			iter->resize(node_limit);
	}
	return true;
}

template <typename T, typename Term>
bool analyze_process(
		istream& is, bool use_term_info,term_info<Term>& tinfo,
		const string& analyze_type, const string& func_hint,
		waf::size_type node_limit, ostream& os) {
	cross_list<T> mat;  // read matrix
	is >> mat;

	if (use_term_info) {  // use term info to cut unconcerned nodes
		waf::termid_type termid_limit=
			min(tinfo.termid_size(),min(mat.row_size(),mat.col_size()));
		for (waf::termid_type termid=0; termid<termid_limit; ++termid) {
			if (tinfo.freq_of_id(termid)>0)
				continue;
			mat.erase(mat.begin_of_row(termid),mat.end_of_row(termid));
			mat.erase(mat.begin_of_col(termid),mat.end_of_col(termid));
		}
	}

	typedef deque<deque<node_info<T> > > node_vecs_type;
	node_vecs_type node_vecs;  // analyze matrix nodes
	if (!analyze_matrix(mat,analyze_type,func_hint,node_limit,node_vecs))
		return EXIT_FAILURE;

	if (use_term_info) {  // output info to output stream
		os << "<from_term>\t<to_term>\t<value>" << endl;
		for (typename node_vecs_type::iterator iter=node_vecs.begin();
				iter!=node_vecs.end(); ++iter) {
			for (typename deque<node_info<T> >::iterator
					it=iter->begin(); it!=iter->end(); ++it) {
				os << tinfo.term_of_id(it->row) << "\t"
					<< tinfo.term_of_id(it->col) << "\t"
					<< it->value << endl;
			}
		}
	} else {
		os << "<from_termid>\t<to_termid>\t<value>" << endl;
		for (typename node_vecs_type::iterator iter=node_vecs.begin();
				iter!=node_vecs.end(); ++iter) {
			copy(iter->begin(),iter->end(),
					ostream_iterator<node_info<T> >(os,"\n"));
		}
	}
	return EXIT_SUCCESS;
}
