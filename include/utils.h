#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <random>
#include <utility>
#include "array"

using namespace std;

float randomBetween(float min, float max);
std::pair<float, float> generateRandomPosition(float sizeA, float sizeB, float objectRadius, bool insideLimit);
std::array<float, 3> cross(const std::array<float, 3>& a, const std::array<float, 3>& b);
float dot(const std::array<float, 3>& a, const std::array<float, 3>& b);
void normalize(std::array<float, 3>& vec);
float degToRad(float degrees);
float radToDeg(float radians);
std::array<float, 3> lerpPosition(const std::array<float, 3>& start, const std::array<float, 3>& end, float t);
float randomFloat(float min, float max);
bool isApproximatelyMultipleOf(float number, int multipleOf, float tolerance);
float length(const std::array<float, 3>& vec);
std::array<float, 3> subtract(const std::array<float, 3>& a, const std::array<float, 3>& b);

#endif
