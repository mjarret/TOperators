#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <set>
#include <algorithm>
#include <random>
#include "Z2.hpp"
#include "SO6.hpp"


/**
 * @file utils.hpp
 * @brief Utility functions for set operations and conversions.
 */
class utils {
public:
    /**
     * @brief Calculate the free multiply depth.
     * @param target_T_count Total T count.
     * @param stored_depth_max Maximum depth to store.
     * @return Free multiply depth.
     */
    static int free_multiply_depth(int target_T_count, int stored_depth_max) {
        return target_T_count - stored_depth_max;
    }

    /**
     * @brief Converts a set of SO6s to a shuffled vector and clears the set.
     * @param s Set of SO6 to be converted.
     * @return A shuffled vector containing the elements originally in the set.
     */
    static std::vector<SO6> convert_to_vector_and_clear(std::set<SO6>& s) {
        std::vector<SO6> v(std::make_move_iterator(s.begin()), std::make_move_iterator(s.end()));
        s.clear(); // Clear the set
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(v.begin(), v.end(), g);
        return v;  // Return vector
    }


    /**
     * @brief Performs the set difference operation (A \ B).
     * Erases elements from set A that are also in set B.
     * @param A Set from which elements will be erased.
     * @param B Set containing elements to be removed from A.
     */
    static void setDifference(std::set<SO6>& A, const std::set<SO6>& B) {
        for (auto it = B.begin(); it != B.end(); ++it) {
            A.erase(*it);
        }
    }

    /**
     * @brief Rotates and clears sets for the next iteration.
     * @param prior Set to be cleared.
     * @param current Set to be moved to prior.
     * @param next Set to be moved to current.
     */
    static void rotate_and_clear(std::set<SO6>& prior, std::set<SO6>& current, std::set<SO6>& next) {
        std::set<SO6>().swap(prior); // Clear prior
        prior.swap(current); // Move current to prior
        current.swap(next); // Move next to current
    }

    /**
     * @brief Calculate the number of generating sets.
     * @param tt Total T count.
     * @param sdm Stored depth maximum.
     * @return Number of generating sets.
     */
    static int num_generating_sets(int total_T_count, int max_stored_depth) {
        return std::min(total_T_count - 1 - max_stored_depth, max_stored_depth); 
    }
};
#endif // UTILS_HPP


