CC=g++
CFLAGS=-c -Wall -std=c++11
LDFLAGS=
SOURCES=main.cpp ModelVariables.cpp FluxLimiter.cpp Variable.cpp
OBJECTS=$(SOURCES:.cpp=.o)
	EXECUTABLE=lare1d

all: $(SOURCES) $(EXECUTABLE)
	    
$(EXECUTABLE): $(OBJECTS) 
	    $(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	    $(CC) $(CFLAGS) $< -o $@

clean:
			rm $(OBJECTS) $(EXECUTABLE)
