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

pair<float, float> generateRandomPosition(float sizeA, float sizeB) {
	float x, y;
	while (true) {
		x = randomBetween(-(sizeA / 2 - 1), (sizeA / 2 - 1));
		y = randomBetween(-(sizeA / 2 - 1), (sizeA / 2 - 1));

		if (!(x >= -(sizeB / 2 + 1) && x <= (sizeB / 2 + 1)
			&& y >= -(sizeB / 2 + 1) && y <= (sizeB / 2 + 1))) {
			break;
		}
	}
	return make_pair(x, y);
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