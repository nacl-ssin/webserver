TARGET = webserver
CXX = g++

SOURCE = src/*.cc main.cpp
HEADER_DIR = include/

FLAGS = -std=c++11 -I$(HEADER_DIR) -lpthread -Wall


all: $(SOURCE)
	$(CXX) -o $(TARGET) $^ $(FLAGS)


.PHONY: clean
clean:
	rm -rf $(TARGET)
