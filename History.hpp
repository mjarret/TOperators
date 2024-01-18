#ifndef HISTORY_HPP
#define HISTORY_HPP

#include <vector>
#include <string>

/**
 * @brief Class to manage the history of operations.
 */
class History {
public:
    History() = default;

    void add(unsigned char operation);
    std::string toString() const;

private:
    std::vector<unsigned char> operations;
};

#endif // HISTORY_HPP
