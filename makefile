CC=g++
CFLAGS=-c -Wall -std=c++11 -I$(INCLUDE_DIR) -I$(LIB_INCLUDE_DIR) -g
LDFLAGS=
SRC_DIR=src
BUILD_DIR=build
INCLUDE_DIR=include
LIB_INCLUDE_DIR=lib/include

SOURCES=$(wildcard $(SRC_DIR)/*.cpp)
OBJECTS=$(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
EXECUTABLE=$(BUILD_DIR)/lare1d

TEST_DIR=test/unit_tests
TEST_SOURCES=$(wildcard $(TEST_DIR)/*.cpp)
TEST_EXECUTABLE=test

all: $(SOURCES) $(EXECUTABLE)
	    
$(EXECUTABLE): $(OBJECTS) $(BUILD_DIR)
	    $(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(BUILD_DIR)
	    $(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR):
			mkdir -p $@

clean:
			rm -rf $(BUILD_DIR) $(TEST_DIR)/$(TEST_EXECUTABLE)

test: $(TEST_DIR)/$(TEST_EXECUTABLE)
			cd $(TEST_DIR); ./$(TEST_EXECUTABLE)

$(TEST_DIR)/$(TEST_EXECUTABLE): all $(TEST_SOURCES)
			$(CC) $(wildcard $(TEST_DIR)/*.cpp) $(filter-out $(BUILD_DIR)/main.o, $(wildcard $(BUILD_DIR)/*.o)) -I include -o $@
			
