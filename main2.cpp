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
#include <stdint.h>
#include <stdlib.h>
#include <omp.h>
#include "Z2.hpp"
#include "SO6.hpp"


using namespace std;

const int8_t numThreads = 1;
const int8_t tCount = 8;
const Z2 inverse_root2 = Z2::inverse_root2();

//Turn on to save data
const bool saveResults = true;

//Turn this on if you want to read in saved data
const bool tIO = false;
//If tIO true, choose which tCount to begin generating from:
const int8_t genFrom = tCount;

//Saves every saveInterval iterations
const long saveInterval = __LONG_MAX__;

SO6 identity() {
    SO6 I = SO6({-1});
    for(int8_t k =0; k<6; k++) {
        I(k,k) = 1;
    }
    I.lexOrder();
    return I;
}

/**
 * Returns the SO6 object corresponding to T[i+1, j+1]
 * @param i the first index to determine the T matrix
 * @param j the second index to determine the T matrix
 * @param matNum the index of the SO6 object in the base vector
 * @return T[i+1,j+1]
 */
const SO6 tMatrix(int8_t i, int8_t j, int8_t matNum) {
    // Generates the T Matrix T[i+1, j+1]
    SO6 t= SO6({matNum});                              
    for(int8_t k = 0; k < 6; k++) t[k][k] = 1;            // Initialize to the identity matrix
    t[i][i] = inverse_root2;                              // Change the i,j cycle to appropriate 1/sqrt(2)
    t[j][j] = inverse_root2;
    t[i][j] = inverse_root2;
    if(abs(i-j)!=1) t[i][j].negate();                     // Lexicographic ordering and sign fixing undoes the utility of this, not sure whether to keep it
    t[j][i] = -t[i][j];
    t.fixSign();
    t.lexOrder();
    return(t);
}

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

int main(){
    // Z2 tmp = Z2(7,0,3);
    // Z2 tmp2 = Z2(1,0,1);
    // tmp = tmp*tmp2;
    // std::cout << tmp << "\n";
    // tmp2 = Z2(3,0,2);
    // std::cout << "-" << tmp2 << "\n";
    // tmp += tmp2;
    // std::cout << tmp << "\n";
    // int x = -1;
    // x <<= 5;
    // std::cout << "x:" << x << "\n";
    // return 0;
    //timing
    auto tbefore = chrono::high_resolution_clock::now();

    set<SO6> prior;
    set<SO6> current({identity()});
    set<SO6> next;
    int8_t start = 0;
    
    vector<SO6> tsv; //t count 1 matrices
    for(int8_t i = 0; i<15; i++){
        if(i<5)         tsv.push_back(tMatrix(0,i+1,i));
        else if(i<9)    tsv.push_back(tMatrix(1, i-3,i));
        else if(i<12)   tsv.push_back(tMatrix(2, i-6,i));
        else if(i<14)   tsv.push_back(tMatrix(3, i-8,i));
        else            tsv.push_back(tMatrix(4,5,i));
        
    }

    bool reject[15][15];
    for(int i = 0; i<15; i++) {
        for(int j = 0; j<15; j++) {
            reject[i][j] = (tsv[i]*tsv[j] == identity());
            // std::cout << tsv[i] << "*" <<  tsv[j] << "\n\n";
            // std::cout << tsv[i]*tsv[j] << "\n";
            // if(i==0 && j==1) std::exit(EXIT_FAILURE);
        }
    }

    // return 0;

    if(tIO && tCount > 2) {
        prior = fileRead(genFrom-2, tsv);
        current = fileRead(genFrom-1, tsv);
        start = genFrom - 1;
    }
    
    for(int8_t i = start; i<tCount; i++){
        std::cout<<"\nBeginning T-Count "<<(i+1)<<"\n";

        auto start = chrono::high_resolution_clock::now();
        
        // Main loop here
        ifstream tfile;
        tfile.open(("data/T" + to_string(i + 1) + ".txt").c_str());

        int currentCount = 0;
        if (!tIO || !tfile) {
            if (tfile) tfile.close();

            for(SO6 t : tsv) {
                for(SO6 curr : current) {
                    next.insert(t*curr);     // New product list for T + 1 stored as next
                    currentCount++;
                    if(!(next.size() % saveInterval)) writeResults(i, t.getLast(), currentCount, next);
                }
                currentCount = 0;
                if(i == tCount -1) break;    // We only need one T matrix at the final T-count
            }
            for(SO6 p : prior) next.erase(p); // Erase T-1
            // Write results out
            writeResults(i, tsv.size(), currentCount, next); // This always finishes at tsv.size() unless we've completed our
        }
        else {
            next = fileRead(i+1, tsv);
            string str;
            getline(tfile, str);
            stringstream s(str);
            getline(s, str, ' ');
            int8_t tsCount = stoi(str);
            getline(s, str, ' ');
            currentCount = stoi(str);
            tfile.close();

            std::vector<SO6>::iterator titr = tsv.begin();
            std::set<SO6>::iterator citr = current.begin();
            advance(titr, tsCount);
            advance(citr, currentCount);
            SO6 t, curr;
            while (titr != tsv.end()) {
                t = *titr;
                while (citr != current.end()) {
                    curr = *citr;
                    if(reject[t.getLast()][curr.getLast()] && curr.getLast()!=-1) {
                        citr++;
                        continue;
                    }
                    next.insert(t*curr);
                    currentCount++;
                    if(!(next.size() % saveInterval)) writeResults(i, tsCount, currentCount, next);
                    citr++;
                }
                for(SO6 p : prior) next.erase(p);
                titr++;
                tsCount++;
                currentCount = 0;
                citr = current.begin();
            }
            writeResults(i, tsv.size(), currentCount, next); // This always finishes at tsv.size()
        }
        // End main loop
        auto end = chrono::high_resolution_clock::now();
        prior.clear();
        prior.swap(current);                                    // T++
        current.swap(next);                                     // T++
        // Begin reporting
        auto ret = chrono::duration_cast<chrono::milliseconds>(end-start).count();
        std::cout << ">>>Found " << current.size() << " new matrices in " << ret << "ms\n";
    }
    chrono::duration<double> timeelapsed = chrono::high_resolution_clock::now() - tbefore;
    std::cout<< "\nTotal time elapsed: "<<chrono::duration_cast<chrono::milliseconds>(timeelapsed).count()<<"ms\n";
    return 0;
}
