#include "simulator.h"
#include "userdevice.h"
#include "subscriber.h"
#include "technology.h"
#include "celltower.h"
#include "core.h"
#include "utils.h" // Colors are defined here now

#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <atomic>
#include <algorithm>
#include <map>
#include <mutex>
#include <iomanip> 
#include <cmath>

std::mutex print_mtx; 

Simulator::Simulator()
: tech_(std::make_shared<FourG>()), tower_(), core_(10,500),
  subs_(), bandwidth_mhz_(1.0), antennas_(4),
  overhead_per_100_(10), core_capacity_msgs_(500),
  allocation_strategy_("best_fit"), next_id_(1), debugMode_(false) {

    tower_.setTechnology(tech_);
    tower_.setBandwidth(bandwidth_mhz_);
    tower_.setAntennas(antennas_);
    core_.setOverheadPer100(overhead_per_100_);
    core_.setCoreCapacity(core_capacity_msgs_);
}

std::shared_ptr<Technology> Simulator::makeTech(const std::string &name){
    if (name == "2G") return std::make_shared<TwoG>();
    if (name == "3G") return std::make_shared<ThreeG>();
    if (name == "4G") return std::make_shared<FourG>();
    if (name == "5G") return std::make_shared<FiveG>();
    throw InputError("Unknown technology: " + name);
}

// --- VISUALIZATION HELPERS ---

std::string Simulator::getSignalQuality(int userId) const {
    int qual = (userId * 17) % 100; 
    if (qual > 80) return GREEN + "Excellent (-60dBm)" + RESET;
    if (qual > 50) return CYAN + "Good (-85dBm)" + RESET;
    if (qual > 20) return YELLOW + "Fair (-100dBm)" + RESET;
    return RED + "Poor (-115dBm)" + RESET;
}

double Simulator::calculateCurrentLatency() const {
    double base = 50.0;
    if (tech_->name() == "5G") base = 10.0;
    else if (tech_->name() == "4G") base = 30.0;
    else if (tech_->name() == "3G") base = 80.0;
    else if (tech_->name() == "2G") base = 150.0;

    double loadFactor = (double)subs_.size() / (double)tower_.totalCapacity();
    return base + (loadFactor * base * 2.0); 
}

// --- CORE SIMULATION ---

