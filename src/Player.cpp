#include "Player.h"
#include <Utilities/Input.h>

template<typename T>
constexpr auto DEGTORAD(T x) { return x / 180.0f * 3.14159; }

Player::Player() {
	pos = { 64.f, 80.f, 64.f };
	rot = { 0.f, 0.f };
	vel = { 0.0, 0.0, 0.0 };

	cam = new GFX::Render3D::Camera(pos, glm::vec3(rot.x, rot.y, 0), 70.0f, 16.0f / 9.0f, 0.2f, 255.0f);
	onGround = false;
}

Player::~Player() {

}



void Player::update(double dt, World* wrld) {

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
		vel.x += -sinf(static_cast<float>(DEGTORAD(-rot.y))) * playerSpeed * static_cast<float>(dt);
		vel.z += -cosf(static_cast<float>(DEGTORAD(-rot.y))) * playerSpeed * static_cast<float>(dt);
	}

	if (Utilities::KeyHold(GLFW_KEY_S) || Utilities::KeyPressed(GLFW_KEY_S) || Utilities::KeyHold(PSP_CTRL_CROSS) || Utilities::KeyPressed(PSP_CTRL_CROSS)) {
		vel.x += sinf(static_cast<float>(DEGTORAD(-rot.y))) * playerSpeed * static_cast<float>(dt);
		vel.z += cosf(static_cast<float>(DEGTORAD(-rot.y))) * playerSpeed * static_cast<float>(dt);
	}

	if (Utilities::KeyHold(GLFW_KEY_A) || Utilities::KeyPressed(GLFW_KEY_A) || Utilities::KeyHold(PSP_CTRL_SQUARE) || Utilities::KeyPressed(PSP_CTRL_SQUARE)) {
		vel.x += sinf(static_cast<float>(DEGTORAD(-rot.y + 270))) * playerSpeed * static_cast<float>(dt) * 0.7f;
		vel.z += cosf(static_cast<float>(DEGTORAD(-rot.y + 270))) * playerSpeed * static_cast<float>(dt) * 0.7f;
	}

	if (Utilities::KeyHold(GLFW_KEY_D) || Utilities::KeyPressed(GLFW_KEY_D) || Utilities::KeyHold(PSP_CTRL_CIRCLE) || Utilities::KeyPressed(PSP_CTRL_CIRCLE)) {
		vel.x += -sinf(static_cast<float>(DEGTORAD(-rot.y + 270))) * playerSpeed * static_cast<float>(dt) * 0.7f;
		vel.z += -cosf(static_cast<float>(DEGTORAD(-rot.y + 270))) * playerSpeed * static_cast<float>(dt) * 0.7f;
	}

	glm::vec3 testPosGrav = pos + vel;
	unsigned int bx = static_cast<int>(testPosGrav.x);
	unsigned int by = static_cast<int>(testPosGrav.y);
	unsigned int byN = static_cast<int>(testPosGrav.y-1.625F);
	if (by < 0) {
		by = 0;
	}

	if (by > 127) {
		by = 127;
	}
	unsigned int bz = static_cast<int>(testPosGrav.z);

	int idx = ((by * 128) + bz) * 128 + bx;
	int blInside = wrld->worldData[idx];
	idx = ((byN * 128) + bz) * 128 + bx;
	int blYInside = wrld->worldData[idx];


	const float jumpSpeed = 6.5f;
	if (Utilities::KeyPressed(GLFW_KEY_SPACE) || Utilities::KeyHold(GLFW_KEY_SPACE) || Utilities::KeyPressed(PSP_CTRL_SELECT) || Utilities::KeyHold(PSP_CTRL_SELECT)) {

		if (onGround) {
			if (blInside == 7 && blYInside == 7) {
				vel.y = jumpSpeed * 0.5f * 1 / 60.0f;
			}
			else {
				vel.y = jumpSpeed * 1 / 60.0f;
			}
		}
	}


	unsigned int gx = static_cast<int>(testPosGrav.x);
	unsigned int gy = static_cast<int>(testPosGrav.y - 1.625f);
	if (gy < 0) {
		gy = 0;
	}

	if (gy > 127) {
		gy = 127;
	}
	unsigned int gz = static_cast<int>(testPosGrav.z);

	idx = ((gy * 128) + gz) * 128 + gx;

	int blkCheck = wrld->worldData[idx];

	if (blkCheck != 0 && blkCheck != 7) {
		//Collide
		vel.y = 0;
		onGround = true;
	}
	else {
		if (blkCheck != 7) {
			onGround = false;
		}
	}

	//Test physics
	glm::vec3 testPos = pos + glm::vec3(vel.x, 0, vel.z);

	int xx = testPos.x + 0.05f;
	int xnx = testPos.x - 0.05f;
	int xy = testPos.y - 1.625f;
	int xz = testPos.z;

	idx = ((xy * 128) + xz) * 128 + xx;
	int xpCheck = wrld->worldData[idx];
	idx = ((xy * 128) + xz) * 128 + xnx;
	int xnpCheck = wrld->worldData[idx];

	if (xpCheck != 0 && xpCheck != 7) {
		vel.x = 0;
	}
	if (xnpCheck != 0 && xnpCheck != 7) {
		vel.x = 0;
	}

	int zx = testPos.x;
	int zy = testPos.y - 1.625f;
	int zz = testPos.z + 0.05f;
	int znz = testPos.z - 0.05f;

	idx = ((zy * 128) + zz) * 128 + zx;
	int zpCheck = wrld->worldData[idx];
	idx = ((zy * 128) + znz) * 128 + zx;
	int znpCheck = wrld->worldData[idx];

	if (zpCheck != 0 && zpCheck != 7) {
		vel.z = 0;
	}
	if (znpCheck != 0 && znpCheck != 7) {
		vel.z = 0;;
	}

	pos += vel;
	vel.x = 0;

	if (!onGround) {
		vel.y -= 18 * dt * dt;
	}

	if (vel.y < -78.4) {
		vel.y = -78.4;
	}

	vel.z = 0;

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
