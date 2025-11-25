#include "simulator.h"
#include <iostream>

int main(int argc, char **argv){
    Simulator sim;
    if (argc == 3 && std::string(argv[1]) == "--file"){
        try {
            sim.runFromFile(argv[2]);
        } catch (const std::exception &e){
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
        return 0;
    }
    // Interactive menu mode
    sim.menuLoop();
    return 0;
}