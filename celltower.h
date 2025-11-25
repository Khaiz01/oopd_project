#pragma once
#include <vector>
#include <memory>
#include <mutex>
#include "technology.h"
#include "subscriber.h"

class CellTower {
public:
    CellTower();
    void setTechnology(std::shared_ptr<Technology> t);
    void setBandwidth(double bw_mhz);
    void setAntennas(int a);
    
    int channels() const;
    int usersPerChannel() const;
    int totalCapacity() const;
    
    void allocate(std::vector<Subscriber> &subs, const std::string &strategy);
    std::vector<int> usersInChannel(int ch) const;
    int channelsUsed() const;

    // --- NEW VISUAL FEATURE ---
    void printSpectrumMap() const;

private:
    std::shared_ptr<Technology> tech_;
    double bandwidth_mhz_;
    int antennas_;
    std::vector<std::vector<int>> allocations_;
    mutable std::mutex mtx_;
};