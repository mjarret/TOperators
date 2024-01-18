#include <iostream>
#include <algorithm>
#include <stdint.h>
#include <array>
#include "Z2.hpp"

/**
 * Default Constructor
 * Initializes a Z2 object to represent the number 0.
 * This is achieved by setting all components of the number (integer part, sqrt(2) part, and log base √2 of the denominator) to 0.
 */
Z2::Z2()
{
    val[0] = 0;
    val[1] = 0;
    val[2] = 0;
}

/**
 * Parameterized Constructor
 * Initializes a Z2 object to represent the number (a+√2*b)/(√2)^c.
 * @param a Integer part of the numerator. Represents the coefficient of the rational component.
 * @param b Coefficient of the √2 component in the numerator. Represents the irrational component multiplied by √2.
 * @param c The exponent c in the denominator, representing the power of √2. Affects the scaling of the number.
 * This constructor allows for the creation of a Z2 number with specific components, enabling the representation of a wide range of values.
 */
Z2::Z2(const z2_int a, const z2_int b, const z2_int c)
{
    val[0] = a;
    val[1] = b;
    val[2] = c;
}



/**
 * Overloads the + operator for Z2 objects.
 * Performs the addition of two Z2 objects.
 * @param other The Z2 object to add to the current object.
 * @return The result of adding the current object and the 'other' object.
 */
Z2 Z2::operator+(Z2 &other)
{
    Z2 tmp = *this;
    tmp += other;
    return tmp;
}

/**
 * Overloads the += operator for Z2 objects.
 * Adds the 'other' Z2 object to the current object.
 * @param other The Z2 object to be added to the current object.
 * @return A reference to the current object after addition.
 */
Z2 &Z2::operator+=(const Z2 &other)
{
    // 0 cases are the only cases where a=even and Z2 is reduced, so screen them out    
    if(other[0]==0) return *this;
    if(val[0]==0) {   
        *this = other;
        return *this;
    }

    // we now know that val[0] and other[0] are odd. We can save a bit or so by exploiting this
    // specifically, we can exploit this by doing a = 2*val[0]-1 whenever val[0]!=0.
    // 2*val[0]-1 == (val[0]<<1)-1, not implemented
    uz2_int exp_diff = std::abs(val[2] - other[2]); 
    bool isNeg = val[2] < other[2];
    bool isOdd = exp_diff%2;
    exp_diff >>= 1;                                 // only want this base 2
    if(!isNeg) {                                    // this has larger denominator than other
        val[0] += (other.val[isOdd] << isOdd+exp_diff);
        val[1] += (other.val[!isOdd] << exp_diff);
        if(!exp_diff) reduce();                     // only need to reduce if exponents were the same
        return *this;
    }    
    std::swap(val[0],val[isOdd]);             // Does nothing if even & positive
    val[0] <<= exp_diff + isOdd;
    val[1] <<= exp_diff;
    val[0] += other[0];
    val[1] += other[1];
    val[2] = other[2];
    return *this;
}

/**
 * Overloads the -= operator for Z2 objects.
 * Subtracts the 'other' Z2 object from the current object.
 * @param other The Z2 object to be subtracted from the current object.
 * @return A reference to the current object after subtraction.
 */
Z2 &Z2::operator-=(const Z2 &other){
    *this += -other;
    return *this;
}

/**
 * Overloads the - operator for negating a Z2 object.
 * @return The negated Z2 object.
 */
Z2 Z2::operator-() const { 
    return Z2(-val[0], -val[1], val[2]); 
}

/**
 * Overloads the - operator for Z2 objects.
 * Performs the subtraction of 'other' from the current object.
 * @param other The Z2 object to be subtracted from the current object.
 * @return The result of the subtraction.
 */
Z2 Z2::operator-(Z2 &other) { return -other + *this; }

