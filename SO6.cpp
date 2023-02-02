#include <iostream>
#include <stdint.h>
#include <vector>
#include <algorithm>
#include "SO6.hpp"
// #include "pattern.hpp"

/**
 * Method to compare two Z2 arrays of length 6 lexicographically
 * @param first array of Z2 of length 6
 * @param second array of Z2 of length 6
 * @return -1 if first < second, 0 if equal, 1 if first > second
 */
int8_t SO6::lexicographical_compare(const Z2 first[6],const Z2 second[6])
{
    bool first_is_negative = false;
    bool second_is_negative = false;
    int row;
    for (row = 0; row < 6; row++)
    {
        if (first[row][0] == 0) {
            if(second[row][0] == 0) {
                continue;
            }
            return 1;
        }
        if (second[row][0] == 0) {
            return -1;
        }

        // Now both first and second are nonzero
        if (first[row] < Z2(0,0,0)) first_is_negative = true;
        if (second[row] < Z2(0,0,0)) second_is_negative = true;
        break;
    }
    
    if(!first_is_negative) {
        if(!second_is_negative) {
        while (row < 6) {
                if(first[row] == second[row]) {
                    row++;
                    continue;
                }
                if(second[row] < first[row]) return -1;
                return 1;
            }
            return 0;
        }
        for (row; row < 6; row++)
            {
                if(first[row] == -second[row]) continue;
                if(-second[row] < first[row]) return -1;
                return 1;
            }
        return 0;
    }
    //first is negative now
    if(!second_is_negative) {
        for (row; row < 6; row++) {
                if(-first[row] == second[row]) continue;
                if(second[row] < -first[row]) return -1;
                return 1;
            }
        return 0;
    }

    //both are negative
    for (int i = row; i < 6; i++)    {
        if(first[i] == second[i]) continue;
        if(-second[i] < -first[i]) return -1;
        return 1;
    }
    return 0;
}

/**
 * Method to compare two Z2 arrays of length 6 lexicographically
 * @param first array of Z2 of length 6
 * @param second array of Z2 of length 6
 * @return -1 if first < second, 0 if equal, 1 if first > second
 */
bool SO6::lexicographical_less(const Z2 first[6],const Z2 second[6])
{
    return (SO6::lexicographical_compare(first,second)<0);
}

/**
 * Basic constructor. Initializes Zero matrix.
 *
 */
SO6::SO6()
{  
}

/**
 * Basic constructor. Initializes to other
 *
 */
SO6::SO6(Z2 other[6][6])
{
    // for(int col =0; col<6; col++) {
    //     for(int row=0; row <6; row++) {
    //         arr[col][row]=other[col][row];
    //     }
    // }
}

// Something much faster than this would be a "multiply by T" method that explicitly does the matrix multiplication given a particular T matrix instead of trying to compute it naively

/**
 * Overloads the * operator with matrix multiplication for SO6 objects
 * @param other reference to SO6 to be multiplied with (*this)
 * @return matrix multiplication of (*this) and other
 */
SO6 SO6::operator*(const SO6 &other) const
{

    // The majority of the time in this computation is spent on the stupid history vector operations
    // it takes about 5x longer than without it 

    // multiplies operators assuming COLUMN,ROW indexing
    SO6 prod;

    // eliminating this next routine would save about 20 percent
    prod.hist.resize(hist.size() + other.hist.size());
    std::copy(other.hist.begin(),other.hist.end(),prod.hist.begin());
    std::copy(hist.begin(),hist.end(),prod.hist.begin()+other.hist.size());

    for (int row = 0; row < 6; row++)
    {
        for (int k = 0; k < 6; k++)
        {
            const Z2& left_element = (this->unpermuted(k,row));
            if (left_element[0] == 0) continue;    
            for (int col = 0; col < 6; col++)
            { 
                if((other.unpermuted(col,k))[0] == 0) continue;
                prod[col][row] += (left_element * other.unpermuted(col,k)); 
            }
        }
    }
    // prod.lexicographic_order(); // This is no longer useful to us, I think
    return prod;
}

/// @brief Left multiply this by a T operator
/// @param i the index of T_i
/// @return the result T_i * this
SO6 SO6::left_multiply_by_T(const int &i) const
{
    if (i < 5)
        return left_multiply_by_T(0, i + 1,(unsigned char) i+1);
    if (i < 9)
        return left_multiply_by_T(1, i - 3,(unsigned char) i+1);
    if (i < 12)
        return left_multiply_by_T(2, i - 6,(unsigned char) i+1);
    if (i < 14)
        return left_multiply_by_T(3, i - 8,(unsigned char) i+1);
    return left_multiply_by_T(4, 5,(unsigned char) i+1);
}

/// @brief left multiply this by a circuit
/// @param circuit circuit listed as a compressed vector of gates
/// @return the result circuit * this
SO6 SO6::left_multiply_by_circuit(std::vector<unsigned char> &circuit)
{
    SO6 prod = *this;
    for (unsigned char i : circuit)
    {
        prod = prod.left_multiply_by_T((i & 15) -1);  
        if(i>15) {
            prod = prod.left_multiply_by_T((i>>4)-1);
        } 
    }
    return prod;
}

SO6 SO6::left_multiply_by_T_transpose(const int &i)
{
    if (i < 5)
        return left_multiply_by_T(i + 1,0,(unsigned char) i+1);
    if (i < 9)
        return left_multiply_by_T(i - 3,1,(unsigned char) i+1);
    if (i < 12)
        return left_multiply_by_T(i - 6,2,(unsigned char) i+1);
    if (i < 14)
        return left_multiply_by_T(i - 8,3,(unsigned char) i+1);
    return left_multiply_by_T(5,4,(unsigned char) i+1);
}

