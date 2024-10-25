#include <gtest/gtest.h>      // Google Test framework
#include <algorithm>          // For std::shuffle
#include <random>             // For generating random numbers
#include <set>
#include <chrono>
#include "SO6.hpp"            // Include SO6 header
#include "Z2.hpp"             // Include Z2 header
#include <iostream>           // For standard input/output

void test_SO6_iterator() {
    SO6 candidate;

    // Initialize the arr[] as a 6x6 matrix for testing
    Z2 val = Z2(1,0,0);
    for (int i = 0; i < 36; ++i) {
        candidate.arr[i] = (val += Z2(1,0,0));
    }

    // Set Row and Col to known permutations
    candidate.Row[0] = 1;  // First row is now the second row
    candidate.Row[1] = 2;  // Second row is now the third row
    candidate.Row[2] = 0;  // Third row is now the first row
    candidate.Row[3] = 4;
    candidate.Row[4] = 5;
    candidate.Row[5] = 3;

    candidate.Col[0] = 2;  // First column is now the third column
    candidate.Col[1] = 1;  // Second column is now the second column
    candidate.Col[2] = 0;  // Third column is now the first column
    candidate.Col[3] = 5;
    candidate.Col[4] = 4;
    candidate.Col[5] = 3;

    // Expected values based on the permutation of Row and Col
    // This simulates the matrix access after Row and Col permutations
    Z2 expected_values[] = {
        candidate.arr[1 * 6 + 2], candidate.arr[1 * 6 + 1], candidate.arr[1 * 6 + 0], candidate.arr[1 * 6 + 5], candidate.arr[1 * 6 + 4], candidate.arr[1 * 6 + 3],
        candidate.arr[2 * 6 + 2], candidate.arr[2 * 6 + 1], candidate.arr[2 * 6 + 0], candidate.arr[2 * 6 + 5], candidate.arr[2 * 6 + 4], candidate.arr[2 * 6 + 3],
        candidate.arr[0 * 6 + 2], candidate.arr[0 * 6 + 1], candidate.arr[0 * 6 + 0], candidate.arr[0 * 6 + 5], candidate.arr[0 * 6 + 4], candidate.arr[0 * 6 + 3],
        candidate.arr[4 * 6 + 2], candidate.arr[4 * 6 + 1], candidate.arr[4 * 6 + 0], candidate.arr[4 * 6 + 5], candidate.arr[4 * 6 + 4], candidate.arr[4 * 6 + 3],
        candidate.arr[5 * 6 + 2], candidate.arr[5 * 6 + 1], candidate.arr[5 * 6 + 0], candidate.arr[5 * 6 + 5], candidate.arr[5 * 6 + 4], candidate.arr[5 * 6 + 3],
        candidate.arr[3 * 6 + 2], candidate.arr[3 * 6 + 1], candidate.arr[3 * 6 + 0], candidate.arr[3 * 6 + 5], candidate.arr[3 * 6 + 4], candidate.arr[3 * 6 + 3]
    };

    int index = 0;
    for (auto it = candidate.begin(); it != candidate.end(); ++it) {
        Z2 actual_value = *it;
        Z2 expected_value = expected_values[index++];
        std::cout << "Expected: " << expected_value << ", Actual: " << actual_value << std::endl;
        assert(actual_value == expected_value);
    }

    std::cout << "All tests passed!" << std::endl;
    std::cin.get();
}

