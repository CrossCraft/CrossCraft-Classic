#include "Player.h"
#include <Utilities/Input.h>

template<typename T>
constexpr auto DEGTORAD(T x) { return x / 180.0f * 3.14159; }

Player::Player() {
	pos = { 64.f, 80.f, 64.f };
	rot = { 0.f, 0.f };
	vel = { 0.0, 0.0, 0.0 };

	cam = new GFX::Render3D::Camera(pos, glm::vec3(rot.x, rot.y, 0), 70.0f, 16.0f / 9.0f, 0.2f, 255.0f);
}

Player::~Player() {

}

void Player::update(double dt) {

	float rotSpeed = 50.0f;

#if CURRENT_PLATFORM == PLATFORM_PSP
	auto aX = Utilities::KeyStrength(PSP_CTRL_ANALOG_X);
	auto aY = Utilities::KeyStrength(PSP_CTRL_ANALOG_Y);

	if (aX > 0.4f || aX < -0.4f) {
		rot.y += aX * rotSpeed * dt;
	}

	if (aY > 0.4f || aY < -0.4f) {
		rot.x -= aY * rotSpeed * dt;
	}
#else
	if (Utilities::KeyHold(GLFW_KEY_UP) || Utilities::KeyPressed(GLFW_KEY_UP)) {
		rot.x -= rotSpeed * static_cast<float>(dt);
	}

	if (Utilities::KeyHold(GLFW_KEY_DOWN) || Utilities::KeyPressed(GLFW_KEY_DOWN)) {
		rot.x += rotSpeed * static_cast<float>(dt);
	}

	if (Utilities::KeyHold(GLFW_KEY_LEFT) || Utilities::KeyPressed(GLFW_KEY_LEFT)) {
		rot.y -= rotSpeed * static_cast<float>(dt);
	}

	if (Utilities::KeyHold(GLFW_KEY_RIGHT) || Utilities::KeyPressed(GLFW_KEY_RIGHT)) {
		rot.y += rotSpeed * static_cast<float>(dt);
	}
#endif

	if (rot.y > 360.0f) {
		rot.y -= 360.0f;
	}

	if (rot.y < 0.0f) {
		rot.y += 360.0f;
	}

	if (rot.x < -89.9f) {
		rot.x = -89.9f;
	}

	if (rot.x > 89.9f) {
		rot.x = 89.9f;
	}

	float playerSpeed = 4.3f;

	if (Utilities::KeyHold(GLFW_KEY_W) || Utilities::KeyPressed(GLFW_KEY_W) || Utilities::KeyHold(PSP_CTRL_TRIANGLE) || Utilities::KeyPressed(PSP_CTRL_TRIANGLE)) {
		vel.x += -sinf(static_cast<float>(DEGTORAD(-rot.y))) * playerSpeed * static_cast<float>(dt) / 10.0f;
		vel.z += -cosf(static_cast<float>(DEGTORAD(-rot.y))) * playerSpeed * static_cast<float>(dt) / 10.0f;
	}

	if (Utilities::KeyHold(GLFW_KEY_S) || Utilities::KeyPressed(GLFW_KEY_S) || Utilities::KeyHold(PSP_CTRL_CROSS) || Utilities::KeyPressed(PSP_CTRL_CROSS)) {
		vel.x -= -sinf(static_cast<float>(DEGTORAD(-rot.y))) * playerSpeed * static_cast<float>(dt) / 10.0f;
		vel.z -= -cosf(static_cast<float>(DEGTORAD(-rot.y))) * playerSpeed * static_cast<float>(dt) / 10.0f;
	}

	if (Utilities::KeyHold(GLFW_KEY_A) || Utilities::KeyPressed(GLFW_KEY_A) || Utilities::KeyHold(PSP_CTRL_SQUARE) || Utilities::KeyPressed(PSP_CTRL_SQUARE)) {
		vel.x += sinf(static_cast<float>(DEGTORAD(-rot.y + 270))) * playerSpeed * static_cast<float>(dt) / 10.0f * 0.7f;
		vel.z += cosf(static_cast<float>(DEGTORAD(-rot.y + 270))) * playerSpeed * static_cast<float>(dt) / 10.0f * 0.7f;
	}

	if (Utilities::KeyHold(GLFW_KEY_D) || Utilities::KeyPressed(GLFW_KEY_D) || Utilities::KeyHold(PSP_CTRL_CIRCLE) || Utilities::KeyPressed(PSP_CTRL_CIRCLE)) {
		vel.x -= sinf(static_cast<float>(DEGTORAD(-rot.y + 270))) * playerSpeed * static_cast<float>(dt) / 10.0f * 0.7f;
		vel.z -= cosf(static_cast<float>(DEGTORAD(-rot.y + 270))) * playerSpeed * static_cast<float>(dt) / 10.0f * 0.7f;
	}

	pos += vel;

	if (pos.x > 128) {
		pos.x = 128;
		vel.x = 0;
	}

	if (pos.y > 128) {
		pos.y = 128;
		vel.y = 0;
	}

	if (pos.z > 128) {
		pos.z = 128;
		vel.z = 0;
	}

	if (pos.x < 0) {
		pos.x = 0;
		vel.x = 0;
	}

	if (pos.y < 0) {
		pos.y = 0;
		vel.y = 0;
	}

	if (pos.z < 0) {
		pos.z = 0;
		vel.z = 0;
	}

	vel *= 0.9f;


	if (Utilities::KeyHold(GLFW_KEY_SPACE) || Utilities::KeyPressed(GLFW_KEY_SPACE) || Utilities::KeyHold(PSP_CTRL_SELECT) || Utilities::KeyPressed(PSP_CTRL_SELECT)) {
		vel.y += playerSpeed * static_cast<float>(dt) / 10.0f;
	}

	if (Utilities::KeyHold(GLFW_KEY_LEFT_SHIFT) || Utilities::KeyPressed(GLFW_KEY_LEFT_SHIFT) || Utilities::KeyHold(PSP_CTRL_DOWN) || Utilities::KeyPressed(PSP_CTRL_DOWN)) {
		vel.y -= playerSpeed * static_cast<float>(dt) / 10.0f;
	}


	//Camera

#if CURRENT_PLATFORM == PLATFORM_PSP
	cam->pos = -pos;
	cam->rot = glm::vec3(-rot.x, rot.y, 0.f);
#else 

	cam->pos = pos;
	cam->rot = glm::vec3(DEGTORAD(rot.x), DEGTORAD(rot.y), 0.f);
#endif
	cam->update();
}

void Player::draw() {

}
