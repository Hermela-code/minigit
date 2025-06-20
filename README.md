# MiniGit - A Minimal Git Clone

A lightweight version control system implemented in C++ for Linux.

## Build & Run
```bash
git clone https://github.com/yourusername/minigit.git
cd minigit
make

Safe Testing Protocol

Always follow these steps to prevent source code loss:

    Create isolated test environment:
    bash

mkdir test_env && cd test_env  # Never test in source directory!
cp ../minigit .  # Copy ONLY the executable
./minigit init   # Initialize test repo

Test commands safely:
bash

echo "test" > demo.txt
./minigit add demo.txt
./minigit commit -m "Test commit"

Update and retest:
bash

    cd ..  # Return to source
    make && cp minigit test_env/  # Update binary
    cd test_env && ./minigit log  # Continue testing

 Basic Commands
bash

# Track files
./minigit add file.txt

# Commit changes
./minigit commit -m "message"

# Branching
./minigit branch new-feature
./minigit checkout new-feature

# Merging
./minigit merge feature

 Team

    Hermela - Core functionality

    Hemen - Branching and merging

    Samrawit - File operations

 Notes

    Built for Linux (uses C++17 filesystem)

    Academic project for Data Structures and Algorithms

    Warning: Always test in test_env/ to protect source files
