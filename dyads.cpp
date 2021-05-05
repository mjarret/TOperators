#include<iostream>
#include<chrono>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

using namespace std;

class Z2 {
// Store a value of (a + b√2)/2^k
    public:
        int natural;
        int root_coeff;
        int denom_exp;
        int ret[3];             // Allocated space for routine operations, 
                                // seems to help. May want to allocate the
                                // scratch space elsewhere so that each 
                                // object doesn't hold its own.

        Z2() {
            natural = 0;
            root_coeff = 0;
            denom_exp = 0;
        }  

        Z2(int a, int b, int c) {
            natural = a;
            root_coeff = b;
            denom_exp = c;
            reduce();
        }

        //  Input: Z2 other
        //  Output: this + other
        Z2  operator+(Z2 &other) {
            int k = max(denom_exp,other.denom_exp);
            int* arr0 = scale(k);
            int* arr1 = other.scale(k);  
            return Z2(arr0[0]+arr1[0],arr0[1]+arr1[1],k);
        }

        Z2 & operator+=(Z2 &other) {
            int k = max(denom_exp,other.denom_exp);
            int* arr0 = scale(k);
            int* arr1 = other.scale(k);  
            natural = arr0[0] + arr1[0];
            root_coeff = arr0[1] +arr1[1];
            denom_exp = k;
            return *this;
        }


        //  Input: Z2 other
        //  Output: this - other
        Z2 operator-(Z2 & other) {
            int k = max(denom_exp,other.denom_exp);
            int* arr0 = scale(k);
            int* arr1 = other.scale(k);
            return Z2(arr0[0]-arr1[0],arr0[1]-arr1[1],k);            
        }

        // Input: a Z2 other
        // Output: this == other
        bool operator==(const Z2 &other) {
            return (natural == other.natural && root_coeff == other.root_coeff && denom_exp == other.denom_exp);
        }

        Z2 operator* (const Z2 &other) {
            Z2 to_return;
            to_return.natural = natural*other.natural + 2*(root_coeff*other.root_coeff);
            to_return.root_coeff = natural*other.root_coeff + root_coeff*other.natural;
            to_return.denom_exp = denom_exp + other.denom_exp;
            to_return.reduce();
            return to_return;
        }

        int * scale(const int & k) {
            int divresult = k-denom_exp;             // floor(difference/2) , remainder
            ret[0] = natural << divresult;                 // natural -> natural / 2^quotient
            ret[1] = root_coeff << divresult;              // root_coeff -> root_coeff / 2^quotient
            ret[2] = k;        
            return ret;                                         
        }

        void reduce(){
            while(natural % 2 == 0 && root_coeff % 2 == 0 && denom_exp > 0) {
                natural = natural >> 1;
                root_coeff = root_coeff >> 1;
                denom_exp--;
            }
            return;
        }
};

class SO6 {
    private:
        Z2 arr[6][6];

    public: 
        SO6() {}

        SO6 operator* (SO6 &other) {
            SO6 to_return;
            Z2 next;
            for(int i = 0; i < 6; i++) {
                for(int j = 0; j < 6; j++) {
                    for(int k = 0; k <6; k++) {
                        next = arr[i][k]*(other.arr)[k][j];
                        arr[i][j] += next;
                    }
                }
            }
            return to_return;
        }

        bool operator==(SO6 &other) {
            SO6 to_return;
            int tot;
            bool flag;
            Z2 dot_product;
            Z2 next;
            for(int i = 0; i < 6; i++) {
                tot = 0;
                for(int j = 0; j < 6; j++) {
                    dot_product = Z2(0,0,0);
                    for(int k = 0; k <6; k++) {
                        next = arr[i][k]*(other.arr)[i][k];
                        dot_product += next;
                    }
                    if(dot_product.root_coeff != 0 || dot_product.denom_exp != 0) {return false;}
                    tot += (dot_product.natural)*(dot_product.natural);
                    if(tot > 1) {return false;}
                }
                if(tot !=1) {return false;}
            }
            return true;
        }
    
        Z2 get_element(int i, int j) {
            return arr[i][j];
        }

        void set_element(int i, int j, Z2 z2) {
            arr[i][j] = z2;
        }
};

int main() {
    Z2 tmp;
    Z2 tmp2 = Z2(81023490,708386,104);
    Z2 tmp4 = Z2(16234968,223934,239);
    auto start = chrono::steady_clock::now();
    for(int i = 0; i < (1e6); i ++) {
        tmp2+tmp4;
        tmp2*tmp4;
    }
    auto end = chrono::steady_clock::now();
    cout << "Elapsed time in microseconds: "
        << chrono::duration_cast<chrono::milliseconds>(end - start).count()
        << " ms" << endl; 
    cout << tmp4.natural << "," << tmp4.root_coeff << "," << tmp4.denom_exp << endl;
    cout << tmp.natural << "," << tmp.root_coeff << "," << tmp.denom_exp << endl;
    printf("\u221A\n");
    SO6 A;
    SO6 B;
    A*B;
    return 0;
};