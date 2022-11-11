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
#include "Z2.hpp"
#include "SO6.hpp"
#include "pattern.hpp"
#include <bitset>

using namespace std;

static int8_t tCount;                 // Default tCount = 0
static int8_t numThreads = 1;         // Default number of threads
static bool saveResults = false;      // Turn on to save data
static bool tIO = false;              // Turn this on if you want to read in saved data
static int8_t stored_depth_max = 127; // By default, you don't want to limit comparison depth
long saveInterval = __LONG_MAX__;     // Default save interval
static chrono::duration<double> timeelapsed;
static SO6 toFind; // in case we're searching for something in particularS
static bool verbose;
static set<SO6> pattern_set;

/**
 * @brief Initialize command line arguments passed to main
 * @param argc Number of arguments
 * @param argv List of arguments
 */
void setParameters(int argc, char **&argv)
{
    if (argc == 1)
    {
        std::cout << "usage: main.out -tcount t [-v] [-threads n] [-save] [-saveinterval m] [-recover] [-stored_depth s]\n\n";
        std::exit(EXIT_SUCCESS);
    }
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-tcount") == 0)
        {
            tCount = std::stoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-stored_depth") == 0)
        {
            stored_depth_max = std::stoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-v") == 0)
        {
            verbose = true;
            // This should be "verbose," but we don't have that ability yet
        }
        if (strcmp(argv[i], "-threads") == 0)
        {
            numThreads = std::stoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-save") == 0)
        {
            saveResults = true;
        }
        if (strcmp(argv[i], "-saveinterval") == 0)
        {
            saveInterval = std::stol(argv[i + 1]);
        }
        if (strcmp(argv[i], "-recover") == 0)
        {
            tIO = true;
        }
    }
}

void readPattern()
{
    string line;
    ifstream pattern_file("patterns.txt");
    if (pattern_file.is_open())
    {
        while (getline(pattern_file, line))
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
        pattern_file.close();
    }
}

/// @brief Recursive T matrix string multiplication. Inefficient.
/// @param prev
/// @param len
void recursiveMultiply(SO6 prev, int len)
{
    if (len == 0)
        return;
    for (int i = 0; i < 15; i++)
    {
        // SO6 next = prev.tMultiply(i);
        // int tmp = len-1;
        recursiveMultiply(prev.tMultiply(i), len - 1);
    }
}

/// @brief This sets a matrix that we're looking for the pattern of
/// this is currently done manually, but patterns can probably be stored
/// as a separate object that requires far less memory since they
/// only have 3 possible (unreduced) nonzero entries, Z(1,0,0), Z(1,1,0), Z(1,0,1)
/// Thus, we only need 2 bits per entry and can store the entire matrix in at most
/// 64 bits.
void setToFind()
{
    // toFind[0][0] = Z2(0,0,3);
    // toFind[1][0] = Z2(0,0,3);
    toFind[2][0] = Z2(1, 0, 3);
    toFind[3][0] = Z2(1, 0, 3);
    toFind[4][0] = Z2(1, 1, 3);
    toFind[5][0] = Z2(1, -1, 3);

    // toFind[0][1] = Z2(0,0,3);
    // toFind[1][1] = Z2(0,0,3);
    toFind[2][1] = Z2(1, -1, 3);
    toFind[3][1] = Z2(1, 1, 3);
    toFind[4][1] = Z2(-1, 0, 3);
    toFind[5][1] = Z2(-1, 0, 3);

    toFind[0][2] = Z2(1, 0, 3);
    toFind[1][2] = Z2(1, -1, 3);
    // toFind[2][2] = Z2(0,0,3);
    toFind[3][2] = Z2(1, 0, 3);
    // toFind[4][2] = Z2(0,0,3);
    toFind[5][2] = Z2(1, 1, 3);

    toFind[0][3] = Z2(1, 0, 3);
    toFind[1][3] = Z2(1, 1, 3);
    toFind[2][3] = Z2(1, 0, 3);
    // toFind[3][3] = Z2(0,0,3);
    toFind[4][3] = Z2(1, -1, 3);
    // toFind[5][3] = Z2(0,0,3);

    toFind[0][4] = Z2(1, 1, 3);
    toFind[1][4] = Z2(-1, 0, 3);
    // toFind[2][4] = Z2(0,0,3);
    toFind[3][4] = Z2(1, -1, 3);
    // toFind[4][4] = Z2(0,0,3);
    toFind[5][4] = Z2(-1, 0, 3);

    toFind[0][5] = Z2(1, -1, 3);
    toFind[1][5] = Z2(-1, 0, 3);
    toFind[2][5] = Z2(1, 1, 3);
    // toFind[3][5] = Z2(0,0,3);
    toFind[4][5] = Z2(-1, 0, 3);
    // toFind[5][5] = Z2(0,0,3);

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            toFind[i][j].reduce();
        }
    }
    toFind.hist.clear();
    toFind.fixSign();
    toFind.lexOrder();
}