void test_SO6_iterator_operations() {
    SO6 candidate;

    // Initialize the arr[] array with values 1 through 36 (representing a 6x6 matrix)
    int val = 1;
    for (int i = 0; i < 36; ++i) {
        candidate.arr[i] = val++;
    }

    // Set Col and Row to non-trivial permutations (for example, reverse order)
    candidate.Col[0] = 5;
    candidate.Col[1] = 4;
    candidate.Col[2] = 3;
    candidate.Col[3] = 2;
    candidate.Col[4] = 1;
    candidate.Col[5] = 0;

    candidate.Row[0] = 2;
    candidate.Row[1] = 3;
    candidate.Row[2] = 1;
    candidate.Row[3] = 5;
    candidate.Row[4] = 0;
    candidate.Row[5] = 4;

    // Now test iterator dereference, increment, and random access using the new permutations

    // Test dereferencing and expected values after applying Row and Col permutations
    auto it = candidate.begin();
    Z2 expected_first_element = candidate.arr[(candidate.Col[0] << 2) + (candidate.Col[0] << 1) + candidate.Row[0]];
    assert(*it == expected_first_element);  // First element according to Row and Col permutations

    ++it;
    Z2 expected_second_element = candidate.arr[(candidate.Col[0] << 2) + (candidate.Col[0] << 1) + candidate.Row[1]];
    assert(*it == expected_second_element);  // Second element after incrementing the iterator

    // Test adding an integer to the iterator (random access)
    it = candidate.begin() + 5;
    Z2 expected_sixth_element = candidate.arr[(candidate.Col[0] << 2) + (candidate.Col[0] << 1) + candidate.Row[5]];
    assert(*it == expected_sixth_element);  // 6th element in the permuted array

    // Test subtracting an integer from the iterator
    it = it - 2;  // Move two steps back
    Z2 expected_fourth_element = candidate.arr[(candidate.Col[0] << 2) + (candidate.Col[0] << 1) + candidate.Row[3]];
    assert(*it == expected_fourth_element);  // 4th element after moving two steps back

    // Test iterator comparison (equality and inequality)
    auto it2 = candidate.begin();
    assert(it != it2);  // Should not be equal (different elements)
    
    it2 = candidate.begin() + 3;
    assert(it2 == it);  // Should now be equal

    // Test iterator difference
    it = candidate.begin() + 10;
    it2 = candidate.begin() + 4;
    assert(it - it2 == 6);  // Difference should be 6

    // Test that end() correctly marks the end of the range
    assert(candidate.end() - candidate.begin() == 36);  // Entire matrix has 36 elements

    std::cout << "All iterator tests with non-trivial Row and Col passed!" << std::endl;
}

// Main function for running tests
int main(int argc, char **argv) {
    test_SO6_iterator_operations();
    // // Create an instance of SO6
    SO6 first = SO6::identity();
    // first.canonical_form_test();
    first = first.left_multiply_by_T(0);
    // first.unpermuted_print();
    first = first.left_multiply_by_T(9);
    SO6 second = first;
    // first.unpermuted_print();
    // std::cout << first.circuit_string() << std::endl;
    first = first.left_multiply_by_T(10);
    // first.unpermuted_print();
    std::cout << first.circuit_string() << std::endl;

    first.unpermuted_print();
    first.physical_print();
    second.unpermuted_print();
    second.physical_print();
    std::cout << (first < second) << std::endl;
    std::cout << (second < first) << std::endl;
    // std::set<SO6> unique_matrices;
    // for(int i = 0; i < 15; i++) {
    //     SO6 test_matrix = first.left_multiply_by_T(i);
    //     test_matrix.canonical_form_test();
    //     auto result = unique_matrices.insert(test_matrix);
    // }
    // std::cout << "Number of unique matrices: " << unique_matrices.size() << std::endl;
    // for (const auto& matrix : unique_matrices) {
    //     matrix.unpermuted_print();
    // }
    // // Generate all possible combinations of first.left_multiply_by_T(0)
    // first = SO6::identity();
    
    // // Start timer
    // auto start = std::chrono::high_resolution_clock::now();
    
    // for (int i = 0; i < 15; i++) {
    //         SO6 combination = first.left_multiply_by_T(i);
    //         unique_matrices.insert(combination);
    // }
    
    // // End timer
    // auto end = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> elapsed = end - start;
    
    // std::cout << "Total number of unique combinations: " << unique_matrices.size() << std::endl;
    // std::cout << "Time taken: " << elapsed.count() << " seconds" << std::endl;
}
