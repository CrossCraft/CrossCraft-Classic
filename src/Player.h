#pragma once
#include <glm/glm.hpp>
#include <GFX/3D/Camera.h>

using namespace Stardust;

class Player {
public:

	Player();
	~Player();

	void update(double dt);
	void draw();

	glm::vec3 pos;
	glm::vec2 rot;
	GFX::Render3D::Camera* cam;

private:
	glm::vec3 vel;
};