TARGET = webserver
CXX = g++

SOURCE = src/*.cc main.cc
HEADER_DIR = include/

COMPILE_FLAGS = -std=c++11 -I$(HEADER_DIR) -lpthread -lmysqlclient -Wall


all: $(SOURCE)
	$(CXX) -o $(TARGET) $^ $(COMPILE_FLAGS)


.PHONY: clean
clean:
	rm -f $(TARGET)
