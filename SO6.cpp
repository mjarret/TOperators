#include <iostream>
#include <iomanip>  // For std::setw
#include <sstream>
#include <stdint.h>
#include <vector>
#include <algorithm>
#include "SO6.hpp"
#include "pattern.hpp"

/**
 * Method to compare two Z2 arrays of length 6 lexicographically
 * we are given the guarantee that the first nonzero element in the row is non-negative
 * @param first array of Z2 of length 6
 * @param second array of Z2 of length 6
 * @return -1 if first < second, 0 if equal, 1 if first > second
 */
int8_t SO6::lexicographical_compare(const Z2 first[6],const Z2 second[6])
{
    bool first_is_negative = false;
    bool second_is_negative = false;
    int row;

    // We search for the first nonzero row of first and if on the way we encounter a nonzero row of second, 
    // we immediately claim second is larger. If both are nonzero, we compare them
    // the goal is to "fix" the sign convention of the row so that the entire
    // column is sorted such that the top element is positive by multiplying everything by -1
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
        first_is_negative = first[row].is_negative();
        second_is_negative = second[row].is_negative();
        // if (first[row] < Z2(0,0,0)) first_is_negative = true;
        // if (second[row] < Z2(0,0,0)) second_is_negative = true;
        break;
    }

    // compare remaining rows until return
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
    for(int col =0; col<6; col++) {
        for(int row=0; row <6; row++) {
            arr[col][row]=other[col][row];
        }
    }
}

// Something much faster than this would be a "multiply by T" method that explicitly does the matrix multiplication given a particular T matrix instead of trying to compute it naively

/**
 * Overloads the * operator with matrix multiplication for SO6 objects
 * @param other reference to SO6 to be multiplied with (*this)
 * @return matrix multiplication of (*this) and other
 */
