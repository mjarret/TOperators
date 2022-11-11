#include <iostream>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include "pattern.hpp"

pattern::pattern() {}

int pattern::hamming_weight_A() {
    int ret;
    for(int i = 0; i<6; i++) {
        for(int j = 0; j<6; j++) {
            ret+= A[i][j];
        }
    }
    return 0;
}

int pattern::hamming_weight_B() {
    int ret;
    for(int i = 0; i<6; i++) {
        for(int j = 0; j<6; j++) {
            ret+= B[i][j];
        }
    }
    return 0;
}

bool pattern::operator==(const pattern &other) const {
    for(int col = 0; col < 6; col++) {
        for(int row = 0; row < 6; row++) {
            if(A[col][row] != other.A[col][row]) return false;
            if(B[col][row] != other.B[col][row]) return false;
        }
    }
    return true;
}

bool pattern::operator<(const pattern &other) const {
    for(int col = 0; col < 6; col++) {
        for(int row = 0; row < 6; row++) {
            // Ordering is 0< Sqrt[2]< 1< 1+Sqrt[2] 
            if(A[col][row] == other.A[col][row]) {
                if(B[col][row] == other.B[col][row]) continue;
                return B[col][row]<other.B[col][row];
            }
            return A[col][row] < other.A[col][row];
        }
    }
    return false;
}

bool pattern::less_than_perm(const int cp1[6], const int rp1[6], const int cp2[6], const int rp2[6]) const {
    for(int i = 0; i < 6; i++) {
        for(int j = 0; j < 6; j++) {
            // Ordering is 0< Sqrt[2]< 1< 1+Sqrt[2] 
            if(A[cp1[i]][rp1[j]] == A[cp2[i]][rp2[j]]) {
                if(B[cp1[i]][rp1[j]] == B[cp2[i]][rp2[j]]) continue;
                return B[cp1[i]][rp1[j]] < B[cp2[i]][rp2[j]];
            }
            return A[cp1[i]][rp1[j]] < A[cp2[i]][rp2[j]];
        }
    }
    return false;
}

int pattern::col_signature(const int &col) const {
    int ret = 0;
    for(int row=0; row<6; row++) ret+=(A[col][row]<<3) + B[col][row];
    return ret;
}

void pattern::lexicographic_sort() {
    int current_col_perm[6] = {0,1,2,3,4,5};
    int current_row_perm[6] = {0,1,2,3,4,5};
    int col_perm[6] = {0,1,2,3,4,5};
    int row_perm[6] = {0,1,2,3,4,5};
    while(std::next_permutation(col_perm,col_perm+6)) {
        while(std::next_permutation(row_perm,row_perm+6)) {
            if(less_than_perm(col_perm,row_perm,current_col_perm,current_row_perm)) {
                for(int i = 0; i<6; i++){
                        current_col_perm[i] = col_perm[i];
                        current_row_perm[i] = row_perm[i];
                }
            }
        }
    }
    pattern tmp;
    for(int col = 0; col<6; col++) {
        for(int row = 0; row<6; row++) {
            tmp.A[col][row] = A[current_col_perm[col]][current_row_perm[row]];
            // std::swap(A[col][row],A[current_col_perm[col]][current_row_perm[row]]);
            tmp.B[col][row] = B[current_col_perm[col]][current_row_perm[row]];
            // std::swap(B[col][row],B[current_col_perm[col]][current_row_perm[row]]);
        }
    }
    *this = tmp;
    // // First retain a vector of the largest signature columns
    // std::vector<int> smallest_cols = {0};
    // int smallest_signature = col_signature(0);

    // for(int col = 1; col<6; col++) {
    //     int current_signature = col_signature(col);
    //     if(current_signature <= smallest_signature) {
    //         for(int to_swap = col; to_swap >0; to_swap--) {
    //             std::swap(A[to_swap], A[to_swap-1]);
    //             std::swap(B[to_swap], B[to_swap-1]);
    //         }
    //         smallest_signature = current_signature;
    //     }
    // }

    // // We have identified the candidates for the smallest columns

    // if(pops == 1) {
    //     *this->row_sort_by_column(smallest_cols.pop_back(););
    // }
}

// void pattern::row_sort_by_column(const int & col) {

// }

/**
 * Overloads << function for SO6.
 * @param os reference to ostream object needed to implement <<
 * @param m reference to SO6 object to be displayed
 * @returns reference ostream with the matrix's display form appended
 */
std::ostream &operator<<(std::ostream &os, const pattern &m)
{
    os << "\n";
    for (int row = 0; row < 6; row++)
    {
        if (row == 0)
            os << "⌈ ";
        else if (row == 5)
            os << "⌊ ";
        else
            os << "| ";
        for (int col = 0; col < 6; col++)
            os << m.A[col][row] << ',' << m.B[col][row] << ' ';
        if (row == 0)
            os << "⌉\n";
        else if (row == 5)
        {
            os << "⌋\n";
        }
        else
        {
            os << "|\n";
        }
    }
    os << "\n";
    return os;
}