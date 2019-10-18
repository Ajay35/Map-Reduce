CC = g++
CFLAGS = -Wall -std=c++1z -pthread
LDFLAGS = -pthread
DEPS =master.hpp mapper.hpp reducer.hpp utility.hpp
OBJ = master.o mapper.o reducer.o utility.o main.o
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

mapreduceproject: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^