void Simulator::allocateAndCompute(const std::string &outBase, bool fileMode){
    tower_.allocate(subs_, allocation_strategy_);
    
    // Feature 1: Visual Spectrum Map
    std::cout << CYAN;
    tower_.printSpectrumMap();
    std::cout << RESET;

    std::vector<std::unique_ptr<UserDevice>> devices;
    long total_messages_sent = 0;
    
    // Calculate Interference Chance based on Load
    double loadFactor = (double)subs_.size() / (double)tower_.totalCapacity();
    int failureChance = (int)(loadFactor * 30.0); // Up to 30% fail rate at full load

    std::cout << "\n" << BOLD << "--- INITIALIZING SIMULATION CLUSTER ---" << RESET << "\n";
    std::cout << "Interference Level: " << failureChance << "% (Load Dependent)\n";

    for (const auto &s : subs_){
        if (s.dropped) continue;
        devices.push_back(std::make_unique<UserDevice>(s.id, s.messages));
        total_messages_sent += s.messages;
    }

    // Launch Threads
    for (auto &d : devices){
        d->start([&](int msgNum){
            std::lock_guard<std::mutex> lk(print_mtx);
            
            // Feature 2: Simulated Packet Loss
            bool dropped = (rand() % 100) < failureChance;
            
            std::cout << "[User " << std::setw(3) << d->id() << "] TX Packet " 
                      << std::setw(2) << msgNum << "/" << std::setw(2) << d->messages();

            if (dropped) {
                 std::cout << RED << " | STATUS: FAILED (Interference) -> RETRYING... OK" << RESET << "\n";
            } else {
                 std::cout << GREEN << " | STATUS: OK" << RESET << "\n";
            }
        });
    }

    for (auto &d : devices){
        d->join();
    }
    std::cout << "\n" << BOLD << "--- SIMULATION COMPLETE ---" << RESET << "\n";

    // --- NEW FEATURE: ASCII GRAPHICAL ANALYTICS (Block Style) ---
    if (total_messages_sent > 0) {
        std::cout << "\n" << CYAN << "--- TRAFFIC DISTRIBUTION ANALYTICS ---" << RESET << "\n";
        long max_msgs = 0;
        for(const auto &s : subs_) if(!s.dropped) max_msgs = std::max((long)s.messages, max_msgs);
        
        for(const auto &s : subs_){
            if (s.dropped) continue;
            // Calculate percentage
            double pct = (max_msgs > 0) ? ((double)s.messages / max_msgs) : 0.0;
            int barLength = (int)(pct * 20.0); // Length of bar (20 chars max)
            
            std::cout << "User " << std::setw(3) << s.id << " |";
            
            // Draw filled part
            std::cout << GREEN; 
            for(int k=0; k<barLength; ++k) std::cout << "\u2588"; 
            std::cout << RESET;
            
            // Draw empty part
            for(int k=barLength; k<20; ++k) std::cout << "\u2591"; 
            
            std::cout << " (" << (int)(pct*100) << "%)\n";
        }
    }
    // -------------------------------------------------------

    int overhead = core_.overheadFor(total_messages_sent);
    int cores = core_.coresNeeded(total_messages_sent);
    
    // Feature 3: Billing System
    double costPerMsg = 0.01;
    if(tech_->name() == "5G") costPerMsg = 0.05;
    else if(tech_->name() == "4G") costPerMsg = 0.03;
    
    double totalRevenue = total_messages_sent * costPerMsg;

    std::cout << CYAN << "------------------------------------------\n";
    std::cout << " ANALYTICS & BILLING REPORT\n";
    std::cout << "------------------------------------------" << RESET << "\n";
    std::cout << " Total Messages Processed : " << total_messages_sent << "\n";
    std::cout << " Network Overhead         : " << overhead << " msgs\n";
    std::cout << " Total Traffic Load       : " << (total_messages_sent + overhead) << " msgs\n";
    std::cout << " Cellular Cores Active    : " << cores << "\n";
    std::cout << " Avg Network Latency      : " << calculateCurrentLatency() << " ms\n";
    std::cout << " Revenue Generated        : " << GREEN << "$" << std::fixed << std::setprecision(2) << totalRevenue << RESET << " (@ $" << costPerMsg << "/msg)\n";
    std::cout << CYAN << "------------------------------------------" << RESET << "\n";
    
    if (fileMode) {
        std::ofstream f(outBase);
        if(f) f << "Report\nTotal Msg: " << total_messages_sent << "\n";
    }
}

// --- FILE PARSING ---

void Simulator::parseConfig(const std::string &path){
    std::ifstream f(path);
    if (!f) throw InputError("Cannot open config file: " + path);
    std::string line;
    while(std::getline(f, line)){
        line = trim(line);
        if (line.empty() || line[0]=='#') continue;
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = trim(line.substr(0, eq));
        std::string val = trim(line.substr(eq+1));

        if (key == "technology") { tech_ = makeTech(val); tower_.setTechnology(tech_); }
        else if (key == "bandwidth_mhz") { bandwidth_mhz_ = to_double(val); tower_.setBandwidth(bandwidth_mhz_); }
        else if (key == "antennas") { antennas_ = to_int(val); tower_.setAntennas(antennas_); }
        else if (key.find("user") == 0) {
            Subscriber s; s.id = next_id_++; s.dropped=false; s.assigned_channel=-1;
            std::stringstream ss(val);
            std::string segment;
            while(std::getline(ss, segment, ',')){
                auto c = segment.find(':');
                if (c == std::string::npos) continue;
                std::string k = trim(segment.substr(0,c)), v = trim(segment.substr(c+1));
                if (k=="name") s.name=v; else if (k=="phone") s.phone=v;
                else if (k=="type") s.type=v; else if (k=="msg") s.messages=to_int(v);
            }
            try {
                if(!isValidName(s.name)) throw std::runtime_error("Invalid Name (Letters only)");
                if(!isValidPhone(s.phone)) throw std::runtime_error("Invalid Phone (Digits only)");
                if (subs_.size() >= (size_t)tower_.totalCapacity()) throw std::runtime_error("Capacity Reached");
                tech_->validate_usage(s.type, s.messages);
                subs_.push_back(s);
            } catch (const std::exception &e) { std::cerr << RED << "Import Error ID " << s.id << ": " << e.what() << RESET << "\n"; }
        }
    }
}

