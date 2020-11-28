#include "MenuState.h"
#include <Utilities/Input.h>
//#include "MainState.h"
#include "../Client.h"

MenuState::MenuState()
{
}

#if CURRENT_PLATFORM == PLATFORM_PSP
#define SOUND_EXT ".bgm"
#define TXT_OFFSET 4
#else
#define SOUND_EXT ".ogg"
#define TXT_OFFSET 0
#endif

void MenuState::init()
{
	//music = new Audio::AudioClip("./assets/menu/" + std::string(SOUND_EXT), true);
	//music->SetLoop(true);
	//music->Play();

	logoTex = GFX::g_TextureManager->loadTex("./assets/menu/logo.png", GFX_FILTER_NEAREST, GFX_FILTER_NEAREST, true);
	buttonTex = GFX::g_TextureManager->loadTex("./assets/menu/button.png", GFX_FILTER_NEAREST, GFX_FILTER_NEAREST, true);

	logo = new GFX::Render2D::Sprite(logoTex);
	button = new GFX::Render2D::Sprite(buttonTex);
	button->setScale(1.5f, 1.5f);

	logo->setPosition(240, 32);
	
	text.init("./assets/font.pgf");
	text.setStyle({ 255, 255, 255, 255, 1.0f, TEXT_RENDERER_CENTER, TEXT_RENDERER_CENTER, 0.0f, 0 });

	sel = 0;
}

void MenuState::cleanup()
{
	text.cleanup();
	music->Stop();
	delete music;
	GFX::g_TextureManager->deleteTex(logoTex);
	delete logo;
}

void MenuState::enter()
{

}

void MenuState::pause()
{

}

void MenuState::resume()
{

}

void MenuState::update(Core::GameStateManager* st) {

	if (Utilities::KeyPressed(GLFW_KEY_UP) || Utilities::KeyPressed(PSP_CTRL_UP)) {
		sel--;
		if (sel < 0) {
			sel = 0;
		}
	}

	if (Utilities::KeyPressed(GLFW_KEY_DOWN) || Utilities::KeyPressed(PSP_CTRL_DOWN)) {
		sel++;
		if (sel > 3) {
			sel = 3;
		}
	}

	if (Utilities::KeyPressed(GLFW_KEY_SPACE) || Utilities::KeyPressed(GLFW_KEY_ENTER) || Utilities::KeyPressed(PSP_CTRL_CROSS) || Utilities::KeyPressed(PSP_CTRL_START)) {
		if (sel == 0) {
			//Load the game
			//MainState* ms = new MainState();
			//ms->init();
			//st->addState(ms);
		}
		else if (sel == 1) {
			try {
				Client* client = new Client("127.0.0.1");

				while (true) {}
				delete client;
			}
			catch (std::exception& e) {
				Utilities::app_Logger->error(e.what());
			}
		}
		else if (sel == 2) {
			//No options yet
		}
		else {
#if CURRENT_PLATFORM == PLATFORM_PSP
			Platform::exitPlatform();
#else
			exit(0);
#endif
		}
	}

}

void MenuState::draw(Core::GameStateManager* st)
{
	logo->draw();


	for (int i = 0; i < 4; i++) {
		button->setPosition(240, 136 - 32 + 32 * i);
		button->draw();
	}


	if (sel == 0) {
		text.setStyle({ 255, 0, 0, 255, 1.0f, TEXT_RENDERER_CENTER, TEXT_RENDERER_CENTER, 0.0f, 0 });
	}
	else {
		text.setStyle({ 255, 255, 255, 255, 1.0f, TEXT_RENDERER_CENTER, TEXT_RENDERER_CENTER, 0.0f, 0 });
	}
	text.draw("Start", { 240, 136 - 32 + TXT_OFFSET });


	if (sel == 1) {
		text.setStyle({ 255, 0, 0, 255, 1.0f, TEXT_RENDERER_CENTER, TEXT_RENDERER_CENTER, 0.0f, 0 });
	}
	else {
		text.setStyle({ 255, 255, 255, 255, 1.0f, TEXT_RENDERER_CENTER, TEXT_RENDERER_CENTER, 0.0f, 0 });
	}
	text.draw("Multiplayer", { 240, 136 + 32 * 0 + TXT_OFFSET });


	if (sel == 2) {
		text.setStyle({ 255, 0, 0, 255, 1.0f, TEXT_RENDERER_CENTER, TEXT_RENDERER_CENTER, 0.0f, 0 });
	}
	else {
		text.setStyle({ 255, 255, 255, 255, 1.0f, TEXT_RENDERER_CENTER, TEXT_RENDERER_CENTER, 0.0f, 0 });
	}
	text.draw("Options", { 240, 136 + 32 * 1 + TXT_OFFSET });

	if (sel == 3) {
		text.setStyle({ 255, 0, 0, 255, 1.0f, TEXT_RENDERER_CENTER, TEXT_RENDERER_CENTER, 0.0f, 0 });
	}
	else {
		text.setStyle({ 255, 255, 255, 255, 1.0f, TEXT_RENDERER_CENTER, TEXT_RENDERER_CENTER, 0.0f, 0 });
	}
	text.draw("Exit", { 240, 136 + 32 * 2 + TXT_OFFSET });

}
