#pragma once

class CellularCore {
public:
    CellularCore(int overhead_per_100=10, int core_capacity_msgs=500);
    void setOverheadPer100(int v);
    void setCoreCapacity(int v);
    int overheadFor(int messages) const;
    int coresNeeded(int messages) const;
private:
    int overhead_per_100_;
    int core_capacity_msgs_;
};