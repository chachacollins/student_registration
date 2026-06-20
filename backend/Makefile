CXX=g++
CXXFLAGS=-Wall -Wextra -std=c++20 -ggdb
INPUT_FILES=src/main.cpp
TARGET=build/backend

$(TARGET): $(INPUT_FILES)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(INPUT_FILES) -o $(TARGET)

serve: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build

.PHONY: serve clean
