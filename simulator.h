#pragma once
#include <vector>
#include <string>
#include <memory>
#include "technology.h"
#include "celltower.h"
#include "core.h"
#include "subscriber.h"

class Simulator {
public:
    Simulator();
    void menuLoop();
    void runFromFile(const std::string &path);

private:
    std::shared_ptr<Technology> makeTech(const std::string &name);
    void parseConfig(const std::string &path);
    void allocateAndCompute(const std::string &outBase, bool fileMode);

    // --- INTERACTIVE ACTIONS ---
    void interactiveConfigure();
    void interactiveAddUser();
    void interactiveListUsers();
    void interactiveEditUser(); // Added back
    
    // --- HELPER FOR REALISM ---
    std::string getSignalQuality(int userId) const;
    double calculateCurrentLatency() const;

    // State
    std::shared_ptr<Technology> tech_;
    CellTower tower_;
    CellularCore core_;
    std::vector<Subscriber> subs_;
    double bandwidth_mhz_;
    int antennas_;
    int overhead_per_100_;
    int core_capacity_msgs_;
    std::string allocation_strategy_;
    int next_id_;
    bool debugMode_;
};