#pragma once
#include <Core/GameState.h>
#include <NonCopy.h>
#include <NonMovable.h>
#include <Utilities/Timer.h>
#include <GFX/2D/Sprite.h>

using namespace Stardust;

class SplashState : public Core::GameState, NonCopyable, NonMovable {
public:
	SplashState();

	void init();
	void cleanup();

	void enter();
	void pause();
	void resume();

	void update(Core::GameStateManager* st);
	void draw(Core::GameStateManager* st);

private:
	GFX::Render2D::Sprite* sprt;
	unsigned int tex;
};