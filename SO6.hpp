
#include <vector>
#include "Z2.hpp"
#include "pattern.hpp"

class SO6{
    public:
        std::vector<unsigned char> hist;
        SO6();
        SO6(Z2[6][6]); //initializes matrix according to a 6x6 array of Z2
        
        SO6 operator*(const SO6&) const; //mutliplication
        Z2& operator()(int &col, int &row) {return arr[permutation[col]][row];} //returns the (i,j)th entry
        bool operator<(const SO6 &) const;
        bool operator==(SO6&); //checking equality up to signed permutation
        bool operator==(const SO6 &) const;
        bool operator!=(const SO6 &other) const;
        Z2* operator[](const int &i) {return arr[i];}  // Return the array element needed. 
        const Z2* operator[](const int &i) const {return arr[i];}  // Return the array element needed. 
        friend std::ostream& operator<<(std::ostream&,const SO6&); //display

        Z2 unpermuted(const int &i, const int &j) const {return arr[permutation[i]][j];}  // Return the array element needed. 
        SO6 left_multiply_by_T(const int &) const;
        SO6 left_multiply_by_circuit(std::vector<unsigned char> &);
        SO6 left_multiply_by_T(const int &,const int &,const unsigned char &) const;
        SO6 left_multiply_by_T_transpose(const int &);        
        void genLDE(); //generates LDE, called after multiplication and constructor

        z2_int getLDE();
        pattern to_pattern();
        SO6 reconstruct();
        SO6 transpose();
        std::string name(); 
        std::string circuit_string(); 
        
        
        void row_permute(int *);
        void unpermuted_print();
        void lexicographic_order();

        static SO6 reconstruct(const std::string);
        static std::string name_as_num(const std::string);
        static int8_t lexicographical_compare(const Z2[6],const Z2[6]);
        static bool lexicographical_less(const Z2[6],const Z2[6]);
        static const SO6 identity() {
            SO6 I;
            for(int k =0; k<6; k++) {
                I(k,k) = 1;
            }
            return I;
        }

    private:
        uint8_t permutation[6] = {0,1,2,3,4,5};
        Z2 arr[6][6];
};
