CC=g++
CFLAGS=-c -Wall -std=c++11 -I $(INCLUDE_DIR)
LDFLAGS=
SRC_DIR=src
SOURCES=$(wildcard $(SRC_DIR)/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=lare1d
BUILD_DIR=build
INCLUDE_DIR=include

TEST_DIR=test
TEST_SOURCES=$(wildcard $(TEST_DIR)/*.cpp)
TEST_EXECUTABLE=test_$(EXECUTABLE)

all: $(SOURCES) $(EXECUTABLE)
	    
$(EXECUTABLE): $(OBJECTS) 
	    $(CC) $(LDFLAGS) $(OBJECTS) -o $@
			mkdir -p $(BUILD_DIR)
			mv $(SRC_DIR)/*.o $(BUILD_DIR)

.cpp.o:
	    $(CC) $(CFLAGS) $< -o $@

clean:
			rm -rf $(BUILD_DIR) $(EXECUTABLE) $(TEST_DIR)/$(TEST_EXECUTABLE)

test: $(TEST_DIR)/$(TEST_EXECUTABLE)
			cd $(TEST_DIR); ./$(TEST_EXECUTABLE)

$(TEST_DIR)/$(TEST_EXECUTABLE): all $(TEST_SOURCES)
			$(CC) $(wildcard $(TEST_DIR)/*.cpp) $(filter-out $(BUILD_DIR)/main.o, $(wildcard $(BUILD_DIR)/*.o)) -I include -o $@
			