// /**
//  * Overloads the * operator for Z2
//  * @param other reference to Z2 object to be multiplied
//  * @return
//  */
// Z2 Z2::operator*(const Z2 other)
// {   
//     return Z2(val[0] * other[0] + ((val[1] * other[1]) << 1), val[0] * other[1] + val[1] * other[0], val[2] + other[2]);
//     // Z2 tmp = Z2(val[0] * other[0] + ((val[1] * other[1]) << 1), val[0] * other[1] + val[1] * other[0], val[2] + other[2]);
//     // tmp.reduce();
//     // if(val[0] == 0) return Z2(0,0,0);              // This check may not be necessary if we're careful
//     // return tmp;
// }


/**
 * Overloads the * operator for Z2 objects.
 * Multiplies the current object with 'other'.
 * @param other The Z2 object to be multiplied with the current object.
 * @return The result of the multiplication.
 */
Z2 Z2::operator*(const Z2 &other) const
{   
    return Z2(val[0] * other[0] + ((val[1] * other[1]) << 1), val[0] * other[1] + val[1] * other[0], val[2] + other[2]);
    // Z2 tmp = Z2(val[0] * other[0] + ((val[1] * other[1]) << 1), val[0] * other[1] + val[1] * other[0], val[2] + other[2]);
    // tmp.reduce();
    // if(val[0] == 0) return Z2(0,0,0);              // This check may not be necessary if we're careful
    // return tmp;
}

void Z2::increaseDE() 
{
    if(val[0]!=0) val[2]++;
}

/**
 * Overloads the == operator for Z2 objects.
 * Checks if the current object is equal to 'other'.
 * @param other The Z2 object to compare with the current object.
 * @return True if the objects are equal, false otherwise.
 */
bool Z2::operator==(const Z2 &other) const
{
    return (val[0] == other[0] && val[1] == other[1] && val[2] == other[2]);
}

/**
 * Overloads the < operator for Z2
 * @param other reference to Z2 object to be compared to
 * @return true if this < other and false otherwise
 */
bool Z2::operator==(Z2 &other)
{  
    return (val[0]==other[0] && val[1]==other[1] && val[2]==other[2]);
    // return val[0]^other[0]^val[1]^other[1]^val[2]^other[2] == 0;
}

/**
 * Overloads the == operator for Z2
 * @param other reference to Z2 object to be compared to
 * @return whether or not the entries of the two Z2s are equal
 */
bool Z2::operator==(const z2_int &i) { return val[0] == i && val[1] == 0 && val[2] == 0; }

/**
 * Overloads the != operator for Z2 objects.
 * Checks if the current object is not equal to 'other'.
 * @param other The Z2 object to compare with the current object.
 * @return True if the objects are not equal, false otherwise.
 */
bool Z2::operator!=(const Z2 &other) const { return !(*this == other); }

// inline uint32_t Z2::as_uint32() const {
//     // return int32_t((val[0] << 16)^(val[1] << 8 )^(val[2]));
//     const uint32_t mask = 0x80'80'80'80;
//     uz2_int tmp[4];
//     tmp[3] = 0;
//     tmp[2] = val[0];
//     tmp[1] = val[1];
//     tmp[0] = val[2];
//     return *reinterpret_cast<uint32_t*>(&tmp) ^ mask;
// }

/**
 * Overloads the < operator for Z2 objects.
 * Compares the current object with 'other' for less-than relation.
 * @param other The Z2 object to compare with the current object.
 * @return True if the current object is less than 'other', false otherwise.
 */
bool Z2::operator<(const Z2 &other) const
{
    if(val[0]<other[0]) 
        return true;
    if(val[0]==other[0]) 
    {
        if(val[1]<other[1]) 
            return true;
        if(val[1]==other[1]) {
            if(val[2]<other[2]) 
                return true;
        }
    }
    return false;
}

/**
 * Overloads the < operator for Z2 objects.
 * Compares the current object with 'other' for less-than relation.
 * @param other The Z2 object to compare with the current object.
 * @return True if the current object is less than 'other', false otherwise.
 */
bool Z2::is_negative() const
{
    if(val[0]<0) 
        return true;
    return val[0]==0 && val[1]<0;
}

/**
 * Overloads the > operator for Z2 objects.
 * Compares the current object with 'other' for greater-than relation.
 * @param other The Z2 object to compare with the current object.
 * @return True if the current object is greater than 'other', false otherwise.
 */