static const std::unordered_map<std::string, std::chrono::milliseconds> suffix {
    {"ms", std::chrono::milliseconds{1}},
    {"s", std::chrono::seconds{1}},
    {"m", std::chrono::minutes{1}},
    {"h", std::chrono::hours{1}},
};

/**
 * @brief This is the main loop
 * @param argc number of command line arguments
 * @param argv vector of command line arguments
 * @return 0
 */
int main(int argc, char **argv)
{
    for(int i = 0; i<15; i++) {
        for(int j=0; j<15; j++) {
            SO6 identity = SO6::identity();
            SO6 tmp1 = identity.tMultiply(i);
            SO6 tmp2 = identity.tMultiplyTranspose(j);
            if(tmp1 == tmp2) {
                // std::cout<< i << "," << j << "\n";
            }
        }
    }
    auto program_init_time = chrono::high_resolution_clock::now(); // Begin timekeeping
    setParameters(argc, argv);                                     // Initialize parameters to command line arguments
    readPattern();
    setToFind();
    set<SO6> toFindSet({toFind}); // Sets the matrices we're looking for
    std::cout << "We are looking for the matrix: \n"
              << toFind << "\n";
    if (stored_depth_max < tCount / 2)
    { // Parameter check
        std::cout << "stored_depth bad\n";
        std::exit(EXIT_FAILURE);
    }

    for(int i = 0; i< 5; i++) {
        set<SO6> tmp = toFindSet;
        for (SO6 curr : toFindSet)
        {
            for(int l = 0; l<15;l++) tmp.insert(curr.tMultiply(l));
        }
        std::swap(toFindSet,tmp);
    }

    stored_depth_max = std::min((int)tCount - 1, (int)stored_depth_max);
    set<SO6> tc[(int)(tCount - stored_depth_max) - 1]; // Array of data we will need to generate the last few t counts, do not need T counts 0/1
    set<SO6> prior;
    set<SO6> current = set<SO6>({SO6::identity()});
    set<SO6> next;

    if (verbose)
        std::cout << "[Note] There are " << pattern_set.size() << " patterns to find.\n[Note] Removing identity pattern.\n";

    if (verbose) std::cout << "[Note] There are " << pattern_set.size() << " patterns to find.\n";
    std::cout << "\n\n[Begin] Starting to generate T=" << 1 << " through T=" << (int)stored_depth_max << " iteratively, but will only save t_0*(T<=" << tCount - stored_depth_max - 1 << ") generators and T=" << (int)stored_depth_max << " in memory\n ||\n";
    for (int i = 0; i < stored_depth_max; i++)
    {
        auto tcount_init_time = chrono::high_resolution_clock::now();
        std::cout << " ||\t[S] Beginning T=" << (i + 1) << "\n";
        std::cout << " ||\t[S] Processing .....   " << std::flush;
        for (int l = 0; l < 15; l++)
        {
            std::cout << "\b\b" << (int) 100*l/15 << "\%" << std::flush;
            for (SO6 curr : current)
            {   
                SO6 toInsert = curr.tMultiply(l);
                next.insert(toInsert);
                // if(pattern_set.erase(pat)) {
                //     pattern_set.erase(pat.pattern_mod());
                //     pat = pat.transpose();
                //     pattern_set.erase(pat);
                //     pattern_set.erase(pat.pattern_mod());
                // }
                if (toFindSet.find(toInsert) != toFindSet.end()) {
                    SO6 found = *toFindSet.find(toInsert);
                    std::cout << "\nFound the matrix we were looking for with name: " << toInsert.printName() << "\n";
                    std::cout << "\nGenerated from toFind by: " << found.printName() << "\n";
                    std::vector<int8_t> hist = found.hist;
                    while(!hist.empty()) {
                        SO6 tmp = toInsert;
                        toInsert = toInsert.tMultiplyTranspose(hist.back());
                        for(int ll = 0; ll < 15; ll++) {
                            if(tmp.tMultiply(ll) == toInsert) {
                                std::cout << ll << "\n";
                            }
                        }
                        hist.pop_back();
                    }
                    if(toInsert == toFind) {
                        std::cout << "toFind has name: " << toInsert.printName() << "\n";
                    }
                    std::exit(0);
                }
            }
            if((int)(100*l/15) > 9) std::cout << "\b";
        }
        std::cout << "\b\b\b100\%" << std::flush;
        for (SO6 p : prior)
            next.erase(p); // Erase T-1
        // set<SO6> toRemove;
        // for(SO6 p : pattern_set) {
        //     SO6 tmp;
        //     for(SO6 n : next) {
        //         SO6 pat = n.getPattern();
        //         if(p*pat == tmp) {
        //             std::cout << p;
        //             std::cout << pat;
        //             std::cin.get();
        //             toRemove.insert(p);
        //         }
        //     }
        // }
        // for(SO6 r : toRemove) {
        //     pattern_set.erase(r);
        // }

        //     if(pattern_set.find(pat) != pattern_set.end()) {
        //             pattern_set.erase(pat);
        //         }
        //     // while(std::next_permutation(row_order,row_order + 6)) {
        //     //     pat.row_permute(row_order);
        //     //     pat.lexOrder();
        //     //     if(pattern_set.find(pat) != pattern_set.end()) {
        //     //         pattern_set.erase(pat);
        //     //     }
        //     // }
        //     // pat = n.getPattern();
        //     // pat.transpose();
        //     // if(pattern_set.find(pat) != pattern_set.end()) {
        //     //         pattern_set.erase(pat);
        //     //     }
        //     // while(std::next_permutation(row_order,row_order + 6)) {
        //     //     pat.row_permute(row_order);
        //     //     pat.lexOrder();
        //     //     if(pattern_set.find(pat) != pattern_set.end()) {
        //     //         pattern_set.erase(pat);
        //     //     }
        //     // }
        // }
        prior.clear();
        prior.swap(current); // T++
        current.swap(next);  // T++
        if (i < tCount - stored_depth_max - 1)
        {
            for (SO6 curr : current)
            {
                tc[i].insert(curr.tMultiply(0)); // Insert this matrix * one T operator as tc[i].
            }
        }
        timeelapsed = chrono::high_resolution_clock::now() - tcount_init_time;
        std::cout << "\n ||\t↪ [C] Found " << current.size() << " new matrices in " << chrono::duration_cast<chrono::milliseconds>(timeelapsed).count() << "ms\n";
        std::cout << " ||\t↪ [P] " << pattern_set.size() << " patterns remain.\n";
        if (i < tCount - stored_depth_max - 1)
            std::cout << " ||\t ↪ [Saved] Stored t_0{T=" << i + 1 << "} as tc[" << i << "]: size=" << tc[i].size() << "\n";
    }
    std::cout << " ||\n[End] Stored T=" << (int)stored_depth_max << " as current set for generating T=" << stored_depth_max + 1 << " through T=" << (int)tCount << "\n";
    timeelapsed = chrono::high_resolution_clock::now() - program_init_time;

    if (verbose)
    {
        std::cout << "\n[Time] Total time elapsed generating SO6s: " << chrono::duration_cast<chrono::milliseconds>(timeelapsed).count() << "ms\n"
                  << "[Note] Completed generating all SO6s that will be stored in memory, beginning online generation.\n"
                  << "[Note] Eliminating unused sets.\n";
        std::cout << "[Note] Current T count to be used has " << current.size() << " total matrices.\n";
        int num_multiplications = 0;
        for (int j = 1; j < tCount - stored_depth_max + 1; j++)
        {
            num_multiplications += tc[j].size();
        }
        std::cout << "[Note] There are a total of " << num_multiplications << " stored matrices to multiply these by.\n"
                  << "[Note] Expecting " << num_multiplications * current.size() << " multiplications.\n";
    }

    if (tCount - stored_depth_max > stored_depth_max - 1)
    {
        current = tc[stored_depth_max - 1];
    }

    std::cout << "\n[Begin] Starting free multiply.\n ||\n";
    unsigned long counter = 0;

    auto free_multiply_init_time = chrono::high_resolution_clock::now();
    // #pragma omp parallel for
    for (int j = 0; j < tCount - stored_depth_max; j++)
    {
        auto tcount_init_time = chrono::high_resolution_clock::now();
        counter = 0;
        std::cout << " ||\t[S] Beginning T=" << j + stored_depth_max + 1 << "\n";
        std::cout << " ||\t[S] Processing";
        if (j == 0)
        {
            for(int l=0; l<15; l++) {
                std::cout << "·" << std::flush;
                for (SO6 curr : current)
                {                
                    SO6 pat = (curr.tMultiply(l)).getPattern();
                    if(pattern_set.erase(pat)) {
                        pattern_set.erase(pat.pattern_mod());
                        pat = pat.transpose();
                        pattern_set.erase(pat);
                        pattern_set.erase(pat.pattern_mod());
                    }
                    // if (toFindSet.find(tmp) != toFindSet.end())
                    // {
                    //     std::cout << "Found the matrix we were looking for\n";
                    //     std::cout << tmp << "\n";
                    //     std::exit(0);
                    // }
                    // SO6 tmp;
                    // set<SO6> toRemove;
                    // for(SO6 p : pattern_set) {
                    //     if(p*pat == tmp) toRemove.insert(p);
                    // }
                    // for(SO6 r : toRemove) {
                    //     pattern_set.erase(r);
                    // }
                    // counter++;
                }
            }
            timeelapsed = chrono::high_resolution_clock::now() - tcount_init_time;
            std::cout << "\n ||\t↪ [C] Finished " << counter << " t_0 multiplications to create T=" << 1 + stored_depth_max << " in " << chrono::duration_cast<chrono::milliseconds>(timeelapsed).count() << "ms\n";
            std::cout << " ||\t↪ [P] " << pattern_set.size() << " patterns remain.\n";
            continue;
        }
        for (SO6 left : tc[j - 1])
        {
            for (SO6 curr : current)
            {
                if (left * curr == toFind)
                    std::cout << "found it!\n";
                counter++;
            }
        }
        timeelapsed = chrono::high_resolution_clock::now() - tcount_init_time;
        std::cout << " ||\t↪ [C] Finished " << counter << " t_0{T=" << j << "} multiplications to create T=" << 1 + j + stored_depth_max << "in " << chrono::duration_cast<chrono::milliseconds>(timeelapsed).count() << "ms\n";
    }
    timeelapsed = chrono::high_resolution_clock::now() - free_multiply_init_time;
    std::cout << " ||\n[End] Free multiply complete.\n"
              << "\n[Time] Performed " << counter << " free multiplications in time: " << chrono::duration_cast<chrono::milliseconds>(timeelapsed).count() << "ms\n";

    timeelapsed = chrono::high_resolution_clock::now() - program_init_time;
    std::cout << "\n[Time] Total time elapsed: " << chrono::duration_cast<chrono::milliseconds>(timeelapsed).count() << "ms\n";

    // std::cout << Z2::count[0] << " " << Z2::count[1] << " " << Z2::count[2] << "\n";    // Useful for statistics tracking only
    return 0;
}
