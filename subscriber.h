#pragma once
#include <string>

struct Subscriber {
    int id;
    std::string name;
    std::string phone;
    std::string type; // "data" or "voice"
    int messages;
    int assigned_channel; // -1 if not assigned
    bool dropped;
};