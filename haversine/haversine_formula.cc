// Script to calculate haversine distance

#include <cmath>

typedef double f64;

const f64 Pi = 3.14159265358979;

f64 Square(f64 x) {
	return x * x;
}

f64 ConvertToRadians(f64 x) {
	return x * (Pi / 180.f);
}

// y: lattitude, x: longitude and these parameters are in degrees
f64 CalculateHaversineDistance(f64 x0, f64 y0, f64 x1, f64 y1, f64 earth_radius) {
	f64 y0_rad = ConvertToRadians(y0);
	f64 y1_rad = ConvertToRadians(y1);
	f64 dy = ConvertToRadians(y1 - y0);
	f64 dx = ConvertToRadians(x1 - x0);

	f64 v = Square(std::sin(dy / 2)) + std::cos(y0_rad) * std::cos(y1_rad) * Square(std::sin(dx / 2));
	f64 d = 2.f * earth_radius * std::asin(std::sqrt(v));
	return d;
}
