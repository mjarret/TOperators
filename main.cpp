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

#include <algorithm>
#include <iostream>
#include <chrono>
#include <set>
#include <cstring>
#include "Z2.hpp"
#include "SO6.hpp"


using namespace std;

static int8_t tCount;                   //Default tCount = 0
static int8_t numThreads = 1;           //Default number of threads
static bool saveResults = false;        //Turn on to save data
static bool tIO = false;                //Turn this on if you want to read in saved data
long saveInterval = __LONG_MAX__;       //Default save interval
static chrono::duration<double> timeelapsed;
const SO6 identity = SO6::identity();

// /**
//  * @brief Reads in data that has been saved from a previous run of this program
//  * @param tc the tcount we want to read in up to
//  * @param tbase not sure what this does
//  * @return should return a current set of SO6 matrices generated at T count tc
//  */
// set<SO6> fileRead(int8_t tc, vector<SO6> tbase) {
//     ifstream tfile;
//     tfile.open(("data/T" + to_string(tc) + ".txt").c_str());
//     if(!tfile) {
//         cout << "File does not exist.\n";
//         exit(1);
//     }
//     set<SO6> tset;
//     char hist;
//     string mat; //Now unused except as a buffer
//     long i = 0;
//     vector<int8_t> tmp;
//     SO6 m;
//     //First line contains data for which iteration to start from, so skip it
//     getline(tfile, mat);
//     while(tfile.get(hist)) {
//         //Convert hex character to integer
//         tmp.push_back((hist >= 'a') ? (hist - 'a' + 10) : (hist - '0'));
//         if (++i%tc == 0) {
//             m = tbase.at(tmp.at(tmp.size() - 1));
//             for(int8_t k = tmp.size()-2; k > -1; k--) {
//                 m = tbase.at(tmp.at(k))*m;
//             }
//             tset.insert(m);
//             tmp.clear();
//         }
//     }
//     return tset;
// }

// /**
//  * @brief Method to write results to a file
//  * @param i index of T count being written out
//  * @param tsCount unsure what this does
//  * @param currentCount unsure what this does
//  * @param next also unsure
//  */
// void writeResults(int8_t i, int8_t tsCount, int8_t currentCount, set<SO6> &next) {
//     if(!saveResults) return;
//     auto start = chrono::high_resolution_clock::now();
//     string fileName = "data/T" + to_string(i+1) + ".tmp";
//     ofstream write = ofstream(fileName);
//     write << +tsCount << ' ' << +currentCount << '\n';
//     for(SO6 n : next) write<<n;
//     write.close();
//     std::rename(("data/T" + to_string(i+1) + ".tmp").c_str(), ("data/T" + to_string(i+1) + ".txt").c_str());
//     auto end = chrono::high_resolution_clock::now();
//     auto ret = chrono::duration_cast<chrono::milliseconds>(end-start).count();
//     cout<<">>>Wrote T-Count "<<(i+1)<<" to 'data/T"<<(i+1)<<".txt' in " << ret << "ms\n";
// }

/**
 * @brief Initialize command line arguments passed to main
 * @param argc Number of arguments
 * @param argv List of arguments
 */
void setParameters(int argc, char** &argv) {
    if(argc==1) {
            std::cout << "usage: main.out -tcount t [-v] [-threads n] [-save] [-saveinterval m] [-recover]\n\n";
            std::exit(EXIT_SUCCESS);
        }
    for(int i=1; i<argc; i++) {
        if(strcmp(argv[i], "-tcount") == 0) {
            tCount = std::stoi(argv[i+1]);
        } 
        if(strcmp(argv[i], "-v") == 0) {
            // This should be "verbose," but we don't have that ability yet
        }
        if(strcmp(argv[i], "-threads") == 0) {
            numThreads = std::stoi(argv[i+1]);
        }
        if(strcmp(argv[i], "-save") == 0) {
            saveResults = true;
        }
        if(strcmp(argv[i], "-saveinterval") == 0) {
            saveInterval = std::stol(argv[i+1]);
        }
        if(strcmp(argv[i], "-recover") == 0) {
            tIO = true;
        }
    }
}

