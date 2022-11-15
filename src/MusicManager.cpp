#include "MusicManager.hpp"
//#include "Option.hpp"
#include "TexturePackManager.hpp"
#include "Utils.hpp"

namespace CrossCraft {
MusicManager::MusicManager() {
    std::string ext = "ogg";

    std::string prefix = "audio/mus/";
    tracks.push_back(prefix + "calm1." + ext);
    tracks.push_back(prefix + "calm2." + ext);
    tracks.push_back(prefix + "calm3." + ext);
    tracks.push_back(prefix + "hal1." + ext);
    tracks.push_back(prefix + "hal2." + ext);
    tracks.push_back(prefix + "hal3." + ext);
    tracks.push_back(prefix + "hal4." + ext);

    timer = -1.0f;
}
MusicManager::~MusicManager() {}

auto MusicManager::update(float dt) -> void {
    //    if (!Option::get().music) {
    //        music->stop();
    //        return;
    //    }

    if (timer >= 0) {
        timer -= dt;
        music->update();
    } else {
        music.reset();
        srand(time(0));
        int r = rand() % tracks.size();
        auto file_path = PLATFORM_FILE_PREFIX + tracks[r];
        SC_APP_INFO("FILE {}", file_path);
        music = create_scopeptr<Audio::Clip>(std::move(file_path), true);
        music->play();
        timer = 420.0f + rand() % 360;
    }
}

} // namespace CrossCraft