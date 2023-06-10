#include <cstdio>
#include <cstdint>
#include <random>

#include "formula.cc"

// Latitude range: [-90 - 90] (degrees)
// Longitude range: [-180 - 180] (degrees)
// Generate Haversine points (x0, y0, x1, y1) and structure it as JSON type, write on a JSON file.

typedef double f64;
typedef uint64_t u64;

struct Point {
	f64 x, y;
};

// To hold two points
struct PointPairs {
	f64 x0, y0, x1, y1;
};

static void write_on_file(PointPairs *points, const u64 *number, const char *fname) {
	FILE *file;
	file = fopen(fname, "w");

	fprintf(file, "{\n\t'points': [\n");
	
	for (u64 i = 0; i < *number; ++i) {
		fprintf(file, "\t\t{'x0': %f, 'y0': %f, 'x1': %f, 'y1': %f}", \
						points[i].x0, points[i].y0, points[i].x1, points[i].y1);

		if (i < *number - 1) 
			fprintf(file, ",\n");
	}
	fprintf(file, "\n\t]\n\n}");
}

static void generate_single_point(const u64 *seed_value, Point *p) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<f64> d(0, 1);

	f64 x, y, z, dis;
	x = d(gen); y = d(gen); z = d(gen);

	dis = sqrt(square(x) + square(y) + square(z));

	if (dis == 0) {
		// TODO (Lilanka): Check if this's ok
		dis = 1; 
	}

	x /= dis; y /= dis; z /= dis;

	// Get latitude/longitude value
	p->x = acos(z);
	p->y = atan(x / y);
}

static void generate_points(const u64 *seed_value, const u64 *number, const char *fname) {
	PointPairs points[*number];

	Point p1, p2;
	for (u64 i = 0; i < *number; ++i) {
		PointPairs p;
		generate_single_point(seed_value, &p1);
		generate_single_point(seed_value, &p2);

		p.x0 = p1.x;
		p.y0 = p1.y;
		p.x1 = p2.x;
		p.y1 = p2.y;

		points[i] = p;
	}

	write_on_file(points, number, fname);
}

int main(int argc, char **args) {

	if (argc > 1) {
		u64 seed_value = atoll(args[1]);
		u64 n_points = atoll(args[2]);
		generate_points(&seed_value, &n_points, args[3]);
	} else {
		fprintf(stderr, "Inputs: [seed] [Number of samples] [Output file name]\n");
	} 
}
