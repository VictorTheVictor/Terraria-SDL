#ifndef PERLIN_NOISE_H_INCLUDED
#define PERLIN_NOISE_H_INCLUDED

#include <math.h>

static float Fade(float t)
{
	return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

static float Lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

static int Hash(int x)
{
	x = (x << 13) ^ x;
	return (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff;
}

static float Gradient1D(int x, float distance)
{
	int h = Hash(x) & 1;

	if (h == 0)
	{
		return distance;
	}

	return -distance;
}

float Perlin1D(float x)
{
	int x0 = (int)floorf(x);
	int x1 = x0 + 1;

	float dx0 = x - (float)x0;
	float dx1 = x - (float)x1;

	float g0 = Gradient1D(x0, dx0);
	float g1 = Gradient1D(x1, dx1);

	float t = Fade(dx0);

	return Lerp(g0, g1, t);
}

float FractalPerlin1D(float x, int octaves, float persistence)
{
	float total = 0.0f;
	float frequency = 1.0f;
	float amplitude = 1.0f;
	float maxValue = 0.0f;

	for (int i = 0; i < octaves; i++)
	{
		total += Perlin1D(x * frequency) * amplitude;

		maxValue += amplitude;
		amplitude *= persistence;
		frequency *= 2.0f;
	}

	return total / maxValue;
}

#endif // PERLIN_NOISE_H_INCLUDED
