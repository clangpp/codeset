#ifndef WAF_SHELL_H_
#define WAF_SHELL_H_

namespace waf {

int run_term_to_termid(int argc, char* argv[]);

int run_termid_frequency(int argc, char* argv[]);

int run_co_occurrence(int argc, char* argv[]);

int run_word_activation_force(int argc, char* argv[]);

int run_affinity_measure(int argc, char* argv[]);

int run_analyze_matrix(int argc, char* argv[]);

int run_filter_termset(int argc, char* argv[]);

int run_help(int argc, char* argv[]);

}  // namespace waf

#endif  // WAF_SHELL_H_
