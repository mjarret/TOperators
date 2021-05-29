#include <algorithm>
#include <iostream>
#include <vector>
#include "Z2.hpp"
#include "SO6.hpp"

/**
 * Method to compare two Z2 arrays of length 6 lexicographically
 * @param first array of Z2 of length 6
 * @param second array of Z2 of length 6
 * @return -1 if first < second, 0 if equal, 1 if first > second
 */
bool lexLess (Z2* first, Z2* second) {
    for(int i = 0; i < 6 ; i++) {
        if(first[i] < second[i]) return true;
        if(second[i] < first[i]) return false;
    }
    return false;
}

/**
 * Basic constructor. Initializes Zero matrix.
 *
 */
SO6::SO6(){
    for(int i=0; i<6; i++){
        for(int j=0; j<6; j++)
            arr[i][j]=Z2();
    }
}

// /**
//  * Constructor that initializes zero matrix with arbitrary name
//  * @param n the name of the object
//  */
// SO6::SO6(std::string n){
//     name = n;
//     //initializes all entries as 0
//     for(int i=0; i<6; i++){
//         for(int j=0; j<6; j++)
//             arr[i][j]=Z2();
//     }
//     genOneNorm();
// }

/**
 * Constructor that initializes arbitrary matrix with arbitrary name
 * @param a array of Z2 that the SO6 will take as values
 * @param n the name of the object
 */
SO6::SO6(Z2 a[6][6]){
    //initializes SO6's entries according to a
    for(int col = 0; col<6; col++){
        short sign = 0;
        short row = 0;
        while(sign == 0) {
            if(a[row][col] == 0) {
                arr[col][row] = Z2(0,0,0);
                row++;
                continue;
            }
            if(a[row][col] < 0) {
                sign = -1;
                arr[col][row] = -a[row][col];
            }
            else {
                sign = 1;
                arr[col][row] = a[row][col];
            }
            row++;
        }
        while(row<6) {
            if(sign == 1) arr[col][row] = a[row][col];
            else arr[col][row]=-a[row][col];
            row++;
        }
    }

    for(int row= 0; row < 6; row++) {
        for(int col = 0; col < 6; col++) {
            std::cout << arr[col][row].toFloat() << ",";
        }
        std::cout << std::endl << std::endl;
    }
    sort();
    std::cout << "***********************************" << std::endl;
    for(int row= 0; row < 6; row++) {
        for(int col = 0; col < 6; col++) {
            std::cout << arr[col][row].toFloat() << ",";
        }
        std::cout << std::endl << std::endl;
    }
}

/**
 * Overloads the * operator with matrix multiplication for SO6 objects
 * @param other reference to SO6 to be multiplied with (*this)
 * @return matrix multiplication of (*this) and other
 */
SO6 SO6::operator*(SO6& other){
    //multiplies operators assuming COLUMN,ROW indexing
    SO6 prod;
    Z2 next;
    for(int i=0; i<6; i++){
        for(int j = 0; j<6; j++){
            for(int k = 0; k<6; k++){
                next = arr[k][i]*other(j,k);
                prod(j,i) += next;
            }
        }
    }
    prod.fixSign();
    prod.sort();
    return prod;
}

void SO6::fixSign() {
    for(int col = 0; col<6; col++){
        short sign = 0;
        short row = 0;
        while(sign == 0) {
            if(arr[col][row] == Z2(0,0,0)) {
                row++;
                continue;
            }
            if(arr[col][row] < 0) {
                sign = -1;
                arr[col][row] = -arr[col][row];
            }
            else {
                sign = 1;
                arr[col][row] = arr[col][row];
            }
            row++;
        }
        while(row<6) {
            if(sign == 1) arr[col][row] = arr[col][row];
            else arr[col][row]=-arr[col][row];
            row++;
        }
    }
}

void SO6::sort() {
    Z2* myZ2[] = {arr[0],arr[1],arr[2],arr[3],arr[4],arr[5]};
    std::vector<Z2*> myvector(myZ2,myZ2+6);
    std::sort (myvector.begin(), myvector.end(),lexLess);
    Z2 arr2[6][6];
    for(int i = 0; i<6; i++) {
        for(int j = 0; j<6; j++) {
            arr2[i][j] = (myvector.at(i))[j];
        }
    }
    for(int i = 0; i<6; i++) {
        for(int j = 0; j<6; j++) {
            arr[i][j] = arr2[i][j];
        }
    }
}

bool SO6::operator<(SO6 &other) {
    for(int i = 0; i < 6; i++) {
        if(lexLess(arr[i], other.arr[i])) return true;
        if(lexLess(other.arr[i], arr[i])) return false;
    }
    return false;
}

