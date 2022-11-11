
class pattern{
    public:
        pattern();
        void lexicographic_sort();
        int hamming_weight_A();
        int hamming_weight_B();
        bool operator==(const pattern &) const;
        bool operator<(const pattern &) const;
        bool A[6][6];
        bool B[6][6];
        int col_signature(const int &) const;
        bool less_than_perm(const int*, const int*, const int*, const int*) const;
        friend std::ostream& operator<<(std::ostream&,const pattern &);
    private:
};