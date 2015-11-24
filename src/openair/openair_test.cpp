/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <vector>
#include <iterator>
#include <algorithm>
#include "libopenair/common.hpp"
#include "libopenair/parallel.hpp"

uint64_t num_factors(uint64_t inp) {
    uint64_t out = 0;

    if (inp <= 2) { return 1;}

    while (inp > 1) {
        if (inp == 2) {out++; inp = 1;}
        for (uint64_t i = 2; i <= inp; i++) {
            if (inp % i == 0) {
                inp /= i;
                out++;
                i = inp;
            }
        }
    }

    return out;
}

uint64_t plus_one(uint64_t x){
    return x + 1;
}

int main(int argc, const char **argv) {
    std::vector<uint64_t> test_vec;
    for (uint64_t i = 0; i < 100000; i++) {
        test_vec.push_back(i);
    }

    std::vector<uint64_t> out(test_vec.size());

//    std::vector<uint64_t> final();

    openair::parallel_apply_unordered(
            num_factors,
            test_vec.begin(),
            test_vec.end(),
            out.begin(),
            8
    );

//    std::transform(test_vec.begin(), test_vec.end(), out.begin(), num_factors);

    return 0;
}
