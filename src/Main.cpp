#include "Gamestate.hpp"
#include <Stardust-Celeste.hpp>

#if PSP
#include <psppower.h>
#endif

using namespace Stardust_Celeste;

class GameApplication : public Core::Application {
public:
  void on_start() override {
    state = create_refptr<CrossCraft::GameState>();
    this->set_state(state);

    Rendering::Color clearcol;
    clearcol.rgba.r = 0x97;
    clearcol.rgba.g = 0xD5;
    clearcol.rgba.b = 0xFF;
    clearcol.rgba.a = 0xFF;

    Rendering::RenderContext::get().set_color(clearcol);
  }

private:
  RefPtr<CrossCraft::GameState> state;
};

Core::Application *CreateNewSCApp() {
  Core::AppConfig config;
  config.headless = false;

  Core::PlatformLayer::get().initialize(config);

#if PSP
  scePowerSetClockFrequency(333, 333, 166);
#endif

  return new GameApplication();
}