bool Z2::operator>(Z2 &other) { return (other < *this); }

/**
 * Overloads the >= operator for Z2 objects.
 * Checks if the current object is greater than or equal to 'other'.
 * @param other The Z2 object to compare with the current object.
 * @return True if the current object is greater than or equal to 'other', false otherwise.
 */
bool Z2::operator>=(Z2 &other) { return !(*this < other); }

/**
 * Overloads the <= operator for Z2 objects.
 * Checks if the current object is less than or equal to 'other'.
 * @param other The Z2 object to compare with the current object.
 * @return True if the current object is less than or equal to 'other', false otherwise.
 */
bool Z2::operator<=(Z2 &other) { return !(*this > other); }

/**
 * Overloads the = operator for Z2
 * @param other reference to object make *this equal to
 * @return *this reference to this object which has been made equal to other
 */
Z2 &Z2::operator=(const z2_int &other)
{
    val[0] = other;
    val[1] = 0;
    val[2] = 0;
    return *this;
}

// Z2 Z2::abs()
// {
//     if (*this < 0)
//         return -*this;
//     return *this;
// }

/**
 * Overloads the = operator for Z2
 * @param other reference to object make *this equal to
 * @return *this reference to this object which has been made equal to other
 */
Z2 &Z2::operator=(const Z2 &other)
{
    // // assigns an operator
    val[0] = other[0];
    val[1] = other[1];
    val[2] = other[2];
    return *this;
}

/**
 * Overloads the = operator for Z2 objects.
 * Assigns the values from another Z2 object to the current object.
 * This assignment makes the current object a copy of the 'other' object.
 * @param other The Z2 object whose values are to be copied.
 * @return A reference to the current object after the assignment.
 */
Z2 &Z2::operator=(Z2 &other)
{
    // assigns an operator
    val[0] = other[0];
    val[1] = other[1];
    val[2] = other[2];
    return *this;
}

/**
 * Reduces this Z2 to its simplest form.
 * This function normalizes the Z2 object by adjusting its components, 
 * ensuring it represents the number in its most reduced form.
 * The reduction process involves manipulating the integer and sqrt(2) parts
 * of the number and adjusting the denominator exponent accordingly.
 * This is particularly useful for maintaining a standard form of Z2 numbers 
 * after arithmetic operations.
 * 
 * @return A reference to this object in its simplified form.
 */
Z2 &Z2::reduce()
{
    // Assume that reduce() doesn't get called if val[0] == 0;
    if (!(val[0]) && !(val[1]))          // If it's 0, it's reduced.
    { 
        val[2] = 0;         
        return *this;       // Can we make all of these point to the same thing? Where would this go wrong? There may be a lot of 0s.
    }

    // reduces a Z2 to its lowest denominator exponent (DE) expression, can be negative, but maybe should require positive?
    // If a is even, (a + b√2)/√2^k ↦ (b + (a/2)√2)/√2^(k-1) in each iteration
    bool integer_part_index = 0;
    
    while (!(val[integer_part_index] % 2))                  
    {                                       
        val[integer_part_index] >>= 1;                 // a ↦ a/2         
        integer_part_index ^= 1;                       // effectively swap a/b for next iteration of loop
        val[2]--;                                        // decrease DE since divided out
    }
    std::swap(val[0],val[integer_part_index]);         // does nothing unless integer_part_index=1
    return *this;
}

/**
 * Overloads the << operator for Z2 objects.
 * This operator allows Z2 objects to be output to an ostream, such as std::cout.
 * It outputs the Z2 object in a specific format, displaying its components.
 * The format used is (integer_part,sqrt(2)_part)e(log_base_2_of_denominator).
 * This representation helps in understanding the state of a Z2 object.
 * 
 * @param os The output stream to which the Z2 object is to be sent.
 * @param z The Z2 object to be output.
 * @return A reference to the output stream.
 */
std::ostream& operator<<(std::ostream& os, const Z2& z){
    os << (int) z[0] << "," << (int) z[1] << "e" << (int) z[2];
    return os;
}