#include "Player.hpp"
#include <Core/Application.hpp>

using namespace Stardust_Celeste;

namespace CrossCraft {

class GameState : public Core::ApplicationState {
  public:
    GameState() = default;
    ~GameState() = default;

    void on_start();
    void on_cleanup();

    void on_update(Core::Application *app, double dt);
    void on_draw(Core::Application *app, double dt);

  private:
    Player player;
};

} // namespace CrossCraft