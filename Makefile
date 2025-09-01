CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
TARGET = polynomial_solver
SOURCE = main.cpp

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: clean run