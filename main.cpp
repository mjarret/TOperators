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
#include <algorithm>
#include <vector>
#include <math.h>
#include <omp.h>
#include <thread>
#include "SO6.hpp"
#include <bitset>
#include <filesystem>

using namespace std;

static uint8_t target_T_count;         // Default target_T_count = 0
static uint8_t THREADS = 3;            // Default number of threads
static bool saveResults = false;       // Turn on to save data
static uint8_t stored_depth_max = 255; // By default, you don't want to limit comparison depth
static chrono::duration<double> timeelapsed;
static SO6 search_token; // in case we're searching for something in particular
static bool verbose;
static set<pattern> pattern_set;          // will hold the set of patterns
static std::string pattern_file = "";     // pattern file
static set<SO6> explicit_search_set;      // Sets the matrices we're looking for
static uint8_t explicit_search_depth = 0; //
bool transpose_multiply = false;
bool explicit_search_mode = false;
static uint8_t free_multiply_depth = 1;
static std::chrono::_V2::high_resolution_clock::time_point tcount_init_time;
static omp_lock_t lock;
static ofstream output_file;


static void insert_all_permutations(pattern &p)
{
    pattern_set.insert(p);
    int row[6] = {0, 1, 2, 3, 4, 5};
    while (std::next_permutation(row, row + 6))
    {
        pattern tmp;
        for (int c = 0; c < 6; c++)
        {
            for (int r = 0; r < 6; r++)
                tmp.arr[c][r] = p.arr[c][row[r]];
        }
        tmp.lexicographic_order();
        pattern_set.insert(tmp);
    }

    // pattern p_mod = p.pattern_mod();
    // while (std::next_permutation(row, row + 6))
    // {
    //     pattern tmp;
    //     for (int c = 0; c < 6; c++)
    //     {
    //         for (int r = 0; r < 6; r++)
    //             tmp.arr[c][r] = p_mod.arr[c][row[r]];
    //     }
    //     tmp.lexicographic_order();
    //     pattern_set.insert(tmp);
    // }

    pattern p_transpose = p.transpose();

    pattern_set.insert(p_transpose);

    while (std::next_permutation(row, row + 6))
    {
        pattern tmp;
        for (int c = 0; c < 6; c++)
        {
            for (int r = 0; r < 6; r++)
                tmp.arr[c][r] = p_transpose.arr[c][row[r]];
        }
        tmp.lexicographic_order();
        pattern_set.insert(tmp);
    }


    // p_mod = p.transpose().pattern_mod();
    // while (std::next_permutation(row, row + 6))
    // {
    //     pattern tmp;
    //     for (int c = 0; c < 6; c++)
    //     {
    //         for (int r = 0; r < 6; r++)
    //             tmp.arr[c][r] = p_mod.arr[c][row[r]];
    //     }
    //     tmp.lexicographic_order();
    //     pattern_set.insert(tmp);
    // }
}

static void erase_all_permutations(pattern &pat)
{
    pattern_set.erase(pat);
    int row[6] = {0, 1, 2, 3, 4, 5};
    while (std::next_permutation(row, row + 6))
    {
        pattern tmp;
        for (int c = 0; c < 6; c++)
        {
            for (int r = 0; r < 6; r++)
                tmp.arr[c][r] = pat.arr[c][row[r]];
        }
        tmp.lexicographic_order();
        pattern_set.erase(tmp);
    }

    // pattern pat_mod = pat.pattern_mod();
    // pattern_set.erase(pat_mod);
    // while (std::next_permutation(row, row + 6))
    // {
    //     pattern tmp;
    //     for (int c = 0; c < 6; c++)
    //     {
    //         for (int r = 0; r < 6; r++)
    //             tmp.arr[c][r] = pat_mod.arr[c][row[r]];
    //     }
    //     tmp.lexicographic_order();
    //     pattern_set.erase(tmp);
    // }

    pattern p_transpose = pat.transpose();
    pattern_set.erase(p_transpose);
    while (std::next_permutation(row, row + 6))
    {
        pattern tmp;
        for (int c = 0; c < 6; c++)
        {
            for (int r = 0; r < 6; r++)
                tmp.arr[c][r] = p_transpose.arr[c][row[r]];
        }
        tmp.lexicographic_order();
        pattern_set.erase(tmp);
    }

    // pat_mod = p_transpose.pattern_mod();
    // while (std::next_permutation(row, row + 6))
    // {
    //     pattern tmp;
    //     for (int c = 0; c < 6; c++)
    //     {
    //         for (int r = 0; r < 6; r++)
    //             tmp.arr[c][r] = pat_mod.arr[c][row[r]];
    //     }
    //     tmp.lexicographic_order();
    //     pattern_set.erase(tmp);
    // }
}