void Simulator::runFromFile(const std::string &path){
    parseConfig(path);
    allocateAndCompute("output_report.csv", true);
}

// --- INTERACTIVE MENU ---

void Simulator::menuLoop(){
    while(true){
        tower_.allocate(subs_, allocation_strategy_); 
        int total_cap = tower_.totalCapacity();
        int users = subs_.size();
        int data_users = 0, voice_users = 0;
        for(const auto& s : subs_) {
            if(s.type == "data" || s.type == "Data" || s.type == "DATA") data_users++; else voice_users++;
        }
        
        double load_pct = (total_cap > 0) ? ((double)users / total_cap) * 100.0 : 0.0;
        double latency = calculateCurrentLatency();
        
        std::string statusColor = GREEN;
        if (load_pct > 90.0) statusColor = RED;
        else if (load_pct > 75.0) statusColor = YELLOW;

        std::cout << "\n";
        std::cout << CYAN << "===============================================================\n";
        std::cout << "             CELLULAR NETWORK CONTROL CENTER                   \n";
        std::cout << "===============================================================" << RESET << "\n";
        std::cout << " SYSTEM STATUS : " << statusColor << (load_pct > 90 ? "CRITICAL" : "ONLINE") << RESET << "\n";
        std::cout << " TECHNOLOGY    : " << BOLD << tech_->name() << RESET << " | " << bandwidth_mhz_ << " MHz | " << antennas_ << "x MIMO\n";
        std::cout << CYAN << "---------------------------------------------------------------" << RESET << "\n";
        std::cout << " NETWORK METRICS\n";
        std::cout << " [LOAD] " << std::fixed << std::setprecision(1) << load_pct << "% Utilized  ";
        std::cout << " [LATENCY] " << latency << " ms (est)\n";
        std::cout << CYAN << "---------------------------------------------------------------" << RESET << "\n";
        std::cout << " CONNECTIVITY\n";
        std::cout << " Active Users  : " << std::setw(4) << users << " / " << std::setw(4) << total_cap << "\n";
        std::cout << " Traffic Type  : " << std::setw(4) << data_users << " Data  | " << std::setw(4) << voice_users << " Voice\n";
        std::cout << CYAN << "===============================================================" << RESET << "\n";
        std::cout << " 1. Configure Network\n";
        std::cout << " 2. Add User\n";
        std::cout << " 3. Show Current Users\n";
        std::cout << " 4. Edit User List\n";
        std::cout << " 5. Run Simulation\n";
        std::cout << " 6. Reset All\n";
        std::cout << " 7. Exit\n";
        std::cout << CYAN << "---------------------------------------------------------------" << RESET << "\n";
        std::cout << "Select Operation > ";
        
        std::string cmd; std::cin >> cmd;

        if (cmd == "1") interactiveConfigure();
        else if (cmd == "2") interactiveAddUser();
        else if (cmd == "3") interactiveListUsers();
        else if (cmd == "4") interactiveEditUser();
        else if (cmd == "5") allocateAndCompute("report.txt", false);
        else if (cmd == "6") { 
            subs_.clear(); next_id_ = 1; 
            std::cout << YELLOW << ">> SYSTEM RESET: All users cleared." << RESET << "\n"; 
        }
        else if (cmd == "7") break;
        else std::cout << RED << "Invalid command." << RESET << "\n";
    }
}

void Simulator::interactiveConfigure(){
    std::string tName;
    std::cout << "\n--- NETWORK CONFIGURATION ---\n";
    std::cout << "Technology (2G/3G/4G/5G): "; std::cin >> tName;
    try {
        tech_ = makeTech(tName);
        tower_.setTechnology(tech_);
        std::cout << "Bandwidth (MHz): "; std::cin >> bandwidth_mhz_; tower_.setBandwidth(bandwidth_mhz_);
        std::cout << "Antennas: "; std::cin >> antennas_; tower_.setAntennas(antennas_);
        std::cout << GREEN << "-> Configuration Applied." << RESET << "\n";
    } catch (std::exception &e) { std::cout << RED << "Error: " << e.what() << RESET << "\n"; }
}

