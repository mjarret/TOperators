
#include <vector>
#include "Z2.hpp"

class SO6{
    public:
        SO6();
        // SO6(std::vector<int8_t> t);
        // SO6(Z2[6][6], std::vector<int8_t> t); //initializes matrix according to a 6x6 array of Z2
        SO6 operator*(SO6&); //mutliplication
        void fixSign();
        void lexOrder();
        inline Z2& operator()(int col, int row){return arr[col][row];} //returns the (i,j)th entry
        bool operator<(const SO6 &) const;
        const Z2& operator()(int i, int j) const {return arr[i][j];} //returns the (i,j)th entry but for const
        bool operator==(SO6&); //checking equality up to signed permutation
        bool operator==(const SO6 &) const;
        Z2* operator[](const int i) {return arr[i];}  // Return the array element needed. 
        const Z2* operator[](const int i) const {return arr[i];}  // Return the array element needed. 
        SO6 left_multiply_by_T(const int &);
        SO6 left_multiply_by_T(const int &,const int &,const int &);
        SO6 left_multiply_by_T_transpose(const int &);        
        SO6 pattern_mod();
        void genLDE(); //generates LDE, called after multiplication and constructor
        friend std::ostream& operator<<(std::ostream&,const SO6&); //display
        static const SO6 identity() {
            SO6 I;
            for(int k =0; k<6; k++) {
                I(k,k) = 1;
            }
            return I;
        }
        z2_int getLDE();
        SO6 getPattern();
        void to_pattern();
        SO6 transpose();
        std::string printName(); 
        void row_permute(int *);
        std::vector<uint8_t> hist;
    private:
        Z2 arr[6][6];
};
