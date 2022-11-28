#include <iostream>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include "pattern.hpp"

pattern::pattern() {
}

pattern::pattern(const bool binary_rep[72]) {
    for (int col = 0; col < 6; col++)
    {
        for (int row = 0; row < 6; row++)
        {
            arr[col][row].first = binary_rep[2 * col + 12 * row]; 
            arr[col][row].second = binary_rep[2 * col + 12 * row + 1]; 
        }
    }
}

bool pattern::operator==(const pattern &other) const {
    for(int col = 0; col < 6; col++) {
        for(int row = 0; row < 6; row++) {
            if(arr[col][row] != other.arr[col][row]) return false;
        }
    }
    return true;
}

bool pattern::operator<(const pattern &other) const {
    for(int col = 0; col < 6; col++) {
        for(int row = 0; row < 6; row++) {
            if(arr[col][row] == other.arr[col][row]) {
                continue;
            }
            return arr[col][row] < other.arr[col][row];
        }
    }
    return false;
}

int8_t pattern::lexicographical_compare(const std::pair<bool,bool> first[6],const std::pair<bool,bool> second[6])
{
    int row;
    for (row = 0; row < 6; row++)
    {
        if (first[row] == second[row]) {
            continue;
        }
        if (first[row] > second[row])           // Reverse ordering is better for tests
            return -1;
        return 1;
    }
    return 0;
}

bool pattern::lexicographical_less(const std::pair<bool,bool> first[6],const std::pair<bool,bool> second[6])
{
    return (pattern::lexicographical_compare(first,second)<0);
}

void pattern::lexicographic_order() {
    for (int i = 1; i < 6; i++)
    {
        for (int j = i; j > 0 && pattern::lexicographical_less(arr[j],arr[j-1]); j--)  std::swap(arr[j], arr[j - 1]);
    }
}

pattern pattern::pattern_mod() {
    pattern ret = *this;
    for (int col = 0; col < 6; col++) {
        for(int row = 0; row < 6; row++) {
            if(ret.arr[col][row].first == 0) continue;
            ret.arr[col][row].second = !ret.arr[col][row].second; 
        }
    }
    ret.lexicographic_order();
    return ret;
}

pattern pattern::transpose() {
    pattern ret = *this;
    for (int col = 0; col < 6; col++) {
        for(int row = col+1; row < 6; row++) {
            std::swap(ret.arr[col][row],ret.arr[row][col]);
        }
    }
    ret.lexicographic_order();
    return ret;
}

// void pattern::row_sort_by_column(const int & col) {

// }

/**
 * Overloads << function for SO6.
 * @param os reference to ostream object needed to implement <<
 * @param m reference to SO6 object to be displayed
 * @returns reference ostream with the matrix's display form appended
 */
std::ostream &operator<<(std::ostream &os, const pattern &m)
{
    os << "\n";
    for (int row = 0; row < 6; row++)
    {
        if (row == 0)
            os << "⌈ ";
        else if (row == 5)
            os << "⌊ ";
        else
            os << "| ";
        for (int col = 0; col < 6; col++)
            os << m.arr[col][row].first << ',' << m.arr[col][row].second << ' ';
        if (row == 0)
            os << "⌉\n";
        else if (row == 5)
        {
            os << "⌋\n";
        }
        else
        {
            os << "|\n";
        }
    }
    os << "\n";
    return os;
}

std::string pattern::name()
{
    std::string ret = "";
    for (char i : hist)
    {
        ret.append(std::to_string(i%15));
    }
    return ret;
}