TARGET=../bin/waf

HEADERS = $(wildcard ../src/*/*.h)
SOURCES = $(wildcard ../src/*/*.cpp ../src/*.cpp)

CXX = g++
CXXFLAGS = -Wall -O3

$(TARGET): $(HEADERS) $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm $(TARGET)

run:
	./$(TARGET)