static void readPattern()
{
    string line;
    ifstream pf(pattern_file);
    if (pf.is_open())
    {
        while (getline(pf, line))
        {
            bool line_in_binary[72];
            int j = 0;
            for (unsigned i = 0; i < line.length(); i++)
            {
                if (line.at(i) == '[' || line.at(i) == ']' || line.at(i) == ' ' || line.at(i) == ',')
                {
                    continue;
                }
                if (line.at(i) == '0')
                {
                    line_in_binary[j] = 0;
                    j++;
                    continue;
                }
                line_in_binary[j] = 1;
                j++;
            }
            pattern pat(line_in_binary);
            pat.lexicographic_order();
            insert_all_permutations(pat);
            // pattern_set.insert(pat);
            // pattern_set.insert(pat.pattern_mod());
            // pat = pat.transpose();
            // pattern_set.insert(pat.pattern_mod());
        }
        pf.close();
        pattern identity = pattern::identity();
        pattern_set.erase(identity);
        pattern_set.erase(identity.pattern_mod());
    }
}

static std::chrono::_V2::high_resolution_clock::time_point now()
{
    return chrono::_V2::high_resolution_clock::now();
}

/// @brief This sets a matrix that we're looking for the pattern of
/// this is currently done manually, but patterns can probably be stored
/// as a separate object that requires far less memory since they
/// only have 3 possible (unreduced) nonzero entries, Z(1,0,0), Z(1,1,0), Z(1,0,1)
/// Thus, we only need 2 bits per entry and can store the entire matrix in at most
/// 64 bits.
static void build_explicit_search_token()
{
    // search_token[0][0] = Z2(0,0,3);
    // search_token[1][0] = Z2(0,0,3);
    search_token[2][0] = Z2(1, 0, 3);
    search_token[3][0] = Z2(1, 0, 3);
    search_token[4][0] = Z2(1, 1, 3);
    search_token[5][0] = Z2(1, -1, 3);

    // search_token[0][1] = Z2(0,0,3);
    // search_token[1][1] = Z2(0,0,3);
    search_token[2][1] = Z2(1, -1, 3);
    search_token[3][1] = Z2(1, 1, 3);
    search_token[4][1] = Z2(-1, 0, 3);
    search_token[5][1] = Z2(-1, 0, 3);

    search_token[0][2] = Z2(1, 0, 3);
    search_token[1][2] = Z2(1, -1, 3);
    // search_token[2][2] = Z2(0,0,3);
    search_token[3][2] = Z2(1, 0, 3);
    // search_token[4][2] = Z2(0,0,3);
    search_token[5][2] = Z2(1, 1, 3);

    search_token[0][3] = Z2(1, 0, 3);
    search_token[1][3] = Z2(1, 1, 3);
    search_token[2][3] = Z2(1, 0, 3);
    // search_token[3][3] = Z2(0,0,3);
    search_token[4][3] = Z2(1, -1, 3);
    // search_token[5][3] = Z2(0,0,3);

    search_token[0][4] = Z2(1, 1, 3);
    search_token[1][4] = Z2(-1, 0, 3);
    // search_token[2][4] = Z2(0,0,3);
    search_token[3][4] = Z2(1, -1, 3);
    // search_token[4][4] = Z2(0,0,3);
    search_token[5][4] = Z2(-1, 0, 3);

    search_token[0][5] = Z2(1, -1, 3);
    search_token[1][5] = Z2(-1, 0, 3);
    search_token[2][5] = Z2(1, 1, 3);
    // search_token[3][5] = Z2(0,0,3);
    search_token[4][5] = Z2(-1, 0, 3);
    // search_token[5][5] = Z2(0,0,3);

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            search_token[i][j].reduce();
        }
    }
    search_token.lexicographic_order();
    if (verbose)
        std::cout << "[Note:] We are searching for search_token=\n"
                  << search_token << "\n";
}

