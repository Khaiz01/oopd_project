CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread

SRC = main.o simulator.o celltower.o core.o technology.o userdevice.o subscriber.o utils.o


OBJ = $(SRC:.cpp=.o)

all: debug

debug: CXXFLAGS += -g -DDEBUG
debug: sim_debug

sim_debug: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o sim_debug

.PHONY: all debug clean