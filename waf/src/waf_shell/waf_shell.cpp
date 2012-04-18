// waf_shell.cpp
#include "waf_shell.h"

#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <string>
#include <set>
#include <stdexcept>
#include <vector>

#include "../configure/configure.h"
#include "../logging/logging.h"
#include "../timing/timing.h"

using namespace logging;
using namespace std;

// internal facility
// =============================================================================
namespace waf_internal {

waf::termid_type avail_termid(
        const waf::TermSet& termset, waf::termid_type feed = 0) {
    while (termset.search(feed)) ++feed;
    return feed;
}

bool resolve_log_option(ofstream& flog) {
    if (!configure::search("log")) {  // no log option, not show common log
        standard_logger().level_of_sink(clog) = ERROR_;
    } else if (""==configure::get<string>("log")) {  // log to stderr
        standard_logger().level_of_sink(clog) = INFO_;
    } else {  // log to file
        string log_file = configure::get<string>("log");
        flog.close();
        flog.clear();
        flog.open(log_file.c_str());
        if (!flog) {
            cerr << "fail to open log file '" << log_file << "'" << endl;
            return false;
        }
        standard_logger().attach_sink(flog, INFO_);
        standard_logger().level_of_sink(clog) = ERROR_;
    }
    return true;
}

}  // namespace waf_internal

// =============================================================================
int run_term_to_termid(int argc, char* argv[]) {
    configure::read(argc, argv);
    if (configure::search("config-file")) {
        configure::read(configure::get<string>("config-file"));
        configure::read(argc, argv);
    }

    if (!configure::search("term-file")) {
        cerr << "option '--term-file' needed"
            << " to specify term file(s) (input)" << endl;
        return -1;
    }
    vector<string> term_files = configure::get<vector<string> >("term-file");
    if (term_files.empty()) {
        cerr << "need at least one file for option '--term-file'" << endl;
        return -1;
    }

    if (!configure::search("termid-file")) {
        cerr << "option '--termid-file' needed"
            << " to specify termid file(s) (output)" << endl;
        return -1;
    }
    vector<string> termid_files =
        configure::get<vector<string> >("termid-file");
    if (termid_files.empty()) {
        cerr << "need at least one file for option '--termid-file'" << endl;
        return -1;
    }
    if (termid_files.size()!=1 && termid_files.size()!=term_files.size()) {
        cerr << "option '--termid-file' should specify either one file"
            << " or the same count files as '--term-file', but" << endl;
        cerr << "    --term-file: " << term_files.size() << " file(s)" << endl;
        cerr << "    --termid-file: "
            << termid_files.size() << " file(s)" << endl;
        return -1;
    }

    vector<string> delimiters = 
        configure::default_get<vector<string> >("delimiters", vector<string>());
    set<string> delim_set;
    copy(delimiters.begin(), delimiters.end(),
            inserter(delim_set, delim_set.end()));

    ofstream flog;
    if (!waf_internal::resolve_log_option(flog)) return -1;

    string init_term_dict_file =
        configure::default_get<string>("init-term-dict", "");
    string term_dict_file =
        configure::default_get<string>("term-dict", "");

    try {
        timing::start();
        log(INFO_) << "operation started" << endl;

        waf::TermSet termset;
        if (init_term_dict_file!="") {  // initialize term set
            log(INFO_) << "initializing term dict from file '"
                << init_term_dict_file << "'" << endl;
            ifstream fin(init_term_dict_file.c_str());
            if (fin) {
                fin >> termset;
            } else {
                log(ERROR_) << "fail to open term dict file '"
                    << init_term_dict_file << "'" << endl;
            }
        }

        ifstream fin;
        ofstream fout;
        for (size_t i=0; i<term_files.size(); ++i) {

            // open term file
            fin.close();
            fin.clear();
            fin.open(term_files[i].c_str());
            if (!fin) {
                log(ERROR_) << "fail to open term file '"
                    << term_files[i] << "', skipping" << endl;
                continue;
            }

            // open termid file
            if (term_files.size() > 1) {
                fout.close();
                fout.clear();
                fout.open(termid_files[i].c_str());
            } else if (!fout.is_open()) {  // only one termid file
                fout.open(termid_files.front().c_str());
            }
            if (!fout) {
                log(ERROR_) << "fail to open termid file '"
                    << termid_files[i] << "', skipping" << endl;
                continue;
            }

            // convertion
            log(INFO_) << "converting from '" << term_files[i] << "' to '"
                << (termid_files.size()>1 ? termid_files[i] : termid_files[0])
                << "'" << endl;
            string term;
            waf::termid_type termid=0;
            while (fin >> term) {
                if (delim_set.count(term)) {
                    fout << static_cast<int>(waf::delim_termid) << "\n";
                    continue;
                }
                if (!termset.search(term)) {
                    termid = waf_internal::avail_termid(termset, termid);
                    termset.insert(termid, term);
                }
                fout << termset[term] << " ";
            }
        }

        if (term_dict_file!="") {  // output term set
            log(INFO_) << "writting term dict to file '"
                << term_dict_file << "'" << endl;
            ofstream fout(term_dict_file.c_str());
            if (fout) {
                fout << termset;
            } else {
                log(ERROR_) << "fail to open term dict file '"
                    << term_dict_file << "'" << endl;
            }
        }

        log(INFO_) << "operation finished successfully, cost "
            << timing::duration() << " seconds" << endl;
        return 0;

    } catch (const std::exception& e) {
		log(ERROR_)
            << "operation terminated with exception: " << e.what()
            << ", cost " << timing::duration() << " seconds" << endl;
        return -1;
    }
}

