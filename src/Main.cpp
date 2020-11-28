/*****************************************************************//**
 * \file   main.cpp
 * \brief  The Main File
 *
 * \author Iridescence - Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * \date   November 2020
 *********************************************************************/
#include <iostream>
#include <Utilities/Logger.h>
#include <GFX/RenderCore.h>
#include <Core/StateManager.h>
#include "States/SplashState.h"

using namespace Stardust;

inline auto setup_logger() -> void {
	Utilities::app_Logger->autoFlush = true;
	Utilities::app_Logger->toConsole = true;
	Utilities::detail::core_Logger->autoFlush = true;
	Utilities::detail::core_Logger->toConsole = true;

	Utilities::app_Logger->currentLevel = Utilities::LOGGER_LEVEL_DEBUG;
	Utilities::detail::core_Logger->currentLevel = Utilities::LOGGER_LEVEL_DEBUG;
}

auto main() -> int {
	Platform::initPlatform();

#if CURRENT_PLATFORM != PLATFORM_PSP 
	Platform::PC::g_Window->setTitle("CrossCraft Classic");
	Platform::PC::g_Window->setVsync(true);
#endif

	setup_logger();

	GFX::g_RenderCore->setDefault2DMode();
	GFX::g_RenderCore->setClearColor(0.1f, 0.0f, 0.1f, 1.0f);

	Core::GameStateManager gsm;
	SplashState* splash = new SplashState();
	splash->init();

	gsm.changeState(splash);

	while (gsm.isRunning()
#if CURRENT_PLATFORM != PLATFORM_PSP 
		&& !Platform::PC::g_Window->shouldClose()
#endif
		) {
		GFX::g_RenderCore->beginFrame();
		GFX::g_RenderCore->clear();

		gsm.update();
		gsm.draw();

		GFX::g_RenderCore->endFrame();
		Platform::platformUpdate();
	}

	Platform::exitPlatform();
	return 0;
}