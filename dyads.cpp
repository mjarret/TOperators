#include<iostream>
#include<chrono>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<iterator>
#include<random>
#include<vector>
#include<algorithm>

using namespace std;

class Z2 {
// Store a value of (A + B√2)/2^K
    public:
        int A,B,K;
        int ret[3];             // Allocated space for routine operations, 
                                // seems to help. May want to allocate the
                                // scratch space elsewhere so that each 
                                // object doesn't hold its own.

        Z2() {}  

        Z2(int a, int b, int c) {
            A = a;
            B = b;
            K = c;
            reduce();
        }

        //  Input: Z2 other
        //  Output: this + other
        //  Best practice is to pass other as a constant so it can't be modified, but it's a bit less efficient and we can, at least in principle, be careful. The issue is scale(k). This could be made independent and also, mayyyyyybe, more efficient by defining a scale method that just rescales arbitrary values A,B,K. It seems like having the A,B,K close to the array ret stored in memory makes it more efficient, though, so it might be tricky to preserve efficiency and implement this function outside of the class unless we made scale a function of two array references which are near one another in memory or, alternatively, a single array of 6 values.
        //  Another good approach might be to reset the values of ret after each use to whatever the values of A,B,K happen to be? I think we want to keep ret and A,B,K close to one another in memory. It may be worth just having a single array?
        Z2  operator+(Z2 &other) {
            int k = max(this->K,other.K);
            int* arr0 = scale(k);
            int* arr1 = other.scale(k);  
            return Z2(arr0[0]+arr1[0],arr0[1]+arr1[1],k);
        }

        Z2 & operator+=(Z2 &other) {
            int k = max(K,other.K);
            int* arr0 = scale(k);
            int* arr1 = other.scale(k);  
            this->A = arr0[0] + arr1[0];
            this->B = arr0[1] +arr1[1];
            this->K = k;
            return *this;
        }

        //  Input: Z2 other
        //  Output: this - other
        Z2 operator-(const Z2 & other) {
            Z2 tmp = other;
            int k = max(K,other.K);
            int* arr0 = scale(k);
            int* arr1 = tmp.scale(k);
            return Z2(arr0[0]-arr1[0],arr0[1]-arr1[1],k);            
        }

        // Input: a Z2 other
        // Output: this == other
        bool operator==(const Z2 &other) {
            return (A == other.A && B == other.B && K == other.K);
        }

        Z2 & operator=(const Z2& other) {
            A = other.A;
            B = other.B;
            K = other.K;
            return *this;
        }

        Z2 operator* (const Z2 &other) {
            Z2 to_return;
            to_return.A = A*other.A + 2*(B*other.B);
            to_return.B = A*other.B + B*other.A;
            to_return.K = K + other.K;
            return to_return.reduce();
        }

        int * scale(const int & k) {
            int divresult = k-K;                   // change in exponent
            ret[0] = A << divresult;                 // natural -> natural / 2^...
            ret[1] = B << divresult;              // root_coeff -> root_coeff / 2^...
            ret[2] = k;        
            return ret;                                         
        }

        Z2 & reduce(){
            while(A % 2 == 0 && B % 2 == 0 && K > 0) {
                A >>= 1;                                 // A -> A/2
                B >>= 1;                                 // B -> B/2
                K--;                            
            }
            if(K < 0) {
                A <<= -K;                           // A -> A*2^(-K)
                B <<= -K;                           // B -> B*2^(-K)
                K = 0;                            
            }
            return *this;
        }

        int * full_reduce() {
            ret[0] = A;
            ret[1] = B;
            ret[2] = K;
            int tmp;
            while(ret[0] % 2 == 0 && ret[2] > 0) {
                 tmp = ret[0];
                 ret[0] = ret[1];
                 ret[1] = tmp >> 1;
                 ret[2]--;
            }
            return ret;
        }
};

ostream& operator<<(ostream& os, const Z2& z2) {
    os << '(' << z2.A << '+' << z2.B << "\u221A2)/2^" << z2.K;
    return os;
}

class SO6 {
    public:
        Z2 arr[6][6];

        SO6() {
            for(int i = 0; i < 6; i++) {
                for(int j = 0; j < 6; j++) {
                    arr[i][j] = Z2(0,0,0);
                }
            }
        }

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
            int tot;
            bool flag;
            Z2 dot_product;
            Z2 next;
            for(int i = 0; i < 6; i++) {
                tot = 0;
                for(int j = 0; j < 6; j++) {
                    dot_product = Z2(0,0,0);
                    for(int k = 0; k <6; k++) {
                        next = arr[i][k]*(other.arr)[j][k];
                        dot_product += next;
                    }
                    if(dot_product.B != 0 || dot_product.K != 0) {return false;}
                    tot += (dot_product.A)*(dot_product.A);
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

int getNum(vector<int>& v)
{
 
    // Size of the vector
    int n = v.size();
 
    // Generate a random number
    srand(time(NULL));
 
    // Make sure the number is within
    // the index range
    int index = rand() % n;
 
    // Get random number from the vector
    int num = v[index];
 
    // Remove the number from the vector
    swap(v[index], v[n - 1]);
    v.pop_back();
 
    // Return the removed number
    return num;
};
 

SO6 rand_perm() {
    std::vector<int> v = {0,1,2,3,4,5};
    std::random_shuffle(v.begin(), v.end());
    SO6 to_return;
    for(int i = 0; i < 6; i++) {
        for(int j = 0; j <6; j++) {
            if(v.at(i) == j) {
                int sign = 1- 2*(rand() % 2);
                to_return.arr[i][j] = Z2(sign,0,0);
            }
        }
    }
    return to_return;
};


int & func(int &i) {
    return i;
}

int func2(int &i) {
    return i;
}

int main() {
    Z2 tmp;
    Z2 tmp2 = Z2(81023490,708386,104);
    Z2 tmp4 = Z2(16234968,223934,239);
    auto start = chrono::steady_clock::now();
    for(int i = 0; i < (1e6); i ++) {
        tmp2 + tmp4;
    }
    auto end = chrono::steady_clock::now();
    cout << "Elapsed time in microseconds: "
        << chrono::duration_cast<chrono::milliseconds>(end - start).count()
        << " ms" << endl; 
    cout << tmp4.A << "," << tmp4.B << "," << tmp4.K << endl;
    cout << tmp.A << "," << tmp.B << "," << tmp.K << endl;
    printf("\u221A\n");
    SO6 A = rand_perm();
    SO6 B = rand_perm();
    // B.arr[5][5] = Z2(10,-3,5);
    cout << (A==B) << endl;
    return (0);
};