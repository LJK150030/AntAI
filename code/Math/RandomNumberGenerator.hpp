#pragma once

struct Vec2;

class RandomNumberGenerator
{
public:
	explicit RandomNumberGenerator(unsigned int seed = 0);

	int GetRandomIntLessThan(int max_not_inclusive);
	int GetRandomIntInRange(int min_inclusive, int max_inclusive);
	float GetRandomFloatZeroToOne();
	float GetRandomFloatInRange(float min_inclusive, float max_inclusive);
	bool PercentChance(float fraction);

protected:
	unsigned int m_seed = 0;
	int m_position = 0;
};