int run_termid_frequency(int argc, char* argv[]) {
    configure::read(argc, argv);
    if (configure::search("config-file")) {
        configure::read(configure::get<string>("config-file"));
        configure::read(argc, argv);
    }

    if (!configure::search("termid-file")) {
        cerr << "option '--termid-file' needed"
            << " to specify termid file(s) (input)" << endl;
        return -1;
    }
    vector<string> termid_files =
        configure::get<vector<string> >("termid-file");
    if (termid_files.empty()) {
        cerr << "need at least one file for option '--termid-file'" << endl;
        return -1;
    }

    if (!configure::search("freq-vector")) {
        cerr << "option '--freq-vector' needed"
            << " to specify termid frequency vector file (output)" << endl;
        return -1;
    }
    string freq_vec_file = configure::get<string>("freq-vector");

    ofstream flog;
    if (!waf_internal::resolve_log_option(flog)) return -1;

    string term_dict_file = configure::default_get<string>("term-dict", "");
    string init_freq_vec_file =
        configure::default_get<string>("init-freq-vector", "");

    try {
        timing::start();
        log(INFO_) << "operation started" << endl;

        waf::TermSet termset;
        if (term_dict_file!="") {  // initialize term set
            log(INFO_) << "reading term dict from file '"
                << term_dict_file << "'" << endl;
            ifstream fin(term_dict_file.c_str());
            if (fin) {
                fin >> termset;
            } else {
                log(ERROR_) << "fail to open term dict file '"
                    << term_dict_file << "'" << endl;
            }
        }

        waf::FreqVector freqvec;
        if (init_freq_vec_file!="") {
            log(INFO_) << "initializing frequency vector from file '"
                << init_freq_vec_file << "'" << endl;
            ifstream fin(init_freq_vec_file.c_str());
            if (fin) {
                fin >> freqvec;
            } else {
                log(ERROR_) << "fail to open frequency file '"
                    << init_freq_vec_file << "'" << endl;
            }
        }

        ifstream fin;
        for (size_t i=0; i<termid_files.size(); ++i) {
            fin.close();
            fin.clear();
            fin.open(termid_files[i].c_str());
            if (!fin) {
                log(ERROR_) << "fail to open termid file '"
                    << termid_files[i] << "', skipping" << endl;
                continue;
            }
            
            log(INFO_) << "counting file '" << termid_files[i] << "'" << endl;
            waf::termid_type termid;
            while (fin >> termid) {
                if (waf::delim_termid == termid) continue;
                if (!term_dict_file.empty() && !termset.search(termid))
                    continue;  // filter by term dict
                ++freqvec[termid];
            }
        }

        log(INFO_) << "writting frequency vector into file '"
            << freq_vec_file << "'" << endl;
        ofstream fout(freq_vec_file.c_str());
        if (!fout) {
            log(ERROR_) << "fail to open frequency vector file '"
                << freq_vec_file << "'" << endl;
            return -1;
        }

        log(INFO_) << "operation finished successfully, cost "
            << timing::duration() << " seconds" << endl;
        return 0;

    } catch (const std::exception& e) {
		log(ERROR_)
            << "operation terminated with exception: " << e.what()
            << ", cost " << timing::duration() << " seconds" << endl;
        return -1;
    }
}

