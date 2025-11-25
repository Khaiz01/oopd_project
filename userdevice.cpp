#include "userdevice.h"
#include <chrono>
#include <thread>

UserDevice::UserDevice(int id, int messages, int delay_ms)
: id_(id), messages_(messages), delay_ms_(delay_ms), running_(false) {}

UserDevice::~UserDevice(){
    if (th_.joinable()) th_.join();
}

void UserDevice::start(std::function<void(int)> onMessage){
    onMessage_ = onMessage;
    running_ = true;
    th_ = std::thread(&UserDevice::run, this);
}

void UserDevice::join(){
    if (th_.joinable()) th_.join();
}

int UserDevice::id() const { return id_; }
int UserDevice::messages() const { return messages_; }

void UserDevice::run(){
    for (int i = 0; i < messages_; ++i){
        // Randomize delay slightly for realism
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms_ + (rand()%100)));
        if (onMessage_) onMessage_(i + 1); // Send message number
    }
    running_ = false;
}