SO6 SO6::left_multiply_by_T(const int &i, const int &j, const unsigned char &p) const
{
    SO6 prod = *this;

    // This should be made into a method b/c right now it is highly managed
    if(hist.empty() || hist.back()>15) {
        prod.hist.reserve(prod.hist.size()+1);              // We will never need to grow beyond this size, so might as well just add the one char
        prod.hist.push_back(p); 
    } else {
        prod.hist.back() = (prod.hist.back())^(p << 4);        
    }

    // We are guaranteed that j > i by def of left_multiply_by_T
    for (int col = 0; col < 6; col++)
    {
        prod[col][i] += (*this)[col][j];
        prod[col][i].increaseDE();
        prod[col][j] -= (*this)[col][i];
        prod[col][j].increaseDE();
    }
    prod.lexicographic_order();
    return prod;
}

/// @brief This implements insertion sort
void SO6::lexicographic_order()
{
    // Should be able to get rid of inverses just by only sorting the permutation and not the array, but was slower
    // For whatever reason this approach is faster than simply moving pointers
    uint8_t temp_perm[6];
    for(int i=0;i<6;i++) temp_perm[permutation[i]]=i;

    std::swap(permutation, temp_perm);
    for (int i = 1; i < 6; i++)
    {
        for (int j = i; j > 0 && SO6::lexicographical_less((*this)[j],(*this)[j-1]); j--)
        {
            std::swap(arr[j], arr[j - 1]);
            std::swap(permutation[j],permutation[j-1]);     // Permutation[i] = original column index now in position i
        }
    }
    
    for(int i=0;i<6;i++) temp_perm[permutation[i]]=i;   // permutation[i] = original column index now in position i       
    std::swap(permutation, temp_perm);  // Permutation[i] = current position of col index i
}

std::string SO6::name()
{
    return std::string(hist.begin(),hist.end());
}

SO6 SO6::reconstruct(const std::string name) {
    SO6 ret = SO6::identity();
    for(unsigned char i : name)
    {
        ret = ret.left_multiply_by_T((i & 15) -1);
        if(i>15) ret = ret.left_multiply_by_T((i>>4)-1);
    }
    ret.lexicographic_order();
    return ret;
}

std::string SO6::name_as_num(const std::string name) {
    std::string ret;
    for(unsigned char i : name)
    {
        ret.append(std::to_string((uint) ((i & 15) -1)) + " ");
        if(i>15) ret.append(std::to_string((uint)((i>>4)-1)) + " ");
    }
    return ret;
}

std::string SO6::circuit_string() {
    std::string ret;
    for(unsigned char i : hist)
    {
        ret.append(std::to_string((int) ((i & 15) -1)) + " ");
        if(i>15) ret.append(std::to_string((int)((i>>4)-1)) + " ");
    }
    return ret;
}

bool SO6::operator<(const SO6 &other) const
{
    for (int col = 0; col < 5; col++)
    { // There is no need to check the final column due to constraints
        switch (lexicographical_compare((*this)[col], other[col])) {
            case -1:
                return true;
            case 1:
                return false;
        }
    }
    return false;
}

z2_int SO6::getLDE()
{
    z2_int ret;
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
            ret[col][row] = (*this)[row][col];
        }
    }
    ret.lexicographic_order();
    return ret;
}

pattern SO6::to_pattern()
{
    pattern ret;
    ret.hist = hist;

    int8_t lde = (*this).getLDE();
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if (arr[i][j][2] < lde - 1 || arr[i][j][0]==0) {
                continue;
            }
            if (arr[i][j][2] == lde)
            {
                ret.arr[i][j].first = 1;
                ret.arr[i][j].second = arr[i][j][1] % 2;
                continue;
            }
            ret.arr[i][j].second = 1;
        }
    }
    ret.lexicographic_order();
    return ret;
}


/** overloads == method to check equality of SO6 matrices
 *  @param other reference to SO6 to be checked against
 *  @return whether or not (*this) and other are equivalent
 */
bool SO6::operator==(SO6 &other)
{
    for(int col = 0; col < 5; col ++) {
        if(lexicographical_compare((*this)[col],other[col])) return false;
    }
    return true;
}

bool SO6::operator==(const SO6 &other) const
{
    for(int col = 0; col < 5; col ++) {
        if(lexicographical_compare((*this)[col],other[col])) return false;
    }
    return true;
}

bool SO6::operator!=(const SO6 &other) const {
    return !(*this==other);
}

SO6 SO6::reconstruct() {
    SO6 ret = SO6::identity();
    for(unsigned char i : hist)
    {
        ret = ret.left_multiply_by_T((i & 15) -1);
        if(i>15) ret = ret.left_multiply_by_T((i>>4)-1);
    }
    ret.hist = hist;
    ret.lexicographic_order();
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

/**
 * Overloads << function for SO6.
 * @param os reference to ostream object needed to implement <<
 * @param m reference to SO6 object to be displayed
 * @returns reference ostream with the matrix's display form appended
 */
void SO6::unpermuted_print()
{
    std::cout << "\n";
    for (int row = 0; row < 6; row++)
    {
        if (row == 0)
            std::cout << "⌈\t";
        else if (row == 5)
            std::cout << "⌊\t";
        else
            std::cout << "|\t";
        for (int col = 0; col < 6; col++)
            std::cout << (*this).unpermuted(col,row) << '\t';
        if (row == 0)
            std::cout << "\t⌉\n";
        else if (row == 5)
        {
            std::cout << "\t⌋\n";
        }
        else
        {
            std::cout << "\t|\n";
        }
    }
    std::cout << "\n";
}