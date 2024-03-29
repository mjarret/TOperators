#include <iostream>
#include "Z2.hpp" // Include the header file where Z2 class is defined

Z2 slowAdd(Z2 aIn, Z2 bIn) {
    Z2 a = aIn;
    Z2 b = bIn;
    // Find Common Denominator
    int common_exponent = std::max(a.exponent, b.exponent);
    // Convert both terms to have the same exponent
    for(int i = 0; i < common_exponent - a.exponent; ++i) {
        std::swap(a.intPart, a.sqrt2Part);
        a.intPart <<= 1;
    }
    a.exponent=common_exponent;

    for(int i = 0; i < common_exponent - b.exponent; ++i) {
        std::swap(b.intPart, b.sqrt2Part);
        b.intPart <<= 1;
    }
    b.exponent = common_exponent;

    // Do addition
    a.intPart += b.intPart;
    a.sqrt2Part += b.sqrt2Part;

    // Reduce the result
    if (a.intPart == 0 && a.sqrt2Part == 0) {
        return Z2(0,0,0); // Reset exponent for a clear representation of zero
    }

    // Loop until at least one part (A or B) becomes odd
    while ((a.intPart & 1) == 0 && (a.sqrt2Part & 1) == 0) {
        // Both parts are even, so divide both by 2 and adjust the exponent
        a.intPart >>= 1; // Equivalent to dividing by 2
        a.sqrt2Part >>= 1; // Equivalent to dividing by 2
        a.exponent -= 2; // Adjusting exponent because we divided the numerator by 2
    }

    // If the integer part is still even (and sqrt2Part is odd or we wouldn't be here)
    if ((a.intPart & 1) == 0) {
        // Divide the even part (A) by 2, effectively moving one sqrt(2) to the numerator
        std::swap(a.intPart, a.sqrt2Part);
        a.sqrt2Part >>= 1;
        // Swap intPart and sqrt2Part to maintain the form, and decrement the exponent
        a.exponent--;
    }

    return a;
}

int main() {
    int totalIterations = 100000000;
    int percentInterval = totalIterations / 100;
    int percentCounter = 0;

    for(int i = 0; i < totalIterations; ++i) {
        // Check if 1 percent of the loop is completed
        if (i % percentInterval == 0) {
            std::cout << "Progress: " << percentCounter << "% completed" << std::endl;
            percentCounter++;
        }

        Z2 a((rand() % 10 + 1) * (rand() % 2 ? 1 : -1), rand() % 21 - 10, rand() % 5);
        Z2 b((rand() % 10 + 1) * (rand() % 2 ? 1 : -1), rand() % 21 - 10, rand() % 5);
        Z2 c = slowAdd(a, b);
        Z2 d = a + b;
        if (c != d) {
            std::cout << "Error: " << a << " + " << b << " = " << c << " != " << d << std::endl;
            return 1;
        }
    }
    return 0;
}