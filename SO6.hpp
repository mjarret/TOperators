
#include <vector>
#include "Z2.hpp"
#include "pattern.hpp"

class SO6{
    public:
        SO6();
        // SO6(std::vector<int8_t> t);
        SO6(Z2[6][6]); //initializes matrix according to a 6x6 array of Z2
        SO6 operator*(SO6&); //mutliplication
        void fixSign();
        inline Z2& operator()(int col, int row){return arr[permutation[col]][row];} //returns the (i,j)th entry
        bool operator<(const SO6 &) const;
        const Z2& operator()(int i, int j) const {return arr[permutation[i]][j];} //returns the (i,j)th entry but for const
        bool operator==(SO6&); //checking equality up to signed permutation
        bool operator==(const SO6 &) const;
        // Z2* operator[](const int i) {return arr[permutation[i]];}  // Return the array element needed. 
        Z2* operator[](const int i) {return arr[i];}  // Return the array element needed. 
        Z2 unpermuted(const int i, const int j) {return arr[permutation[i]][j];}  // Return the array element needed. 
        // const Z2* operator[](const int i) const {return arr[permutation[i]];}  // Return the array element needed. 
        const Z2* operator[](const int i) const {return arr[i];}  // Return the array element needed. 
        SO6 left_multiply_by_T(const int &);
        SO6 left_multiply_by_circuit(std::vector<unsigned char> &);
        SO6 left_multiply_by_T(const int &,const int &,const unsigned char &);
        SO6 left_multiply_by_T_transpose(const int &);        
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
        pattern to_pattern();
        pattern pattern_mod();
        SO6 reconstruct();
        static SO6 reconstruct(const std::string);
        SO6 transpose();
        std::string name(); 
        static std::string name_as_num(const std::string);
        void row_permute(int *);
        void unpermuted_print();
        std::vector<unsigned char> hist;
        static int8_t lexicographical_compare(const Z2[6],const Z2[6]);
        static bool lexicographical_less(const Z2[6],const Z2[6]);
        void lexicographic_order();
        Z2 arr[6][6];
        uint8_t permutation[6] = {0,1,2,3,4,5};
        struct gate {
            char first : 4;
            char second : 4;
        };
    private:
        
};
