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
#include "SO6.hpp"
#include "pattern.hpp"
#include <bitset>

using namespace std;

static uint8_t target_T_count;                  // Default target_T_count = 0
static uint8_t numThreads = 1;                  // Default number of threads
static bool saveResults = false;                // Turn on to save data
static uint8_t stored_depth_max = 255;          // By default, you don't want to limit comparison depth
static chrono::duration<double> timeelapsed;
static SO6 search_token;                        // in case we're searching for something in particular
static bool verbose;
static set<SO6> pattern_set;                    // will hold the set of patterns
static std::string pattern_file = "";           // pattern file
static set<SO6> explicit_search_set;            // Sets the matrices we're looking for
static uint8_t explicit_search_depth = 0;       // 
bool transpose_multiply = false;
bool explicit_search_mode = false;
static uint8_t free_multiply_depth = 1;

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
            SO6 tmp;
            pattern pat;
            for (int col = 0; col < 6; col++)
            {
                for (int row = 0; row < 6; row++)
                {
                    int a = (int)line_in_binary[2 * col + 12 * row];
                    pat.A[col][row] = line_in_binary[2 * col + 12 * row];
                    int b = (int)line_in_binary[2 * col + 12 * row + 1];
                    pat.B[col][row] = line_in_binary[2 * col + 12 * row];
                    tmp[col][row] = Z2(a, b, 0);
                }
            }
            tmp.lexOrder();
            pattern_set.insert(tmp);
            pattern_set.insert(tmp.pattern_mod());
            tmp = tmp.transpose();
            pattern_set.insert(tmp);
            pattern_set.insert(tmp.pattern_mod());
        }
        SO6 identity = SO6::identity();
        pattern_set.erase(identity);
        pattern_set.erase(identity.pattern_mod());
        pf.close();
    }
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
    search_token.lexOrder();
    if(verbose) std::cout << "[Note:] We are searching for search_token=\n" << search_token << "\n";
}

static void build_explicit_search_set(int depth)
{
    explicit_search_set.insert(search_token);
    for(int i = 0; i< depth; i++) {
        set<SO6> tmp = explicit_search_set;
        for (SO6 curr : explicit_search_set)
        {
            for(int l = 0; l<15;l++) tmp.insert(curr.left_multiply_by_T(l));
        }
        std::swap(explicit_search_set,tmp);
    }
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
            pattern_file = argv[i+1];
        }        
        if (strcmp(argv[i], "-v") == 0)
        {
            verbose = true;
        }
        if (strcmp(argv[i], "-threads") == 0)
        {
            numThreads = std::stoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-save") == 0)
        {
            saveResults = true;
        }
        if (strcmp(argv[i], "-transpose_multiply") == 0)
        {
            transpose_multiply=true;
        }
        if (strcmp(argv[i], "-explicit_search") == 0)
        {
            explicit_search_mode=true;
            explicit_search_depth= std::stoi(argv[i + 1]);
        }
    }
}

/**
 * @brief Erases the pattern of an SO6 from pattern_set
 * @param s the SO6 to be erased
 */
static void erase_pattern(SO6 &s) {
    s = s.getPattern();
    if(pattern_set.erase(s)) {
        pattern_set.erase(s.pattern_mod());
        s = s.transpose();
        pattern_set.erase(s);
        pattern_set.erase(s.pattern_mod());
    }        
}



/**
 * @brief Checks whether an SO6 is one that we are explicitly searching for
 * @param s the SO6 we're attempting to find
 */
static void find_specific_matrix(SO6 &s) {
    if (!explicit_search_mode) return;
    if (explicit_search_set.find(s) != explicit_search_set.end()) {
        SO6 found = *explicit_search_set.find(s);
        std::cout << "\n[Break] Found the matrix we were looking for with name: " << s.printName() << "\n";
        std::cout << "\n[Break] Generated from search_token by: " << found.printName() << "\n";
        std::vector<uint8_t> hist = found.hist;
        while(!hist.empty()) {
            s = s.left_multiply_by_T_transpose(hist.back());
            hist.pop_back();
        }
        if(s == search_token) {
            std::cout << "[Break] Search_token has name: " << s.printName() << "\n[Break] Exiting." << std::endl;
        }
        std::exit(0);      
    }
}

