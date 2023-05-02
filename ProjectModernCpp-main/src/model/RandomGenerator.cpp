#include "RandomGenerator.h"

RandomGenerator::RandomGenerator()
	: m_device(), m_generator(m_device())
{
}

uint64_t RandomGenerator::generate(uint64_t left, uint64_t right)
{
	std::uniform_int_distribution<uint64_t> m_distribution(left, right);
	return m_distribution(m_generator);
}

std::mt19937& RandomGenerator::engine()
{
	return m_generator;
}
