//
// Created by nsamson on 12/20/15.
//

#ifndef OPENAIR_WORLD_HPP
#define OPENAIR_WORLD_HPP

#include <string>

#include "../libopenair/sqlite3pp.hpp"
#include "../libopenair/common.hpp"

namespace openair {

    class QuarteredYear {
        uint32_t year;
        uint8_t quarter;

    public:
        QuarteredYear();
        QuarteredYear(uint32_t year, uint8_t quarter);

        std::string to_string() const;

        bool before(const QuarteredYear & that) const;
        bool after(const QuarteredYear & that) const;

        bool operator==(const QuarteredYear & that) const;
        bool operator!=(const QuarteredYear & that) const;
        bool operator>(const QuarteredYear & that) const;
        bool operator<(const QuarteredYear & that) const;
        bool operator>=(const QuarteredYear & that) const;
        bool operator<=(const QuarteredYear & that) const;

        QuarteredYear operator+(uint32_t quarters) const;
        QuarteredYear operator-(uint32_t quarters) const;
        QuarteredYear& operator++();
        QuarteredYear operator++(int);
        QuarteredYear& operator--();
        QuarteredYear operator--(int);

        static QuarteredYear from_string(std::string input);

    };

    class World {
        QuarteredYear time;
    };

}

#endif //OPENAIR_WORLD_HPP
