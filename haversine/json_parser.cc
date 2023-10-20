#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <sys/stat.h>

typedef double f64;
typedef uint8_t u8;

struct Buffer {
	size_t count;
	u8 *data;
};

static Buffer AllocateBuffer(size_t count) {
	Buffer result = {};
	result.data = (u8 *)malloc(count);
	if (result.data) {
		result.count = count;
	} else {
		fprintf(stderr, "Unable to allocate");
	}
	return result;
}

static void FreeBuffer(Buffer *buffer) {
	if (buffer->data) {
		free(buffer->data);
	}
	*buffer = {};
}

static Buffer ReadJSONFile(const char *fname) {
	Buffer result = {};
	FILE *file = fopen(fname, "r");

	if (file) {
		struct stat s;
		stat(fname, &s);

		result = AllocateBuffer(s.st_size);
		if (result.data) {
				if (fread(result.data, result.count, 1, file) != 1) {
				fprintf(stderr, "Unable to read the file");
				FreeBuffer(&result);
				}
		}
	} else {
		fprintf(stderr, "Unable to open the file");
	}
	return result;
} 

static void ParseJSON(const char *fname) {
	Buffer input_data = ReadJSONFile(fname);
	printf("%s\n", input_data.data);
}

int main(int argc, char **args) {
	const char *fname = "haversine_points.json";
	ParseJSON(fname);
	/* NOTE (lilanka): This is working just ignore it while building
	if (argc > 1) {
		ParseJSON(args[1]);	
	}	else {
		fprintf(stderr, "Input the JSON filename");
	}
	*/
}
