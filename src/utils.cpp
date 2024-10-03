#include <random>
#include <array>

using namespace std;

#ifdef _WIN32
#define M_PI       3.14159265358979323846f
#endif

float randomBetween(float min, float max) {
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(min, max);
	return dis(gen);
}

pair<float, float> generateRandomPosition(float sizeA, float sizeB, float objectRadius, bool insideLimit) {
	float x, y;
	while (true) {
		x = randomBetween(-(sizeA / 2 - objectRadius), (sizeA / 2 - objectRadius));
		y = randomBetween(-(sizeA / 2 - objectRadius), (sizeA / 2 - objectRadius));

		bool positionedInsideLimit = !(x >= -(sizeB / 2 + objectRadius) && x <= (sizeB / 2 + objectRadius)
			&& y >= -(sizeB / 2 + objectRadius) && y <= (sizeB / 2 + objectRadius));

		if (insideLimit) { // supposed to be in water
			if (!positionedInsideLimit) {
				break;
			}
		}
		else { // supposed to be in land
			if (positionedInsideLimit) {
				break;
			}
		}
		//if (!waterPosition && positionedInWater) {
		//	break;
		//}
	}
	return make_pair(x, y);
}

float randomFloat(float min, float max) {
	return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

std::array<float, 3> cross(const std::array<float, 3>& a, const std::array<float, 3>& b) {
	return {
		a[1] * b[2] - a[2] * b[1],
		a[2] * b[0] - a[0] * b[2],
		a[0] * b[1] - a[1] * b[0]
	};
}

float dot(const std::array<float, 3>& a, const std::array<float, 3>& b) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void normalize(std::array<float, 3>& vec) {
	float length = std::sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	if (length > 0.0f) {
		for (int i = 0; i < 3; ++i) {
			vec[i] /= length;
		}
	}
}

float degToRad(float degrees)
{
	return (float)(degrees * (M_PI / 180.0f));
};

float radToDeg(float radians)
{
	return (float)(radians * (180.0f / M_PI));
};

std::array<float, 3> lerpPosition(const std::array<float, 3>& start, const std::array<float, 3>& end, float t) {
	return {
		start[0] + t * (end[0] - start[0]),
		start[1] + t * (end[1] - start[1]),
		start[2] + t * (end[2] - start[2])
	};
};

bool isApproximatelyMultipleOf(float number, int multipleOf, float tolerance) {
	float remainder = std::fmod(std::abs(number), multipleOf);
	return remainder <= tolerance || remainder >= (multipleOf - tolerance);
}

float length(const std::array<float, 3>& vec) {
	return std::sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
}

std::array<float, 3> subtract(const std::array<float, 3>& a, const std::array<float, 3>& b) {
	return { a[0] - b[0], a[1] - b[1], a[2] - b[2] };
}