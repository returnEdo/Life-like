#include "Color.hpp"

#include <cmath>

#include "MathUtils.hpp"


namespace Dirac
{

/*
H = tVec.x in [0, 360)
S = tVec.y in [0, 1]
V = tVec.z in [0, 1]
*/
Math::vec3 HSVtoRGB(const Math::vec3& tVec)
{

	float lChroma = tVec.y * tVec.z;
	float lX = lChroma * (1.0f - abs(fmod(tVec.x / 60.0f, 2.0f) - 1.0f));

	float lM = tVec.z - lChroma;

	Math::vec3 lRgb {lM};

	if (tVec.x >= 0.0f and tVec.x < 60.0f)
	{
		lRgb += Math::vec3(lChroma, lX, 0.0f);
	}
	else if (tVec.x >= 60.0f and tVec.x < 120.0f)
	{
		lRgb += Math::vec3(lX, lChroma, 0.0f);
	}
	else if (tVec.x >= 120.0f and tVec.x < 180.0f)
	{
		lRgb += Math::vec3(0.0f, lChroma, lX);
	}
	else if (tVec.x >= 180.0f and tVec.x < 240.0f)
	{
		lRgb += Math::vec3(0.0f, lX, lChroma);
	}
	else if (tVec.x >= 240.0f and tVec.x < 300.0f)
	{
		lRgb += Math::vec3(lX, 0.0f, lChroma);
	}
	else
	{
		lRgb += Math::vec3(lChroma, 0.0f, lX);
	}

	return lRgb;
}


Math::vec3 randomPastel(void)
{
	return HSVtoRGB(Math::vec3(Math::randf(0.0f, 360.0f),
				   Math::randf(0.25f, 0.95f),
				   Math::randf(0.75f, .95f)));
}


}
