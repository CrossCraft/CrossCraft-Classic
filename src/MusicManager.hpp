#pragma once
#include <Audio/Clip.hpp>
#include <string>
#include <vector>

namespace CrossCraft {
using namespace Stardust_Celeste;

class MusicManager {
  public:
    MusicManager();
    ~MusicManager();

    auto update(float dt) -> void;

    inline static auto get() -> MusicManager & {
        static MusicManager mm;
        return mm;
    }

  private:
    std::vector<std::string> tracks;
    float timer;
    ScopePtr<Audio::Clip> music;
};
} // namespace CrossCraft