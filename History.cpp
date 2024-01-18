#include "History.hpp"
#include <sstream>

void History::add(unsigned char operation) {
    if (!operations.empty() && (operations.back() & 0xF0) == 0) {
        operations.back() |= (operation << 4);
    } else {
        operations.push_back(operation);
    }
}

std::string History::toString() const {
    std::stringstream ss;
    for (unsigned char op : operations) {
        int lower = (op & 15) - 1;
        ss << lower << " ";
        if (op > 15) {
            int upper = (op >> 4) - 1;
            ss << upper << " ";
        }
    }
    return ss.str();
}
