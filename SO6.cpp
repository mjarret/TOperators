#include <iostream>
#include <stdint.h>
#include <vector>
#include <algorithm>
#include "Z2.hpp"
#include "SO6.hpp"

/**
 * Method to compare two Z2 arrays of length 6 lexicographically
 * @param first array of Z2 of length 6
 * @param second array of Z2 of length 6
 * @return -1 if first < second, 0 if equal, 1 if first > second
 */
bool lexicographical_less(Z2 *first, Z2 *second)
{
    for (int i = 0; i < 6; i++)
    {
        if (first[i] != second[i])
            return second[i] < first[i]; // This "reversed" ordering guarantees identity displays normal
    }
    return false;
}

/**
 * @brief Method to avoid multiple calls to lexicographical_less when we need to lexicographically compare two strings
 * It doesn't seem like this will hit 0, since this only used when sorting, I think...
 *
 * @param first
 * @param second
 * @return int8_t
 */
int8_t lexComp(const Z2 first[6], const Z2 second[6])
{
    for (int i = 0; i < 6; i++)
    {
        if (first[i] == second[i])
            continue; // Presently, this is where we spend most of our time. I don't know how to make this any faster.
        if (second[i] < first[i])
            return -1;
        return 1;
    }
    return 0;
}

/**
 * Basic constructor. Initializes Zero matrix.
 *
 */
SO6::SO6()
{
}

// /**
//  * Constructor that initializes arbitrary matrix with arbitrary name
//  * @param t the object history
//  */
// SO6::SO6(std::vector<int8_t> t){
//     // for(int8_t i=0; i<6; i++){
//     //     for(int8_t j=0; j<6; j++)
//     //         arr[i][j]=Z2();
//     // }
//     // hist = t;
// }

// Something much faster than this would be a "multiply by T" method that explicitly does the matrix multiplication given a particular T matrix instead of trying to compute it naively

/**
 * Overloads the * operator with matrix multiplication for SO6 objects
 * @param other reference to SO6 to be multiplied with (*this)
 * @return matrix multiplication of (*this) and other
 */
SO6 SO6::operator*(SO6 &other)
{
    // multiplies operators assuming COLUMN,ROW indexing
    SO6 prod;
    // Compute product
    for (int col = 0; col < 6; col++)
    {
        for (int row = 0; row < 6; row++)
        {  
            bool flag = false;
            bool first = true;
            for (int k = 0; k < 6; k++)
            {
                if (arr[k][row][0] == 0 || other[col][k][0] == 0)
                    continue;                                 // Skip zeros
                Z2 tmp = (arr[k][row] * other[col][k]); // This not transpose * other
                prod[col][row] += tmp;
                
                // This will fix the sign inline
                
                // if(flag || (first && prod[col][row][0]<0)) {
                //     flag = true;
                //     prod[col][row].negate();
                // }
                // first = false;
            }
        }
    }
    prod.lexOrder();
    return prod;
}

SO6 SO6::tMultiply(const int &i)
{
    if (i < 5)
        return tMultiply(0, i + 1,i);
    if (i < 9)
        return tMultiply(1, i - 3,i);
    if (i < 12)
        return tMultiply(2, i - 6,i);
    if (i < 14)
        return tMultiply(3, i - 8,i);
    return tMultiply(4, 5,i);
}

SO6 SO6::tMultiplyTranspose(const int &i)
{
    if (i < 5)
        return tMultiply(i + 1,0,i+15);
    if (i < 9)
        return tMultiply(i - 3,1,i+15);
    if (i < 12)
        return tMultiply(i - 6,2,i+15);
    if (i < 14)
        return tMultiply(i - 8,3,i+15);
    return tMultiply(5,4,i+15);
}

SO6 SO6::tMultiply(const int &i, const int &j, const int &p)
{
    SO6 prod = *this;
    prod.hist.push_back(p); 
    // We are guaranteed that j > i by def of tMultiply
    for (int col = 0; col < 6; col++)
    {
        prod[col][i] += arr[col][j];
        prod[col][i].increaseDE();
        prod[col][j] -= arr[col][i]; // May want to overload -=
        prod[col][j].increaseDE();
    }
    prod.lexOrder();
    return prod;
}

/**
 * @brief Sign fixing method, puts things in appropriate order
 */
void SO6::fixSign()
{
    for (int col = 0; col < 6; col++)
    {
        for (int row = 0; row < 6; row++)
        {
            if (arr[col][row][0] == 0)
                continue;
            if (arr[col][row][0] < 0)
            {
                while (row < 6)
                    arr[col][row++].negate();
            }
            else
            {
                break;
            }
        }
    }
}

/// @brief This implements insertion sort
void SO6::lexOrder()
{
    fixSign();                                  // lex ordering requires fixing the sign, sort of
    for (int i = 1; i < 6; i++)
    {
        for (int j = i; j > 0 && lexicographical_less(arr[j],arr[j-1]); j--)
        {
            std::swap(arr[j], arr[j - 1]);
        }
    }
}

std::string SO6::printName()
{
    std::string ret = "";
    // std::reverse(hist.begin(),hist.end());
    for (int8_t i : hist)
    {
        ret.append(std::to_string((int) i%15));
        if(i > 14) ret.append("t");
        ret.append(" ");
    }
    // std::reverse(hist.begin(),hist.end());
    return ret;
}

