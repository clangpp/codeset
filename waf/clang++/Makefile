BIN_DIR = ../bin
SRC_DIR = ../src

TARGET = $(BIN_DIR)/waf

HEADERS = $(wildcard $(SRC_DIR)/*/*.h)
SOURCES = $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/../../*/*/*/*.cpp)

CXX = clang++
CXXFLAGS = -Wall -O3

$(TARGET): $(HEADERS) $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

run:
	./$(TARGET)
