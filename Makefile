# Minecraft Village Generator - Part A Makefile

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = -I./include
LIBS = -lmcpp

# Source files
SOURCES = src/main.cpp src/plot_validation.cpp src/terraforming.cpp src/wall_builder.cpp src/waypoint_placement.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = gen-village

# Test files
TEST_SOURCES = tests/test_suite.cpp
TEST_TARGET = test-suite

# Default target
all: $(TARGET)

# Build main executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# Build test executable
test: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INCLUDES)

# Compile object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET) $(TEST_TARGET)

# Run tests
run-tests: $(TEST_TARGET)
	./$(TEST_TARGET)

# Run main program
run: $(TARGET)
	./$(TARGET) --testmode

.PHONY: all test clean run-tests run
