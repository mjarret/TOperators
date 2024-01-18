/**
 * T Operator Product Generation Main File
 * @file main.cpp
 * @author Connor Mooney
 * @author Michael Jarret
 * @author Andrew Glaudell
 * @author Jacob Weston
 * @author Mingzhen Tian
 * @version 6/1/21
 */

#include <iostream>
#include <chrono>
#include <set>
#include <cstring>
#include <string>
#include <fstream>
#include <vector>
#include <omp.h>
#include <thread>
#include <filesystem>
#include "utils.hpp"

using namespace std;

// Global variables

// Threading and performance tracking
static uint8_t THREADS = (uint)std::thread::hardware_concurrency()-1; // Default number of threads
static omp_lock_t lock;
static std::chrono::_V2::high_resolution_clock::time_point tcount_init_time;
static chrono::duration<double> timeelapsed;

// Pattern handling and search settings
static set<pattern> pattern_set;          // Holds the set of patterns
static set<SO6> explicit_search_set;      // Sets the matrices we're looking for
static SO6 search_token;                  // In case we're searching for something in particular
static std::string pattern_file = "";     // Pattern file
static uint8_t explicit_search_depth = 0; // Explicit search depth

// Configuration and state variables
static uint8_t target_T_count;            // Default target_T_count = 0
static uint8_t stored_depth_max = 255;    // Limit comparison depth, default 255
static bool saveResults = false;          // Flag to save data
static bool verbose;
static bool transpose_multiply = false;
static bool explicit_search_mode = false;


static void insert_all_permutations(pattern &p)
{
    pattern_set.insert(p);
    int row[6] = {0, 1, 2, 3, 4, 5};
    while (std::next_permutation(row, row + 6))
    {

        // First, insert the permutation from the original matrix
        pattern perm_of_orig;
        for (int c = 0; c < 6; c++)
        {
            for (int r = 0; r < 6; r++)
                perm_of_orig.arr[c][r] = p.arr[c][row[r]];
        }
        perm_of_orig.lexicographic_order();
        pattern_set.insert(perm_of_orig);

        for(unsigned int counter = 0; counter < (1 << 6); counter++) {
            // This loop iterates over all 64 possible combinations of row modifications.
            // 'counter' represents a combination, where each bit corresponds to a row.
            // If a bit is set (1), the corresponding row is modified.
            // For example:
            //  - counter = 0 (binary 000000) means no rows are modified.
            //  - counter = 1 (binary 000001) means only the first row is modified.
            //  - counter = 3 (binary 000011) means the first and second rows are modified.
            //  - and so on, up to counter = 63 (binary 111111), where all rows are modified.

            pattern mod_of_perm = perm_of_orig; // Start with a copy of the original permutation.
            for(int j = 0; j < 6; j++) {
                if((counter >> j) & 1) {
                    // Check if the j-th bit of 'counter' is set.
                    // '1 << j' creates a bitmask with only the j-th bit set.
                    // If the j-th bit is set in 'counter', modify the j-th row.
                    mod_of_perm.mod_row(j);
                }
            }

            mod_of_perm.lexicographic_order(); // Order the pattern lexicographically.
            pattern_set.insert(mod_of_perm);   // Insert the modified pattern into the set.
        }
    }

    // Insert the transposes, just one transpose now
    pattern p_transpose = p.transpose();
    pattern_set.insert(p_transpose);
}

static void erase_all_permutations(pattern &pat)
{
    pattern_set.erase(pat);
    int row[6] = {0, 1, 2, 3, 4, 5};
    while (std::next_permutation(row, row + 6))
    {
        pattern perm_of_orig;
        for (int c = 0; c < 6; c++)
        {
            for (int r = 0; r < 6; r++)
                perm_of_orig.arr[c][r] = pat.arr[c][row[r]];
        }
        perm_of_orig.lexicographic_order();
        pattern_set.erase(perm_of_orig);

        // Now erase all things that are off by identity
        for(unsigned int counter = 0; counter < 64; counter++) {
            pattern mod_of_perm = perm_of_orig;
            for(int j = 0; j < 6; j++) {
                if(counter & (1 << j)) {
                    mod_of_perm.mod_row(j);
                }
            }
            mod_of_perm.lexicographic_order();
            pattern_set.erase(mod_of_perm);
        }
    }
}

