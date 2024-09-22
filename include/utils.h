#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <random>
#include <utility>

float randomBetween(float min, float max);
std::pair<float, float> generateRandomPosition(float sizeA, float sizeB);
std::array<float, 3> cross(const std::array<float, 3>& a, const std::array<float, 3>& b);
float dot(const std::array<float, 3>& a, const std::array<float, 3>& b);
void normalize(std::array<float, 3>& vec);
float degToRad(float degrees);
float radToDeg(float radians);

#endif