void Simulator::interactiveAddUser(){
    int n; 
    std::cout << "\n--- ADD USER(S) ---\n";
    std::cout << "How many users to add? (Enter 1 for single): ";
    std::cin >> n;

    for (int i=0; i<n; i++){
        Subscriber s; s.id = next_id_++; s.dropped=false; s.assigned_channel=-1;
        std::cout << "\n[User " << s.id << " Input]\n";
        std::cout << " Name               : "; std::cin >> s.name;
        
        if(!isValidName(s.name)) {
            std::cout << RED << "!!! FAILURE: Invalid Name '" << s.name << "'. Name must contain ONLY letters." << RESET << "\n";
            next_id_--; continue; 
        }

        std::cout << " Phone              : "; std::cin >> s.phone;
        if(!isValidPhone(s.phone)) {
            std::cout << RED << "!!! FAILURE: Invalid Phone '" << s.phone << "'. Must contain ONLY digits." << RESET << "\n";
            next_id_--; continue;
        }

        std::cout << " Type (data/voice)  : "; std::cin >> s.type;
        std::cout << " Msg Count          : "; std::cin >> s.messages;

        if (subs_.size() >= (size_t)tower_.totalCapacity()) {
            std::cout << RED << "!!! FAILURE: Capacity Limit Reached. Cannot add user." << RESET << "\n"; next_id_--; continue;
        }
        
        try {
            tech_->validate_usage(s.type, s.messages);
            subs_.push_back(s);
            std::cout << GREEN << "-> User " << s.name << " added successfully." << RESET << "\n";
        } catch (std::exception &e) {
            std::cout << RED << "!!! FAILURE: " << e.what() << RESET << "\n"; next_id_--;
        }
    }
}

void Simulator::interactiveListUsers(){
    std::cout << "\n--- CURRENT CONNECTED USERS ---\n";
    std::cout << std::left << std::setw(5) << "ID" 
              << std::setw(15) << "Name" 
              << std::setw(15) << "Phone" 
              << std::setw(10) << "Type" 
              << std::setw(10) << "Msgs" 
              << "Signal Quality" << "\n";
    std::cout << "------------------------------------------------------------------------\n";
    
    if (subs_.empty()) {
        std::cout << "(No users connected)\n";
        return;
    }

    for(const auto &s : subs_){
        std::cout << std::left << std::setw(5) << s.id 
                  << std::setw(15) << s.name 
                  << std::setw(15) << s.phone 
                  << std::setw(10) << s.type 
                  << std::setw(10) << s.messages 
                  << getSignalQuality(s.id) << "\n";
    }
    std::cout << "------------------------------------------------------------------------\n";
}

void Simulator::interactiveEditUser(){
    int id; 
    std::cout << "Enter User ID to edit: "; 
    std::cin >> id;
    
    auto it = std::find_if(subs_.begin(), subs_.end(), [id](const Subscriber& s){ return s.id == id; });
    if (it == subs_.end()) { std::cout << "User ID not found.\n"; return; }
    
    std::cout << "Editing User: " << it->name << "\n";
    std::string n, p, t; int m;
    
    std::cout << "New Name : "; std::cin >> n;
    if(!isValidName(n)) { std::cout << RED << "!!! FAILURE: Invalid Name." << RESET << "\n"; return; }

    std::cout << "New Phone: "; std::cin >> p;
    if(!isValidPhone(p)) { std::cout << RED << "!!! FAILURE: Invalid Phone." << RESET << "\n"; return; }

    std::cout << "New Type : "; std::cin >> t;
    std::cout << "New Msgs : "; std::cin >> m;

    try {
        tech_->validate_usage(t, m);
        it->name = n; it->phone = p; it->type = t; it->messages = m;
        std::cout << GREEN << "-> User updated successfully." << RESET << "\n";
    } catch (std::exception &e) {
        std::cout << RED << "!!! FAILURE: " << e.what() << RESET << "\n";
    }
}