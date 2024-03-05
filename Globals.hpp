// Globals.hpp

#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <cstdint>
#include <string>
#include <set>
#include <vector>
#include <chrono>
#include <omp.h>
#include "pattern.hpp" // Assuming this is your custom class
#include "SO6.hpp"     // Assuming this is your custom class

// Threading and performance tracking
extern uint8_t THREADS;
extern omp_lock_t lock;
extern std::chrono::high_resolution_clock::time_point tcount_init_time;
extern std::chrono::duration<double> timeelapsed;

// Pattern handling and search settings
extern std::set<pattern> pattern_set;
extern std::set<pattern> case_set;
extern std::vector<pattern> cases;
extern std::set<SO6> explicit_search_set;
extern SO6 search_token;
extern std::string pattern_file;
extern std::string case_file;

// Configuration and state variables
extern uint8_t target_T_count;
extern uint8_t stored_depth_max;
extern bool saveResults;
extern bool verbose;
extern bool transpose_multiply;
extern bool explicit_search_mode;
extern bool cases_flag;

class Globals {
    public:
        static void setParameters(int argc, char *argv[]);
        static void configure();
};
#endif // GLOBALS_HPP
