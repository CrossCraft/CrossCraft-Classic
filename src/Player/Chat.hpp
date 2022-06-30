#pragma once
#include <deque>
#include <string>

namespace CrossCraft {

struct ChatData {
    std::string text;
    double timer;
};

class Chat {
  public:
    Chat();

    void add_message(std::string str);
    void update(double dt);

    std::deque<ChatData> data;
};

} // namespace CrossCraft