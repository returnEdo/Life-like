#pragma once

#include <vector>

#include "vec3.hpp"


namespace Dirac
{

namespace Automaton
{


enum class Distance
{
	L_INFINITY,	// Moore
	CHEBYSHEV	// Von Neumann
};


struct Rule
{
	std::vector<int>	mBirth;
	std::vector<int>	mSurvive;

	Distance mDistance;

	int mRadius;
	int mStatesCount;
};


struct Simulation
{
	int 	mN;			// grid size
	float 	mProbability;		// probability to be alive
	float	mRadius;

	float 	mTimeStep;

	std::vector<Math::vec3>	mColors;
};


struct LifeState
{	

	~LifeState(void)
	{
		delete[] mPreviousState;
		delete[] mCurrentState;
	}

	int* mCurrentState	{ nullptr };
	int* mPreviousState	{ nullptr };
};


}

}
