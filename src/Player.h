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

using namespace Stardust;

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
	void update(double dt);

	/**
	 * Draws the GUI.
	 */
	void draw();

	glm::vec3 pos;
	glm::vec2 rot;
	GFX::Render3D::Camera* cam;

private:
	glm::vec3 vel;
};