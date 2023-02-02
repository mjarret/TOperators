
class pattern{
    public:
        pattern();
        pattern(const bool[72]);
        void lexicographic_order();
        pattern pattern_mod();
        pattern transpose();
        bool* to_binary();

        bool operator==(const pattern &) const;
        bool operator<(const pattern &) const;
        friend std::ostream& operator<<(std::ostream&, const pattern &);

        std::pair<bool,bool> arr[6][6];
        std::vector<unsigned char> hist;
        std::string name(); 
        std::string human_readable();

        static int8_t lexicographical_compare(const std::pair<bool,bool>[6],const std::pair<bool,bool>[6]);
        static bool lexicographical_less(const std::pair<bool,bool>[6],const std::pair<bool,bool>[6]);
        static const pattern identity() {
            pattern I;
            for(int k =0; k<6; k++) {
                I.arr[k][k].first = 1;
            }
            return I;
        }

    private:
};