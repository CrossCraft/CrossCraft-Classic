#include "SplashState.h"
#include <Utilities/Logger.h>
#include "MainState.h"

SplashState::SplashState() : sprt(), tex(0)
{
}

void SplashState::init()
{
	tex = GFX::g_TextureManager->loadTex("./assets/splash/logo.png", GFX_FILTER_LINEAR, GFX_FILTER_LINEAR, true);

	sprt = new GFX::Render2D::Sprite(tex, { 480, 272 });
	sprt->setPosition(240, 136);
}

void SplashState::cleanup()
{
	GFX::g_TextureManager->deleteTex(tex);
	delete sprt;
}

void SplashState::enter()
{
}

void SplashState::pause()
{
}

void SplashState::resume()
{
}

MainState* menu;
void SplashState::update(Core::GameStateManager* st)
{
	Utilities::g_AppTimer.deltaTime();
	if (Utilities::g_AppTimer.elapsed() >= 4) {
		Utilities::app_Logger->info("End of Splash!");
		menu = new MainState();
		menu->init();
		st->changeState(menu);
	}
}

void SplashState::draw(Core::GameStateManager* st)
{
	sprt->draw();
}
