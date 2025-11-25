#pragma once
#include <string>

class Technology {
public:
    virtual ~Technology() = default;
    virtual std::string name() const = 0;
    virtual int users_per_channel() const = 0;
    virtual int channel_bandwidth_khz() const = 0;
    virtual int channels_for_bandwidth(double bw_mhz) const = 0;
    
    // STRICT VALIDATION: This function checks limits and throws an error if violated
    virtual void validate_usage(const std::string &type, int messages) const = 0;
};

class TwoG : public Technology {
public:
    std::string name() const override { return "2G"; }
    int users_per_channel() const override { return 16; }
    int channel_bandwidth_khz() const override { return 200; }
    int channels_for_bandwidth(double bw_mhz) const override { 
        return static_cast<int>(bw_mhz*1000) / channel_bandwidth_khz(); 
    }
    // 2G Specific Rules: Max 5 Data, Max 15 Voice
    void validate_usage(const std::string &type, int messages) const override;
};

class ThreeG : public Technology {
public:
    std::string name() const override { return "3G"; }
    int users_per_channel() const override { return 32; }
    int channel_bandwidth_khz() const override { return 200; }
    int channels_for_bandwidth(double bw_mhz) const override { 
        return static_cast<int>(bw_mhz*1000) / channel_bandwidth_khz(); 
    }
    void validate_usage(const std::string &type, int messages) const override;
};

class FourG : public Technology {
public:
    std::string name() const override { return "4G"; }
    int users_per_channel() const override { return 30; }
    int channel_bandwidth_khz() const override { return 10; }
    int channels_for_bandwidth(double bw_mhz) const override { 
        return static_cast<int>(bw_mhz*1000) / channel_bandwidth_khz(); 
    }
    void validate_usage(const std::string &type, int messages) const override;
};

class FiveG : public Technology {
public:
    std::string name() const override { return "5G"; }
    int users_per_channel() const override { return 30; }
    int channel_bandwidth_khz() const override { return 1000; }
    int channels_for_bandwidth(double bw_mhz) const override { 
        return static_cast<int>(bw_mhz*1000) / channel_bandwidth_khz(); 
    }
    void validate_usage(const std::string &type, int messages) const override;
};