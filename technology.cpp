#include "technology.h"
#include <stdexcept>
#include <string>
#include <iostream>

// Helper functions for case-insensitive checks
bool is_voice(const std::string &t) { return t == "voice" || t == "Voice" || t == "VOICE"; }
bool is_data(const std::string &t) { return t == "data" || t == "Data" || t == "DATA"; }

void TwoG::validate_usage(const std::string &type, int messages) const {
    if (is_data(type)) {
        if (messages > 5) 
            throw std::runtime_error("Error: 2G Data limit exceeded. Max 5 messages allowed.");
    } else if (is_voice(type)) {
        if (messages > 15) 
            throw std::runtime_error("Error: 2G Voice limit exceeded. Max 15 messages allowed.");
    } else {
        throw std::runtime_error("Error: Invalid 2G Type. Must be 'data' or 'voice'.");
    }
}

void ThreeG::validate_usage(const std::string &type, int messages) const {
    // 3G/4G/5G use Packet Switching for everything.
    // Project constraints usually imply a fixed packet limit (e.g., 10)
    (void)type; 
    if (messages > 10) 
        throw std::runtime_error("Error: 3G Packet limit exceeded. Max 10 messages allowed.");
}

void FourG::validate_usage(const std::string &type, int messages) const {
    (void)type; 
    if (messages > 10) 
        throw std::runtime_error("Error: 4G Packet limit exceeded. Max 10 messages allowed.");
}

void FiveG::validate_usage(const std::string &type, int messages) const {
    (void)type; 
    if (messages > 10) 
        throw std::runtime_error("Error: 5G Packet limit exceeded. Max 10 messages allowed.");
}