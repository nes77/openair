//
// Created by nsamson on 12/20/15.
//

#include <string>
#include <sstream>
#include <stdexcept>
#include <regex>
#include "../libopenair/world.hpp"

std::regex Q_YEAR_REGEX(R"s((\d+)\.([1-4]))s", std::regex::ECMAScript | std::regex_constants::optimize);

openair::QuarteredYear::QuarteredYear(uint32_t year,
                                      uint8_t quarter)
        : year(year), quarter(quarter) {}

std::string openair::QuarteredYear::to_string() const {
    std::ostringstream oss;

    oss << this->year << "." << (uint32_t) (this->quarter + 1);

    return oss.str();
}

bool openair::QuarteredYear::before(const ::openair::QuarteredYear &that) const {
    if (this->year > that.year) {
        return false;
    } else if (this-> year < that.year) {
        return true;
    } else {
        return this->quarter < that.quarter;
    }
}

bool openair::QuarteredYear::after(const ::openair::QuarteredYear &that) const {
    if (this->year < that.year) {
        return false;
    } else if (this-> year > that.year) {
        return true;
    } else {
        return this->quarter > that.quarter;
    }
}

bool openair::QuarteredYear::operator==(const ::openair::QuarteredYear &that) const {
    return !(this->before(that) || this->after(that));
}

bool openair::QuarteredYear::operator!=(const ::openair::QuarteredYear &that) const {
    return !(*this == that);
}

bool openair::QuarteredYear::operator>(const ::openair::QuarteredYear &that) const {
    return this->after(that);
}

bool openair::QuarteredYear::operator<(const ::openair::QuarteredYear &that) const {
    return this->before(that);
}

bool openair::QuarteredYear::operator>=(const ::openair::QuarteredYear &that) const {
    return (this->after(that) || *this == that);
}

bool openair::QuarteredYear::operator<=(const ::openair::QuarteredYear &that) const {
    return !(*this > that);
}

openair::QuarteredYear openair::QuarteredYear::operator+(uint32_t quarters) const {
    openair::QuarteredYear out;

    quarters += this->quarter;

    out.year = this->year + quarters / 4;
    out.quarter = (uint8_t) (quarters % 4);

    return out;
}

openair::QuarteredYear openair::QuarteredYear::operator-(uint32_t quarters) const {
    openair::QuarteredYear out;

    if ((quarters / 4) + 1 > this->year) {
        throw std::overflow_error(
                std::string("Cannot subtract ")
                        .append(std::to_string(quarters))
                        .append("from ")
                        .append(this->to_string())
        );
    }

    out.year = this->year;
    out.quarter = this->quarter;

    if (quarters > out.quarter) {
        out.year -= 1;
        out.quarter += 4;
    }

    out.year -= quarters / 4;
    out.quarter -= (uint8_t) (quarters % 4);

    return out;
}

openair::QuarteredYear &openair::QuarteredYear::operator++() {
    *this = *this + 1;
    return *this;
}

openair::QuarteredYear &openair::QuarteredYear::operator--() {
    *this = *this - 1;
    return *this;
}

openair::QuarteredYear::QuarteredYear() {
    this->quarter = 0;
    this->year = 0;
}

openair::QuarteredYear openair::QuarteredYear::from_string(std::string input) {

    openair::QuarteredYear out;

    std::smatch match;
    std::regex_match(input, match, Q_YEAR_REGEX);

    if (match.size() < 3) {
        throw std::invalid_argument(input.append(" does not match the regex for a Quartered Year."));
    }

    uint32_t year = (uint32_t) std::atoi(match[1].str().c_str());
    uint8_t q = (uint8_t) (std::atoi(match[2].str().c_str()) - 1);

    return openair::QuarteredYear(year, q);
}

openair::QuarteredYear openair::QuarteredYear::operator++(int) {
    auto out = openair::QuarteredYear(*this);
    this->operator++();
    return out;
}

openair::QuarteredYear openair::QuarteredYear::operator--(int) {
    auto out = openair::QuarteredYear(*this);
    this->operator--();
    return out;
}
