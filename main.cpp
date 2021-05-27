/**
 * T Operator Product Generation Main File
 * @file main.cpp
 * @author Andrew Glaudell
 * @author Michael Jarret
 * @author Swan Klein
 * @author Connor Mooney
 * @author Mingzhen Tian
 * @author Jacob Weston
 * @version 5/25/21
 */

#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <unordered_map>
#include <future>
#include "Z2.hpp"
#include "SO6.hpp"


using namespace std;

//Generate up to this tCount
const int tCount = 4;

//These make up a linked list of SO6 objects
//It won't have many operations, it only needs to do a few things
struct Node {
    SO6 so6{};
    struct Node* next{};
};

//Creates the dummy head of a linked list
struct Node* createHead(SO6 so6) {
    struct Node* head = new Node();
    struct Node* node = new Node();
    head->so6 = SO6();
    head->next = node;
    node->so6 = so6;
    node->next = NULL;
    return head;
}

//Push a node to the spot after the dummy head
void push(struct Node* &head, SO6 so6) {
    struct Node* node = new Node();
    node->so6 = so6;
    node->next = head->next;
    head->next = node;
}

/**
 * Returns the SO6 object corresponding to T[i+1, j+1]
 * @param i the first index to determine the T matrix
 * @param j the second index to determine the T matrix
 * @return T[i+1,j+1]
 */

SO6 tMatrix(int i, int j) {
    // Generates the T Matrix T[i+1, j+1]
    SO6 t("T(" + to_string(i) + "," + to_string(j) + ")");
    int sign;
    if ((i + 1 == j && i <= 4 && i >= 0) || (j + 1 == i && j <= 4 && j >= 0))
        sign = 1;
    else
        sign = -1;
    for (int k = 0; k < 6; k++) {
        t(k, k) = Z2(1, 0, 0);
    }
    t(i, i) = Z2(0, 1, 1);
    t(i, j) = Z2(0, -sign, 1);
    t(j, i) = Z2(0, sign, 1);
    t(j, j) = Z2(0, 1, 1);
    t.genOneNorm();
    return(t);
}

//Appends an SO6 matrix to its 1-norm's linked list
void mapAppend(unordered_map<float, struct Node*> &map, SO6 so6) {
    float norm = so6.normFloat();
    auto head = map.find(norm);
    if (head != map.end()) {
        //Head is not actually the head, it is an iterator that
        //points to a pair containing the head
        push(head->second, so6);
    }
    else {
        map[norm] = createHead(so6);
    }
}

/**
 * Returns the product of each matrix in t with each matrix in s
 * @param t an unordered_map of SO6 objects
 * @param s an unordered_map of (the 15 base) SO6 objects
 * @return an unordered_map of the products of matrices in t and s
 */
unordered_map<float, struct Node*> genProds(unordered_map<float, struct Node*> t, unordered_map<float, struct Node*> s) {
    unordered_map<float, struct Node*> prod;
    std::unordered_map<float, struct Node*>::iterator titr = t.begin();
    while (titr != t.end()) {
        struct Node* tnode = titr++->second->next;
        while (tnode != NULL) {
            std::unordered_map<float, struct Node*>::iterator sitr = s.begin();
            while (sitr != s.end()) {
                struct Node* snode = sitr++->second->next;
                while (snode != NULL) {
                    mapAppend(prod, snode->so6 * tnode->so6);
                    snode = snode->next;
                }
            }
            tnode = tnode->next;
        }
    }
    return prod;
}

/**
 * Deletes elements unique up to signed column permutations
 * @param t an unordered_map of T-Count N SO6 objects to prune
 * @param s an unordered_map of T-Count N-2 SO6 objects
 */
void prunePerms(unordered_map<float, struct Node*> t, unordered_map<float, struct Node*> t2) {
    std::unordered_map<float, struct Node*>::iterator titr = t.begin();
    while (titr != t.end()) {
        struct Node* tnode = titr++->second;
        bool nextDelete = false; //Don't go to the next node if the previous next node was deleted
        //Compare with self and with t2
        while (tnode->next != NULL) {
            struct Node* cnode = tnode->next;
            while (cnode->next != NULL) {
                if (tnode->next->so6.isPerm(cnode->next->so6)) {
                    cnode->next = cnode->next->next;
                    nextDelete = true;
                }
                if (!nextDelete) {
                    cnode = cnode->next;
                }
                else {
                    nextDelete = false;
                }
            }
            float norm = tnode->next->so6.normFloat();
            auto head = t2.find(norm);
            if (head != t2.end()) {
                cnode = head->second;
                while (cnode->next != NULL) {
                    if (tnode->next->so6.isPerm(cnode->next->so6)) {
                        tnode->next = tnode->next->next;
                        nextDelete = true;
                        break;
                    }
                    cnode = cnode->next;
                }
            }
            if (!nextDelete) {
                tnode = tnode->next;
            }
            else {
                nextDelete = false;
            }
        }
    }
}

//Writes all matrices of a T-count to the file given as input
void writeSO6(unordered_map<float, struct Node*> t, string fileName) {
    ofstream write = ofstream(fileName);
    std::unordered_map<float, struct Node*>::iterator itr = t.begin();
    int matrixCount = 0; //For checking results against other programs
    while (itr != t.end()) {
        struct Node* node = itr++->second->next;
        while (node != NULL) {
            write << node->so6.getName() << "\n";
            write << node->so6;
            matrixCount++;
            node = node->next;
        }
    }
    cout << fileName << " vector count : " << to_string(matrixCount) << "\n";
    write.close();
 }

int main() {

    string fileName = "T1.txt";
    //The keys of this map are the norm of the first row of the SO6 in the linked list
    unordered_map<float, struct Node*> t[tCount];

    //timing
    auto tbefore = chrono::high_resolution_clock::now();
    //generating list of T matrices in the order Andrew wanted
    for (int i = 0; i < 15; i++) {
        if (i < 5)
            mapAppend(t[0],tMatrix(0, i + 1));
        else if (i < 9)
            mapAppend(t[0],tMatrix(1, i - 3));
        else if (i < 12)
            mapAppend(t[0],tMatrix(2, i - 6));
        else if (i < 14)
            mapAppend(t[0],tMatrix(3, i - 8));
        else
            mapAppend(t[0],tMatrix(4, 5));
    }
    cout << "Generated T count 1 \n";

    writeSO6(t[0], fileName);
    cout << "Wrote T-Count 1 to '" << fileName << "' \n\n";

    //Generating Higher T-Counts
    for (int i = 1; i < tCount; i++) {
        fileName = "T" + to_string(i + 1) + ".txt";
        t[i] = genProds(t[i - 1], t[0]);
        prunePerms(t[i], (i > 1) ? t[i - 2] : unordered_map<float, struct Node*>{});
        cout << "Generated T-count " << (i + 1) << "\n";
        writeSO6(t[i], fileName);
        cout << "Wrote T-Count " << (i + 1) << " to '" << fileName << "' \n\n";
    }

    //Time display and exit message
    chrono::duration<double> timeelapsed = chrono::high_resolution_clock::now() - tbefore;
    cout << "Time elapsed to generate up to T-count " << tCount << ": " << timeelapsed.count() << "\n";
    cout << "Press any character and then Enter to continue...";
    string i;
    cin >> i;
    return 0;
}