SO6 SO6::operator*(const SO6 &other) const
{
    // multiplies operators assuming COLUMN,ROW indexing
    SO6 prod;

    // let's see what happens if i turn off history printing
    prod.hist.reserve(hist.size() + other.hist.size());  // Reserve instead of resize
    std::copy(other.hist.begin(), other.hist.end(), std::back_inserter(prod.hist));
    std::copy(hist.begin(), hist.end(), std::back_inserter(prod.hist));

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

/**
 * Overloads the * operator with matrix multiplication for SO6 objects
 * @param other reference to pattern to be multiplied with (*this)
 * @return matrix multiplication of (*this) and other
 */
SO6 SO6::operator*(const pattern &other) const
{
    // multiplies operators assuming COLUMN,ROW indexing
    SO6 prod;
    prod.hist = hist; // patterns don't typically have histories in this code base, could be changed but currently not so 

    for (int row = 0; row < 6; row++)
    {
        for (int k = 0; k < 6; k++)
        {
            const Z2& left_element = (this->unpermuted(k,row));            
            if (left_element[0] == 0) continue;    
            
            Z2 smallerLDE = left_element;
            smallerLDE[2]--; //decrease lde

            for (int col = 0; col < 6; col++)
            { 
                if(other.arr[col][k].first) prod[col][row] += left_element;
                if(other.arr[col][k].second) prod[col][row] += smallerLDE; 
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

void SO6::update_history(const unsigned char &p) {
    // Check if we need to start a new history entry
    if (hist.empty() || (hist.back() & 0xF0) != 0) {
        hist.reserve(hist.size() + 1);  // Reserve space for one more element
        hist.push_back(p);              // Add the new entry
    } else {
        // Pack the new entry into the higher 4 bits of the last byte
        hist.back() |= (p << 4);
    }
}

/**
 * @brief Performs a left multiplication of this SO(6) matrix by a specific T matrix.
 *
 * This function multiplies the current SO(6) matrix by a T matrix identified by 
 * indices `i` and `j` and updates its internal state based on the parameter `p`. 
 * The operation is row-dependent and independent of the matrix's initial permutation.
 * The resulting matrix is reordered in lexicographic order and returned as a new SO6 object.
 *
 * @param i The first index for the T matrix (row index), integral part of specifying the T matrix.
 * @param j The second index for the T matrix (row index), must be greater than `i`.
 * @param p An unsigned char value used to index Tₚ
 * @return SO6 The product of the multiplication, returned as a new SO6 object.
 *
 * @note The function ensures that `j` is always greater than `i`, which is essential for
 *       defining the specific T matrix multiplication. The lexicographic ordering of the
 *       resulting matrix is maintained.
 */
SO6 SO6::left_multiply_by_T(const int &i, const int &j, const unsigned char &p) const
{
    SO6 prod = *this;

    for (int col = 0; col < 6; col++)
    {
        prod[col][i] += (*this)[col][j];
        prod[col][i].increaseDE();
        prod[col][j] -= (*this)[col][i];
        prod[col][j].increaseDE();
    }
    prod.lexicographic_order();
    prod.update_history(p);
    return prod;
}

/// @brief This implements insertion sort
void SO6::lexicographic_order()
{

    int inverse_permutation[6] = {0,1,2,3,4,5};
    for(int i = 0; i < 6; i++) {
        inverse_permutation[permutation[i]] = i;
    }

    for (int i = 1; i < 6; i++)
    {
        int j = i;
        while (j > 0 && SO6::lexicographical_less(arr[j], arr[j - 1]))
        {
            // Swap the actual Z2 arrays in-place
            for (int k = 0; k < 6; k++) {
                std::swap(arr[j][k], arr[j - 1][k]);
            }
            // Swap the corresponding indices in the permutation array
            std::swap(inverse_permutation[j], inverse_permutation[j - 1]); 
            j--;
        }
    }

    for(int i = 0; i < 6; i++) {
        permutation[inverse_permutation[i]] = i;
    }
}

std::string SO6::name()
{
    return std::string(hist.begin(),hist.end());
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

SO6 SO6::reconstruct(const std::string name) {
    SO6 ret = SO6::identity();
    for(unsigned char i : name) {
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
    ret.pop_back();
    return ret;
}

std::string SO6::circuit_string() {
    std::string ret;
    for (unsigned char byte : hist) {
        int lower = (byte & 15) - 1;  // Lower 4 bits
        ret.append(std::to_string(lower) + " ");

        // Check for upper 4 bits
        if (byte > 15) {
            int upper = (byte >> 4) - 1;  // Upper 4 bits
            ret.append(std::to_string(upper) + " ");
        }
    }
    ret.pop_back();
    return ret;
}

std::vector<unsigned char> invert_circuit_string(const std::string& input) {
    std::vector<unsigned char> hist;
    std::istringstream iss(input);
    int lower, upper;

    while (iss >> lower) {
        lower += 1;  // Reverse the subtraction of 1 for the lower 4 bits
        unsigned char byte = static_cast<unsigned char>(lower & 15);

        // Check if there's an upper part
        if (iss >> upper) {
            upper += 1;  // Reverse the subtraction of 1 for the upper 4 bits
            byte |= static_cast<unsigned char>((upper & 15) << 4);
        }

        hist.push_back(byte);
    }

    return hist;
}

SO6 SO6::reconstruct_from_circuit_string(const std::string& input) {
    std::istringstream iss(input);
    int number;
    SO6 ret = SO6::identity();

    // Iterate over each integer in the string
    while (iss >> number) {
        // Process each number, for example, print it
        ret = ret.left_multiply_by_T(number);
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

const z2_int SO6::getLDE() const
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

// SO6 SO6::transpose() 
// {
//     SO6 ret;
//     for(int col = 0; col<6; col++) {
//         for(int row =0; row < 6; row++) {
//             ret[col][row] std::vector<unsigned char> invert_circuit_string(const std::string& input) {
//     std::vector<unsigned char> hist;
//     std::istringstream iss(input);
//     int lower, upper;

//     while (iss >> lower) {
//         lower += 1;  // Reverse the subtraction of 1 for the lower 4 bits
//         unsigned char byte = static_cast<unsigned char>(lower & 15);

//         // Check if there's an upper part
//         if (iss >> upper) {
//             upper += 1;  // Reverse the subtraction of 1 for the upper 4 bits
//             byte |= static_cast<unsigned char>((upper & 15) << 4);
//         }

//         hist.push_back(byte);
//     }

//     return hist;
//     }
//     }
//     ret.lexicographic_order();
//     return ret;
// }

pattern SO6::to_pattern() const
{
    pattern ret;
    ret.hist.reserve(hist.size());
    ret.hist = hist;

    const int8_t& lde = getLDE();
    for (int col = 0; col < 6; col++)
    {
        for (int row = 0; row < 6; row++)
        {
            if (arr[col][row][2] < lde - 1 || arr[col][row][0]==0) {
                continue;
            }
            if (arr[col][row][2] == lde)
            {
                ret.arr[col][row].first = 1;
                ret.arr[col][row].second = arr[col][row][1] % 2;
                continue;
            }
            ret.arr[col][row].second = 1;
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

/**
 * Overloads << function for SO6.
 * @param os reference to ostream object needed to implement <<
 * @param m reference to SO6 object to be displayed
 * @returns reference ostream with the matrix's display form appended
 */
std::ostream &operator<<(std::ostream &os, const SO6 &m) {
    int maxWidth = 0;

    // Find the maximum width of the elements
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 6; col++) {
            std::stringstream ss;
            ss << m[col][row];
            maxWidth = std::max(maxWidth, static_cast<int>(ss.str().length()));
        }
    }

    const int width = maxWidth + 3; // Adjust the width by adding 2

    os << "\n";
    for (int row = 0; row < 6; row++) {
        std::string leftBorder = (row == 0) ? "⌈" : ((row == 5) ? "⌊" : "|");
        std::string rightBorder = (row == 0) ? "⌉" : ((row == 5) ? "⌋" : "|");

        os << leftBorder << "  ";
        for (int col = 0; col < 6; col++) {
            os << std::setw(width) << m[col][row];
        }
        os << "\t" << rightBorder << "\n";
    }
    os << "\n";

    return os;
}

void SO6::unpermuted_print() {
    int maxWidth = 0;

    // Find the maximum width of the elements
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 6; col++) {
            std::stringstream ss;
            ss << this->unpermuted(col, row);
            maxWidth = std::max(maxWidth, static_cast<int>(ss.str().length()));
        }
    }

    const int width = maxWidth + 2; // Adjust the width by adding 2

    std::cout << "\n";
    for (int row = 0; row < 6; row++) {
        std::string leftBorder = (row == 0) ? "⌈" : ((row == 5) ? "⌊" : "|");
        std::string rightBorder = (row == 0) ? "⌉" : ((row == 5) ? "⌋" : "|");

        std::cout << leftBorder << "\t";
        for (int col = 0; col < 6; col++) {
            std::cout << std::setw(width) << this->unpermuted(col, row);
        }
        std::cout << "\t" << rightBorder << "\n";
    }
    std::cout << "\n";
}