#include <array>

typedef int8_t z2_int;
typedef uint8_t uz2_int;

class Z2{
// elements of Z[1/sqrt(2)] are stored in the form (val[0] + val[1]*sqrt(2))/2^val[2]
public:
    Z2(const z2_int,const z2_int,const z2_int); // the ints paseed form the entries of val
    Z2();// the entries of val are all 0
    void increaseDE();
    inline z2_int& operator[](z2_int i){return val[i];} //returns ith component of val
    inline const z2_int& operator[](z2_int i) const{return val[i];} //does the same but for const Z2
    inline uint32_t as_uint32() const;
    Z2 operator+(Z2&); //handles addition
    Z2& operator+=(Z2&); //handles +=
    Z2& operator-=(Z2&); //handles -=
    Z2 operator-() const; //handles negation
    Z2 operator-(Z2&); //handles subtraction
    bool operator<(const z2_int&);
    bool operator<(const Z2&) const;
    bool operator>(const z2_int&);
    bool operator>(Z2&);
    bool operator<=(Z2&);
    bool operator>=(Z2&);
    // Z2 operator*(const Z2); //function that handles multiplication
    // Z2 operator*(Z2&); //function that handles multiplication
    Z2 operator*(const Z2&); //function that handles multiplication
    bool operator==(const Z2&) const; //function that checks equality between two Z2
    bool operator==(Z2&);
    bool operator==(const z2_int&); //function that checks equality between two Z2
    bool operator!=(const Z2&) const; //function that checks equality between two Z2
    Z2& operator=(const z2_int&); //function that makes the operator have equal entries to parameter
    Z2& operator=(const Z2&); //function that makes the operator have equal entries to parameter
    Z2& operator=(Z2&); //function that makes the operator have equal entries to parameter
    friend std::ostream& operator<<(std::ostream&,const Z2&); //display
    void negate(){val[0]=-val[0];val[1]=-val[1];}
    // static const Z2 inverse_root2() {return Z2(1,0,1);}
    // static const Z2 one() {return Z2(1,0,0);}
    // static const Z2 zero() {return Z2(0,0,0);}
    // inline static z2_int count[] = {0,0,0}; // Useful for counting statistics, only for debugging/analysis
    Z2& reduce(); //auxiliary function to make sure every triad is in a consistent most reduced form
    z2_int val[3]; //values of the Z2
private:
    
};
