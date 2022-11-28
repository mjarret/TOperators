#include <iostream>
#include <algorithm>
#include <stdint.h>
#include <array>
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
Z2::Z2(const z2_int a, const z2_int b, const z2_int c)
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

Z2 &Z2::operator-=(Z2 &other){
    *this = *this - other;
    return *this;
}

/**
 * Overloads the - operator for Z2
 * @param
 * @return negation of Z2
 */
Z2 Z2::operator-() const { 
    Z2 ret = Z2(-val[0], -val[1], val[2]); 
    return ret;
}

/**
 * Overloads the - operator for Z2
 * @param other reference to Z2 object to be subtracted
 * @return summation the subtraction *this - other
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
 * Overloads the * operator for Z2
 * @param other reference to Z2 object to be multiplied
 * @return
 */
Z2 Z2::operator*(const Z2 &other)
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
 * Overloads the == operator for Z2
 * @param other reference to Z2 object to be compared to
 * @return whether or not the entries of the two Z2s are equal
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
 * @brief Overloads the < operator for Z2. This is similar to radix ordering. It DOES NOT return actual x < y, but rather compares term by term
 * @param other reference to Z2 object to be compared to 
 * @return true if this < other in a radix sense and false otherwise
 */
bool Z2::operator<(const Z2 &other) const
{
    // return as_uint32() < other.as_uint32();

    // return as_int32() < other.as_int32();
    // return std::lexicographical_compare(val,val+2,other.val,other.val+2);
    // int32_t tmp = (val[0]<<16)^(val[1]<<8)^val[2];
    // int32_t tmp2 = (other[0]<<16)^(other[1]<<8)^(other[2]);
    // return tmp < tmp2;
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
    /**
     *  This used to compare term by term as below 
     */
    // int k = std::max(val[2], other[2]);
    // int a = val[0] << (k - val[2]);
    // int b = val[1] << (k - val[2]);
    // a -= (other[0] << (k - other[2]));
    // b -= (other[1] << (k - other[2]));
    // if (a < 0)
    // {
    //     if (b <= 0) return true;        // a<0 and b<=0 means that diff < 0
    //     a *= a;                         // compute a^2
    //     b = (b * b) << 1;               // compute 2b^2
    //     if (a > b) return true;         // a<0, b>0, and a^2 > 2 b^2 implies that a+sqrt(2)b <0
    //     return false;                   // a<0, b>0, and a^2 <= 2 b^2 implies that a+sqrt(2)b >= 0
    // }
    // if (b >= 0) return false;           // a>=0 and b>=0 means that diff >=0
    // a *= a;                             // compute a^2
    // b = (b * b) << 1;                   // compute 2b^2
    // if (a < b) return true;             // a>0, b<0, and a^2 < 2b^2 implies that a + sqrt(2) b < 0
    // return false;                       // a>0, b<0, and a^2 >= 2b^2 implies that a+sqrt(2)b >= 0
}

bool Z2::operator>(Z2 &other) { return (other < *this); }
bool Z2::operator>=(Z2 &other) { return !(*this < other); }
bool Z2::operator<=(Z2 &other) { return !(*this > other); }

// /**
//  * Overloads the < operator for Z2
//  * @param other reference to an integer
//  * @return true if this < other and false otherwise
//  */
// bool Z2::operator<(const z2_int &i)
// {
//     Z2 tmp = Z2(i, 0, 0);
//     return *this < tmp;
// }

// bool Z2::operator>(const z2_int &i)
// {
//     Z2 tmp = Z2(i, 0, 0);
//     return *this > tmp;
// }

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
    // assigns an operator
    val[0] = other[0];
    val[1] = other[1];
    val[2] = other[2];
    return *this;
}

/**
 * Overloads the = operator for Z2
 * @param other reference to object make *this equal to
 * @return *this reference to this object which has been made equal to other
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