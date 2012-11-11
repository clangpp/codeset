// waf_shell.h
#ifndef WAF_SHELL_H_
#define WAF_SHELL_H_

#include "../waf_core/waf_core.h"
#include "../waf_facility/waf_facility.h"

int run_term_to_termid(int argc, char* argv[]);

int run_termid_frequency(int argc, char* argv[]);

int run_co_occurrence(int argc, char* argv[]);

int run_word_activation_force(int argc, char* argv[]);

int run_affinity_measure(int argc, char* argv[]);

int run_analyze_matrix(int argc, char* argv[]);

int run_help(int argc, char* argv[]);

#include "waf_shell-inl.h"

#endif  // WAF_SHELL_H_
