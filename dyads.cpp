#include<iostream>
#include<chrono>
#include<unistd.h>

using namespace std;

class Z2 {

    public:
        int natural;
        int root_coeff;
        int denom_exp;

        Z2() {
            natural = 0;
            root_coeff = 0;
            denom_exp = 0;
        }  

        Z2(int a, int b, int c) {
            natural = a;
            root_coeff = b;
            denom_exp = c;
        }

        Z2  operator+(const Z2& other){
            Z2 tmp = other;
            int* arr0 = new int[3];
            int* arr1 = new int[3];
            int k = max(other.denom_exp,denom_exp);
            arr0 = scale(k);
            arr1 = tmp.scale(k);
            Z2 ret = Z2(arr0[0]+arr1[0],arr0[1]+arr1[1],k);
            ret.reduce();
            return ret;
        }

        int * scale(int k){
            int *ret = new int[3];
            ret[0] = natural;
            ret[1] = root_coeff;
            ret[2] = denom_exp;
            while(ret[2] < k) {
                int tmp = ret[0];
                ret[0] = ret[1]<<1;
                ret[1] = tmp;
                ret[2]++;
            }
            return ret;
        }

        void reduce(){
            while(natural % 2 == 0 && denom_exp > 0) {
                int tmp = natural;
                natural = root_coeff;
                root_coeff = tmp>>1;
                denom_exp = denom_exp-1;
            }
            return;
        }

};

int * scale(int (&a)[3], int k) {
        int * ret = new int[3];
        int tmp;
        ret = a;
        while(ret[2] < k) {
            tmp = ret[0];
            ret[0] = ret[1]<<1;
            ret[1] = tmp;
            ret[2] = ret[2]+1;
        }
        return ret;
};

int * add(int (&a)[3], int (&b)[3]) {
        int k = max(a[2],b[2]);
        int * v0 = new int[3];
        int * v1 = new int[3];
        v0 = scale(a,k);
        v1 = scale(b, k);
        int * ret = new int[3];
        ret[0] = v0[0]+v1[0];
        ret[1] = v0[1] + v1[1]; 
        ret[2] = k;
        return ret;
};

int main() {
    Z2 tmp;
    Z2 tmp2 = Z2(14098400,42341098,8321);
    Z2 tmp4 = Z2(39481920,49276910,8340);
    long int s = 9999999999;
    auto start = chrono::steady_clock::now();
    for(int i = 0; i < 10000; i ++) {
        tmp2 + tmp4;
    }
    auto end = chrono::steady_clock::now();
    cout << "Elapsed time in milliseconds: "
        << chrono::duration_cast<chrono::milliseconds>(end - start).count()
        << " ms" << endl; 

    long long i = (1e20+1);
    cout << i << endl;

    cout << sizeof(i) << endl;
    cout << sizeof(s) << endl;
    return 0;
};