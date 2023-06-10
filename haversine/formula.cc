#include <cmath>

typedef double f64;

#define Earth_Radius 6371.0 // Km 

f64 square(f64 value) {
	return value * value;
}

// x represents latitute values in radians, while y represents logitude values in radians
// This returs distance in Km 
static f64 reference_haversine_formula(f64 x0, f64 y0, f64 x1, f64 y1) {
	f64 result = square(sin((x1 - x0) / 2.0)) + cos(x0) * cos(x1) * square((y1 - y0) / 2.0);
	result = 2.0 * Earth_Radius * asin(sqrt(result));
	return result;
}
