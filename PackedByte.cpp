class PackedByte {
public:
    unsigned char value;

    // Constructor to initialize with two 4-bit values
    PackedByte(unsigned char lower, unsigned char upper) {
        setLower(lower);
        setUpper(upper);
    }

    // Default constructor
    PackedByte() : value(0) {}

    // Set the lower 4 bits
    void setLower(unsigned char lower) {
        value = (value & 0xF0) | (lower & 0x0F);
    }

    // Set the upper 4 bits
    void setUpper(unsigned char upper) {
        value = (value & 0x0F) | ((upper & 0x0F) << 4);
    }

    // Get the lower 4 bits
    unsigned char getLower() const {
        return value & 0x0F;
    }

    // Get the upper 4 bits
    unsigned char getUpper() const {
        return (value >> 4) & 0x0F;
    }
};
