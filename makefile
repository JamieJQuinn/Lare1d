CC=g++
CFLAGS=-c -Wall -std=c++11 -I$(INCLUDE_DIR) -g
LDFLAGS=
SRC_DIR=src
BUILD_DIR=build
INCLUDE_DIR=include

SOURCES=$(wildcard $(SRC_DIR)/*.cpp)
OBJECTS=$(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
EXECUTABLE=lare1d

TEST_DIR=test
TEST_SOURCES=$(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJECTS=$(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(TEST_SOURCES))

TEST_EXECUTABLE=lare1d_test

all: $(SOURCES) $(BUILD_DIR)/$(EXECUTABLE) $(BUILD_DIR)
	    
$(BUILD_DIR)/$(EXECUTABLE): $(OBJECTS) $(BUILD_DIR)
	    $(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(BUILD_DIR)
	    $(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR):
			mkdir -p $@

clean:
			rm -rf $(BUILD_DIR)

test: $(TEST_SOURCES) $(BUILD_DIR)/$(TEST_EXECUTABLE)
			cd $(BUILD_DIR); ./$(TEST_EXECUTABLE)

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp $(BUILD_DIR)
	    $(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TEST_EXECUTABLE): $(TEST_OBJECTS) $(BUILD_DIR)
	    $(CC) $(LDFLAGS) $(filter-out $(BUILD_DIR)/main.o, $(OBJECTS)) $(TEST_OBJECTS) -o $@
			
