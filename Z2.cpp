#include <algorithm>
#include <iostream>
#include <stdint.h>
#include "Z2.hpp"

/**
 * Constructor to initialize Z2 corresponding to 0.
 */
Z2::Z2()
{
    val[0] = 0;
    val[1] = 0;
    val[2] = 0;
}

/**
 * Constructor that initializes object corresponding to (a+√2*b)/(√2)^c
 * @param a integer part of numerator
 * @param b sqrt(2) part of numerator
 * @param c log_2 of the denominator
 */
Z2::Z2(const int8_t a, const int8_t b, const int8_t c)
{
    val[0] = a;
    val[1] = b;
    val[2] = c;
}



/**
 * Overloads + operator for Z2
 * @param other reference to Z2 object to be added
 * @return summation the sum of other and *this
 */
Z2 Z2::operator+(Z2 &other)
{
    Z2 tmp = *this;
    tmp += other;
    return tmp;
}

/**
 * Overloads += operator for Z2
 * @param other reference to Z2 object to be added to *this
 * @return *this reference to this object, onto which other has been added
 */
Z2 &Z2::operator+=(Z2 &other)
{
    // 0 cases are the only cases where a=even and Z2 is reduced, so screen them out    
    if(!other[0]) return *this;
    if(!val[0]) {   
        *this = other;
        return *this;
    }

    uint8_t exp_diff = std::abs(val[2] - other[2]); 
    bool isNeg = val[2] < other[2];
    bool isOdd = exp_diff%2;
    exp_diff >>= 1;                                 // only want this base 2
    if(!isNeg) {                                    // this has larger denominator than other
        val[0] += (other[isOdd] << isOdd+exp_diff);
        val[1] += (other[!isOdd] << exp_diff);
        if(!exp_diff) reduce();                     // only need to reduce of exponents were the same
        return *this;
    }
    
    std::swap(val[0],val[isOdd]);     // Does nothing if even
    val[0] <<= exp_diff + isOdd;
    val[1] <<= exp_diff;
    val[0] += other[0];
    val[1] += other[1];
    val[2] = other[2];
    return *this;
}

/**
 * Overloads the - operator for Z2
 * @param
 * @return negation of Z2
 */
Z2 Z2::operator-() { 
    Z2 ret = Z2(-val[0], -val[1], val[2]); 
    return ret;
}

/**
 * Overloads the - operator for Z2
 * @param other reference to Z2 object to be subtracted
 * @return summation the subtraction *this - other
 */
Z2 Z2::operator-(Z2 &other) { return -other + *this; }

/**
 * Overloads the * operator for Z2
 * @param other reference to Z2 object to be multiplied
 * @return
 */
Z2 Z2::operator*(const Z2 &other)
{   
    // if(!val[0] || !other[0]) return Z2(0,0,0);              // This check may not be necessary if we're careful
    return Z2(val[0] * other[0] + ((val[1] * other[1]) << 1), val[0] * other[1] + val[1] * other[0], val[2] + other[2]);
}

/**
 * Overloads the == operator for Z2
 * @param other reference to Z2 object to be compared to
 * @return whether or not the entries of the two Z2s are equal
 */
const bool Z2::operator==(const Z2 &other) const
{
    // count[0]++;
    // if(val[0] == 0 && other[0]==0) return true;
    //     return true;
    // }
    // count[1]++;
    // if(val[1] == other[1]) count[1]++;
    // if(val[2] == other[2]) count[2]++;
    return val[0] == other[0] && val[1] == other[1] && val[2] == other[2];
}

/**
 * Overloads the < operator for Z2
 * @param other reference to Z2 object to be compared to
 * @return true if this < other and false otherwise
 */
bool Z2::operator==(Z2 &other)
{  
    return (val[0]==other[0] && val[1]==other[1] && val[2]==other[2]);
}

/**
 * Overloads the == operator for Z2
 * @param other reference to Z2 object to be compared to
 * @return whether or not the entries of the two Z2s are equal
 */
bool Z2::operator==(const int8_t &i) { return val[0] == i && val[1] == 0 && val[2] == 0; }

bool Z2::operator!=(const Z2 &other) { return !(*this == other); }

/**
 * Overloads the < operator for Z2
 * @param other reference to Z2 object to be compared to
 * @return true if this < other and false otherwise
 */
