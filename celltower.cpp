#include "celltower.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

CellTower::CellTower(): tech_(nullptr), bandwidth_mhz_(1.0), antennas_(1) {}

void CellTower::setTechnology(std::shared_ptr<Technology> t){
    std::lock_guard<std::mutex> lk(mtx_);
    tech_ = t;
}

void CellTower::setBandwidth(double bw_mhz){
    std::lock_guard<std::mutex> lk(mtx_);
    bandwidth_mhz_ = bw_mhz;
}

void CellTower::setAntennas(int a){
    std::lock_guard<std::mutex> lk(mtx_);
    antennas_ = a;
}

int CellTower::channels() const{
    if (!tech_) return 0;
    return tech_->channels_for_bandwidth(bandwidth_mhz_);
}

int CellTower::usersPerChannel() const{
    if (!tech_) return 0;
    return tech_->users_per_channel() * std::max(1, antennas_);
}

int CellTower::totalCapacity() const{
    return channels() * usersPerChannel();
}

void CellTower::allocate(std::vector<Subscriber> &subs, const std::string &strategy){
    std::lock_guard<std::mutex> lk(mtx_);
    allocations_.clear();
    int ch = channels();
    if (ch <= 0) return;
    
    allocations_.resize(ch);
    int per = usersPerChannel();
    
    // Sort subs by ID for consistency
    // (Optional, keeps visualization stable)
    
    if (strategy == "round_robin"){
        int idx = 0;
        for (auto &s : subs){
            if (s.dropped) continue;
            bool placed = false;
            for (int k=0; k<ch; k++){
                int i = (idx + k) % ch;
                if ((int)allocations_[i].size() < per){
                    allocations_[i].push_back(s.id);
                    s.assigned_channel = i+1;
                    placed = true;
                    break;
                }
            }
            if (!placed){ s.dropped = true; s.assigned_channel = -1; }
            idx++;
        }
    } else { // best_fit
        int cur = 0;
        for (auto &s : subs){
            if (s.dropped) continue;
            while (cur < ch && (int)allocations_[cur].size() >= per) cur++;
            if (cur >= ch){ s.dropped = true; s.assigned_channel = -1; continue; }
            allocations_[cur].push_back(s.id);
            s.assigned_channel = cur + 1;
        }
    }
}

std::vector<int> CellTower::usersInChannel(int ch) const{
    std::lock_guard<std::mutex> lk(mtx_);
    if (ch < 1 || ch > (int)allocations_.size()) return {};
    return allocations_[ch-1];
}

int CellTower::channelsUsed() const {
    std::lock_guard<std::mutex> lk(mtx_);
    int count = 0;
    for(const auto &c : allocations_) if(!c.empty()) count++;
    return count;
}

// --- NEW VISUALIZER ---
void CellTower::printSpectrumMap() const {
    std::lock_guard<std::mutex> lk(mtx_);
    std::cout << "\n--- SPECTRUM ALLOCATION MAP ---\n";
    if (!tech_) { std::cout << "System Offline\n"; return; }

    int capPerCh = usersPerChannel();
    for(size_t i=0; i<allocations_.size(); ++i) {
        std::cout << " CH " << std::setw(2) << (i+1) << " |";
        
        // Draw occupied slots
        for(int uid : allocations_[i]) {
            std::cout << "[U" << std::setw(3) << uid << "]";
        }
        
        // Draw empty slots
        int empty = capPerCh - allocations_[i].size();
        for(int k=0; k<empty; ++k) {
            std::cout << "[ ... ]";
        }
        std::cout << "\n";
    }
    std::cout << "-------------------------------\n";
}