BIN_DIR = .
SRC_DIR = ../../../src

TARGET = $(BIN_DIR)/test

HEADERS = $(wildcard $(SRC_DIR)/*/*.h)
SOURCES = $(wildcard $(SRC_DIR)/*/*.cpp *.cpp)

CXX = g++
CXXFLAGS = -Wall -O3

$(TARGET): $(HEADERS) $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -I$(SRC_DIR) -o $(TARGET)

clean:
	rm -f $(TARGET)

run:
	./$(TARGET)