/// @brief Reads binary patterns from a file and processes them.
///        Each line in the file is expected to be a binary string representing a pattern.
///        This function converts each line into a pattern object, orders it lexicographically,
///        inserts all its permutations into a set, and then handles the identity pattern.
static void read_pattern_file(std::string pattern_file_path)
{
    std::ifstream patternFile(pattern_file); // More descriptive variable name

    if (!patternFile.is_open())
    {
        std::cerr << "Failed to open pattern file: " << pattern_file << std::endl;
        return;
    }

    std::string binaryString; // Renamed for clarity
    while (getline(patternFile, binaryString))
    {
        int index = 0; // Index for binaryArray
        pattern currentPattern(binaryString); // More descriptive name
        currentPattern.lexicographic_order();
        insert_all_permutations(currentPattern);
    }

    patternFile.close(); // Close file after processing

    // Handle special case of the identity pattern
    pattern identityPattern = pattern::identity();
    pattern_set.erase(identityPattern);
    pattern_set.erase(identityPattern.pattern_mod());
}

static std::chrono::_V2::high_resolution_clock::time_point now()
{
    return chrono::_V2::high_resolution_clock::now();
}

/**
 * @brief Initialize command line arguments passed to main
 * @param argc Number of arguments
 * @param argv List of arguments
 */
