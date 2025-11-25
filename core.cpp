#include "core.h"
#include <cmath>

CellularCore::CellularCore(int overhead_per_100, int core_capacity_msgs)
: overhead_per_100_(overhead_per_100), core_capacity_msgs_(core_capacity_msgs) {}

void CellularCore::setOverheadPer100(int v){ overhead_per_100_ = v; }
void CellularCore::setCoreCapacity(int v){ core_capacity_msgs_ = v; }

int CellularCore::overheadFor(int messages) const {
    int blocks = (messages + 99) / 100;
    return blocks * overhead_per_100_;
}

int CellularCore::coresNeeded(int messages) const {
    if (messages == 0) return 0;
    int overhead = overheadFor(messages);
    int total = messages + overhead;
    return (total + core_capacity_msgs_ - 1) / core_capacity_msgs_;
}