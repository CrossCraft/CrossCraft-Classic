#pragma once
#include <Core/GameState.h>
#include <NonCopy.h>
#include <NonMovable.h>
#include <Utilities/Timer.h>
#include <GFX/2D/Sprite.h>
#include <GFX/UI/TextRenderer.h>
#include <Audio/AudioClip.h>
#include "../Player.h"
#include "../World.h"

using namespace Stardust;

class MainState : public Core::GameState, NonCopyable, NonMovable {
public:
	MainState();

	void init();
	void cleanup();

	void enter();
	void pause();
	void resume();

	void update(Core::GameStateManager* st);
	void draw(Core::GameStateManager* st);

private:

	void uiPass();
	void mainPass();

	GFX::UI::TextRenderer* textRenderer;
	Player* player;
	World* world;
};