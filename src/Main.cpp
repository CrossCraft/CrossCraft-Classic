#include <Stardust-Celeste.hpp>

using namespace Stardust_Celeste;

class GameApplication : public Core::Application {
  public:
    void on_start() override {
		SC_APP_INFO("TEST!");
    }

  private:
};

Core::Application *CreateNewSCApp() {
    Core::AppConfig config;
	config.headless = false;
	
    Core::PlatformLayer::get().initialize(config);

    return new GameApplication();
}