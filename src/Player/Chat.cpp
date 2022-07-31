#include "Chat.hpp"
#include <vector>

namespace CrossCraft {

Chat::Chat() { data.clear(); }

void Chat::add_message(std::string str) { data.push_back({str, 5.0f}); }

void Chat::update(double dt) {
    std::vector<int> removal;

    for (int i = 0; i < data.size(); i++) {
        auto &p = data.at(i);

        p.timer -= dt;

        if (p.timer < 0)
            removal.push_back(i);
    }

    for (auto &id : removal) {
        if(id < data.size())
        data.erase(data.begin() + id);
    }
}
} // namespace CrossCraft