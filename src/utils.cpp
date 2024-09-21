#include <random>
using namespace std;

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