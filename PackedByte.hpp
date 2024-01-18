class PackedByte {
public:
    unsigned char value;

    // Constructor to initialize with two 4-bit values
    PackedByte(unsigned char, unsigned char);

    // Default constructor
    PackedByte();

    // Set the lower 4 bits
    void setLower(unsigned char);

    // Set the upper 4 bits
    void setUpper(unsigned char);

    // Get the lower 4 bits
    unsigned char getLower();

    // Get the upper 4 bits
    unsigned char getUpper();
};
