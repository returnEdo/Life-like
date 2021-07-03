#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cstring>

#include "ScreenManager.hpp"
#include "Shader.hpp"
#include "Manager.hpp"
#include "Entity.hpp"
#include "GameRenderer.hpp"
#include "RenderingComponents.hpp"
#include "GameComponents.hpp"
#include "Clock.hpp"
#include "MathUtils.hpp"	// randf()
#include "CameraUtils.hpp"	// lookAt()
#include "Color.hpp"


Dirac::Manager gManager;

using namespace Dirac;



int main()
{
	srand(time(nullptr));

	Dirac::ScreenManager screenManager("Life-Like game");

	screenManager.setBackgroundColor(Math::vec3(0.1f));

	gManager.init();

	// get componenents ids
	unsigned int viewID 		= gManager.getComponentID<View>();
	unsigned int ruleID 		= gManager.getComponentID<Automaton::Rule>();
	unsigned int simID 		= gManager.getComponentID<Automaton::Simulation>();
	unsigned int stateID 		= gManager.getComponentID<Automaton::LifeState>();
	
	// set system signature
	Dirac::Signature rendererSignature;
	rendererSignature.set(ruleID, true);
	rendererSignature.set(simID, true);
	rendererSignature.set(stateID, true);

	// create system
	gManager.setSignature<Automaton::GameRenderer>(rendererSignature);
	std::shared_ptr<Automaton::GameRenderer> pGameRenderer = gManager.getSystem<Automaton::GameRenderer>();

	// Initialize buffers
	pGameRenderer -> init();

	// Create entities and add components
	EntityID lGame = gManager.createEntity();

	int lSize = 70;
	gManager.addComponent<Automaton::Rule>(lGame,
					       {
							{4},
							{4},
							Automaton::Distance::L_INFINITY,
							1,
							5
					       });

	// Builder
	gManager.addComponent<Automaton::Rule>(lGame,
					       {
							{4, 6, 8, 9},
							{2, 6, 9},
							Automaton::Distance::L_INFINITY,
							1,
							10
					       });

	Automaton::Rule lRule = gManager.getComponent<Automaton::Rule>(lGame);

	std::vector<Math::vec3> lColors;

	Math::vec3 lColorA {.1f, .1f, .1f};
	Math::vec3 lColorB {1.0f, 1.0f, 1.0f};
	
	for (int i = 0; i < lRule.mStatesCount; i++)
	{
		lColors.push_back(randomPastel());
	}


	gManager.addComponent<Automaton::Simulation>(lGame,
						     {
							lSize,
							.32f,
							10.0f,
							.07f,
							lColors
						     });
	

	gManager.addComponent<Automaton::LifeState>(lGame, {});
	
	pGameRenderer -> setInitialCondition();

	// Camera
	EntityID cameraID = gManager.createEntity();
	gManager.addComponent<Transform>(cameraID,
					 {	
						Math::vec3(0.0f, .0f, 5.0f),
						Math::Rotor(.0f, Math::vec3(.0f, 0.0f, 1.0f)),
						Math::mat3()
					 });
	gManager.addComponent<PerspectiveView>(cameraID, {});

	pGameRenderer -> setCameraUniforms(cameraID);
	
	// Get compoenent references to play with
	Transform& camera	= gManager.getComponent<Transform>(cameraID);
	PerspectiveView& view	= gManager.getComponent<PerspectiveView>(cameraID);

	Physics::Clock clock;
	clock.refreshTimer();

	float dx = 0.03f;

	float lTheta = M_PI / 4.0f;
	float lPhi = M_PI / 7.0f;
	float lR = 180.0f;

	while (screenManager.shouldRun())
	{

		screenManager.clear();
		pGameRenderer -> update(cameraID, clock.refreshTimer());
		screenManager.update();

		if (screenManager.isPressedOrHeld(GLFW_KEY_RIGHT))
		{
			lTheta += dx;
		}
		if (screenManager.isPressedOrHeld(GLFW_KEY_LEFT))
		{
			lTheta -= dx;
		}
		if (screenManager.isPressedOrHeld(GLFW_KEY_UP))
		{
			lPhi += dx;
		}
		if (screenManager.isPressedOrHeld(GLFW_KEY_DOWN))
		{
			lPhi -= dx;
		}
		if (screenManager.isPressedOrHeld(GLFW_KEY_ESCAPE))
		{
			screenManager.shouldDie();
		}
		if (screenManager.isPressedOrHeld(GLFW_KEY_KP_ADD))
		{
			lR += 1.0f;
		}
		if (screenManager.isPressedOrHeld(GLFW_KEY_KP_SUBTRACT))
		{
			lR -= 1.0f;
		}

		lPhi = Math::clamp(lPhi, -M_PI/2.0f + dx, M_PI/2.0f - dx);
		lTheta = fmod(lTheta, 2.0f * M_PI);

		camera.mPosition.x = lR * cos(lPhi) * sin(lTheta);
		camera.mPosition.y = lR * sin(lPhi);
		camera.mPosition.z = lR * cos(lPhi) * cos(lTheta);

		lookAt(cameraID, Math::vec3());
	}


	return 0;
}

