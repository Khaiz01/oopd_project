#pragma once
#include <thread>
#include <atomic>
#include <functional>
#include <string>

class UserDevice {
public:
    UserDevice(int id, int messages, int delay_ms = 100);
    ~UserDevice();
    void start(std::function<void(int)> onMessage);
    void join();
    int id() const;
    int messages() const;

private:
    void run();
    int id_;
    int messages_;
    int delay_ms_;
    std::thread th_;
    std::function<void(int)> onMessage_;
    bool running_;
};