int run_co_occurrence(int argc, char* argv[]) {
    configure::read(argc, argv);
    if (configure::search("config-file")) {
        configure::read("config-file");
        configure::read(argc, argv);
    }

    if (!configure::search("termid-file")) {
        cerr << "option '--termid-file' needed"
            << " to specify termid file(s) (input)" << endl;
        return -1;
    }
    vector<string> termid_files =
        configure::get<vector<string> >("termid-file");
    if (termid_files.empty()) {
        cerr << "need at least one file for option '--termid-file'" << endl;
        return -1;
    }

    if (!configure::search("window-size")) {
        cerr << "option '--window-size' needed"
            << " to specify co-occurrence window size" << endl;
        return -1;
    }
    waf::size_type window_size = configure::get<waf::size_type>("window-size");
    if (window_size < 2) {
        cerr << "option '--window-size' should be at least 2" << endl;
        return -1;
    }

    if (!configure::search("co-matrix")) {
        cerr << "option '--co-matrix' needed"
            << " to specify co-occurrence matrix (output)" << endl;
        return -1;
    }
    string co_matrix_file = configure::get<string>("co-matrix");

    string left_term_dict_file, right_term_dict_file, term_dict_file;
    term_dict_file =
        configure::default_get<string>("term-dict", "");  // default option
    left_term_dict_file =
        configure::default_get<string>("left-term-dict", term_dict_file);
    right_term_dict_file =
        configure::default_get<string>("right-term-dict", term_dict_file);

    ofstream flog;
    if (!waf_internal::resolve_log_option(flog)) return -1;

    try {
        timing::start();
        log(INFO_) << "operation started" << endl;

        ofstream fout(co_matrix_file.c_str());
        if (!fout) {
            log(ERROR_) << "fail to open co occurrence matrix file '"
                << co_matrix_file << "'" << endl;
            return -1;
        }

        waf::Care care_left, care_right;
        waf::TermSet termset_left, termset_right;
        if (left_term_dict_file!="") {
            ifstream fin(left_term_dict_file.c_str());
            if (!fin) {
                log(ERROR_) << "fail to open left term dict file '"
                    << left_term_dict_file << "'" << endl;
                return -1;
            }
            fin >> termset_left;
            care_left = waf::care_in(termset_left);
        } else {
            care_left = waf::care_all();
        }
        if (right_term_dict_file!="") {
            ifstream fin(right_term_dict_file.c_str());
            if (!fin) {
                log(ERROR_) << "fail to open right term dict file '"
                    << right_term_dict_file << "'" << endl;
                return -1;
            }
            fin >> termset_right;
            care_right = waf::care_in(termset_right);
        } else {
            care_right = waf::care_all();
        }

        SparseMatrix<waf::cooccur_type> co_mat;
        ifstream fin;
        for (size_t i=0; i<termid_files.size(); ++i) {
            fin.close();
            fin.clear();
            fin.open(termid_files[i].c_str());
            if (!fin) {
                log(ERROR_) << "fail to open termid file '"
                    << termid_files[i] << "', skipping" << endl;
                continue;
            }

            log(INFO_) << "counting co-occurrence in '"
                << termid_files[i] << "'" << endl;
            istream_iterator<waf::termid_type> is_iter(fin), is_end;
            waf::co_occurrence(is_iter, is_end,
                    care_left, care_right, window_size, co_mat);
        }

        log(INFO_) << "post processing co-occurrence matrix" << endl;
        waf::mean_distance(co_mat.begin(), co_mat.end());

        log(INFO_) << "writting co occurrence matrix into '"
            << co_matrix_file << "'" << endl;
        fout << co_mat;

        log(INFO_) << "operation finished successfully, cost "
            << timing::duration() << " seconds" << endl;
        return 0;

    } catch (const std::exception& e) {
		log(ERROR_)
            << "operation terminated with exception: " << e.what()
            << ", cost " << timing::duration() << " seconds" << endl;
        return -1;
    }
}

