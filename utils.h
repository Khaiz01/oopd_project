#pragma once
#include <string>
#include <stdexcept>

struct InputError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

int to_int(const std::string &s);
double to_double(const std::string &s);
std::string trim(const std::string &s);

// --- NEW VALIDATION HELPERS ---
bool isValidName(const std::string &name);
bool isValidPhone(const std::string &phone);

// --- COLOR DEFINITIONS (Fixes your error) ---
const std::string RESET   = "\033[0m";
const std::string RED     = "\033[31m";
const std::string GREEN   = "\033[32m";
const std::string YELLOW  = "\033[33m";
const std::string BLUE    = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN    = "\033[36m";
const std::string BOLD    = "\033[1m";