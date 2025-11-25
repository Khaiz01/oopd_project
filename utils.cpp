#include "utils.h"
#include <sstream>
#include <algorithm>
#include <cctype>

int to_int(const std::string &s) {
    try {
        size_t idx;
        int v = std::stoi(s,&idx);
        if (idx!=s.size()) throw InputError("Invalid integer: "+s);
        return v;
    } catch(...) { throw InputError("Invalid integer: "+s); }
}

double to_double(const std::string &s) {
    try {
        size_t idx;
        double v = std::stod(s,&idx);
        if (idx!=s.size()) throw InputError("Invalid double: "+s);
        return v;
    } catch(...) { throw InputError("Invalid double: "+s); }
}

std::string trim(const std::string &s){
    auto b = s.find_first_not_of(" \t\r\n");
    if (b==std::string::npos) return "";
    auto e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e-b+1);
}

// --- STRICT REAL-WORLD VALIDATION ---

bool isValidName(const std::string &name) {
    if (name.empty()) return false;
    // Check if every character is a letter
    return std::all_of(name.begin(), name.end(), [](unsigned char c){ 
        return std::isalpha(c); 
    });
}

bool isValidPhone(const std::string &phone) {
    if (phone.empty() || phone.size() < 3) return false;
    // Check if every character is a digit
    return std::all_of(phone.begin(), phone.end(), [](unsigned char c){ 
        return std::isdigit(c); 
    });
}