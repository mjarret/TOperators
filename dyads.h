#ifndef DYADS_H
#define DYADS_H
#include<stdlib.h>
#include<iostream>
#include<algorithm>
using namespace std;

class Z2 {
// Store a value of (A + B√2)/2^K
    private:
        int A;
        int B;
        int K;
        int ret[3];             // Allocated space for routine operations, 
                                // seems to help. May want to allocate the
                                // scratch space elsewhere so that each 
                                // object doesn't hold its own.
    public:
        Z2();
        Z2(int a, int b, int c);
        //  Input: Z2 other
        //  Output: this + other

        void setA(int A) {
            this->A = A;
        }

        int getA() {
            return this->A;
        }

        void setB(int B) {
            this->B = B;
        }

        int getB(int B) {
            return this->B;
        }

        void setK(int K) {
            this->K = K;
        }

        int getK() {
            return this->K;
        }

        Z2 & operator+=(Z2 &other) {
            int k = max(K,other.K);
            int* arr0 = scale(k);
            int* arr1 = other.scale(k);  
            A = arr0[0] + arr1[0];
            B = arr0[1] +arr1[1];
            K = k;
            return *this;
        }


        //  Input: Z2 other
        //  Output: this - other
        Z2 operator-(Z2 & other) {
            int k = max(K,other.K);
            int* arr0 = scale(k);
            int* arr1 = other.scale(k);
            return Z2(arr0[0]-arr1[0],arr0[1]-arr1[1],k);            
        }

        // Input: a Z2 other
        // Output: this == other
        bool operator==(const Z2 &other) {
            return (A == other.A && B == other.B && K == other.K);
        }

        Z2 operator+ (const Z2 &other);

        Z2 operator* (const Z2 &other) {
            Z2 to_return;
            to_return.A = A*other.A + 2*(B*other.B);
            to_return.B = A*other.B + B*other.A;
            to_return.K = K + other.K;
            to_return.reduce();
            return to_return;
        }

        int * scale(const int & k) {
            int divresult = k-K;                   // change in exponent
            ret[0] = A << divresult;                 // natural -> natural / 2^...
            ret[1] = B << divresult;              // root_coeff -> root_coeff / 2^...
            ret[2] = k;        
            return ret;                                         
        }

        void reduce();
};

Z2  operator+(Z2 &a, Z2 &other) {
    int k = max(a.K,other.K);
    int* arr = a.scale(k);
    int* arr0 = a.scale(k);
    int* arr1 = other.scale(k);  
    return Z2(arr0[0]+arr1[0],arr0[1]+arr1[1],k);
}

void reduce(Z2 &a) {
    while(a.A % 2 == 0 && a.B % 2 == 0 && a.K > 0) {
        a.A = a.A >> 1;
        a.B = a.B >> 1;
        a.K--;
    }
    return;
}

Z2::Z2() {
    this->A = 0;
    this->B = 0;
    this->K = 0;
}

Z2::Z2(int a, int b, int c) {
    this->A = a;
    this->B = b;
    this->K = c;
    reduce();
}

#endif