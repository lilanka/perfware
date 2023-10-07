// Script to generate haversine inputs, should store it in a JSON file
// 
// Script takes number of points as an input 
// x: [-180, 180]
// y: [-90, 90]
// 
// JSON format:
// {"pairs": [
//		{"x0": 34, "y0": 3, "x1": 4, "y1": 45},
//		{"x0": 3, "y0": 5, "x1": 56, "y1": 41},
//		{"x0": 5, "y0": 1, "x1": 6, "y1": 1}
// ]}	

#include <cstdio>
#include <cstdint>
#include <random>

#include "haversine_formula.cc"

typedef uint64_t u64;

struct Point {
	f64 x, y;
};

struct PointPair {
	Point p1, p2;
};

static void GenerateJSONFile(PointPair *points, const u64 *n_points) {
	FILE *file;
	file = fopen("haversine_points.json", "w");

	fprintf(file, "{\n\t'points': [\n");
	
	for (u64 i = 0; i < *n_points; ++i) {
		fprintf(file, "\t\t{'x0': %f, 'y0': %f, 'x1': %f, 'y1': %f}", \
						points[i].p1.x, points[i].p1.y, points[i].p2.x, points[i].p2.y);

		if (i < *n_points - 1) 
			fprintf(file, ",\n");
	}
	fprintf(file, "\n\t]\n}");
}

static void GenerateSinglePoint(Point *p) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<f64> d(0, 1);

	f64 x = d(gen);
	f64 y = d(gen);
	f64 z = d(gen);
	f64 dis = std::sqrt(Square(x) + Square(y) + Square(z));

	if (dis == 0) {
		dis = 1;
	}

	x /= dis;
	y /= dis;
	y /= dis;

	p->x = std::acos(z);
	p->y = std::atan(x / y);
}

static void GeneratePoints(const u64 *n_points) {
	PointPair point_pairs[*n_points];

	Point p1, p2;
	for (u64 i = 0; i < *n_points; ++i) {
		PointPair p;
		GenerateSinglePoint(&p1);
		GenerateSinglePoint(&p2);

		p.p1 = p1;
		p.p2 = p2;
		point_pairs[i] = p;
	}
	GenerateJSONFile(point_pairs, n_points);
}

int main(int argc, char **args) {
	if (argc > 1) {
		u64 n_points = atoll(args[1]);
		GeneratePoints(&n_points);
	}	else {
		fprintf(stderr, "Inputs: [Number of samples]");
	}		
}
