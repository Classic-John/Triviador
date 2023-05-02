#pragma once

#include <stdint.h>
#include <random>

class RandomGenerator
{
public:
	RandomGenerator();

	// generate a uniform random integer in range [left, right]
	uint64_t generate(uint64_t left, uint64_t right);

	std::mt19937& engine();
private:
	std::random_device m_device;
	std::mt19937 m_generator;
	std::uniform_int_distribution<> m_distribution;
};

