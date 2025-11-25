CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread

SRC = src/main.cpp \
      src/simulator.cpp \
      src/technology.cpp \
      src/subscriber.cpp \
      src/celltower.cpp \
      src/core.cpp \
      src/utils.cpp \
      src/userdevice.cpp

OBJ = $(SRC:.cpp=.o)

all: sim_debug

sim_debug: $(OBJ)
	$(CXX) $(CXXFLAGS) -o sim_debug $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o sim_debug simulation_report.txt

.PHONY: all clean