/// @brief This implements an optimal sorting network. Somehow this is worse than insertion sort, despite (naively) having fewer total operations of the same types.
// void SO6::lexOrder()
// {
//     if(lexicographical_less(arr[1],arr[0])) std::swap(arr[1],arr[0]);
//     if(lexicographical_less(arr[3],arr[2])) std::swap(arr[3],arr[2]);
//     if(lexicographical_less(arr[5],arr[4])) std::swap(arr[5],arr[4]);
//     if(lexicographical_less(arr[2],arr[0])) std::swap(arr[2],arr[0]);
//     if(lexicographical_less(arr[5],arr[3])) std::swap(arr[3],arr[5]);
//     if(lexicographical_less(arr[4],arr[1])) std::swap(arr[1],arr[4]);
//     if(lexicographical_less(arr[1],arr[0])) std::swap(arr[1],arr[0]);
//     if(lexicographical_less(arr[3],arr[2])) std::swap(arr[3],arr[2]);
//     if(lexicographical_less(arr[5],arr[4])) std::swap(arr[5],arr[4]);
//     if(lexicographical_less(arr[2],arr[1])) std::swap(arr[2],arr[1]);
//     if(lexicographical_less(arr[4],arr[3])) std::swap(arr[4],arr[3]);
//     if(lexicographical_less(arr[3],arr[2])) std::swap(arr[3],arr[2]);
// }

// /// @brief This uses std::sort
// void SO6::lexOrder() {
//     Z2 *myZ2[] = {arr[0], arr[1], arr[2], arr[3], arr[4], arr[5]};  //List of pointers to data addresses
//     std::vector<Z2 *> myvector(myZ2, myZ2 + 6);                     //Vector of pointers
//     std::sort(myvector.begin(), myvector.end(), lexicographical_less);           //Sort the pointers
//     Z2 arr2[6][6];
//     for (int i = 0; i < 6; i++)
//     {
//         for (int j = 0; j < 6; j++)
//         {
//             arr2[i][j] = myvector.at(i)[j];                       //myvector.at(i) is a pointer to arr[x]
//         }
//     }
//     for (int i = 0; i < 6; i++)
//     {
//         for (int j = 0; j < 6; j++)
//         {
//             arr[i][j] = arr2[i][j];
//         }
//     }
// }

bool SO6::operator<(const SO6 &other) const
{
    for (int col = 0; col < 5; col++)
    { // There is no need to check the final column due to constraints
        switch (lexComp((*this)[col], other[col]))
        {
        case -1:
            return true;
        case 1:
            return false;
        }
    }
    return false;
}

int8_t SO6::getLDE()
{
    int8_t ret;
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            ret = std::max(ret, arr[i][j][2]);
        }
    }
    return ret;
}

SO6 SO6::transpose() {
    SO6 ret;
    for(int col = 0; col<6; col++) {
        for(int row =0; row < 6; row++) {
            ret[col][row] = arr[row][col];
        }
    }
    // ret.lexOrder();
    return ret;
}

void SO6::row_permute(int rows[6]) {
    for(int col=0; col<6; col++) {
        for(int i=0; i<6; i++) {
            std::swap(arr[col][rows[i]],arr[col][i]);
        }
    }
    lexOrder();
}

SO6 SO6::getPattern()
{
    SO6 ret;
    int8_t lde = (*this).getLDE();
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if (arr[i][j][2] < lde - 1)
                continue;
            if (arr[i][j][0] == 0)
                continue;
            if (arr[i][j][2] == lde)
            {
                ret[i][j] = Z2(1, arr[i][j][1] % 2, 0);
                continue;
            }
            ret[i][j][1]++;
        }
    }
    ret.lexOrder();
    return ret;
}

/** overloads == method to check equality of SO6 matrices
 *  @param other reference to SO6 to be checked against
 *  @return whether or not (*this) and other are equivalent
 */
bool SO6::operator==(SO6 &other)
{
    for (int col = 5; col > -1; col--)
    {
        for (int row = 5; row > 5 - col - 1; row--)
        {
            if (arr[col][row] != other[col][row])
                return false;
        }
    }
    return true;
}

SO6 SO6::pattern_mod() {
    SO6 ret = getPattern();
    for (int col = 0; col < 6; col++) {
        for(int row = 0; row < 6; row++) {
            if(ret[col][row][0] == 0) continue;
            if(ret[col][row][1]==1 && ret[col][row][0] == 0) std::exit(0);
            ret[col][row][1] = !ret[col][row][1]; 
        }
    }
    ret.lexOrder();
    return ret;
}

// Don't think this works
//  bool SO6::opral induction is likely the correct proof technique here...)ower right triangle seems super fast, but can try out others
//      for (int col = 5; col > -1; col--)
//      {
//          for (int row = 5; row > 5 - col - 1; row--)
//          {
//              if (arr[col][row] < other[col][row] || other[col][row] < arr[col][row])
//                  return false;
//          }
//      }
//      return true;
//  }

/**
 * Overloads << function for SO6.
 * @param os reference to ostream object needed to implement <<
 * @param m reference to SO6 object to be displayed
 * @returns reference ostream with the matrix's display form appended
 */
std::ostream &operator<<(std::ostream &os, const SO6 &m)
{
    os << "\n";
    for (int row = 0; row < 6; row++)
    {
        if (row == 0)
            os << "⌈\t";
        else if (row == 5)
            os << "⌊\t";
        else
            os << "|\t";
        for (int col = 0; col < 6; col++)
            os << m[col][row] << '\t';
        if (row == 0)
            os << "\t⌉\n";
        else if (row == 5)
        {
            os << "\t⌋\n";
        }
        else
        {
            os << "\t|\n";
        }
    }
    os << "\n";
    return os;
}