static std::string time_since(std::chrono::_V2::high_resolution_clock::time_point &s) {
    chrono::duration<double> duration = chrono::_V2::high_resolution_clock::now()-s;
    int64_t time = chrono::duration_cast<chrono::milliseconds>(duration).count();
    if(time < 1000) return std::to_string(time).append("ms");
    if(time < 60000) return std::to_string((float) time/1000).substr(0,5).append("s");
    if(time < 3600000) return std::to_string((float) time/60000).substr(0,5).append("min");
    if(time < 86400000) return std::to_string((float) time/3600000).substr(0,5).append("hr");
    return std::to_string((float) time/86400000).substr(0,5).append("days");
}

static void configure() {
    stored_depth_max = std::min((int)target_T_count - 1, (int)stored_depth_max);
    
    if (stored_depth_max < target_T_count / 2)
    { // Parameter check
        std::cout << "stored_depth bad\n";
        std::exit(EXIT_FAILURE);
    }
    free_multiply_depth = target_T_count-stored_depth_max;

    // Load search criteria
    if(!pattern_file.empty()) {
        std::cout << "[S] Reading patterns from " << pattern_file << std::endl;
        readPattern();
        std::cout << "[C] Loaded " << pattern_set.size() << " non-identity patterns." << std::endl;
    }
    if(explicit_search_mode) {
        build_explicit_search_token();
        std::cout << "[S] Generating lookup table for token=\n" << search_token;
        build_explicit_search_set(explicit_search_depth);
        std::cout << "[C] Lookup table contains " << explicit_search_set.size() << " unique SO6s." << std::endl;
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
    auto program_init_time = chrono::_V2::high_resolution_clock::now(); // Begin timekeeping
    setParameters(argc, argv);  // Initialize parameters to command line argument
    configure();                // Configure the search

    int num_generating_sets = target_T_count - stored_depth_max;              // T count 1 can be done with .tMultiply
    if(num_generating_sets>stored_depth_max-1) num_generating_sets=stored_depth_max-1;

    set<SO6> prior;
    set<SO6> current = set<SO6>({SO6::identity()});
    set<SO6> next;

    // This stores the generating sets. Note that the initial generating set is just the 15 T matrices and, thus, doesn't need to be stored
    std::vector<SO6> generating_set[num_generating_sets];
    
    // Begin reporting non-brute-force T counts
    std::cout << "\n\n[Begin] Generating T=1 through T=" << (int)stored_depth_max << " iteratively, but will only save ";
    if(free_multiply_depth == 2) std::cout << "T=2 and "; 
    if(free_multiply_depth > 2) std::cout << "2≤T≤" << (int) free_multiply_depth << " and ";
    std::cout << "T=" << (int)stored_depth_max << " in memory\n ||\n";

    uint percent_complete;
    uint64_t count;
    std::chrono::_V2::high_resolution_clock::time_point tcount_init_time;
    for (int curr_T_count = 0; curr_T_count < stored_depth_max; curr_T_count++)
    {
        percent_complete = 0;
        count = 0;
        tcount_init_time = chrono::_V2::high_resolution_clock::now();
        std::cout << " ||\t[S] Beginning T=" << curr_T_count+1 << "\n ||\t[S] Processing .....    " << std::flush;
        
        // Loop over all T matrices
        for (SO6 S : current)
        {
            if((int) 100*count/current.size()-percent_complete > 4) {
                percent_complete = (int) 100*count/current.size();
                if(percent_complete > 9) std::cout << "\b";
                std::cout << "\b\b" << percent_complete << "\%" << std::flush;
            }
            count++;

            // Multiply every element of curr by every T_i, might be faster if we looped over inside instead
            for (int T = 0; T < 15; T++)
            {   
                SO6 toInsert = S.left_multiply_by_T(T);
                if(next.insert(toInsert).second) {
                    erase_pattern(toInsert);
                    if(transpose_multiply) {
                        toInsert = S.left_multiply_by_T_transpose(T);
                        if(next.insert(toInsert).second) erase_pattern(toInsert);
                    }
                    find_specific_matrix(toInsert);
                }
            }
        }
        //Report that we're done with the loop
        std::cout << "\b\b\b100\%" << std::endl;

        // Erase all elements that were repeated
        for (SO6 P : prior) next.erase(P); 
        prior.clear();
        prior.swap(current); // T++
        current.swap(next);  // T++
        std::cout << " ||\t↪ [C] Found " << current.size() << " new matrices in " << time_since(tcount_init_time) << "\n ||\t↪ [P] " << pattern_set.size() << " patterns remain.\n";
        
        // save_to_generating_set(current, i-1);
        // if((0 < curr_T_count && curr_T_count < free_multiply_depth) || curr_T_count == stored_depth_max-1) {
        if((0 < curr_T_count && curr_T_count < free_multiply_depth)) {
            int index = std::min(curr_T_count-1,num_generating_sets-1);
            std::cout << " ||\t↪ [S] Saving as generating_set[" << index << "]\n";
            for(SO6 curr : current) generating_set[index].push_back(curr);
        }
    }
    prior.clear();
    // current.clear();
    next.clear();
    std::cout << " ||\n[End] Stored T=" << (int)stored_depth_max << " as generating_set[" << num_generating_sets-1<< "] to generate T=" << stored_depth_max + 1 << " through T=" << (int)target_T_count << "\n\n[Begin] Starting free multiply.\n ||\n";
   
    for (int curr_T_count = 0; curr_T_count < target_T_count - stored_depth_max; curr_T_count++)
    {
        // Bookkeeping
        percent_complete = 0;
        count = 0;
        tcount_init_time = chrono::high_resolution_clock::now();
        std::cout << " ||\t[S] Beginning T=" << (curr_T_count + stored_depth_max+1) << "\n ||\t[S] Processing .....  " << percent_complete << "\%" << std::flush;

        // Main loop
        // for (int i = 0; i < generating_set[num_generating_sets-1].size(); i++) {
        //     SO6 S = generating_set[num_generating_sets-1][i];
        // Track completion in increments of 5%
        // if((int) 100*i/generating_set[num_generating_sets-1].size()-percent_complete > 4) {
        //     percent_complete = (int) 100*i/generating_set[num_generating_sets-1].size();
        //     if(percent_complete > 9) std::cout << "\b";
        //     std::cout << "\b\b" << percent_complete << "\%" << std::flush;
        // }

        for (SO6 S : current) {
            // Track completion in increments of 5%
            if((int) 100*count/current.size()-percent_complete > 4) {
                percent_complete = (int) 100*count/current.size();
                if(percent_complete > 9) std::cout << "\b";
                std::cout << "\b\b" << percent_complete << "\%" << std::flush;
            }
            count++;

            // Do T multiplication the standard way to generate the first depth
            if(curr_T_count == 0) {
                for(uint T = 0; T<15; T++) {
                    SO6 tmp = S.left_multiply_by_T(T);
                    find_specific_matrix(tmp);
                    erase_pattern(tmp);
                }
                continue;
            }

            // Do multiplication by stored matrices
            for(SO6 G : generating_set[curr_T_count-1]) {
            // for(int it = 0; it<generating_set[curr_T_count-1].size(); it++) {
                SO6 tmp = G*S;
                find_specific_matrix(tmp);
                erase_pattern(tmp);
            }
        }
        std::cout << "\b\b\b100\%\n ||\t↪ [P] " << pattern_set.size() << " patterns remain.\n ||\t↪ [C] Completed in " << time_since(tcount_init_time) << "\n";
    }
    std::cout << " ||\n[End] Free multiply complete.\n\n[Time] Total time elapsed: " << time_since(program_init_time) << "\n";
    return 0;
}