void recursiveMultiply(SO6 prev, int len) {
    if(len == 0) return;
    for(int i = 0; i < 15; i++) {
        // SO6 next = prev.tMultiply(i);
        // int tmp = len-1;
        recursiveMultiply(prev.tMultiply(i),len-1);
    }
}



/**
 * @brief This is the main loop
 * @param argc number of command line arguments
 * @param argv vector of command line arguments
 * @return 0
 */
int main(int argc, char** argv){
    setParameters(argc,argv);                               // Initialize parameters to command line arguments
    auto program_init_time = chrono::high_resolution_clock::now();    // Begin timekeeping
    set<SO6> prior;                                         // Set of SO6s that will contain T count i-1
    set<SO6> current({SO6::identity()});                         // Current set of SO6s for current T count i, initialized to T count 0 = the identity circuit
    set<SO6> next;                                          // Set to hold the results of T count i+1

    SO6 tofind;
    tofind[2][0] = Z2(1,0,0);
    tofind[3][0] = Z2(1,0,0);
    tofind[4][0] = Z2(1,1,0);
    tofind[5][0] = Z2(1,1,0);
    tofind[2][1] = Z2(1,1,0);
    tofind[3][1] = Z2(1,1,0);
    tofind[4][1] = Z2(1,0,0);
    tofind[5][1] = Z2(1,0,0);
    tofind[0][2] = Z2(1,0,0);
    tofind[1][2] = Z2(1,1,0);
    tofind[3][2] = Z2(1,0,0);
    tofind[5][2] = Z2(1,1,0);
    tofind[0][3] = Z2(1,0,0);
    tofind[1][3] = Z2(1,1,0);
    tofind[2][3] = Z2(1,0,0);
    tofind[4][3] = Z2(1,1,0);
    tofind[0][4] = Z2(1,1,0);
    tofind[1][4] = Z2(1,0,0);
    tofind[3][4] = Z2(1,1,0);
    tofind[5][4] = Z2(1,0,0);
    tofind[0][5] = Z2(1,1,0);
    tofind[1][5] = Z2(1,0,0);
    tofind[2][5] = Z2(1,1,0);
    tofind[4][5] = Z2(1,0,0);
    tofind.lexOrder();


    for(int i = 0; i< tCount; i++){
        auto tcount_init_time = chrono::high_resolution_clock::now();
        std::cout<<"\nBeginning T-Count "<<(i+1)<<"\n";
            for(int l=0; l<15; l++) {
                for(SO6 curr : current) {
                    SO6 toInsert =curr.tMultiply(l); 
                    next.insert(toInsert);
                    // currentCount++;
                }
                if(i == tCount -1) break;    // We only need one T matrix at the final T-count
            }
            for(SO6 p : prior) next.erase(p); // Erase T-1
        auto end = chrono::high_resolution_clock::now();
        prior.clear();
        prior.swap(current);                                    // T++
        current.swap(next);                                     // T++
        // Begin reporting
        timeelapsed = chrono::high_resolution_clock::now() - tcount_init_time;
        std::cout << ">>> Found " << current.size() << " new matrices in " << chrono::duration_cast<chrono::milliseconds>(timeelapsed).count() << "ms\n";
        std::cout << ">>> Increase " << (float) current.size()/prior.size() << " times\n";
    }
    timeelapsed = chrono::high_resolution_clock::now() - program_init_time;
    std::cout<< "\nTotal time elapsed: " << chrono::duration_cast<chrono::milliseconds>(timeelapsed).count()<<"ms\n";
    // std::cout << Z2::count[0] << " " << Z2::count[1] << " " << Z2::count[2] << "\n";    // Useful for statistics tracking only
    return 0;
}
