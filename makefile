CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

SRC = minigit.cpp main.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = minigit

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean