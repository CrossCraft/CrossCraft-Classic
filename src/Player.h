/*****************************************************************//**
 * \file   Player.h
 * \brief  The main player
 * 
 * \author Iridescence - Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * \date   December 2020
 *********************************************************************/
#pragma once
#include <glm/glm.hpp>
#include <GFX/3D/Camera.h>
#include "World.h"
#include "AABB.h"
#include "Frustum.h"

using namespace Stardust;

class World;
/**
 * Describes the player.
 */
class Player {
public:

	Player();
	~Player();

	/**
	 * Updates position.
	 * 
	 * \param dt - based on delta time
	 */
	void update(double dt, World* wrld);

	/**
	 * Draws the GUI.
	 */
	void draw();

	glm::vec3 pos;
	glm::vec2 rot;
	GFX::Render3D::Camera* cam;
	bool onGround;

	glm::mat4 proj;
	glm::mat4 view;

	ViewFrustum m_frustum;

private:
	glm::vec3 vel;
};