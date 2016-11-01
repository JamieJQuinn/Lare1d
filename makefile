CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=main.cpp Variables.cpp
OBJECTS=$(SOURCES:.cpp=.o)
	EXECUTABLE=lare1d

all: $(SOURCES) $(EXECUTABLE)
	    
$(EXECUTABLE): $(OBJECTS) 
	    $(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	    $(CC) $(CFLAGS) $< -o $@

clean:
			rm $(OBJECTS) $(EXECUTABLE)
