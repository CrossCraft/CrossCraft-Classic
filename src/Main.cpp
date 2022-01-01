#include "Gamestate.hpp"
#include <Stardust-Celeste.hpp>

using namespace Stardust_Celeste;

class GameApplication : public Core::Application {
  public:
    void on_start() override {
        state = create_refptr<CrossCraft::GameState>();
        this->set_state(state);
    }

  private:
    RefPtr<CrossCraft::GameState> state;
};

Core::Application *CreateNewSCApp() {
    Core::AppConfig config;
    config.headless = false;

    Core::PlatformLayer::get().initialize(config);

    return new GameApplication();
}