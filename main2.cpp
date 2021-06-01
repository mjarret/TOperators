/**
 * T Operator Product Generation Main File
 * @file main.cpp
 * @author Connor Mooney
 * @author Michael Jarret
 * @author Andrew Glaudell
 * @author Jacob Weston
 * @author Mingzhen Tian
 * @version 5/20/21
 */

#include <algorithm>
#include <iostream>
#include <iterator>
//#include <execution>
#include <vector>
// #include <thread>
// #include <future>
#include <fstream>
#include <chrono>
#include <set>
#include "Z2.hpp"
#include "SO6.hpp"


using namespace std;

// Identity here is lexicographically ordered.
SO6 identity() {
    SO6 I;
    for(int k =0; k<6; k++) {
        I(k,5-k) = Z2(1,0,0);
    }
    return I;
}

/**
 * Returns the SO6 object corresponding to T[i+1, j+1]
 * @param i the first index to determine the T matrix
 * @param j the second index to determine the T matrix
 * @return T[i+1,j+1]
 */
SO6 tMatrix(int i, int j){
    // Generates the T Matrix T[i+1, j+1]
    SO6 t;
    int sign;
    if((i+1==j&&i<=4&&i>=0)||(j+1==i&&j<=4&&j>=0))
        sign = 1;
    else
        sign = -1;
    for(int k=0; k<6; k++){
        t(k,k) = Z2(1,0,0);
    }
    t(i,i) = Z2(0,1,1);
    t(i,j) = Z2(0,-sign, 1);
    t(j,i) = Z2(0, sign, 1);
    t(j,j) = Z2(0,1,1);
    // t.fixSign();
    // t.lexOrder();
    return(t);
}

int main(){

    //Asking for number of threads and tCount to generate to
    // Seems to slow things down, so may remove this and just hardcode it in
    int numThreads, tCount;
    std::cout<<"How many threads would you like to utilize? Please enter here: ";
    cin>>numThreads;
    std::cout<<"To what T-Count do you want to generate? Please enter here: ";
    cin>>tCount;
    std::cout<<"\n";

    //timing
    auto tbefore = chrono::high_resolution_clock::now();
    //generating list of T matrices
    //in the order Andrew wanted
    SO6 I = identity();
    set<SO6> ts;
    for(int i = 0; i<15; i++) {
        if(i<5)       ts.insert(tMatrix(0,i+1));
        else if(i<9)  ts.insert(tMatrix(1, i-3));
        else if(i<12) ts.insert(tMatrix(2, i-6));
        else if(i<14) ts.insert(tMatrix(3, i-8));
        else          ts.insert(tMatrix(4,5));
    }
    auto tafter = chrono::high_resolution_clock::now();
    
    vector<SO6> tmp;
    for(SO6 n : ts) tmp.push_back(n);
    bool rej[15][15];
    for(int i = 0; i <15; i++) {
        for(int j = 0; j<15; j++) {
            SO6 first = tmp[i]*tmp[j];
            SO6 second = tmp[j]*tmp[i];
            rej[i][j] = (first==second);
        }
    }

    // exit(0);

    set<SO6> prior;            
    set<SO6> current({I});
    set<SO6> next;

    for(int i = 0; i<tCount; i++){
        std::cout<<"\nBeginning T-Count "<<(i+1)<<"\n";
        auto start = chrono::high_resolution_clock::now();
        next.clear();
        // Main loop here
        // for(int j = 0; j < 15; j++) {
        for(SO6 t : ts) {
            for(SO6 curr : current) {
                // if(rej[j][curr.prev]) continue;
                // SO6 toInsert = tmp[j]*curr;
                // toInsert.prev = j;
                // next.insert(toInsert);     // New product list for T + 1 stored as next
                next.insert(t*curr);
            }
            for(SO6 p : prior) next.erase(p);                // Erase T-1
        }
        // End main loop
        auto end = chrono::high_resolution_clock::now();   
        prior = current;                                    // T++
        current = next;                                     // T++

        // // Begin reporting
        // auto ret = chrono::duration_cast<chrono::milliseconds>(end-start).count();
        // std::cout << ">>>Found " << next.size() << " new matrices in " << ret << "ms\n";

        // vector<int> tmp;
        // for(int n = 0; n < 5; n++) 
        //     tmp.push_back(n);
        // vector<vector<int>> toOut;

        // start = chrono::high_resolution_clock::now();
        // string fileName = "./data/T" + to_string(i+1) + ".txt";
        // ofstream write = ofstream(fileName, std::ios_base::binary);
        // std::copy(tmp.rbegin(), tmp.rend(), std::ostream_iterator<int>(write, ","));        
        // end = chrono::high_resolution_clock::now();
        // ret = chrono::duration_cast<chrono::milliseconds>(end-start).count();
        // cout<<">>>Wrote T-Count "<<(i+1)<<" to 'T"<<(i+1)<<".txt' in " << ret << "ms\n";
    }
    chrono::duration<double> timeelapsed = chrono::high_resolution_clock::now() - tbefore;
    std::cout<< "\nTotal time elapsed: "<<chrono::duration_cast<chrono::milliseconds>(timeelapsed).count()<<"ms\n";
    return 0;
}
