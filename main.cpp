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
#include <vector>
#include <thread>
#include <future>
#include <fstream>
#include <chrono>
#include <set>
#include <string>
#include <sstream>
#include <functional>
#include <cstring>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "Z2.hpp"
#include "SO6.hpp"


using namespace std;
const Z2 inverse_root2 = Z2::inverse_root2();

static int8_t tCount;                   //Default tCount = 0
static int8_t numThreads = 1;           //Default number of threads
static bool saveResults = false;        //Turn on to save data
static bool tIO = false;                //Turn this on if you want to read in saved data
long saveInterval = __LONG_MAX__;       //Default save interval
static chrono::duration<double> timeelapsed;

/**
 * @brief Reads in data that has been saved from a previous run of this program
 * @param tc the tcount we want to read in up to
 * @param tbase not sure what this does
 * @return should return a current set of SO6 matrices generated at T count tc
 */
set<SO6> fileRead(int8_t tc, vector<SO6> tbase) {
    ifstream tfile;
    tfile.open(("data/T" + to_string(tc) + ".txt").c_str());
    if(!tfile) {
        cout << "File does not exist.\n";
        exit(1);
    }
    set<SO6> tset;
    char hist;
    string mat; //Now unused except as a buffer
    long i = 0;
    vector<int8_t> tmp;
    SO6 m;
    //First line contains data for which iteration to start from, so skip it
    getline(tfile, mat);
    while(tfile.get(hist)) {
        //Convert hex character to integer
        tmp.push_back((hist >= 'a') ? (hist - 'a' + 10) : (hist - '0'));
        if (++i%tc == 0) {
            m = tbase.at(tmp.at(tmp.size() - 1));
            for(int8_t k = tmp.size()-2; k > -1; k--) {
                m = tbase.at(tmp.at(k))*m;
            }
            tset.insert(m);
            tmp.clear();
        }
    }
    return tset;
}

/**
 * @brief Method to write results to a file
 * @param i index of T count being written out
 * @param tsCount unsure what this does
 * @param currentCount unsure what this does
 * @param next also unsure
 */
void writeResults(int8_t i, int8_t tsCount, int8_t currentCount, set<SO6> &next) {
    if(!saveResults) return;
    auto start = chrono::high_resolution_clock::now();
    string fileName = "data/T" + to_string(i+1) + ".tmp";
    ofstream write = ofstream(fileName);
    write << +tsCount << ' ' << +currentCount << '\n';
    for(SO6 n : next) write<<n;
    write.close();
    std::rename(("data/T" + to_string(i+1) + ".tmp").c_str(), ("data/T" + to_string(i+1) + ".txt").c_str());
    auto end = chrono::high_resolution_clock::now();
    auto ret = chrono::duration_cast<chrono::milliseconds>(end-start).count();
    cout<<">>>Wrote T-Count "<<(i+1)<<" to 'data/T"<<(i+1)<<".txt' in " << ret << "ms\n";
}

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

    for(int i = 0; i< tCount; i++){
        auto tcount_init_time = chrono::high_resolution_clock::now();
        std::cout<<"\nBeginning T-Count "<<(i+1)<<"\n";
        // int currentCount = 0;    //Tracks number of inserts
        // ifstream tfile;
        // tfile.open(("data/T" + to_string(i + 1) + ".txt").c_str());
        
        // if (!tIO || !tfile) {
        //     if (tfile) tfile.close();
            for(int l=0; l<15; l++) {
                for(SO6 curr : current) {
                    next.insert(curr.tMultiply(l));
                    // currentCount++;
                }
                // currentCount = 0;
                if(i == tCount -1) break;    // We only need one T matrix at the final T-count
            }
            for(SO6 p : prior) next.erase(p); // Erase T-1
            // Write results out
            // writeResults(i, tsv.size(), currentCount, next); // This always finishes at tsv.size() unless we've completed our
        // }
        // else {
        //     // next = fileRead(i+1, tsv);
        //     // string str;
        //     // getline(tfile, str);
        //     // stringstream s(str);
        //     // getline(s, str, ' ');
        //     // int8_t tsCount = stoi(str);
        //     // getline(s, str, ' ');
        //     // currentCount = stoi(str);
        //     // tfile.close();

        //     // std::vector<SO6>::iterator titr = tsv.begin();
        //     // std::set<SO6>::iterator citr = current.begin();
        //     // advance(titr, tsCount);
        //     // advance(citr, currentCount);
        //     // SO6 t, curr;
        //     // while (titr != tsv.end()) {
        //     //     t = *titr;
        //     //     while (citr != current.end()) {
        //     //         curr = *citr;
        //     //         if(reject[t.getLast()][curr.getLast()] && curr.getLast()!=-1) {
        //     //             citr++;
        //     //             continue;
        //     //         }
        //     //         next.insert(t*curr);
        //     //         currentCount++;
        //     //         if(!(next.size() % saveInterval)) writeResults(i, tsCount, currentCount, next);
        //     //         citr++;
        //     //     }
        //     //     for(SO6 p : prior) next.erase(p);
        //     //     titr++;
        //     //     tsCount++;
        //     //     currentCount = 0;
        //     //     citr = current.begin();
        //     // }
        //     // writeResults(i, tsv.size(), currentCount, next); // This always finishes at tsv.size()
        // }
        // End main loop
        auto end = chrono::high_resolution_clock::now();
        prior.clear();
        prior.swap(current);                                    // T++
        current.swap(next);                                     // T++
        // Begin reporting
        timeelapsed = chrono::high_resolution_clock::now() - tcount_init_time;
        std::cout << ">>> Found " << current.size() << " new matrices in " << chrono::duration_cast<chrono::milliseconds>(timeelapsed).count() << "ms\n";
    }
    timeelapsed = chrono::high_resolution_clock::now() - program_init_time;
    std::cout<< "\nTotal time elapsed: " << chrono::duration_cast<chrono::milliseconds>(timeelapsed).count()<<"ms\n";
    // std::cout << Z2::count[0] << " " << Z2::count[1] << " " << Z2::count[2] << "\n";    // Useful for statistics tracking only
    return 0;
}