int run_word_activation_force(int argc, char* argv[]) {
    configure::read(argc, argv);
    if (configure::search("config-file")) {
        configure::read("config-file");
        configure::read(argc, argv);
    }

    if (!configure::search("co-matrix")) {
        cerr << "option '--co-matrix' needed"
            << " to specify co-occurrence matrix (input)" << endl;
        return -1;
    }
    string co_matrix_file = configure::get<string>("co-matrix");

    if (!configure::search("waf-matrix")) {
        cerr << "option '--waf-matrix' needed"
            << " to specify word-activation-force matrix (output)" << endl;
        return -1;
    }
    string waf_matrix_file = configure::get<string>("waf-matrix");

    if (!configure::search("freq-vector")) {
        cerr << "option '--freq-vector' needed"
            << " to specify frequency vector (input)" << endl;
        return -1;
    }
    string freq_vec_file = configure::get<string>("freq-vector");

    string left_term_dict_file, right_term_dict_file, term_dict_file;
    term_dict_file =
        configure::default_get<string>("term-dict", "");  // default option
    left_term_dict_file =
        configure::default_get<string>("left-term-dict", term_dict_file);
    right_term_dict_file =
        configure::default_get<string>("right-term-dict", term_dict_file);

    waf::force_type precision =
        configure::default_get<waf::force_type>("precision", 0.0);

    ofstream flog;
    if (!waf_internal::resolve_log_option(flog)) return -1;

    try {
        timing::start();
        log(INFO_) << "operation started" << endl;

        ifstream fin(co_matrix_file.c_str());
        if (!fin) {
            log(ERROR_) << "fail to open co-occurrence matrix file '"
                << co_matrix_file << "'" << endl;
            return -1;
        }

        ofstream fout(waf_matrix_file.c_str());
        if (!fout) {
            log(ERROR_) << "fail to open word-activation-force matrix"
                << " file '" << waf_matrix_file << "'" << endl;
            return -1;
        }

        waf::FreqVector freqvec;
        ifstream ffreq(freq_vec_file.c_str());
        if (!ffreq) {
            log(ERROR_) << "fail to open frequency vector file '"
                << freq_vec_file << "'" << endl;
            return -1;
        }
        ffreq >> freqvec;

        waf::Care care_left, care_right;
        waf::TermSet termset_left, termset_right;
        if (left_term_dict_file!="") {
            ifstream fin(left_term_dict_file.c_str());
            if (!fin) {
                log(ERROR_) << "fail to open left term dict file '"
                    << left_term_dict_file << "'" << endl;
                return -1;
            }
            fin >> termset_left;
            care_left = waf::care_in(termset_left);
        } else {
            care_left = waf::care_all();
        }
        if (right_term_dict_file!="") {
            ifstream fin(right_term_dict_file.c_str());
            if (!fin) {
                log(ERROR_) << "fail to open right term dict file '"
                    << right_term_dict_file << "'" << endl;
                return -1;
            }
            fin >> termset_right;
            care_right = waf::care_in(termset_right);
        } else {
            care_right = waf::care_all();
        }

        log(INFO_) << "calculating word-activation-force matrix" << endl;
        waf::word_activation_force(fin, care_left, care_right,
                waf::freq_dict(freqvec), precision, fout);

        log(INFO_) << "operation finished successfully, cost "
            << timing::duration() << " seconds" << endl;
        return 0;

    } catch (const std::exception& e) {
		log(ERROR_)
            << "operation terminated with exception: " << e.what()
            << ", cost " << timing::duration() << " seconds" << endl;
        return -1;
    }
}

