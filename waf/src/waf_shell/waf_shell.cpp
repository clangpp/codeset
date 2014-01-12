// waf_shell.cpp
#include "waf_shell.h"

#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <limits>
#include <string>
#include <set>
#include <stdexcept>
#include <vector>

#include "../../../configure/src/configure/configure.h"
#include "../../../logging/src/logging/logging.h"
#include "../../../serialization/src/serialization/serialization.h"
#include "../../../timing/src/timing/timing.h"

using namespace logging;
using namespace serialization::sparsematrix;
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
        cerr << "option '--term-file' required"
            << " to specify term file(s) (input)" << endl;
        return -1;
    }
    vector<string> term_files = configure::get<vector<string> >("term-file");
    if (term_files.empty()) {
        cerr << "need at least one file for option '--term-file'" << endl;
        return -1;
    }

    if (!configure::search("termid-file")) {
        cerr << "option '--termid-file' required"
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
            fout << static_cast<int>(waf::delim_termid) << "\n";
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
        cerr << "option '--termid-file' required"
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
        cerr << "option '--freq-vector' required"
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
        if (fout) {
            fout << freqvec;
        } else {
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
        cerr << "option '--termid-file' required"
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
        cerr << "option '--window-size' required"
            << " to specify co-occurrence window size" << endl;
        return -1;
    }
    waf::size_type window_size = configure::get<waf::size_type>("window-size");
    if (window_size < 2) {
        cerr << "option '--window-size' should be at least 2" << endl;
        return -1;
    }

    if (!configure::search("co-matrix")) {
        cerr << "option '--co-matrix' required"
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
        cerr << "option '--co-matrix' required"
            << " to specify co-occurrence matrix (input)" << endl;
        return -1;
    }
    string co_matrix_file = configure::get<string>("co-matrix");

    if (!configure::search("waf-matrix")) {
        cerr << "option '--waf-matrix' required"
            << " to specify word-activation-force matrix (output)" << endl;
        return -1;
    }
    string waf_matrix_file = configure::get<string>("waf-matrix");

    if (!configure::search("freq-vector")) {
        cerr << "option '--freq-vector' required"
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
        cerr << "option '--waf-matrix' required"
            << " to specify word-activation-force matrix (input)" << endl;
        return -1;
    }
    vector<string> waf_matrix_files =
        configure::get<vector<string> >("waf-matrix");
    string affinity_matrix_file, affinity_vector_file;
    switch (waf_matrix_files.size()) {
    case 1:  // one waf-matrix to one affinity matrix
        if (!configure::search("affinity-matrix")) {
            cerr << "option '--affinity-matrix' required"
                << " to specify affinity matrix (output)" << endl;
            return -1;
        }
        affinity_matrix_file = configure::get<string>("affinity-matrix");
        break;
    case 2:  // two waf-matrix to one affinity vector
        if (!configure::search("affinity-vector")) {
            cerr << "option '--affinity-vector' required"
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

    string term_dict_file =
        configure::default_get<string>("term-dict", "");  // default option
    string care_term_dict_file =
        configure::default_get<string>("care-term-dict", term_dict_file);
    string back_term_dict_file =
        configure::default_get<string>("back-term-dict", term_dict_file);

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

                if (""!=care_term_dict_file) {
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

enum AnalyzeMethod {ANAL_PAIR, ANAL_INLINK, ANAL_OUTLINK};

template <typename T>
void analyze_matrix(
        const string& matrix_file, const waf::TermSet& termset,
        bool term_filter, bool term_mapping, waf::size_type result_count,
        AnalyzeMethod analyze_method, ostream& os);

int run_analyze_matrix(int argc, char* argv[]) {
    configure::read(argc, argv);
    if (configure::search("config-file")) {
        configure::read("config-file");
        configure::read(argc, argv);
    }

    enum MatrixToken {MAT_NONE, MAT_WAF, MAT_AFFINITY};
    MatrixToken matrix_token = MAT_NONE;
    string waf_matrix_file =
        configure::default_get<string>("waf-matrix", "");
    if (waf_matrix_file!="") matrix_token = MAT_WAF;
    string affinity_matrix_file =
        configure::default_get<string>("affinity-matrix", "");
    if (affinity_matrix_file!="") {
        if (MAT_WAF==matrix_token) {
            cerr << "WARNING: waf-matrix file '" << waf_matrix_file
                << "' specified by '--waf-matrix' is ignored" << endl;
        }
        matrix_token = MAT_AFFINITY;
    }
    if (MAT_NONE==matrix_token) {
        cerr << "at least one of '--waf-matrix', '--affinity-matrix' should be specified (input)" << endl;
        return -1;
    }

    string pair_result_file =
        configure::default_get<string>("pair-result", "");
    string inlink_result_file =
        configure::default_get<string>("inlink-result", "");
    string outlink_result_file =
        configure::default_get<string>("outlink-result", "");
    if (pair_result_file=="" &&
            inlink_result_file=="" && outlink_result_file=="") {
        cerr << "at least one of '--pair-result', '--inlink-result', '--outlink-result' should be specified (output)" << endl;
        return -1;
    }
    vector<pair<AnalyzeMethod, string> > analyze_options;
    analyze_options.push_back(make_pair(ANAL_PAIR, pair_result_file));
    analyze_options.push_back(make_pair(ANAL_INLINK, inlink_result_file));
    analyze_options.push_back(make_pair(ANAL_OUTLINK, outlink_result_file));

    string term_dict_file =
        configure::default_get<string>("term-dict", "");
    waf::size_type result_count =
        configure::default_get<waf::size_type>("result-count",
                std::numeric_limits<waf::size_type>::max());

    ofstream flog;
    if (!waf_internal::resolve_log_option(flog)) return -1;

    try {
        timing::start();
        log(INFO_) << "operation started" << endl;

        waf::TermSet termset;
        bool term_filter = false;
        bool term_mapping = false;
        if (term_dict_file!="") {
            ifstream fin(term_dict_file.c_str());
            if (!fin) {
                log(ERROR_) << "fail to open term dict file '"
                    << term_dict_file << "'" << endl;
                return -1;
            }
            fin >> termset;
            term_filter = true;
            term_mapping = true;
        }

        switch (matrix_token) {
        case MAT_WAF:
            {
                {
                    ifstream fin(waf_matrix_file.c_str());
                    if (!fin) {
                        log(ERROR_) << "fail to open matrix file '"
                            << waf_matrix_file << "'" << endl;
                        return -1;
                    }
                }

                for (size_t i=0; i<analyze_options.size(); ++i) {
                    const AnalyzeMethod& analyze_method = analyze_options[i].first;
                    const string& result_file = analyze_options[i].second;
                    if (result_file=="") continue;
                    ofstream fout(result_file.c_str());
                    if (!fout) {
                        log(ERROR_) << "fail to open result file '"
                            << result_file << "'" << endl;
                        return -1;
                    }
                    log(INFO_) << "analyzing word-activation-force matrix" << endl;
                    analyze_matrix<waf::force_type>(waf_matrix_file, termset,
                            term_filter, term_mapping, result_count, analyze_method, fout);
                }
            }
            break;
        case MAT_AFFINITY:
            {
                {
                    ifstream fin(affinity_matrix_file.c_str());
                    if (!fin) {
                        log(ERROR_) << "fail to open matrix file '"
                            << affinity_matrix_file << "'" << endl;
                        return -1;
                    }
                }

                for (size_t i=0; i<analyze_options.size(); ++i) {
                    const AnalyzeMethod& analyze_method = analyze_options[i].first;
                    const string& result_file = analyze_options[i].second;
                    if (result_file=="") continue;
                    ofstream fout(result_file.c_str());
                    if (!fout) {
                        log(ERROR_) << "fail to open result file '"
                            << result_file << "'" << endl;
                        return -1;
                    }
                    log(INFO_) << "analyzing affinity matrix" << endl;
                    analyze_matrix<waf::affinity_type>(affinity_matrix_file, termset,
                            term_filter, term_mapping, result_count, analyze_method, fout);
                }
            }
            break;
        default:
            {
                log(ERROR_) << "unsupported matrix type" << endl;
                return -1;
            }
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

void filter_freq_vector_by_frequency(
        waf::FreqVector& freqvec, waf::size_type result_count);

int run_filter_termset(int argc, char* argv[]) {
    configure::read(argc, argv);
    if (configure::search("config-file")) {
        configure::read("config-file");
        configure::read(argc, argv);
    }

    if (!configure::search("term-dict")) {
        cerr << "option '--term-dict' required"
            << " to specify source termset (input)" << endl;
        return -1;
    }

    if (!configure::search("freq-vector")) {
        cerr << "option '--freq-vector' required"
            << " to specify termid frequency vector (input)" << endl;
        return -1;
    }

    if (!configure::search("filtered-term-dict")) {
        cerr << "option '--filtered-term-dict required'"
            << " to specify filtered termset (output)" << endl;
        return -1;
    }

    string term_dict_file = configure::get<string>("term-dict");
    string freq_vec_file = configure::get<string>("freq-vector");
    string filtered_term_dict_file =
        configure::get<string>("filtered-term-dict");
    waf::size_type result_count =
        configure::default_get<waf::size_type>("result-count",
                numeric_limits<waf::size_type>::max());

    ofstream flog;
    if (!waf_internal::resolve_log_option(flog)) return -1;

    try {
        timing::start();
        log(INFO_) << "operation started" << endl;

        ifstream freqvec_is(freq_vec_file.c_str());
        if (!freqvec_is) {
            log(ERROR_) << "fail to open frequency vector file '"
                << freq_vec_file << "'" << endl;
            return -1;
        }

        waf::FreqVector freqvec;
        freqvec_is >> freqvec;
        freqvec_is.close();

        log(INFO_) << "filtering frequency vector (by frequency)" << endl;
        filter_freq_vector_by_frequency(freqvec, result_count);

        ifstream termset_is(term_dict_file.c_str());
        if (!termset_is) {
            log(ERROR_) << "fail to open term dict file '"
                << term_dict_file << "'" << endl;
            return -1;
        }
        waf::TermSet termset, termset_filtered;
        termset_is >> termset;
        termset_is.close();

        log(INFO_) << "filtering termset by filtered frequency vector" << endl;
        for (waf::termid_type termid=0;
                termid<=termset.max_termid(); ++termid) {
            if (termset.search(termid) && freqvec[termid]>0)
                termset_filtered.insert(termid, termset[termid]);
        }

        ofstream termset_os(filtered_term_dict_file.c_str());
        if (!termset_os) {
            log(ERROR_) << "fail to open term dict file '"
                << filtered_term_dict_file << "'" << endl;
            return -1;
        }
        termset_os << termset_filtered;

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

struct CommandDescription {
    string name;
    string description;
    vector<string> options;
};

int run_help(int argc, char* argv[]) {
    vector<CommandDescription> commands;

    {
        CommandDescription command;
        command.name = "term-to-termid";
        command.description = "convert term to termid";
        command.options.push_back("--term-file (input)(required)");
        command.options.push_back("--termid-file (output)(required)");
        command.options.push_back("--delimiters");
        command.options.push_back("--init-term-dict (input)");
        command.options.push_back("--term-dict (output)");
        command.options.push_back("--config-file");
        command.options.push_back("--log");
        commands.push_back(command);
    }
    {
        CommandDescription command;
        command.name = "termid-frequency";
        command.description = "calculate termid frequency";
        command.options.push_back("--termid-file (input)(required)");
        command.options.push_back("--freq-vector (output)(required)");
        command.options.push_back("--term-dict: term filter");
        command.options.push_back("--init-freq-vector (input)");
        command.options.push_back("--config-file");
        command.options.push_back("--log");
        commands.push_back(command);
    }
    {
        CommandDescription command;
        command.name = "co-occurrence";
        command.description = "calculate co-occurrence of termid";
        command.options.push_back("--termid-file (input)(required)");
        command.options.push_back("--co-matrix (output)(required)");
        command.options.push_back("--window-size (required)");
        command.options.push_back("--term-dict: term filter");
        command.options.push_back("--left-term-dict: left term filter (cover --term-dict)");
        command.options.push_back("--right-term-dict: right term filter (cover --term-dict)");
        command.options.push_back("--config-file");
        command.options.push_back("--log");
        commands.push_back(command);
    }
    {
        CommandDescription command;
        command.name = "word-activation-force";
        command.description = "calculate word-activation-force matrix";
        command.options.push_back("--co-matrix (input)(required)");
        command.options.push_back("--freq-vector (input)(required)");
        command.options.push_back("--waf-matrix (output)(required)");
        command.options.push_back("--precision");
        command.options.push_back("--term-dict: term filter");
        command.options.push_back("--left-term-dict: left term filter (cover --term-dict)");
        command.options.push_back("--right-term-dict: right term filter (cover --term-dict)");
        command.options.push_back("--config-file");
        command.options.push_back("--log");
        commands.push_back(command);
    }
    {
        CommandDescription command;
        command.name = "affinity-measure";
        command.description = "calculate affinity matrix";
        command.options.push_back("--waf-matrix (input)(required)");
        command.options.push_back("--affinity-matrix or --affinity-vector (output)(required)");
        command.options.push_back("--precision");
        command.options.push_back("--term-dict: term filter");
        command.options.push_back("--care-term-dict: cared term filter (cover --term-dict)");
        command.options.push_back("--back-term-dict: background term filter (cover --term-dict)");
        command.options.push_back("--config-file");
        command.options.push_back("--log");
        commands.push_back(command);
    }
    {
        CommandDescription command;
        command.name = "analyze-matrix";
        command.description = "analyze matrix elements";
        command.options.push_back("--waf-matrix or --affinity-matrix (input)(required)");
        command.options.push_back("--pair-result or --inlink-result or --outlink-result (output)(required)");
        command.options.push_back("--result-count: (total for --pair-result, each elements for --inlink-result and --outlink-result)");
        command.options.push_back("--term-dict: term filter and dictionary");
        command.options.push_back("--config-file");
        command.options.push_back("--log");
        commands.push_back(command);
    }
    {
        CommandDescription command;
        command.name = "filter-termset";
        command.description = "filter termset by term frequency";
        command.options.push_back("--term-dict (input)(required)");
        command.options.push_back("--freq-vector (input)(required)");
        command.options.push_back("--filtered-term-dict (output)(required)");
        command.options.push_back("--result-count");
        command.options.push_back("--config-file");
        command.options.push_back("--log");
        commands.push_back(command);
    }

    // print helping information
    bool cmd_exists = false;
    size_t cmd_index = 0;
    if (1==argc) {
        string command_name = argv[0];
        for (size_t i=0; i<commands.size(); ++i) {
            if (commands[i].name==command_name) {
                cmd_exists = true;
                cmd_index = i;
                break;
            }
        }
    }
    if (!cmd_exists) {
        cerr << "usage: waf <command> <command-options>" << endl;
        cerr << "<command> list:" << endl;
        for (size_t i=0; i<commands.size(); ++i) {
            cerr << "\t" << commands[i].name
                << ": " << commands[i].description << endl;
        }
        cerr << "try 'waf help <command>' for command options" << endl;
    } else {
        const CommandDescription& command = commands[cmd_index];
        cerr << "usage: waf " << command.name << " <command-options>" << endl;
        cerr << "<command-options> list:" << endl;
        for (size_t i=0; i<command.options.size(); ++i)
            cerr << "\t" << command.options[i] << endl;
    }
    return 0;
}

// =============================================================================

template <typename T>
bool cell_compare(const Cell<T>& lhs, const Cell<T>& rhs) {
    if (lhs.value != rhs.value) {
        return lhs.value > rhs.value;
    } else if (lhs.row != rhs.row) {
        return lhs.row < rhs.row;
    } else {
        return lhs.column < rhs.column;
    }
}

template <typename T>
void sort_matrix_term_pairs_pair(
        istream& is_mat, const waf::TermSet& termset, bool term_filter,
        waf::size_type result_count, vector<vector<Cell<T> > >& term_pairs) {
    waf::Care care = waf::care_all();
    if (term_filter) care = waf::care_in(termset);
    term_pairs.resize(1, vector<Cell<T> >());
    vector<Cell<T> >& arr = term_pairs[0];
    Cell<T> cell;
    Dimension dim;
    while (next_cell(is_mat, cell, dim)) {
        if (!care(cell.row) || !care(cell.column)) continue;
        arr.push_back(cell);
        push_heap(arr.begin(), arr.end(), cell_compare<T>);
        if (arr.size() > result_count) {
            pop_heap(arr.begin(), arr.end(), cell_compare<T>);
            arr.pop_back();
        }
    }
    sort_heap(arr.begin(), arr.end(), cell_compare<T>);
}

template <typename T>
void sort_matrix_term_pairs_inlink(
        istream& is_mat, const waf::TermSet& termset, bool term_filter,
        waf::size_type result_count, vector<vector<Cell<T> > >& term_pairs) {
    waf::Care care = waf::care_all();
    if (term_filter) care = waf::care_in(termset);
    term_pairs.clear();  // column prior
    Cell<T> cell;
    Dimension dim;
    while (next_cell(is_mat, cell, dim)) {
        if (!care(cell.row) || !care(cell.column)) continue;
        if (term_pairs.size() <= cell.column)
            term_pairs.resize(cell.column+1);
        vector<Cell<T> >& arr = term_pairs[cell.column];
        arr.push_back(cell);
        push_heap(arr.begin(), arr.end(), cell_compare<T>);
        if (arr.size() > result_count) {
            pop_heap(arr.begin(), arr.end(), cell_compare<T>);
            arr.pop_back();
        }
    }
    for (size_t i=0; i<term_pairs.size(); ++i)
        sort_heap(term_pairs[i].begin(), term_pairs[i].end(), cell_compare<T>);
}

template <typename T>
void sort_matrix_term_pairs_outlink(
        istream& is_mat, const waf::TermSet& termset, bool term_filter,
        waf::size_type result_count, vector<vector<Cell<T> > >& term_pairs) {
    waf::Care care = waf::care_all();
    if (term_filter) care = waf::care_in(termset);
    term_pairs.clear();  // row prior
    Cell<T> cell;
    Dimension dim;
    while (next_cell(is_mat, cell, dim)) {
        if (!care(cell.row) || !care(cell.column)) continue;
        if (term_pairs.size() <= cell.row)
            term_pairs.resize(cell.row+1);
        vector<Cell<T> >& arr = term_pairs[cell.row];
        arr.push_back(cell);
        push_heap(arr.begin(), arr.end(), cell_compare<T>);
        if (arr.size() > result_count) {
            pop_heap(arr.begin(), arr.end(), cell_compare<T>);
            arr.pop_back();
        }
    }
    for (size_t i=0; i<term_pairs.size(); ++i)
        sort_heap(term_pairs[i].begin(), term_pairs[i].end(), cell_compare<T>);
}

template <typename T>
void term_pairs_output(const vector<vector<Cell<T> > >& sorted_pairs,
        const waf::TermSet& termset, bool term_mapping, ostream& os) {
    if (term_mapping) {
        os << "<from_term>\t<to_term>\t<value>" << endl;
        for (typename vector<vector<Cell<T> > >::const_iterator
                it1=sorted_pairs.begin(); it1!=sorted_pairs.end(); ++it1) {
            for (typename vector<Cell<T> >::const_iterator
                    it2=it1->begin(); it2!=it1->end(); ++it2) {
                os << termset[it2->row] << "\t"
                    << termset[it2->column] << "\t"
                    << it2->value << endl;
            }
        }
    } else {
        os << "<from_termid>\t<to_termid>\t<value>" << endl;
        for (typename vector<vector<Cell<T> > >::const_iterator
                it1=sorted_pairs.begin(); it1!=sorted_pairs.end(); ++it1) {
            for (typename vector<Cell<T> >::const_iterator
                    it2=it1->begin(); it2!=it1->end(); ++it2) {
                os << it2->row << "\t"
                    << it2->column << "\t"
                    << it2->value << endl;
            }
        }
    }
}

template <typename T>
void analyze_matrix(
        const string& matrix_file, const waf::TermSet& termset,
        bool term_filter, bool term_mapping, waf::size_type result_count,
        AnalyzeMethod analyze_method, ostream& os) {
    ifstream is_mat(matrix_file.c_str());
    vector<vector<Cell<T> > > term_pairs;
    switch (analyze_method) {
    case ANAL_PAIR:
        sort_matrix_term_pairs_pair(
                is_mat, termset, term_filter, result_count, term_pairs);
        break;
    case ANAL_INLINK:
        sort_matrix_term_pairs_inlink(
                is_mat, termset, term_filter, result_count, term_pairs);
        break;
    case ANAL_OUTLINK:
        sort_matrix_term_pairs_outlink(
                is_mat, termset, term_filter, result_count, term_pairs);
        break;
    default:
        break;
    }
    term_pairs_output(term_pairs, termset, term_mapping, os);
}

bool freq_pair_compare(
        const pair<waf::termid_type, waf::size_type>& lhs,
        const pair<waf::termid_type, waf::size_type>& rhs) {
    if (lhs.second != rhs.second) {
        return lhs.second > rhs.second;
    } else {
        return lhs.first < rhs.first;
    }
}

void filter_freq_vector_by_frequency(
        waf::FreqVector& freqvec, waf::size_type result_count) {
    if (freqvec.size() <= result_count) return;
    vector<pair<waf::termid_type, waf::size_type> > freq_pairs;

    // // filter 'result_count' high frequency termids
    // // time O(freqvec.size() * log(result_count)), space O(result_count)
    // for (waf::termid_type termid=0; termid<freqvec.size(); ++termid) {
    //     freq_pairs.push_back(make_pair(termid, freqvec[termid]));
    //     push_heap(freq_pairs.begin(), freq_pairs.end(), freq_pair_compare);
    //     if (freq_pairs.size() > result_count) {
    //         pop_heap(freq_pairs.begin(), freq_pairs.end(), freq_pair_compare);
    //         freq_pairs.pop_back();
    //     }
    // }

    // filter 'result_count' high frequency termids
    // time O(result_count), space O(freqvec.size())
    for (waf::termid_type termid=0; termid<freqvec.size(); ++termid)
        freq_pairs.push_back(make_pair(termid, freqvec[termid]));
    nth_element(freq_pairs.begin(), freq_pairs.begin()+result_count,
            freq_pairs.end(), freq_pair_compare);
    freq_pairs.resize(result_count);

    // set frequencies: low frequency termids' to 0, high's to 1
    for (size_t i=0; i<freqvec.size(); ++i) freqvec[i] = 0;
    for (size_t i=0; i<freq_pairs.size(); ++i)
        freqvec[freq_pairs[i].first] = 1;
}
