#include "minigit.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <command> [args]" << std::endl;
        std::cerr << "Commands: init, add, commit -m \"message\", log, status, branch, checkout, merge" << std::endl;
        return 1;
    }
    
    MiniGit git;
    
    // Load existing repository if available
    if (fs::exists(".minigit")) {
        git.loadState();
    }
    
    std::string command = argv[1];
    
    if (command == "init") {
        git.init();
    } 
    else if (command == "add" && argc > 2) {
        for (int i = 2; i < argc; i++) {
            git.add(argv[i]);
        }
    }
    else if (command == "commit" && argc > 3 && std::string(argv[2]) == "-m") {
        std::string message = argv[3];
        git.commit(message);
    }
    else if (command == "log") {
        git.log();
    }
    else if (command == "status") {
        git.status();
    }
    else if (command == "branch") {
        if (argc > 2) {
            git.branch(argv[2]);
        } else {
            // Show branches if no name provided
            MiniGit git;
            if (fs::exists(".minigit")) git.loadState();
            git.status(); // Shows branches in status
        }
    }
    else if (command == "checkout" && argc > 2) {
        git.checkout(argv[2]);
    }
    else if (command == "merge" && argc > 2) {
        git.merge(argv[2]);
    }
    else {
        std::cerr << "Invalid command or arguments" << std::endl;
        return 1;
    }
    
    return 0;
}