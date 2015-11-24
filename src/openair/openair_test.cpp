/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <vector>
#include "libopenair/parallel.hpp"

int main(int argc, const char **argv) {
    std::vector<int> test_vec = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    std::vector<int> out(test_vec.size());

    openair::parallel_apply_unordered([](int x) -> int { return x + 1; },
                             test_vec.begin(),
                             test_vec.end(),
                             out.begin()
    );

    return 0;
}