static void setParameters(int argc, char **&argv)
{
    if (argc == 1)
    {
        std::cout << "usage: main.out -tcount t [-v] [-threads n] [-save] [-recover] [-stored_depth s] [-pattern_file f] [-explicit_search depth] [-transpose_multiply]\n\n";
        std::exit(EXIT_SUCCESS);
    }
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-tcount") == 0)
        {
            target_T_count = std::stoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-stored_depth") == 0)
        {
            stored_depth_max = std::stoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-pattern_file") == 0)
        {
            pattern_file = argv[i + 1];
        }
        if (strcmp(argv[i], "-v") == 0)
        {
            verbose = true;
        }
        if (strcmp(argv[i], "-threads") == 0)
        {
            if (strcmp(argv[i + 1], "max") == 0)
            {
                THREADS = (uint)std::thread::hardware_concurrency();
            }
            else
            {
                if (std::stoi(argv[i + 1]) < 0)
                {
                    THREADS = std::thread::hardware_concurrency() + std::stoi(argv[i + 1]);
                }
                else
                {
                    THREADS = std::min(std::stoi(argv[i + 1]), (int)std::thread::hardware_concurrency());
                }
            }
        } 
        if (strcmp(argv[i], "-save") == 0)
        {
            saveResults = true;
        }
        if (strcmp(argv[i], "-transpose_multiply") == 0)
        {
            transpose_multiply = true;
        }
        if (strcmp(argv[i], "-explicit_search") == 0)
        {
            explicit_search_mode = true;
            explicit_search_depth = std::stoi(argv[i + 1]);
        }
    }

    // Make stored_depth_max appropriate

    stored_depth_max = std::min(stored_depth_max,static_cast<uint8_t>(target_T_count - 1));

    std::cout << "[Config] Generating up to T=" << (int)target_T_count << "." << std::endl;
    std::cout << "[Config] Storing at most T=" << (int)stored_depth_max << " in memory." << std::endl;
    std::cout << "[Config] Running on " << (int)THREADS << " threads." << std::endl;
    if(!pattern_file.empty()) {
        std::cout << "[Config] Pattern file " << pattern_file << std::endl;
    } else {
        std::cout << "[Warning] No pattern file." << std::endl;
    }
}

/**
 * @brief Erases the pattern of an SO6 from pattern_set
 * @param s the SO6 to be erased
 */
static void erase_and_record_pattern(SO6 &s, std::ofstream& of) {
    pattern pat = s.to_pattern();
    if (pattern_set.find(pat) != pattern_set.end()) {
        omp_set_lock(&lock);
        // Double check after grabbing the lock
        if (pattern_set.find(pat) != pattern_set.end()) {
            erase_all_permutations(pat);
            of << s.circuit_string() << std::endl;
        }
        omp_unset_lock(&lock);
    }
}

/**
 * @brief Erases the pattern of an SO6 from pattern_set
 * @param s the SO6 to be erased
 */
static void erase_pattern(SO6 &s) {
    pattern pat = s.to_pattern();
    if (pattern_set.find(pat) != pattern_set.end()) {
        omp_set_lock(&lock);
        // Double check after grabbing the lock
        if (pattern_set.find(pat) != pattern_set.end()) {
            erase_all_permutations(pat);
        }
        omp_unset_lock(&lock);
    }
}

/// @brief Reads dat file and prints string of gates circuit
/// @param file_name 
static void read_dat(std::string file_name) {
    std::string line;
    std::ifstream file(file_name);
    std::ofstream null_stream("/dev/null");
    if (file.is_open()) {
        while (getline(file, line)) {
            SO6 s = SO6::reconstruct(line);
            std::cout << "current size: " << pattern_set.size() << "\n";
            erase_pattern(s);
            std::cout << s.circuit_string() << "\n";
        }
    }
}

/**
 * @brief Checks whether an SO6 is one that we are explicitly searching for
 * @param s the SO6 we're attempting to find
 */
static void find_specific_matrix(SO6 &s)
{
    if (!explicit_search_mode)
        return;
    if (explicit_search_set.find(s) != explicit_search_set.end())
    {
        SO6 found = *explicit_search_set.find(s);
        std::cout << "\n[Break] Found the matrix we were looking for with name: " << SO6::name_as_num(s.name());
        std::cout << "\n[Break] Generated from search_token by: " << SO6::name_as_num(found.name()) << "\n";
        std::vector<unsigned char> hist = found.hist;
        SO6 tmp = s;
        std::reverse(hist.begin(),hist.end());
        for (unsigned char i : hist)
        {
            if(i>15) {
                s = s.left_multiply_by_T((i>>4)-1);
            }
            s = s.left_multiply_by_T((i & 15) -1);
        }
        if (s==search_token)
        {
            std::cout << "[Break] Search_token has name: " << SO6::name_as_num(s.name()) << "\n[Break] Exiting." << std::endl;
        }
        std::exit(0);
    }
}

static std::string time_since(std::chrono::_V2::high_resolution_clock::time_point &s)
{
    chrono::duration<double> duration = now() - s;
    int64_t time = chrono::duration_cast<chrono::milliseconds>(duration).count();
    if (time < 1000)
        return std::to_string(time).append("ms");
    if (time < 60000)
        return std::to_string((float)time / 1000).substr(0, 5).append("s");
    if (time < 3600000)
        return std::to_string((float)time / 60000).substr(0, 5).append("min");
    if (time < 86400000)
        return std::to_string((float)time / 3600000).substr(0, 5).append("hr");
    return std::to_string((float)time / 86400000).substr(0, 5).append("days");
}

static void configure()
{
    stored_depth_max = std::min((int)target_T_count - 1, (int)stored_depth_max);

    if (stored_depth_max < target_T_count / 2)
    { // Parameter check
        std::cout << "stored_depth bad\n";
        std::exit(EXIT_FAILURE);
    }

    // Load search criteria
    if (!pattern_file.empty())
    {
        std::cout << "[Read] Reading patterns from " << pattern_file << std::endl;
        read_pattern_file(pattern_file);
        std::cout << "[Finished] Loaded " << pattern_set.size() << " non-identity patterns." << std::endl;
    }
    if (explicit_search_mode)
    {
        // build_explicit_search_token();
        std::cout << "[Start] Generating lookup table for token=\n"
                  << search_token;
        // build_explicit_search_set(explicit_search_depth);
        std::cout << "[Finished] Lookup table contains " << explicit_search_set.size() << " unique SO6s." << std::endl;
    }
}

/**
 * @brief Method to report the start of a T count iteration
 * @param T the number of the current T count
 */
static void report_begin_T_count(const int T) {
    std::cout << " ||\t[Start] Beginning T=" << T << std::endl;
    tcount_init_time = now();
}

/**
 * @brief Method to report percentage completed
 * @param c current integer countr
 * @param s total size
 */
static void report_percent_complete(const uint64_t &c, const uint64_t s)
{
    if ((c & 0x1FFF) == 0) // if c is divisible by 8192
    {
        std::cout << "\033[A\033[A\r ||\t↪ [Progress] Processing .....    "
                  << (100*c/s) << "\%" << "\n ||\t↪ [Patterns] "
                  << pattern_set.size() << " patterns remain." << std::endl;
    }
}

/**
 * @brief Function to report completion
 * @param matrices_found how many matrices were found
 */
static void finish_io(const uint &matrices_found, const bool b, std::ofstream &of) {
    std::cout << "\033[A\033[A\r ||\t↪ [Progress] Processing .....    100\%" << std::endl; 
    std::cout << " ||\t↪ [Patterns] " << pattern_set.size() << " patterns remain." << std::endl;
    if(b) std::cout << " ||\t↪ [Finished] Found " << matrices_found << " new matrices in " << time_since(tcount_init_time) << "\n ||" << std::endl;
    else std::cout << " ||\t↪ [Finished] Completed in " << time_since(tcount_init_time) << "\n ||" << std::endl;
    of.close();
}


static std::ofstream prepare_T_count_io(const int t, uint8_t &stored_depth_max, uint8_t &target_T_count) {
    int free_multiply_depth = utils::free_multiply_depth(target_T_count,stored_depth_max);
    // Begin reporting for T=1 with specific depth information
    if (t == 1) {
        std::cout << "\n\n[Begin] Generating T=1 through T=" << static_cast<int>(stored_depth_max);
        std::cout << " iteratively, but will only save ";

        if (free_multiply_depth >= 2) {
            std::cout << (free_multiply_depth == 2 ? "T=2" : "2≤T≤" + std::to_string(free_multiply_depth));
            std::cout << " and ";
        }
        std::cout << "T=" << static_cast<int>(stored_depth_max) << " in memory\n ||\n";
    }

    report_begin_T_count(t);
    std::string file_string = "./data/" + to_string(t) + ".dat";
    std::ofstream of;
    of.open(file_string, ios::out | ios::trunc);
    if(!of.is_open()) std::exit(0);
    std::cout << " ||\t↪ [Save] Opening file " << file_string << "\n";
    if(t == stored_depth_max+1) 
        std::cout << " ||\t↪ [Rep] Left multiplying everything by T₀\n";
    if(t > stored_depth_max+1) 
        std::cout << " ||\t↪ [Rep] Using generating_set[" << t-stored_depth_max-1 << "]\n";
    std::cout << " ||\t↪ [Progress] Processing .....    0\%\n"
                << " ||\t↪ [Patterns] " << pattern_set.size() << " patterns remain." << std::endl;
    return of;
}


/**
 * @brief Store specific cosets T_0{curr} based on the current T count and free multiply depth.
 * This method saves a subset of the SO6 objects to the generating set, which are used in later iterations.
 * 
 * @param curr_T_count The current T count in the main computation loop.
 * @param free_multiply_depth The depth until which free multiplication is performed.
 * @param num_generating_sets The total number of generating sets.
 * @param current The current set of SO6 objects.
 * @param generating_set Reference to an array of vectors of SO6 objects to store the generated sets.
 */
void storeCosets(int curr_T_count, int free_multiply_depth, int num_generating_sets, 
                 std::set<SO6>& current, std::vector<SO6> (&generating_set)[])
{
    int ff = utils::free_multiply_depth(target_T_count,stored_depth_max);
    if (curr_T_count < ff - 1)
    {
        int index = min(curr_T_count, num_generating_sets - 1);
        std::cout << "\033[A\r ||\t↪ [Save] Saving coset T₀{T=" << curr_T_count + 1 << "} as generating_set[" << index << "]\n ||" << endl;
        generating_set[index] = std::vector<SO6>(current.begin(),current.end());
    }
}

/**
 * @brief This is the main loop
 * @param argc number of command line arguments
 * @param argv vector of command line arguments
 * @return 0
 */
int main(int argc, char **argv)
{
    auto program_init_time = now(); // Begin timekeeping
    setParameters(argc, argv);      // Initialize parameters to command line argument
    configure();                    // Configure the search

    SO6 root = SO6::identity();
    set<SO6> prior, current = std::set<SO6>({root});

    // This stores the generating sets. Note that the initial generating set is just the 15 T matrices and, thus, doesn't need to be stored
    int ngs = utils::num_generating_sets(target_T_count, stored_depth_max);
    int ff = utils::free_multiply_depth(target_T_count, stored_depth_max);

    std::vector<SO6> generating_set[ngs];

    
    std::string file_string;

    for (int curr_T_count = 0; curr_T_count < stored_depth_max; ++curr_T_count)
    {
        set<SO6> next;
        std::ofstream of = prepare_T_count_io(curr_T_count+1,stored_depth_max,target_T_count);

        uint64_t count = 0, interval_size = 15*current.size();

        for (int T = 0; T < 15; T++)
        {                
            for(const SO6& S : current)
            {
                SO6 toInsert = S.left_multiply_by_T(T);
                if (next.insert(toInsert).second)               // Want to insert toInsert no matter what, if we haven't seen this
                { 
                    if(T==0) erase_and_record_pattern(toInsert, of);           // Only erase the pattern if the last T operation was done by 0, since we'll do things up to permutation?
                }
                report_percent_complete(++count, interval_size);
            }
        }

        utils::setDifference(next,prior);
        utils::rotate_and_clear(prior, current, next); // current is now ready for next iteration

        finish_io(current.size(), true, of);
        storeCosets(curr_T_count, ff, ngs, current, generating_set);
    }
    
    std::set<SO6>().swap(prior); // Swap to clear
    std::cout << " ||\n[End] Stored T=" << (int)stored_depth_max << " as current to generate T=" << stored_depth_max + 1 << " through T=" << (int)target_T_count << "\n" << std::endl;

    std::vector<SO6> to_compute = utils::convert_to_vector_and_clear(current);

    int pss = pattern_set.size();
    std::cout << "[Report] Current patterns: " << pss << std::endl;

    std::cout << "[Begin] Beginning brute force multiply.\n ||" << std::endl;
    uint64_t set_size = to_compute.size();
    uint64_t interval_size = std::ceil(set_size / THREADS); // Equally divide among threads, not sure how to balance but each should take about the same time


    for (int curr_T_count = stored_depth_max; curr_T_count < target_T_count; ++curr_T_count)
    {    
        std::ofstream of = prepare_T_count_io(curr_T_count+1,stored_depth_max, target_T_count);
        omp_init_lock(&lock);
        #pragma omp parallel for schedule(static, interval_size) num_threads(THREADS)
        for (uint64_t i = 0; i < set_size; i++)
        {
            const SO6 &S = to_compute.at(i); 
            if (omp_get_thread_num() == 0)
                report_percent_complete(i % interval_size, interval_size);

            if (curr_T_count == stored_depth_max)
            {
                SO6 N = S.left_multiply_by_T(0);
                erase_and_record_pattern(N, of);
                continue;
            }

            for (const SO6 &G : generating_set[curr_T_count-stored_depth_max - 1])
            {
                SO6 N = G*S;
                erase_and_record_pattern(N, of);
            }
        }
        omp_destroy_lock(&lock);
        finish_io(0, false, of);
    }
    std::cout << " ||\n[Finished] Free multiply complete.\n\n[Time] Total time elapsed: " << time_since(program_init_time) << std::endl;
    return 0;
}
