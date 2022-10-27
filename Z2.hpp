
class Z2{
// elements of Z[1/sqrt(2)] are stored in the form (val[0] + val[1]*sqrt(2))/2^val[2]
public:
    Z2(const int8_t,const int8_t,const int8_t); // the ints paseed form the entries of val
    Z2();// the entries of val are all 0
    void increaseDE();
    inline int8_t& operator[](int8_t i){return val[i];} //returns ith component of val
    inline const int8_t& operator[](int8_t i) const{return val[i];} //does the same but for const Z2
    Z2 operator+(Z2&); //handles addition
    Z2& operator+=(Z2&); //handles +=
    Z2 operator-(); //handles negation
    Z2 operator-(Z2&); //handles subtraction
    bool operator<(const int8_t&);
    const bool operator<(const Z2&) const;
    bool operator>(const int8_t&);
    bool operator>(Z2&);
    bool operator<=(Z2&);
    bool operator>=(Z2&);
    Z2 operator*(const Z2&); //function that handles multiplication
    const bool operator==(const Z2&) const; //function that checks equality between two Z2
    bool operator==(Z2&);
    bool operator==(const int8_t&); //function that checks equality between two Z2
    bool operator!=(const Z2&) const; //function that checks equality between two Z2
    Z2& operator=(const int8_t&); //function that makes the operator have equal entries to parameter
    Z2& operator=(const Z2&); //function that makes the operator have equal entries to parameter
    friend std::ostream& operator<<(std::ostream&,const Z2&); //display
    void negate(){val[0]=-val[0];val[1]=-val[1];}
    static const Z2 inverse_root2() {return Z2(1,0,1);}
    static const Z2 one() {return Z2(1,0,0);}
    static const Z2 zero() {return Z2(0,0,0);}
    inline static int count[] = {0,0,0}; // Useful for counting statistics, only for debugging/analysis
private:
    Z2& reduce(); //auxiliary function to make sure every triad is in a consistent most reduced form
    int8_t val[3]; //values of the Z2
};
