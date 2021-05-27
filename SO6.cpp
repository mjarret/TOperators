#include <algorithm>
#include <iostream>
#include <vector>
#include "Z2.hpp"
#include "SO6.hpp"

/**
 * Basic constructor. Initializes Zero matrix.
 *
 */
SO6::SO6(){
    name = "0";
    for(int i=0; i<6; i++){
        for(int j=0; j<6; j++)
            arr[i][j]=Z2();
    }
    genOneNorm();
}

/**
 * Constructor that initializes zero matrix with arbitrary name
 * @param n the name of the object
 */
SO6::SO6(std::string n){
    name = n;
    //initializes all entries as 0
    for(int i=0; i<6; i++){
        for(int j=0; j<6; j++)
            arr[i][j]=Z2();
    }
    genOneNorm();
}

bool lexLEQ(Z2* first, Z2* second) {
    for(int i = 0; i < 6 ; i++) {
        if(first[i].toFloat() < second[i].toFloat()) return true;
        if(first[i].toFloat() > second[i].toFloat()) return false;
    }
    // At this point they are the same
    return true;
}

/**
 * Constructor that initializes arbitrary matrix with arbitrary name
 * @param a array of Z2 that the SO6 will take as values
 * @param n the name of the object
 */
SO6::SO6(Z2 a[6][6], std::string n){
    name = n;
    Z2 null = Z2(0,0,0);
    //initializes SO6's entries according to a
    for(int i = 0; i<6; i++){
        int sign = 0;
        int j = 0;
        while(sign == 0) {
            if(a[i][j] == null) {
                arr[i][j] = null;
                j++;
                continue;
            }
            if(a[i][j].toFloat() > 0) sign = 1;
            else sign = -1;
            j++;
        }
        while(j<6) {
            if(sign == 1) arr[i][j] = a[i][j];
            else arr[i][j]=-a[i][j];
        }
    }
}

/**
 * Overloads the * operator with matrix multiplication for SO6 objects
 * @param other reference to SO6 to be multiplied with (*this)
 * @return matrix multiplication of (*this) and other
 */
SO6 SO6::operator*(SO6& other){
    //multiplies operators
    SO6 prod(name + other.getName());
    Z2 next;
    for(int i=0; i<6; i++){
        for(int j = 0; j<6; j++){
            for(int k = 0; k<6; k++){
                next = arr[i][k]*other(k,j);
                prod(i,j) += next;
            }
        }
    }
    prod.genOneNorm();
    return prod;
}

/** overloads == method to check equality up to signed column permutation
 *  @param other reference to SO6 to be checked against
 *  @return whether or not (*this) and other are equivalent up to signed column permutation
 */
bool SO6::operator==(SO6 &other) {
    // int tot;
    // bool flag;
    Z2 dot_product;
    Z2 next;
    for(int i = 0; i < 6; i++) {
        for(int j = 0; j < 6; j++) {
            dot_product = Z2(0,0,0);
            for(int k = 0; k <6; k++) {
                next = arr[i][k]*other(j,k);
                dot_product += next;
            }
            if(dot_product[2] != 0) return false;
        }
    }
    return true;
}

void SO6::genOneNorm()
{
    norm = Z2();
    for (int i = 0; i < 6; i++) {
        Z2 tmp = arr[0][i].abs();
        norm += tmp;
    }
}


//Returns true if and only if s and o are +/- column permutations of each other
bool SO6::isPerm(SO6 s) {
    bool perms[6] = { false, false, false, false, false, false };
    bool isMatch;
    for (int i = 0; i < 6; i++) {
        isMatch = false;                                        // Column i is presently unmatched
        for (int j = 0; j < 6; j++) {
            if(perms[j]) continue;                              // If we have already matched this column of s, don't look at it again
            
            // Check if they match with the same sign
            for(int k = 0; k < 6; k++) {
               if(arr[k][i] != s.arr[k][j]) break;              // We need them to match for every element.  
               if(k == 5) isMatch = true;                       // Only hit me if everything has matched
            }
            if(isMatch) {                                       // If we matched, we can move on at this point
                perms[j] = true;                                // Exclude j from future consideration
                break;                                          // Move to next value of i
            }

            // Check if they match with opposite sign, can be combined with above methods to read easier. This shouldn't hit with the way that things have currently been implemented in the Z2 constructor, but I'm worried about the T matrices or the matrices that result after matrix multiplication. Should also add a method for multiplying a Z2 by an int.
            for(int k = 0; k <6; k++) {
                if(-arr[k][i] != s.arr[k][j]) break;
                if(k == 5) isMatch = true;
            }

            if(isMatch) {
                perms[j] = true;
                break;
            }
        }    
        if(!isMatch) return false;                              //We completed a loop with no match, so return false at this point
    }
    return true;                                                // Everything was matched
}

/**
 * Generates LDE. Needs to be called any time the matrix is modified to make sure LDE stays current
 */
//void SO6::genLDE(){
//    int maximum = 0;
//    for(int i = 0; i<6; i++){
//        for(int j = 0; j<6; j++){
//            LDE = arr[i][j].getLDE();
//            maximum = std::max(LDE,maximum);
//        }
//    }
//    LDE = maximum;
//}

/**
 * Overloads << function for SO6.
 * @param os reference to ostream object needed to implement <<
 * @param m reference to SO6 object to be displayed
 * @returns reference ostream with the matrix's display form appended
 */
std::ostream& operator<<(std::ostream& os, const SO6& m){
    for(int i = 0; i<6; i++){
        os << '[';
        for(int j = 0; j<6; j++)
            os << m(i,j)<<' ';
        os << "] \n";
    }
    os << "\n";
    return os;
}


