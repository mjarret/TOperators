
class pattern{
    public:
        pattern();
        pattern(const bool[72]);
        void lexicographic_order();
        static int8_t lexicographical_compare(const std::pair<bool,bool>[6],const std::pair<bool,bool>[6]);
        static bool lexicographical_less(const std::pair<bool,bool>[6],const std::pair<bool,bool>[6]);
        pattern pattern_mod();
        pattern transpose();
        bool operator==(const pattern &) const;
        bool operator<(const pattern &) const;
        std::pair<bool,bool> arr[6][6];
        int col_signature(const int &) const;
        bool less_than_perm(const int*, const int*, const int*, const int*) const;
        friend std::ostream& operator<<(std::ostream&,const pattern &);
        std::vector<uint8_t> hist;
        struct gate {
            char first : 4;
            char second : 4;
        };
        static const pattern identity() {
            pattern I;
            for(int k =0; k<6; k++) {
                I.arr[k][k].first = 1;
            }
            return I;
        }
        std::string name(); 
    private:
};