//
// Created by nsamson on 11/26/15.
//

#include <iostream>
#include <stdexcept>
#include <string>
#include <limits>
#include <random>
#include <chrono>
#include <sstream>
#include <cassert>
#include "libopenair/world.hpp"

template <typename RandGen>
std::string generate_random_success_string(RandGen& rgen) {

    std::uniform_int_distribution<uint32_t> uint32_gen(1, std::numeric_limits<uint32_t>::max());
    std::uniform_int_distribution<uint8_t> uint8_gen(1, 4);

    uint32_t year = uint32_gen(rgen);
    uint8_t quarters = uint8_gen(rgen);

    std::ostringstream oss;

    oss << year << "." << (uint32_t) quarters;

    return oss.str();

}

void test_failure() {

    try {
        std::string failure_test("f.4");
        auto result = openair::QuarteredYear::from_string(failure_test);
        std::cerr << result.to_string() << "\n";
    } catch (const std::invalid_argument& ia) {
        std::cout << "Caught exception and was supposed to.\n";
        return;
    }


    throw std::runtime_error("Function did not fail: QuarteredYear::from_string");
}

void test_success(uint32_t seed, uint32_t iterations) {

    std::default_random_engine generator(seed);

    for (uint32_t i = 0; i < iterations; i++) {
        std::string test_str = generate_random_success_string(generator);
        std::cout << "Testing with string " << test_str << "\n";
        auto result = openair::QuarteredYear::from_string(test_str);
        assert(result.to_string() == test_str);
        std::cout << "Got " << result.to_string() << "\n";
    }

    std::cout.flush();

}

void test_addition() {
    openair::QuarteredYear a(2015, 3);
    auto b = openair::QuarteredYear::from_string("2015.1") + 5;

    a++;

    std::cout << a.to_string() << std::endl;
    std::cout << b.to_string() << std::endl;

    assert(a.to_string() == std::string("2016.1"));
    assert(b.to_string() == std::string("2016.2"));
}

void test_ordering() {
    openair::QuarteredYear a(2015, 3);
    openair::QuarteredYear b(2016, 3);
    openair::QuarteredYear c(2015, 2);
    openair::QuarteredYear d(2015, 1);

    assert(d < c && c < a && a < b);
    assert(d < a && c < b);
    assert(d < b);

    assert(b > a && a > c && c > d);
    assert(b > c && a > d);
    assert(b > d);

    c++;

    assert(a == c);
    assert(b != c);

    std::cout << "Ordering is working.\n";

}

int main(int argc, const char** argv) {

    unsigned long long int seed;

    if (argc < 2) {
        seed = (unsigned long long) std::chrono::system_clock::now().time_since_epoch().count();
    } else {
        seed = std::strtoull(argv[1], nullptr, 10);
    }

    test_failure();
    test_success((uint32_t) seed, (uint32_t) 10000);
    test_addition();
    test_ordering();

    std::cout << "TEST SUCCEEDED" << std::endl;

}