#include "GameRenderer.hpp"

#include <cstring>
#include <cmath>

#include "Manager.hpp"
#include "RenderingComponents.hpp"
#include "GameComponents.hpp"
#include "DiracMacros.hpp"
#include "HeapSort.hpp"
#include "MathUtils.hpp"


extern Dirac::Manager gManager;



namespace Dirac
{
namespace Automaton
{


namespace Meshes
{

	float lCubeMesh[] = 
	{
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
						 
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
						 
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
						 
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
						 
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
						 
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
	};


	float lSquareMesh[] = 
	{
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.5f, 0.5f,
		-0.5f, -0.5f,
		0.5f, 0.5f,
		-0.5f, 0.5f
	};

};




void GameRenderer::init(void)
{	
	mShader.allocate(new Graphics::Shader("resources/shaders/squareVertex.shader",
			     	    	      "resources/shaders/voxelFragment.shader"));

	mVertexArray.allocate(new Graphics::VertexArray());
	mVertexBuffer.allocate(new Graphics::Buffer(GL_ARRAY_BUFFER));
	
	mVertexArray 	-> bind();
	mVertexBuffer 	-> bind();

	mVertexBuffer	-> load<float>(Meshes::lSquareMesh, sizeof(Meshes::lSquareMesh));

	mVertexArray 	-> addAttribute({2, GL_FLOAT, 2 * sizeof(float), 0});

	mVertexBuffer 	-> bind(false);
	mVertexArray 	-> bind(false);
}


void GameRenderer::setInitialCondition(void) const
{
	Simulation& lSim 	= gManager.getComponent<Simulation>(*mEntities.begin());
	LifeState& lState 	= gManager.getComponent<LifeState>(*mEntities.begin());
	Rule& lRule 		= gManager.getComponent<Rule>(*mEntities.begin());

	int lN = lSim.mN * lSim.mN * lSim.mN;

	lState.mPreviousState = new int[lN];
	lState.mCurrentState  = new int[lN];

	memset(lState.mCurrentState, 0, sizeof(int) * lN);

	float lOffset = static_cast<float>(lSim.mN) / 2.0f;

	for (int k = 0; k < lSim.mN; k++)
	{
		for (int j = 0; j < lSim.mN; j++)
		{
			for (int i = 0; i < lSim.mN; i++)
			{
				float li = static_cast<float>(i) - lOffset;
				float lj = static_cast<float>(j) - lOffset;
				float lk = static_cast<float>(k) - lOffset;

				if (fabs(li) - lSim.mRadius < 0.0f and
				    fabs(lj) - lSim.mRadius < 0.0f and
				    fabs(lk) - lSim.mRadius < 0.0f)
				{
					lState.mPreviousState[getIndex(i, j, k, lSim.mN)] = (Math::randf(0.0f, 1.0f) <= lSim.mProbability? (lRule.mStatesCount - 1): 0);
				}
				else
				{
					lState.mPreviousState[getIndex(i, j, k, lSim.mN)] = 0;
				}
			}
		}
	}

}


void GameRenderer::setCameraUniforms(EntityID tCameraID)
{
	PerspectiveView lCameraView = gManager.getComponent<PerspectiveView>(tCameraID);

	mShader -> bind();

	mShader -> setUniform("uCameraTan", tan(lCameraView.mPhi / 2.0f));
	mShader -> setUniform("uCameraFar", lCameraView.mFarPlane);
	mShader -> setUniform("uCameraNear", lCameraView.mNearPlane);
	mShader -> setUniform("uCameraAspectRatio", lCameraView.mAspectRatio);
}



void GameRenderer::update(EntityID tCameraID, float tDeltaTime)
{
	mShader 	-> bind();
	mVertexArray	-> bind();
	mVertexBuffer	-> bind();

	auto lCameraTransform	= gManager.getComponent<Transform>(tCameraID);

	Math::mat3 lCameraMatrix = transpose(lCameraTransform.mRotor.getMatrixForm());

	// camera transform uniforms
	mShader -> setUniform("uCameraPosition", lCameraTransform.mPosition);
	mShader -> setUniform("uCameraAttitude", lCameraMatrix);

	// camera view uniforms

	DIRAC_ASSERT(mEntities.size(), "There is no game available to be rendered!!");

	Rule& lRule 		= gManager.getComponent<Automaton::Rule>(*mEntities.begin());	
	Simulation& lSim 	= gManager.getComponent<Automaton::Simulation>(*mEntities.begin());
	LifeState& lState	= gManager.getComponent<Automaton::LifeState>(*mEntities.begin());

	bool lUpdateState = toUpdate(tDeltaTime, lSim.mTimeStep);	
	//lUpdateState = true;
	float lOffset = static_cast<float>(lSim.mN) / 2.0f;
	int lSum;

	for (int k = 0; k < lSim.mN; k++)
	{
		for (int j = 0; j < lSim.mN; j++)
		{
			for (int i = 0; i < lSim.mN; i++)
			{
				int lIndex = getIndex(i, j, k, lSim.mN);
				int lCellState = lState.mPreviousState[lIndex];

				lSum = getSum(i, j, k, lSim.mN, lRule.mRadius, lRule.mStatesCount, lState.mPreviousState);

				// If current state is alive or dying
				if (lCellState != 0)
				{
					if (lSum != 26 or not isInternal(i, j, k, lSim.mN))
					{
						Math::vec3 lPosition = Math::vec3(static_cast<float>(i) - lOffset,
									          static_cast<float>(j) - lOffset,
									          static_cast<float>(k) - lOffset);
						Math::vec3 lColor = lSim.mColors[lCellState - 1];
						Math::vec3 lColorA = .3f * lColor;
						Math::vec3 lColorB = lColor;
						lColor = Math::vLerp(lColorB, lColorA, static_cast<float>(lSum) / 26.0f);

						mShader -> setUniform("uVoxelPosition", lPosition);
						mShader -> setUniform("uVoxelColor", lColor); 

						glDrawArrays(GL_TRIANGLES, 0, sizeof(Meshes::lSquareMesh) / (2 * sizeof(float)));
					}

					if (lUpdateState)					{	

						// If the cell is alive check survival
						if (lCellState == lRule.mStatesCount - 1)
						{
							lState.mCurrentState[lIndex] = lRule.mStatesCount - ((Sort::binarySearch(lRule.mSurvive, lSum) != -1)? 1: 2);
						}
						// If the cell is dying decrease the state
						else
						{
							lState.mCurrentState[lIndex] = lCellState - 1;
						}
					}
				}
				// If the current cell state is dead check for birth
				else
				{
					if (lUpdateState) { lState.mCurrentState[lIndex] = ((Sort::binarySearch(lRule.mBirth, lSum) != -1)? lRule.mStatesCount - 1: 0); }
				}
			}
		}
	}
	if (lUpdateState) { memcpy(lState.mPreviousState, lState.mCurrentState, sizeof(int) * lSim.mN * lSim.mN * lSim.mN); }
}


// Returns the appropriate index in the state vector
int GameRenderer::getIndex(int i, int j, int k, int tN) const
{
	return ((i % tN) + (j % tN) * tN + (k % tN) * tN * tN);
}

// Number of active neughbours: TODO: Von Neumann neighbourhood
int GameRenderer::getSum(int i, int j, int k, int tN, int tRadius, int tStatesCount, int* tPreviousState) const
{
	int lSum = 0;

	for (int li = i - tRadius; li <= i + tRadius; li++)
	{
		for (int lj = j - tRadius; lj <= j + tRadius; lj++)
		{
			for (int lk = k - tRadius; lk <= k + tRadius; lk++)
			{
				lSum += ((tPreviousState[getIndex(li, lj, lk, tN)] == (tStatesCount - 1))? 1: 0);		
			}
		}
	}
	lSum -= ((tPreviousState[getIndex(i, j, k, tN)] == (tStatesCount - 1))? 1: 0);

	return lSum;
}


bool GameRenderer::isInternal(int i, int j, int k, int tSize) const
{
	return ((i and i != tSize - 1) and
		(j and j != tSize - 1) and
		(k and k != tSize - 1));
}

bool GameRenderer::toUpdate(float tDeltaTime, float tTimeStep)
{
	mTimer += tDeltaTime;
	
	if (mTimer >= tTimeStep)
	{
		mTimer = 0.0f;

		return true;
	}
	return false;
}

}
}
