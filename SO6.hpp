#ifndef SO6_HPP
#define SO6_HPP

#include <vector>
#include "Z2.hpp"
#include "pattern.hpp"

class pattern;

class SO6{
    public:
        std::vector<unsigned char> hist;
        SO6();
        SO6(Z2[6][6]); //initializes matrix according to a 6x6 array of Z2
        
        SO6 operator*(const SO6&) const; //mutliplication
        SO6 operator*(const pattern &) const;

        bool operator<(const SO6 &) const;
        bool operator==(SO6&); //checking equality up to signed permutation
        bool operator==(const SO6 &) const;
        bool operator!=(const SO6 &other) const;
        // Z2* operator[](const int &i) {return arr[i];}  // Return the array element needed. 
        inline int get_index(const int &row, const int &col) const {return (col<<2) + (col<<1) + row;}
        Z2* operator[](const int &col) {return arr + get_index(0,col);}  // Return the array element needed.
        Z2& get_element(const int &row, const int &col) {return arr[get_index(row,col)];}  // Return the array element needed.
        // const Z2* operator[](const int &i) const {return arr[i];}  // Return the array element needed. 
        const Z2* operator[](const int &col) const {return arr + get_index(0,col);}  // Return the array element needed. 
        friend std::ostream& operator<<(std::ostream&,const SO6&); //display

        // Z2 unpermuted(const int &col, const int &row) const {return arr[permutation[col]][row];}  // Return the array element needed. 
        SO6 left_multiply_by_T(const int &) const;
        SO6 left_multiply_by_circuit(std::vector<unsigned char> &);
        SO6 left_multiply_by_T(const int &,const int &,const unsigned char &) const;
        SO6 left_multiply_by_T_transpose(const int &);        

        const z2_int getLDE() const;
        pattern to_pattern() const;
        SO6 reconstruct();
        SO6 transpose();
        std::string name(); 
        std::string circuit_string(); 
        static SO6 reconstruct_from_circuit_string(const std::string& );        
        
        void update_history(const unsigned char &); 
        
        void row_permute(int *);
        void unpermuted_print();
        void lexicographic_order();

        static SO6 reconstruct(const std::string);
        static std::string name_as_num(const std::string);
        static int8_t lexicographical_compare(const Z2[6],const Z2[6]);
        int8_t lexicographical_compare(const int &col1,const int & col2);
        static bool lexicographical_less(const Z2[6],const Z2[6]);
        bool lexicographical_less(const int &col1,const int & col2);
        static const SO6 identity() {
            SO6 I;
            for(int k =0; k<6; k++) {
                I.arr[(k<<2) + (k<<1) + k] = 1;
            }
            return I;
        }
        uint8_t lex_order[6] = {0,1,2,3,4,5};
    private:
        // Z2 arr[6][6];
        Z2 arr[36];
};

#endif