int run_affinity_measure(int argc, char* argv[]) {
    configure::read(argc, argv);
    if (configure::search("config-file")) {
        configure::read("config-file");
        configure::read(argc, argv);
    }

    if (!configure::search("waf-matrix")) {
        cerr << "option '--waf-matrix' needed"
            << " to specify word-activation-force matrix (input)" << endl;
        return -1;
    }
    vector<string> waf_matrix_files =
        configure::get<vector<string> >("waf-matrix");
    string affinity_matrix_file, affinity_vector_file;
    switch (waf_matrix_files.size()) {
    case 1:  // one waf-matrix to one affinity matrix
        if (!configure::search("affinity-matrix")) {
            cerr << "option '--affinity-matrix' needed"
                << " to specify affinity matrix (output)" << endl;
            return -1;
        }
        affinity_matrix_file = configure::get<string>("affinity-matrix");
        break;
    case 2:  // two waf-matrix to one affinity vector
        if (!configure::search("affinity-vector")) {
            cerr << "option '--affinity-vector' needed"
                << " to specify affinity vector (output)" << endl;
            return -1;
        }
        affinity_vector_file = configure::get<string>("affinity-vector");
        break;
    default:
        cerr << "option '--waf-matrix' need exactly 1 or 2 file(s)" << endl;
        return -1;
        break;
    }

    string care_term_dict_file =
        configure::default_get<string>("care-term-dict", "");
    string back_term_dict_file =
        configure::default_get<string>("back-term-dict", "");

    waf::affinity_type precision =
        configure::default_get<waf::affinity_type>("precision", 0.0);

    ofstream flog;
    if (!waf_internal::resolve_log_option(flog)) return -1;

    try {
        timing::start();
        log(INFO_) << "operation started" << endl;

        waf::Care care, back;
        waf::TermSet termset_care, termset_back;
        if (care_term_dict_file!="") {
            ifstream fin(care_term_dict_file.c_str());
            if (!fin) {
                log(ERROR_) << "fail to open care term dict file '"
                    << care_term_dict_file << "'" << endl;
                return -1;
            }
            fin >> termset_care;
            care = waf::care_in(termset_care);
        } else {
            care = waf::care_all();
        }
        if (back_term_dict_file!="") {
            ifstream fin(back_term_dict_file.c_str());
            if (!fin) {
                log(ERROR_) << "fail to open back term dict file '"
                    << back_term_dict_file << "'" << endl;
                return -1;
            }
            fin >> termset_back;
            back = waf::care_in(termset_back);
        } else {
            back = waf::care_all();
        }

        switch (waf_matrix_files.size()) {
        case 1:  // one waf-matrix to one affinity-matrix
            {
                ifstream fin(waf_matrix_files[0].c_str());
                if (!fin) {
                    log(ERROR_) << "fail to open word-activation-force matrix"
                        " file '" << waf_matrix_files[0] << "'" << endl;
                    return -1;
                }
                CrossList<waf::force_type> waf_mat;
                fin >> waf_mat;

                ofstream fout(affinity_matrix_file.c_str());
                if (!fout) {
                    log(ERROR_) << "fail to open affinity matrix"
                        " file '" << affinity_matrix_file << "'" << endl;
                    return -1;
                }

                log(INFO_) << "calculating affinity matrix" << endl;
                waf::affinity_measure(waf_mat, care, back,
                        precision, waf::null_affinity, fout);
            }
            break;
        case 2:  // two waf-matrix to one affinity-vector
            {
                CrossList<waf::force_type> waf_mat1, waf_mat2;
                ifstream fin1(waf_matrix_files[0].c_str());
                if (!fin1) {
                    log(ERROR_) << "fail to open word-activation-force matrix"
                        " file '" << waf_matrix_files[0] << "'" << endl;
                    return -1;
                }
                fin1 >> waf_mat1;
                fin1.close();

                ifstream fin2(waf_matrix_files[1].c_str());
                if (!fin2) {
                    log(ERROR_) << "fail to open word-activation-force matrix"
                        " file '" << waf_matrix_files[1] << "'" << endl;
                    return -1;
                }
                fin2 >> waf_mat2;
                fin2.close();

                ofstream fout(affinity_vector_file.c_str());
                if (!fout) {
                    log(ERROR_) << "fail to open affinity vector"
                        " file '" << affinity_vector_file << "'" << endl;
                    return -1;
                }

                if (""==care_term_dict_file) {
                    log(WARNING_) << "care term dict '"
                        << care_term_dict_file << "' ignored" << endl;
                }
                if (precision > 0.0) {
                    log(WARNING_) << "precision "
                        << precision << " ignored" << endl;
                }

                log(INFO_) << "calculating affinity vector" << endl;
                waf::affinity_measure(
                        waf_mat1, back, waf_mat2, back, waf::null_affinity,
                        ostream_iterator<waf::affinity_type>(fout, " "));
            }
            break;
        default:
            break;
        }

        log(INFO_) << "operation finished successfully, cost "
            << timing::duration() << " seconds" << endl;
        return 0;

    } catch (const std::exception& e) {
		log(ERROR_)
            << "operation terminated with exception: " << e.what()
            << ", cost " << timing::duration() << " seconds" << endl;
        return -1;
    }
}

int run_help(int argc, char* argv[]) {
    return 0;
}
