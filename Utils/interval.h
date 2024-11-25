#ifndef INTERVAL_H
#define INTERVAL_H

#include "utility.h"

class Interval {
public:
    double min, max;

    Interval() : min(+Utility::infinity), max(-Utility::infinity) {}
    Interval(double min, double max) : min(min), max(max) {}
    Interval(float min, float max) : min(static_cast<double>(min)), max(static_cast<double>(max)) {}
    Interval(const Interval& a, const Interval& b) {
        min = a.min <= b.min ? a.min : b.min;
        max = a.max >= b.max ? a.max : b.max;
    }

    double size() const {
        return max - min;
    }

    bool contains(double x) const {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const {
        return min < x&& x < max;
    }

    double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    Interval expand(double delta) const {
        auto padding = delta / 2;
        return Interval(min - padding, max + padding);
    }

    static const Interval empty, universe;
};

const Interval Interval::empty = Interval(+Utility::infinity, -Utility::infinity);
const Interval Interval::universe = Interval(-Utility::infinity, +Utility::infinity);

Interval operator+(const Interval& ival, double displacement) {
    return Interval(ival.min + displacement, ival.max + displacement);
}

Interval operator+(double displacement, const Interval& ival) {
    return ival + displacement;
}


#endif