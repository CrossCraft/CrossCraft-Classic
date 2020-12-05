/*****************************************************************//**
 * \file   MainState.h
 * \brief  The main game
 * 
 * \author Iridescence - Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * \date   December 2020
 *********************************************************************/
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

/**
 * Our game state.
 */
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

	std::unique_ptr<GFX::UI::TextRenderer> textRenderer;
	std::shared_ptr<Player> player;
	std::unique_ptr<World> world;
};