static void build_explicit_search_set(int depth)
{
    explicit_search_set.insert(search_token);
    for (int i = 0; i < depth; i++)
    {
        set<SO6> tmp = explicit_search_set;
        for (SO6 curr : explicit_search_set)
        {
            for (unsigned char l = 0; l < 15; l++)
            {
                SO6 C = curr.left_multiply_by_T_transpose(l);
                tmp.insert(C);
            }
        }
        std::swap(explicit_search_set, tmp);
    }
}

std::vector<SO6> read_file_in_parallel() {
    std::vector<SO6> ret;
    return ret;
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
static void erase_pattern(SO6 &s)
{
    pattern pat = s.to_pattern();
    // pat.lexicographic_order();
    // if(pattern_set.find(pat) == pattern_set.end() && !(pat == pattern::identity())) {
    //     std::cout << pat;
    //     int row[6] = {0, 1, 2, 3, 4, 5};
    //     while (std::next_permutation(row, row + 6))
    //     {
    //         pattern tmp;
    //         for (int c = 0; c < 6; c++)
    //         {
    //             for (int r = 0; r < 6; r++)
    //                 tmp.arr[c][r] = pat.arr[c][row[r]];
    //         }
    //         tmp.lexicographic_order();
    //         if(pattern_set.find(tmp)!=pattern_set.end()) std::cout << "here\n";
    //     }

    //     pattern p2 = pat.pattern_mod();
    //     while (std::next_permutation(row, row + 6))
    //     {
    //         pattern tmp;
    //         for (int c = 0; c < 6; c++)
    //         {
    //             for (int r = 0; r < 6; r++)
    //                 tmp.arr[c][r] = p2.arr[c][row[r]];
    //         }
    //         tmp.lexicographic_order();
    //         if(pattern_set.find(tmp)!=pattern_set.end()) std::cout << "here\n";
    //     }


    //     std::exit(0);
    // }
    // return;

    if (pattern_set.find(pat) != pattern_set.end())
    {
        omp_set_lock(&lock);
        output_file << pat.name() << std::endl;
        erase_all_permutations(pat);
        // if(pat.name().length() > 3) {
        //     std::cout << pat.name() << "\n";
        //     // SO6 tmp = SO6::reconstruct(pat.name());
        //     // std::cout << tmp;
        // }
        omp_unset_lock(&lock);
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
    free_multiply_depth = target_T_count - stored_depth_max;

    // Load search criteria
    if (!pattern_file.empty())
    {
        std::cout << "[Read] Reading patterns from " << pattern_file << std::endl;
        readPattern();
        std::cout << "[Finished] Loaded " << pattern_set.size() << " non-identity patterns." << std::endl;
    }
    if (explicit_search_mode)
    {
        build_explicit_search_token();
        std::cout << "[Start] Generating lookup table for token=\n"
                  << search_token;
        build_explicit_search_set(explicit_search_depth);
        std::cout << "[Finished] Lookup table contains " << explicit_search_set.size() << " unique SO6s." << std::endl;
    }
}

static void explore_tree_rooted_at(SO6 &S)
{
    set<SO6> prior, next, current = set<SO6>({S});
    uint percent_complete;
    uint64_t iteration_counter;
    // for (int curr_T_count = 0; curr_T_count < stored_depth_max; curr_T_count++)
    for (int curr_T_count = 0; curr_T_count < 4; curr_T_count++)
    {
        percent_complete = 0;
        iteration_counter = 0;
        tcount_init_time = now();
        // std::cout << " ||\t[S] Beginning T=" << curr_T_count+1 << "\n ||\t[S] Processing .....    " << std::flush;

        // Loop over all T matrices
        for (SO6 S : current)
        {
            if ((int)100 * iteration_counter / current.size() - percent_complete > 1)
            {
                percent_complete = (int)100 * iteration_counter / current.size();
                if (percent_complete > 9)
                    std::cout << "\b";
                std::cout << "\b\b" << percent_complete << "\%" << std::flush;
            }
            iteration_counter++;

            // Multiply every element of curr by every T_i, might be faster if we looped over inside instead
            // for (uint T = 0; T < 15; T++)
            for (uint T = 0; T < 15; T++)
            {
                SO6 toInsert = S.left_multiply_by_T(T);
                if (next.insert(toInsert).second)
                {
                    erase_pattern(toInsert);
                    find_specific_matrix(toInsert);
                }
            }
        }
        // Report that we're done with the loop
        //  std::cout << "\b\b\b100\%" << std::endl;

        // Erase all elements that were repeated
        for (SO6 P : prior)
            next.erase(P);
        prior.clear();
        prior.swap(current); // T++
        current.swap(next);  // T++
        // std::cout << " ||\t↪ [FINISHED] Found " << current.size() << " new matrices in " << time_since(tcount_init_time) << "\n ||\t↪ [P] " << pattern_set.size() << " patterns remain.\n";
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
 * @param percent_complete percent complete at previous iteration
 */
static void report_percent_complete(const uint64_t &c, const uint64_t s, uint &percent_complete)
{
    if (std::floor((100 * c)/s)  > percent_complete)
    {
        percent_complete = 100 * c / s;                       // Update percent complete, can probably increment with ++
        std::cout << "\033[A\r ||\t↪ [Progress] Processing .....    " << percent_complete << "\%" << "\n ||\t↪ [Patterns] " << pattern_set.size() << " patterns remain." << std::flush;
        if(percent_complete == 100) std::cout << std::endl;
        return;
    }
    if(c == 0) {
        std::cout << " ||\t↪ [Progress] Processing .....    0\%" << "\n ||\t↪ [Patterns] " << pattern_set.size() << " patterns remain.";
        return;
    }
}

/**
 * @brief Function to report completion
 * @param matrices_found how many matrices were found
 */
static void report_done(const uint &matrices_found) {
    uint percent_complete = 99; // Any number < 100 is fine here.
    report_percent_complete(1, 1, percent_complete);
    if(matrices_found != 0) std::cout << " ||\t↪ [Finished] Found " << matrices_found << " new matrices in " << time_since(tcount_init_time) << "\n ||\n";
    else std::cout << " ||\t↪ [Finished] Total time: " << time_since(tcount_init_time) << "\n ||" << std::endl;
}

/**
 * @brief This method converts a set of SO6s to a vector of SO6s without using up too much space in the process
 * @param s origin set of type SO6
 * @param v target vector of type SO6
 */
static void set_to_vector(std::set<SO6> &s, std::vector<SO6> &v)
{
    std::cout << "[Begin] Converting current to vector to_compute.\n";
    // The following method could potentially be improved by impleminting increasingly large arrays, only necessary if we're really close to the memory limit
    while (!s.empty())
    {
        v.push_back(*s.begin());
        s.erase(s.begin());
    }
    v.shrink_to_fit(); // This vector will never grow again.

    std::cout << "[Randomize] Shuffling vector for (possible) efficiency.\n";
    std::random_shuffle(v.begin(), v.end()); // In principle, this guarantees that each thread has aboutan equal number of operations
    std::cout << "[End] Done.\n" << std::endl;
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

    int num_generating_sets = std::min(target_T_count - 1 - stored_depth_max, stored_depth_max - 1);

    SO6 root = SO6::identity();
    set<SO6> prior, next, current = set<SO6>({root});

    // This stores the generating sets. Note that the initial generating set is just the 15 T matrices and, thus, doesn't need to be stored
    std::vector<SO6> generating_set[num_generating_sets];

    // Begin reporting non-brute-force T counts
    std::cout << "\n\n[Begin] Generating T=1 through T=" << (int)stored_depth_max << " iteratively, but will only save ";
    if (free_multiply_depth == 2)
        std::cout << "T=2 and ";
    if (free_multiply_depth > 2)
        std::cout << "2≤T≤" << (int) free_multiply_depth << " and ";
    std::cout << "T=" << (int) stored_depth_max << " in memory\n ||\n";

    uint percent_complete;
    uint64_t count;
    int curr_T_count = 0;
    // std::filesystem::create_directory("./data");
    std::string file_string;
    while (curr_T_count < stored_depth_max)
    {
        report_begin_T_count(curr_T_count+1);
        file_string = "./data/" + to_string(curr_T_count+1) + ".dat";
        output_file.open(file_string, ios::out | ios::trunc);
        if(!output_file.is_open()) std::exit(0);
        std::cout << " ||\t↪ [Save] Opening file " << file_string << std::endl;

        percent_complete = 0;
        uint64_t count = 0;
        // for (int i = 0; i < iterate_over.size(); i++)
        for (int T = 0; T < 15; T++)
        {
            for(SO6 S : current)
            {
                report_percent_complete(count++, 15*current.size(), percent_complete);
                SO6 toInsert = S.left_multiply_by_T(T);
                if (next.insert(toInsert).second)
                { // The insertion operation appears to be the expensive part
                    if(T==0) erase_pattern(toInsert);            // Mildly inefficient, but okay.
                    // find_specific_matrix(toInsert);
                }
            }
        }
        // Erase all elements that were repeated
        for (SO6 P : prior) next.erase(P);

        // Shuffle storage around
        prior.clear();
        prior.swap(current); // T++
        current.swap(next);  // T++

        report_done(current.size());

        // Store cosets T_0{curr} only
        if (curr_T_count < free_multiply_depth-1)
        {
            // int index = std::min(curr_T_count - 1, num_generating_sets - 1);
            int index = std::min(curr_T_count, num_generating_sets);
            std::cout << "\033[A\r ||\t↪ [Save] Saving coset T₀{T="<< curr_T_count + 1<< "} as generating_set[" << index << "]\n ||" << std::endl;
            for (SO6 curr : current) {
                generating_set[index].push_back(curr.left_multiply_by_T(0));
            }
        }
        output_file.close();
        curr_T_count++;
    }
    prior.clear();
    next.clear();
    std::cout << " ||\n[End] Stored T=" << (int)stored_depth_max << " as current to generate T=" << stored_depth_max + 1 << " through T=" << (int)target_T_count << "\n" << std::endl;

    std::vector<SO6> to_compute;
    set_to_vector(current,to_compute);

    std::cout << "[Begin] Beginning brute force multiply.\n ||" << std::endl;
    uint64_t set_size = to_compute.size();
    uint64_t interval_size = std::ceil(set_size / THREADS); // Equally divide among threads, not sure how to balance but each should take about the same time

    while (curr_T_count < target_T_count)
    {    
        percent_complete = 0;
        report_begin_T_count(curr_T_count+1);
        file_string = "./data/" + to_string(curr_T_count+1) + ".dat";
        std::cout << " ||\t↪ [Save] Opening file " << file_string << std::endl;
        output_file.open(file_string, ios::out | ios::trunc);

        if (curr_T_count != stored_depth_max) std::cout << " ||\t↪ [Rep] Using generating_set[" << curr_T_count-stored_depth_max-1 << "]" << std::endl;
        else std::cout << " ||\t↪ [Rep] Left multiplying everything by T₀" << std::endl;
        omp_init_lock(&lock);
        #pragma omp parallel for schedule(static, interval_size) num_threads(THREADS)
        for (uint64_t i = 0; i < set_size; i++)
        {
            // if(percent_complete >= 1) continue;
            SO6 S = to_compute.at(i); // Get current SO6
            if (omp_get_thread_num() == THREADS - 1)
                report_percent_complete(i % interval_size, interval_size, percent_complete);

            // Do T multiplication the standard way to generate the first depth
            if (curr_T_count == stored_depth_max)
            {
                SO6 tmp = S.left_multiply_by_T(0);
                find_specific_matrix(tmp);
                erase_pattern(tmp);
                continue;
            }

            for (SO6 G : generating_set[curr_T_count-stored_depth_max - 1])
            {
                SO6 N = G*S;
                find_specific_matrix(N);
                erase_pattern(N);
            }
        }
        omp_destroy_lock(&lock);
        output_file.close();
        report_done(0);
        curr_T_count++;
    }
    std::cout << " ||\n[Finished] Free multiply complete.\n\n[Time] Total time elapsed: " << time_since(program_init_time) << std::endl;
    output_file.close();
    return 0;
}