/** overloads == method to check equality of SO6 matrices
 *  @param other reference to SO6 to be checked against
 *  @return whether or not (*this) and other are equivalent
 */
bool SO6::operator==(SO6 &other) {
    // SO6 are the same if any triangle is the same
    // TODO: maybe figure out which triangle (upper,lower,right,left) exits fastest on average
    //       my guess is that because of sorting, lower triangles should exit faster than upper triangles
    //       since we sort with the left, it may also be the case that the lower right triangle exists faster than lower left
    for(int col = 5; col>-1 ; col--) {                                
        for(int row = 5; row>5-col-1; row--) {                        
            if(arr[col][row]!=other.arr[col][row]) return false;
        }
    }
    return true;
}

bool SO6::operator%(SO6 &other) {
    return false;
}

// /** overloads == method to check equality up to signed column permutation
//  *  @param other reference to SO6 to be checked against
//  *  @return whether or not (*this) and other are equivalent up to signed column permutation
//  */
// bool SO6::operator==(SO6 &other) {
//     bool perms[6] = { false, false, false, false, false, false };
//     bool isMatch;
//     for (int i = 0; i < 6; i++) {
//         isMatch = false;                                                                            // Column i is presently unmatched
//         for (int j = 0; j < 6; j++) {
//             if(perms[j]) continue;                                                                  // If we have already matched this column of s, don't look at it again
//             isMatch = std::equal(std::begin(arr[i]),std::end(arr[i]),std::begin(other.arr[j]));     // Check if the columns are equal
//             if(isMatch) {
//                 perms[j]=true;                                                                      // Exclude j in the future
//                 break;
//             }
//         }    
//         if(!isMatch) return false;                              //We completed a loop with no match, so return false at this point
//     }
//     return true;                                                // Everything was matched
// }

void SO6::genOneNorm()
{
    norm = Z2();
    for (int i = 0; i < 6; i++) {
        Z2 tmp = arr[0][i].abs();
        norm += tmp;
    }
}

bool SO6::isPerm(SO6 s) {
    return *this == s;
}

// //Returns true if and only if s and o are +/- column permutations of each other
// bool SO6::isPerm(SO6 s) {
//     bool perms[6] = { false, false, false, false, false, false };
//     bool isMatch;
//     for (int i = 0; i < 6; i++) {
//         isMatch = false;                                        // Column i is presently unmatched
//         for (int j = 0; j < 6; j++) {
//             if(perms[j]) continue;                              // If we have already matched this column of s, don't look at it again
            
//             // Check if they match with the same sign
//             for(int k = 0; k < 6; k++) {
//                if(arr[k][i] != s.arr[k][j]) break;              // We need them to match for every element.  
//                if(k == 5) isMatch = true;                       // Only hit me if everything has matched
//             }
//             if(isMatch) {                                       // If we matched, we can move on at this point
//                 perms[j] = true;                                // Exclude j from future consideration
//                 break;                                          // Move to next value of i
//             }

//             // Check if they match with opposite sign, can be combined with above methods to read easier. This shouldn't hit with the way that things have currently been implemented in the Z2 constructor, but I'm worried about the T matrices or the matrices that result after matrix multiplication. Should also add a method for multiplying a Z2 by an int.
//             for(int k = 0; k <6; k++) {
//                 if(-arr[k][i] != s.arr[k][j]) break;
//                 if(k == 5) isMatch = true;
//             }

//             if(isMatch) {
//                 perms[j] = true;
//                 break;
//             }
//         }    
//         if(!isMatch) return false;                              //We completed a loop with no match, so return false at this point
//     }
//     return true;                                                // Everything was matched
// }

/**
 * Generates LDE. Needs to be called any time the matrix is modified to make sure LDE stays current
 */
void SO6::genLDE(){
   int maximum = 0;
   for(int i = 0; i<6; i++){
       for(int j = 0; j<6; j++){
           LDE = arr[i][j].getLDE();
           maximum = std::max(LDE,maximum);
       }
   }
   LDE = maximum;
}

/**
 * Overloads << function for SO6.
 * @param os reference to ostream object needed to implement <<
 * @param m reference to SO6 object to be displayed
 * @returns reference ostream with the matrix's display form appended
 */
std::ostream& operator<<(std::ostream& os, const SO6& m){
    for(int row = 0; row<6; row++){
        os << '[';
        for(int col = 0; col<6; col++)
            os << m(col,row)<<' ';
        os << "] \n";
    }
    os << "\n";
    return os;
}


