CC=g++
CFLAGS=-c -Wall -std=c++11 -I include
LDFLAGS=
SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=lare1d
TEST_EXECUTABLE=test_$(EXECUTABLE)
BUILD_DIR=build
SRC_DIR=src
TEST_DIR=test

all: $(SOURCES) $(EXECUTABLE)
	    
$(EXECUTABLE): $(OBJECTS) 
	    $(CC) $(LDFLAGS) $(OBJECTS) -o $@
			mkdir -p $(BUILD_DIR)
			mv $(SRC_DIR)/*.o $(BUILD_DIR)

.cpp.o:
	    $(CC) $(CFLAGS) $< -o $@

clean:
			rm -rf $(BUILD_DIR) $(EXECUTABLE) $(TEST_DIR)/$(TEST_EXECUTABLE)

test: all
			$(CC) $(wildcard $(TEST_DIR)/*.cpp) $(filter-out $(BUILD_DIR)/main.o, $(wildcard $(BUILD_DIR)/*.o)) -I include -L$(BUILD_DIR) -o $(TEST_DIR)/$(TEST_EXECUTABLE)

