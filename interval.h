#ifndef INTERVAL
#define INTERVAL

#include <cmath>
#include <limits>

class Interval {
public:
	Interval();
	Interval(double _min, double _max);
	Interval(const Interval& a, const Interval& b);

	double size() const;
	Interval expand(double delta) const;
	bool contains(double val) const;
	bool surrounds(double val) const;
	double clamp(double val) const;

public:
	double min, max;
	static const Interval empty, universe;
};

const Interval Interval::empty = Interval(std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
const Interval Interval::universe = Interval(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());

Interval operator+(const Interval& intervalValue, double displacement) {
	return Interval(intervalValue.min + displacement, intervalValue.max + displacement);
}

Interval operator+(double displacement, const Interval& intervalValue) {
	return intervalValue + displacement;
}

Interval::Interval() {
	min = std::numeric_limits<double>::infinity();
	max = -std::numeric_limits<double>::infinity();
}

Interval::Interval(double _min, double _max) {
	min = _min;
	max = _max;
}

Interval::Interval(const Interval& a, const Interval& b) {
	min = std::fmin(a.min, b.min);
	max = std::fmax(a.max, b.max);
}

double Interval::size() const {
	return max - min;
}

Interval Interval::expand(double delta) const {
	double padding = delta / 2.0;
	return Interval(min - padding, max + padding);
}

bool Interval::contains(double val) const {
	return min <= val && val <= max;
}

bool Interval::surrounds(double val) const {
	return min < val && val <= max;
}

double Interval::clamp(double val) const {
	if (val < min) {
		return min;
	}
	if (val > max) {
		return max;
	}
	return val;
}

#endif