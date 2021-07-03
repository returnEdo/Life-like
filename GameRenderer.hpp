#pragma once

#include <memory>

#include "Entity.hpp"
#include "System.hpp"
#include "vec3.hpp"
#include "mat3.hpp"
#include "Rotor.hpp"
#include "Memory/Unique.hpp"

#include "Shader.hpp"
#include "Buffer.hpp"
#include "VertexArray.hpp"


namespace Dirac
{

/*
SIGNATURE:
-Rule
-Simulation
-State
*/

namespace Automaton
{

class GameRenderer:  public ISystem
{
	private:
	
	Memory::Unique<Graphics::Shader> 	mShader;
	Memory::Unique<Graphics::Buffer> 	mVertexBuffer;
	Memory::Unique<Graphics::VertexArray>	mVertexArray;

	float mTimer { 0.0f };

	int getIndex(int i, int j, int k, int tN) const;
	int getSum(int i, int j, int k, int tN, int tRadius, int tStatesCount, int* tPreviousState) const;

	bool isInternal(int i, int j, int k, int tSize) const;
	bool toUpdate(float tDeltaTime, float tTimeStep);

	public:
	
	void init(void);
	void update(EntityID tCameraID, float tDeltaTime); 

	void setInitialCondition(void) const;
	void setCameraUniforms(EntityID tCameraID);
};

}
}
