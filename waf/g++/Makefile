TARGET=../bin/waf
CXX = g++
CXXFLAGS = -Wall -O3
$(TARGET): ../src/*/*.h ../src/*.cpp
	$(CXX) $(CXXFLAGS) ../src/*.cpp -o $(TARGET)

clean:
	rm $(TARGET)