bool Z2::operator<(Z2 &other)
{
    Z2 diff = *this - other;                            // Find difference and store as Z2, this could be expensive
    if (diff.val[0] < 0)
    {
        if (diff.val[1] <= 0) return true;              // a<0 and b<=0 means that diff < 0
        int a2 = diff.val[0] * diff.val[0];             // compute a^2
        int b2 = (diff.val[1] * diff.val[1]) << 1;      // compute 2b^2
        if (a2 > b2) return true;                       // a<0, b>0, and a^2 > 2 b^2 implies that a+sqrt(2)b <0
        return false;                                   // a<0, b>0, and a^2 <= 2 b^2 implies that a+sqrt(2)b >= 0
    }
    if (diff.val[1] >= 0)   return false;               // a>=0 and b>=0 means that diff >=0
    int a2 = diff.val[0] * diff.val[0];                 // compute a^2
    int b2 = (diff.val[1] * diff.val[1]) << 1;          // compute 2b^2
    if (a2 < b2) return true;                           // a>0, b<0, and a^2 < 2b^2 implies that a + sqrt(2) b < 0
    return false;                                       // a>0, b<0, and a^2 >= 2b^2 implies that a+sqrt(2)b >= 0
}

/**
 * Overloads the < operator for Z2
 * @param other reference to Z2 object to be compared to
 * @return true if this < other and false otherwise
 */
const bool Z2::operator<(const Z2 &other) const
{
    int8_t k = std::max(val[2], other[2]);
    int a = val[0] << (k - val[2]);
    int b = val[1] << (k - val[2]);
    a -= (other[0] << (k - other[2]));
    b -= (other[1] << (k - other[2]));
    if (a < 0)
    {
        if (b <= 0) return true;        // a<0 and b<=0 means that diff < 0
        a *= a;                         // compute a^2
        b = (b * b) << 1;               // compute 2b^2
        if (a > b) return true;         // a<0, b>0, and a^2 > 2 b^2 implies that a+sqrt(2)b <0
        return false;                   // a<0, b>0, and a^2 <= 2 b^2 implies that a+sqrt(2)b >= 0
    }
    if (b >= 0) return false;           // a>=0 and b>=0 means that diff >=0
    a *= a;                             // compute a^2
    b = (b * b) << 1;                   // compute 2b^2
    if (a < b) return true;             // a>0, b<0, and a^2 < 2b^2 implies that a + sqrt(2) b < 0
    return false;                       // a>0, b<0, and a^2 >= 2b^2 implies that a+sqrt(2)b >= 0
}

bool Z2::operator>(Z2 &other) { return other < *this; }
bool Z2::operator>=(Z2 &other) { return !(*this < other); }
bool Z2::operator<=(Z2 &other) { return !(*this > other); }

/**
 * Overloads the < operator for Z2
 * @param other reference to an integer
 * @return true if this < other and false otherwise
 */
bool Z2::operator<(const int8_t &i)
{
    Z2 tmp = Z2(i, 0, 0);
    return *this < tmp;
}

bool Z2::operator>(const int8_t &i)
{
    Z2 tmp = Z2(i, 0, 0);
    return *this > tmp;
}

/**
 * Overloads the = operator for Z2
 * @param other reference to object make *this equal to
 * @return *this reference to this object which has been made equal to other
 */
Z2 &Z2::operator=(const int8_t &other)
{
    val[0] = other;
    val[1] = 0;
    val[2] = 0;
    return *this;
}

Z2 Z2::abs()
{
    if (*this < 0)
        return -*this;
    return *this;
}

/**
 * Overloads the = operator for Z2
 * @param other reference to object make *this equal to
 * @return *this reference to this object which has been made equal to other
 */
Z2 &Z2::operator=(const Z2 &other)
{
    // assigns an operator
    val[0] = other[0];
    val[1] = other[1];
    val[2] = other[2];
    return *this;
}

/**
 * Reduces this Z2 to simplest form
 * @return *this reference to simplified object
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
    bool par = 0;
    while (!(val[par] % 2))                  
    {                                       
        val[par] >>= 1;                 // a ↦ a/2         
        par ^= 1;                       // effectively swap a/b for next iteration of loop
        val[2]--;                       // decrease DE since divided out
    }
    std::swap(val[0],val[par]);         // does nothing unless par=1
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Z2& z){
    os << (int) z[0] << "," << (int) z[1] << "e" << (int) z[2];
    return os;
}